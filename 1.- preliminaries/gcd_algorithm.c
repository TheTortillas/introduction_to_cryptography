#include <stdio.h>

int gcd_euclid(int a, int b);
int recursive_gcd(int a, int b);

int main()
{
	int a = 0, b = 0;

	printf("Enter a number (a): ");
	scanf("%d", &a);
	printf("Enter another number (b): ");
	scanf("%d", &b);

	printf("\n- The GCD of %d and %d is: %d\n", a, b, gcd(a, b));
	printf("- The GCD (recursive) of %d and %d is: %d\n", a, b, recursive_gcd(a, b));

	return 0;
}

int gcd(int a, int b)
{
	int temp = 0;
	while (b != 0)
	{
		temp = b;
		b = a % b;
		a = temp;
	}
	return a;
}

int recursive_gcd(int a, int b)
{
	if (b == 0)
	{
		return a;
	}
	else
	{
		recursive_gcd(b, (a % b));
	}
}
