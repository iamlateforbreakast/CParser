/* Grammar.c */

#include "Grammar.h"

#include "Common.h"
#include "Token.h"

#include <stdint.h>

unsigned int Grammar_matchExternalDeclaration(Grammar* this, Token* token);
unsigned int Grammar_matchFunctionDeclaration(Grammar* this, Token* token);
unsigned int Grammar_matchDeclaration(Grammar* this, Token* token);
unsigned int Grammar_matchDeclarationSpecifiers(Grammar* this, Token* token);
unsigned int Grammar_matchStorageClass(Grammar* this, Token* token);
unsigned int Grammar_matchInitDeclaratorList(Grammar* this, Token* token);
unsigned int Grammar_matchDeclarator(Grammar* this, Token* token);
unsigned int Grammar_matchDirectDeclarator(Grammar* this, Token* token);
unsigned int Grammar_matchTypeSpecifier(Grammar* this, Token* token);
unsigned int Grammar_matchPointer(Grammar* this, Token* token);
unsigned int Grammar_matchConstantExpression(Grammar* this, Token* token);
unsigned int Grammar_matchTypeQualifier(Grammar* this, Token* token);
unsigned int Grammar_matchCompountStatement(Grammar* this, Token* token);
unsigned int Grammar_matchStructOrUnion(Grammar* this, Token* token);
unsigned int Grammar_matchStructDeclarationList(Grammar* this, Token* token);
void Grammar_reset(Grammar* this);

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
  this->directDeclaratorCnt = 0;
  this->declaratorCnt = 0;
  this->initDeclaratorListCnt = 0;
  this->declarationSpecifiersCnt = 0;
  this->declarationCnt = 0;
  this->functionDeclarationCnt = 0;
  this->compountStatementCnt = 0;
}

/****************************************************************************
****************************************************************************/
void Grammar_pushToken(Grammar* this, Token* token)
{
  this->resultDeclarationSpecifiers = 0;
  this->resultDeclarator = 0;
  this->evaluatedDeclarationSpecifiers = 0;
  this->evaluatedDeclarator = 0;
  if (Grammar_matchExternalDeclaration(this, token))
  {
    // Reset al internal states?
  }
}

/**************************************************
external_declaration
	: function_definition
	| declaration
**************************************************/
unsigned int Grammar_matchExternalDeclaration(Grammar* this, Token* token)
{
  unsigned int result = 0;

  if (Grammar_matchDeclaration(this, token))
  {
  } else if (Grammar_matchFunctionDeclaration(this, token))
  {
  }

  return result;
}

/**************************************************
function_definition
	: declaration_specifiers declarator declaration_list compound_statement
	| declaration_specifiers declarator compound_statement
	| declarator declaration_list compound_statement
	| declarator compound_statement
	;
**************************************************/
unsigned int Grammar_matchFunctionDeclaration(Grammar* this, Token* token)
{
  unsigned int result = 0;

  switch(this->functionDeclarationCnt)
  {
    case 0:
      if (Grammar_matchDeclarationSpecifiers(this, token))
      {
      }
      else if (Grammar_matchDeclarator(this, token))
      {
        this->functionDeclarationCnt  = 1;
      }
      else
      {
        // Syntax Error
      }
      break;
    case 1:
      if (Grammar_matchDeclarator(this, token))
      {
        // Consume
      }
      else if (Grammar_matchCompountStatement(this, token))
      {
        String_print(this->declarator.name,"Func. Declaration: ");
        String_delete(this->declarator.name);
        this->declarator.name = NULL;
        Grammar_reset(this);
        result = 1;
      }
      else
      {
        // Keep looking for '}'
      }
      break;
  }
  return result;
}

/**************************************************
declaration
	: declaration_specifiers ';'
	| declaration_specifiers init_declarator_list ';'
	;
****************************************************************************/
unsigned int Grammar_matchDeclaration(Grammar* this, Token* token)
{
  unsigned int result = 0;

  switch(this->declarationCnt)
  {
    case 0:
      if (Grammar_matchDeclarationSpecifiers(this, token))
      {
        this->declarationCnt = 1 ;
      }
      else if (Grammar_matchInitDeclaratorList(this, token))
      {
        this->declarationCnt = 2 ;
      }
      else
      {
        // Syntax Error
      }
      break;
    case 1:
      if (Grammar_matchDeclarationSpecifiers(this, token))
      {
      }
      else if (Grammar_matchInitDeclaratorList(this, token))
      {
        this->declarationCnt = 2 ;
      }
      else if ((token->id == TOK_UNKNOWN) && ((uintptr_t)token->value == ';'))
      {
        // Must be typedef
        result = 1;
        this->declarationCnt = 0 ;
      }
      else
      {
      }
      break;
    case 2:
      if (Grammar_matchInitDeclaratorList(this, token))
      {
        // Keep matching until ';'
      }
      else if ((token->id == TOK_UNKNOWN) && ((uintptr_t)token->value == ';'))
      {
        //printf("YEAHHHHHHHH!\n");
        String_print(this->declarator.name,"Var. Declaration: ");
        String_delete(this->declarator.name);
        this->declarator.name = NULL;
        Grammar_reset(this);
        result = 1;
        this->declarationCnt = 0 ;
      }
      else
      {
        //this->declarationCnt = 0 ;
      }
      break;
  }
  return result;
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
unsigned int Grammar_matchDeclarationSpecifiers(Grammar* this, Token* token)
{
  if (!this->evaluatedDeclarationSpecifiers)
  {
  switch(this->declarationSpecifiersCnt)
  {
    case 0:
      if (Grammar_matchStorageClass(this, token))
      {
	        this->resultDeclarationSpecifiers = 1;
      }
      else if (Grammar_matchTypeSpecifier(this, token))
      {
	        this->resultDeclarationSpecifiers = 1;
      }
      else if (Grammar_matchTypeQualifier(this, token))
      {
	        this->resultDeclarationSpecifiers = 1;
      }
	  }
	  this->evaluatedDeclarationSpecifiers = 1;
  }

  return this->resultDeclarationSpecifiers;
}

/****************************************************************************
init_declarator_list
	: init_declarator
	| init_declarator_list ',' init_declarator
****************************************************************************/
unsigned int Grammar_matchInitDeclaratorList(Grammar* this, Token* token)
{
  unsigned int result = 0;

  switch (this->initDeclaratorListCnt)
  {
    case 0:
      if (Grammar_matchDeclarator(this, token))
      {
        result = 1;
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
  return result;
}

/****************************************************************************
init_declarator
	: declarator
	| declarator '=' initializer
****************************************************************************/
unsigned int Grammar_matchDeclarator(Grammar* this, Token* token)
{
  if (!this->evaluatedDeclarator)
  {
  switch (this->declaratorCnt)
  {
    case 0:
      if (Grammar_matchPointer(this, token))
      {
        this->declaratorCnt = 1;
      }
      else if (Grammar_matchDirectDeclarator(this, token))
      {
          this->resultDeclarator = 1;
      }
      else
      {
        // Syntax Error
      }
    case 1:
      if (Grammar_matchDirectDeclarator(this, token))
      {
          this->resultDeclarator = 1;
      }
      else
      {
        // Syntax Error
      }
  }
     this->evaluatedDeclarator = 1;
   }

  return this->resultDeclarator;
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
unsigned int Grammar_matchDirectDeclarator(Grammar* this, Token* token)
{
  unsigned int result = 0;

  switch (this->directDeclaratorCnt)
  {
    case 0:
      if (token->id == TOK_IDENTIFIER)
      {
        result = 1;
        this->directDeclaratorCnt = 1;
        this->declarator.name = String_dup((String*)token->value);
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
        this->directDeclaratorCnt = 2;
      }
      else if ((token->id == TOK_UNKNOWN) && ((uintptr_t)token->value == '('))
      {
        this->directDeclaratorCnt = 3;
      }
      break;
    case 2:
      if ((token->id == TOK_UNKNOWN) && ((uintptr_t)token->value == ']'))
      {
      }
      else if (Grammar_matchConstantExpression(this, token))
      {
        this->directDeclaratorCnt = 4;
      }
      break;
    case 3:
      if ((token->id == TOK_UNKNOWN) && ((uintptr_t)token->value == ')'))
      {
        result = 1;
      }
      break;
    case 4:
      printf("Not Supported!/n");
      break;
  }
  return result;
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
unsigned int Grammar_matchStorageClass(Grammar* this, Token* token)
{
  unsigned int result = 0;

  if ((token->id == TOK_EXTERN) || (token->id == TOK_TYPEDEF) ||
      (token->id == TOK_STATIC) || (token->id == TOK_AUTO) ||
      (token->id == TOK_REGISTER))
  {
    result = 1;
  }

  return result;
}

/****************************************************************************
****************************************************************************/
unsigned int Grammar_matchConstantExpression(Grammar* this, Token* token)
{
  unsigned int result = 0;

  return result;
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
unsigned int Grammar_matchTypeSpecifier(Grammar* this, Token* token)
{
  unsigned int result = 0;

  if ((token->id == TOK_VOID) || (token->id == TOK_CHAR) ||
      (token->id == TOK_SHORT) || (token->id == TOK_INT) ||
      (token->id == TOK_LONG) || (token->id == TOK_FLOAT) ||
      (token->id == TOK_DOUBLE) || (token->id == TOK_SIGNED) ||
      (token->id == TOK_UNSIGNED))
  {
    result = 1;
  }
  else if (token->id == TOK_IDENTIFIER)
  {
    //String_print((String*)token->value, "Hey I am here ");
    if (String_cmp((String*)token->value,"Uint")) result = 1;
    //result = 1;
  }
  else
  {
    result = Grammar_matchStructOrUnion(this, token);
  }

  return result;
}

/**************************************************
type_qualifier
	: CONST
	| VOLATILE
	;
****************************************************************************/
unsigned int Grammar_matchTypeQualifier(Grammar* this, Token* token)
{
  unsigned int result = 0;

  if ((token->id == TOK_CONST) || (token->id == TOK_VOLATILE))
  {
    return 1;
  }

  return result;
}

/****************************************************************************
pointer
	: '*'
	| '*' type_qualifier_list
	| '*' pointer
	| '*' type_qualifier_list pointer
	;
****************************************************************************/
unsigned int Grammar_matchPointer(Grammar* this, Token* token)
{
  unsigned int result = 0;

  result += 0;

  return 0;
}

/****************************************************************************
compound_statement
	: '{' '}'
	| '{' statement_list '}'
	| '{' declaration_list '}'
	| '{' declaration_list statement_list '}'
	;
****************************************************************************/
unsigned int Grammar_matchCompountStatement(Grammar* this, Token* token)
{
  unsigned int result = 0;

  switch(this->compountStatementCnt)
  {
    case 0:
      if ((token->id == TOK_UNKNOWN) && ((uintptr_t)token->value == '{'))
      {
        this->compountStatementCnt = 1;
        result = 1;
      }
    case 1:
      if ((token->id == TOK_UNKNOWN) && ((uintptr_t)token->value == '}'))
      {
        result = 1;
      }
  }

  return result;
}

/****************************************************************************
struct_or_union_specifier
	: struct_or_union IDENTIFIER '{' struct_declaration_list '}'
	| struct_or_union '{' struct_declaration_list '}'
	| struct_or_union IDENTIFIER
  ;
****************************************************************************/
unsigned int Grammar_matchStructOrUnion(Grammar* this, Token* token)
{
  unsigned int result = 0;

  switch (this->structOrUnionCnt)
  {
    case 0:
      if ((token->id == TOK_STRUCT) || (token->id == TOK_UNION))
      {
        this->structOrUnionCnt = 1;
        result = 1;
      }
      break;
    case 1:
      if ((token->id == TOK_IDENTIFIER))
      {

      } else if ((token->id == TOK_UNKNOWN) && ((uintptr_t)token->value == '{'))
      {

      }
      break;
    case 2:
      if (Grammar_matchStructDeclarationList(this, token))
      {
      }
      else if ((token->id == TOK_UNKNOWN) && ((uintptr_t)token->value == '}'))
      {

      }
  }
  return result;
}

/****************************************************************************
struct_declaration_list
	: struct_declaration
	| struct_declaration_list struct_declaration
	;

struct_declaration
	: specifier_qualifier_list struct_declarator_list ';' struct_or_union IDENTIFIER
  ;
****************************************************************************/
unsigned int Grammar_matchStructDeclarationList(Grammar* this, Token* token)
{
  unsigned int result = 0;

  return result;
}
