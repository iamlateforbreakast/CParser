/* Grammar.c */

#include "Grammar.h"

#include "Common.h"
#include "Token.h"
#include "List.h"

#include <stdint.h>

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
  E_STRUCT_DECLARATION_LIST,
  E_STRUCT_DECLARATION,
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
  E_JUMP_STATEMENT
} RuleName;

typedef enum{
  E_UNKNOWN_DECLARATOR = 0,
  E_FUNCTION_DECLARATOR,
  E_VARIABLE_DECLARATOR,
  E_TYPE_DECLARATOR
} DeclaratorClass;

typedef enum
{
  E_LOCAL_SCOPE = 1,
  E_GLOBAL_SCOPE = 2
} Scope;
  
typedef void (*EvalFunction)(Grammar* this, Token* token);

typedef struct MatchRule
{
  RuleName name;
  unsigned int isMatched;
  unsigned int isEvaluated;
  EvalFunction evaluate;
  unsigned int count;
} MatchRule;

typedef struct Declarator Declarator;

struct Declarator
{
  DeclaratorClass class;
  Declarator* type;
  //DeclaratorQualifier qualifier;
  String* name;
  List* args;
};

struct Grammar{
  unsigned int directDeclaratorCnt;
  unsigned int declaratorCnt;
  unsigned int initDeclaratorListCnt;
  unsigned int declarationSpecifiersCnt;
  unsigned int declarationCnt;
  unsigned int functionDeclarationCnt;
  unsigned int compountStatementCnt;
  unsigned int structOrUnionCnt;
  unsigned int structDeclarationListCnt;
  unsigned int evaluatedDeclarationSpecifiers;
  unsigned int resultDeclarationSpecifiers;
  unsigned int evaluatedDeclarator;
  unsigned int resultDeclarator;
  Scope scope;
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
void Grammar_matchStructDeclarationList(Grammar* this, Token* token);
void Grammar_matchEnumSpecifier(Grammar* this, Token* token);
void Grammar_matchEnumerator(Grammar* this, Token* token);
void Grammar_matchEnumeratorList(Grammar* this, Token* token);
void Grammar_matchStructDelaration(Grammar* this, Token* token);
void Grammar_evaluateRule(Grammar* this, Token* token, RuleName r);
void Grammar_matchInitializer(Grammar* this, Token* token);
void Grammar_matchParameterTypeList(Grammar* this, Token* token);
void Grammar_matchIdentifierList(Grammar* this, Token* token);
void Grammar_matchParameterList(Grammar* this, Token* token);
void Grammar_matchAbstractDeclarator(Grammar* this, Token* token);
void Grammar_matchDirectAbstractDeclarator(Grammar* this, Token* token);
void Grammar_matchDeclarationList(Grammar* this, Token* token);
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
void Grammar_printDeclarator(Grammar* this);
void Grammar_reset(Grammar* this);

MatchRule rules[] = { { E_EXTERNAL_DECLARATION , 0 , 0, &Grammar_matchExternalDeclaration, 0 },
                      { E_FUNCTION_DECLARATION , 0 , 0, &Grammar_matchFunctionDeclaration, 0 },
                      { E_DECLARATION , 0 , 0, &Grammar_matchDeclaration, 0 },
                      { E_COMPOUND_STATEMENT , 0 , 0, &Grammar_matchCompountStatement, 0 },
                      { E_DECLARATION_SPECIFIERS , 0 , 0, &Grammar_matchDeclarationSpecifiers, 0 },
                      { E_DECLARATOR , 0 , 0, &Grammar_matchDeclarator, 0 },
                      { E_DIRECT_DECLARATOR , 0 , 0, &Grammar_matchDirectDeclarator, 0 },
                      { E_DECLARATOR_LIST , 0 , 0, &Grammar_matchDeclaratorList, 0 },
                      { E_INIT_DECLARATOR_LIST , 0 , 0, &Grammar_matchInitDeclaratorList, 0 },
                      { E_INIT_DECLARATOR , 0 , 0, &Grammar_matchInitDeclarator, 0 },
                      { E_STORAGE_CLASS , 0 , 0, &Grammar_matchStorageClass, 0 },
                      { E_TYPE_SPECIFIER , 0 , 0, &Grammar_matchTypeSpecifier, 0 },
                      { E_TYPE_QUALIFIER , 0 , 0, &Grammar_matchTypeQualifier, 0 },
                      { E_TYPE_QUALIFIER_LIST , 0 , 0, &Grammar_matchTypeQualifierList, 0 },
                      { E_STRUCT_OR_UNION_SPECIFIER , 0 , 0, &Grammar_matchStructOrUnionSpecifier, 0 },
                      { E_STRUCT_DECLARATION_LIST , 0 , 0, &Grammar_matchStructDeclarationList, 0 },
                      { E_STRUCT_DECLARATION , 0 , 0, &Grammar_matchStructDelaration, 0 },
                      { E_ENUMERATOR , 0 , 0, &Grammar_matchEnumerator, 0 },
                      { E_ENUMERATOR_LIST , 0 , 0, &Grammar_matchEnumeratorList, 0 },
                      { E_ENUM_SPECIFIER , 0 , 0, &Grammar_matchEnumSpecifier, 0 },
                      { E_POINTER , 0 , 0, &Grammar_matchPointer, 0 },
                      { E_CONSTANT_EXPRESSION , 0 , 0, &Grammar_matchConstantExpresion, 0 },
                      { E_INITIALIZER, 0, 0, &Grammar_matchInitializer, 0 },
                      { E_PARAMETER_TYPE_LIST, 0, 0, &Grammar_matchParameterTypeList, 0 },
                      { E_IDENTIFIER_LIST, 0, 0, &Grammar_matchIdentifierList, 0 },
                      { E_PARAMETER_LIST, 0, 0, &Grammar_matchParameterList, 0 },
                      { E_ABSTRACT_DECLARATOR, 0, 0, &Grammar_matchAbstractDeclarator, 0 },
                      { E_DIRECT_ABSTRACT_DECLARATOR, 0, 0, &Grammar_matchDirectAbstractDeclarator, 0 },
                      { E_DECLARATION_LIST, 0, 0, &Grammar_matchDeclarationList, 0 },
                      { E_PARAMETER_DECLARATION, 0, 0, &Grammar_matchParameterDeclaration, 0 },
		                  { E_CONDITIONAL_EXPRESSION, 0, 0, &Grammar_matchConditionalExpression, 0 },
                      { E_STATEMENT_LIST, 0, 0, &Grammar_matchStatementList, 0 },
                      { E_STATEMENT, 0, 0, &Grammar_matchStatement, 0 },
                      { E_LABELED_STATEMENT, 0, 0, &Grammar_matchLabeledStatement, 0 },
                      { E_EXPRESSION_STATEMENT, 0, 0, &Grammar_matchExpresionStatement, 0 },
                      { E_SELECTION_STATEMENT, 0, 0, &Grammar_matchSelectionStatement, 0 },
                      { E_ITERATION_STATEMENT, 0, 0, &Grammar_matchIterationStatement, 0 },
                      { E_JUMP_STATEMENT, 0, 0, &Grammar_matchJumpStatement, 0 },
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
  Grammar_reset(this);

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
  unsigned int i = 0;
  
  printf("Grammar Reset\n");
  for (i=0; i<nbRules; i++)
  {
    rules[i].isMatched = 0;
    rules[i].isEvaluated = 0;
    rules[i].count = 0;
  }
  this->declarator.name = NULL;
  this->declarator.class = E_UNKNOWN_DECLARATOR;
  this->scope = E_GLOBAL_SCOPE;
}

/****************************************************************************
****************************************************************************/
void Grammar_pushToken(Grammar* this, Token* token)
{
  unsigned int nbRules = sizeof(rules)/sizeof(MatchRule);
  unsigned int i = 0;
  
  Grammar_matchExternalDeclaration(this, token);
  
  /* if rules[E_EXTERNAL-DECLARATION].isMatched = 1
  {
    Grammar_reset();
  } */
  
  if (rules[E_EXTERNAL_DECLARATION].isMatched)
  {
    Grammar_printDeclarator(this);
    Grammar_reset(this);
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
  
  if (rules[E_DECLARATION].isMatched) 
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

  switch(rules[E_FUNCTION_DECLARATION].count)
  {
    case 0:
      Grammar_evaluateRule(this, token, E_DECLARATION_SPECIFIERS);
      Grammar_evaluateRule(this, token, E_DECLARATOR);
      if (rules[E_DECLARATION_SPECIFIERS].isMatched)
      {
      }
      else if ((rules[E_DECLARATOR].isMatched))
      {
        rules[E_FUNCTION_DECLARATION].count = 1;
      }
      else
      {
        // Syntax Error
      }
      break;
    case 1:
      Grammar_evaluateRule(this, token, E_DECLARATOR);
      Grammar_evaluateRule(this, token, E_COMPOUND_STATEMENT);
      if (rules[E_DECLARATOR].isMatched)
      {
        // Consume
      }
      else if (rules[E_COMPOUND_STATEMENT].isMatched)
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
  
  switch(rules[E_DECLARATION].count)
  {
    case 0:
      Grammar_evaluateRule(this, token, E_DECLARATION_SPECIFIERS);
      if (rules[E_DECLARATION_SPECIFIERS].isMatched)
      {
        rules[E_DECLARATION].count = 1 ;
      }
      else
      {
      }
      break;
    case 1:
      Grammar_evaluateRule(this, token, E_INIT_DECLARATOR_LIST);
      if (rules[E_INIT_DECLARATOR_LIST].isMatched)
      {
        // COnsume
      }
      else if ((token->id == TOK_UNKNOWN) && ((uintptr_t)token->value == ';'))
      {
        rules[E_DECLARATION].isMatched = 1;
        rules[E_DECLARATION].count = 0 ;
      }
      else if ((token->id == TOK_UNKNOWN) && ((uintptr_t)token->value == '{'))
      {
        rules[E_DECLARATION].count = 0 ;
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

  switch (rules[E_INIT_DECLARATOR_LIST].count)
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
    
    switch (rules[E_INIT_DECLARATOR].count)
    {
      case 0:
        Grammar_evaluateRule(this, token, E_DECLARATOR);
        if (rules[E_DECLARATOR].isMatched)
        {
          rules[E_INIT_DECLARATOR].isMatched = 1;
          rules[E_INIT_DECLARATOR].count = 1;
        }
        break;
      case 1:
        if ((token->id == TOK_UNKNOWN) && ((uintptr_t)token->value == '='))
        {
          rules[E_INIT_DECLARATOR].count = 2;
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
  
  switch (rules[E_DECLARATOR].count)
  {
    case 0:
      Grammar_evaluateRule(this, token, E_POINTER);
      Grammar_evaluateRule(this, token, E_DIRECT_DECLARATOR);
      if (rules[E_POINTER].isMatched)
      {
        rules[E_DECLARATOR].count = 1;
      }
      else if (rules[E_DIRECT_DECLARATOR].isMatched)
      {
          rules[E_DECLARATOR].isMatched = 1;
      }
      else
      {
        // Syntax Error
      }
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
  
  switch (rules[E_DIRECT_DECLARATOR].count)
  {
    case 0:
      if (token->id == TOK_IDENTIFIER)
      {
        rules[E_DIRECT_DECLARATOR].isMatched = 1;
        rules[E_DIRECT_DECLARATOR].count = 1;
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
        rules[E_DIRECT_DECLARATOR].count = 2;
      }
      else if ((token->id == TOK_UNKNOWN) && ((uintptr_t)token->value == '('))
      {
        rules[E_DIRECT_DECLARATOR].count = 3;
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
        rules[E_DIRECT_DECLARATOR].count = 4;
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
        rules[E_DIRECT_DECLARATOR].count = 5;
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
  if ((token->id == TOK_VOID) || (token->id == TOK_CHAR) ||
      (token->id == TOK_SHORT) || (token->id == TOK_INT) ||
      (token->id == TOK_LONG) || (token->id == TOK_FLOAT) ||
      (token->id == TOK_DOUBLE) || (token->id == TOK_SIGNED) ||
      (token->id == TOK_UNSIGNED))
  {
    rules[E_TYPE_SPECIFIER].isMatched = 1;
  }
  else if (token->id == TOK_IDENTIFIER)
  {
    rules[E_TYPE_SPECIFIER].isMatched = 1;
  }
  else if (rules[E_STRUCT_OR_UNION_SPECIFIER].isMatched)
  {
    rules[E_TYPE_SPECIFIER].isMatched = 1;
    printf("THis is a struct here\n");
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

  rules[E_TYPE_QUALIFIER].isEvaluated = 1;
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

  switch(rules[E_POINTER].count)
  {
    case 0:
      Grammar_evaluateRule(this, token, E_TYPE_QUALIFIER_LIST);
      if (rules[E_TYPE_QUALIFIER_LIST].isMatched)
      {
        rules[E_POINTER].count = 1;
      }
      if ((token->id == TOK_UNKNOWN) && ((uintptr_t)token->value == '*'))
      {
        
      }
      //Grammar_evaluateRule(this, token, E_DECLARATION_SPECIFIER);
      //Grammar_evaluateRule(this, token, E_DECLARATOR_LIST);
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
  
  switch(rules[E_COMPOUND_STATEMENT].count)
  {
    case 0:
      if ((token->id == TOK_UNKNOWN) && ((uintptr_t)token->value == '{'))
      {
        rules[E_COMPOUND_STATEMENT].count = 1;
        this->scope = E_LOCAL_SCOPE;
        printf("Start compound statement\n");

      }
      break;
    case 1:
      Grammar_evaluateRule(this, token, E_STATEMENT_LIST);
      Grammar_evaluateRule(this, token, E_DECLARATION_LIST);
      if ((token->id == TOK_UNKNOWN) && ((uintptr_t)token->value == '}'))
      {
        rules[E_COMPOUND_STATEMENT].isMatched  = 1;
        this->scope = E_GLOBAL_SCOPE;
        printf("End compound statement\n");
      }
      else if (rules[E_STATEMENT_LIST].isMatched)
      {
      }
      else if (rules[E_DECLARATION_LIST].isMatched)
      {
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
    printf("Declaration_list: Matched local declaration\n");
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
  
  switch (rules[E_STRUCT_OR_UNION_SPECIFIER].count)
  {
    case 0:
      if ((token->id == TOK_STRUCT) || (token->id == TOK_UNION))
      {
        rules[E_STRUCT_OR_UNION_SPECIFIER].count = 1;
      }
      break;
    case 1:
      if ((token->id == TOK_IDENTIFIER))
      {
        rules[E_STRUCT_OR_UNION_SPECIFIER].isMatched = 1;
      }
      else if ((token->id == TOK_UNKNOWN) && ((uintptr_t)token->value == '{'))
      {
        rules[E_STRUCT_OR_UNION_SPECIFIER].count = 3;
      }
      break;
    case 2:
      if ((token->id == TOK_UNKNOWN) && ((uintptr_t)token->value == '{'))
      {
        rules[E_STRUCT_OR_UNION_SPECIFIER].count = 3;
      }
      break;
    case 3:
      Grammar_evaluateRule(this, token, E_STRUCT_DECLARATION_LIST);
      if (rules[E_STRUCT_DECLARATION_LIST].isMatched)
      {
        rules[E_STRUCT_OR_UNION_SPECIFIER].count = 4;
      }
      break;
    case 4:
      if ((token->id == TOK_UNKNOWN) && ((uintptr_t)token->value == '}'))
      {
        rules[E_STRUCT_OR_UNION_SPECIFIER].isMatched = 1;
      }
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
  
  switch (rules[E_STRUCT_DECLARATION_LIST].count)
  {
    case 0:
      break;
    case 1:
      break;
    case 2:
      break;
  }
}

/****************************************************************************
struct_declaration
	: specifier_qualifier_list struct_declarator_list ';'
  ;
****************************************************************************/
void Grammar_matchStructDelaration(Grammar* this, Token* token)
{
  rules[E_STRUCT_DECLARATION].isMatched = 0;
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
  
  switch (rules[E_ENUM_SPECIFIER].count)
  {
    case 0:
      if (token->id == TOK_ENUM)
      {
        rules[E_ENUM_SPECIFIER].count = 1;
      }
      break;
    case 1: 
      if (token->id == TOK_IDENTIFIER)
      {
        rules[E_ENUM_SPECIFIER].count = 2;
        rules[E_ENUM_SPECIFIER].isMatched = 1;
      } 
      else if  ((token->id == TOK_UNKNOWN) && ((uintptr_t)token->value == '{'))
      {
        rules[E_ENUM_SPECIFIER].count = 2;
      }
    case 2:
      if ((token->id == TOK_UNKNOWN) && ((uintptr_t)token->value == '{'))
      {
        rules[E_ENUM_SPECIFIER].count = 3;
      }
      break;
    case 3:
      Grammar_evaluateRule(this, token, E_ENUMERATOR_LIST);
      if (rules[E_ENUMERATOR_LIST].isMatched)
      {
        rules[E_ENUM_SPECIFIER].count = 4;
      }
      break;
    case 4:
      if ((token->id == TOK_UNKNOWN) && ((uintptr_t)token->value == '}'))
      {
        rules[E_ENUM_SPECIFIER].isMatched = 1;
      }
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
  
  switch (rules[E_ENUMERATOR_LIST].count)
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
  
  switch (rules[E_ENUMERATOR].count)
  {
    case 0:
      if (token->id == TOK_IDENTIFIER)
      {
        rules[E_ENUMERATOR].isMatched = 1;
      }
      break;
    case 1:
      if ((token->id == TOK_UNKNOWN) && ((uintptr_t)token->value == '='))
      {
      }
      break;
    case 2:
      //if ((token->id == TOK_!!!)
      {
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
  
  switch (rules[E_PARAMETER_TYPE_LIST].count)
  {
    case 0:
      Grammar_evaluateRule(this, token, E_PARAMETER_LIST);
      if (rules[E_PARAMETER_LIST].isMatched)
      {
        rules[E_PARAMETER_TYPE_LIST].count = 1;
      }
      break;
    case 1:
      if ((token->id == TOK_UNKNOWN) && ((uintptr_t)token->value == ','))
      {
        rules[E_PARAMETER_TYPE_LIST].count = 2;
      }
      else
      {
        rules[E_PARAMETER_TYPE_LIST].count = 0;
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
  
  switch (rules[E_PARAMETER_LIST].count)
  {
    case 0:
      Grammar_evaluateRule(this, token, E_PARAMETER_DECLARATION);
      if (rules[E_PARAMETER_DECLARATION].isMatched)
      {
        rules[E_PARAMETER_LIST].isMatched = 1;
        rules[E_PARAMETER_LIST].count = 1;
      }
      break;
    case 1:
      if ((token->id == TOK_UNKNOWN) && ((uintptr_t)token->value == ','))
      {
        rules[E_PARAMETER_LIST].count = 0;
      }
      else
      {
        // Error?
        rules[E_PARAMETER_LIST].count = 0;
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
  
  switch (rules[E_PARAMETER_DECLARATION].count)
  {
    case 0:
      Grammar_evaluateRule(this, token, E_DECLARATION_SPECIFIERS);
      if (rules[E_DECLARATION_SPECIFIERS].isMatched)
      {
        rules[E_PARAMETER_LIST].count = 1;
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
      }
      String_delete(this->declarator.name);
      break;
    case E_FUNCTION_DECLARATOR:
      if (this->scope == E_GLOBAL_SCOPE) 
      {
        String_print(this->declarator.name,"Global function declaration found: ");
      }
      String_delete(this->declarator.name);
      break;
    case E_VARIABLE_DECLARATOR:
      if (this->scope == E_GLOBAL_SCOPE) 
      {
        String_print(this->declarator.name,"Global variable declaration found: ");
      }
      String_delete(this->declarator.name);
      break;
    case E_UNKNOWN_DECLARATOR:
      String_print(this->declarator.name,"Unkown declaration found: ");
      String_delete(this->declarator.name);
      break;
  }
}