#include <stdio.h>
#include <stdlib.h>

typedef struct
{
    int num;
    int den;
} Frac;

// Function prototypes
int gcd(int a, int b);
void simplify(Frac *f);
int mod(int a, int n);
int determinant_mod(int n, Frac mat[n][n], int mod_n);
void print_frac(Frac f);
void print_frac_mod(Frac f, int n);
int invert_matrix(int n, Frac mat[n][n], Frac result[n][n]);
int apply_modulo_to_matrix(int n, Frac mat[n][n], Frac result[n][n], int mod_n);
void inverse_matrix(int n, Frac mat[n][n], int mod_n);
int mod_inverse(int a, int n);
int add_inverse(int b, int n);

Frac frac(int num, int den);
Frac add(Frac a, Frac b);
Frac sub(Frac a, Frac b);
Frac mul(Frac a, Frac b);
Frac divf(Frac a, Frac b);
Frac determinant(int n, Frac mat[n][n]);

int main()
{
    int n, mod_n;
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

    printf("Ingrese el valor del módulo n: ");
    scanf("%d", &mod_n);

    inverse_matrix(n, mat, mod_n);
    return 0;
}

// Function implementations
int gcd(int a, int b)
{
    a = (b == 0) ? a : gcd(b, (a % b));
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

Frac determinant(int n, Frac mat[n][n])
{
    // Caso base: matriz 1x1
    if (n == 1)
    {
        return mat[0][0];
    }

    // Caso base: matriz 2x2
    if (n == 2)
    {
        return sub(mul(mat[0][0], mat[1][1]), mul(mat[0][1], mat[1][0]));
    }

    Frac det = frac(0, 1);

    // Expansión por cofactores a lo largo de la primera fila
    for (int j = 0; j < n; j++)
    {
        // Crear submatriz
        Frac submat[n - 1][n - 1];
        for (int i = 1; i < n; i++)
        {
            int col_idx = 0;
            for (int k = 0; k < n; k++)
            {
                if (k != j)
                {
                    submat[i - 1][col_idx++] = mat[i][k];
                }
            }
        }

        // Calcular cofactor
        Frac cofactor = determinant(n - 1, submat);

        // Alternar signo: (-1)^(i+j)
        if (j % 2 == 1)
        {
            cofactor.num = -cofactor.num;
        }

        // Sumar al determinante
        det = add(det, mul(mat[0][j], cofactor));
    }

    return det;
}

int mod(int a, int n)
{
    return ((a % n) + n) % n; // Asegura que el valor sea positivo.
}

int determinant_mod(int n, Frac mat[n][n], int mod_n)
{
    // Calcular determinante normal
    Frac det = determinant(n, mat);

    // Aplicar módulo
    int det_num = mod(det.num, mod_n);
    int det_den = mod(det.den, mod_n);

    // Calcular inverso multiplicativo del denominador en mod_n
    int inv_den = mod_inverse(det_den, mod_n);
    if (inv_den == -1)
    {
        return -1; // No existe el inverso multiplicativo
    }

    // Multiplicar numerador por inverso multiplicativo del denominador
    return (det_num * inv_den) % mod_n;
}

void print_frac(Frac f)
{
    if (f.den == 1)
        printf("%8d      ", f.num);
    else
        printf("%8d/%-8d", f.num, f.den);
}

void print_frac_mod(Frac f, int n)
{
    f.num = mod(f.num, n);
    f.den = mod(f.den, n);
    if (f.den == 1)
        printf("%8d      ", f.num);
    else
        printf("%8d/%-8d", f.num, f.den);
}

int invert_matrix(int n, Frac mat[n][n], Frac result[n][n])
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
                return 0; // La matriz no tiene inversa
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

    // Copiar la matriz inversa al resultado
    for (int i = 0; i < n; i++)
    {
        for (int j = 0; j < n; j++)
        {
            result[i][j] = aug[i][j + n];
        }
    }

    return 1; // Éxito
}

int apply_modulo_to_matrix(int n, Frac mat[n][n], Frac result[n][n], int mod_n)
{
    for (int i = 0; i < n; i++)
    {
        for (int j = 0; j < n; j++)
        {
            // Numerador inverso aditivo módulo mod_n
            mat[i][j].num = mod(mat[i][j].num, mod_n);
            if (mat[i][j].num < 0)
            {
                mat[i][j].num = add_inverse(mat[i][j].num, mod_n);
            }

            // Denominador módulo mod_n
            int den = mod(mat[i][j].den, mod_n);

            // Denominador inverso multiplicativo módulo mod_n
            int inv_den = mod_inverse(den, mod_n);
            if (inv_den == -1)
            {
                return 0; // No existe el inverso multiplicativo
            }

            // Multiplicar numerador y denominador
            result[i][j].num = (mat[i][j].num * inv_den) % mod_n;
            result[i][j].den = 1;
        }
    }
    return 1; // Éxito
}

void inverse_matrix(int n, Frac mat[n][n], int mod_n)
{
    // Calcular y mostrar el determinante
    int det = determinant_mod(n, mat, mod_n);
    if (det == 0)
    {
        printf("La matriz no tiene inversa porque su determinante es 0.\n");
        return;
    }
    else if (det == -1)
    {
        printf("La matriz no tiene inversa porque el determinante no tiene inverso multiplicativo módulo %d.\n", mod_n);
        return;
    }

    printf("Determinante (mod %d): %d\n", mod_n, det);
    Frac inv_mat[n][n];
    Frac mod_mat[n][n];

    // Paso 1: Invertir la matriz
    if (!invert_matrix(n, mat, inv_mat))
    {
        printf("La matriz no tiene inversa.\n");
        return;
    }

    // Imprimir matriz inversa
    printf("Matriz inversa:\n");
    for (int i = 0; i < n; i++)
    {
        printf("\t|\t");
        for (int j = 0; j < n; j++)
        {
            print_frac(inv_mat[i][j]);
            printf("  ");
        }
        printf("|\n");
    }

    // Paso 2: Aplicar módulo a la matriz invertida
    if (!apply_modulo_to_matrix(n, inv_mat, mod_mat, mod_n))
    {
        printf("No existe el inverso multiplicativo del denominador.\n");
        return;
    }

    // Imprimir matriz inversa modular
    printf("Matriz inversa (mod %d):\n", mod_n);
    for (int i = 0; i < n; i++)
    {
        printf("\t|\t");
        for (int j = 0; j < n; j++)
        {
            print_frac_mod(mod_mat[i][j], mod_n);
            printf("  ");
        }
        printf("|\n");
    }
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
        q = r / new_r;

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
