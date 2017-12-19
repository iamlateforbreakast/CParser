/**********************************************//** 
  @file CParser.h
  
  @brief TBD
  @details TBD
**************************************************/
#ifndef _CPARSER_H_
#define _CPARSER_H_

typedef struct CParser CParser;

PUBLIC CParser* CParser_new();
PUBLIC void CParser_delete(CParser* this);
PUBLIC void CParser_parse(CParser* this, char* fileName);

#endif
