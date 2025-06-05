#include <stdio.h>
#include <stdlib.h>

int gcd(int a, int b);
int mod_inverse(int a, int n);

int main()
{
    int a, n;
    printf(" ----- EXTENDED EUCLIDEAN ALGORITHM -----\n");

    printf("Enter the value of a: ");
    scanf("%d", &a);
    printf("Enter the value of n: ");
    scanf("%d", &n);

    int inv_a = mod_inverse(a, n);
    if (inv_a == -1)
    {
        printf("\n	- The modular inverse of %d in %d does not exist.\n", a, n);
        return EXIT_FAILURE;
    }
    else if (inv_a == -2)
    {
        printf("\n	- The modular inverse of %d in modulo %d does not exist.\n", a, n);
        return EXIT_FAILURE;
    }
    else
    {
        printf("\n	- %d^(-1) mod %d = %d <- (MULTIPLICATIVE INVERSE OF a mod %d)\n ", a, n, inv_a, n);
    }

    return EXIT_SUCCESS;
}

int gcd(int a, int b)
{
    a = (b == 0) ? a : gcd(b, (a % b));
}

int mod_inverse(int a, int n)
{
    int t = 0, new_t = 1;
    int r = n, new_r = a;
    int q;

    printf("Initial values:\n");
    printf("t = %d\tnew_t = %d\tr = %d\tnew_r = %d\n", t, new_t, r, new_r);

    // If the greatest common divisor (gcd) between a and n is not 1, modular inverse does not exist
    if (gcd(a, n) != 1)
        return -1;

    while (new_r != 0)
    {
        // Calculate the quotient of the division
        q = r / new_r;
        printf("\nQuotient calculation: %d / %d = %d\n", r, new_r, q);

        // Update t and new_t
        int temp_t = t;
        t = new_t;
        new_t = temp_t - q * new_t;

        // Update r and new_r
        int temp_r = r;
        r = new_r;
        new_r = temp_r - q * new_r;

        printf("State after iteration:\n");
        printf("t = %d\tnew_t = %d\tr = %d\tnew_r = %d\n", t, new_t, r, new_r);
    }

    // If the last r is not 1, then there is no modular inverse
    if (r > 1)
        return -2;

    // If t is negative, adjust it by adding n
    if (t < 0)
    {
        printf("\nt=%d + %d", t, n);
        t = t + n;
    }
    return t;
}

int add_inverse(int b, int n)
{
    return (n - b % n) % n;
}
