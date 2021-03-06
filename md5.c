#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <stdint.h>
#include <limits.h>
#include <time.h>
// leftrotate function definition
#define LEFTROTATE(x, c) (((x) << (c)) | ((x) >> (32 - (c))))

// These vars will contain the hash
uint32_t h0, h1, h2, h3;

void md5(uint8_t *initial_msg, size_t initial_len) {

    // Message (to prepare)
    uint8_t *msg = NULL;

    // Note: All variables are unsigned 32 bit and wrap modulo 2^32 when calculating

    // r specifies the per-round shift amounts

    uint32_t r[] = {7, 12, 17, 22, 7, 12, 17, 22, 7, 12, 17, 22, 7, 12, 17, 22,
                    5, 9, 14, 20, 5, 9, 14, 20, 5, 9, 14, 20, 5, 9, 14, 20,
                    4, 11, 16, 23, 4, 11, 16, 23, 4, 11, 16, 23, 4, 11, 16, 23,
                    6, 10, 15, 21, 6, 10, 15, 21, 6, 10, 15, 21, 6, 10, 15, 21};

    // Use binary integer part of the sines of integers (in radians) as constants// Initialize variables:
    uint32_t k[] = {
            0xd76aa478, 0xe8c7b756, 0x242070db, 0xc1bdceee,
            0xf57c0faf, 0x4787c62a, 0xa8304613, 0xfd469501,
            0x698098d8, 0x8b44f7af, 0xffff5bb1, 0x895cd7be,
            0x6b901122, 0xfd987193, 0xa679438e, 0x49b40821,
            0xf61e2562, 0xc040b340, 0x265e5a51, 0xe9b6c7aa,
            0xd62f105d, 0x02441453, 0xd8a1e681, 0xe7d3fbc8,
            0x21e1cde6, 0xc33707d6, 0xf4d50d87, 0x455a14ed,
            0xa9e3e905, 0xfcefa3f8, 0x676f02d9, 0x8d2a4c8a,
            0xfffa3942, 0x8771f681, 0x6d9d6122, 0xfde5380c,
            0xa4beea44, 0x4bdecfa9, 0xf6bb4b60, 0xbebfbc70,
            0x289b7ec6, 0xeaa127fa, 0xd4ef3085, 0x04881d05,
            0xd9d4d039, 0xe6db99e5, 0x1fa27cf8, 0xc4ac5665,
            0xf4292244, 0x432aff97, 0xab9423a7, 0xfc93a039,
            0x655b59c3, 0x8f0ccc92, 0xffeff47d, 0x85845dd1,
            0x6fa87e4f, 0xfe2ce6e0, 0xa3014314, 0x4e0811a1,
            0xf7537e82, 0xbd3af235, 0x2ad7d2bb, 0xeb86d391};

    h0 = 0x67452301;
    h1 = 0xefcdab89;
    h2 = 0x98badcfe;
    h3 = 0x10325476;

    // Pre-processing: adding a single 1 bit
    // append "1" bit to message
    /* Notice: the input bytes are considered as bits strings,
       where the first bit is the most significant bit of the byte.[37] */

    // Pre-processing: padding with zeros
    //append "0" bit until message length in bit ≡ 448 (mod 512)
    //append length mod (2 pow 64) to message

    int new_len;
    for (new_len = initial_len * 8 + 1; new_len % 512 != 448; new_len++);
    new_len /= 8;

    msg = calloc(new_len + 64, 1); // also appends "0" bits
    // (we alloc also 64 extra bytes...)
    memcpy(msg, initial_msg, initial_len);
    msg[initial_len] = 128; // write the "1" bit

    uint32_t bits_len = 8 * initial_len; // note, we append the len
    memcpy(msg + new_len, &bits_len, 4); // in bits at the end of the buffer

    // Process the message in successive 512-bit chunks:
    //for each 512-bit chunk of message:
    int offset;
    for (offset = 0; offset < new_len; offset += (512 / 8)) {

        // break chunk into sixteen 32-bit words w[j], 0 ≤ j ≤ 15
        uint32_t *w = (uint32_t * )(msg + offset);

        // Initialize hash value for this chunk:
        uint32_t a = h0;
        uint32_t b = h1;
        uint32_t c = h2;
        uint32_t d = h3;

        // Main loop:
        uint32_t i;
        for (i = 0; i < 64; i++) {

            uint32_t f, g;

            if (i < 16) {
                f = (b & c) | ((~b) & d);
                g = i;
            } else if (i < 32) {
                f = (d & b) | ((~d) & c);
                g = (5 * i + 1) % 16;
            } else if (i < 48) {
                f = b ^ c ^ d;
                g = (3 * i + 5) % 16;
            } else {
                f = c ^ (b | (~d));
                g = (7 * i) % 16;
            }

            // #ifdef ROUNDS
            //             printf("f=%x g=%d w[g]=%x\n", f, g, w[g]);
            // #endif
            uint32_t temp = d;
            d = c;
            c = b;
            // printf("rotateLeft(%x + %x + %x + %x, %d)\n", a, f, k[i], w[g], r[i]);
            b = b + LEFTROTATE((a + f + k[i] + w[g]), r[i]);
            a = temp;
        }

        // Add this chunk's hash to result so far:

        h0 += a;
        h1 += b;
        h2 += c;
        h3 += d;
    }

    // cleanup
    free(msg);
}

char *compute_hash(char *msg, int hash_length, char *return_hash) {
    size_t len = strlen(msg);
    char hash[32];

    md5(msg, len);

    //var char digest[16] := h0 append h1 append h2 append h3 //(Output is in little-endian)
    uint8_t *p;
    char temp[2];

    // display result

    int i, j = 0;
    p = (uint8_t * ) & h0;
    // sprintf(hash0, "", p[0], p[1], p[2], p[3]);
    for (i = 0; i < 4; i++) {
        char temp[10];
        sprintf(temp, "%.2x ", p[i]);
        hash[j++] = temp[0];
        hash[j++] = temp[1];
    }

    p = (uint8_t * ) & h1;
    for (i = 0; i < 4; i++) {
        char temp[10];
        sprintf(temp, "%.2x ", p[i]);
        hash[j++] = temp[0];
        hash[j++] = temp[1];
    }

    p = (uint8_t * ) & h2;
    for (i = 0; i < 4; i++) {
        char temp[10];
        sprintf(temp, "%.2x ", p[i]);
        hash[j++] = temp[0];
        hash[j++] = temp[1];
    }

    p = (uint8_t * ) & h3;
    for (i = 0; i < 4; i++) {
        char temp[10];
        sprintf(temp, "%.2x ", p[i]);
        hash[j++] = temp[0];
        hash[j++] = temp[1];
    }

    // return hash with hash length
    for (i = 0; i < hash_length; i++) {
        return_hash[i] = hash[i];
    }
    return_hash[hash_length] = '\0';
    return return_hash;
}

void floyd_cycle(char *msg, int hash_length) {
    // deploy Floyd cycle algorithms
    char hash[33];
    int i = 0;
    char temp[33], temp1[33], temp2[33];


    //step 1: find the starting repetition point
    //Start with (a0, b0) ← (x0, x0)
    compute_hash(msg, hash_length, temp);     //hash(x0)
    compute_hash(msg, hash_length, temp1);    //
    compute_hash(temp1, hash_length, temp1);     //hash(hash(x0))
    //Iteratively compute (ai, bi) ← (H(ai−1), H(H(bi−1))) = (xi, x2i) Stop whenever ai0 = bi0
    while (strcmp(temp, temp1) != 0) {
        compute_hash(temp, hash_length, temp); // f(xi)
        compute_hash(temp1, hash_length, temp1); // f(f(xi))
        compute_hash(temp1, hash_length, temp1);
    }
    printf("%s \t %s \n", temp, temp1);

    //step 2 : Find the position μ of first repetition.
    unsigned long mu = 0;
    
    // Start with (c0, d0) ← (x0, xi0) = (x0, xk.δ)
    compute_hash(msg, hash_length, temp2); // hash(x0)
    compute_hash(temp1, hash_length, temp1);

    // Iteratively compute (ci, di) ← (H(ci−1), H(di−1)) = (xi, xi+k.δ) Stop whenever ci1 = di1
    while (strcmp(temp1, temp2) != 0) {
        compute_hash(temp2, hash_length, temp2);
        compute_hash(temp1, hash_length, temp1);
        mu += 1;
    }

    /* step 3: # Find the length of the shortest cycle starting from x_μ
     * temp2 moves one step at a time while temp1 is still.
     * lam is incremented until λ is found.  */
    unsigned long lam = 1;
    compute_hash(temp1, hash_length, temp2);
    while (strcmp(temp2, temp1) != 0) {
        compute_hash(temp2, hash_length, temp2);
        lam += 1;
    }
    printf("the starting position is: %lu value: %s, the cycle length: %lu \n", mu, temp, lam);
}

int main(int argc, char **argv) {
    if (argc < 2) {
        printf("usage: %s 'string'\n", argv[0]);
        return 1;
    }
    char *msg = argv[1];
    int i;
    for (i = 1; i < 32; i++) {
        printf("%d \n ", i);
        clock_t t;
        t = clock();
        floyd_cycle(msg, i);
        t = clock() - t;
        double time_taken = ((double) t) / CLOCKS_PER_SEC;
        printf("It took %f seconds to find the collision \n", time_taken);
    }
    return 0;
}

    
    
		
