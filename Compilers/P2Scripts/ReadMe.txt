Instructions on how to use parser generator:
1) Put your grammar into "OriginalGrammar.txt"
2) Run ParserGenerator

Outputs:
   Grammar folder:
      Files that when printed give you each step of massaging your grammar.
   Parser folder:
      Parser.cpp and Parser.h.. This is all the code you need for project 2. I know you're celebrating. You should be.
      Synch.txt... Copy this into data folder. It holds all of the synch sets.
      

If you accidentally do something evil I don't know.. Here's the grammar format for OriginalGrammar.txt...
   # non terminal
   # "\t" + production
   # Epsilon is represented by a single tab

   # "#"ed lines are ignored, and blank lines are also ignored.
   # Watch out for extraneous tabs!
