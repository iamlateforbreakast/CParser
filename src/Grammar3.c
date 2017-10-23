/* Grammar.c */

#include "Grammar.h"

#include "Common.h"
#include "Token.h"
#include "List.h"
#include "SdbMgr.h"

#include <stdint.h>

#define DEBUG (1)

#define CONTEXT_DEPTH (5)

typedef enum
{
  E_EXTERNAL_DECLARATION = 0,
  E_FUNCTION_DECLARATION,
  E_DECLARATION,
  E_COMPOUND_STATEMENT,
  E_DECLARATION_SPECIFIERS,
  E_DECLARATOR,
  E_DIRECT_DECLARATOR,
  E_DECLARATOR_LIST,
  E_INIT_DECLARATOR_LIST,
  E_INIT_DECLARATOR,
  E_STORAGE_CLASS,
  E_TYPE_SPECIFIER,
  E_TYPE_QUALIFIER,
  E_TYPE_QUALIFIER_LIST,
  E_STRUCT_OR_UNION_SPECIFIER,
  E_STRUCT_OR_UNION_SPECIFIER2,
  E_STRUCT_DECLARATION_LIST,
  E_STRUCT_DECLARATION,
  E_SPECIFIER_QUALIFIER_LIST,
  E_STRUCT_DECLARATOR_LIST,
  E_STRUCT_DECLARATOR,
  E_ENUMERATOR,
  E_ENUMERATOR_LIST,
  E_ENUM_SPECIFIER,
  E_POINTER,
  E_CONSTANT_EXPRESSION,
  E_INITIALIZER,
  E_PARAMETER_TYPE_LIST,
  E_IDENTIFIER_LIST,
  E_PARAMETER_LIST,
  E_ABSTRACT_DECLARATOR,
  E_DIRECT_ABSTRACT_DECLARATOR,
  E_DECLARATION_LIST,
  E_PARAMETER_DECLARATION,
  E_CONDITIONAL_EXPRESSION,
  E_STATEMENT_LIST,
  E_STATEMENT,
  E_LABELED_STATEMENT,
  E_EXPRESSION_STATEMENT,
  E_SELECTION_STATEMENT,
  E_ITERATION_STATEMENT,
  E_JUMP_STATEMENT,
  E_EXPRESSION,
  E_ASSIGNMENT_EXPRESSION,
  E_PRIMARY_EXPRESSION,
  E_POSTFIX_EXPRESSION,
  E_UNARY_EXPRESSION,
  E_CAST_EXPRESSION,
  E_TYPE_NAME
} RuleName;

typedef enum{
  E_UNKNOWN_DECLARATOR = 0,
  E_FUNCTION_DECLARATOR,
  E_VARIABLE_DECLARATOR,
  E_TYPE_DECLARATOR
} DeclaratorClass;

typedef enum
{
  E_UNKOWN_SCOPE = 0,
  E_LOCAL_SCOPE = 1,
  E_GLOBAL_SCOPE = 2
} Scope;

typedef void (*EvalFunction)(Grammar* this, Token* token);

typedef struct MatchRule
{
  RuleName name;
  const char* description;
  unsigned int isMatched;
  unsigned int isEvaluated;
  EvalFunction evaluate;
  unsigned int count[CONTEXT_DEPTH];
} MatchRule;

typedef struct TypeInfo TypeInfo;

struct TypeInfo
{
  String* name;
  unsigned int nbFields;
  List* fields;
};

typedef struct Declarator Declarator;

struct Declarator
{
  DeclaratorClass class;
  TypeInfo type;
  //DeclaratorQualifier qualifier;
  String* name;
  List* args;
  String* fName;
  unsigned int line;
  unsigned int col;
};

struct Grammar{

  unsigned int isInStructDefinition;
  unsigned int requestReset;
  Scope scope;
  unsigned int tokenNumber;
  Declarator localDeclarator;
  Declarator declarator;
  unsigned int context;
  RuleName entryRule[CONTEXT_DEPTH];
};

void Grammar_matchExternalDeclaration(Grammar* this, Token* token);
void Grammar_matchFunctionDeclaration(Grammar* this, Token* token);
void Grammar_matchDeclaration(Grammar* this, Token* token);
void Grammar_matchDeclarationSpecifiers(Grammar* this, Token* token);
void Grammar_matchStorageClass(Grammar* this, Token* token);
void Grammar_matchInitDeclaratorList(Grammar* this, Token* token);
void Grammar_matchInitDeclarator(Grammar* this, Token* token);
void Grammar_matchDeclarator(Grammar* this, Token* token);
void Grammar_matchDeclaratorList(Grammar* this, Token* token);
void Grammar_matchDirectDeclarator(Grammar* this, Token* token);
void Grammar_matchTypeSpecifier(Grammar* this, Token* token);
void Grammar_matchPointer(Grammar* this, Token* token);
void Grammar_matchConstantExpression(Grammar* this, Token* token);
void Grammar_matchTypeQualifier(Grammar* this, Token* token);
void Grammar_matchTypeQualifierList(Grammar* this, Token* token);
void Grammar_matchCompountStatement(Grammar* this, Token* token);
void Grammar_matchStructDeclarationList(Grammar* this, Token* token);
void Grammar_matchSpecifierQualifierList(Grammar* this, Token* token);
void Grammar_evaluateRule(Grammar* this, Token* token, RuleName r);
void Grammar_matchInitializer(Grammar* this, Token* token);
void Grammar_matchParameterTypeList(Grammar* this, Token* token);
void Grammar_matchIdentifierList(Grammar* this, Token* token);
void Grammar_matchParameterList(Grammar* this, Token* token);
void Grammar_matchAbstractDeclarator(Grammar* this, Token* token);
void Grammar_matchDirectAbstractDeclarator(Grammar* this, Token* token);
void Grammar_matchDeclarationList(Grammar* this, Token* token);
void Grammar_matchParameterDeclaration(Grammar* this, Token* token);
void Grammar_matchStatementList(Grammar *this, Token* token);
void Grammar_printDeclarator(Grammar* this);
void Grammar_printMatchingRules(Grammar* this, Token* token);
void Grammar_reset(Grammar* this);
void Grammar_insertDeclaration(Grammar* this, Declarator* declarator);
unsigned int Grammar_isTypeDefined(Grammar* this, String* typeName);
void Grammar_saveContext(Grammar* this, RuleName entryRule);
void Grammar_restoreContext(Grammar* this, RuleName entryRule);

MatchRule rules[] = { { E_EXTERNAL_DECLARATION , "EXTERNAL_DECLARATION", 0 , 0, &Grammar_matchExternalDeclaration, {0, 0, 0, 0, 0 } },
                      { E_FUNCTION_DECLARATION , "FUNCTION_DECLARATION", 0 , 0, &Grammar_matchFunctionDeclaration, {0, 0, 0, 0, 0 } },
                      { E_DECLARATION , "E_DECLARATION", 0 , 0, &Grammar_matchDeclaration, {0, 0, 0, 0, 0 } },
                      { E_COMPOUND_STATEMENT , "COMPOUND_STATEMENT", 0 , 0, &Grammar_matchCompountStatement, {0, 0, 0, 0, 0 } },
                      { E_DECLARATION_SPECIFIERS , "DECLARATION_SPECIFIERS", 0 , 0, &Grammar_matchDeclarationSpecifiers, {0, 0, 0, 0, 0 } },
                      { E_DECLARATOR , "DECLARATOR", 0 , 0, &Grammar_matchDeclarator, {0, 0, 0, 0, 0 } },
                      { E_DIRECT_DECLARATOR , "DIRECT_DECLARATOR", 0 , 0, &Grammar_matchDirectDeclarator, {0, 0, 0, 0, 0 } },
                      { E_DECLARATOR_LIST , "DECLARATOR_LIST", 0 , 0, &Grammar_matchDeclaratorList, {0, 0, 0, 0, 0 } },
                      { E_INIT_DECLARATOR_LIST , "INIT_DECLARATOR_LIST", 0 , 0, &Grammar_matchInitDeclaratorList, {0, 0, 0, 0, 0 } },
                      { E_INIT_DECLARATOR , "INIT_DECLARATOR", 0 , 0, &Grammar_matchInitDeclarator, {0, 0, 0, 0, 0 } },
                      { E_STORAGE_CLASS , "STORAGE_CLASS", 0 , 0, &Grammar_matchStorageClass, {0, 0, 0, 0, 0 } },
                      { E_TYPE_SPECIFIER , "TYPE_SPECIFIER", 0 , 0, &Grammar_matchTypeSpecifier, {0, 0, 0, 0, 0 } },
                      { E_TYPE_QUALIFIER , "TYPE_QUALIFIER", 0 , 0, &Grammar_matchTypeQualifier, {0, 0, 0, 0, 0 } },
                      { E_TYPE_QUALIFIER_LIST , "TYPE_QUALIFIER_LIST", 0 , 0, &Grammar_matchTypeQualifierList, {0, 0, 0, 0, 0 } },
                      { E_STRUCT_OR_UNION_SPECIFIER , "STRUCT_OR_UNION_SPECIFIER", 0 , 0, NULL, {0, 0, 0, 0, 0 } },
                      { E_STRUCT_OR_UNION_SPECIFIER2 , "STRUCT_OR_UNION_SPECIFIER2", 0 , 0, NULL, {0, 0, 0, 0, 0 } },
                      { E_STRUCT_DECLARATION_LIST , "STRUCT_DECLARATION_LIST ", 0 , 0, NULL, {0, 0, 0, 0, 0 } },
                      { E_STRUCT_DECLARATION , "STRUCT_DECLARATION", 0 , 0, NULL, {0, 0, 0, 0, 0 } },
                      { E_SPECIFIER_QUALIFIER_LIST, "SPECIFIER_QUALIFIER_LIST", 0, 0, &Grammar_matchSpecifierQualifierList, {0, 0, 0, 0, 0 } },
                      { E_STRUCT_DECLARATOR_LIST, "STRUCT_DECLARATOR_LIST", 0, 0, NULL, {0, 0, 0, 0, 0 } },
                      { E_STRUCT_DECLARATOR, "STRUCT_DECLARATOR", 0, 0, NULL, {0, 0, 0, 0, 0 } },
                      { E_ENUMERATOR , "ENUMERATOR", 0 , 0, NULL, {0, 0, 0, 0, 0 } },
                      { E_ENUMERATOR_LIST , "ENUMERATOR_LIST", 0 , 0, NULL, {0, 0, 0, 0, 0 } },
                      { E_ENUM_SPECIFIER , "ENUM_SPECIFIER", 0 , 0, NULL, {0, 0, 0, 0, 0 } },
                      { E_POINTER , "POINTER", 0 , 0, &Grammar_matchPointer, {0, 0, 0, 0, 0 } },
                      { E_CONSTANT_EXPRESSION , "CONSTANT_EXPRESSION", 0 , 0, NULL, {0, 0, 0, 0, 0 } },
                      { E_INITIALIZER, "INITIALIZER", 0, 0, NULL, {0, 0, 0, 0, 0 } },
                      { E_PARAMETER_TYPE_LIST, "PARAMETER_TYPE_LIST", 0, 0, &Grammar_matchParameterTypeList, {0, 0, 0, 0, 0 } },
                      { E_IDENTIFIER_LIST, "IDENTIFIER_LIST", 0, 0, &Grammar_matchIdentifierList, {0, 0, 0, 0, 0 } },
                      { E_PARAMETER_LIST, "PARAMETER_LIST", 0, 0, &Grammar_matchParameterList, {0, 0, 0, 0, 0 } },
                      { E_ABSTRACT_DECLARATOR, "ABSTRACT_DECLARATOR", 0, 0, &Grammar_matchAbstractDeclarator, {0, 0, 0, 0, 0 } },
                      { E_DIRECT_ABSTRACT_DECLARATOR, "DIRECT_ABSTRACT_DECLARATOR", 0, 0, &Grammar_matchDirectAbstractDeclarator, {0, 0, 0, 0, 0 } },
                      { E_DECLARATION_LIST, "DECLARATION_LIST", 0, 0, &Grammar_matchDeclarationList, {0, 0, 0, 0, 0 } },
                      { E_PARAMETER_DECLARATION, "PARAMETER_DECLARATION", 0, 0, &Grammar_matchParameterDeclaration, {0, 0, 0, 0, 0 } },
		                  { E_CONDITIONAL_EXPRESSION, "CONDITIONAL_EXPRESSION", 0, 0, NULL, {0, 0, 0, 0, 0 } },
                      { E_STATEMENT_LIST, "STATEMENT_LIST", 0, 0, &Grammar_matchStatementList, {0, 0, 0, 0, 0 } },
                      { E_STATEMENT, "STATEMENT", 0, 0, NULL, {0, 0, 0, 0, 0 } },
                      { E_LABELED_STATEMENT, "LABELED_STATEMENT", 0, 0, NULL, {0, 0, 0, 0, 0 } },
                      { E_EXPRESSION_STATEMENT, "EXPRESSION_STATEMENT", 0, 0, NULL, {0, 0, 0, 0, 0 } },
                      { E_SELECTION_STATEMENT, "SELECTION_STATEMENT", 0, 0, NULL, {0, 0, 0, 0, 0 } },
                      { E_ITERATION_STATEMENT, "ITERATION_STATEMENT", 0, 0, NULL, {0, 0, 0, 0, 0 } },
                      { E_JUMP_STATEMENT, "JUMP_STATEMENT", 0, 0, NULL, {0, 0, 0, 0, 0 } },
                      { E_EXPRESSION, "EXPRESSION", 0, 0, NULL, {0, 0, 0, 0, 0 } },
                      { E_ASSIGNMENT_EXPRESSION, "ASSIGNMENT_EXPRESSION", 0, 0, NULL, {0, 0, 0, 0, 0 } },
                      { E_PRIMARY_EXPRESSION, "PRIMARY_EXPRESSION", 0, 0, NULL, {0, 0, 0, 0, 0 } },
                      { E_POSTFIX_EXPRESSION, "POSTFIX_EXPRESSION", 0, 0, NULL, {0, 0, 0, 0, 0 } },
                      { E_UNARY_EXPRESSION, "UNARY_EXPRESSION", 0, 0, NULL, {0, 0, 0, 0, 0 } },
                      { E_CAST_EXPRESSION, "CAST_EXPRESSION", 0, 0, NULL, {0, 0, 0, 0, 0 } },
                      { E_TYPE_NAME, "TYPE_NAME", 0, 0, NULL, {0, 0, 0, 0, 0 } }
                    };

/****************************************************************************
****************************************************************************/
Grammar* Grammar_new()
{
  Grammar* this = NULL;

  this = (Grammar*)Memory_alloc(sizeof(Grammar));
  Grammar_reset(this);
  this->scope = E_GLOBAL_SCOPE;
  this->tokenNumber = 1;
  this->requestReset = 0;
  this->isInStructDefinition = 0;
  this->context = 0;
  
  memset(this->entryRule, 0, CONTEXT_DEPTH);
  this->entryRule[0] = E_EXTERNAL_DECLARATION;
  
  return this;
}

/****************************************************************************
****************************************************************************/
void Grammar_delete(Grammar* this)
{
  // delete Global var hash
  Memory_free(this, sizeof(Grammar));

}

/****************************************************************************
****************************************************************************/
void Grammar_reset(Grammar* this)
{
  unsigned int nbRules = sizeof(rules)/sizeof(MatchRule);
  RuleName i = E_EXTERNAL_DECLARATION;

  TRACE(("Grammar Reset\n"));
  for (i=E_EXTERNAL_DECLARATION; i<nbRules; i++)
  {
    rules[i].isMatched = 0;
    rules[i].isEvaluated = 0;
    if (rules[i].count[0]!=0) TRACE(("-- Set count to zero: %s\n", rules[i].description));
        memset(rules[i].count, 0, sizeof(rules[i].count));
  }
  this->declarator.name = NULL;
  this->declarator.class = E_UNKNOWN_DECLARATOR;
  this->declarator.fName = NULL;
  this->declarator.line = 0;
  this->declarator.col = 0;
}

/****************************************************************************
****************************************************************************/
void Grammar_resetContext(Grammar* this)
{
  unsigned int nbRules = sizeof(rules)/sizeof(MatchRule);
  RuleName i = E_EXTERNAL_DECLARATION;

  TRACE(("Grammar Reset Context\n"));
  for (i=E_EXTERNAL_DECLARATION; i<nbRules; i++)
  {
    rules[i].isMatched = 0;
    rules[i].isEvaluated = 0;
    if (rules[i].count[this->context]!=0) TRACE(("-- Set count to zero: %s\n", rules[i].description));
    memset(rules[i].count, 0, sizeof(rules[i].count));
  }
  // this->declarator.name = NULL;
  // this->declarator.class = E_UNKNOWN_DECLARATOR;
  // this->declarator.fName = NULL;
  // this->declarator.line = 0;
  // this->declarator.col = 0;
}

/****************************************************************************
****************************************************************************/
void Grammar_pushToken(Grammar* this, Token* token)
{
  unsigned int nbRules = sizeof(rules)/sizeof(MatchRule);
  unsigned int i = 0;

  TRACE(("-- Processing token #%d: %s\n", this->tokenNumber, token->text));

  //Grammar_matchExternalDeclaration(this, token);
  Grammar_evaluateRule(this, token, this->entryRule[this->context]);
  
  this->tokenNumber++;
  Grammar_printMatchingRules(this, token);

  if (rules[E_EXTERNAL_DECLARATION].isMatched)
  {
    Grammar_printDeclarator(this);
    Grammar_reset(this);
  }
  if (this->requestReset)
  {
    Grammar_reset(this);
    this->requestReset = 0;
  }
  for (i=0; i<nbRules; i++)
  {
    rules[i].isEvaluated = 0;
    rules[i].isMatched = 0;
  }

}

/**************************************************
external_declaration
	: function_definition
	| declaration
**************************************************/
void Grammar_matchExternalDeclaration(Grammar* this, Token* token)
{
  if (this->scope == E_GLOBAL_SCOPE) Grammar_evaluateRule(this, token, E_DECLARATION);
  Grammar_evaluateRule(this, token, E_FUNCTION_DECLARATION);

  if ((rules[E_DECLARATION].isMatched) && (this->scope == E_GLOBAL_SCOPE))
  {
    // Place Declarator structure in store
    rules[E_EXTERNAL_DECLARATION].isMatched = 1;
  }
  else if (rules[E_FUNCTION_DECLARATION].isMatched)
  {
    // Place Declarator structure in store
    rules[E_EXTERNAL_DECLARATION].isMatched = 1;
  }
}

/**************************************************
function_definition
	: declaration_specifiers declarator declaration_list compound_statement
	| declaration_specifiers declarator compound_statement
	| declarator declaration_list compound_statement
	| declarator compound_statement
	;
**************************************************/
void Grammar_matchFunctionDeclaration(Grammar* this, Token* token)
{ 
  rules[E_FUNCTION_DECLARATION].isMatched = 0;

  switch (rules[E_FUNCTION_DECLARATION].count[this->context])
  {
    case 0:
      Grammar_evaluateRule(this, token, E_DECLARATION_SPECIFIERS);
      if (rules[E_DECLARATION_SPECIFIERS].isMatched)
      {
        rules[E_FUNCTION_DECLARATION].count[this->context] = 1;
      }
      break;
    case 1:
      Grammar_evaluateRule(this, token, E_DECLARATION_SPECIFIERS);
      if (rules[E_DECLARATION_SPECIFIERS].isMatched)
      {
      }
      else
      {
        Grammar_evaluateRule(this, token, E_DECLARATOR);
        if (rules[E_DECLARATOR].isMatched)
        {
          rules[E_FUNCTION_DECLARATION].count[this->context] = 2;
        }
      }
      break;
    case 2:
      Grammar_evaluateRule(this, token, E_DECLARATOR);
      Grammar_evaluateRule(this, token, E_COMPOUND_STATEMENT);
      if (rules[E_COMPOUND_STATEMENT].isMatched)
      {
        rules[E_FUNCTION_DECLARATION].isMatched = 1;
        this->declarator.class = E_FUNCTION_DECLARATOR;
      }
      break;
  }
}

/**************************************************
declaration
	: declaration_specifiers ';'
	| declaration_specifiers init_declarator_list ';'
	;
****************************************************************************/
void Grammar_matchDeclaration(Grammar* this, Token* token)
{
  rules[E_DECLARATION].isMatched = 0;
  
  switch (rules[E_DECLARATION].count[this->context])
  {
    case 0:
      Grammar_evaluateRule(this, token, E_DECLARATION_SPECIFIERS);
      if (rules[E_DECLARATION_SPECIFIERS].isMatched)
      {
        rules[E_DECLARATION].count[this->context] = 1;
      }
      break;
    case 1:
      Grammar_evaluateRule(this, token, E_DECLARATION_SPECIFIERS);
      if (rules[E_DECLARATION_SPECIFIERS].isMatched)
      {
      }
      else if ((token->id == TOK_UNKNOWN) && ((uintptr_t)token->value == ';'))
      {
        rules[E_DECLARATION].isMatched = 1;
        rules[E_DECLARATION].count[this->context] = 0;
      }
      /* else if ; */
      else
      {
        Grammar_evaluateRule(this, token, E_INIT_DECLARATOR_LIST);
        if (rules[E_INIT_DECLARATOR_LIST].isMatched)
        {
          rules[E_DECLARATION].count[this->context] = 2;
        }
      }
      break;
    case 2:
      Grammar_evaluateRule(this, token, E_INIT_DECLARATOR_LIST);
      if (rules[E_INIT_DECLARATOR_LIST].isMatched)
      {
      }
      else if ((token->id == TOK_UNKNOWN) && ((uintptr_t)token->value == ';'))
      {
        rules[E_DECLARATION].isMatched = 1;
        rules[E_DECLARATION].count[this->context] = 0;
      }
      break;
  }
}

/****************************************************************************
declaration_specifiers
	: storage_class_specifier
	| storage_class_specifier declaration_specifiers
	| type_specifier
	| type_specifier declaration_specifiers
	| type_qualifier
	| type_qualifier declaration_specifiers
****************************************************************************/
void Grammar_matchDeclarationSpecifiers(Grammar* this, Token* token)
{
  rules[E_DECLARATION_SPECIFIERS].isMatched = 0;

  Grammar_evaluateRule(this, token, E_STORAGE_CLASS);
  Grammar_evaluateRule(this, token, E_TYPE_SPECIFIER);
  Grammar_evaluateRule(this, token, E_TYPE_QUALIFIER);

  if (rules[E_STORAGE_CLASS].isMatched)
  {
	  rules[E_DECLARATION_SPECIFIERS].isMatched = 1;
  }
  else if (rules[E_TYPE_SPECIFIER].isMatched)
  {
	  rules[E_DECLARATION_SPECIFIERS].isMatched = 1;
  }
  else if (rules[E_TYPE_QUALIFIER].isMatched)
  {
	  rules[E_DECLARATION_SPECIFIERS].isMatched = 1;
  }
}

/****************************************************************************
init_declarator_list
	: init_declarator
	| init_declarator_list ',' init_declarator
****************************************************************************/
void Grammar_matchInitDeclaratorList(Grammar* this, Token* token)
{
  rules[E_INIT_DECLARATOR_LIST].isMatched = 0;

}

/****************************************************************************
init_declarator
	: declarator
	| declarator '=' initializer
****************************************************************************/
void Grammar_matchInitDeclarator(Grammar* this, Token* token)
{
    rules[E_INIT_DECLARATOR].isMatched = 0;

}

/****************************************************************************
declarator
	: pointer direct_declarator
	| direct_declarator
	;
****************************************************************************/
void Grammar_matchDeclarator(Grammar* this, Token* token)
{
  rules[E_DECLARATOR].isMatched = 0;
  switch(rules[E_DECLARATOR].count[this->context])
  {
    case 0:
      Grammar_evaluateRule(this, token, E_POINTER);
      if (rules[E_POINTER].isMatched)
      {
      }
      else if (rules[E_DIRECT_DECLARATOR].isMatched)
      {
        Grammar_evaluateRule(this, token, E_DIRECT_DECLARATOR);
        rules[E_DECLARATOR].isMatched = 1;
      }
    case 1:
      Grammar_evaluateRule(this, token, E_POINTER);
      Grammar_evaluateRule(this, token, E_DIRECT_DECLARATOR);
      if (rules[E_DIRECT_DECLARATOR].isMatched)
      {
        rules[E_DECLARATOR].isMatched = 1;
      }
  }
}

/****************************************************************************
declarator
	: pointer direct_declarator
	| direct_declarator
	;
****************************************************************************/
void Grammar_matchDeclaratorList(Grammar* this, Token* token)
{
  rules[E_DECLARATOR_LIST].isMatched = 0;
}

/****************************************************************************
direct_declarator
	: IDENTIFIER
	| '(' declarator ')'
	| direct_declarator '[' constant_expression ']'
	| direct_declarator '[' ']'
	| direct_declarator '(' parameter_type_list ')'
	| direct_declarator '(' identifier_list ')'
	| direct_declarator '(' ')'
****************************************************************************/
void Grammar_matchDirectDeclarator(Grammar* this, Token* token)
{
  rules[E_DIRECT_DECLARATOR].isMatched = 0;

  switch (rules[E_DIRECT_DECLARATOR].count[this->context])
  {
    case 0:
      if (token->id == TOK_IDENTIFIER)
      {
        rules[E_DIRECT_DECLARATOR].isMatched = 1;
        rules[E_DIRECT_DECLARATOR].count[this->context] = 1;
        
        if (this->context == 0)
        {
          this->declarator.name = String_dup((String*)token->value);
          this->declarator.fName = token->fileName;
        }
      }
      break;
    case 1:
      if ((token->id == TOK_UNKNOWN) && ((uintptr_t)token->value == '('))
      {
        rules[E_DIRECT_DECLARATOR].count[this->context] = 2;
      }
      break;
    case 2:
      if ((token->id == TOK_UNKNOWN) && ((uintptr_t)token->value == ')'))
      {
        rules[E_DIRECT_DECLARATOR].isMatched = 1;
        rules[E_DIRECT_DECLARATOR].count[this->context] = 0;
        Grammar_evaluateRule(this, token, E_EXTERNAL_DECLARATION);
      }
      else
      {
        Grammar_saveContext(this, E_PARAMETER_TYPE_LIST);
        Grammar_evaluateRule(this, token, E_PARAMETER_TYPE_LIST);
      }
  }
}

/****************************************************************************
storage_class_specifier
	: TYPEDEF
	| EXTERN
	| STATIC
	| AUTO
	| REGISTER
	;
****************************************************************************/
void Grammar_matchStorageClass(Grammar* this, Token* token)
{
  rules[E_STORAGE_CLASS].isMatched = 0;

  if ((token->id == TOK_EXTERN) || (token->id == TOK_STATIC) ||
      (token->id == TOK_AUTO) || (token->id == TOK_REGISTER))
  {
    rules[E_STORAGE_CLASS].isMatched = 1;
  }
  else if (token->id == TOK_TYPEDEF)
  {
    rules[E_STORAGE_CLASS].isMatched = 1;
    this->declarator.class = E_TYPE_DECLARATOR;
  }
}

/****************************************************************************
type_specifier
	: VOID
	| CHAR
	| SHORT
	| INT
	| LONG
	| FLOAT
	| DOUBLE
	| SIGNED
	| UNSIGNED
	| struct_or_union_specifier
	| enum_specifier
	| TYPE_NAME
	;
****************************************************************************/
void Grammar_matchTypeSpecifier(Grammar* this, Token* token)
{
  printf("Called match Type Specifier\n");
  rules[E_TYPE_SPECIFIER].isMatched = 0;

  //Grammar_evaluateRule(this, token, E_STRUCT_OR_UNION_SPECIFIER);
  //Grammar_evaluateRule(this, token, E_ENUM_SPECIFIER);
  if ((token->id == TOK_VOID) || (token->id == TOK_CHAR) ||
      (token->id == TOK_SHORT) || (token->id == TOK_INT) ||
      (token->id == TOK_LONG) || (token->id == TOK_FLOAT) ||
      (token->id == TOK_DOUBLE) || (token->id == TOK_SIGNED) ||
      (token->id == TOK_UNSIGNED))
  {
    rules[E_TYPE_SPECIFIER].isMatched = 1;
  }
  else if (rules[E_STRUCT_OR_UNION_SPECIFIER].isMatched)
  {
    rules[E_TYPE_SPECIFIER].isMatched = 1;
    TRACE(("This is a struct here\n"));
  }
  else if (token->id == TOK_IDENTIFIER)
  {
    //rules[E_TYPE_SPECIFIER].isMatched = 1;
    //String_print((String*)token->value, "TYPE SPECIFIER =");
    //rules[E_TYPE_SPECIFIER].isMatched = Grammar_isTypeDefined(this, (String*)token->value);
  }
  else if (rules[E_ENUM_SPECIFIER].isMatched)
  {
    rules[E_TYPE_SPECIFIER].isMatched = 1;
  }

}

/**************************************************
type_qualifier
	: CONST
	| VOLATILE
	;
****************************************************************************/
void Grammar_matchTypeQualifier(Grammar* this, Token* token)
{
  rules[E_TYPE_QUALIFIER].isMatched = 0;

  if ((token->id == TOK_CONST) || (token->id == TOK_VOLATILE))
  {
    rules[E_TYPE_QUALIFIER].isMatched = 1;
  }

}

/**************************************************
type_qualifier_list
	: type_qualifier
	| type_qualifier_list type_qualifier
	;
****************************************************************************/
void Grammar_matchTypeQualifierList(Grammar* this, Token* token)
{
  rules[E_TYPE_QUALIFIER_LIST].isMatched = 0;
}

/****************************************************************************
pointer
	: '*'
	| '*' type_qualifier_list
	| '*' pointer
	| '*' type_qualifier_list pointer
	;
****************************************************************************/
void Grammar_matchPointer(Grammar* this, Token* token)
{
  rules[E_POINTER].isMatched = 0;

  switch(rules[E_POINTER].count[0])
  {
    case 0:
      Grammar_evaluateRule(this, token, E_TYPE_QUALIFIER_LIST);
      if (rules[E_TYPE_QUALIFIER_LIST].isMatched)
      {
        rules[E_POINTER].count[0] = 1;
      }
      if ((token->id == TOK_UNKNOWN) && ((uintptr_t)token->value == '*'))
      {
        rules[E_POINTER].isMatched = 1;
      }
      //Grammar_evaluateRule(this, token, E_DECLARATION_SPECIFIER);
      //Grammar_evaluateRule(this, token, E_DECLARATOR_LIST);
      break;
    case 1:
      break;
  }
}

/****************************************************************************
compound_statement
	: '{' '}'
	| '{' statement_list '}'
	| '{' declaration_list '}'
	| '{' declaration_list statement_list '}'
	;
****************************************************************************/
void Grammar_matchCompountStatement(Grammar* this, Token* token)
{ 
  rules[E_COMPOUND_STATEMENT].isMatched = 0;

  switch (rules[E_COMPOUND_STATEMENT].count[this->context])
  {
    case 0:
      if ((token->id == TOK_UNKNOWN) && (token->value == '{'))
      {
        rules[E_COMPOUND_STATEMENT].count[this->context] = 1;
        Grammar_saveContext(this, E_COMPOUND_STATEMENT);
        rules[E_COMPOUND_STATEMENT].count[this->context] = 1;
      }
      break;
    case 1:
      if ((token->id == TOK_UNKNOWN) && (token->value == '}'))
      {
        rules[E_COMPOUND_STATEMENT].isMatched = 1;
        rules[E_COMPOUND_STATEMENT].isEvaluated = 1;
        Grammar_restoreContext(this, E_EXTERNAL_DECLARATION);
        Grammar_evaluateRule(this, token, E_EXTERNAL_DECLARATION);
      }
      else
      {
        Grammar_evaluateRule(this, token, E_DECLARATION_LIST);
        Grammar_evaluateRule(this, token, E_STATEMENT_LIST);
      }
      break;
  }
}

/****************************************************************************
statement_list
	: statement
	| statement_list statement
	;
****************************************************************************/
void Grammar_matchStatementList(Grammar* this, Token* token)
{
  rules[E_STATEMENT_LIST].isMatched = 0;

}

/****************************************************************************
statement
	: labeled_statement
	| compound_statement
	| expression_statement
	| selection_statement
	| iteration_statement
	| jump_statement
	;
****************************************************************************/
void Grammar_matchStatement(Grammar* this, Token* token)
{
  rules[E_STATEMENT].isMatched = 0;

}

/****************************************************************************
declaration_list
	: declaration
	| declaration_list declaration
	;
****************************************************************************/
void Grammar_matchDeclarationList(Grammar* this, Token* token)
{
  rules[E_DECLARATION_LIST].isMatched = 0;

  Grammar_evaluateRule(this, token, E_DECLARATION);
  if (rules[E_DECLARATION].isMatched)
  {
     printf("Matched local declaration\n");
  }
}

/****************************************************************************
labeled_statement
	: IDENTIFIER ':' statement
	| CASE constant_expression ':' statement
	| DEFAULT ':' statement
	;
****************************************************************************/
void Grammar_matchLabeledStatement(Grammar* this, Token* token)
{
  rules[E_LABELED_STATEMENT].isMatched = 0;
  
}

/****************************************************************************
expression_statement
	: ';'
	| expression ';'
	;
****************************************************************************/
void Grammar_matchExpresionStatement(Grammar* this, Token* token)
{
  rules[E_EXPRESSION_STATEMENT].isMatched = 0;
  
}

/****************************************************************************
selection_statement
	: IF '(' expression ')' statement
	| IF '(' expression ')' statement ELSE statement
	| SWITCH '(' expression ')' statement
  ;
****************************************************************************/
void Grammar_matchSelectionStatement(Grammar* this, Token* token)
{
  rules[E_SELECTION_STATEMENT].isMatched = 0;
  
}

/****************************************************************************
iteration_statement
	: WHILE '(' expression ')' statement
	| DO statement WHILE '(' expression ')' ';'
	| FOR '(' expression_statement expression_statement ')' statement
	| FOR '(' expression_statement expression_statement expression ')' statement
	;
****************************************************************************/
void Grammar_matchIterationStatement(Grammar* this, Token* token)
{
  rules[E_ITERATION_STATEMENT].isMatched = 0;
  
}

/****************************************************************************
jump_statement
	: GOTO IDENTIFIER ';'
	| CONTINUE ';'
	| BREAK ';'
	| RETURN ';'
	| RETURN expression ';'
	;
****************************************************************************/
void Grammar_matchJumpStatement(Grammar* this, Token* token)
{
  rules[E_JUMP_STATEMENT].isMatched = 0;

}

/****************************************************************************
struct_or_union_specifier
	: struct_or_union IDENTIFIER '{' struct_declaration_list '}'
	| struct_or_union '{' struct_declaration_list '}'
	| struct_or_union IDENTIFIER
  ;

struct_or_union
	: STRUCT
	| UNION
	;
****************************************************************************/
void Grammar_matchStructOrUnionSpecifier(Grammar* this, Token* token)
{
  rules[E_STRUCT_OR_UNION_SPECIFIER].isMatched = 0;
  
}

/****************************************************************************
specifier_qualifier_list
	: type_specifier specifier_qualifier_list
	| type_specifier
	| type_qualifier specifier_qualifier_list
	| type_qualifier
	;
****************************************************************************/
void Grammar_matchSpecifierQualifierList(Grammar* this, Token* token)
{
  rules[E_SPECIFIER_QUALIFIER_LIST].isMatched = 0;

  switch (rules[E_SPECIFIER_QUALIFIER_LIST].count[0])
  {
    case 0:
      Grammar_evaluateRule(this, token, E_TYPE_SPECIFIER);
      Grammar_evaluateRule(this, token, E_TYPE_QUALIFIER);
      if (rules[E_TYPE_SPECIFIER].isMatched)
      {
        rules[E_SPECIFIER_QUALIFIER_LIST].isMatched = 1;
      }
      else if (rules[E_TYPE_QUALIFIER].isMatched)
      {
        rules[E_SPECIFIER_QUALIFIER_LIST].isMatched = 1;
      }
      break;
  }
}

/****************************************************************************
identifier_list
	: IDENTIFIER
	| identifier_list ',' IDENTIFIER
	;
****************************************************************************/
void Grammar_matchIdentifierList(Grammar* this, Token* token)
{
  rules[E_IDENTIFIER_LIST].isMatched = 0;
}

/****************************************************************************
parameter_type_list
	: parameter_list
	| parameter_list ',' ELLIPSIS
	;
****************************************************************************/
void Grammar_matchParameterTypeList(Grammar* this, Token* token)
{
  rules[E_PARAMETER_TYPE_LIST].isMatched = 0;

  Grammar_evaluateRule(this, token, E_PARAMETER_LIST);
  if (rules[E_PARAMETER_LIST].isMatched)
  {
    rules[E_PARAMETER_TYPE_LIST].isMatched = 1;
    Grammar_restoreContext(this,E_FUNCTION_DECLARATION);
  }
}

/****************************************************************************
	: parameter_declaration
	| parameter_list ',' parameter_declaration
****************************************************************************/
void Grammar_matchParameterList(Grammar* this, Token* token)
{
  rules[E_PARAMETER_LIST].isMatched = 0;
  
  switch(rules[E_PARAMETER_LIST].count[this->context])
  {
    case 0:
      Grammar_evaluateRule(this, token, E_PARAMETER_DECLARATION);
      if (rules[E_PARAMETER_DECLARATION].isMatched)
      {
        rules[E_PARAMETER_LIST].isMatched = 1;
        rules[E_PARAMETER_LIST].count[this->context] = 1;
      }
      break;
    case 1:
      if  ((token->id == TOK_UNKNOWN) && (token->value == ','))
      {
        rules[E_DECLARATION_SPECIFIERS].count[this->context] = 0;
        rules[E_DECLARATOR].count[this->context] = 0;
        rules[E_DIRECT_DECLARATOR].count[this->context] = 0;
      }
      else
      {
        Grammar_evaluateRule(this, token, E_PARAMETER_DECLARATION);
        if (rules[E_PARAMETER_DECLARATION].isMatched)
        {
          rules[E_PARAMETER_LIST].isMatched = 1;
        }
      }
      break;
  }
}

/****************************************************************************
parameter_declaration
	: declaration_specifiers declarator
	| declaration_specifiers abstract_declarator
	| declaration_specifiers
****************************************************************************/
void Grammar_matchParameterDeclaration(Grammar* this, Token* token)
{
  rules[E_PARAMETER_DECLARATION].isMatched = 0;

  switch(rules[E_PARAMETER_DECLARATION].count[this->context])
  {
    case 0:
      Grammar_evaluateRule(this, token, E_DECLARATION_SPECIFIERS);
      if (rules[E_DECLARATION_SPECIFIERS].isMatched)
      {
        rules[E_PARAMETER_DECLARATION].count[this->context] = 1;
        rules[E_PARAMETER_DECLARATION].isMatched = 1;
      }
      break;
    case 1:
      Grammar_evaluateRule(this, token, E_DECLARATION_SPECIFIERS);
      if (rules[E_DECLARATION_SPECIFIERS].isMatched)
      {
        rules[E_PARAMETER_DECLARATION].isMatched = 1;
      }
      else
      {
        Grammar_evaluateRule(this, token, E_DECLARATOR);
        if (rules[E_DECLARATOR].isMatched)
        {
          rules[E_PARAMETER_DECLARATION].isMatched = 1;
          rules[E_DECLARATOR].count[this->context] = 2;
        }
      }
      break;
    case 2:
      Grammar_evaluateRule(this, token, E_DECLARATOR);
      if (rules[E_DECLARATOR].isMatched)
        {
          rules[E_PARAMETER_DECLARATION].isMatched = 1;
        }
      break;
  }
}

/****************************************************************************
abstract_declarator
	: pointer
	| direct_abstract_declarator
	| pointer direct_abstract_declarator
****************************************************************************/
void Grammar_matchAbstractDeclarator(Grammar* this, Token* token)
{
  rules[E_ABSTRACT_DECLARATOR].isMatched = 0;
}

/****************************************************************************
direct_abstract_declarator
	: '(' abstract_declarator ')'
	| '[' ']'
	| '[' constant_expression ']'
	| direct_abstract_declarator '[' ']'
	| direct_abstract_declarator '[' constant_expression ']'
	| '(' ')'
	| '(' parameter_type_list ')'
	| direct_abstract_declarator '(' ')'
	| direct_abstract_declarator '(' parameter_type_list ')'
****************************************************************************/
void Grammar_matchDirectAbstractDeclarator(Grammar* this, Token* token)
{
  rules[E_DIRECT_ABSTRACT_DECLARATOR].isMatched = 0;
}

/****************************************************************************
****************************************************************************/
void Grammar_evaluateRule(Grammar* this, Token* token, RuleName r)
{
  if (!rules[r].isEvaluated)
  {
    rules[r].evaluate(this, token);
    rules[r].isEvaluated = 1;
  }
}

/****************************************************************************
****************************************************************************/
void Grammar_saveContext(Grammar* this, RuleName entryRule)
{
  if (this->context < CONTEXT_DEPTH)
  {
    this->context++;
    this->entryRule[this->context] = entryRule;
  }
}

/****************************************************************************
****************************************************************************/
void Grammar_restoreContext(Grammar* this, RuleName entryRule)
{
  if (this->context > 0)
  {
    this->context--;
    this->entryRule[this->context] = entryRule;
  }
}

/****************************************************************************
****************************************************************************/
void Grammar_printDeclarator(Grammar* this)
{
  switch (this->declarator.class)
  {
    case E_TYPE_DECLARATOR:
      if (this->scope == E_GLOBAL_SCOPE)
      {
        String_print(this->declarator.name,"Global type declaration found: ");
        Grammar_insertDeclaration(this, &this->declarator);
      }
      String_delete(this->declarator.name);
      break;
    case E_FUNCTION_DECLARATOR:
      if (this->scope == E_GLOBAL_SCOPE)
      {
        String_print(this->declarator.name,"Global function declaration found: ");
        Grammar_insertDeclaration(this, &this->declarator);
      }
      else
      {
        String_print(this->declarator.name,"Local function declaration found: ");
      }
      String_delete(this->declarator.name);
      break;
    case E_VARIABLE_DECLARATOR:
      if (this->scope == E_GLOBAL_SCOPE)
      {
        String_print(this->declarator.name,"Global variable declaration found: ");
        Grammar_insertDeclaration(this, &this->declarator);
      }
      String_delete(this->declarator.name);
      break;
    case E_UNKNOWN_DECLARATOR:
      String_print(this->declarator.name,"Unkown declaration found: ");
      String_delete(this->declarator.name);
      break;
  }
}

/****************************************************************************
****************************************************************************/
void Grammar_printMatchingRules(Grammar* this, Token* token)
{
  unsigned int nbRules = sizeof(rules)/sizeof(MatchRule);
  RuleName i = E_EXTERNAL_DECLARATION;

  for (i=E_EXTERNAL_DECLARATION; i<nbRules; i++)
  {
    if (rules[i].isMatched)
    {
      printf("--- %s is matched.\n", rules[i].description);
    }
  }

}

/****************************************************************************
****************************************************************************/
void Grammar_insertDeclaration(Grammar* this, Declarator* declarator)
{
  SdbMgr* sdbMgr = SdbMgr_getSdbMgr();
  char cmd[255];
  char name[255];
  char fName[255];
  char *classText[] = {"Unknown","Function","Variable","Type"};
  char *scopeText[] = {"Unknown","Local","Global"};

  memset(name, 0, 255);
  memset(fName, 0, 255);
  memset(cmd, 0, 255);
  if (declarator->name)
  {
    memcpy(name, declarator->name->buffer, declarator->name->length);
    memcpy(fName, declarator->fName->buffer, declarator->fName->length);
  
    sprintf(cmd, "INSERT INTO Declarations ( name, type, scope, fname, line, column ) "
                 "VALUES ('%s','%s','%s','%s',%d, %d);", 
                 name, 
                 classText[declarator->class], 
                 scopeText[this->scope],
                 fName,
                 declarator->line,
                 declarator->col);
  }
  SdbMgr_execute(sdbMgr, cmd);
  SdbMgr_delete(sdbMgr);
}

/****************************************************************************
****************************************************************************/
unsigned int Grammar_isTypeDefined(Grammar* this, String* typeName)
{
  unsigned int result = 0;
  SdbMgr* sdbMgr = SdbMgr_getSdbMgr();
  char cmd[255];
  char name[255];
  char **query = NULL;

  memset(cmd, 0, 255);
  memset(name, 0, 255);

  memcpy(name, typeName->buffer, typeName->length);
  sprintf(cmd,"SELECT * FROM Declarations WHERE name='%s';", name);

  SdbMgr_execute(sdbMgr, cmd);

  if (SdbMgr_getQueryCount(sdbMgr))
  {
    printf("Found!\n");
    query = SdbMgr_getQueryResult(sdbMgr);
    Memory_free(query, sizeof(query));
    result = 1;
  }

  SdbMgr_delete(sdbMgr);
  
  return result;
}
