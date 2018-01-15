int addInt(int n, int m)
{
  return m+n;
}

typedef int (*myFuncDef)(int, int);

myFuncDef functionFactory(int n) 
{
  myFuncDef functionPtr = addInt;
  return functionPtr;
}
