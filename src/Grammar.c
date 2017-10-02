/* Grammar.c */

#include "Grammar.h"

#include "Common.h"
#include "Token.h"
#include "List.h"
#include "SdbMgr.h"

#include <stdint.h>

#define DEBUG (1)

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
  unsigned int count[5];
  unsigned int context;
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
};

struct Grammar{

  unsigned int isInStructDefinition;
  unsigned int requestReset;
  Scope scope;
  unsigned int tokenNumber;
  Declarator declarator;
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
void Grammar_matchStructOrUnionSpecifier(Grammar* this, Token* token);
void Grammar_matchStructOrUnionSpecifier2(Grammar* this, Token* token);
void Grammar_matchStructDeclarationList(Grammar* this, Token* token);
void Grammar_matchSpecifierQualifierList(Grammar* this, Token* token);
void Grammar_matchStructDeclaratorList(Grammar* this, Token* token);
void Grammar_matchStructDeclarator(Grammar* this, Token* token);
void Grammar_matchEnumSpecifier(Grammar* this, Token* token);
void Grammar_matchEnumerator(Grammar* this, Token* token);
void Grammar_matchEnumeratorList(Grammar* this, Token* token);
void Grammar_matchStructDeclaration(Grammar* this, Token* token);
void Grammar_evaluateRule(Grammar* this, Token* token, RuleName r);
void Grammar_matchInitializer(Grammar* this, Token* token);
void Grammar_matchParameterTypeList(Grammar* this, Token* token);
void Grammar_matchIdentifierList(Grammar* this, Token* token);
void Grammar_matchParameterList(Grammar* this, Token* token);
void Grammar_matchAbstractDeclarator(Grammar* this, Token* token);
void Grammar_matchDirectAbstractDeclarator(Grammar* this, Token* token);
void Grammar_matchDeclarationList(Grammar* this, Token* token);
void Grammar_matchParameterDeclaration(Grammar* this, Token* token);
void Grammar_matchConstantExpresion(Grammar* this, Token* token);
void Grammar_matchConditionalExpression(Grammar* this, Token* token);
void Grammar_matchIterationStatement(Grammar* this, Token* token);
void Grammar_matchJumpStatement(Grammar* this, Token* token);
void Grammar_matchSelectionStatement(Grammar* this, Token* token);
void Grammar_matchExpresionStatement(Grammar* this, Token* token);
void Grammar_matchLabeledStatement(Grammar* this, Token* token);
void Grammar_matchStatement(Grammar* this, Token* token);
void Grammar_matchStatementList(Grammar* this, Token* token);
void Grammar_matchExpression(Grammar* this, Token* token);
void Grammar_matchPrimaryExpression(Grammar* this, Token* token);
void Grammar_matchAssignmentExpression(Grammar* this, Token* token);
void Grammar_matchPostfixExpression(Grammar* this, Token* token);
void Grammar_matchUnaryExpression(Grammar* this, Token* token);
void Grammar_matchCastExpression(Grammar* this, Token* token);
void Grammar_matchTypeName(Grammar* this, Token* token);
void Grammar_printDeclarator(Grammar* this);
void Grammar_printMatchingRules(Grammar* this, Token* token);
void Grammar_reset(Grammar* this, Scope scope);
void Grammar_insertDeclaration(Grammar* this, Declarator* declarator);
unsigned int Grammar_isTypeDefined(Grammar* this, String* typeName);

MatchRule rules[] = { { E_EXTERNAL_DECLARATION , "EXTERNAL_DECLARATION", 0 , 0, &Grammar_matchExternalDeclaration, {0, 0, 0, 0, 0 } ,0 },
                      { E_FUNCTION_DECLARATION , "FUNCTION_DECLARATION", 0 , 0, &Grammar_matchFunctionDeclaration, {0, 0, 0, 0, 0 } ,0 },
                      { E_DECLARATION , "E_DECLARATION", 0 , 0, &Grammar_matchDeclaration, {0, 0, 0, 0, 0 } ,0 },
                      { E_COMPOUND_STATEMENT , "COMPOUND_STATEMENT", 0 , 0, &Grammar_matchCompountStatement, {0, 0, 0, 0, 0 } ,0 },
                      { E_DECLARATION_SPECIFIERS , "DECLARATION_SPECIFIERS", 0 , 0, &Grammar_matchDeclarationSpecifiers, {0, 0, 0, 0, 0 } ,0 },
                      { E_DECLARATOR , "DECLARATOR", 0 , 0, &Grammar_matchDeclarator, {0, 0, 0, 0, 0 } ,0 },
                      { E_DIRECT_DECLARATOR , "DIRECT_DECLARATOR", 0 , 0, &Grammar_matchDirectDeclarator, {0, 0, 0, 0, 0 } ,0 },
                      { E_DECLARATOR_LIST , "DECLARATOR_LIST", 0 , 0, &Grammar_matchDeclaratorList, {0, 0, 0, 0, 0 } ,0 },
                      { E_INIT_DECLARATOR_LIST , "INIT_DECLARATOR_LIST", 0 , 0, &Grammar_matchInitDeclaratorList, {0, 0, 0, 0, 0 } ,0 },
                      { E_INIT_DECLARATOR , "INIT_DECLARATOR", 0 , 0, &Grammar_matchInitDeclarator, {0, 0, 0, 0, 0 } ,0 },
                      { E_STORAGE_CLASS , "STORAGE_CLASS", 0 , 0, &Grammar_matchStorageClass, {0, 0, 0, 0, 0 } ,0 },
                      { E_TYPE_SPECIFIER , "TYPE_SPECIFIER", 0 , 0, &Grammar_matchTypeSpecifier, {0, 0, 0, 0, 0 } ,0 },
                      { E_TYPE_QUALIFIER , "TYPE_QUALIFIER", 0 , 0, &Grammar_matchTypeQualifier, {0, 0, 0, 0, 0 } ,0 },
                      { E_TYPE_QUALIFIER_LIST , "TYPE_QUALIFIER_LIST", 0 , 0, &Grammar_matchTypeQualifierList, {0, 0, 0, 0, 0 } ,0 },
                      { E_STRUCT_OR_UNION_SPECIFIER , "STRUCT_OR_UNION_SPECIFIER", 0 , 0, &Grammar_matchStructOrUnionSpecifier, {0, 0, 0, 0, 0 } ,0 },
                      { E_STRUCT_OR_UNION_SPECIFIER2 , "STRUCT_OR_UNION_SPECIFIER2", 0 , 0, &Grammar_matchStructOrUnionSpecifier2, {0, 0, 0, 0, 0 } ,0 },
                      { E_STRUCT_DECLARATION_LIST , "STRUCT_DECLARATION_LIST ", 0 , 0, &Grammar_matchStructDeclarationList, {0, 0, 0, 0, 0 } ,0 },
                      { E_STRUCT_DECLARATION , "STRUCT_DECLARATION", 0 , 0, &Grammar_matchStructDeclaration, {0, 0, 0, 0, 0 } ,0 },
                      { E_SPECIFIER_QUALIFIER_LIST, "SPECIFIER_QUALIFIER_LIST", 0, 0, &Grammar_matchSpecifierQualifierList, {0, 0, 0, 0, 0 } ,0 },
                      { E_STRUCT_DECLARATOR_LIST, "STRUCT_DECLARATOR_LIST", 0, 0, &Grammar_matchStructDeclaratorList, {0, 0, 0, 0, 0 } ,0 },
                      { E_STRUCT_DECLARATOR, "STRUCT_DECLARATOR", 0, 0, &Grammar_matchStructDeclarator, {0, 0, 0, 0, 0 } ,0 },
                      { E_ENUMERATOR , "ENUMERATOR", 0 , 0, &Grammar_matchEnumerator, {0, 0, 0, 0, 0 } ,0 },
                      { E_ENUMERATOR_LIST , "ENUMERATOR_LIST", 0 , 0, &Grammar_matchEnumeratorList, {0, 0, 0, 0, 0 } ,0 },
                      { E_ENUM_SPECIFIER , "ENUM_SPECIFIER", 0 , 0, &Grammar_matchEnumSpecifier, {0, 0, 0, 0, 0 } ,0 },
                      { E_POINTER , "POINTER", 0 , 0, &Grammar_matchPointer, {0, 0, 0, 0, 0 } ,0 },
                      { E_CONSTANT_EXPRESSION , "CONSTANT_EXPRESSION", 0 , 0, &Grammar_matchConstantExpresion, {0, 0, 0, 0, 0 } ,0 },
                      { E_INITIALIZER, "INITIALIZER", 0, 0, &Grammar_matchInitializer, {0, 0, 0, 0, 0 } ,0 },
                      { E_PARAMETER_TYPE_LIST, "PARAMETER_TYPE_LIST", 0, 0, &Grammar_matchParameterTypeList, {0, 0, 0, 0, 0 } ,0 },
                      { E_IDENTIFIER_LIST, "IDENTIFIER_LIST", 0, 0, &Grammar_matchIdentifierList, {0, 0, 0, 0, 0 } ,0 },
                      { E_PARAMETER_LIST, "PARAMETER_LIST", 0, 0, &Grammar_matchParameterList, {0, 0, 0, 0, 0 } ,0 },
                      { E_ABSTRACT_DECLARATOR, "ABSTRACT_DECLARATOR", 0, 0, &Grammar_matchAbstractDeclarator, {0, 0, 0, 0, 0 } ,0 },
                      { E_DIRECT_ABSTRACT_DECLARATOR, "DIRECT_ABSTRACT_DECLARATOR", 0, 0, &Grammar_matchDirectAbstractDeclarator, {0, 0, 0, 0, 0 } ,0 },
                      { E_DECLARATION_LIST, "DECLARATION_LIST", 0, 0, &Grammar_matchDeclarationList, {0, 0, 0, 0, 0 } ,0 },
                      { E_PARAMETER_DECLARATION, "PARAMETER_DECLARATION", 0, 0, &Grammar_matchParameterDeclaration, {0, 0, 0, 0, 0 } ,0 },
		                  { E_CONDITIONAL_EXPRESSION, "CONDITIONAL_EXPRESSION", 0, 0, &Grammar_matchConditionalExpression, {0, 0, 0, 0, 0 } ,0 },
                      { E_STATEMENT_LIST, "STATEMENT_LIST", 0, 0, &Grammar_matchStatementList, {0, 0, 0, 0, 0 } ,0 },
                      { E_STATEMENT, "STATEMENT", 0, 0, &Grammar_matchStatement, {0, 0, 0, 0, 0 } ,0 },
                      { E_LABELED_STATEMENT, "LABELED_STATEMENT", 0, 0, &Grammar_matchLabeledStatement, {0, 0, 0, 0, 0 } ,0 },
                      { E_EXPRESSION_STATEMENT, "EXPRESSION_STATEMENT", 0, 0, &Grammar_matchExpresionStatement, {0, 0, 0, 0, 0 } ,0 },
                      { E_SELECTION_STATEMENT, "SELECTION_STATEMENT", 0, 0, &Grammar_matchSelectionStatement, {0, 0, 0, 0, 0 } ,0 },
                      { E_ITERATION_STATEMENT, "ITERATION_STATEMENT", 0, 0, &Grammar_matchIterationStatement, {0, 0, 0, 0, 0 } ,0 },
                      { E_JUMP_STATEMENT, "JUMP_STATEMENT", 0, 0, &Grammar_matchJumpStatement, {0, 0, 0, 0, 0 } ,0 },
                      { E_EXPRESSION, "EXPRESSION", 0, 0, &Grammar_matchExpression, {0, 0, 0, 0, 0 } ,0 },
                      { E_ASSIGNMENT_EXPRESSION, "ASSIGNMENT_EXPRESSION", 0, 0, &Grammar_matchAssignmentExpression, {0, 0, 0, 0, 0 } ,0 },
                      { E_PRIMARY_EXPRESSION, "PRIMARY_EXPRESSION", 0, 0, &Grammar_matchPrimaryExpression, {0, 0, 0, 0, 0 } ,0 },
                      { E_POSTFIX_EXPRESSION, "POSTFIX_EXPRESSION", 0, 0, &Grammar_matchPostfixExpression, {0, 0, 0, 0, 0 } ,0 },
                      { E_UNARY_EXPRESSION, "UNARY_EXPRESSION", 0, 0, &Grammar_matchUnaryExpression, {0, 0, 0, 0, 0 } ,0 },
                      { E_CAST_EXPRESSION, "CAST_EXPRESSION", 0, 0, &Grammar_matchCastExpression, {0, 0, 0, 0, 0 } ,0 },
                      { E_TYPE_NAME, "TYPE_NAME", 0, 0, Grammar_matchTypeName, {0, 0, 0, 0, 0 } ,0 }
                    };

/****************************************************************************
****************************************************************************/
Grammar* Grammar_new()
{
  Grammar* this = NULL;

  this = (Grammar*)Memory_alloc(sizeof(Grammar));
  // New typelist hash
  // New function hash
  // New Global Var hash
  Grammar_reset(this, E_GLOBAL_SCOPE);
  this->scope = E_GLOBAL_SCOPE;
  this->tokenNumber = 1;
  this->requestReset = 0;
  this->isInStructDefinition = 0;

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
void Grammar_reset(Grammar* this, Scope scope)
{
  unsigned int nbRules = sizeof(rules)/sizeof(MatchRule);
  RuleName i = E_EXTERNAL_DECLARATION;

  TRACE(("Grammar Reset\n"));
  for (i=E_EXTERNAL_DECLARATION; i<nbRules; i++)
  {
      if ((scope == E_LOCAL_SCOPE) && ((i==E_FUNCTION_DECLARATION) || (i==E_STRUCT_OR_UNION_SPECIFIER) || (i==E_DECLARATION)))
      {
      }
      else
      {
        rules[i].isMatched = 0;
        rules[i].isEvaluated = 0;
        if (rules[i].count[0]!=0) TRACE(("-- Set count to zero: %s\n", rules[i].description));
        memset(rules[i].count, 0, sizeof(rules[i].count));
      }
  }
  this->declarator.name = NULL;
  this->declarator.class = E_UNKNOWN_DECLARATOR;
}

/****************************************************************************
****************************************************************************/
void Grammar_pushToken(Grammar* this, Token* token)
{
  unsigned int nbRules = sizeof(rules)/sizeof(MatchRule);
  unsigned int i = 0;

  TRACE(("-- Processing token #%d: %s\n", this->tokenNumber, token->text));

  Grammar_matchExternalDeclaration(this, token);

  this->tokenNumber++;
  Grammar_printMatchingRules(this, token);

  if (rules[E_EXTERNAL_DECLARATION].isMatched)
  {
    Grammar_printDeclarator(this);
    Grammar_reset(this, E_GLOBAL_SCOPE);
  }
  if (this->requestReset)
  {
    Grammar_reset(this, E_LOCAL_SCOPE);
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

  switch(rules[E_FUNCTION_DECLARATION].count[0])
  {
    case 0:
      Grammar_evaluateRule(this, token, E_DECLARATION_SPECIFIERS);
      if (rules[E_DECLARATION_SPECIFIERS].isMatched)
      {
        rules[E_FUNCTION_DECLARATION].count[0] = 1;
      }
      break;
    case 1:
      Grammar_evaluateRule(this, token, E_DECLARATION_SPECIFIERS);
      Grammar_evaluateRule(this, token, E_DECLARATOR);
      if (rules[E_DECLARATION_SPECIFIERS].isMatched)
      {
      }
      else if ((rules[E_DECLARATOR].isMatched))
      {
        rules[E_FUNCTION_DECLARATION].count[0] = 2;
      }
      else
      {
        // Syntax Error
      }
      break;
    case 2:
      Grammar_evaluateRule(this, token, E_DECLARATOR);
      Grammar_evaluateRule(this, token, E_COMPOUND_STATEMENT);
      if (rules[E_DECLARATOR].isMatched)
      {
        // Consume
      }
      else if ((rules[E_COMPOUND_STATEMENT].isMatched) && (this->scope == E_GLOBAL_SCOPE))
      {
        rules[E_FUNCTION_DECLARATION].isMatched = 1;
      }
      else
      {
        // Keep looking for '}'
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

  if (!this->isInStructDefinition)
  {
  switch(rules[E_DECLARATION].count[0])
  {
    case 0:
      Grammar_evaluateRule(this, token, E_DECLARATION_SPECIFIERS);
      if (rules[E_DECLARATION_SPECIFIERS].isMatched)
      {
        rules[E_DECLARATION].count[0] = 1;
      }
      break;
    case 1:
      Grammar_evaluateRule(this, token, E_DECLARATION_SPECIFIERS);
      Grammar_evaluateRule(this, token, E_INIT_DECLARATOR_LIST);
      if (rules[E_DECLARATION_SPECIFIERS].isMatched)
      {

      }
      else if (rules[E_INIT_DECLARATOR_LIST].isMatched)
      {
        rules[E_DECLARATION].count[0] = 2 ;
      }
      else if ((token->id == TOK_UNKNOWN) && ((uintptr_t)token->value == ';'))
      {
        rules[E_DECLARATION].isMatched = 1;
        rules[E_DECLARATION].count[0] = 0 ;
        TRACE(("matchDeclaration count %d\n", rules[E_DECLARATION].count[0]));
      }
      break;
    case 2:
      Grammar_evaluateRule(this, token, E_INIT_DECLARATOR_LIST);
      if (rules[E_INIT_DECLARATOR_LIST].isMatched)
      {
        // COnsume
      }
      else if ((token->id == TOK_UNKNOWN) && ((uintptr_t)token->value == ';'))
      {
        rules[E_DECLARATION].isMatched = 1;
        rules[E_DECLARATION].count[0] = 0 ;
      }
      else if ((token->id == TOK_UNKNOWN) && ((uintptr_t)token->value == '{'))
      {
        rules[E_DECLARATION].count[0] = 0 ;
      }
      break;
  }
  }
  else
  {
    switch(rules[E_DECLARATION].count[0])
    {
    case 0:
      break;
    case 1:
      Grammar_evaluateRule(this, token, E_DECLARATION_SPECIFIERS);
      break;
    case 2:
      break;
    }
  }
  // Check if inside a struct definition
  if (rules[E_STRUCT_OR_UNION_SPECIFIER].count[0] != 0)
  {
    rules[E_DECLARATION].isMatched = 0;
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

  switch (rules[E_INIT_DECLARATOR_LIST].count[0])
  {
    case 0:
      Grammar_evaluateRule(this, token, E_INIT_DECLARATOR);
      if (rules[E_INIT_DECLARATOR].isMatched)
      {
        rules[E_INIT_DECLARATOR_LIST].isMatched = 1;
      }
      else if ((token->id == TOK_UNKNOWN) && ((uintptr_t)token->value == ','))
      {
      }
      else
      {
        // Syntax Error
      }
      break;
  }
}

/****************************************************************************
init_declarator
	: declarator
	| declarator '=' initializer
****************************************************************************/
void Grammar_matchInitDeclarator(Grammar* this, Token* token)
{
    rules[E_INIT_DECLARATOR].isMatched = 0;

    switch (rules[E_INIT_DECLARATOR].count[0])
    {
      case 0:
        Grammar_evaluateRule(this, token, E_DECLARATOR);
        if (rules[E_DECLARATOR].isMatched)
        {
          rules[E_INIT_DECLARATOR].isMatched = 1;
          rules[E_INIT_DECLARATOR].count[0] = 1;
        }
        break;
      case 1:
        if ((token->id == TOK_UNKNOWN) && ((uintptr_t)token->value == '='))
        {
          rules[E_INIT_DECLARATOR].count[0] = 2;
        }
        else
        {
         // Error
        }
        break;
      case 2:
        Grammar_evaluateRule(this, token, E_INITIALIZER);
        if (rules[E_INITIALIZER].isMatched)
        {
          rules[E_INIT_DECLARATOR].isMatched = 1;
        }
        break;
    }
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

  switch (rules[E_DECLARATOR].count[0])
  {
    case 0:
      Grammar_evaluateRule(this, token, E_POINTER);
      Grammar_evaluateRule(this, token, E_DIRECT_DECLARATOR);
      if (rules[E_POINTER].isMatched)
      {
        rules[E_DECLARATOR].count[0] = 1;
      }
      else if (rules[E_DIRECT_DECLARATOR].isMatched)
      {
          rules[E_DECLARATOR].isMatched = 1;
      }
      else
      {
        // Syntax Error
      }
      break;
    case 1:
      Grammar_evaluateRule(this, token, E_DIRECT_DECLARATOR);
      if (rules[E_DIRECT_DECLARATOR].isMatched)
      {
          rules[E_DECLARATOR].isMatched = 1;
      }
      else
      {
        // Syntax Error
      }
      break;
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

  switch (rules[E_DIRECT_DECLARATOR].count[0])
  {
    case 0:
      if (token->id == TOK_IDENTIFIER)
      {
        rules[E_DIRECT_DECLARATOR].isMatched = 1;
        rules[E_DIRECT_DECLARATOR].count[0] = 1;
        this->declarator.name = String_dup((String*)token->value);
        if (this->declarator.class!=E_TYPE_DECLARATOR)
        {
          this->declarator.class = E_VARIABLE_DECLARATOR;
        }
      }
      else if ((token->id == TOK_UNKNOWN) && ((uintptr_t)token->value == '('))
      {
        // Not supported
      }
      else
      {
        // Syntax Error
      }
      break;
    case 1:
      if ((token->id == TOK_UNKNOWN) && ((uintptr_t)token->value == '['))
      {
        rules[E_DIRECT_DECLARATOR].count[0] = 2;
      }
      else if ((token->id == TOK_UNKNOWN) && ((uintptr_t)token->value == '('))
      {
        rules[E_DIRECT_DECLARATOR].count[0] = 3;
        this->declarator.class = E_FUNCTION_DECLARATOR;
      }
      break;
    case 2:
      Grammar_evaluateRule(this, token, E_CONSTANT_EXPRESSION);
      if ((token->id == TOK_UNKNOWN) && ((uintptr_t)token->value == ']'))
      {
      }
      else if (rules[E_CONSTANT_EXPRESSION].isMatched)
      {
        rules[E_DIRECT_DECLARATOR].count[0] = 4;
      }
      break;
    case 3:
      Grammar_evaluateRule(this, token, E_PARAMETER_TYPE_LIST);
      Grammar_evaluateRule(this, token, E_IDENTIFIER_LIST);
      if ((token->id == TOK_UNKNOWN) && ((uintptr_t)token->value == ')'))
      {
        rules[E_DIRECT_DECLARATOR].isMatched = 1;
      }
      else if (rules[E_PARAMETER_TYPE_LIST].isMatched)
      {
        rules[E_DIRECT_DECLARATOR].count[0] = 5;
      }
      else if (rules[E_IDENTIFIER_LIST].isMatched)
      {
        printf("Not supported\n");
      }
      break;
    case 4:
      printf("Not Supported!/n");
      break;
    case 5:
      if ((token->id == TOK_UNKNOWN) && ((uintptr_t)token->value == ')'))
      {
        rules[E_DIRECT_DECLARATOR].isMatched = 1;
      }
      break;
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
  rules[E_TYPE_SPECIFIER].isMatched = 0;

  Grammar_evaluateRule(this, token, E_STRUCT_OR_UNION_SPECIFIER);
  Grammar_evaluateRule(this, token, E_ENUM_SPECIFIER);
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
    rules[E_TYPE_SPECIFIER].isMatched = Grammar_isTypeDefined(this, (String*)token->value);
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
  unsigned int context = rules[E_COMPOUND_STATEMENT].context;
  static unsigned int nb_compound_statement;
  
  rules[E_COMPOUND_STATEMENT].isMatched = 0;

  switch(rules[E_COMPOUND_STATEMENT].count[context])
  {
    case 0:
      if ((token->id == TOK_UNKNOWN) && ((uintptr_t)token->value == '{'))
      {
        rules[E_COMPOUND_STATEMENT].count[context] = 1;
        this->scope = E_LOCAL_SCOPE;
        nb_compound_statement = 1;
        TRACE(("-- Start compound statement\n"));
      }
      break;
    case 1:

      if ((token->id == TOK_UNKNOWN) && ((uintptr_t)token->value == '}'))
      {
        if (nb_compound_statement > 0)
        {
          nb_compound_statement--;
        }
        if (nb_compound_statement == 0) this->scope = E_GLOBAL_SCOPE;
        rules[E_COMPOUND_STATEMENT].isMatched  = 1;
        TRACE(("-- End compound statement\n"));
      }
      else if ((token->id == TOK_UNKNOWN) && ((uintptr_t)token->value == '{'))
      {
        nb_compound_statement++;
        rules[E_COMPOUND_STATEMENT].isEvaluated = 1;
        Grammar_evaluateRule(this, token, E_STATEMENT_LIST);
        Grammar_evaluateRule(this, token, E_DECLARATION_LIST);
      }
      else
      {
        rules[E_COMPOUND_STATEMENT].isEvaluated = 1;
        Grammar_evaluateRule(this, token, E_STATEMENT_LIST);
        Grammar_evaluateRule(this, token, E_DECLARATION_LIST);
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

  Grammar_evaluateRule(this, token, E_STATEMENT);
  if (rules[E_STATEMENT].isMatched)
  {
    rules[E_STATEMENT_LIST].isMatched = 1;
  }
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
    rules[E_DECLARATION_LIST].isMatched = 1;
    TRACE(("Declaration_list: Matched local declaration\n"));
  }
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

  Grammar_evaluateRule(this, token, E_LABELED_STATEMENT);
  Grammar_evaluateRule(this, token, E_COMPOUND_STATEMENT);
  Grammar_evaluateRule(this, token, E_EXPRESSION_STATEMENT);
  Grammar_evaluateRule(this, token, E_SELECTION_STATEMENT);
  Grammar_evaluateRule(this, token, E_ITERATION_STATEMENT);
  Grammar_evaluateRule(this, token, E_JUMP_STATEMENT);

  if (rules[E_LABELED_STATEMENT].isMatched)
  {
    rules[E_STATEMENT].isMatched = 1;
        printf("Statement matched\n");
  }
  else if (rules[E_COMPOUND_STATEMENT].isMatched)
  {
    rules[E_STATEMENT].isMatched = 1;
    printf("matchStatement: watch out!\n");
  }
  else if (rules[E_EXPRESSION_STATEMENT].isMatched)
  {
    rules[E_STATEMENT].isMatched = 1;
        printf("Statement matched\n");
  }
  else if (rules[E_SELECTION_STATEMENT].isMatched)
  {
    rules[E_STATEMENT].isMatched = 1;
        printf("Statement matched\n");
  }
  else if (rules[E_ITERATION_STATEMENT].isMatched)
  {
    rules[E_STATEMENT].isMatched = 1;
        printf("Statement matched\n");
  }
  else if (rules[E_ITERATION_STATEMENT].isMatched)
  {
    rules[E_STATEMENT].isMatched = 1;
        printf("Statement matched\n");
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
  static int preventRecursion;
  
  rules[E_SELECTION_STATEMENT].isMatched = 0;
  
  if (!preventRecursion)
  {
  switch(rules[E_SELECTION_STATEMENT].count[0])
  {
    case 0:
      if (token->id == TOK_IF)
      {
        rules[E_SELECTION_STATEMENT].count[0] = 1;
      }
      break;
    case 1:
      if ((token->id == TOK_UNKNOWN) && ((uintptr_t)token->value == '('))
      {
        rules[E_SELECTION_STATEMENT].count[0] = 2;
      }
      break;
    case 2:
      Grammar_evaluateRule(this, token, E_EXPRESSION);
      if ((token->id == TOK_UNKNOWN) && ((uintptr_t)token->value == ')'))
      {
        rules[E_SELECTION_STATEMENT].count[0] = 3;
      }
      break;
    case 3:
      preventRecursion = 1;
      Grammar_evaluateRule(this, token, E_STATEMENT);
      preventRecursion = 0;
      if (token->id == TOK_ELSE)
      {
        rules[E_SELECTION_STATEMENT].count[0] = 4;
      }
      else if (rules[E_STATEMENT].isMatched)
      {
        rules[E_SELECTION_STATEMENT].isMatched = 1;
      }
    case 4:
      preventRecursion = 1;
      Grammar_evaluateRule(this, token, E_STATEMENT);
      preventRecursion = 0;
      if (rules[E_STATEMENT].isMatched)
      {
        rules[E_SELECTION_STATEMENT].isMatched = 1;
      }
  }
  }
  else
  {
    //printf("Error no recursive if\n");
  }
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

  switch (rules[E_JUMP_STATEMENT].count[0])
  {
    case 0:
      if (token->id == TOK_GOTO)
      {
      }
      else if (token->id == TOK_CONTINUE)
      {
      }
      else if (token->id == TOK_BREAK)
      {
      }
      else if (token->id == TOK_RETURN)
      {
        rules[E_JUMP_STATEMENT].count[0] = 1;
      }
      break;
    case 1:
      if (rules[E_EXPRESSION].isMatched)
      {
      }
      else if ((token->id == TOK_UNKNOWN) && ((uintptr_t)token->value == ';'))
      {
        rules[E_JUMP_STATEMENT].isMatched = 1;
      }
      break;
  }
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
  static int preventRecursion;

  rules[E_STRUCT_OR_UNION_SPECIFIER].isMatched = 0;

  if (preventRecursion)
  {
    Grammar_evaluateRule(this, token, E_STRUCT_OR_UNION_SPECIFIER2);
    if (rules[E_STRUCT_OR_UNION_SPECIFIER2].isMatched)
    {
      rules[E_STRUCT_OR_UNION_SPECIFIER].isMatched = 1;
    }
    else if ((token->id == TOK_UNKNOWN) && ((uintptr_t)token->value == '}'))
    {
      rules[E_STRUCT_OR_UNION_SPECIFIER].isMatched = 1;
      rules[E_STRUCT_OR_UNION_SPECIFIER].count[0] = 0;
      this->isInStructDefinition = 0;
    }
  }
  else
  {
  switch (rules[E_STRUCT_OR_UNION_SPECIFIER].count[0])
  {
    case 0:
      if ((token->id == TOK_STRUCT) || (token->id == TOK_UNION))
      {
        rules[E_STRUCT_OR_UNION_SPECIFIER].count[0] = 1;
      }
      // else if ((token->id == TOK_UNKNOWN) && ((uintptr_t)token->value == '}') && (isInStructDefinition))
      // {
        // rules[E_STRUCT_OR_UNION_SPECIFIER].isMatched = 1;
        // rules[E_STRUCT_OR_UNION_SPECIFIER].count = 0;
        // isInStructDefinition = 0;
      // }
      break;
    case 1:
      if (token->id == TOK_IDENTIFIER)
      {
        rules[E_STRUCT_OR_UNION_SPECIFIER].isMatched = 1;
        rules[E_STRUCT_OR_UNION_SPECIFIER].count[0] = 2;
      }
      else
      {
        rules[E_STRUCT_OR_UNION_SPECIFIER].count[0] = 0;
      }
      break;
    case 2:
      if ((token->id == TOK_UNKNOWN) && ((uintptr_t)token->value == '{'))
      {
        rules[E_STRUCT_OR_UNION_SPECIFIER].count[0] = 3;
        this->isInStructDefinition = 1;
      }
      else
      {
        // Error: unexpected token
        rules[E_STRUCT_OR_UNION_SPECIFIER].count[0] = 0;
      }
      break;
    case 3:
      preventRecursion = 1;
      Grammar_evaluateRule(this, token, E_STRUCT_DECLARATION_LIST);
      preventRecursion = 0;
      if (rules[E_STRUCT_DECLARATION_LIST].isMatched)
      {
      }
      else if ((token->id == TOK_UNKNOWN) && ((uintptr_t)token->value == '}'))
      {
        rules[E_STRUCT_OR_UNION_SPECIFIER].isMatched = 1;
        rules[E_STRUCT_OR_UNION_SPECIFIER].count[0] = 0;
        this->isInStructDefinition = 0;
      }
      break;
  }
  }
}

/****************************************************************************
struct_or_union_specifier2
	: struct_or_union IDENTIFIER
  ;
****************************************************************************/
void Grammar_matchStructOrUnionSpecifier2(Grammar* this, Token* token)
{
  rules[E_STRUCT_OR_UNION_SPECIFIER2].isMatched = 0;

  switch (rules[E_STRUCT_OR_UNION_SPECIFIER2].count[0])
  {
    case 0:
      if ((token->id == TOK_STRUCT) || (token->id == TOK_UNION))
      {
        rules[E_STRUCT_OR_UNION_SPECIFIER2].count[0] = 1;
      }
      break;
    case 1:
      if ((token->id == TOK_IDENTIFIER))
      {
        rules[E_STRUCT_OR_UNION_SPECIFIER2].isMatched = 1;
        rules[E_STRUCT_OR_UNION_SPECIFIER2].count[0] = 0;
      }
      break;
  }
}

/****************************************************************************
struct_declaration_list
	: struct_declaration
	| struct_declaration_list struct_declaration
	;
****************************************************************************/
void Grammar_matchStructDeclarationList(Grammar* this, Token* token)
{
  rules[E_STRUCT_DECLARATION_LIST].isMatched = 0;

  Grammar_evaluateRule(this, token, E_STRUCT_DECLARATION);
  if (rules[E_STRUCT_DECLARATION].isMatched)
  {
    rules[E_STRUCT_DECLARATION_LIST].isMatched = 1;
  }
}

/****************************************************************************
struct_declaration
	: specifier_qualifier_list struct_declarator_list ';'
  ;
****************************************************************************/
void Grammar_matchStructDeclaration(Grammar* this, Token* token)
{
  rules[E_STRUCT_DECLARATION].isMatched = 0;

  switch (rules[E_STRUCT_DECLARATION].count[0])
  {
    case 0:
      Grammar_evaluateRule(this, token, E_SPECIFIER_QUALIFIER_LIST);
      Grammar_evaluateRule(this, token, E_STRUCT_DECLARATOR_LIST);
      if (rules[E_SPECIFIER_QUALIFIER_LIST].isMatched)
      {
      }
      else if (rules[E_STRUCT_DECLARATOR_LIST].isMatched)
      {
        rules[E_STRUCT_DECLARATION].count[0] = 1;
      }
      break;
    case 1:
      Grammar_evaluateRule(this, token, E_STRUCT_DECLARATOR_LIST);
      if (rules[E_STRUCT_DECLARATOR_LIST].isMatched)
      {
      }
      else if ((token->id == TOK_UNKNOWN) && ((uintptr_t)token->value == ';'))
      {
        rules[E_STRUCT_DECLARATION].isMatched = 1;
        rules[E_STRUCT_DECLARATION].count[0] = 0;
        this->requestReset = 1;
      }
      break;
  }
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
struct_declarator_list
	: struct_declarator
	| struct_declarator_list ',' struct_declarator
	;
****************************************************************************/
void Grammar_matchStructDeclaratorList(Grammar* this, Token* token)
{
  rules[E_STRUCT_DECLARATOR_LIST].isMatched = 0;

  switch (rules[E_STRUCT_DECLARATOR_LIST].count[0])
  {
    case 0:
      Grammar_evaluateRule(this, token, E_STRUCT_DECLARATOR);
      if (rules[E_STRUCT_DECLARATOR].isMatched)
      {
        rules[E_STRUCT_DECLARATOR_LIST].isMatched = 1;
      }
      break;
  }
}

/****************************************************************************
struct_declarator
	: declarator
	| ':' constant_expression
	| declarator ':' constant_expression
	;
****************************************************************************/
void Grammar_matchStructDeclarator(Grammar* this, Token* token)
{
  rules[E_STRUCT_DECLARATOR].isMatched = 0;

  switch (rules[E_STRUCT_DECLARATOR].count[0])
  {
    case 0:
      Grammar_evaluateRule(this, token, E_DECLARATOR);
      if (rules[E_DECLARATOR].isMatched)
      {
        rules[E_STRUCT_DECLARATOR].isMatched = 1;
      }
      else if ((token->id == TOK_UNKNOWN) && ((uintptr_t)token->value == ':'))
      {
        rules[E_STRUCT_DECLARATOR].count[0] = 1;
      }
      break;
    case 1:
      Grammar_evaluateRule(this, token, E_CONSTANT_EXPRESSION);
      if (rules[E_CONSTANT_EXPRESSION].isMatched)
      {
        rules[E_STRUCT_DECLARATOR].isMatched = 1;
        rules[E_STRUCT_DECLARATOR].count[0] = 0;
      }
  }
}

/****************************************************************************
enum_specifier
	: ENUM '{' enumerator_list '}'
	| ENUM IDENTIFIER '{' enumerator_list '}'
	| ENUM IDENTIFIER
	;
****************************************************************************/
void Grammar_matchEnumSpecifier(Grammar* this, Token* token)
{
  rules[E_ENUM_SPECIFIER].isMatched = 0;

  switch (rules[E_ENUM_SPECIFIER].count[0])
  {
    case 0:
      if (token->id == TOK_ENUM)
      {
        rules[E_ENUM_SPECIFIER].count[0] = 1;
      }
      break;
    case 1:
      if (token->id == TOK_IDENTIFIER)
      {
        rules[E_ENUM_SPECIFIER].count[0] = 2;
        rules[E_ENUM_SPECIFIER].isMatched = 1;
      }
      else if  ((token->id == TOK_UNKNOWN) && ((uintptr_t)token->value == '{'))
      {
        rules[E_ENUM_SPECIFIER].count[0] = 2;
      }
    case 2:
      if ((token->id == TOK_UNKNOWN) && ((uintptr_t)token->value == '{'))
      {
        rules[E_ENUM_SPECIFIER].count[0] = 3;
      }
      break;
    case 3:
      Grammar_evaluateRule(this, token, E_ENUMERATOR_LIST);
      if (rules[E_ENUMERATOR_LIST].isMatched)
      {
        rules[E_ENUM_SPECIFIER].count[0] = 4;
      }
      break;
    case 4:
      if ((token->id == TOK_UNKNOWN) && ((uintptr_t)token->value == '}'))
      {
        rules[E_ENUM_SPECIFIER].isMatched = 1;
      }
      break;
  }
}

/****************************************************************************
enumerator_list
	: enumerator
	| enumerator_list ',' enumerator
	;
****************************************************************************/
void Grammar_matchEnumeratorList(Grammar* this, Token* token)
{
  rules[E_ENUMERATOR_LIST].isMatched = 0;

  switch (rules[E_ENUMERATOR_LIST].count[0])
  {
    case 0:
      Grammar_evaluateRule(this, token, E_ENUMERATOR);
      if (rules[E_ENUMERATOR_LIST].isMatched)
      {
        rules[E_ENUMERATOR_LIST].isMatched = 1;
      }
      break;
    case 1:
      if ((token->id == TOK_UNKNOWN) && ((uintptr_t)token->value == ','))
      {
      }
      break;
  }
}

/****************************************************************************
enumerator
	: IDENTIFIER
	| IDENTIFIER '=' constant_expression
	;
****************************************************************************/
void Grammar_matchEnumerator(Grammar* this, Token* token)
{
  rules[E_ENUMERATOR].isMatched = 0;

  switch (rules[E_ENUMERATOR].count[0])
  {
    case 0:
      if (token->id == TOK_IDENTIFIER)
      {
        rules[E_ENUMERATOR].isMatched = 1;
      }
      else if ((token->id == TOK_UNKNOWN) && ((uintptr_t)token->value == '='))
      {
        rules[E_ENUMERATOR].count[0] = 1;
      }
      break;
    case 1:
      Grammar_evaluateRule(this, token, E_CONSTANT_EXPRESSION);
      if (rules[E_CONSTANT_EXPRESSION].isMatched)
      {
        rules[E_ENUMERATOR].isMatched = 1;
      }
      break;
  }
}

/****************************************************************************
constant_expression
	: conditional_expression
	;
****************************************************************************/
void Grammar_matchConstantExpression(Grammar* this, Token* token)
{
  rules[E_CONSTANT_EXPRESSION].isMatched = 0;
  Grammar_evaluateRule(this, token, E_CONDITIONAL_EXPRESSION);
  if (rules[E_CONDITIONAL_EXPRESSION].isMatched)
  {
    rules[E_CONSTANT_EXPRESSION].isMatched = 1;
  }
}

/****************************************************************************
initializer
	: assignment_expression
	| '{' initializer_list '}'
	| '{' initializer_list ',' '}'
	;
****************************************************************************/
void Grammar_matchInitializer(Grammar* this, Token* token)
{
  rules[E_INITIALIZER].isMatched = 0;
  
  switch (rules[E_INITIALIZER].count[0])
  {
    case 0:
      Grammar_evaluateRule(this, token, E_ASSIGNMENT_EXPRESSION);
      if (rules[E_ASSIGNMENT_EXPRESSION].isMatched)
      {
        rules[E_INITIALIZER].isMatched = 1;
      }
  }
}

/****************************************************************************
assignment_expression
	: conditional_expression
	| unary_expression assignment_operator assignment_expression
****************************************************************************/
void Grammar_matchAssignmentExpression(Grammar* this, Token* token)
{
  rules[E_ASSIGNMENT_EXPRESSION].isMatched = 0;
  Grammar_evaluateRule(this, token, E_CONDITIONAL_EXPRESSION);
  if (rules[E_CONDITIONAL_EXPRESSION].isMatched)
  {
    rules[E_ASSIGNMENT_EXPRESSION].isMatched = 1;
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

  switch (rules[E_PARAMETER_TYPE_LIST].count[0])
  {
    case 0:
      Grammar_evaluateRule(this, token, E_PARAMETER_LIST);
      if (rules[E_PARAMETER_LIST].isMatched)
      {
        rules[E_PARAMETER_TYPE_LIST].count[0] = 1;
      }
      break;
    case 1:
      if ((token->id == TOK_UNKNOWN) && ((uintptr_t)token->value == ','))
      {
        rules[E_PARAMETER_TYPE_LIST].count[0] = 2;
      }
      else
      {
        rules[E_PARAMETER_TYPE_LIST].count[0] = 0;
      }
      break;
    case 2:
      printf("Unsupported\n");
      break;
  }
}

/****************************************************************************
	: parameter_declaration
	| parameter_list ',' parameter_declaration
****************************************************************************/
void Grammar_matchParameterList(Grammar* this, Token* token)
{
  rules[E_PARAMETER_LIST].isMatched = 0;

  switch (rules[E_PARAMETER_LIST].count[0])
  {
    case 0:
      Grammar_evaluateRule(this, token, E_PARAMETER_DECLARATION);
      if (rules[E_PARAMETER_DECLARATION].isMatched)
      {
        rules[E_PARAMETER_LIST].isMatched = 1;
        rules[E_PARAMETER_LIST].count[0] = 1;
      }
      break;
    case 1:
      if ((token->id == TOK_UNKNOWN) && ((uintptr_t)token->value == ','))
      {
        rules[E_PARAMETER_LIST].count[0] = 0;
      }
      else
      {
        // Error?
        rules[E_PARAMETER_LIST].count[0] = 0;
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

  switch (rules[E_PARAMETER_DECLARATION].count[0])
  {
    case 0:
      Grammar_evaluateRule(this, token, E_DECLARATION_SPECIFIERS);
      if (rules[E_DECLARATION_SPECIFIERS].isMatched)
      {
        rules[E_PARAMETER_LIST].count[0] = 1;
        rules[E_PARAMETER_DECLARATION].isMatched = 1;
      }
      break;
    case 1:
      Grammar_evaluateRule(this, token, E_DECLARATOR);
      Grammar_evaluateRule(this, token, E_ABSTRACT_DECLARATOR);
      if (rules[E_DECLARATOR].isMatched)
      {
        rules[E_PARAMETER_DECLARATION].isMatched = 1;
      }
      else if (rules[E_ABSTRACT_DECLARATOR].isMatched)
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
constant_expression
	: conditional_expression
	;
****************************************************************************/
void Grammar_matchConstantExpresion(Grammar* this, Token* token)
{
    rules[E_CONSTANT_EXPRESSION].isMatched = 0;
}

/****************************************************************************
conditional_expression
	: logical_or_expression
	| logical_or_expression '?' expression ':' conditional_expression
	;
****************************************************************************/
void Grammar_matchConditionalExpression(Grammar* this, Token* token)
{
    rules[E_CONDITIONAL_EXPRESSION].isMatched = 0;
    Grammar_evaluateRule(this, token, E_CAST_EXPRESSION);
    if (rules[E_CAST_EXPRESSION].isMatched)
    {
      rules[E_CONDITIONAL_EXPRESSION].isMatched = 1;
    }
}

/****************************************************************************
primary_expression
	: IDENTIFIER
	| CONSTANT
	| STRING_LITERAL
	| '(' expression ')'
	;
****************************************************************************/
void Grammar_matchPrimaryExpression(Grammar* this, Token* token)
{
  rules[E_PRIMARY_EXPRESSION].isMatched = 0;

  switch (rules[E_PRIMARY_EXPRESSION].count[0])
  {
    case 0:
      if (token->id == TOK_IDENTIFIER)
      {
        rules[E_PRIMARY_EXPRESSION].isMatched = 1;
      }
      else if (token->id == TOK_CONSTANT)
      {
        rules[E_PRIMARY_EXPRESSION].isMatched = 1;
      }
      break;
  }
}

/****************************************************************************
postfix_expression
	: primary_expression
	| postfix_expression '[' expression ']'
	| postfix_expression '(' ')'
	| postfix_expression '(' argument_expression_list ')'
	| postfix_expression '.' IDENTIFIER
	| postfix_expression PTR_OP IDENTIFIER
	| postfix_expression INC_OP
	| postfix_expression DEC_OP
****************************************************************************/
void Grammar_matchPostfixExpression(Grammar* this, Token* token)
{
  rules[E_POSTFIX_EXPRESSION].isMatched = 0;

  switch (rules[E_POSTFIX_EXPRESSION].count[0])
  {
    case 0:
      Grammar_evaluateRule(this, token, E_PRIMARY_EXPRESSION);
      if (rules[E_PRIMARY_EXPRESSION].isMatched)
      {
        rules[E_POSTFIX_EXPRESSION].isMatched = 1;
        rules[E_POSTFIX_EXPRESSION].count[0] = 1;
      }
      break;
    case 1:
      break;
  }
}

/****************************************************************************
unary_expression
	: postfix_expression
	| INC_OP unary_expression
	| DEC_OP unary_expression
	| unary_operator cast_expression
	| SIZEOF unary_expression
	| SIZEOF '(' type_name ')'
	;
****************************************************************************/
void Grammar_matchUnaryExpression(Grammar* this, Token* token)
{
    rules[E_UNARY_EXPRESSION].isMatched = 0;
    Grammar_evaluateRule(this, token, E_POSTFIX_EXPRESSION);
    if (rules[E_POSTFIX_EXPRESSION].isMatched)
    {
      rules[E_UNARY_EXPRESSION].isMatched = 1;
    }
}

/****************************************************************************
cast_expression
	: unary_expression
	| '(' type_name ')' cast_expression
****************************************************************************/
void Grammar_matchCastExpression(Grammar* this, Token* token)
{
    rules[E_CAST_EXPRESSION].isMatched = 0;
    
    switch (rules[E_CAST_EXPRESSION].count[0])
    {
      case 0:
        Grammar_evaluateRule(this, token, E_UNARY_EXPRESSION);
        if ((token->id == TOK_UNKNOWN) && ((uintptr_t)token->value == '('))
        {
          rules[E_CAST_EXPRESSION].count[0] = 1;
        }
        else if (rules[E_UNARY_EXPRESSION].isMatched)
        {
          rules[E_CAST_EXPRESSION].isMatched = 1;
        }
        break;
      case 1:
        Grammar_evaluateRule(this, token, E_TYPE_NAME);
        break;
    }
}



/****************************************************************************
expression
	: assignment_expression
	| expression ',' assignment_expression
	;
****************************************************************************/
void Grammar_matchExpression(Grammar* this, Token* token)
{
    rules[E_EXPRESSION].isMatched = 0;
    if (token->id == TOK_IDENTIFIER)
    {
      rules[E_EXPRESSION].isMatched = 1;
      printf("Matched expression\n");
    }
}

/****************************************************************************
type_name
	: specifier_qualifier_list
	| specifier_qualifier_list abstract_declarator
	;
****************************************************************************/
void Grammar_matchTypeName(Grammar* this, Token* token)
{
    rules[E_TYPE_NAME].isMatched = 0;
    Grammar_evaluateRule(this, token, E_SPECIFIER_QUALIFIER_LIST);
    if (rules[E_SPECIFIER_QUALIFIER_LIST].isMatched)
    {
      rules[E_TYPE_NAME].isMatched = 1;
    }
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
  char *classText[] = {"Unknown","Function","Variable","Type"};
  char *scopeText[] = {"Unknown","Local","Global"};

  memset(name, 0, 255);
  memset(cmd, 0, 255);
  memcpy(name, declarator->name->buffer, declarator->name->length);

  sprintf(cmd, "INSERT INTO Declarations ( name, type, scope ) "
               "VALUES ('%s','%s','%s');", name, classText[declarator->class], scopeText[this->scope]);

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
  sprintf(cmd,"SELECT * FROM Declarations WHERE name LIKE '%s';", name);

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
