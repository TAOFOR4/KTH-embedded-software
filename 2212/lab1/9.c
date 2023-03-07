#include <stdio.h>

int main()
{
    int amountInserted = 0;
    int bottleCost = 10;

    while (amountInserted < bottleCost)
    {
        printf("\nEnter an input (10, 5, 0): ");
        int coin;
        scanf("%d", &coin);

        if (coin == 0 || coin == 5 || coin == 10)
        {
            amountInserted += coin;
        }
        else
        {
            printf("\nInvalid coin.\n");
        }

        if (amountInserted == bottleCost)
        {
            printf("\nOutput: Bottle");
            amountInserted = 0;
        }
        else if(amountInserted > bottleCost)
        {
            printf("\nOutput: Bottle, Return");
            amountInserted = 0;
        }
        else
        {
            printf("\nOutput: Nothing");
        }

    }
    return 0;
}
