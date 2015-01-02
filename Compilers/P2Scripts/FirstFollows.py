from GrammarFileIO import loadGrammar
from MassageGrammar import nullable, massageYourGrammar

# I added the first_dict = dict() to allow this to be used outside of the class..
# However, outside of the class there'll be no optimization. So there is that caveat.
# Passing in a first_dict will compute and memoize the computation of first or follows
# to see how to initialize dictionary, see first() or follows() -- which computes first follows for all.
def firstOfTok(str1, nts, productions, first_dict = dict()):
   if not str1:
      return {''} # first of epsilon is epsilon
   elif str1 not in nts:
      return {str1}; # firsts of a terminal is itself
   else:
      return firstNT(str1, nts, productions)

def firstNT(nt, nts, productions, first_dict = dict()):
   if first_dict and first_dict[nt] != set(): # memoizing calculation of first
      return first_dict[nt]

   fSet = set()
   for prod in productions[nt]:
      fSet |= firstTokStream(prod, nts, productions, first_dict)
   if first_dict: # Don't want my default argument to change
      first_dict[nt] = fSet
   return fSet;

def firstTokStream(toks, nts, productions, first_dict = dict()):
   retSet = set()
   canBeNulled = True;
   for tok in toks:
      retSet |= (firstOfTok(tok, nts, productions, first_dict) - {''})
      if not nullable(tok, productions): #4.2 -- the tok is nullable : Else 4.1
         canBeNulled = False
         break
   if canBeNulled: # 4.3
      retSet |= {''}
   return retSet

# Obviously calling with firsts and follows precomputed will be faster
def followsOf(nonTerm, nts, productions, first_dict = dict(), follows_dict = dict()):
   def followsIg(nt, igls):
      if follows_dict and follows_dict[nt]:
         return follows_dict[nt]
      ret = set()
      if nt == nts[0]:
         ret |= {'$'}
      for nt2,prodset in productions.items():
         for prod in prodset:
            for i in range(0, len(prod)): # need to know index of match
               if prod[i] == nt:
                  if i == len(prod)-1: # case 3
                     if nt2 not in igls:
                        igls |= {nt}
                        ret |= followsIg(nt2, igls)
                  else:
                     ret |= (firstTokStream(prod[i+1:], nts, productions, first_dict) - {''})
                     if all(nullable(tok,productions) for tok in prod[i+1:]):
                        if nt2 not in igls:
                           igls |= {nt}
                           ret |= followsIg(nt2, igls)    
      if follows_dict:
         follows_dict[nt] = ret
      return ret
   return followsIg(nonTerm, set())

# Computes firsts for all nts using memoization
def firsts(nts, productions):
   first_dic = {key : set() for key in nts};
   for nt in nts:
      firstNT(nt, nts, productions, first_dic) 
   return first_dic

def follows(nts, productions, first_dict):
   follows_dict = {key : set() for key in nts}
   for nt in nts:
      followsOf(nt, nts, productions, first_dict, follows_dict)
   return follows_dict

if __name__ == '__main__':
   g = 'grammar/'
   start,nts,terms,prods =  massageYourGrammar(g+"FormattedGrammar.txt"
      , g+"NoEpsilons.txt", g+"NoLeftRec.txt", g+"LeftFactored.txt")  #printProds(productions, nts)

   first_dict = firsts(nts, prods)
   follows_dict = follows(nts, prods, first_dict)

   print("\n\n\n*** FIRSTS ***\n\n")
   for nt in nts:
      print (nt, first_dict[nt])
   print("\n\n*** FOLLOWS ***\n\n")
   for nt in nts:
      print (nt, follows_dict[nt])