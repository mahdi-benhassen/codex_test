#include <errno.h>
#include <stdint.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#define ROTR(x, n) (((x) >> (n)) | ((x) << (32U - (n))))
#define CH(x, y, z) (((x) & (y)) ^ (~(x) & (z)))
#define MAJ(x, y, z) (((x) & (y)) ^ ((x) & (z)) ^ ((y) & (z)))
#define BSIG0(x) (ROTR((x), 2U) ^ ROTR((x), 13U) ^ ROTR((x), 22U))
#define BSIG1(x) (ROTR((x), 6U) ^ ROTR((x), 11U) ^ ROTR((x), 25U))
#define SSIG0(x) (ROTR((x), 7U) ^ ROTR((x), 18U) ^ ((x) >> 3U))
#define SSIG1(x) (ROTR((x), 17U) ^ ROTR((x), 19U) ^ ((x) >> 10U))

static const uint32_t K[64] = {
    0x428a2f98U, 0x71374491U, 0xb5c0fbcfU, 0xe9b5dba5U,
    0x3956c25bU, 0x59f111f1U, 0x923f82a4U, 0xab1c5ed5U,
    0xd807aa98U, 0x12835b01U, 0x243185beU, 0x550c7dc3U,
    0x72be5d74U, 0x80deb1feU, 0x9bdc06a7U, 0xc19bf174U,
    0xe49b69c1U, 0xefbe4786U, 0x0fc19dc6U, 0x240ca1ccU,
    0x2de92c6fU, 0x4a7484aaU, 0x5cb0a9dcU, 0x76f988daU,
    0x983e5152U, 0xa831c66dU, 0xb00327c8U, 0xbf597fc7U,
    0xc6e00bf3U, 0xd5a79147U, 0x06ca6351U, 0x14292967U,
    0x27b70a85U, 0x2e1b2138U, 0x4d2c6dfcU, 0x53380d13U,
    0x650a7354U, 0x766a0abbU, 0x81c2c92eU, 0x92722c85U,
    0xa2bfe8a1U, 0xa81a664bU, 0xc24b8b70U, 0xc76c51a3U,
    0xd192e819U, 0xd6990624U, 0xf40e3585U, 0x106aa070U,
    0x19a4c116U, 0x1e376c08U, 0x2748774cU, 0x34b0bcb5U,
    0x391c0cb3U, 0x4ed8aa4aU, 0x5b9cca4fU, 0x682e6ff3U,
    0x748f82eeU, 0x78a5636fU, 0x84c87814U, 0x8cc70208U,
    0x90befffaU, 0xa4506cebU, 0xbef9a3f7U, 0xc67178f2U
};

static void sha256_transform(uint32_t state[8], const uint8_t block[64]) {
    uint32_t w[64];
    for (size_t i = 0; i < 16; ++i) {
        w[i] = ((uint32_t)block[i * 4] << 24U) |
               ((uint32_t)block[i * 4 + 1] << 16U) |
               ((uint32_t)block[i * 4 + 2] << 8U) |
               ((uint32_t)block[i * 4 + 3]);
    }
    for (size_t i = 16; i < 64; ++i) {
        w[i] = SSIG1(w[i - 2]) + w[i - 7] + SSIG0(w[i - 15]) + w[i - 16];
    }

    uint32_t a = state[0];
    uint32_t b = state[1];
    uint32_t c = state[2];
    uint32_t d = state[3];
    uint32_t e = state[4];
    uint32_t f = state[5];
    uint32_t g = state[6];
    uint32_t h = state[7];

    for (size_t i = 0; i < 64; ++i) {
        uint32_t t1 = h + BSIG1(e) + CH(e, f, g) + K[i] + w[i];
        uint32_t t2 = BSIG0(a) + MAJ(a, b, c);

        h = g;
        g = f;
        f = e;
        e = d + t1;
        d = c;
        c = b;
        b = a;
        a = t1 + t2;
    }

    state[0] += a;
    state[1] += b;
    state[2] += c;
    state[3] += d;
    state[4] += e;
    state[5] += f;
    state[6] += g;
    state[7] += h;
}

static int sha256_bytes(const uint8_t *input, size_t len, uint8_t digest[32]) {
    uint32_t state[8] = {
        0x6a09e667U, 0xbb67ae85U, 0x3c6ef372U, 0xa54ff53aU,
        0x510e527fU, 0x9b05688cU, 0x1f83d9abU, 0x5be0cd19U
    };

    uint64_t bit_len = (uint64_t)len * 8ULL;
    size_t new_len = len + 1;
    while ((new_len % 64) != 56) {
        ++new_len;
    }

    size_t total_len = new_len + 8;
    uint8_t *msg = (uint8_t *)calloc(total_len, sizeof(uint8_t));
    if (!msg) {
        return -1;
    }

    if (len > 0) {
        memcpy(msg, input, len);
    }
    msg[len] = 0x80U;

    for (size_t i = 0; i < 8; ++i) {
        msg[new_len + i] = (uint8_t)(bit_len >> (56U - (uint32_t)(i * 8)));
    }

    for (size_t i = 0; i < total_len; i += 64) {
        sha256_transform(state, msg + i);
    }

    free(msg);

    for (size_t i = 0; i < 8; ++i) {
        digest[i * 4] = (uint8_t)(state[i] >> 24U);
        digest[i * 4 + 1] = (uint8_t)(state[i] >> 16U);
        digest[i * 4 + 2] = (uint8_t)(state[i] >> 8U);
        digest[i * 4 + 3] = (uint8_t)(state[i]);
    }

    return 0;
}

static int read_file_bytes(const char *path, uint8_t **buffer, size_t *len) {
    FILE *fp = fopen(path, "rb");
    if (!fp) {
        fprintf(stderr, "Error opening file '%s': %s\n", path, strerror(errno));
        return -1;
    }

    if (fseek(fp, 0, SEEK_END) != 0) {
        fprintf(stderr, "Error seeking file '%s'\n", path);
        fclose(fp);
        return -1;
    }

    long sz = ftell(fp);
    if (sz < 0) {
        fprintf(stderr, "Error getting file size for '%s'\n", path);
        fclose(fp);
        return -1;
    }

    if (fseek(fp, 0, SEEK_SET) != 0) {
        fprintf(stderr, "Error rewinding file '%s'\n", path);
        fclose(fp);
        return -1;
    }

    *len = (size_t)sz;
    *buffer = NULL;

    if (*len > 0) {
        *buffer = (uint8_t *)malloc(*len);
        if (!*buffer) {
            fprintf(stderr, "Allocation failed while reading '%s'\n", path);
            fclose(fp);
            return -1;
        }

        size_t read_n = fread(*buffer, 1, *len, fp);
        if (read_n != *len) {
            fprintf(stderr, "Error reading file '%s'\n", path);
            free(*buffer);
            *buffer = NULL;
            fclose(fp);
            return -1;
        }
    }

    fclose(fp);
    return 0;
}

static void print_usage(const char *prog) {
    fprintf(stderr, "Usage: %s -s \"text\" | -f <file>\n", prog);
}

int main(int argc, char *argv[]) {
    if (argc != 3) {
        print_usage(argv[0]);
        return 1;
    }

    uint8_t *data = NULL;
    size_t len = 0;

    if (strcmp(argv[1], "-s") == 0) {
        data = (uint8_t *)argv[2];
        len = strlen(argv[2]);
    } else if (strcmp(argv[1], "-f") == 0) {
        if (read_file_bytes(argv[2], &data, &len) != 0) {
            return 1;
        }
    } else {
        print_usage(argv[0]);
        return 1;
    }

    uint8_t digest[32];
    if (sha256_bytes(data, len, digest) != 0) {
        if (strcmp(argv[1], "-f") == 0) {
            free(data);
        }
        fprintf(stderr, "Allocation failed during SHA-256 computation\n");
        return 1;
    }

    for (size_t i = 0; i < 32; ++i) {
        printf("%02x", digest[i]);
    }
    printf("\n");

    if (strcmp(argv[1], "-f") == 0) {
        free(data);
    }

    return 0;
}
