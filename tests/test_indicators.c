#include <stdio.h>
#include <stdlib.h>
#include "indicators.h"

int main()
{
    double prices[] = {1, 2, 3, 4, 5, 6, 7, 8, 9, 10, 11, 12, 13, 14, 15, 16, 17, 18, 19, 20, 21, 22, 23, 24, 25, 26, 27, 28, 29, 30};
    int length = sizeof(prices) / sizeof(prices[0]);
    int window = 5;

    double *sma = compute_SMA(prices, length, window);
    if (!sma)
    {
        fprintf(stderr, "SMA computation failed\n");
        return 1;
    }

    printf("SMA results:\n");
    for (int i = 0; i < length - window + 1; i++)
    {
        printf("%.2f ", sma[i]);
    }
    printf("\n");

    c_free(sma);
    return 0;
}
