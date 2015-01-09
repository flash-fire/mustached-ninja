from GrammarFileIO import *
from MassageGrammar import *

INSTANCE_SEPARATOR = '_'

def defName(nt, count = 1):
   return nt + INSTANCE_SEPARATOR + str(count)
 
def renameGrammar(prods, nts, terms):
   def renameProd(nt, prod):
      counts = {nonterm:0 for nonterm in nts + terms}
      counts[nt] = 1
      ret = ("<<begin>>",)
      if not prod:
         ret += ("EPSILON",)
      for targ in prod:
         counts[targ] += 1
         name = defName(targ, counts[targ])
         if targ in nts:
            ret += ("" + name + "",)
         elif name:
            ret += (name,)
      ret +=  ("<<end>>",)
      return ret
   retDic = {defName(nt):set() for nt in nts}
   for nt in nts:
      for prod in prods[nt]:
         retDic[defName(nt)] |= {renameProd(nt, prod)}
   return retDic
   
if __name__ == '__main__':
   g = 'grammar/'
   start,nts,terms,productions = massageYourGrammar(g+"FormattedGrammar.txt"
   , g+"NoEpsilons.txt", g+"NoLeftRec.txt", g+"LeftFactored.txt", False)
   renamed = renameGrammar(productions,nts,terms)
   ntsPrime = [defName(nt) for nt in nts]
   writeGrammarShenoiFormat("grammar/InstancifiedGrammarShenoi.txt" , ntsPrime, renamed, True)
   