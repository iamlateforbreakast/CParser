int addInt(int n, int m)
{
  return m+n;
}

typedef int (*myFuncDef)(int, int);

void function1(void* param11, void *(param12)(void* t))
{
}

void* function2(int param21, int param22)
{
}

myFuncDef function3(int param31) 
{
  myFuncDef functionPtr = addInt;
  return functionPtr;
}
