#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <wchar.h>
#include <wctype.h>
#include <time.h>

// Alfabeto español como arreglo de strings para soportar 'ñ'
const char *alfabeto_es[27] = {
    "a", "b", "c", "d", "e", "f", "g", "h", "i", "j", "k", "l", "m",
    "n", "ñ", "o", "p", "q", "r", "s", "t", "u", "v", "w", "x", "y", "z"};

// Alfabeto inglés
const char *alfabeto_en[26] = {
    "a", "b", "c", "d", "e", "f", "g", "h", "i", "j", "k", "l", "m",
    "n", "o", "p", "q", "r", "s", "t", "u", "v", "w", "x", "y", "z"};

// Función para obtener el índice de una letra en el alfabeto
int letra_a_numero(const char *letra, const char *alfabeto[], int longitud);

// Función para convertir un número a letra según el alfabeto
const char *numero_a_letra(int numero, const char *alfabeto[], int longitud);

// Prototipos para funciones de procesamiento de texto y matrices
void texto_a_matriz_numerica(const char *texto, int n, int **matriz, const char *alfabeto[], int longitud);
void imprimir_matriz_numerica(int n, int **matriz);
void imprimir_matriz_alfabetica(int n, int **matriz, const char *alfabeto[], int longitud);
void encrypt_message();
int dividir_texto_en_bloques(const char *texto, int rows, int cols, int ***bloques, const char *alfabeto[], int longitud);
void multiplicar_matrices_mod(int rows_a, int cols_a, int cols_b, int **A, int **B, int **C, int mod_n);
void cifrar_mensaje(int block_rows, int block_cols, int num_blocks, int **key_matrix, int ***plaintext_blocks, int ***ciphertext_blocks, int mod_n);
void matrices_a_texto(int block_rows, int block_cols, int num_blocks, int ***matrices, char *texto, const char *alfabeto[], int longitud);

// Prototipos de funciones para operaciones aritméticas básicas
int gcd(int a, int b);
int mod(int a, int n);
int mod_inverse(int a, int n);
int add_inverse(int b, int n);

// Definición de estructura para fracciones
typedef struct
{
    int num;
    int den;
} Frac;

// Prototipos para operaciones con fracciones
void simplify(Frac *f);
Frac frac(int num, int den);
Frac add(Frac a, Frac b);
Frac sub(Frac a, Frac b);
Frac mul(Frac a, Frac b);
Frac divf(Frac a, Frac b);
void print_frac(Frac f);
void print_frac_mod(Frac f, int n);

// Prototipos para operaciones con matrices de fracciones
Frac determinant(int n, Frac **mat);
int determinant_mod(int n, Frac **mat, int mod_n);
int invert_matrix(int n, Frac **mat, Frac **result);
int apply_modulo_to_matrix(int n, Frac **mat, Frac **result, int mod_n);
void inverse_matrix(int n, Frac **mat, int mod_n);
void calculate_inverse_matrix();

// Prototipos para cifrado y descifrado
void decrypt_message();
int calculate_inverse_key(int n, int **key_matrix, int **inverse_key, int mod_n);
int generate_valid_key(int n, int **key_matrix, int mod_n);

// Función de utilidad para alocar memoria para una matriz
int **allocate_matrix(int rows, int cols)
{
    int **matrix = (int **)malloc(rows * sizeof(int *));
    for (int i = 0; i < rows; i++)
    {
        matrix[i] = (int *)malloc(cols * sizeof(int));
    }
    return matrix;
}

// Función de utilidad para liberar memoria de una matriz
void free_matrix(int **matrix, int rows)
{
    for (int i = 0; i < rows; i++)
    {
        free(matrix[i]);
    }
    free(matrix);
}

// Función de utilidad para alocar memoria para una matriz de fracciones
Frac **allocate_frac_matrix(int rows, int cols)
{
    Frac **matrix = (Frac **)malloc(rows * sizeof(Frac *));
    for (int i = 0; i < rows; i++)
    {
        matrix[i] = (Frac *)malloc(cols * sizeof(Frac));
    }
    return matrix;
}

// Función de utilidad para liberar memoria de una matriz de fracciones
void free_frac_matrix(Frac **matrix, int rows)
{
    for (int i = 0; i < rows; i++)
    {
        free(matrix[i]);
    }
    free(matrix);
}

// Función de utilidad para alocar memoria para un arreglo 3D de matrices
int ***allocate_block_matrices(int num_blocks, int rows, int cols)
{
    int ***blocks = (int ***)malloc(num_blocks * sizeof(int **));
    for (int b = 0; b < num_blocks; b++)
    {
        blocks[b] = allocate_matrix(rows, cols);
    }
    return blocks;
}

// Función de utilidad para liberar memoria de un arreglo 3D de matrices
void free_block_matrices(int ***blocks, int num_blocks, int rows)
{
    for (int b = 0; b < num_blocks; b++)
    {
        free_matrix(blocks[b], rows);
    }
    free(blocks);
}

int main()
{
    int option = 0;

    while (option != 3)
    {
        // Display menu
        printf("\n===== HILL CIPHER =====\n");
        printf("1. Cifrar\n");
        printf("2. Descifrar\n");
        printf("3. Salir\n");
        printf("Seleccione una opcion: ");
        scanf("%d", &option);

        switch (option)
        {
        case 1:
            printf("\nOpcion de cifrado seleccionada.\n");
            encrypt_message();
            break;

        case 2:
            printf("\nOpcion de descifrado seleccionada.\n");
            decrypt_message();
            break;

        case 3:
            printf("\nSaliendo del programa. ¡Hasta luego!\n");
            break;

        default:
            printf("\nOpción invalida. Por favor, intente de nuevo.\n");
            break;
        }
    }

    return 0;
}

// Función para obtener el índice de una letra en el alfabeto
int letra_a_numero(const char *letra, const char *alfabeto[], int longitud)
{
    for (int i = 0; i < longitud; i++)
    {
        if (strcmp(letra, alfabeto[i]) == 0)
        {
            return i;
        }
    }
    return -1;
}

// Función para convertir un número a letra según el alfabeto
const char *numero_a_letra(int numero, const char *alfabeto[], int longitud)
{
    if (numero >= 0 && numero < longitud)
    {
        return alfabeto[numero];
    }
    return "?";
}

// Función para dividir el texto en bloques
int dividir_texto_en_bloques(const char *texto, int rows, int cols, int ***bloques, const char *alfabeto[], int longitud)
{
    int num_blocks = 0;
    int i = 0; // Índice para el texto

    // Mientras haya caracteres por procesar
    while (texto[i] != '\0' && num_blocks < 100)
    {
        int row = 0, col = 0;
        int characters_processed = 0;

        // Llenar un bloque
        while (texto[i] != '\0' && row < rows)
        {
            char letra[5] = {0}; // Buffer para almacenar un carácter

            // Verificar si es un carácter multibyte (como 'ñ')
            if ((unsigned char)texto[i] >= 0xC3)
            {
                letra[0] = texto[i];
                letra[1] = texto[i + 1];
                i += 2;
            }
            else
            {
                letra[0] = texto[i];
                i += 1;
            }

            // Convertir la letra a número según el alfabeto
            int numero = letra_a_numero(letra, alfabeto, longitud);
            if (numero != -1)
            {
                bloques[num_blocks][row][col] = numero;
                col++;
                characters_processed++;

                // Si completamos una fila, pasamos a la siguiente
                if (col >= cols)
                {
                    col = 0;
                    row++;
                }
            }

            // Si se completó el bloque o no hay más caracteres, salimos del bucle
            if (row >= rows || texto[i] == '\0')
            {
                break;
            }
        }

        // Rellenar con ceros (equivalente a 'a') el resto del bloque si es necesario
        while (row < rows)
        {
            while (col < cols)
            {
                bloques[num_blocks][row][col] = 0; // 'a' en ambos alfabetos
                col++;
            }
            col = 0;
            row++;
        }

        // Si procesamos al menos un carácter, contamos este bloque
        if (characters_processed > 0)
        {
            num_blocks++;
        }
        else
        {
            // Si no procesamos ningún carácter, salimos del bucle principal
            break;
        }
    }

    return num_blocks;
}

// Función para convertir texto a matriz numérica según el alfabeto
void texto_a_matriz_numerica(const char *texto, int n, int **matriz, const char *alfabeto[], int longitud)
{
    int i = 0;             // Índice para texto
    int fila = 0, col = 0; // Índices para la matriz

    // Recorrer el texto caracter por caracter
    while (texto[i] != '\0' && fila < n)
    {
        char letra[5] = {0}; // Buffer para almacenar un carácter (incluso multibyte)

        // Verificar si es un carácter especial (multibyte como 'ñ')
        if ((unsigned char)texto[i] >= 0xC3)
        {
            letra[0] = texto[i];
            letra[1] = texto[i + 1];
            i += 2;
        }
        else
        {
            letra[0] = texto[i];
            i += 1;
        }

        // Convertir la letra a número según el alfabeto
        int numero = letra_a_numero(letra, alfabeto, longitud);
        if (numero != -1)
        {
            matriz[fila][col] = numero;
            col++;
            if (col >= n)
            {
                col = 0;
                fila++;
            }
        }

        // Si llegamos al final de la matriz pero aún hay texto, ignorarlo
        if (fila >= n)
            break;
    }

    // Rellenar el resto de la matriz con ceros si el texto no es suficiente
    while (fila < n)
    {
        while (col < n)
        {
            matriz[fila][col] = 0; // 0 corresponde a 'a' en ambos alfabetos
            col++;
        }
        col = 0;
        fila++;
    }
}

// Función para imprimir una matriz numérica
void imprimir_matriz_numerica(int n, int **matriz)
{
    for (int i = 0; i < n; i++)
    {
        for (int j = 0; j < n; j++)
        {
            printf("%2d ", matriz[i][j]);
        }
        printf("\n");
    }
}

// Función para imprimir una matriz en formato alfabético
void imprimir_matriz_alfabetica(int n, int **matriz, const char *alfabeto[], int longitud)
{
    for (int i = 0; i < n; i++)
    {
        for (int j = 0; j < n; j++)
        {
            printf("%2s ", numero_a_letra(matriz[i][j], alfabeto, longitud));
        }
        printf("\n");
    }
}

// Función que contiene la funcionalidad original para calcular la matriz inversa
void calculate_inverse_matrix()
{
    int n, mod_n;
    printf("Ingrese el tamanio de la matriz cuadrada (n x n): ");
    scanf("%d", &n);

    Frac **mat = allocate_frac_matrix(n, n);
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

    printf("Ingrese el valor del modulo n: ");
    scanf("%d", &mod_n);

    inverse_matrix(n, mat, mod_n);

    free_frac_matrix(mat, n);
}

// Función para multiplicar dos matrices y aplicar módulo n
void multiplicar_matrices_mod(int rows_a, int cols_a, int cols_b, int **A, int **B, int **C, int mod_n)
{
    int i, j, k;

    // Inicializar la matriz resultado con ceros
    for (i = 0; i < rows_a; i++)
    {
        for (j = 0; j < cols_b; j++)
        {
            C[i][j] = 0;
        }
    }

    // Multiplicar las matrices
    for (i = 0; i < rows_a; i++)
    {
        for (j = 0; j < cols_b; j++)
        {
            for (k = 0; k < cols_a; k++)
            {
                C[i][j] = (C[i][j] + A[i][k] * B[k][j]) % mod_n;
            }

            // Asegurar que el resultado sea no negativo
            if (C[i][j] < 0)
            {
                C[i][j] = (C[i][j] + mod_n) % mod_n;
            }
        }
    }
}

// Función para cifrar todos los bloques del mensaje
void cifrar_mensaje(int block_rows, int block_cols, int num_blocks, int **key_matrix, int ***plaintext_blocks,
                    int ***ciphertext_blocks, int mod_n)
{
    // Para cada bloque del mensaje
    for (int b = 0; b < num_blocks; b++)
    {
        // Multiplicar el bloque del mensaje por la matriz clave
        multiplicar_matrices_mod(block_rows, block_cols, block_cols, plaintext_blocks[b], key_matrix, ciphertext_blocks[b], mod_n);
    }
}

// Función para convertir matrices numéricas a texto
void matrices_a_texto(int block_rows, int block_cols, int num_blocks, int ***matrices, char *texto, const char *alfabeto[], int longitud)
{
    int pos = 0;

    // Procesar cada bloque
    for (int b = 0; b < num_blocks; b++)
    {
        // Recorrer la matriz fila por fila
        for (int i = 0; i < block_rows; i++)
        {
            for (int j = 0; j < block_cols; j++)
            {
                // Convertir el número a letra según el alfabeto
                const char *letra = numero_a_letra(matrices[b][i][j], alfabeto, longitud);

                // Copiar la letra al texto resultante
                strcpy(texto + pos, letra);
                pos += strlen(letra);
            }
        }
    }

    // Asegurar que el texto termine correctamente
    texto[pos] = '\0';
}

// =============================================================================
// OPERACIONES ARITMÉTICAS BÁSICAS
// =============================================================================

int gcd(int a, int b)
{
    return b == 0 ? abs(a) : gcd(b, a % b);
}

// Calculo del módulo de un número asegurando que sea positivo
int mod(int a, int n)
{
    return ((a % n) + n) % n; // Asegura que el valor sea positivo.
}

// Calculo del inverso multiplicativo de a módulo n usando el algoritmo extendido de Euclides
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

// Calculo del inverso aditivo de b módulo n
int add_inverse(int b, int n)
{
    return (n - b % n) % n;
}

// =============================================================================
// OPERACIONES CON FRACCIONES
// =============================================================================

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
        printf("%d", f.num);
    else
        printf("%d/%d \t\t", f.num, f.den);
}

void print_frac_mod(Frac f, int n)
{
    // Aplicar módulo al numerador
    int num = mod(f.num, n);

    // Si el denominador no es 1, calcular el inverso multiplicativo
    if (f.den != 1)
    {
        int inv_den = mod_inverse(mod(f.den, n), n);
        if (inv_den == -1)
        {
            printf("ERROR: No existe inverso multiplicativo para %d mod %d", f.den, n);
            return;
        }
        // Aplicar el inverso multiplicativo
        num = mod(num * inv_den, n);
        printf("%d", num);
    }
    else
    {
        printf("%d", num);
    }
}

// =============================================================================
// OPERACIONES CON MATRICES
// =============================================================================

// Optimización de determinant para matrices grandes
Frac determinant(int n, Frac **mat)
{
    // Casos base
    if (n == 1)
        return mat[0][0];

    if (n == 2)
        return sub(mul(mat[0][0], mat[1][1]), mul(mat[0][1], mat[1][0]));

    Frac det = frac(0, 1);

    // Crear un array para seguir qué columnas están disponibles
    int *available = (int *)malloc(n * sizeof(int));
    for (int j = 0; j < n; j++)
        available[j] = 1;

    // Expansión por cofactores optimizada
    for (int j = 0; j < n; j++)
    {
        if (mat[0][j].num == 0)
            continue; // Saltar cálculos innecesarios

        // Crear submatriz más eficientemente
        Frac **submat = allocate_frac_matrix(n - 1, n - 1);
        available[j] = 0; // Marcar esta columna como usada

        for (int i = 1; i < n; i++)
        {
            int col_idx = 0;
            for (int k = 0; k < n; k++)
            {
                if (available[k])
                {
                    submat[i - 1][col_idx++] = mat[i][k];
                }
            }
        }

        available[j] = 1; // Restaurar esta columna para la siguiente iteración

        // Calcular cofactor
        Frac cofactor = determinant(n - 1, submat);

        // Alternar signo: (-1)^(i+j)
        if (j % 2 == 1)
            cofactor.num = -cofactor.num;

        // Sumar al determinante
        det = add(det, mul(mat[0][j], cofactor));

        free_frac_matrix(submat, n - 1);
    }

    free(available);
    return det;
}

// Mejora en el cálculo del determinante modular
int determinant_mod(int n, Frac **mat, int mod_n)
{
    // Para matrices grandes, primero reducir todos los elementos módulo mod_n
    Frac **mod_mat = allocate_frac_matrix(n, n);
    for (int i = 0; i < n; i++)
    {
        for (int j = 0; j < n; j++)
        {
            int num = mod(mat[i][j].num, mod_n);
            int den = mod(mat[i][j].den, mod_n);

            // Calcular el inverso multiplicativo del denominador
            int inv_den = mod_inverse(den, mod_n);
            if (inv_den == -1)
            {
                free_frac_matrix(mod_mat, n);
                return -1; // No existe inverso multiplicativo
            }

            mod_mat[i][j] = frac(mod(num * inv_den, mod_n), 1);
        }
    }

    // Calcular determinante de la matriz ya reducida en módulo mod_n
    Frac det = determinant(n, mod_mat);

    free_frac_matrix(mod_mat, n);

    // Como todos los elementos ya están en módulo mod_n con denominador 1,
    // el determinante también tendrá denominador 1
    return mod(det.num, mod_n);
}

// Invierte una matriz sin aplicar módulo
int invert_matrix(int n, Frac **mat, Frac **result)
{
    Frac **aug = allocate_frac_matrix(n, 2 * n);

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
                free_frac_matrix(aug, n);
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

    free_frac_matrix(aug, n);
    return 1; // Éxito
}

// Aplica módulo n a una matriz
int apply_modulo_to_matrix(int n, Frac **mat, Frac **result, int mod_n)
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

            // Denominador inverso multiplicativo módulo mod_n
            int inv_den = mod_inverse(mat[i][j].den, mod_n);
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

// Función principal que calcula la inversa modular de una matriz
void inverse_matrix(int n, Frac **mat, int mod_n)
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
        printf("La matriz no tiene inversa porque el determinante no tiene inverso multiplicativo modulo %d.\n", mod_n);
        return;
    }

    printf("Determinante (mod %d): %d\n", mod_n, det);
    Frac **inv_mat = allocate_frac_matrix(n, n);
    Frac **mod_mat = allocate_frac_matrix(n, n);

    // Paso 1: Invertir la matriz
    if (!invert_matrix(n, mat, inv_mat))
    {
        printf("La matriz no tiene inversa.\n");
        free_frac_matrix(inv_mat, n);
        free_frac_matrix(mod_mat, n);
        return;
    }

    // Imprimir matriz inversa
    printf("Matriz inversa:\n");
    for (int i = 0; i < n; i++)
    {
        for (int j = 0; j < n; j++)
        {
            print_frac(inv_mat[i][j]);
            printf("\t");
        }
        printf("\n");
    }

    // Paso 2: Aplicar módulo a la matriz invertida
    if (!apply_modulo_to_matrix(n, inv_mat, mod_mat, mod_n))
    {
        printf("No existe el inverso multiplicativo del denominador.\n");
        free_frac_matrix(inv_mat, n);
        free_frac_matrix(mod_mat, n);
        return;
    }

    // Imprimir matriz inversa modular
    printf("Matriz inversa (mod %d):\n", mod_n);
    for (int i = 0; i < n; i++)
    {
        for (int j = 0; j < n; j++)
        {
            print_frac_mod(mod_mat[i][j], mod_n);
            printf("\t");
        }
        printf("\n");
    }

    free_frac_matrix(inv_mat, n);
    free_frac_matrix(mod_mat, n);
}

// Función para descifrar un mensaje usando el cifrado Hill
void decrypt_message()
{
    int opcion_alfabeto;
    const char **alfabeto;
    int longitud_alfabeto;
    int block_cols; // Tamaño de la matriz clave (NxN) y número de columnas por bloque
    int block_rows; // Número de filas por bloque
    char ciphertext[1000];
    char key_text[1000];
    int **key_matrix;         // Matriz clave
    int **inverse_key;        // Matriz clave inversa
    int ***ciphertext_blocks; // Para almacenar los bloques del mensaje cifrado
    int ***plaintext_blocks;  // Para almacenar los bloques del mensaje descifrado
    int num_blocks = 0;
    char decrypted_text[1000] = {0}; // Texto descifrado

    // 1. Seleccionar el alfabeto
    printf("Seleccione el alfabeto:\n");
    printf("1. Español (incluye ñ, 27 caracteres)\n");
    printf("2. Inglés (26 caracteres)\n");
    printf("Opción: ");
    scanf("%d", &opcion_alfabeto);
    getchar(); // Consumir salto de línea

    if (opcion_alfabeto == 1)
    {
        alfabeto = alfabeto_es;
        longitud_alfabeto = 27;
        printf("Alfabeto español seleccionado (módulo %d).\n", longitud_alfabeto);
    }
    else if (opcion_alfabeto == 2)
    {
        alfabeto = alfabeto_en;
        longitud_alfabeto = 26;
        printf("Alfabeto inglés seleccionado (módulo %d).\n", longitud_alfabeto);
    }
    else
    {
        printf("Opción inválida. Se usará el alfabeto inglés por defecto.\n");
        alfabeto = alfabeto_en;
        longitud_alfabeto = 26;
    }

    // 2. Solicitar tamaño de la matriz clave (que es cuadrada NxN)
    printf("Ingrese el tamaño de la matriz clave (n x n): ");
    scanf("%d", &block_cols);
    getchar(); // Consumir salto de línea

    // Validar el tamaño de la clave
    if (block_cols <= 0 || block_cols > 10)
    {
        printf("Error: El tamaño de la matriz clave debe estar entre 1 y 10.\n");
        return;
    }

    // Asignar memoria para matrices de clave
    key_matrix = allocate_matrix(block_cols, block_cols);
    inverse_key = allocate_matrix(block_cols, block_cols);

    // 2.1. Solicitar número de filas por bloque para el mensaje
    printf("Ingrese el número de filas por bloque para el mensaje: ");
    scanf("%d", &block_rows);
    getchar(); // Consumir salto de línea

    // Validar el número de filas por bloque
    if (block_rows <= 0 || block_rows > 10)
    {
        printf("Error: El número de filas por bloque debe estar entre 1 y 10.\n");
        free_matrix(key_matrix, block_cols);
        free_matrix(inverse_key, block_cols);
        return;
    }

    // 3. Solicitar la clave como texto
    printf("Ingrese la clave (al menos %d caracteres): ", block_cols * block_cols);
    fgets(key_text, sizeof(key_text), stdin);
    key_text[strcspn(key_text, "\n")] = '\0'; // Eliminar salto de línea

    // 4. Solicitar el mensaje cifrado
    printf("Ingrese el mensaje cifrado: ");
    fgets(ciphertext, sizeof(ciphertext), stdin);
    ciphertext[strcspn(ciphertext, "\n")] = '\0'; // Eliminar salto de línea

    // 5. Convertir la clave a matriz numérica
    texto_a_matriz_numerica(key_text, block_cols, key_matrix, alfabeto, longitud_alfabeto);

    // Asignar memoria para los bloques
    ciphertext_blocks = (int ***)malloc(100 * sizeof(int **));
    plaintext_blocks = (int ***)malloc(100 * sizeof(int **));
    for (int b = 0; b < 100; b++)
    {
        ciphertext_blocks[b] = allocate_matrix(block_rows, block_cols);
        plaintext_blocks[b] = allocate_matrix(block_rows, block_cols);
    }

    // 6. Convertir el mensaje cifrado en bloques
    num_blocks = dividir_texto_en_bloques(ciphertext, block_rows, block_cols, ciphertext_blocks, alfabeto, longitud_alfabeto);

    // 7. Mostrar información
    printf("\n=== INFORMACIÓN DE DESCIFRADO ===\n");
    printf("Alfabeto: %s (%d caracteres)\n",
           opcion_alfabeto == 1 ? "Español" : "Inglés",
           longitud_alfabeto);
    printf("Tamaño de la matriz clave: %d x %d\n", block_cols, block_cols);
    printf("Tamaño de los bloques de mensaje: %d x %d\n", block_rows, block_cols);
    printf("Clave (texto): %s\n", key_text);
    printf("Mensaje cifrado: %s\n", ciphertext);

    // 8. Mostrar la matriz clave en formato numérico
    printf("\n=== MATRIZ CLAVE (NUMÉRICA) ===\n");
    imprimir_matriz_numerica(block_cols, key_matrix);

    // 9. Mostrar la matriz clave en formato alfabético
    printf("\n=== MATRIZ CLAVE (ALFABÉTICA) ===\n");
    imprimir_matriz_alfabetica(block_cols, key_matrix, alfabeto, longitud_alfabeto);

    // 10. Calcular la inversa de la matriz clave
    if (!calculate_inverse_key(block_cols, key_matrix, inverse_key, longitud_alfabeto))
    {
        printf("No se puede descifrar: la matriz clave no tiene inversa módulo %d.\n", longitud_alfabeto);
        free_matrix(key_matrix, block_cols);
        free_matrix(inverse_key, block_cols);
        for (int b = 0; b < 100; b++)
        {
            free_matrix(ciphertext_blocks[b], block_rows);
            free_matrix(plaintext_blocks[b], block_rows);
        }
        free(ciphertext_blocks);
        free(plaintext_blocks);
        return;
    }

    printf("\n=== MATRIZ CLAVE INVERSA (NUMÉRICA) ===\n");
    imprimir_matriz_numerica(block_cols, inverse_key);

    printf("\n=== MATRIZ CLAVE INVERSA (ALFABÉTICA) ===\n");
    imprimir_matriz_alfabetica(block_cols, inverse_key, alfabeto, longitud_alfabeto);

    // 11. Mostrar los bloques del mensaje cifrado
    printf("\n======================================================\n");
    printf("=== BLOQUES DEL MENSAJE CIFRADO (TOTAL: %d) ===\n", num_blocks);
    printf("======================================================\n");

    for (int b = 0; b < num_blocks; b++)
    {
        printf("\n============= BLOQUE %d DE %d =============\n", b + 1, num_blocks);

        printf("Representación numérica:\n");
        for (int i = 0; i < block_rows; i++)
        {
            for (int j = 0; j < block_cols; j++)
            {
                printf("%2d ", ciphertext_blocks[b][i][j]);
            }
            printf("\n");
        }

        printf("Representación alfabética:\n");
        for (int i = 0; i < block_rows; i++)
        {
            for (int j = 0; j < block_cols; j++)
            {
                printf("%2s ", numero_a_letra(ciphertext_blocks[b][i][j], alfabeto, longitud_alfabeto));
            }
            printf("\n");
        }

        printf("==========================================\n");
    }

    // 12. Realizar el proceso de descifrado
    printf("\n=== PROCESO DE DESCIFRADO ===\n");

    // Descifrar cada bloque del mensaje (Multiplicamos cada bloque por la inversa de la clave)
    for (int b = 0; b < num_blocks; b++)
    {
        multiplicar_matrices_mod(block_rows, block_cols, block_cols, ciphertext_blocks[b], inverse_key, plaintext_blocks[b], longitud_alfabeto);
    }

    // 13. Mostrar los bloques descifrados
    printf("\n======================================================\n");
    printf("=== BLOQUES DEL MENSAJE DESCIFRADO (TOTAL: %d) ===\n", num_blocks);
    printf("======================================================\n");

    for (int b = 0; b < num_blocks; b++)
    {
        printf("\n============= BLOQUE %d DE %d =============\n", b + 1, num_blocks);

        printf("Representación numérica:\n");
        for (int i = 0; i < block_rows; i++)
        {
            for (int j = 0; j < block_cols; j++)
            {
                printf("%2d ", plaintext_blocks[b][i][j]);
            }
            printf("\n");
        }

        printf("Representación alfabética:\n");
        for (int i = 0; i < block_rows; i++)
        {
            for (int j = 0; j < block_cols; j++)
            {
                printf("%2s ", numero_a_letra(plaintext_blocks[b][i][j], alfabeto, longitud_alfabeto));
            }
            printf("\n");
        }

        printf("==========================================\n");
    }

    // 14. Convertir los bloques descifrados a texto
    matrices_a_texto(block_rows, block_cols, num_blocks, plaintext_blocks, decrypted_text, alfabeto, longitud_alfabeto);

    // 15. Mostrar el texto descifrado
    printf("\n=== TEXTO DESCIFRADO ===\n");
    printf("%s\n", decrypted_text);

    // Liberar memoria
    free_matrix(key_matrix, block_cols);
    free_matrix(inverse_key, block_cols);

    for (int b = 0; b < 100; b++)
    {
        free_matrix(ciphertext_blocks[b], block_rows);
        free_matrix(plaintext_blocks[b], block_rows);
    }
    free(ciphertext_blocks);
    free(plaintext_blocks);
}

// Función para calcular la inversa de la matriz clave modulo n
int calculate_inverse_key(int n, int **key_matrix, int **inverse_key, int mod_n)
{
    // Convertir la matriz de enteros a matriz de fracciones
    Frac **mat = allocate_frac_matrix(n, n);
    for (int i = 0; i < n; i++)
    {
        for (int j = 0; j < n; j++)
        {
            mat[i][j] = frac(key_matrix[i][j], 1);
        }
    }

    // Calcular el determinante modular
    int det = determinant_mod(n, mat, mod_n);
    if (det == 0 || det == -1)
    {
        free_frac_matrix(mat, n);
        return 0; // No existe inversa
    }

    // Calcular la matriz inversa
    Frac **inv_mat = allocate_frac_matrix(n, n);
    if (!invert_matrix(n, mat, inv_mat))
    {
        free_frac_matrix(mat, n);
        free_frac_matrix(inv_mat, n);
        return 0; // No se pudo invertir la matriz
    }

    // Aplicar módulo a la matriz inversa
    Frac **mod_mat = allocate_frac_matrix(n, n);
    if (!apply_modulo_to_matrix(n, inv_mat, mod_mat, mod_n))
    {
        free_frac_matrix(mat, n);
        free_frac_matrix(inv_mat, n);
        free_frac_matrix(mod_mat, n);
        return 0; // No existe inverso multiplicativo para algún denominador
    }

    // Convertir la matriz de fracciones a matriz de enteros
    for (int i = 0; i < n; i++)
    {
        for (int j = 0; j < n; j++)
        {
            inverse_key[i][j] = mod_mat[i][j].num; // Como el denominador es 1, solo copiamos el numerador
        }
    }

    free_frac_matrix(mat, n);
    free_frac_matrix(inv_mat, n);
    free_frac_matrix(mod_mat, n);
    return 1; // Éxito
}

// Función para generar una matriz clave válida (con inversa mod n)
int generate_valid_key(int n, int **key_matrix, int mod_n)
{
    // Usar time como semilla para números aleatorios
    srand(time(NULL));

    int max_attempts = 1000; // Límite de intentos para encontrar una matriz válida
    int attempts = 0;

    while (attempts < max_attempts)
    {
        // Generar matriz aleatoria
        for (int i = 0; i < n; i++)
        {
            for (int j = 0; j < n; j++)
            {
                key_matrix[i][j] = rand() % mod_n;
            }
        }

        // Verificar si la matriz tiene inversa (det != 0 y gcd(det, mod_n) = 1)
        Frac **mat = allocate_frac_matrix(n, n);
        for (int i = 0; i < n; i++)
        {
            for (int j = 0; j < n; j++)
            {
                mat[i][j] = frac(key_matrix[i][j], 1);
            }
        }

        int det = determinant_mod(n, mat, mod_n);
        free_frac_matrix(mat, n);

        if (det != 0 && det != -1)
        {
            // La matriz tiene inversa
            return 1;
        }

        attempts++;
    }

    return 0; // No se pudo encontrar una matriz válida después de max_attempts intentos
}

// Función para cifrar un mensaje usando el cifrado Hill
void encrypt_message()
{
    int opcion_alfabeto;
    const char **alfabeto;
    int longitud_alfabeto;
    int block_cols; // Tamaño de la matriz de clave (NxN) y número de columnas por bloque
    int block_rows; // Número de filas por bloque
    char plaintext[1000];
    char key_text[1000];
    int **key_matrix;        // Matriz clave
    int ***plaintext_blocks; // Para almacenar los bloques del mensaje
    int num_blocks = 0;
    int opcion_clave;

    // 1. Seleccionar el alfabeto
    printf("Seleccione el alfabeto:\n");
    printf("1. Español (incluye ñ, 27 caracteres)\n");
    printf("2. Inglés (26 caracteres)\n");
    printf("Opción: ");
    scanf("%d", &opcion_alfabeto);
    getchar(); // Consumir salto de línea

    if (opcion_alfabeto == 1)
    {
        alfabeto = alfabeto_es;
        longitud_alfabeto = 27;
        printf("Alfabeto español seleccionado (módulo %d).\n", longitud_alfabeto);
    }
    else if (opcion_alfabeto == 2)
    {
        alfabeto = alfabeto_en;
        longitud_alfabeto = 26;
        printf("Alfabeto inglés seleccionado (módulo %d).\n", longitud_alfabeto);
    }
    else
    {
        printf("Opción inválida. Se usará el alfabeto inglés por defecto.\n");
        alfabeto = alfabeto_en;
        longitud_alfabeto = 26;
    }

    // 2. Solicitar tamaño de la matriz clave (que es cuadrada NxN)
    printf("Ingrese el tamaño de la matriz clave (n x n): ");
    scanf("%d", &block_cols);
    getchar(); // Consumir salto de línea

    // Validar el tamaño de la clave
    if (block_cols <= 0 || block_cols > 10)
    {
        printf("Error: El tamaño de la matriz clave debe estar entre 1 y 10.\n");
        return;
    }

    // Asignar memoria para la matriz clave
    key_matrix = allocate_matrix(block_cols, block_cols);

    // 2.1. Solicitar número de filas por bloque para el mensaje
    printf("Ingrese el número de filas por bloque para el mensaje: ");
    scanf("%d", &block_rows);
    getchar(); // Consumir salto de línea

    // Validar el número de filas por bloque
    if (block_rows <= 0 || block_rows > 10)
    {
        printf("Error: El número de filas por bloque debe estar entre 1 y 10.\n");
        free_matrix(key_matrix, block_cols);
        return;
    }

    // 3. Preguntar si desea generar una clave válida o introducirla manualmente
    printf("¿Desea generar una clave válida automáticamente o ingresarla manualmente?\n");
    printf("1. Generar automáticamente\n");
    printf("2. Ingresar manualmente\n");
    printf("Opción: ");
    scanf("%d", &opcion_clave);
    getchar(); // Consumir salto de línea

    if (opcion_clave == 1)
    {
        // Generar matriz clave válida
        if (generate_valid_key(block_cols, key_matrix, longitud_alfabeto))
        {
            printf("Se ha generado una clave válida para módulo %d.\n", longitud_alfabeto);

            // Mostrar la clave generada en formato texto
            printf("Clave generada (numérica): \n");
            imprimir_matriz_numerica(block_cols, key_matrix);

            printf("Clave generada (alfabética): \n");
            imprimir_matriz_alfabetica(block_cols, key_matrix, alfabeto, longitud_alfabeto);

            // Crear una representación textual de la clave
            int pos = 0;
            for (int i = 0; i < block_cols; i++)
            {
                for (int j = 0; j < block_cols; j++)
                {
                    const char *letra = numero_a_letra(key_matrix[i][j], alfabeto, longitud_alfabeto);
                    strcpy(key_text + pos, letra);
                    pos += strlen(letra);
                }
            }
            key_text[pos] = '\0';
            printf("Clave generada (texto): %s\n", key_text);
        }
        else
        {
            printf("Error al generar una clave válida. Por favor, inténtelo de nuevo.\n");
            free_matrix(key_matrix, block_cols);
            return;
        }
    }
    else
    {
        // 3. Solicitar la clave como texto
        printf("Ingrese la clave (al menos %d caracteres): ", block_cols * block_cols);
        fgets(key_text, sizeof(key_text), stdin);
        key_text[strcspn(key_text, "\n")] = '\0'; // Eliminar salto de línea

        // Convertir la clave a matriz numérica
        texto_a_matriz_numerica(key_text, block_cols, key_matrix, alfabeto, longitud_alfabeto);

        // Verificar que la clave sea válida (tenga inversa)
        Frac **mat = allocate_frac_matrix(block_cols, block_cols);
        for (int i = 0; i < block_cols; i++)
        {
            for (int j = 0; j < block_cols; j++)
            {
                mat[i][j] = frac(key_matrix[i][j], 1);
            }
        }

        int det = determinant_mod(block_cols, mat, longitud_alfabeto);
        free_frac_matrix(mat, block_cols);

        if (det == 0 || det == -1)
        {
            printf("La clave ingresada no es válida (no tiene inversa módulo %d).\n", longitud_alfabeto);
            printf("Por favor, intente con otra clave.\n");
            free_matrix(key_matrix, block_cols);
            return;
        }
    }

    // 4. Solicitar el mensaje a cifrar
    printf("Ingrese el mensaje a cifrar: ");
    fgets(plaintext, sizeof(plaintext), stdin);
    plaintext[strcspn(plaintext, "\n")] = '\0'; // Eliminar salto de línea

    // Asignar memoria para bloques
    plaintext_blocks = (int ***)malloc(100 * sizeof(int **));
    for (int b = 0; b < 100; b++)
    {
        plaintext_blocks[b] = allocate_matrix(block_rows, block_cols);
    }

    // 5. Convertir el mensaje en bloques (la clave ya está convertida)
    num_blocks = dividir_texto_en_bloques(plaintext, block_rows, block_cols, plaintext_blocks, alfabeto, longitud_alfabeto);

    // 7. Mostrar información
    printf("\n=== INFORMACIÓN DE CIFRADO ===\n");
    printf("Alfabeto: %s (%d caracteres)\n",
           opcion_alfabeto == 1 ? "Español" : "Inglés",
           longitud_alfabeto);
    printf("Tamaño de la matriz clave: %d x %d\n", block_cols, block_cols);
    printf("Tamaño de los bloques de mensaje: %d x %d\n", block_rows, block_cols);
    printf("Clave (texto): %s\n", key_text);
    printf("Mensaje a cifrar: %s\n", plaintext);

    // 8. Mostrar la matriz clave en formato numérico
    printf("\n=== MATRIZ CLAVE (NUMÉRICA) ===\n");
    imprimir_matriz_numerica(block_cols, key_matrix);

    // 9. Mostrar la matriz clave en formato alfabético
    printf("\n=== MATRIZ CLAVE (ALFABÉTICA) ===\n");
    imprimir_matriz_alfabetica(block_cols, key_matrix, alfabeto, longitud_alfabeto);

    // 10. Mostrar los bloques del mensaje en formato numérico y alfabético
    printf("\n======================================================\n");
    printf("=== BLOQUES DEL MENSAJE A CIFRAR (TOTAL: %d) ===\n", num_blocks);
    printf("======================================================\n");

    for (int b = 0; b < num_blocks; b++)
    {
        printf("\n============= BLOQUE %d DE %d =============\n", b + 1, num_blocks);

        printf("Representación numérica:\n");
        for (int i = 0; i < block_rows; i++)
        {
            for (int j = 0; j < block_cols; j++)
            {
                printf("%2d ", plaintext_blocks[b][i][j]);
            }
            printf("\n");
        }

        printf("Representación alfabética:\n");
        for (int i = 0; i < block_rows; i++)
        {
            for (int j = 0; j < block_cols; j++)
            {
                printf("%2s ", numero_a_letra(plaintext_blocks[b][i][j], alfabeto, longitud_alfabeto));
            }
            printf("\n");
        }

        printf("==========================================\n");
    }

    // 11. Realizar el proceso de cifrado
    printf("\n=== PROCESO DE CIFRADO ===\n");

    // Arreglos para almacenar el mensaje cifrado
    int ***ciphertext_blocks = (int ***)malloc(100 * sizeof(int **));
    for (int b = 0; b < 100; b++)
    {
        ciphertext_blocks[b] = allocate_matrix(block_rows, block_cols);
    }
    char ciphertext[1000] = {0};

    // Cifrar cada bloque del mensaje
    cifrar_mensaje(block_rows, block_cols, num_blocks, key_matrix, plaintext_blocks, ciphertext_blocks, longitud_alfabeto);

    // 12. Mostrar los bloques cifrados
    printf("\n======================================================\n");
    printf("=== BLOQUES DEL MENSAJE CIFRADO (TOTAL: %d) ===\n", num_blocks);
    printf("======================================================\n");

    for (int b = 0; b < num_blocks; b++)
    {
        printf("\n============= BLOQUE %d DE %d =============\n", b + 1, num_blocks);

        printf("Representación numérica:\n");
        for (int i = 0; i < block_rows; i++)
        {
            for (int j = 0; j < block_cols; j++)
            {
                printf("%2d ", ciphertext_blocks[b][i][j]);
            }
            printf("\n");
        }

        printf("Representación alfabética:\n");
        for (int i = 0; i < block_rows; i++)
        {
            for (int j = 0; j < block_cols; j++)
            {
                printf("%2s ", numero_a_letra(ciphertext_blocks[b][i][j], alfabeto, longitud_alfabeto));
            }
            printf("\n");
        }

        printf("==========================================\n");
    }

    // 13. Convertir los bloques cifrados a texto
    matrices_a_texto(block_rows, block_cols, num_blocks, ciphertext_blocks, ciphertext, alfabeto, longitud_alfabeto);

    // 14. Mostrar el texto cifrado
    printf("\n=== TEXTO CIFRADO ===\n");
    printf("%s\n", ciphertext);

    // Liberar la memoria
    free_matrix(key_matrix, block_cols);

    for (int b = 0; b < 100; b++)
    {
        free_matrix(plaintext_blocks[b], block_rows);
        free_matrix(ciphertext_blocks[b], block_rows);
    }
    free(plaintext_blocks);
    free(ciphertext_blocks);
}
