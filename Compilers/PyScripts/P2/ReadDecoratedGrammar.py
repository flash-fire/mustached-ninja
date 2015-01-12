from MakeInstance import *
from MassageGrammar import *
# Loads the grammar from the file and returns it. Heck yeah!


def loadDecGrammar(filename, nts):
   def handleSkip(f, line):
      while True:
         if not line: # empty
            return line
         if not line.strip() or line[0] == '#': # mostly empty or comment ;)
            line = f.readline()
         else: # normal
            return line
   f = open(filename,'r')
   line = f.readline()
   codeDict = dict()
   varsDict = {nt : set() for nt in nts}
   errorDict = {nt : "" for nt in nts}
   
   while True:
      line = handleSkip(f, line)
      if not line:
         print("Decorated Grammar Read Successfully!\n\n")
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
      if not line:
         print("Expected <<vars>>; received EOF")
         break
         
      lineStrip = line.strip()
      if lineStrip[0:8].lower() == "<<vars>>":
         varsDict[nt] |= set(lineStrip.split(' ')[1:])
         ###print("Found vars : ", lineStrip.split(' ')[1:])
         line = f.readline()
         line = handleSkip(f, line)
         lineStrip = line.strip()
      # handle production
      while lineStrip[0:9].lower() == "<<begin>>":
         prod = tuple(lineStrip.split(' '))
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
            while not lineStrip[0:9] == "<<begin>>":
               if lineStrip in prod:
                  print("In the annoying special case!")
                  if (nt, prod) in codeDict:
                     codeDict[(nt, prod)] |= {(targ, code)}
                  else:
                     codeDict[(nt, prod)] = {(targ, code)}
                  code = ""
                  targ = lineStrip
                  line = f.readline()
                  lineStrip = line.strip()
                  continue
               elif lineStrip[0:9] == "<<begin>>":
                  print("\t\t\t" + lineStrip[0:9] + " found another production")
                  break;
               elif lineStrip[:-1] in nts:
                  print("\t\t\t" + lineStrip[:-1], " is in nts")
                  break
               elif lineStrip[0:9] == "<<error>>":
                  print("FOUND ERRORS")
                  break
               #print("\t\t\tCODE~~: " + lineStrip)
               code += line
               line = f.readline()
               line = handleSkip(f, line)
               lineStrip = line.strip()
            
            if (nt, prod) in codeDict:
               codeDict[(nt, prod)] |= {(targ, code)}
            else:
               codeDict[(nt, prod)] = {(targ, code)}
      
      if lineStrip[0:9] == "<<error>>": # optional error code for when SynError occurs at end of method.
         errCode = "";
         line = f.readline()
         lineStrip = line.strip()
         while lineStrip and lineStrip[:-1] not in nts:
            errCode += line
            line = f.readline();
            lineStrip = line.strip()
            print("LINE", lineStrip[:-1])
         print("FOUND NT:", line)
         errorDict[nt] = errCode
   f.close()
   return varsDict, codeDict, errorDict

def printDecGrammar(varsDict, codeDict, errorsDict, ntsPrime):
   print("\nPRINTING VAR DICT")
   for nt in ntsPrime:
      if varsDict[nt]:
         print("\t", nt, varsDict[nt])

   print("\nPRINTING ERROR DICT")
   for nt in ntsPrime:
      if errorsDict[nt]:
         print("\tNT: ", nt)
         print("\tCODE:", errorsDict[nt])
   print("\n\n\n" + "PRINTING CODE DICT")
   for key, val in codeDict.items():
      if val:
         print ("NT: ", key[0])
         print ("\tPROD: ", key[1])
         for code in val:
            print ("\tCODE_LOC: ", code[0])
            print ("\tCODE: ", code[1])

if __name__ == '__main__':
   g = 'grammar/'
   start,nts,terms,productions = massageYourGrammar(g+"FormattedGrammar.txt"
   , g+"NoEpsilons.txt", g+"NoLeftRec.txt", g+"LeftFactored.txt", False)
   renamed = renameGrammar(productions,nts,terms)
   ntsPrime = [defName(nt) for nt in nts]
   
   varsDict, codeDict, errorDict = loadDecGrammar("DecoratedGrammar.txt", ntsPrime)
   
   printDecGrammar(varsDict, codeDict, errorDict, ntsPrime)