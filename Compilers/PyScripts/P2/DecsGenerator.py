from MassageGrammar import *
from FirstFollows import *

# Path to where stuff is :D
p = "parser/"
d = "data/"
g = "grammar/"
INSTANCE_SEPARATOR = '_'
PARSE_NODE_NAME = 'parent'

def createParseTableEntry(nt, terms, productions, firsts_dict, follows_dict):
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

def writeParseEntry(nt, nts, terms, productions, first_dict, follows_dict):
   outStr = "void Project2::" + nt + "(ParseNode* " + PARSE_NODE_NAME + " ) {\n" # declaration
   outStr += "\tstd::string nt = \"" + nt + "\";\n" # make nt
   #outStr += "\tstd::string debugLEX = lookAhead.lex;\n" # removing debugs for now
   #outStr += "\tint debugLineNum = lookAhead.line+1;\n"
   #outStr += "\tint debugCharCol = lookAhead.charNum+1;\n"
   
   entry, expected, follows = createParseTableEntry(nt, terms, productions, first_dict, follows_dict)
   outStr += "\tstd::string exp = \"" + tupToString(expected)[:-1] + "\";\n"
   outStr += "\tParseNode* ref = " + PARSE_NODE_NAME + ";"
   temp = ""
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

def writeNonEps(nt, nts, term, prod):
   outStr = ""
   outStr += "\n\tif("

   outStr += "lookAhead.token == p->GTT(\"" + term + "\")) {\n" # if statements
   
   names = prodToNames(nt, nts, prod)
   
#		ParseNode* type_1 = &ParseNode(curr, "type", std::list<std::string>());
#		ParseNode* decsLR1_2 = &ParseNode(curr, "decsLR1", std::list<std::string>());
#		curr->appendChild(type_1);
#		curr->appendChild(decsLR1_2);
   # initialize variables
   for i in range(0,len(prod)):
      targ = prod[i]
      if targ in nts:
         outStr += "\t\tParseNode* " + names[i] + " = new ParseNode(" + PARSE_NODE_NAME + ",\"" + targ + "\", std::list<std::string>());\n"
         outStr += "\t\t" + PARSE_NODE_NAME + "->appendChild(" + names[i] + ");\n"
   
   # Token stuff
   for i in range(0,len(prod)):
      targ = prod[i]
      if targ in nts:
         outStr += "\t\tref = " + names[i] + ";\n"
         outStr += "\t\t" + targ + "(" + names[i] + ");\n"
      elif targ in terms:
#         outStr += "\t\tToken WTF" + str(i) + " = " +  "Match(p->GTT(\"" + targ + "\") ,nt, \"" + targ + "\");\n";
#         outStr += "\t\t" + PARSE_NODE_NAME + ".appendToken( WTF" + str(i) + ", ref);\n"
         outStr += PARSE_NODE_NAME + "->appendToken(Match(p->GTT(\"" + targ + "\") ,nt, \"" + targ + "\"), ref);\n"
      else:
         print(prod, "ERROR: DOOM!!!!", targ)
   outStr += "\t\treturn;\n" # essentially a break in a case statement when using ifs
   outStr += "\t}\n" # close if  
   return outStr

# nt and prod added for project 3 filler for now
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
def writeParser(start, nts, terms, productions, first_dict, follows_dict):
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
      s,s2 = writeParseEntry(nt, nts, terms, productions, first_dict, follows_dict)
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
def writeMiscTextFiles(nts, firsts_dict, follows_dict): 
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
   outstr += "\tParseNode* root = new ParseNode(NULL, \"" + start + "\", std::list<std::string>());\n" 
   outstr += "\tlookAhead = p->nextToken();\n"+"\t" + start + "(root);\n"
   outstr += "\tMatchEOF();\n"
   outstr += "\tstd::ofstream file;\n"
   outstr += "\tfile.open(\"Tree.txt\", std::ios::out | std::ios::trunc);\n"
   outstr += "\tParseNode::WriteUndecoratedTree(ParseNode::Wrap(root, 0), &file);\n"
   outstr += "}\n\n"
   return outstr

if __name__ == '__main__':
   reformGrammar("OriginalGrammar.txt",g+"FormattedGrammar.txt")
   start,nts,terms,productions = massageYourGrammar(g+"FormattedGrammar.txt"
   , g+"NoEpsilons.txt", g+"NoLeftRec.txt", g+"LeftFactored.txt")
   
   first_dict = firsts(nts, productions)
   follows_dict = follows(nts, productions, first_dict)
   
   writeParser(start, nts, terms,productions, first_dict, follows_dict)
   writeMiscTextFiles(nts, first_dict, follows_dict)