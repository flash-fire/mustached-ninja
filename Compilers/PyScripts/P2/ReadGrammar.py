from MakeInstance import *
   
# Loads the grammar from the file and returns it. Heck yeah!
def loadDecGrammar(filename, nts):
   def handleSkip(f, line):
      while True:
         if not line:
            print ("EOF", line)
            return line
         if not line.strip() or line[0] == '#':
            print("COMMENT: ", line[:-1])
            line = f.readline()
         else:
            #print ("GOOD", line[:-1])
            return line
   f = open(filename,'r')
   line = f.readline()
   codeDict = {nt : set() for nt in nts}
   varsDict = {nt : set() for nt in nts}
            
   while True:
      line = handleSkip(f, line)
      if not line:
         print("Good fail")
         break
      line = line.strip()
      nt = line[:-1] # we found nonterminal
      if nt not in nts:
         print("<" + nts[0] + ">", nt)
         print("Expected nt. Got", nt, " is not in non terminals. Sorry.")
         break
         
      print("WE GOT NT : ", nt)
      var = line = f.readline().strip()
      line = handleSkip(f, line)
      #print("Current line before vars", line)
      if not line:
         print("Expected <<vars>>; received EOF")
         break
         
      lineStrip = line.strip()
      #print("WANT VARS: FOUND", lineStrip)
      if lineStrip[0:8].lower() == "<<vars>>":
         varsDict[nt] |= set(lineStrip.split(' ')[1:])
         print("Found vars : ", lineStrip.split(' ')[1:])
         line = f.readline()
         line = handleSkip(f, line)
         lineStrip = line.strip()
      # handle production
      while lineStrip[0:9].lower() == "<<begin>>":
         prod = tuple(lineStrip.split(' '))
         #print("PROD", prod)
         code = ""
         line = f.readline()
         # handles code
         lineStrip = line.strip()
         print("\tPROD", prod)
         # handle nonterminals for the code and productions
         while lineStrip in prod:
            targ = lineStrip
            line = f.readline()
            line = handleSkip(f, line)
            lineStrip = line.strip()
            while not lineStrip[0:9] == "<<begin>>" and not lineStrip[-1] in nts:
               if lineStrip in prod:
                  if (nt, prod) in codeDict:
                     codeDict[(nt, prod)] |= {(targ, code)}
                  else:
                     codeDict[(nt, prod)] = {(targ, code)}
                  continue
               elif lineStrip[0:9] == "<<begin>>":
                  print("\t\t\t" + lineStrip[0:9] + " found another production")
                  break;
               elif lineStrip[:-1] in nts:
                  print("\t\t\t" + lineStrip[:-1], " is in ", nts)
                  break
               else:
                  print("\t\t\tCODE~~: " + lineStrip)
                  code += line
               line = f.readline()
               line = handleSkip(f, line)
               lineStrip = line.strip()
            
            if (nt, prod) in codeDict:
               codeDict[(nt, prod)] |= {(targ, code)}
            else:
               codeDict[(nt, prod)] = {(targ, code)}
      #line = f.readline()
   f.close()
   return varsDict, codeDict

if __name__ == '__main__':
   g = 'grammar/'
   start,nts,terms,productions = massageYourGrammar(g+"FormattedGrammar.txt"
   , g+"NoEpsilons.txt", g+"NoLeftRec.txt", g+"LeftFactored.txt", False)
   renamed = renameGrammar(productions,nts,terms)
   ntsPrime = [defName(nt) for nt in nts]
   #f = open("DecoratedGrammar.txt", 'r')
   #for l in f:
   #   print(l)
   varsDict, codeDict = loadDecGrammar("DecoratedGrammar.txt", ntsPrime)
   
   print("\n\n\nPRINTING VAR DICT")
   for nt in ntsPrime:
      if varsDict[nt]:
         print(nt, varsDict[nt])

   print("\n\n\n" + "PRINTING CODE DICT")
   for key, val in codeDict.items():
      if val:
         print ("NT: ", key[0])
         print ("\tPROD: ", key[1])
         for code in val:
            print ("\tCODE_LOC: ", code[0])
            print ("\tCODE: ", code[1])
   #print(codeDict)