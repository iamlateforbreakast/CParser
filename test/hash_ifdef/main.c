#define TEST1

#ifdef TEST1

void success1()
{
}

#else

void fail1()
{
}

#endif

#define TEST2

#ifndef TEST2

void fail2()
{
}

#else

void succes2()
{
}

#endif
