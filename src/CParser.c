/* CParser.c */

#include "Common.h"
#include "CParser.h"
#include "FileList.h"

CParser* CParser_new()
{
    CParser* this;

    this=(CParser*)Memory_alloc(sizeof(CParser));

    this->tokenList = TokenList_new();
    this->fileList = FileList_new();

    return this;
}

void CParser_delete(CParser* this)
{
    TokenList_delete(this->tokenList);
    FileList_delete(this->fileList);

    Memory_free(this, sizeof(CParser));
}

void CParser_parse(CParser* this, const char* dirName)
{
    String* newDirName;

    newDirName = String_new(dirName);

    FileList_list(this->fileList, newDirName, String_new(".c"));

    // List all C files in newDirName and sub directories
    // for each C file add to the TokenList
    //Initialise from initial fileName
    TokenList_initialise(this->tokenList, newDirName);
    
    /*while (TokensList_get())
	populate functions from list of tokens
	populate global vars
	populate local vars
	populate code blocks */
    String_delete(newDirName);
}

/*void CParser_populateFunctions()
{
    if Grammar_matchFunctions(element)
    {
    }
}

void CParser_populateGlobalVars()
{
}*/
