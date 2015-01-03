import copy
import random
from GrammarFileIO import *

LEFT_FACTORING_ABREV = 'LF'
LEFT_RECURSION_ABREV = 'LR'

def nullable(x, productions):
   def nullable1(x, productions, visited):
      if x not in productions or x in visited:
         return False
      if any(not prod for prod in productions[x]):
         return True
      for prod in productions[x]:
         visited |= {x}
         if all(nullable1(tok, productions, visited) for tok in prod):
            return True
      return False
   return nullable1(x, productions, set())

def remove_eps(startSym, productions):
   nully = {nt for nt in productions if(nullable(nt, productions))}
   # Create new production dictionary without epsilon productions, and then copy that dictionary to original dictionary 
   productions_NO_EPS = dict()
   for nt,prodSet in productions.items():
      for prod in prodSet:
         if not prod:
            continue
         elif nt in productions_NO_EPS: # add tuples
            productions_NO_EPS[nt] |= {prod}
         else:
            productions_NO_EPS[nt] = {prod}
   productions = productions_NO_EPS
   
   # Adds new productions needed from removing epsilons from grammar.
   i = 1
   while i != 0:
      i = 0
      for nt,prodSet in productions.items():
         prodSet2 = prodSet.copy()
         for toks in prodSet:
            for j in range(len(toks)):
               if toks[j] in nully:
                  new_rule = toks[:j] + toks[j+1:]
                  # Handles is S (start) --> epsilon   as only place that can have epsilon production
                  if new_rule not in productions[nt]:
                     if (len(new_rule) == 0 and nt == startSym):
                        prodSet2 |= {()}
                        i+=1
                     # Handles everything else. Makes sure proposed production does not already exist
                     elif len(new_rule) != 0:
                        prodSet2 |= {new_rule}
                        i+=1
         productions[nt] = prodSet2
   return productions

def removeLeftRecursion(startSym,nts,productions):
   nully = {nt for nt in productions if(nullable(nt, productions))}
   if len(nully) > 0: # if nullable productions exist:
      productions = remove_eps(startSym,productions)

   removedRec = dict() # Productions without direct left recursion
   for nt,prodSet in productions.items():
      alphas = set()
      betas = set()
      for prod in prodSet:
         if not prod:
            print("Error: Not all epsilon removed! This program is very confused?"
            ,"Start may go to epsilon, and I have no idea how to handle that.")
            break;
         elif prod[0] == nt: #direct left recursion
            alphas |= {prod[1:]}
         else:
            betas |= {prod}
      if alphas:
         nt1 = findNTInsName(nts, nt, LEFT_RECURSION_ABREV)
         f = lambda x: x+(nt1,)
         A = {f(alpha) for alpha in alphas} |{()}
         B = {f(beta) for beta in betas}
         removedRec[nt]  = B
         removedRec[nt1] = A
         insNT(nts, nt, nt1)
      else:
         removedRec[nt] = productions[nt]
   return removedRec

# Finds name for next added nonterminal
def findNTInsName(nts, orig, prefix):
   k=1
   while (orig + prefix + str(k)) in nts:
      k += 1
   return orig+ prefix + str(k)

# Inserts a nonterminal into the nonterminals list
def insNT(nts, orig, newNT):
   def findIndex():
      ind = nts.index(orig)
      for i in range(ind, len(nts)):
         if nts[i].find(orig) != 0:
            return i
      return len(nts)
   nts.insert(findIndex(), newNT)

# We first create set with same starting element.
# We then look at the next element, and see if everyone in that set has same next element
# If they do, we repeat this
# Otherwise, perform left factoring with alpha as common subsequence, and y as elements that are not in original set.
def leftFactoring(nts,productions):
   def fixLeft(nt, productions):
      # I remove and re add epsilon for avoiding special case for prod[0] of ()
      hasEps = () in productions[nt]
      origSet = productions[nt] - {()}
      productions[nt] = set()
      if hasEps:
         productions[nt] |= {()}

      # Formats the productions into a format that's easier to compute left factoring
      alphaMap = dict()
      for prod in origSet:
         if prod[0] in alphaMap:
            alphaMap[prod[0]] |= {prod}
         else:
            alphaMap[prod[0]] = {prod}

      for alpha,betas in alphaMap.items():
         if len(betas) == 1:
            productions[nt] |= betas
         else: # Left factoring
            currBetas = betas # I could reuse betas, but this is less confusing
            oldBetas = currBetas #copy it like it is hot!  
            currAlpha = alpha
            alphaTup = (currAlpha,)

            while all(beta and beta[0] == currAlpha for beta in currBetas):
               oldBetas = currBetas #copy
               currBetas = {beta[1:] for beta in currBetas}
               currAlpha = random.sample(currBetas, 1)[0] # Chooses a fixed element of currBetas
               if currAlpha: # currAlpha could be empty. If it is, I want to avoid it.
                  currAlpha = currAlpha[0]
               alphaTup = alphaTup + (currAlpha,) # hehe empty adds a nonsensical ((),) to be chopped off
            alphaTup = alphaTup[:-1] # remove the last failing element which always occurs yay
 
            # Generate and insert name of left factored thing
            ntPrime = findNTInsName(nts, nt, LEFT_FACTORING_ABREV)
            insNT(nts, nt,ntPrime)
            productions[nt] |= {alphaTup + (ntPrime,)}
            productions[ntPrime] = currBetas
            
            fixLeft(ntPrime, productions) # to handle possible nested left factoring
   for nt in nts:
      fixLeft(nt,productions)
   return productions

def massageYourGrammar(fileName, epsFileName, leftRecFileName, leftFactorFileName):
   start,nts,terms,orig_prods = loadGrammar(fileName)
   productions = remove_eps(start, orig_prods );
   writeProductions(epsFileName, nts, productions)

   productions = removeLeftRecursion(start, nts, productions)
   writeProductions(leftRecFileName, nts, productions)
   productions = leftFactoring(nts, productions)
   writeProductions(leftFactorFileName, nts, productions)
   return start,nts,terms,productions
   
if __name__ == '__main__':
   g="grammar/"
   reformGrammar("OriginalGrammar.txt",g+"FormattedGrammar.txt")
   massageYourGrammar(g+"FormattedGrammar.txt"
   , g+"NoEpsilons.txt", g+"NoLeftRec.txt", g+"LeftFactored.txt")  #printProds(productions, nts)
   print("Done!")