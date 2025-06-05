#include <stdio.h>
#include <stdlib.h>

// Define data type
typedef struct
{
    int num;
    int den;
} Frac;

// Function prototypes
int gcd(int a, int b);
void simplify(Frac *f);
Frac frac(int num, int den);
Frac add(Frac a, Frac b);
Frac sub(Frac a, Frac b);
Frac mul(Frac a, Frac b);
Frac divf(Frac a, Frac b);
void print_frac(Frac f);
void inverse_matrix(int n, Frac mat[][n]);

int main()
{
    int n;
    printf("Ingrese el tamaño de la matriz cuadrada (n x n): ");
    scanf("%d", &n);

    Frac mat[n][n];
    printf("Ingrese los elementos de la matriz (%d x %d):\n", n, n);
    for (int i = 0; i < n; i++)
    {
        for (int j = 0; j < n; j++)
        {
            int val;
            printf("Elemento [%d][%d]: ", i, j);
            scanf("%d", &val);
            mat[i][j] = frac(val, 1);
        }
    }

    inverse_matrix(n, mat);
    return 0;
}

// Function implementations
int gcd(int a, int b)
{
    return b == 0 ? abs(a) : gcd(b, a % b);
}

void simplify(Frac *f)
{
    if (f->den < 0)
    {
        f->num = -f->num;
        f->den = -f->den;
    }
    int g = gcd(f->num, f->den);
    f->num /= g;
    f->den /= g;
}

Frac frac(int num, int den)
{
    Frac f = {num, den};
    simplify(&f);
    return f;
}

Frac add(Frac a, Frac b)
{
    return frac(a.num * b.den + b.num * a.den, a.den * b.den);
}

Frac sub(Frac a, Frac b)
{
    return frac(a.num * b.den - b.num * a.den, a.den * b.den);
}

Frac mul(Frac a, Frac b)
{
    return frac(a.num * b.num, a.den * b.den);
}

Frac divf(Frac a, Frac b)
{
    return frac(a.num * b.den, a.den * b.num);
}

void print_frac(Frac f)
{
    if (f.den == 1)
        printf("%8d      ", f.num);
    else
        printf("%8d/%-8d", f.num, f.den);
}

void inverse_matrix(int n, Frac mat[n][n])
{
    Frac aug[n][2 * n];

    // Crear la matriz aumentada [mat | I]
    for (int i = 0; i < n; i++)
    {
        for (int j = 0; j < n; j++)
        {
            aug[i][j] = mat[i][j];
        }
        for (int j = n; j < 2 * n; j++)
        {
            aug[i][j] = frac(i == (j - n), 1);
        }
    }

    // Gauss-Jordan
    for (int i = 0; i < n; i++)
    {
        // Pivote no nulo
        if (aug[i][i].num == 0)
        {
            int swap = -1;
            for (int j = i + 1; j < n; j++)
            {
                if (aug[j][i].num != 0)
                {
                    swap = j;
                    break;
                }
            }
            if (swap == -1)
            {
                printf("La matriz no tiene inversa.\n");
                return;
            }
            for (int k = 0; k < 2 * n; k++)
            {
                Frac tmp = aug[i][k];
                aug[i][k] = aug[swap][k];
                aug[swap][k] = tmp;
            }
        }

        // Normalizar la fila
        Frac div_by = aug[i][i];
        for (int j = 0; j < 2 * n; j++)
        {
            aug[i][j] = divf(aug[i][j], div_by);
        }

        // Hacer ceros en otras filas
        for (int j = 0; j < n; j++)
        {
            if (j == i)
                continue;
            Frac factor = aug[j][i];
            for (int k = 0; k < 2 * n; k++)
            {
                aug[j][k] = sub(aug[j][k], mul(factor, aug[i][k]));
            }
        }
    }

    // Imprimir matriz inversa
    printf("Matriz inversa:\n");
    for (int i = 0; i < n; i++)
    {
        printf("\t|\t");
        for (int j = n; j < 2 * n; j++)
        {
            print_frac(aug[i][j]);
            printf("  ");
        }
        printf("|\n");
    }
}
