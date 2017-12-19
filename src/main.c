/* Main */

#include "Common.h"
#include "CParser.h"

int main(int argc, char** argv)
{
  int result = 0;
  CParser* cparser = NULL;
  
  if (argc == 2)
  {
    cparser=CParser_new();
    CParser_parse(cparser, argv[1]);
    CParser_delete(cparser);
    Memory_report();
  }
  else
  {
	  printf("Usage: cparser filename.c\n");
  }

    return result;
}
