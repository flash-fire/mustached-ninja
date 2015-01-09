from MassageGrammar import *
from FirstFollows import *
from ReadDecoratedGrammar import loadDecGrammar
from MakeInstance import defName

# Path to where stuff is :D
p = "parser/"
d = "data/"
g = "grammar/"
INSTANCE_SEPARATOR = '_'
#PARSE_NODE_NAME = ''


# This generates the data necessary for a parse table entry
# In other words, think of this as generating a row in the parse table
# The output is the entry [which contains what SHOULD have only one val for each terminal key]
# The expected [the set of expected tokens-- used when an error occurs to tell the user what they should have typed
# And the follows, which is used later to make synchronization sets [for error handling]
def createParseTableEntry(nt):
   entry = {term:[] for term in terms}
   expected = set()
   follows = []
   for prod in productions[nt]:
      firsts = firstTokStream(prod, nts, productions, first_dict)
      firstsLs = list(firsts)
      for i in range(0, len(firstsLs)):
         if '' in firsts:
            followsSet = follows_dict[nt]
            if nt == 'stmt' + LEFT_FACTORING_ABREV + '1':
               followsSet -={'else'}
            expected |= followsSet
            follows = list(followsSet)
            for term in follows:
               ent = entry[term] + [prod]
               entry[term] = ent
               continue # ambiguous else
         else:
            term = firstsLs[i]
            expected |= {term}
            ent = entry[term] + [prod]
            entry[term] = ent
   for key, val in entry.items():
      if val and len(val) > 1:
         print("Ambiguous parse table.  YOU WILL DIE AND I WILL KILL YOU AT:", key, len(val), val)

   return entry, expected, follows

   
# Writes the parse() method in the cpp class.
def writeParseEntry(nt):
   outStr = "void Project2::" + nt + "(ParseNode* " + defName(nt) + " ) {\n" # declaration
   outStr += "\tstd::string nt = \"" + nt + "\";\n" # make nt
   
   entry, expected, follows = createParseTableEntry(nt)
   outStr += "\tstd::string exp = \"" + tupToString(expected)[:-1] + "\";\n"
   outStr += "\tParseNode* ref = " + defName(nt) + ";"
   
   temp = "" #temp handles writing the parse table entry for shenoi to see that you wrote your parse table correctly
   for term in terms: 
      prod = entry[term]
      if term and prod:
         if prod[0]:
            temp += "\t" + term + " -> "  + tupToString(prod[0]) + "\n"
         else:
            temp += "\t" + term + " -> "  + 'EPSILON' + "\n"
   outStr2 = nt + "\n" +  temp
   
   for term, prod in entry.items():
      if prod and term not in follows:
         outStr += writeNonEps(nt, nts, term, prod[0])
   outStr += writeEps(nt, prod, follows)
   
   outStr += "\tSynErrorTok(nt, exp);\n"
   outStr += "}\n\n" # close method
   return outStr, outStr2

# Generates names for instances of a NT in a production. Not the most efficient, but should be fast enough probably hopefully maybe.
def prodToNames(nt, nts, prod):
   counts = {nonterm:0 for nonterm in nts}
   counts[nt] = 1
   ret = []
   for targ in prod:
      if targ in nts:
         counts[targ] += 1
         ret.append(targ + INSTANCE_SEPARATOR + str(counts[targ]))
      else:
         ret.append(targ)
   return ret

# Handles the case where the terminal's production is not epsilon.
def writeNonEps(nt, nts, term, prod):
   outStr = ""
   outStr += "\n\tif("

   outStr += "lookAhead.token == p->GTT(\"" + term + "\")) {\n" # if statements
   
   names = prodToNames(nt, nts, prod)
   
   # initialize variables
   for i in range(0,len(prod)):
      targ = prod[i]
      if targ in nts:
         outStr += "\t\tParseNode* " + names[i] + " = new ParseNode(" + defName(nt) + ",\"" + targ + "\", std::vector<std::string>());\n"
         outStr += "\t\t" + defName(nt) + "->appendChild(" + names[i] + ");\n"
   
   # Token stuff
   for i in range(0,len(prod)):
      targ = prod[i]
      if targ in nts:
         outStr += "\t\tref = " + names[i] + ";\n"
         outStr += "\t\t" + targ + "(" + names[i] + ");\n"
      elif targ in terms:
         outStr += "\t\t" + defName(nt) + "->appendToken(Match(p->GTT(\"" + targ + "\") ,nt, \"" + targ + "\"), ref);\n"
      else:
         print(prod, "ERROR: DOOM!!!!", targ)
   outStr += "\t\treturn;\n" # essentially a break in a case statement when using ifs
   outStr += "\t}\n" # close if  
   return outStr

# Handles the epsilon case for the parse writing method
def writeEps(nt, prod, followsLs):
   if not followsLs:
      return ""
   outStr = "\n\tif("
   for j in range(0, len(followsLs)):
      if j == len(followsLs) -1:
         outStr += "lookAhead.token == p->GTT(\"" + followsLs[j] + "\")) {\n"
         outStr += "\t\treturn;\n"
         outStr += "\t}\n" #close if
      else:
         outStr += "lookAhead.token == p->GTT(\"" + followsLs[j] + "\") || "
   return outStr

 
# First of a single token   
def writeParser():
   # Here we write the parser
   parseboil = open(d+"ParserBoilerCode.txt","r")
   headStart = open(d+"ParserHeaderStart.txt","r")
   headEnd   = open(d+"ParserHeaderEnd.txt","r")
   parseTable = open(p+"ParseTable.txt","w")
   pout = open(p+"Project3.cpp",'w')
   hout = open(p+"Project3.h",'w')

   copyFile(parseboil, pout)
   copyFile(headStart, hout)
   pout.write(startParseMethod(start))
   parseTable.write("Terminal Order" + str(terms) + "\n")
   for nt in nts:
      s,s2 = writeParseEntry(nt)
      parseTable.write(s2)
      pout.write(s)
      hout.write("\tvoid Project2::" + nt + "(ParseNode* par);\n")
   
   copyFile(headEnd, hout)
   
   parseboil.close()
   headStart.close()
   headEnd.close()
   pout.close()
   hout.close()
 
# Writes synch file, firsts and follows file...
def writeMiscTextFiles(): 
   synch     = open(p+"synch.txt","w") # Writes synchset
   firsts    = open(p+"firsts.txt","w")
   follows   = open(p+"follows.txt","w")
   
   synch_dict = {nt: follows_dict[nt] | {'$'} for nt in nts}
   writeSynchSetFormat(synch, nts, synch_dict)
   writeShenoi(firsts, nts, first_dict)
   writeShenoi(follows, nts, follows_dict)
   
   synch.close()
   firsts.close()
   follows.close()

def startParseMethod(start):
   outstr =  "void Project2::Parse()\n"
   outstr  += "{\n"
   outstr += "\tParseNode* root = new ParseNode(NULL, \"" + start + "\", std::vector<std::string>());\n" 
   outstr += "\tlookAhead = p->nextToken();\n"+"\t" + start + "(root);\n"
   outstr += "\tMatchEOF();\n"
   outstr += "\tstd::ofstream file;\n"
   outstr += "\tfile.open(\"Tree.txt\", std::ios::out | std::ios::trunc);\n"
   outstr += "\tParseNode::WriteUndecoratedTree(ParseNode::Wrap(root, ParseNode::DEF_INSTANCE), &file);\n"
   outstr += "\tstd::ofstream decFile;\n"
   outstr += "\tdecFile.open(\"DecTree.txt\", std::ios::out | std::ios::trunc);\n"
   outstr += "\tParseNode::WriteDecoratedTree(ParseNode::Wrap(root, ParseNode::DEF_INSTANCE), &decFile);\n"
   outstr += "}\n\n"
   return outstr


reformGrammar("OriginalGrammar.txt",g+"FormattedGrammar.txt")
start,nts,terms,productions = massageYourGrammar(g+"FormattedGrammar.txt"
, g+"NoEpsilons.txt", g+"NoLeftRec.txt", g+"LeftFactored.txt", False)

first_dict = firsts(nts, productions)
follows_dict = follows(nts, productions, first_dict)
ntsPrime = [defName(nt) for nt in nts]
varsDict, codeDict = loadDecGrammar("DecoratedGrammar.txt", ntsPrime) # vars dict NT ==> SET(NT) ; # codeDict (nt', prod') ==> (nt', code)
writeParser()
print("Project 2 and 3 generated successfully!")
#writeMiscTextFiles(nts, first_dict, follows_dict)