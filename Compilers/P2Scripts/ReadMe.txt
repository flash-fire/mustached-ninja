Instructions on how to use parser generator:
1) Put your grammar into "OriginalGrammar.txt" [more instructions there]
2) Run ParserGenerator

Outputs:
   Grammar folder:
      Files that when printed give you each step of massaging your grammar.
   Parser folder:
      Parser.cpp and Parser.h to copy into your project

But if you accidentally do something evil I don't know.. Here's the grammar format for OriginalGrammar.txt...
   # non terminal
   # "\t" + production
   # Epsilon is represented by a single tab

   # "#"ed lines are ignored, and blank lines are also ignored.
   # Watch out for extraneous tabs!