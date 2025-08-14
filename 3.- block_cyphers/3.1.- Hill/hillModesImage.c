#include <stdio.h>
#include <stdlib.h>
#include <locale.h>
#include <time.h>
#include <string.h>

// Funciones de matriz
void multiplicar_matrices(int **A, int **B, int **C, int m, int n, int p, int mod);
void imprimir_matriz(int **matriz, int filas, int columnas);
void copiar_matriz(int **origen, int **destino, int filas, int columnas);
void XOR_matrices(int **A, int **B, int **C, int filas, int columnas, int mod);

// Funciones matemáticas
int gcd(int a, int b);
int mod_inverse(int a, int n);
int add_inverse(int b, int n);

// Funciones para operaciones de cifrado
void modo_ECB(int **imagen, int **llave, int **imagen_cifrada, int filas, int columnas_llave, int mod);
void modo_CBC(int **imagen, int **llave, int **IV, int **imagen_cifrada, int filas, int columnas_llave, int mod);
void modo_CFB(int **imagen, int **llave, int **IV, int **imagen_cifrada, int filas, int columnas_llave, int mod);
void modo_OFB(int **imagen, int **llave, int **IV, int **imagen_cifrada, int filas, int columnas_llave, int mod);
void modo_CTR(int **imagen, int **llave, int **IV, int **imagen_cifrada, int filas, int columnas_llave, int mod);

// Función para incrementar contador en modo CTR
void incrementar_contador(int **contador, int filas, int columnas, int mod);

int main()
{
    setlocale(LC_ALL, "");
    int filas_imagen, columnas_imagen = 3; // RGB = 3 columnas fijas
    int filas_llave, columnas_llave;
    int mod, modo;
    int i, j;

    printf("===== CIFRADO DE HILL PARA IMÁGENES =====\n\n");

    // Solicitar dimensiones de la matriz imagen
    printf("- Número de filas de la imagen: ");
    scanf("%d", &filas_imagen);

    // Fijar columnas a 3 (RGB)
    printf("- Número de columnas de la imagen: %d (RGB)\n", columnas_imagen);

    // Solicitar dimensiones de la llave
    printf("- Número de filas de la llave: ");
    scanf("%d", &filas_llave);

    // Las columnas de la llave deben ser iguales a las filas de cada bloque (3)
    columnas_llave = columnas_imagen;
    printf("- Número de columnas de la llave: %d (debe ser igual a las columnas de la imagen)\n", columnas_llave);

    // Solicitar el módulo
    printf("- Ingrese el valor del módulo (n): ");
    scanf("%d", &mod);

    // Asignación de memoria para matrices
    // Matriz imagen
    int **imagen = (int **)malloc(filas_imagen * sizeof(int *));
    for (i = 0; i < filas_imagen; i++)
        imagen[i] = (int *)malloc(columnas_imagen * sizeof(int));

    // Matriz llave
    int **llave = (int **)malloc(columnas_imagen * sizeof(int *));
    for (i = 0; i < columnas_imagen; i++)
        llave[i] = (int *)malloc(columnas_imagen * sizeof(int));

    // Matriz para la imagen cifrada
    int **imagen_cifrada = (int **)malloc(filas_imagen * sizeof(int *));
    for (i = 0; i < filas_imagen; i++)
        imagen_cifrada[i] = (int *)malloc(columnas_imagen * sizeof(int));

    // Vector de inicialización (IV)
    int **IV = (int **)malloc(1 * sizeof(int *));
    IV[0] = (int *)malloc(columnas_imagen * sizeof(int));

    // Solicitar datos de la matriz imagen
    printf("\n- Ingrese los elementos de la matriz imagen (%d x %d):\n", filas_imagen, columnas_imagen);
    for (i = 0; i < filas_imagen; i++)
    {
        for (j = 0; j < columnas_imagen; j++)
        {
            printf("  - Elemento imagen[%d][%d]: ", i + 1, j + 1);
            scanf("%d", &imagen[i][j]);
        }
    }

    // Solicitar datos de la matriz llave
    printf("\n- Ingrese los elementos de la matriz llave (%d x %d):\n", columnas_imagen, columnas_imagen);
    for (i = 0; i < columnas_imagen; i++)
    {
        for (j = 0; j < columnas_imagen; j++)
        {
            printf("  - Elemento llave[%d][%d]: ", i + 1, j + 1);
            scanf("%d", &llave[i][j]);
        }
    }

    // Mostrar matrices ingresadas
    printf("\n- Matriz imagen:\n");
    imprimir_matriz(imagen, filas_imagen, columnas_imagen);

    printf("\n- Matriz llave:\n");
    imprimir_matriz(llave, columnas_imagen, columnas_imagen);

    // Seleccionar modo de cifrado
    printf("\n===== MODOS DE CIFRADO =====\n");
    printf("1. ECB (Electronic Codebook)\n");
    printf("2. CBC (Cipher Block Chaining)\n");
    printf("3. CFB (Cipher Feedback)\n");
    printf("4. OFB (Output Feedback)\n");
    printf("5. CTR (Counter)\n");
    printf("- Seleccione un modo de cifrado (1-5): ");
    scanf("%d", &modo);

    // Si el modo no es ECB, solicitar vector de inicialización
    if (modo != 1)
    {
        printf("\n- Ingrese el vector de inicialización (IV) de tamaño %d:\n", columnas_imagen);
        for (j = 0; j < columnas_imagen; j++)
        {
            printf("  - Elemento IV[%d]: ", j + 1);
            scanf("%d", &IV[0][j]);
        }
    }

    // Realizar el cifrado según el modo seleccionado
    switch (modo)
    {
    case 1:
        printf("\n===== MODO ECB =====\n");
        modo_ECB(imagen, llave, imagen_cifrada, filas_imagen, columnas_imagen, mod);
        break;
    case 2:
        printf("\n===== MODO CBC =====\n");
        modo_CBC(imagen, llave, IV, imagen_cifrada, filas_imagen, columnas_imagen, mod);
        break;
    case 3:
        printf("\n===== MODO CFB =====\n");
        modo_CFB(imagen, llave, IV, imagen_cifrada, filas_imagen, columnas_imagen, mod);
        break;
    case 4:
        printf("\n===== MODO OFB =====\n");
        modo_OFB(imagen, llave, IV, imagen_cifrada, filas_imagen, columnas_imagen, mod);
        break;
    case 5:
        printf("\n===== MODO CTR =====\n");
        modo_CTR(imagen, llave, IV, imagen_cifrada, filas_imagen, columnas_imagen, mod);
        break;
    default:
        printf("Modo no válido.\n");
        return 1;
    }

    // Mostrar resultado
    printf("\n- Imagen cifrada:\n");
    imprimir_matriz(imagen_cifrada, filas_imagen, columnas_imagen);

    // Liberar memoria
    for (i = 0; i < filas_imagen; i++)
    {
        free(imagen[i]);
        free(imagen_cifrada[i]);
    }
    free(imagen);
    free(imagen_cifrada);

    for (i = 0; i < columnas_imagen; i++)
    {
        free(llave[i]);
    }
    free(llave);

    free(IV[0]);
    free(IV);

    return 0;
}

// Función para multiplicar matrices aplicando módulo
void multiplicar_matrices(int **A, int **B, int **C, int m, int n, int p, int mod)
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
            // Aplicar módulo
            C[i][j] = ((C[i][j] % mod) + mod) % mod;
        }
    }
}

// Función para imprimir una matriz
void imprimir_matriz(int **matriz, int filas, int columnas)
{
    int i, j;
    for (i = 0; i < filas; i++)
    {
        printf("\t|\t");
        for (j = 0; j < columnas; j++)
        {
            printf("%d\t", matriz[i][j]);
        }
        printf("|");
        printf("\n");
    }
}

// Función para copiar una matriz
void copiar_matriz(int **origen, int **destino, int filas, int columnas)
{
    int i, j;
    for (i = 0; i < filas; i++)
    {
        for (j = 0; j < columnas; j++)
        {
            destino[i][j] = origen[i][j];
        }
    }
}

// Función para realizar XOR entre matrices
void XOR_matrices(int **A, int **B, int **C, int filas, int columnas, int mod)
{
    int i, j;
    for (i = 0; i < filas; i++)
    {
        for (j = 0; j < columnas; j++)
        {
            C[i][j] = (A[i][j] ^ B[i][j]) % mod;
        }
    }
}

// Función para calcular el máximo común divisor
int gcd(int a, int b)
{
    return (b == 0) ? a : gcd(b, (a % b));
}

// Función para calcular el inverso multiplicativo modular
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

// Función para calcular el inverso aditivo modular
int add_inverse(int b, int n)
{
    return (n - b % n) % n;
}

// Implementación del modo ECB (Electronic Codebook)
void modo_ECB(int **imagen, int **llave, int **imagen_cifrada, int filas, int columnas_llave, int mod)
{
    int i, j, k;
    int **bloque_cifrado = (int **)malloc(1 * sizeof(int *));
    bloque_cifrado[0] = (int *)malloc(columnas_llave * sizeof(int));

    printf("\n=== DETALLE DEL PROCESO DE CIFRADO ECB ===\n");

    // Procesar cada fila (bloque) de la imagen
    for (i = 0; i < filas; i++)
    {
        printf("\nProcesando bloque %d:\n", i + 1);
        printf("  - Bloque de imagen original: [ ");
        for (j = 0; j < columnas_llave; j++)
        {
            printf("%d ", imagen[i][j]);
        }
        printf("]\n");

        printf("  - Cálculo de multiplicación de matrices:\n");

        // Mostrar el cálculo de cada elemento
        for (j = 0; j < columnas_llave; j++)
        {
            printf("    C[%d] = ", j);
            for (k = 0; k < columnas_llave; k++)
            {
                printf("(%d × %d)", imagen[i][k], llave[k][j]);
                if (k < columnas_llave - 1)
                    printf(" + ");
            }

            // Calcular el resultado sin módulo
            int resultado = 0;
            for (k = 0; k < columnas_llave; k++)
            {
                resultado += imagen[i][k] * llave[k][j];
            }

            // Aplicar módulo
            int resultado_mod = ((resultado % mod) + mod) % mod;
            printf(" = %d mod %d = %d\n", resultado, mod, resultado_mod);
        }

        // Cifrar el bloque actual
        multiplicar_matrices(&imagen[i], llave, &imagen_cifrada[i], 1, columnas_llave, columnas_llave, mod);

        printf("  - Bloque cifrado: [ ");
        for (j = 0; j < columnas_llave; j++)
        {
            printf("%d ", imagen_cifrada[i][j]);
        }
        printf("]\n");
    }

    // Liberar memoria
    free(bloque_cifrado[0]);
    free(bloque_cifrado);
}

// Implementación del modo CBC (Cipher Block Chaining)
void modo_CBC(int **imagen, int **llave, int **IV, int **imagen_cifrada, int filas, int columnas_llave, int mod)
{
    int i, j, k;
    int **bloque_actual = (int **)malloc(1 * sizeof(int *));
    bloque_actual[0] = (int *)malloc(columnas_llave * sizeof(int));

    int **bloque_xor = (int **)malloc(1 * sizeof(int *));
    bloque_xor[0] = (int *)malloc(columnas_llave * sizeof(int));

    printf("\n=== DETALLE DEL PROCESO DE CIFRADO CBC ===\n");

    printf("\nProcesando bloque 1:\n");
    printf("  - Bloque de imagen original: [ ");
    for (j = 0; j < columnas_llave; j++)
    {
        printf("%d ", imagen[0][j]);
    }
    printf("]\n");

    printf("  - Vector de inicialización (IV): [ ");
    for (j = 0; j < columnas_llave; j++)
    {
        printf("%d ", IV[0][j]);
    }
    printf("]\n");

    // Para el primer bloque, XOR con el IV
    printf("  - Operación XOR con IV:\n");
    for (j = 0; j < columnas_llave; j++)
    {
        printf("    %d ⊕ %d = %d\n", imagen[0][j], IV[0][j], imagen[0][j] ^ IV[0][j]);
        bloque_xor[0][j] = (imagen[0][j] ^ IV[0][j]) % mod;
    }

    printf("  - Resultado del XOR: [ ");
    for (j = 0; j < columnas_llave; j++)
    {
        printf("%d ", bloque_xor[0][j]);
    }
    printf("]\n");

    printf("  - Cálculo de multiplicación de matrices:\n");
    // Mostrar el cálculo de cada elemento
    for (j = 0; j < columnas_llave; j++)
    {
        printf("    C[%d] = ", j);
        for (k = 0; k < columnas_llave; k++)
        {
            printf("(%d × %d)", bloque_xor[0][k], llave[k][j]);
            if (k < columnas_llave - 1)
                printf(" + ");
        }

        // Calcular el resultado sin módulo
        int resultado = 0;
        for (k = 0; k < columnas_llave; k++)
        {
            resultado += bloque_xor[0][k] * llave[k][j];
        }

        // Aplicar módulo
        int resultado_mod = ((resultado % mod) + mod) % mod;
        printf(" = %d mod %d = %d\n", resultado, mod, resultado_mod);
    }

    // Cifrar el primer bloque
    multiplicar_matrices(bloque_xor, llave, &imagen_cifrada[0], 1, columnas_llave, columnas_llave, mod);

    printf("  - Bloque 1 cifrado: [ ");
    for (j = 0; j < columnas_llave; j++)
    {
        printf("%d ", imagen_cifrada[0][j]);
    }
    printf("]\n");

    // Para los bloques restantes
    for (i = 1; i < filas; i++)
    {
        printf("\nProcesando bloque %d:\n", i + 1);
        printf("  - Bloque de imagen original: [ ");
        for (j = 0; j < columnas_llave; j++)
        {
            printf("%d ", imagen[i][j]);
        }
        printf("]\n");

        printf("  - Bloque cifrado anterior: [ ");
        for (j = 0; j < columnas_llave; j++)
        {
            printf("%d ", imagen_cifrada[i - 1][j]);
        }
        printf("]\n");

        // XOR con el bloque cifrado anterior
        printf("  - Operación XOR con bloque cifrado anterior:\n");
        for (j = 0; j < columnas_llave; j++)
        {
            printf("    %d ⊕ %d = %d\n", imagen[i][j], imagen_cifrada[i - 1][j], imagen[i][j] ^ imagen_cifrada[i - 1][j]);
            bloque_xor[0][j] = (imagen[i][j] ^ imagen_cifrada[i - 1][j]) % mod;
        }

        printf("  - Resultado del XOR: [ ");
        for (j = 0; j < columnas_llave; j++)
        {
            printf("%d ", bloque_xor[0][j]);
        }
        printf("]\n");

        printf("  - Cálculo de multiplicación de matrices:\n");
        // Mostrar el cálculo de cada elemento
        for (j = 0; j < columnas_llave; j++)
        {
            printf("    C[%d] = ", j);
            for (k = 0; k < columnas_llave; k++)
            {
                printf("(%d × %d)", bloque_xor[0][k], llave[k][j]);
                if (k < columnas_llave - 1)
                    printf(" + ");
            }

            // Calcular el resultado sin módulo
            int resultado = 0;
            for (k = 0; k < columnas_llave; k++)
            {
                resultado += bloque_xor[0][k] * llave[k][j];
            }

            // Aplicar módulo
            int resultado_mod = ((resultado % mod) + mod) % mod;
            printf(" = %d mod %d = %d\n", resultado, mod, resultado_mod);
        }

        // Cifrar el bloque actual
        multiplicar_matrices(bloque_xor, llave, &imagen_cifrada[i], 1, columnas_llave, columnas_llave, mod);

        printf("  - Bloque %d cifrado: [ ", i + 1);
        for (j = 0; j < columnas_llave; j++)
        {
            printf("%d ", imagen_cifrada[i][j]);
        }
        printf("]\n");
    }

    // Liberar memoria
    free(bloque_actual[0]);
    free(bloque_actual);
    free(bloque_xor[0]);
    free(bloque_xor);
}

// Implementación del modo CFB (Cipher Feedback)
void modo_CFB(int **imagen, int **llave, int **IV, int **imagen_cifrada, int filas, int columnas_llave, int mod)
{
    int i, j;
    int **bloque_cifrado = (int **)malloc(1 * sizeof(int *));
    bloque_cifrado[0] = (int *)malloc(columnas_llave * sizeof(int));

    // Cifrar IV para el primer bloque
    multiplicar_matrices(IV, llave, bloque_cifrado, 1, columnas_llave, columnas_llave, mod);

    // XOR del resultado con el primer bloque de texto plano
    for (j = 0; j < columnas_llave; j++)
    {
        imagen_cifrada[0][j] = (imagen[0][j] ^ bloque_cifrado[0][j]) % mod;
    }

    // Para los bloques restantes
    for (i = 1; i < filas; i++)
    {
        // Cifrar el bloque cifrado anterior
        multiplicar_matrices(&imagen_cifrada[i - 1], llave, bloque_cifrado, 1, columnas_llave, columnas_llave, mod);

        // XOR del resultado con el bloque de texto plano actual
        for (j = 0; j < columnas_llave; j++)
        {
            imagen_cifrada[i][j] = (imagen[i][j] ^ bloque_cifrado[0][j]) % mod;
        }
    }

    // Liberar memoria
    free(bloque_cifrado[0]);
    free(bloque_cifrado);
}

// Implementación del modo OFB (Output Feedback)
void modo_OFB(int **imagen, int **llave, int **IV, int **imagen_cifrada, int filas, int columnas_llave, int mod)
{
    int i, j;
    int **feedback = (int **)malloc(1 * sizeof(int *));
    feedback[0] = (int *)malloc(columnas_llave * sizeof(int));

    int **cifrado_feedback = (int **)malloc(1 * sizeof(int *));
    cifrado_feedback[0] = (int *)malloc(columnas_llave * sizeof(int));

    // Copiar IV al feedback inicial
    for (j = 0; j < columnas_llave; j++)
    {
        feedback[0][j] = IV[0][j];
    }

    // Para cada bloque
    for (i = 0; i < filas; i++)
    {
        // Cifrar el feedback
        multiplicar_matrices(feedback, llave, cifrado_feedback, 1, columnas_llave, columnas_llave, mod);

        // XOR del resultado con el bloque de texto plano
        for (j = 0; j < columnas_llave; j++)
        {
            imagen_cifrada[i][j] = (imagen[i][j] ^ cifrado_feedback[0][j]) % mod;
        }

        // El feedback se actualiza con el cifrado del feedback
        for (j = 0; j < columnas_llave; j++)
        {
            feedback[0][j] = cifrado_feedback[0][j];
        }
    }

    // Liberar memoria
    free(feedback[0]);
    free(feedback);
    free(cifrado_feedback[0]);
    free(cifrado_feedback);
}

// Función para incrementar el contador en el modo CTR
void incrementar_contador(int **contador, int filas, int columnas, int mod)
{
    int i = filas - 1;
    int j = columnas - 1;

    // Incrementar desde el último elemento
    while (i >= 0)
    {
        while (j >= 0)
        {
            contador[i][j] = (contador[i][j] + 1) % mod;
            // Si no hay acarreo, terminamos
            if (contador[i][j] != 0)
            {
                return;
            }
            j--;
        }
        j = columnas - 1;
        i--;
    }
}

// Implementación del modo CTR (Counter)
void modo_CTR(int **imagen, int **llave, int **IV, int **imagen_cifrada, int filas, int columnas_llave, int mod)
{
    int i, j;
    int **contador = (int **)malloc(1 * sizeof(int *));
    contador[0] = (int *)malloc(columnas_llave * sizeof(int));

    int **contador_cifrado = (int **)malloc(1 * sizeof(int *));
    contador_cifrado[0] = (int *)malloc(columnas_llave * sizeof(int));

    // Inicializar contador con IV
    for (j = 0; j < columnas_llave; j++)
    {
        contador[0][j] = IV[0][j];
    }

    // Para cada bloque
    for (i = 0; i < filas; i++)
    {
        // Cifrar el contador
        multiplicar_matrices(contador, llave, contador_cifrado, 1, columnas_llave, columnas_llave, mod);

        // XOR del resultado con el bloque de texto plano
        for (j = 0; j < columnas_llave; j++)
        {
            imagen_cifrada[i][j] = (imagen[i][j] ^ contador_cifrado[0][j]) % mod;
        }

        // Incrementar el contador
        incrementar_contador(contador, 1, columnas_llave, mod);
    }

    // Liberar memoria
    free(contador[0]);
    free(contador);
    free(contador_cifrado[0]);
    free(contador_cifrado);
}