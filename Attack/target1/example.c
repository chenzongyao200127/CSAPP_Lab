#include <stdio.h>

int main()
{
    int arg1 = 100;
    test(arg1);
    return 0;
}

void test(int n)
{
    n *= 2;
    int m = 10;
    printf("arg1: %d", n + m);
}