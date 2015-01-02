from MassageGrammar import *


# First of a single token  
def buildParser(nts, productions):
   first_calc = {key : False for key in nts}
   first_dict = {key : set() for key in nts}
   follows_calc = {key : False for key in nts}
   follows_dict = {key : set() for key in nts}
   follows_dict[nts[0]] = {'$'}

   def firstOfTok(str1):
      if not str1:
         return {''} # first of epsilon is epsilon
      elif str1 not in nts:
         return {str1}; # firsts of a terminal is itself
      else:
         return firstNT(str1)

   def firstNT(nt):
      if first_calc[nt]: # memoizing calculation of first
         return first_dict[nt]

      for prod in productions[nt]:
         first_dict[nt] |= firstTokStream(prod)

      first_calc[nt] = True;
      return first_dict[nt];

   def firstTokStream(toks):
      retSet = set()
      canBeNulled = True;
      for tok in toks:
         retSet |= (firstOfTok(tok) - {''})
         if not nullable(tok, productions): #4.2 -- the tok is nullable : Else 4.1
            canBeNulled = False
            break
      if canBeNulled: # 4.3
         retSet |= {''}
      return retSet

   def followsOf(nonTerm):
      def followsIg(nt, igls):
         if follows_calc[nt]:
            return follows_dict[nt]
         
         for nt2,prodset in productions.items():
            for prod in prodset:
               for i in range(0, len(prod)): # need to know index of match
                  if prod[i] == nt:
                     if i == len(prod)-1: # case 3
                        if nt2 not in igls:
                           igls.append(nt)
                           follows_dict[nt] |= followsIg(nt2, igls)
                     else:
                        follows_dict[nt] |= (firstTokStream(prod[i+1:]) - {''})
                        if all(nullable(tok,productions) for tok in prod[i+1:]):
                           if nt2 not in igls:
                              igls.append(nt)
                              follows_dict[nt] |= followsIg(nt2, igls)           
         return follows_dict[nt]
      ret = followsIg(nonTerm, [])
      follows_calc[nonTerm] = True   
      return ret

   def writeParser():
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
            firsts = firstTokStream(prods)
            firstsLs = list(firsts)
            for i in range(0, len(firstsLs)):
               if '' in firsts:
                  outStr += "\n\tif("
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
                  #outStr += "\tstd::string curr = \"" + firstsLs[i] + "\";\n"
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
      p = "parser/"
      d = "data/"
      synch     = open(p+"synch.txt","w") # Writes synchset
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
      
      # Here we write the synch sets
      synch_dict = {nt: follows_dict[nt] | {'$'} for nt in nts}
      writeSynchSetFormat(synch, nts, synch_dict)
      
      synch.close()
      parseboil.close()
      headStart.close()
      headEnd.close()
      pout.close()
      hout.close()
     
   for nt in nts:
      firstNT(nt)
   for nt in nts:
      followsOf(nt)
   print("\n\n\n*** FIRSTS ***\n\n")
   for nt in nts:
      print (nt, first_dict[nt])
   print("\n\n*** FOLLOWS ***\n\n")
   for nt in nts:
      print (nt, follows_dict[nt])
   writeParser()


if __name__ == '__main__':
   g="grammar/"
   reformGrammar("OriginalGrammar.txt",g+"FormattedGrammar.txt")
   start,nts,terms,productions = massageYourGrammar(g+"FormattedGrammar.txt"
   , g+"NoEpsilons.txt", g+"NoLeftRec.txt", g+"LeftFactored.txt")
   buildParser(nts, productions)
   print("Done!")