lex C99_Parser.l
yacc -dy C99_Parser.y
yacc C99_Parser.y
gcc main.c lex.yy.c y.tab.c -ll -ly -o C99_Parser
