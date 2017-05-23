/* Main */

#include "Common.h"
#include "CParser.h"

int main(int argc, char** argv)
{
  int result = 0;

  if (argc == 2)
  {
    cparser=CParser_new();
    CParser_parse(cparser, argv[1]);
    CParser_delete(cparser);
	  printf("Nb bytes not freed: %d\n", Memory_nbBytesAllocated);
    printf("Max nb bytes used: %d\n", Memory_maxNbBytesAllocated);
  }
  else
  {
	  printf("Usage: cparser filename.c\n");
  }

    return result;
}
