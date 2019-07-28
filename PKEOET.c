#include <pbc/pbc.h>
#include <pbc/pbc_test.h>
#include <openssl/sha.h>
#include <openssl/evp.h>
#include <string.h>
#include <gmp.h>
#include <stdio.h>
#include <stdlib.h>
#include <stdbool.h>

#define SIZE_M 1000


element_t  p, g, h, zeta;
element_t s, t, a, b, c, d;
element_t alpha, r;

element_t pk1[3], pk2[3];
element_t dpk, dsk;

element_t w1[2], w2[2], x[2], y[2];
element_t temp_x[3];
element_t temp_y[5];
element_t theta0, theta1, theta2;

element_t m_a[SIZE_M], m_b[SIZE_M];

element_t tk1[6], tk2[6];

element_t temp_e[24];

element_t omega1, omega2;
element_t v[4];
element_t temp_v[28];

pairing_t pairing;

element_t temp_z;

int i = 0;

unsigned char w1_s1[256], w1_s2[256], w1_s3[256];
unsigned char w2_s1[256], w2_s2[256], w2_s3[256];
unsigned char tx_s[256], x_s[256], y_s[256];
unsigned char x1_s[256], x2_s[256];

unsigned char* theta0_s;
unsigned char* theta1_s;
unsigned char* theta2_s;

double total_time = 0.0, start = 0.0;

struct Ciphertext
{
    unsigned char c1_s[256];
    unsigned char c2_s[256];
    unsigned char c3_s[128];
    unsigned char c4_s[128];

    element_t c1;
    element_t c2;
    element_t c3;
    element_t c4;
}c_a[SIZE_M], c_b[SIZE_M];


char* get_hash(unsigned char tw1_s[], unsigned char tw2_s[], unsigned char tx_s[]) {
    EVP_MD_CTX ctx;
 
    unsigned char h_in_s[640];
    static char theta_s[64];
    int size_h_out;

    int k;
    for (k = 0; k < 640; k++)
    {
        if (k < 256) 
            h_in_s[k] = tw1_s[k];
        else if (k < 512)
            h_in_s[k] = tw2_s[k-256];
        else
            h_in_s[k] = tx_s[k-512];
    }
        
    EVP_DigestInit(&ctx, EVP_sha512());
    EVP_DigestUpdate(&ctx, h_in_s, 640);
    EVP_DigestFinal(&ctx, theta_s, &size_h_out);

    return theta_s;
}

void init() {
    element_init_Zr(s, pairing);
    element_init_Zr(t, pairing);
    element_init_Zr(a, pairing);
    element_init_Zr(b, pairing);
    element_init_Zr(c, pairing);
    element_init_Zr(d, pairing);

    element_init_Zr(alpha, pairing);
    element_init_Zr(r, pairing);

    for (i = 0; i < 6; i++)
    {
        element_init_G2(tk1[i], pairing);
        element_init_G2(tk2[i], pairing);
    }
    

    for (i = 0; i < 2; i++)
    {
        element_init_G1(temp_x[i], pairing);
    }
    element_init_GT(temp_x[2], pairing);

    for (i = 0; i < 5; i++)
    {
        element_init_G1(temp_y[i], pairing);
    }
    
    for (i = 0; i < 2; i++)
    {
        element_init_G1(w1[i], pairing);
        element_init_G1(w2[i], pairing);
        element_init_GT(x[i], pairing);
        element_init_GT(y[i], pairing);
    }

    for (i = 0; i < SIZE_M; i++)
    {
        element_init_GT(m_a[i], pairing);
        element_init_GT(m_b[i], pairing);
    }

    element_init_Zr(theta0, pairing);
    element_init_Zr(theta1, pairing);
    element_init_Zr(theta2, pairing);

    for (i = 0; i < 24; i++)
    {
        element_init_GT(temp_e[i], pairing);
    }
    
    element_init_Zr(omega1, pairing);
    element_init_Zr(omega2, pairing);

    for (i = 0; i < 4; i++)
    {
        element_init_GT(v[i], pairing);
    }

    for (i = 0; i < 28; i++)
    {
        element_init_GT(temp_v[i], pairing);
    }

    element_init_Zr(temp_z, pairing);

    for (i = 0; i < 1000; i++)
    {
        element_init_G1(c_a[i].c1, pairing);
        element_init_G1(c_a[i].c2, pairing);
        element_init_GT(c_a[i].c3, pairing);
        element_init_GT(c_a[i].c4, pairing);

        element_init_G1(c_b[i].c1, pairing);
        element_init_G1(c_b[i].c2, pairing);
        element_init_GT(c_b[i].c3, pairing);
        element_init_GT(c_b[i].c4, pairing);
    }
    
    
}

void ukg(element_t pk[]) {
    for (i = 0; i < 3; i++)
    {
        element_init_G1(pk[i], pairing);
    }

    start = pbc_get_time();

    element_random(s);
    element_random(t);
    element_random(a);
    element_random(b);
    element_random(c);
    element_random(d);

    element_pow2_zn(pk[0], g, s, h, t);  // pk[0] = g^s * h^t
    element_pow2_zn(pk[1], g, a, h, b);  // pk[1] = g^a * h^b
    element_pow2_zn(pk[2], g, c, h, d);  // pk[2] = g^c * h^d

    double ukg_time = pbc_get_time() - start;

    printf("UKG Time: %.2fs\n", ukg_time);

    total_time += ukg_time;
}

void dkg() {
    element_init_G2(dpk, pairing);
    element_init_Zr(dsk, pairing);

    start = pbc_get_time();

    element_random(alpha);
    element_pow_zn(dpk, zeta, alpha);  // dpk = zeta^alpha

    double dkg_time = pbc_get_time() - start;

    printf("DKG Time: %.2fs\n", dkg_time);

    total_time += dkg_time;
    element_set(dsk, alpha);
}

void enc(int num, element_t w1_t, element_t w2_t, element_t x_t, element_t y_t, element_t m[], struct Ciphertext tc[]) {
    start = pbc_get_time();

    if (num > SIZE_M) num = SIZE_M;

    for (i = 0; i < num; i++)
    {
        element_random(r);

        element_pow_zn(w1_t, g, r);  // w1 = g^r
        element_pow_zn(w2_t, h, r);  // w2 = h^r

        element_to_bytes(w1_s1, w1_t);
        element_to_bytes(w2_s1, w2_t);

        // printf("w1_s is %02x\n", w1_s1);
        // printf("w2_s is %02x\n", w2_s1);

        element_pow2_zn(temp_x[0], g, s, h, t); // g^s * h^t
        element_pow_zn(temp_x[1], temp_x[0], r); // (g^s * h^t)^r
        element_pairing(temp_x[2], temp_x[1], zeta);  // x = e((g^s * h^t)^r, zeta)
        element_mul(x_t, temp_x[2], m[i]);  // x * m
       
        element_to_bytes(tx_s, temp_x[2]);
        element_to_bytes(x_s, x_t);

        theta0_s = get_hash(w1_s1, w2_s1, x_s);
        element_from_hash(theta0, theta0_s, 64);  // theta0 = H(w1, w2, x * m)
        //element_printf("Theta0 is: %B\n", theta0);

        element_pow2_zn(temp_y[0], g, a, h, b);  // g^a * h^b
        element_pow2_zn(temp_y[1], g, c, h, d);  // g^c * h^d
        element_pow_zn(temp_y[2], temp_y[2], theta0);  //(g^c * h^d)^theta0

        element_mul(temp_y[3], temp_y[0], temp_y[2]);  // (g^a * h^b) * (g^c * h^d)^theta0
        element_pow_zn(temp_y[4], temp_y[3], r);  // ((g^a * h^b) * (g^c * h^d)^theta0)^r

        element_pairing(y_t, temp_y[4], zeta);

        element_to_bytes(y_s, y_t);

        // element_printf("w1 is: %B\n", w1_t);
        // element_printf("w2 is: %B\n", w2_t);
        // element_printf("x is: %B\n", x_t);
        // element_printf("y is: %B\n", y_t);

        int j;
        for (j = 0; j < 256; j++)
        {
            tc[i].c1_s[j] = w1_s1[j];
            tc[i].c2_s[j] = w2_s1[j];
        }
        for (j = 0; j < 128; j++)
        {
            tc[i].c3_s[j] = x_s[j];
        }
        for (j = 0; j < 20; j++)
        {
            tc[i].c4_s[j] = y_s[j];
        }

        element_set(tc[i].c1, w1_t);
        element_set(tc[i].c2, w2_t);
        element_set(tc[i].c3, x_t);
        element_set(tc[i].c4, y_t);
    }

    double enc_time = pbc_get_time() - start;
    printf("Encrypt %d Time: %.2fs\n", num, enc_time);

    total_time += enc_time;
}

void tkg(element_t temp_tk[]) {
    start = pbc_get_time();

    element_pow_zn(temp_tk[0], dpk, s);
    element_pow_zn(temp_tk[1], dpk, t);
    element_pow_zn(temp_tk[2], dpk, a);
    element_pow_zn(temp_tk[3], dpk, b);
    element_pow_zn(temp_tk[4], dpk, c);
    element_pow_zn(temp_tk[5], dpk, d);  // tk[] = [dpk^s, dpk^t, dpk^a, dpk^b, dpk^c, dpk^d]

    double tkg_time = pbc_get_time() - start;

    printf("TKG Time: %.2fs\n", tkg_time);

    total_time += tkg_time;
}

void IVgen(struct Ciphertext ct1[], struct Ciphertext ct2[]) {
    element_random(omega1);
    element_random(omega2);

    // element_from_bytes(w1[0], ct1[0].c1);
    // element_from_bytes(w2[0], ct1[0].c2);
    // element_from_bytes(x[0], ct1[0].c3);
    // element_from_bytes(y[0], ct1[0].c4);

    // element_from_bytes(w1[1], ct2[0].c1);
    // element_from_bytes(w2[1], ct2[0].c2);
    // element_from_bytes(x[1], ct2[0].c3);
    // element_from_bytes(y[1], ct2[0].c4);

    element_set(w1[0], ct1[0].c1);
    element_set(w2[0], ct1[0].c2);
    element_set(x[0], ct1[0].c3);
    element_set(y[0], ct1[0].c4);

    element_set(w1[1], ct2[0].c1);
    element_set(w2[1], ct2[0].c2);
    element_set(x[1], ct2[0].c3);
    element_set(y[1], ct2[0].c4);

    // element_printf("w1 is: %B\n", w1[0]);
    // element_printf("w2 is: %B\n", w2[0]);
    // element_printf("x is: %B\n", x[0]);
    // element_printf("y is: %B\n", y[0]);

    element_div(temp_v[0], x[0], x[1]);  // x1 / x2
    element_pow_zn(v[0], temp_v[0], omega1);  // v1 = (x1 / x2)^omega1

    element_pairing(temp_v[1], w1[0], tk1[0]);  // e(w1(1), t1(1))
    element_pairing(temp_v[2], w1[1], tk1[1]);  // e(w1(2), t1(2))
    element_mul(temp_v[3], temp_v[1], temp_v[2]); 
    
    element_pairing(temp_v[4], w2[0], tk2[0]);  // e(w2(1), t2(1))
    element_pairing(temp_v[5], w2[1], tk2[1]);  // e(w2(2), t2(2))
    element_mul(temp_v[6], temp_v[4], temp_v[5]);
    element_div(temp_v[7], temp_v[3], temp_v[6]);
    element_pow_zn(v[1], temp_v[7], omega1);  // v2

    element_div(temp_v[8], y[0], y[1]);  // y1 / y2
    element_pow_zn(v[2], temp_v[8], omega2);  // v3 = (y1 / y2)^omega2

    element_to_bytes(w1_s2, w1[0]);
    element_to_bytes(w1_s3, w1[1]);
    element_to_bytes(w2_s2, w2[0]);
    element_to_bytes(w2_s3, w2[1]);
    element_to_bytes(x1_s, x[0]);
    element_to_bytes(x2_s, x[1]);

    theta1_s = get_hash(w1_s2, w1_s3, x1_s);
    element_from_hash(theta1, theta1_s, 64);  // theta1 = H(w1(1), w1(2), x1)


    theta2_s = get_hash(w2_s2, w2_s3, x2_s);
    element_from_hash(theta2, theta2_s, 64);  // theta2 = H(w2(1), w2(2), x2)


    element_pairing(temp_v[9], w1[0], tk1[2]);  // e(w1(1), t1(3))
    element_pairing(temp_v[10], w1[1], tk1[3]);  // e(w1(2), t1(4))
    element_mul(temp_v[11], temp_v[9], temp_v[10]);

    element_pairing(temp_v[12], w1[0], tk1[4]);  // e(w1(1), t1(5))
    element_pow_zn(temp_v[13], temp_v[12], theta1);  // (e(w1(1), t1(5)))^theta1
    element_pairing(temp_v[14], w1[1], tk1[5]);  // e(w1(2), t1(6))
    element_pow_zn(temp_v[15], temp_v[14], theta1);  // (e(w1(2), t1(6)))^theta1
    element_mul(temp_v[16], temp_v[13], temp_v[15]);
    element_mul(temp_v[17], temp_v[11], temp_v[16]);

    element_pairing(temp_v[18], w2[0], tk2[2]);  // e(w2(1), t2(3))
    element_pairing(temp_v[19], w2[1], tk2[3]);  // e(w2(2), t2(4))
    element_mul(temp_v[20], temp_v[18], temp_v[19]);

    element_pairing(temp_v[21], w2[0], tk2[4]);  // e(w2(1), t2(5))
    element_pow_zn(temp_v[22], temp_v[20], theta2);  // (e(w2(1), t2(5)))^theta2
    element_pairing(temp_v[23], w2[1], tk2[5]);  // e(w2(2), t2(6))
    element_pow_zn(temp_v[24], temp_v[22], theta2);  // (e(w2(2), t2(6)))^theta2
    element_mul(temp_v[25], temp_v[22], temp_v[24]);
    element_mul(temp_v[26], temp_v[20], temp_v[25]);

    element_div(temp_v[27], temp_v[17], temp_v[26]);
    element_pow_zn(v[3], temp_v[27], omega2);  // v4

    for (i = 0; i < 4; i++)
    {
        element_printf("V%d is: %B\n", i+1, v[i]);
    }
}

void pTest(struct Ciphertext ct1[], struct Ciphertext ct2[], element_t temp_pk1[], element_t temp_pk2[], element_t temp_tk1[], element_t temp_tk2[]) {
    start = pbc_get_time();

    element_pairing(temp_e[0], temp_pk1[0], dpk);  // e(pk1(1), dpk)
    element_pairing(temp_e[1], g, temp_tk1[0]);  // e(g, dpk^s)
    element_pairing(temp_e[2], h, temp_tk1[1]);  // e(h, dpk^t)
    element_mul(temp_e[3], temp_e[1], temp_e[2]);

    int b1 = !element_cmp(temp_e[0], temp_e[3]); // cmp returns 0 if a and b are the same, nonzero otherwise

    element_pairing(temp_e[4], temp_pk1[1], dpk);
    element_pairing(temp_e[5], g, temp_tk1[2]);
    element_pairing(temp_e[6], h, temp_tk1[3]);
    element_mul(temp_e[7], temp_e[5], temp_e[6]);

    int b2 = !element_cmp(temp_e[4], temp_e[7]);

    element_pairing(temp_e[8], temp_pk1[2], dpk);
    element_pairing(temp_e[9], g, temp_tk1[4]);
    element_pairing(temp_e[10], h, temp_tk1[5]);
    element_mul(temp_e[11], temp_e[9], temp_e[10]);

    int b3 = !element_cmp(temp_e[8], temp_e[11]);

    element_pairing(temp_e[12], temp_pk2[0], dpk);
    element_pairing(temp_e[13], g, temp_tk2[0]);
    element_pairing(temp_e[14], h, temp_tk2[1]);
    element_mul(temp_e[15], temp_e[13], temp_e[14]);

    int b4 = !element_cmp(temp_e[12], temp_e[15]);

    element_pairing(temp_e[16], temp_pk2[1], dpk);
    element_pairing(temp_e[17], g, temp_tk2[2]);
    element_pairing(temp_e[18], h, temp_tk2[3]);
    element_mul(temp_e[19], temp_e[18], temp_e[17]);

    int b5 = !element_cmp(temp_e[16], temp_e[19]);

    element_pairing(temp_e[20], temp_pk2[2], dpk);
    element_pairing(temp_e[21], g, temp_tk2[4]);
    element_pairing(temp_e[22], h, temp_tk2[5]);
    element_mul(temp_e[23], temp_e[22], temp_e[21]);

    int b6 = !element_cmp(temp_e[20], temp_e[23]);

    if (b1 && b2 && b3 && b4 && b5 && b6)
    {
        IVgen(ct1, ct2);
        printf("PTest success!\n");
    }
    else
    {
        printf("PTest failed!\n");
    }
    
    double pTest_time = pbc_get_time() - start;
    printf("PTest Time is: %.2f\n", pTest_time);
    
}

int dTest(element_t vt[]) {
    element_t temp[2];
    element_t alpha_t;

    for (i = 0; i < 2; i++)
    {
        element_init_GT(temp[i], pairing);
    }
    element_init_G2(alpha_t, pairing);

    element_neg(alpha_t, alpha);
    element_pow_zn(temp[0], v[3], alpha_t);

    if (element_cmp(v[2], temp[0]))
        return -1;
    else
    {
        element_pow_zn(temp[1], v[1], alpha_t);
        int res = !element_cmp(v[0], temp[1]);

        return res;
    }
}


int main(int argc, char **argv) {
    start = pbc_get_time();

    pbc_demo_pairing_init(pairing, argc, argv);

    element_init_G1(g, pairing);
    element_init_G1(h, pairing);
    element_init_G2(zeta, pairing);

    element_random(g);
    element_random(h);
    element_random(zeta);

    double setup_time = pbc_get_time() - start;
    printf("Setup time: %.2fs\n", setup_time);

    total_time += setup_time;

    init();

    dkg();

    ukg(pk1);
    tkg(tk1);

    ukg(pk2);
    tkg(tk2);

    enc(10, w1[0], w2[0], x[0], y[0], m_a, c_a);
    enc(10, w1[1], w2[1], x[1], y[1], m_b, c_b);

    // printf("Size of g: %d\n", element_length_in_bytes(g));
    // printf("Size of zeta: %d\n", element_length_in_bytes(zeta));
    // printf("Size of x: %d\n", element_length_in_bytes(x[0]));
    // printf("Size of a: %d\n", element_length_in_bytes(a));
    // element_printf("a is: %B\n", a);

    pTest(c_a, c_b, pk1, pk2, tk1, tk2);

    // IVgen(c_a, c_b);

    printf("DTest result: %d\n", dTest(v));

    printf("PKEOET Finish!\n");

    return 0;
}
