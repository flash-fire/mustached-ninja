separator = "->"

def copyFile(fStart, fEnd):
   for line in fStart:
      fEnd.write(line)

# write the format used for loading synch sets in my wonderful compiler
def writeSynchSetFormat(fileHandle, nts, dict):
   ls = nts.copy()
   ls.sort()
   for nt in ls:
      fileHandle.write(nt)
      temp = '' #'\t'
      e1 = True
      for str1 in dict[nt]:
         if e1:
            temp = '\n' +  str1
            e1 = False
         else:
            temp = temp + ' ' + str1
      temp = temp + '\n'
      fileHandle.write(temp)

# write the format used for making shenoi happier
def writeShenoi(fileHandle, nts, dict):
   ls = nts.copy()
   ls.sort()
   for nt in ls:
      fileHandle.write(nt)
      temp = '' #'\t'
      e1 = True
      for str1 in dict[nt]:
         if e1:
            temp = '\n\t' +  str1
            e1 = False
         else:
            temp = temp + ' ' + str1
      temp = temp + '\n'
      fileHandle.write(temp)
   
def writeProductions(fileName, nts, dict):
   fileHandle = open(fileName,'w')
   fileHandle.truncate()
   # Ensure that the start production is the first written to the file
   fileHandle.write("# First symbol used is start symbol\n# Format is a->b ... For epsilon, simply have empty string be b\n")
   for nt in nts: 
      temp = "" #'\t'
      for str1 in dict[nt]:
         fileHandle.write(nt + " " + separator + " " + tupToString(str1) + "\n")
   fileHandle.close()

def tupToString(toks):
   outStr = ""
   for lex in toks:
      outStr += (lex + " ")
   return outStr

# Loads the grammar from the file and returns it. Heck yeah!
def loadGrammar(gramLoc):
   nts = []
   gram = open(gramLoc,'r')
   productions = dict()
   first = True
   for prod in gram: # Load Grammar
      if prod[0] == '#': # skip comments
         continue
      nt,str1 = prod.split(separator) # nonterminal -> production
      
      # Start symbol is at top of file
      if first:
         startSym = nt.strip()
         first = False

      nt = nt.strip()
      str1 = str1.strip()
      toks = tuple(str1.split(' '));
      
      # Make epsilons into empty tuples
      if toks[0] == '':
         toks = tuple()

      if nt in productions: # add tuples
         productions[nt] |= {toks}
      else:
         nts.append(nt)
         productions[nt] = {toks}
   terms = findTerms(nts,productions)
   terms = sorted(terms)
   return (startSym,nts,terms,productions)

def reformGrammar(fileIn, fileOut):
   filename = fileIn
   f = open(filename,'r')
   outname = fileOut
   o = open(outname,'w')
   o.truncate()
   line = f.readline()
   name = ''
   i=0
   while True:
      i += 1
      if not line:
         break
      if line in ['\n', '\r\n'] or line[0] == '#':
         line = f.readline()
         continue
      line2 = line.rstrip()
      if line2 and line2[0] != '\t':
         name = line2
      else:
         o.write( name + ' ' + separator + ' ' + line2[1:] + '\n')
      line = f.readline()
   f.close()
   o.close()

def findTerms(nts, productions):
   terms = []
   for key,prodSet in productions.items():
      for prod in prodSet:
         for term in prod:
            if term and term not in nts and term not in terms:
               terms.append(term)
   return terms
   
def prodToString(nt, prod):
   return nt + separator + tupToString(prod)

def printProd(productions, nt):
   for prod in productions[nt]:
      print(prodToString(nt, prod))

def printProds(productions, nts):
   for nt in nts:
      printProd(productions,nt)

if __name__ == '__main__':
   g='grammar/'
   reformGrammar("OriginalGrammar.txt",g+"FormattedGrammar.txt")