#include <stdio.h>
#include <stdint.h>
#include <stdlib.h>
#include <string.h>

// DES processes 64-bit blocks using a 56-bit key
#define DES_BLOCK_SIZE 64
#define DES_KEY_SIZE 56
#define DES_SUBKEY_SIZE 48
#define DES_NUM_ROUNDS 16
#define DES_NUM_SBOXES 8

/* Initial Permutation Table */
static char IP[] = {
    58, 50, 42, 34, 26, 18, 10, 2,
    60, 52, 44, 36, 28, 20, 12, 4,
    62, 54, 46, 38, 30, 22, 14, 6,
    64, 56, 48, 40, 32, 24, 16, 8,
    57, 49, 41, 33, 25, 17, 9, 1,
    59, 51, 43, 35, 27, 19, 11, 3,
    61, 53, 45, 37, 29, 21, 13, 5,
    63, 55, 47, 39, 31, 23, 15, 7};

/* Inverse Initial Permutation Table */
static char PI[] = {
    40, 8, 48, 16, 56, 24, 64, 32,
    39, 7, 47, 15, 55, 23, 63, 31,
    38, 6, 46, 14, 54, 22, 62, 30,
    37, 5, 45, 13, 53, 21, 61, 29,
    36, 4, 44, 12, 52, 20, 60, 28,
    35, 3, 43, 11, 51, 19, 59, 27,
    34, 2, 42, 10, 50, 18, 58, 26,
    33, 1, 41, 9, 49, 17, 57, 25};

/* E Bit - selection table */
static char E[] = {
    32, 1, 2, 3, 4, 5,
    4, 5, 6, 7, 8, 9,
    8, 9, 10, 11, 12, 13,
    12, 13, 14, 15, 16, 17,
    16, 17, 18, 19, 20, 21,
    20, 21, 22, 23, 24, 25,
    24, 25, 26, 27, 28, 29,
    28, 29, 30, 31, 32, 1};

/* Post S-Box permutation */
static char P[] = {
    16, 7, 20, 21,
    29, 12, 28, 17,
    1, 15, 23, 26,
    5, 18, 31, 10,
    2, 8, 24, 14,
    32, 27, 3, 9,
    19, 13, 30, 6,
    22, 11, 4, 25};

/* The S-Box tables */
static char S[8][64] = {{/* S1 */
                         14, 4, 13, 1, 2, 15, 11, 8, 3, 10, 6, 12, 5, 9, 0, 7,
                         0, 15, 7, 4, 14, 2, 13, 1, 10, 6, 12, 11, 9, 5, 3, 8,
                         4, 1, 14, 8, 13, 6, 2, 11, 15, 12, 9, 7, 3, 10, 5, 0,
                         15, 12, 8, 2, 4, 9, 1, 7, 5, 11, 3, 14, 10, 0, 6, 13},
                        {/* S2 */
                         15, 1, 8, 14, 6, 11, 3, 4, 9, 7, 2, 13, 12, 0, 5, 10,
                         3, 13, 4, 7, 15, 2, 8, 14, 12, 0, 1, 10, 6, 9, 11, 5,
                         0, 14, 7, 11, 10, 4, 13, 1, 5, 8, 12, 6, 9, 3, 2, 15,
                         13, 8, 10, 1, 3, 15, 4, 2, 11, 6, 7, 12, 0, 5, 14, 9},
                        {/* S3 */
                         10, 0, 9, 14, 6, 3, 15, 5, 1, 13, 12, 7, 11, 4, 2, 8,
                         13, 7, 0, 9, 3, 4, 6, 10, 2, 8, 5, 14, 12, 11, 15, 1,
                         13, 6, 4, 9, 8, 15, 3, 0, 11, 1, 2, 12, 5, 10, 14, 7,
                         1, 10, 13, 0, 6, 9, 8, 7, 4, 15, 14, 3, 11, 5, 2, 12},
                        {/* S4 */
                         7, 13, 14, 3, 0, 6, 9, 10, 1, 2, 8, 5, 11, 12, 4, 15,
                         13, 8, 11, 5, 6, 15, 0, 3, 4, 7, 2, 12, 1, 10, 14, 9,
                         10, 6, 9, 0, 12, 11, 7, 13, 15, 1, 3, 14, 5, 2, 8, 4,
                         3, 15, 0, 6, 10, 1, 13, 8, 9, 4, 5, 11, 12, 7, 2, 14},
                        {/* S5 */
                         2, 12, 4, 1, 7, 10, 11, 6, 8, 5, 3, 15, 13, 0, 14, 9,
                         14, 11, 2, 12, 4, 7, 13, 1, 5, 0, 15, 10, 3, 9, 8, 6,
                         4, 2, 1, 11, 10, 13, 7, 8, 15, 9, 12, 5, 6, 3, 0, 14,
                         11, 8, 12, 7, 1, 14, 2, 13, 6, 15, 0, 9, 10, 4, 5, 3},
                        {/* S6 */
                         12, 1, 10, 15, 9, 2, 6, 8, 0, 13, 3, 4, 14, 7, 5, 11,
                         10, 15, 4, 2, 7, 12, 9, 5, 6, 1, 13, 14, 0, 11, 3, 8,
                         9, 14, 15, 5, 2, 8, 12, 3, 7, 0, 4, 10, 1, 13, 11, 6,
                         4, 3, 2, 12, 9, 5, 15, 10, 11, 14, 1, 7, 6, 0, 8, 13},
                        {/* S7 */
                         4, 11, 2, 14, 15, 0, 8, 13, 3, 12, 9, 7, 5, 10, 6, 1,
                         13, 0, 11, 7, 4, 9, 1, 10, 14, 3, 5, 12, 2, 15, 8, 6,
                         1, 4, 11, 13, 12, 3, 7, 14, 10, 15, 6, 8, 0, 5, 9, 2,
                         6, 11, 13, 8, 1, 4, 10, 7, 9, 5, 0, 15, 14, 2, 3, 12},
                        {/* S8 */
                         13, 2, 8, 4, 6, 15, 11, 1, 10, 9, 3, 14, 5, 0, 12, 7,
                         1, 15, 13, 8, 10, 3, 7, 4, 12, 5, 6, 11, 0, 14, 9, 2,
                         7, 11, 4, 1, 9, 12, 14, 2, 0, 6, 10, 13, 15, 3, 5, 8,
                         2, 1, 14, 7, 4, 10, 8, 13, 15, 12, 9, 0, 3, 5, 6, 11}};

/* Permuted Choice 1 Table */
static char PC1[] = {
    57, 49, 41, 33, 25, 17, 9,
    1, 58, 50, 42, 34, 26, 18,
    10, 2, 59, 51, 43, 35, 27,
    19, 11, 3, 60, 52, 44, 36,

    63, 55, 47, 39, 31, 23, 15,
    7, 62, 54, 46, 38, 30, 22,
    14, 6, 61, 53, 45, 37, 29,
    21, 13, 5, 28, 20, 12, 4};

/* Permuted Choice 2 Table */
static char PC2[] = {
    14, 17, 11, 24, 1, 5,
    3, 28, 15, 6, 21, 10,
    23, 19, 12, 4, 26, 8,
    16, 7, 27, 20, 13, 2,
    41, 52, 31, 37, 47, 55,
    30, 40, 51, 45, 33, 48,
    44, 49, 39, 56, 34, 53,
    46, 42, 50, 36, 29, 32};

/* Iteration Shift Array */
static char iteration_shift[] = {
    /* 1   2   3   4   5   6   7   8   9  10  11  12  13  14  15  16 */
    1, 1, 2, 2, 2, 2, 2, 2, 1, 2, 2, 2, 2, 2, 2, 1};

// Function prototypes
// Key generation and processing
uint64_t string_to_binary(const char *str);
size_t get_num_blocks(size_t message_length);
void string_to_blocks(const char *input, uint64_t *blocks, size_t num_blocks);
uint64_t apply_permutation(uint64_t input, const char *table, int input_len, int output_len);
uint32_t left_rotate_28(uint32_t value, int shift_count);
void generate_subkeys(uint64_t key, uint64_t *subkeys);

// DES core functions
uint64_t des_encrypt(uint64_t block, uint64_t *subkeys);
uint64_t des_decrypt(uint64_t block, uint64_t *subkeys);
uint32_t f_function(uint32_t r, uint64_t subkey);
uint32_t s_boxes(uint64_t expanded_xor);

// Utility functions
void print_binary(uint64_t num, int bits);
void print_hex(uint64_t num);
void print_hex_bytes(uint64_t num); // New standardized hex byte format
int parse_hex_blocks(const char *hex_string, uint64_t **blocks, size_t *num_blocks);

int main()
{
    char input_text[1024];
    char key[1024];
    int operation;

    // Get operation choice from user
    printf("Choose operation:\n");
    printf("1. Encrypt\n");
    printf("2. Decrypt\n");
    printf("Enter choice (1 or 2): ");
    scanf("%d", &operation);
    getchar(); // Clear the newline from buffer

    if (operation != 1 && operation != 2)
    {
        printf("Invalid choice. Please enter 1 for encryption or 2 for decryption.\n");
        return 1;
    }

    // Read input text based on operation
    if (operation == 1)
    {
        printf("Enter plaintext: ");
    }
    else
    {
        printf("Enter ciphertext (as hex bytes, e.g., AA BB CC DD 11 22 33 44): ");
    }
    scanf("%1023[^\n]", input_text);
    getchar(); // Clear the newline from buffer

    // Read key using scanf with field width limit
    printf("Enter key (8 characters): ");
    scanf("%8[^\n]", key);
    getchar(); // Clear the newline from buffer

    // Process key
    uint64_t key_block = string_to_binary(key);

    // Print key
    printf("\nKey Block:\n");
    printf("Binary: ");
    print_binary(key_block, 64);
    printf("Hex: ");
    print_hex(key_block);

    // Generate the 16 round subkeys
    uint64_t subkeys[16];
    generate_subkeys(key_block, subkeys);

    // Print all subkeys
    printf("\nSubkeys:\n");
    for (int i = 0; i < 16; i++)
    {
        printf("\nSubkey %d:\n", i + 1);
        printf("Binary: ");
        print_binary(subkeys[i], 48);
        printf("Hex: ");
        print_hex(subkeys[i]);
    }

    if (operation == 1)
    {
        // ENCRYPTION
        printf("\n=== ENCRYPTION MODE ===\n");

        // Calculate number of blocks needed for plaintext
        size_t num_blocks = get_num_blocks(strlen(input_text));
        uint64_t *plaintext_blocks = malloc(num_blocks * sizeof(uint64_t));

        // Convert plaintext to blocks
        string_to_blocks(input_text, plaintext_blocks, num_blocks);

        // Print all plaintext blocks
        printf("\nPlaintext Blocks:\n");
        for (size_t i = 0; i < num_blocks; i++)
        {
            printf("\nBlock %zu:\n", i + 1);
            printf("Binary: ");
            print_binary(plaintext_blocks[i], 64);
            printf("Hex: ");
            print_hex(plaintext_blocks[i]);
        }

        // Encrypt each block
        uint64_t *ciphertext_blocks = malloc(num_blocks * sizeof(uint64_t));

        printf("\nEncryption Process:\n");
        for (size_t i = 0; i < num_blocks; i++)
        {
            printf("\nEncrypting Block %zu:\n", i + 1);
            ciphertext_blocks[i] = des_encrypt(plaintext_blocks[i], subkeys);

            printf("\nCiphertext Block %zu:\n", i + 1);
            printf("Binary: ");
            print_binary(ciphertext_blocks[i], 64);
            printf("Hex: ");
            print_hex(ciphertext_blocks[i]);
        }

        printf("\n=== FINAL ENCRYPTED RESULT ===\n");
        for (size_t i = 0; i < num_blocks; i++)
        {
            printf("Block %zu: ", i + 1);
            print_hex_bytes(ciphertext_blocks[i]);
        }

        free(plaintext_blocks);
        free(ciphertext_blocks);
    }
    else
    {
        // DECRYPTION
        printf("\n=== DECRYPTION MODE ===\n");

        // Parse hex input - can handle multiple blocks separated by spaces
        uint64_t *ciphertext_blocks;
        size_t num_blocks;

        if (!parse_hex_blocks(input_text, &ciphertext_blocks, &num_blocks))
        {
            printf("Error: Invalid hex format. Please enter hex like: AA BB CC DD 11 22 33 44 or AABBCCDD11223344\n");
            return 1;
        }

        printf("\nCiphertext Blocks:\n");
        for (size_t i = 0; i < num_blocks; i++)
        {
            printf("\nBlock %zu:\n", i + 1);
            printf("Binary: ");
            print_binary(ciphertext_blocks[i], 64);
            printf("Hex: ");
            print_hex(ciphertext_blocks[i]);
        }

        // Decrypt each block
        printf("\nDecryption Process:\n");
        char *decrypted_text = malloc((num_blocks * 8 + 1) * sizeof(char));

        for (size_t i = 0; i < num_blocks; i++)
        {
            printf("\nDecrypting Block %zu:\n", i + 1);
            uint64_t decrypted = des_decrypt(ciphertext_blocks[i], subkeys);

            printf("\nDecrypted Block %zu:\n", i + 1);
            printf("Binary: ");
            print_binary(decrypted, 64);
            printf("Hex: ");
            print_hex(decrypted);

            // Convert back to characters
            for (int j = 0; j < 8; j++)
            {
                decrypted_text[i * 8 + j] = (decrypted >> (56 - j * 8)) & 0xFF;
            }
        }
        decrypted_text[num_blocks * 8] = '\0';

        printf("\n=== FINAL DECRYPTED RESULT ===\n");
        printf("Decrypted text: '%s'\n", decrypted_text);

        free(ciphertext_blocks);
        free(decrypted_text);
    }
    return 0;
}

void print_binary(uint64_t num, int bits)
{
    for (int i = bits - 1; i >= 0; i--)
    {
        printf("%d", (num >> i) & 1);
        if (i % 8 == 0)
            printf(" "); // space every 8 bits
    }
    printf("\n");
}

void print_hex(uint64_t num)
{
    printf("0x%016lX\n", num);
}

// Print 64-bit number as space-separated hex bytes (standardized format)
void print_hex_bytes(uint64_t num)
{
    for (int i = 7; i >= 0; i--)
    {
        printf("%02X", (uint8_t)((num >> (i * 8)) & 0xFF));
        if (i > 0)
            printf(" ");
    }
    printf("\n");
}

// Convert string to 64-bit blocks
uint64_t string_to_binary(const char *str)
{
    uint64_t binary = 0;
    for (int i = 0; i < 8 && str[i] != '\0'; i++)
    {
        binary = (binary << 8) | str[i];
    }
    return binary;
}

// Calculate number of 64-bit blocks needed
size_t get_num_blocks(size_t message_length)
{
    return (message_length + 7) / 8; // Round up division by 8
}

// Process string into blocks
void string_to_blocks(const char *input, uint64_t *blocks, size_t num_blocks)
{
    size_t input_len = strlen(input);

    for (size_t i = 0; i < num_blocks; i++)
    {
        blocks[i] = 0;
        for (size_t j = 0; j < 8; j++)
        {
            size_t pos = i * 8 + j;
            // If we still have input, use it; otherwise pad with spaces
            unsigned char byte = (pos < input_len) ? input[pos] : ' ';
            blocks[i] = (blocks[i] << 8) | byte;
        }
    }
}

// Apply a permutation to bits according to the provided table
uint64_t apply_permutation(uint64_t input, const char *table, int input_len, int output_len)
{
    uint64_t output = 0;

    for (int i = 0; i < output_len; i++)
    {
        // Permutation tables are 1-indexed, so subtract 1
        int pos = table[i] - 1;

        // Get the bit at position pos from input
        uint64_t bit = (input >> (input_len - 1 - pos)) & 1;

        // Place it at position i in output
        output |= (bit << (output_len - 1 - i));
    }

    return output;
}

// Left rotate a 28-bit value by shift_count positions
uint32_t left_rotate_28(uint32_t value, int shift_count)
{
    // Ensure value is 28 bits
    value &= 0x0FFFFFFF;

    // Perform rotation
    uint32_t left_part = (value << shift_count) & 0x0FFFFFFF;
    uint32_t right_part = value >> (28 - shift_count);

    return left_part | right_part;
}

// Generate all 16 subkeys for DES rounds
void generate_subkeys(uint64_t key, uint64_t *subkeys)
{
    // Apply PC1 permutation (64 bits -> 56 bits)
    uint64_t permuted_key = apply_permutation(key, PC1, 64, 56);

    // Split into 28-bit halves C0 and D0
    uint32_t C = (uint32_t)((permuted_key >> 28) & 0x0FFFFFFF);
    uint32_t D = (uint32_t)(permuted_key & 0x0FFFFFFF);

    printf("\nAfter PC1:\n");
    printf("C0: ");
    print_binary(C, 28);
    printf("D0: ");
    print_binary(D, 28);

    // Generate the 16 subkeys
    for (int i = 0; i < 16; i++)
    {
        // Apply rotation based on iteration_shift
        C = left_rotate_28(C, iteration_shift[i]);
        D = left_rotate_28(D, iteration_shift[i]);

        printf("\nAfter rotation %d:\n", i + 1);
        printf("C%d: ", i + 1);
        print_binary(C, 28);
        printf("D%d: ", i + 1);
        print_binary(D, 28);

        // Combine C and D
        uint64_t combined = ((uint64_t)C << 28) | D;

        // Apply PC2 (56 bits -> 48 bits) to get the subkey
        subkeys[i] = apply_permutation(combined, PC2, 56, 48);
    }
}

// The main DES encryption function
uint64_t des_encrypt(uint64_t block, uint64_t *subkeys)
{
    printf("\n--- ENCRYPTION PROCESS ---\n");
    printf("Step 1: Apply initial permutation (IP)\n");
    // Apply initial permutation
    uint64_t permuted = apply_permutation(block, IP, 64, 64);
    printf("After initial permutation: ");
    print_hex(permuted);

    // Split into left and right halves
    uint32_t L = (uint32_t)(permuted >> 32) & 0xFFFFFFFF;
    uint32_t R = (uint32_t)(permuted & 0xFFFFFFFF);

    printf("\nStep 2: Split into L0 and R0 (32 bits each)\n");
    printf("L0: ");
    print_binary(L, 32);
    printf("R0: ");
    print_binary(R, 32);

    // Apply 16 rounds of encryption
    for (int i = 0; i < 16; i++)
    {
        printf("\nRound %d:\n", i + 1);

        uint32_t prev_L = L;

        // Print the operation first
        printf("Operation: L%d = R%d\n", i + 1, i);
        L = R; // L[i] = R[i-1]
        printf("L%d: ", i + 1);
        print_binary(L, 32);

        // Print and calculate the f-function result
        printf("Operation: Calculate f(R%d, K%d)\n", i, i + 1);
        uint32_t f_result = f_function(R, subkeys[i]);
        printf("f(R%d, K%d) result: ", i, i + 1);
        print_binary(f_result, 32);

        // Print XOR operation and result
        printf("Operation: R%d = L%d XOR f(R%d, K%d)\n", i + 1, i, i, i + 1);
        R = prev_L ^ f_result; // R[i] = L[i-1] XOR f(R[i-1], K[i])
        printf("R%d: ", i + 1);
        print_binary(R, 32);
    }

    // Final swap and inverse permutation
    printf("\nStep 4: Final swap - combine R16 and L16 (note the swap)\n");
    uint64_t pre_output = ((uint64_t)R << 32) | L; // Note the swap here
    printf("After final swap (R16|L16): ");
    print_hex(pre_output);

    printf("\nStep 5: Apply inverse permutation (IP^-1)\n");
    uint64_t output = apply_permutation(pre_output, PI, 64, 64);
    printf("After inverse permutation: ");
    print_hex(output);

    return output;
}

// The main DES decryption function - same as encrypt but with subkeys in reverse order
uint64_t des_decrypt(uint64_t block, uint64_t *subkeys)
{
    printf("\n--- DECRYPTION PROCESS ---\n");
    printf("Step 1: Apply initial permutation (IP)\n");
    // Apply initial permutation
    uint64_t permuted = apply_permutation(block, IP, 64, 64);
    printf("After initial permutation: ");
    print_hex(permuted);

    // Split into left and right halves
    uint32_t L = (uint32_t)(permuted >> 32) & 0xFFFFFFFF;
    uint32_t R = (uint32_t)(permuted & 0xFFFFFFFF);

    printf("\nStep 2: Split into L0 and R0 (32 bits each)\n");
    printf("L0: ");
    print_binary(L, 32);
    printf("R0: ");
    print_binary(R, 32);

    // Apply 16 rounds of decryption (using subkeys in reverse)
    for (int i = 0; i < 16; i++)
    {
        printf("\nRound %d (using K%d):\n", i + 1, 16 - i);

        uint32_t prev_L = L;

        // Print the operation first
        printf("Operation: L%d = R%d\n", i + 1, i);
        L = R;
        printf("L%d: ", i + 1);
        print_binary(L, 32);

        // Print and calculate the f-function result
        printf("Operation: Calculate f(R%d, K%d)\n", i, 16 - i);
        uint32_t f_result = f_function(R, subkeys[15 - i]);
        printf("f(R%d, K%d) result: ", i, 16 - i);
        print_binary(f_result, 32);

        // Print XOR operation and result
        printf("Operation: R%d = L%d XOR f(R%d, K%d)\n", i + 1, i, i, 16 - i);
        R = prev_L ^ f_result; // R[i] = L[i-1] XOR f(R[i-1], K[16-i])
        printf("R%d: ", i + 1);
        print_binary(R, 32);
    }

    // Final swap and inverse permutation
    printf("\nStep 4: Final swap - combine R16 and L16 (note the swap)\n");
    uint64_t pre_output = ((uint64_t)R << 32) | L;
    printf("After final swap (R16|L16): ");
    print_hex(pre_output);

    printf("\nStep 5: Apply inverse permutation (IP^-1)\n");
    uint64_t output = apply_permutation(pre_output, PI, 64, 64);
    printf("After inverse permutation: ");
    print_hex(output);

    return output;
}

// The f-function: the heart of DES
uint32_t f_function(uint32_t r, uint64_t subkey)
{
    printf("    f-function operations:\n");

    printf("    1. Expansion: 32-bit R -> 48-bit using E-table\n");
    // 1. Expansion: expand 32-bit R to 48-bit using E-table
    uint64_t expanded = apply_permutation(r, E, 32, 48);
    printf("    Expanded R: ");
    print_binary(expanded, 48);
    printf("    Subkey: ");
    print_binary(subkey, 48);

    printf("    2. XOR: 48-bit expanded R XOR 48-bit subkey\n");
    // 2. XOR with subkey
    uint64_t expanded_xor = expanded ^ subkey;
    printf("    After XOR with subkey: ");
    print_binary(expanded_xor, 48);

    printf("    3. S-box substitution: 48-bit -> 32-bit\n");
    // 3. S-box substitution: 48-bit to 32-bit
    uint32_t s_output = s_boxes(expanded_xor);
    printf("    After S-box substitution: ");
    print_binary(s_output, 32);

    printf("    4. P-box permutation: Rearrange 32-bit S-box output\n");
    // 4. Permutation using P table
    uint32_t p_output = apply_permutation(s_output, P, 32, 32);
    printf("    After P permutation: ");
    print_binary(p_output, 32);

    return p_output;
}

// Apply S-box substitution to convert 48 bits to 32 bits
uint32_t s_boxes(uint64_t expanded_xor)
{
    uint32_t result = 0;

    // Process 8 groups of 6 bits each
    for (int i = 0; i < 8; i++)
    {
        // Extract 6-bit block
        int sixBit = (expanded_xor >> (42 - i * 6)) & 0x3F;

        // Compute row and column
        int row = ((sixBit & 0x20) >> 4) | (sixBit & 0x01); // bits 0 and 5
        int col = (sixBit >> 1) & 0x0F;                     // bits 1-4

        // Look up value in S-box
        int val = S[i][row * 16 + col];

        // Add to result
        result |= (val << (28 - i * 4));

        printf("    S%d: 6-bit input: %02X, row: %d, col: %d, output: %X\n",
               i + 1, sixBit, row, col, val);
    }

    return result;
}

// Parse hex string into blocks for decryption
// Supports both formats: "AABBCCDD11223344" or "AA BB CC DD 11 22 33 44"
int parse_hex_blocks(const char *hex_string, uint64_t **blocks, size_t *num_blocks)
{
    char *input_copy = strdup(hex_string);
    char *token;
    size_t byte_count = 0;
    uint8_t *bytes = NULL;

    // First pass: count hex bytes
    char *temp_copy = strdup(hex_string);
    token = strtok(temp_copy, " ,\t\n");
    while (token != NULL)
    {
        size_t token_len = strlen(token);
        if (token_len == 2)
        {
            // Individual byte format: "AA BB CC DD"
            byte_count++;
        }
        else if (token_len == 16)
        {
            // 64-bit hex format: "AABBCCDD11223344"
            byte_count += 8;
        }
        else
        {
            printf("Error: Invalid hex format '%s'. Use either 2-digit bytes (AA BB) or 16-digit blocks (AABBCCDD11223344).\n", token);
            free(temp_copy);
            free(input_copy);
            return 0;
        }
        token = strtok(NULL, " ,\t\n");
    }
    free(temp_copy);

    if (byte_count == 0)
    {
        free(input_copy);
        return 0;
    }

    // Allocate temporary byte array
    bytes = malloc(byte_count);
    size_t byte_idx = 0;

    // Second pass: parse hex values
    token = strtok(input_copy, " ,\t\n");
    while (token != NULL && byte_idx < byte_count)
    {
        size_t token_len = strlen(token);
        if (token_len == 2)
        {
            // Parse single byte
            unsigned int hex_byte;
            if (sscanf(token, "%2x", &hex_byte) != 1)
            {
                printf("Error parsing hex byte: %s\n", token);
                free(input_copy);
                free(bytes);
                return 0;
            }
            bytes[byte_idx++] = (uint8_t)hex_byte;
        }
        else if (token_len == 16)
        {
            // Parse 64-bit value and convert to bytes
            uint64_t hex_block;
            if (sscanf(token, "%lx", &hex_block) != 1)
            {
                printf("Error parsing hex block: %s\n", token);
                free(input_copy);
                free(bytes);
                return 0;
            }
            // Convert 64-bit value to 8 bytes (big-endian)
            for (int i = 7; i >= 0; i--)
            {
                bytes[byte_idx++] = (uint8_t)((hex_block >> (i * 8)) & 0xFF);
            }
        }
        token = strtok(NULL, " ,\t\n");
    }

    // Convert bytes to 64-bit blocks
    *num_blocks = (byte_count + 7) / 8; // Round up to complete blocks
    *blocks = malloc(*num_blocks * sizeof(uint64_t));

    for (size_t i = 0; i < *num_blocks; i++)
    {
        (*blocks)[i] = 0;
        for (int j = 0; j < 8; j++)
        {
            size_t byte_pos = i * 8 + j;
            if (byte_pos < byte_count)
            {
                (*blocks)[i] = ((*blocks)[i] << 8) | bytes[byte_pos];
            }
            else
            {
                (*blocks)[i] = (*blocks)[i] << 8; // Pad with zeros
            }
        }
    }

    free(input_copy);
    free(bytes);
    return 1;
}