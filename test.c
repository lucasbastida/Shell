#include <stdlib.h>
#include <stdio.h>
#include <unistd.h>

int main()
{

    for (size_t i = 0; i < 5; i++)
    {
        printf("%ld\n", i);
    }

    sleep(10);

    for (size_t i = 5; i < 10; i++)
    {
        printf("%ld\n", i);
    }
    return 0;
}
