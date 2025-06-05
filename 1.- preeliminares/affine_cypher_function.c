#include <stdio.h>
#include <stdlib.h>

int gcd(int a, int b);
int mod_inverse(int a, int n);
int add_inverse(int b, int n);

int main()
{
    int a, b, n;
    printf(" ----- AFFINE FUNCTION -----\n");

    printf(" - Encryption:\n");
    printf(" 	- D_k(a,B,n) : C = a*p + B mod n\n\n");

    printf(" - Decryption:\n");
    printf(" 	- E_k(a,B,n) : C = a^(-1) * [C + (-B)] mod n\n\n");

    printf("Enter the value of a: ");
    scanf("%d", &a);
    printf("Enter the value of B: ");
    scanf("%d", &b);
    printf("Enter the value of n: ");
    scanf("%d", &n);

    if (b >= (n - 1))
    {
        printf("\n	- The number %d is outside the ring Z_%d, its value will be %d mod %d = %d\n", b, n, b, n, b % n);
        b = b % n;
    }

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

    int inv_b = add_inverse(b, n);
    printf("	- %d + (-%d) mod %d = %d <- (ADDITIVE INVERSE OF B mod %d)\n ", b, b, n, inv_b, n);

    printf("\n - Encryption Function:\n");
    printf("	- C = (%d * p + %d) mod %d\n", a, b, n);

    printf("\n - Decryption Function 1:\n");
    printf("	- P = %d * (C + %d) mod %d\n", inv_a, inv_b, n);

    printf("\n - Decryption Function 2:\n");
    printf("	- P = %d*C + %d mod %d\n", inv_a, ((inv_a * inv_b) % n), n);

    return EXIT_SUCCESS;
}

int gcd(int a, int b)
{
    //   	if (b == 0)
    //		return a;
    //	else
    //		gcd(b,(a%b));
    a = (b == 0) ? a : gcd(b, (a % b));
}

int mod_inverse(int a, int n)
{
    int t = 0, new_t = 1;
    int r = n, new_r = a;
    int q;

    if (gcd(a, n) != 1)
        return -1;

    while (new_r != 0)
    {
        int q = r / new_r;

        int temp_t = t;
        t = new_t;
        new_t = temp_t - q * new_t;

        int temp_r = r;
        r = new_r;
        new_r = temp_r - q * new_r;
    }

    if (r > 1)
        return -2;

    if (t < 0)
        t = t + n;

    return t;
}

int add_inverse(int b, int n)
{
    return (n - b % n) % n;
}
