#include <stdio.h>
#include <stdint.h>
#include <stdlib.h>
#include <string.h>

// AES processes 128-bit blocks using a 128/192/256-bit key
#define AES_BLOCK_SIZE 16 // 128 bits = 16 bytes
#define AES_STATE_ROWS 4
#define AES_STATE_COLS 4
#define AES_128_KEY_SIZE 16 // 128 bits = 16 bytes
#define AES_128_ROUNDS 10   // 10 rounds for 128-bit key

// Forward S-box for SubBytes transformation
static const uint8_t s_box[256] = {
    0x63, 0x7c, 0x77, 0x7b, 0xf2, 0x6b, 0x6f, 0xc5, 0x30, 0x01, 0x67, 0x2b, 0xfe, 0xd7, 0xab, 0x76,
    0xca, 0x82, 0xc9, 0x7d, 0xfa, 0x59, 0x47, 0xf0, 0xad, 0xd4, 0xa2, 0xaf, 0x9c, 0xa4, 0x72, 0xc0,
    0xb7, 0xfd, 0x93, 0x26, 0x36, 0x3f, 0xf7, 0xcc, 0x34, 0xa5, 0xe5, 0xf1, 0x71, 0xd8, 0x31, 0x15,
    0x04, 0xc7, 0x23, 0xc3, 0x18, 0x96, 0x05, 0x9a, 0x07, 0x12, 0x80, 0xe2, 0xeb, 0x27, 0xb2, 0x75,
    0x09, 0x83, 0x2c, 0x1a, 0x1b, 0x6e, 0x5a, 0xa0, 0x52, 0x3b, 0xd6, 0xb3, 0x29, 0xe3, 0x2f, 0x84,
    0x53, 0xd1, 0x00, 0xed, 0x20, 0xfc, 0xb1, 0x5b, 0x6a, 0xcb, 0xbe, 0x39, 0x4a, 0x4c, 0x58, 0xcf,
    0xd0, 0xef, 0xaa, 0xfb, 0x43, 0x4d, 0x33, 0x85, 0x45, 0xf9, 0x02, 0x7f, 0x50, 0x3c, 0x9f, 0xa8,
    0x51, 0xa3, 0x40, 0x8f, 0x92, 0x9d, 0x38, 0xf5, 0xbc, 0xb6, 0xda, 0x21, 0x10, 0xff, 0xf3, 0xd2,
    0xcd, 0x0c, 0x13, 0xec, 0x5f, 0x97, 0x44, 0x17, 0xc4, 0xa7, 0x7e, 0x3d, 0x64, 0x5d, 0x19, 0x73,
    0x60, 0x81, 0x4f, 0xdc, 0x22, 0x2a, 0x90, 0x88, 0x46, 0xee, 0xb8, 0x14, 0xde, 0x5e, 0x0b, 0xdb,
    0xe0, 0x32, 0x3a, 0x0a, 0x49, 0x06, 0x24, 0x5c, 0xc2, 0xd3, 0xac, 0x62, 0x91, 0x95, 0xe4, 0x79,
    0xe7, 0xc8, 0x37, 0x6d, 0x8d, 0xd5, 0x4e, 0xa9, 0x6c, 0x56, 0xf4, 0xea, 0x65, 0x7a, 0xae, 0x08,
    0xba, 0x78, 0x25, 0x2e, 0x1c, 0xa6, 0xb4, 0xc6, 0xe8, 0xdd, 0x74, 0x1f, 0x4b, 0xbd, 0x8b, 0x8a,
    0x70, 0x3e, 0xb5, 0x66, 0x48, 0x03, 0xf6, 0x0e, 0x61, 0x35, 0x57, 0xb9, 0x86, 0xc1, 0x1d, 0x9e,
    0xe1, 0xf8, 0x98, 0x11, 0x69, 0xd9, 0x8e, 0x94, 0x9b, 0x1e, 0x87, 0xe9, 0xce, 0x55, 0x28, 0xdf,
    0x8c, 0xa1, 0x89, 0x0d, 0xbf, 0xe6, 0x42, 0x68, 0x41, 0x99, 0x2d, 0x0f, 0xb0, 0x54, 0xbb, 0x16};

// Inverse S-box for InvSubBytes transformation
static const uint8_t inv_s_box[256] = {
    0x52, 0x09, 0x6a, 0xd5, 0x30, 0x36, 0xa5, 0x38, 0xbf, 0x40, 0xa3, 0x9e, 0x81, 0xf3, 0xd7, 0xfb,
    0x7c, 0xe3, 0x39, 0x82, 0x9b, 0x2f, 0xff, 0x87, 0x34, 0x8e, 0x43, 0x44, 0xc4, 0xde, 0xe9, 0xcb,
    0x54, 0x7b, 0x94, 0x32, 0xa6, 0xc2, 0x23, 0x3d, 0xee, 0x4c, 0x95, 0x0b, 0x42, 0xfa, 0xc3, 0x4e,
    0x08, 0x2e, 0xa1, 0x66, 0x28, 0xd9, 0x24, 0xb2, 0x76, 0x5b, 0xa2, 0x49, 0x6d, 0x8b, 0xd1, 0x25,
    0x72, 0xf8, 0xf6, 0x64, 0x86, 0x68, 0x98, 0x16, 0xd4, 0xa4, 0x5c, 0xcc, 0x5d, 0x65, 0xb6, 0x92,
    0x6c, 0x70, 0x48, 0x50, 0xfd, 0xed, 0xb9, 0xda, 0x5e, 0x15, 0x46, 0x57, 0xa7, 0x8d, 0x9d, 0x84,
    0x90, 0xd8, 0xab, 0x00, 0x8c, 0xbc, 0xd3, 0x0a, 0xf7, 0xe4, 0x58, 0x05, 0xb8, 0xb3, 0x45, 0x06,
    0xd0, 0x2c, 0x1e, 0x8f, 0xca, 0x3f, 0x0f, 0x02, 0xc1, 0xaf, 0xbd, 0x03, 0x01, 0x13, 0x8a, 0x6b,
    0x3a, 0x91, 0x11, 0x41, 0x4f, 0x67, 0xdc, 0xea, 0x97, 0xf2, 0xcf, 0xce, 0xf0, 0xb4, 0xe6, 0x73,
    0x96, 0xac, 0x74, 0x22, 0xe7, 0xad, 0x35, 0x85, 0xe2, 0xf9, 0x37, 0xe8, 0x1c, 0x75, 0xdf, 0x6e,
    0x47, 0xf1, 0x1a, 0x71, 0x1d, 0x29, 0xc5, 0x89, 0x6f, 0xb7, 0x62, 0x0e, 0xaa, 0x18, 0xbe, 0x1b,
    0xfc, 0x56, 0x3e, 0x4b, 0xc6, 0xd2, 0x79, 0x20, 0x9a, 0xdb, 0xc0, 0xfe, 0x78, 0xcd, 0x5a, 0xf4,
    0x1f, 0xdd, 0xa8, 0x33, 0x88, 0x07, 0xc7, 0x31, 0xb1, 0x12, 0x10, 0x59, 0x27, 0x80, 0xec, 0x5f,
    0x60, 0x51, 0x7f, 0xa9, 0x19, 0xb5, 0x4a, 0x0d, 0x2d, 0xe5, 0x7a, 0x9f, 0x93, 0xc9, 0x9c, 0xef,
    0xa0, 0xe0, 0x3b, 0x4d, 0xae, 0x2a, 0xf5, 0xb0, 0xc8, 0xeb, 0xbb, 0x3c, 0x83, 0x53, 0x99, 0x61,
    0x17, 0x2b, 0x04, 0x7e, 0xba, 0x77, 0xd6, 0x26, 0xe1, 0x69, 0x14, 0x63, 0x55, 0x21, 0x0c, 0x7d};

// Round constants for key expansion
static const uint8_t rcon[11] = {
    0x00, 0x01, 0x02, 0x04, 0x08, 0x10, 0x20, 0x40, 0x80, 0x1b, 0x36};

// Function prototypes
// Key expansion
void key_expansion(const uint8_t *key, uint8_t *expanded_key);

// AES core transformations
void sub_bytes(uint8_t *state);
void inv_sub_bytes(uint8_t *state);
void shift_rows(uint8_t *state);
void inv_shift_rows(uint8_t *state);
void mix_columns(uint8_t *state);
void inv_mix_columns(uint8_t *state);
void add_round_key(uint8_t *state, const uint8_t *round_key);

// Galois Field (GF(2^8)) multiplication
uint8_t gmul(uint8_t a, uint8_t b);

// AES encryption and decryption
void aes_encrypt_block(const uint8_t *input_block, uint8_t *output_block, const uint8_t *expanded_key);
void aes_decrypt_block(const uint8_t *input_block, uint8_t *output_block, const uint8_t *expanded_key);

// Utility functions
void string_to_bytes(const char *str, uint8_t *bytes, size_t len);
size_t get_num_blocks(size_t message_length);
void print_hex_block(const uint8_t *block, size_t len);
void print_state(const uint8_t *state);
void bytes_to_string(const uint8_t *bytes, char *str, size_t len);

int main()
{
    char plaintext[1024];
    char key[17]; // For AES-128, key is 16 bytes

    // Read plaintext
    printf("Enter plaintext: ");
    scanf("%1023[^\n]", plaintext);
    getchar(); // Clear the newline

    // Read key
    printf("Enter key (16 characters for AES-128): ");
    scanf("%16[^\n]", key);
    getchar(); // Clear the newline

    // Process key - convert to bytes
    uint8_t key_bytes[AES_128_KEY_SIZE];
    string_to_bytes(key, key_bytes, AES_128_KEY_SIZE);

    // Print key
    printf("\nKey:\n");
    print_hex_block(key_bytes, AES_128_KEY_SIZE);

    // Key expansion - generate round keys
    uint8_t expanded_key[AES_128_KEY_SIZE * (AES_128_ROUNDS + 1)]; // 11 round keys for AES-128
    key_expansion(key_bytes, expanded_key);

    // Print round keys
    printf("\nRound Keys:\n");
    for (int i = 0; i <= AES_128_ROUNDS; i++)
    {
        printf("Round %2d: ", i);
        print_hex_block(&expanded_key[i * AES_128_KEY_SIZE], AES_128_KEY_SIZE);
    }

    // Calculate number of blocks
    size_t plaintext_len = strlen(plaintext);
    size_t num_blocks = get_num_blocks(plaintext_len);

    // Allocate memory for blocks
    uint8_t **plaintext_blocks = (uint8_t **)malloc(num_blocks * sizeof(uint8_t *));
    uint8_t **ciphertext_blocks = (uint8_t **)malloc(num_blocks * sizeof(uint8_t *));
    uint8_t **decrypted_blocks = (uint8_t **)malloc(num_blocks * sizeof(uint8_t *));

    for (size_t i = 0; i < num_blocks; i++)
    {
        plaintext_blocks[i] = (uint8_t *)malloc(AES_BLOCK_SIZE);
        ciphertext_blocks[i] = (uint8_t *)malloc(AES_BLOCK_SIZE);
        decrypted_blocks[i] = (uint8_t *)malloc(AES_BLOCK_SIZE);

        // Initialize with zeros
        memset(plaintext_blocks[i], 0, AES_BLOCK_SIZE);
    }

    // Convert plaintext to blocks
    for (size_t i = 0; i < plaintext_len; i++)
    {
        size_t block_idx = i / AES_BLOCK_SIZE;
        size_t byte_idx = i % AES_BLOCK_SIZE;
        plaintext_blocks[block_idx][byte_idx] = plaintext[i];
    }

    // Print plaintext blocks
    printf("\nPlaintext Blocks:\n");
    for (size_t i = 0; i < num_blocks; i++)
    {
        printf("Block %zu:\n", i + 1);
        print_state(plaintext_blocks[i]);
    }

    // Encrypt each block
    printf("\nEncryption Process:\n");
    for (size_t i = 0; i < num_blocks; i++)
    {
        printf("\nEncrypting Block %zu:\n", i + 1);
        aes_encrypt_block(plaintext_blocks[i], ciphertext_blocks[i], expanded_key);

        printf("Ciphertext Block %zu:\n", i + 1);
        print_state(ciphertext_blocks[i]);
    }

    // Decrypt each block
    printf("\nDecryption Process:\n");
    for (size_t i = 0; i < num_blocks; i++)
    {
        printf("\nDecrypting Block %zu:\n", i + 1);
        aes_decrypt_block(ciphertext_blocks[i], decrypted_blocks[i], expanded_key);

        printf("Decrypted Block %zu:\n", i + 1);
        print_state(decrypted_blocks[i]);

        // Convert back to characters and print
        char decrypted_text[AES_BLOCK_SIZE + 1];
        bytes_to_string(decrypted_blocks[i], decrypted_text, AES_BLOCK_SIZE);
        printf("Text: %s\n", decrypted_text);
    }

    // Free allocated memory
    for (size_t i = 0; i < num_blocks; i++)
    {
        free(plaintext_blocks[i]);
        free(ciphertext_blocks[i]);
        free(decrypted_blocks[i]);
    }
    free(plaintext_blocks);
    free(ciphertext_blocks);
    free(decrypted_blocks);

    return 0;
}

// Convert string to byte array
void string_to_bytes(const char *str, uint8_t *bytes, size_t len)
{
    size_t str_len = strlen(str);
    for (size_t i = 0; i < len; i++)
    {
        bytes[i] = (i < str_len) ? str[i] : 0; // Zero padding if str is shorter
    }
}

// Convert byte array back to string
void bytes_to_string(const uint8_t *bytes, char *str, size_t len)
{
    for (size_t i = 0; i < len; i++)
    {
        str[i] = bytes[i];
    }
    str[len] = '\0'; // Null-terminate
}

// Calculate number of blocks needed
size_t get_num_blocks(size_t message_length)
{
    return (message_length + AES_BLOCK_SIZE - 1) / AES_BLOCK_SIZE; // Round up division
}

// Print a block of bytes in hex format
void print_hex_block(const uint8_t *block, size_t len)
{
    for (size_t i = 0; i < len; i++)
    {
        printf("%02X ", block[i]);
    }
    printf("\n");
}

// Print the state matrix in a grid format
void print_state(const uint8_t *state)
{
    for (int i = 0; i < 4; i++)
    {
        for (int j = 0; j < 4; j++)
        {
            printf("%02X ", state[i + 4 * j]); // Column-major order
        }
        printf("\n");
    }
}

// Key expansion routine
void key_expansion(const uint8_t *key, uint8_t *expanded_key)
{
    // First round key is the original key
    memcpy(expanded_key, key, AES_128_KEY_SIZE);

    uint8_t temp[4];
    int i = 1;

    // Generate the remaining round keys
    while (i <= AES_128_ROUNDS)
    {
        // Last 4 bytes of previous round key
        for (int j = 0; j < 4; j++)
        {
            temp[j] = expanded_key[(i - 1) * 16 + 12 + j];
        }

        // Perform key schedule core
        if (i % 1 == 0)
        {
            // Rotate
            uint8_t k = temp[0];
            temp[0] = temp[1];
            temp[1] = temp[2];
            temp[2] = temp[3];
            temp[3] = k;

            // SubBytes
            for (int j = 0; j < 4; j++)
            {
                temp[j] = s_box[temp[j]];
            }

            // XOR with round constant
            temp[0] ^= rcon[i];
        }

        // First 4 bytes of current round key = First 4 bytes of previous round key XOR temp
        for (int j = 0; j < 4; j++)
        {
            expanded_key[i * 16 + j] = expanded_key[(i - 1) * 16 + j] ^ temp[j];
        }

        // Generate the remaining 12 bytes of the round key
        for (int j = 4; j < 16; j++)
        {
            expanded_key[i * 16 + j] = expanded_key[i * 16 + j - 4] ^ expanded_key[(i - 1) * 16 + j];
        }

        i++;
    }
}

// SubBytes transformation - replaces each byte using the S-box
void sub_bytes(uint8_t *state)
{
    for (int i = 0; i < 16; i++)
    {
        state[i] = s_box[state[i]];
    }
}

// Inverse SubBytes transformation
void inv_sub_bytes(uint8_t *state)
{
    for (int i = 0; i < 16; i++)
    {
        state[i] = inv_s_box[state[i]];
    }
}

// ShiftRows transformation - cyclically shifts rows of the state
void shift_rows(uint8_t *state)
{
    uint8_t temp;

    // Row 1: shift left by 1
    temp = state[1];
    state[1] = state[5];
    state[5] = state[9];
    state[9] = state[13];
    state[13] = temp;

    // Row 2: shift left by 2
    temp = state[2];
    state[2] = state[10];
    state[10] = temp;
    temp = state[6];
    state[6] = state[14];
    state[14] = temp;

    // Row 3: shift left by 3 (which is right by 1)
    temp = state[15];
    state[15] = state[11];
    state[11] = state[7];
    state[7] = state[3];
    state[3] = temp;
}

// Inverse ShiftRows transformation
void inv_shift_rows(uint8_t *state)
{
    uint8_t temp;

    // Row 1: shift right by 1
    temp = state[13];
    state[13] = state[9];
    state[9] = state[5];
    state[5] = state[1];
    state[1] = temp;

    // Row 2: shift right by 2
    temp = state[2];
    state[2] = state[10];
    state[10] = temp;
    temp = state[6];
    state[6] = state[14];
    state[14] = temp;

    // Row 3: shift right by 3 (which is left by 1)
    temp = state[3];
    state[3] = state[7];
    state[7] = state[11];
    state[11] = state[15];
    state[15] = temp;
}

// Multiply two numbers in GF(2^8)
uint8_t gmul(uint8_t a, uint8_t b)
{
    uint8_t p = 0;
    uint8_t hi_bit_set;

    for (int i = 0; i < 8; i++)
    {
        if ((b & 1) == 1)
        {
            p ^= a;
        }

        hi_bit_set = (a & 0x80);
        a <<= 1;
        if (hi_bit_set == 0x80)
        {
            a ^= 0x1b; // Irreducible polynomial x^8 + x^4 + x^3 + x + 1
        }

        b >>= 1;
    }

    return p;
}

// MixColumns transformation - treats each column as a polynomial and multiplies with a fixed polynomial
void mix_columns(uint8_t *state)
{
    uint8_t temp[4];

    for (int i = 0; i < 4; i++)
    {
        for (int j = 0; j < 4; j++)
        {
            temp[j] = state[i * 4 + j];
        }

        state[i * 4 + 0] = gmul(0x02, temp[0]) ^ gmul(0x03, temp[1]) ^ temp[2] ^ temp[3];
        state[i * 4 + 1] = temp[0] ^ gmul(0x02, temp[1]) ^ gmul(0x03, temp[2]) ^ temp[3];
        state[i * 4 + 2] = temp[0] ^ temp[1] ^ gmul(0x02, temp[2]) ^ gmul(0x03, temp[3]);
        state[i * 4 + 3] = gmul(0x03, temp[0]) ^ temp[1] ^ temp[2] ^ gmul(0x02, temp[3]);
    }
}

// Inverse MixColumns transformation
void inv_mix_columns(uint8_t *state)
{
    uint8_t temp[4];

    for (int i = 0; i < 4; i++)
    {
        for (int j = 0; j < 4; j++)
        {
            temp[j] = state[i * 4 + j];
        }

        state[i * 4 + 0] = gmul(0x0e, temp[0]) ^ gmul(0x0b, temp[1]) ^ gmul(0x0d, temp[2]) ^ gmul(0x09, temp[3]);
        state[i * 4 + 1] = gmul(0x09, temp[0]) ^ gmul(0x0e, temp[1]) ^ gmul(0x0b, temp[2]) ^ gmul(0x0d, temp[3]);
        state[i * 4 + 2] = gmul(0x0d, temp[0]) ^ gmul(0x09, temp[1]) ^ gmul(0x0e, temp[2]) ^ gmul(0x0b, temp[3]);
        state[i * 4 + 3] = gmul(0x0b, temp[0]) ^ gmul(0x0d, temp[1]) ^ gmul(0x09, temp[2]) ^ gmul(0x0e, temp[3]);
    }
}

// AddRoundKey transformation - XOR state with round key
void add_round_key(uint8_t *state, const uint8_t *round_key)
{
    for (int i = 0; i < 16; i++)
    {
        state[i] ^= round_key[i];
    }
}

// AES encryption of a single block
void aes_encrypt_block(const uint8_t *input_block, uint8_t *output_block, const uint8_t *expanded_key)
{
    // Copy input to output for in-place operations
    memcpy(output_block, input_block, AES_BLOCK_SIZE);

    printf("Initial state:\n");
    print_state(output_block);

    // Initial round - just AddRoundKey
    printf("\nRound 0 (AddRoundKey):\n");
    add_round_key(output_block, expanded_key);
    print_state(output_block);

    // Main rounds
    for (int round = 1; round < AES_128_ROUNDS; round++)
    {
        printf("\nRound %d:\n", round);

        printf("After SubBytes:\n");
        sub_bytes(output_block);
        print_state(output_block);

        printf("After ShiftRows:\n");
        shift_rows(output_block);
        print_state(output_block);

        printf("After MixColumns:\n");
        mix_columns(output_block);
        print_state(output_block);

        printf("After AddRoundKey:\n");
        add_round_key(output_block, expanded_key + round * AES_BLOCK_SIZE);
        print_state(output_block);
    }

    // Final round - no MixColumns
    printf("\nFinal Round:\n");

    printf("After SubBytes:\n");
    sub_bytes(output_block);
    print_state(output_block);

    printf("After ShiftRows:\n");
    shift_rows(output_block);
    print_state(output_block);

    printf("After AddRoundKey:\n");
    add_round_key(output_block, expanded_key + AES_128_ROUNDS * AES_BLOCK_SIZE);
    print_state(output_block);
}

// AES decryption of a single block
void aes_decrypt_block(const uint8_t *input_block, uint8_t *output_block, const uint8_t *expanded_key)
{
    // Copy input to output for in-place operations
    memcpy(output_block, input_block, AES_BLOCK_SIZE);

    printf("Initial state:\n");
    print_state(output_block);

    // Initial round - just AddRoundKey with last round key
    printf("\nRound 0 (AddRoundKey):\n");
    add_round_key(output_block, expanded_key + AES_128_ROUNDS * AES_BLOCK_SIZE);
    print_state(output_block);

    // Main rounds in reverse
    for (int round = AES_128_ROUNDS - 1; round > 0; round--)
    {
        printf("\nRound %d:\n", AES_128_ROUNDS - round);

        printf("After InvShiftRows:\n");
        inv_shift_rows(output_block);
        print_state(output_block);

        printf("After InvSubBytes:\n");
        inv_sub_bytes(output_block);
        print_state(output_block);

        printf("After AddRoundKey:\n");
        add_round_key(output_block, expanded_key + round * AES_BLOCK_SIZE);
        print_state(output_block);

        printf("After InvMixColumns:\n");
        inv_mix_columns(output_block);
        print_state(output_block);
    }

    // Final round
    printf("\nFinal Round:\n");

    printf("After InvShiftRows:\n");
    inv_shift_rows(output_block);
    print_state(output_block);

    printf("After InvSubBytes:\n");
    inv_sub_bytes(output_block);
    print_state(output_block);

    printf("After AddRoundKey:\n");
    add_round_key(output_block, expanded_key); // First round key
    print_state(output_block);
}
