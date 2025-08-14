#include <stdio.h>
#include <stdlib.h>

// Function prototypes
void multiply_matrix(int **A, int **B, int **C, int m, int n, int p);
void print_matrix(int **matrix, int rows, int columns);
void print_matrix_mod(int **matrix, int rows, int columns, int n);
int gcd(int a, int b);
int mod_inverse(int a, int n);
int add_inverse(int b, int n);

int main()
{
    int m, n, p, mod;
    int i, j;

    printf("- Rows in matrix (A): ");
    scanf("%d", &m);
    printf("- Columns in matrix (A) / Rows in matrix (B): ");
    scanf("%d", &n);
    printf("- Columns in matrix (B): ");
    scanf("%d", &p);

    int **A = (int **)malloc(m * sizeof(int *));
    int **B = (int **)malloc(n * sizeof(int *));
    int **C = (int **)malloc(m * sizeof(int *));

    for (i = 0; i < m; i++)
        A[i] = (int *)malloc(n * sizeof(int));

    for (i = 0; i < n; i++)
        B[i] = (int *)malloc(p * sizeof(int));

    for (i = 0; i < m; i++)
        C[i] = (int *)malloc(p * sizeof(int));

    printf("- Enter the elements of matrix A (%d x %d):\n", m, n);
    for (i = 0; i < m; i++)
    {
        for (j = 0; j < n; j++)
        {
            printf("  - Element A[%d][%d]: ", i + 1, j + 1);
            scanf("%d", &A[i][j]);
        }
    }

    printf("- Enter the elements of matrix B (%d x %d):\n", n, p);
    for (i = 0; i < n; i++)
    {
        for (j = 0; j < p; j++)
        {
            printf("  - Element B[%d][%d]: ", i + 1, j + 1);
            scanf("%d", &B[i][j]);
        }
    }

    printf("- Matrix A:\n");
    print_matrix(A, m, n);

    printf("- Matrix B:\n");
    print_matrix(B, n, p);

    multiply_matrix(A, B, C, m, n, p);

    printf("- The resulting matrix from multiplication A x B is:\n");
    print_matrix(C, m, p);

    print_matrix_mod(C, m, p, mod);

    for (i = 0; i < m; i++)
    {
        free(A[i]);
    }
    free(A);

    for (i = 0; i < n; i++)
    {
        free(B[i]);
    }
    free(B);

    for (i = 0; i < m; i++)
    {
        free(C[i]);
    }
    free(C);

    return 0;
}

// Function implementations
void multiply_matrix(int **A, int **B, int **C, int m, int n, int p)
{
    int i, j, k;
    for (i = 0; i < m; i++)
    {
        for (j = 0; j < p; j++)
        {
            C[i][j] = 0;
            for (k = 0; k < n; k++)
            {
                C[i][j] += A[i][k] * B[k][j];
            }
        }
    }
}

void print_matrix(int **matrix, int rows, int columns)
{
    int i, j;
    for (i = 0; i < rows; i++)
    {
        printf("\t|\t");
        for (j = 0; j < columns; j++)
        {
            printf("%8d\t", matrix[i][j]);
        }
        printf("|");
        printf("\n");
    }
}

void print_matrix_mod(int **matrix, int rows, int columns, int n)
{
    int i, j;
    int num, mod_result;

    printf("Enter the value of n for the operation mod n: ");
    scanf("%d", &n);

    printf("- Matrix mod %d:\n", n);

    for (i = 0; i < rows; i++)
    {
        printf("\t|\t");
        for (j = 0; j < columns; j++)
        {
            num = matrix[i][j];
            if (num < 0)
            {
                mod_result = add_inverse(num, n);
            }
            else if (num == 0)
            {
                mod_result = 0;
            }
            else
            {
                mod_result = num % n;
            }
            printf("%8d\t", mod_result);
        }
        printf("|\n");
    }
}

int gcd(int a, int b)
{
    return (b == 0) ? a : gcd(b, a % b);
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
