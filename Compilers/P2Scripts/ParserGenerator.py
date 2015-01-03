from MassageGrammar import *
from FirstFollows import *

# Path to where stuff is :D
p = "parser/"
d = "data/"
g = "grammar/"

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


# First of a single token   
def writeParser(nts, productions, first_dict, follows_dict):
   def wizardPowers(nt):
      # Here we handle a given nonterminal's method for the parser
      outStr = "void Project2::" + nt + "() {\n" # declaration
      outStr += "\tstd::string nt = \"" + nt + "\";\n" # make nt
      outStr += "\tstd::string debugLEX = lookAhead.lex;\n"
      outStr += "\tint debugLineNum = lookAhead.line+1;\n"
      outStr += "\tint debugCharCol = lookAhead.charNum+1;\n"
      
      if '' in first_dict[nt]:
         exp = (first_dict[nt] | follows_dict[nt]) - {''}
      else:
         exp = first_dict[nt]
      expout = ""
      for x in exp:
         expout = expout + x + " "
      expout = expout[:-1]
      outStr += "\tstd::string exp = \"" + expout + "\";\n" # make expected
      
      for prods in productions[nt]:
         firsts = firstTokStream(prods, nts, productions, first_dict)
         firstsLs = list(firsts)
         for i in range(0, len(firstsLs)):
            if '' in firsts:
               outStr += "\n\tif("
               if nt == 'stmt' + LEFT_FACTORING_ABREV + '1':
                  continue # ambiguous else
               followsLs = list(follows_dict[nt])
               for j in range(0, len(followsLs)):
                  if j == len(followsLs) -1:
                     outStr += "lookAhead.token == p->GTT(\"" + followsLs[j] + "\")) {\n"
                     outStr += "\t\treturn;\n"
                     outStr += "\t}\n" #close if
                  else:
                     outStr += "lookAhead.token == p->GTT(\"" + followsLs[j] + "\") || "                  
            else:
            
               checkStr = firstsLs[i]
               outStr += "\n\tif("
               outStr += "lookAhead.token == p->GTT(\"" + checkStr + "\")) {\n" # if statements
               for targ in prods:
                  if targ in nts:
                     outStr += "\t\t" + targ + "();\n"
                  else:
                     outStr += "\t\tMatch(p->GTT(\"" + targ + "\") ,nt, \"" + targ + "\");\n"
               outStr += "\t\treturn;\n" # essentially a break in a case statement when using ifs
               outStr += "\t}\n" # close if
      outStr += "\tSynErrorTok(nt, exp);\n"
      outStr += "}\n\n" # close method
      return outStr

   # Here we write the parser
   parseboil = open(d+"ParserBoilerCode.txt","r")
   headStart = open(d+"ParserHeaderStart.txt","r")
   headEnd   = open(d+"ParserHeaderEnd.txt","r")
   pout = open(p+"Parser.cpp",'w')
   hout = open(p+"Parser.h",'w')

   copyFile(parseboil, pout)
   copyFile(headStart, hout)
   
   for nt in nts:
      s = wizardPowers(nt)
      pout.write(s)
      hout.write("\tvoid Project2::" + nt + "();\n")
   
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

if __name__ == '__main__':
   reformGrammar("OriginalGrammar.txt",g+"FormattedGrammar.txt")
   start,nts,terms,productions = massageYourGrammar(g+"FormattedGrammar.txt"
   , g+"NoEpsilons.txt", g+"NoLeftRec.txt", g+"LeftFactored.txt")
   
   first_dict = firsts(nts, productions)
   follows_dict = follows(nts, productions, first_dict)
   
  # writeParser(nts, productions, first_dict, follows_dict)
  # writeMiscTextFiles(nts, first_dict, follows_dict)
   #entry, exp, follows = createParseTableEntry("decsLR1", terms, productions, first_dict, follows_dict)
   #print("EXP:",tupToString(exp))
   #print("FOLLOWS:",follows)
   #for e,res in entry.items():
   #   if res:
   #      print(e, "   ", prodToString('decsLR1', res[0] ))
   #print("Done!")
