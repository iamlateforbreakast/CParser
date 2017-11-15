#define _TEST_H_

#define TRUE 1

#define INC(X) X=X+1

#define ADD(X,Y) X=X+Y

void f()
{
  int a;
  int b;

  INC(a);
  ADD(a,b);

  a = TRUE;
}
