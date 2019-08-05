#include <pbc/pbc.h>
#include <pbc/pbc_test.h>
// #include <openssl/sha.h>
// #include <openssl/evp.h>
#include <string.h>
#include <gmp.h>
#include <stdio.h>
#include <stdlib.h>
#include <stdbool.h>

#define SIZE_M 1000


element_t  p, g, h, zeta;
element_t sk1[6], sk2[6];
element_t alpha, r;

element_t pk1[3], pk2[3];
element_t dpk, dsk;

element_t w1[2], w2[2], x[2], y[2];
element_t temp_x[3];
element_t temp_y[3];
element_t theta0, theta1, theta2;

element_t m_a[SIZE_M], m_b[SIZE_M];

element_t tk1[6], tk2[6];

element_t temp_e[24];

element_t omega1, omega2;
element_t v[4];
element_t temp_v[50];

pairing_t pairing;

int i = 0;
int n = 0;

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

struct IV
{
    element_t v1;
    element_t v2;
    element_t v3;
    element_t v4;
}iv[SIZE_M];



// char* get_hash(unsigned char tw1_s[], unsigned char tw2_s[], unsigned char tx_s[]) {
//     EVP_MD_CTX ctx;
 
//     unsigned char h_in_s[640];
//     static char theta_s[64];
//     int size_h_out;

//     int k;
//     for (k = 0; k < 640; k++)
//     {
//         if (k < 256) 
//             h_in_s[k] = tw1_s[k];
//         else if (k < 512)
//             h_in_s[k] = tw2_s[k-256];
//         else
//             h_in_s[k] = tx_s[k-512];
//     }
        
//     EVP_DigestInit(&ctx, EVP_sha512());
//     EVP_DigestUpdate(&ctx, h_in_s, 640);
//     EVP_DigestFinal(&ctx, theta_s, &size_h_out);

//     return theta_s;
// }

void init() {
    for (i = 0; i < 6; i++)
    {
        element_init_Zr(sk1[i], pairing);
        element_init_Zr(sk2[i], pairing);
    }

    element_init_Zr(alpha, pairing);
    element_init_Zr(r, pairing);

    for (i = 0; i < 6; i++)
    {
        element_init_G2(tk1[i], pairing);
        element_init_G2(tk2[i], pairing);
    }

    element_init_G1(temp_x[0], pairing);
    element_init_GT(temp_x[1], pairing);

    for (i = 0; i < 3; i++)
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

    for (i = 0; i < SIZE_M; i++)
    {
        element_init_GT(iv[i].v1, pairing);
        element_init_GT(iv[i].v2, pairing);
        element_init_GT(iv[i].v3, pairing);
        element_init_GT(iv[i].v4, pairing);
    }

    for (i = 0; i < 50; i++)
    {
        element_init_GT(temp_v[i], pairing);
    }

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
    
    element_t temp_2;
    element_init_G2(temp_2, pairing);
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

void ukg(element_t pk[], element_t sk[]) {
    for (i = 0; i < 3; i++)
    {
        element_init_G1(pk[i], pairing);
    }

    start = pbc_get_time();

    for (i = 0; i < 6; i++)
    {
        element_random(sk[i]);
    }

    element_pow2_zn(pk[0], g, sk[0], h, sk[1]);  // pk[0] = g^s * h^t
    element_pow2_zn(pk[1], g, sk[2], h, sk[3]);  // pk[1] = g^a * h^b
    element_pow2_zn(pk[2], g, sk[4], h, sk[5]);  // pk[2] = g^c * h^d

    double ukg_time = pbc_get_time() - start;

    printf("UKG Time: %.2fs\n", ukg_time);

    total_time += ukg_time;
}

void tkg(element_t temp_tk[], element_t temp_sk[]) {
    start = pbc_get_time();

    for (i = 0; i < 6; i++)
    {
        element_pow_zn(temp_tk[i], dpk, temp_sk[i]);  // tk[] = [dpk^s, dpk^t, dpk^a, dpk^b, dpk^c, dpk^d]
    }

    double tkg_time = pbc_get_time() - start;

    printf("TKG Time: %.2fs\n", tkg_time);

    total_time += tkg_time;
}

void enc(int num, element_t tpk[], element_t tm[], struct Ciphertext tc[]) {
    element_t w1_t, w2_t, x_t, y_t;

    element_init_G1(w1_t, pairing);
    element_init_G1(w2_t, pairing);
    element_init_GT(x_t, pairing);
    element_init_GT(y_t, pairing);

    start = pbc_get_time();

    if (num > SIZE_M) num = SIZE_M;
    n = num;

    for (i = 0; i < num; i++)
    {
        element_random(r);

        element_pow_zn(w1_t, g, r);  // w1 = g^r
        element_pow_zn(w2_t, h, r);  // w2 = h^r

        element_to_bytes(w1_s1, w1_t);
        element_to_bytes(w2_s1, w2_t);

        element_pow_zn(temp_x[0], tpk[0], r); // (g^s * h^t)^r
        element_pairing(temp_x[1], temp_x[0], zeta);  // x = e((g^s * h^t)^r, zeta)
        //element_mul(x_t, temp_x[1], tm[i]);  // x * m
        element_set(x_t, temp_x[1]);  // x
       
        element_to_bytes(x_s, x_t);

        // theta0_s = get_hash(w1_s1, w2_s1, x_s);
        // element_from_hash(theta0, theta0_s, 64);  // theta0 = H(w1, w2, x * m)
        //element_printf("Theta0 is: %B\n", theta0);
        element_random(theta0);

        element_pow_zn(temp_y[0], tpk[2], theta0);  //(g^c * h^d)^theta0

        element_mul(temp_y[1], tpk[1], temp_y[0]);  // (g^a * h^b) * (g^c * h^d)^theta0
        element_pow_zn(temp_y[2], temp_y[1], r);  // ((g^a * h^b) * (g^c * h^d)^theta0)^r

        element_pairing(y_t, temp_y[2], zeta);

        element_to_bytes(y_s, y_t);

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
        for (j = 0; j < 128; j++)
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

void IVgen(int ii, struct Ciphertext ct1[], struct Ciphertext ct2[], struct IV tiv[], element_t temp_tk1[], element_t temp_tk2[]) {
    element_random(omega1);
    element_random(omega2);

    element_set(w1[0], ct1[ii].c1);  // c = (w1, w2, x, y)
    element_set(w2[0], ct1[ii].c2);
    element_set(x[0], ct1[ii].c3);
    element_set(y[0], ct1[ii].c4);

    element_set(w1[1], ct2[ii].c1);
    element_set(w2[1], ct2[ii].c2);
    element_set(x[1], ct2[ii].c3);
    element_set(y[1], ct2[ii].c4);

    element_pow_zn(temp_v[0], x[0], omega1);  // x1^omega1
    element_pow_zn(temp_v[31], x[1], omega1);  // x2^omega1
    element_div(v[0], temp_v[0], temp_v[31]);  // v1

    element_pairing(temp_v[1], w1[0], temp_tk1[0]);  // e(w1(1), t1(1))
    element_pairing(temp_v[2], w1[1], temp_tk1[1]);  // e(w1(2), t1(2))
    element_mul(temp_v[3], temp_v[1], temp_v[2]);   // e(w1(1), t1(1)) * e(w1(2), t1(2))
    
    element_pairing(temp_v[4], w2[0], temp_tk2[0]);  // e(w2(1), t2(1))
    element_pairing(temp_v[5], w2[1], temp_tk2[1]);  // e(w2(2), t2(2))
    element_mul(temp_v[6], temp_v[4], temp_v[5]);  // e(w2(1), t2(1)) * e(w2(2), t2(2))

    element_pow_zn(temp_v[33], temp_v[3], omega1);
    element_pow_zn(temp_v[34], temp_v[6], omega1);
    element_div(v[1], temp_v[33], temp_v[34]);  // v2

    element_pow_zn(temp_v[38], v[0], alpha);

    element_pow_zn(temp_v[8], y[0], omega2);
    element_pow_zn(temp_v[28], y[1], omega2);
    element_div(v[2], temp_v[8], temp_v[28]); // v3

    element_to_bytes(w1_s2, w1[0]);
    element_to_bytes(w1_s3, w1[1]);
    element_to_bytes(w2_s2, w2[0]);
    element_to_bytes(w2_s3, w2[1]);
    element_to_bytes(x1_s, x[0]);
    element_to_bytes(x2_s, x[1]);

    // theta1_s = get_hash(w1_s2, w1_s3, x1_s);
    // element_from_hash(theta1, theta1_s, 64);  // theta1 = H(w1(1), w1(2), x1)

    // theta2_s = get_hash(w2_s2, w2_s3, x2_s);
    // element_from_hash(theta2, theta2_s, 64);  // theta2 = H(w2(1), w2(2), x2)

    element_random(theta1);
    element_random(theta2);

    element_pairing(temp_v[9], w1[0], temp_tk1[2]);  // e(w1(1), t1(3))
    element_pairing(temp_v[10], w1[1], temp_tk1[3]);  // e(w1(2), t1(4))
    element_mul(temp_v[11], temp_v[9], temp_v[10]);  // e(w1(1), t1(3)) * e(w1(2), t1(4))

    element_pairing(temp_v[12], w1[0], temp_tk1[4]);  // e(w1(1), t1(5))
    element_pairing(temp_v[13], w1[1], temp_tk1[5]);  // e(w1(2), t1(6))
    element_mul(temp_v[14], temp_v[12], temp_v[13]);  // e(w1(1), t1(5)) * e(w1(2), t1(6))
    element_pow_zn(temp_v[15], temp_v[14], theta1);  // (e(w1(1), t1(5)) * e(w1(2), t1(6)))^theta1
    element_mul(temp_v[27], temp_v[11], temp_v[15]);

    element_pairing(temp_v[18], w2[0], temp_tk2[2]);  // e(w2(1), t2(3))
    element_pairing(temp_v[19], w2[1], temp_tk2[3]);  // e(w2(2), t2(4))
    element_mul(temp_v[20], temp_v[18], temp_v[19]);  // e(w2(1), t2(3)) * e(w2(2), t2(4))

    element_pairing(temp_v[21], w2[0], temp_tk2[4]);  // e(w2(1), t2(5))
    element_pairing(temp_v[22], w2[1], temp_tk2[5]);  // e(w2(2), t2(6))
    element_mul(temp_v[23], temp_v[21], temp_v[22]);  // e(w2(1), t2(5)) * e(w2(2), t2(6))
    element_pow_zn(temp_v[24], temp_v[23], theta2);  // (e(w2(1), t2(5)) * e(w2(2), t2(6)))^theta2
    element_mul(temp_v[26], temp_v[20], temp_v[24]);

    element_pow_zn(temp_v[30], temp_v[27], omega2);
    element_pow_zn(temp_v[29], temp_v[26], omega2);
    element_div(v[3], temp_v[30], temp_v[29]);  // v4

    element_div(temp_v[35], temp_v[27], temp_v[26]);
    element_div(temp_v[36], y[0], y[1]);

    int diunilowmu = element_cmp(temp_v[26], y[0]);
    if (diunilowmu == 0)
    {
        printf("HHH\n");
    }
    

    element_set(tiv[ii].v1, v[0]);
    element_set(tiv[ii].v2, v[1]);
    element_set(tiv[ii].v3, v[2]);
    element_set(tiv[ii].v4, v[3]);

    element_pow_zn(temp_v[42], v[2], alpha);
    int diu = element_cmp(temp_v[36], temp_v[35]);
    if (diu == 0)
    {
        printf("Success!!!\n\n");
    }
    else
    {
        printf("Fail!!!\n\n");
    }
}

void pTest(int num, struct Ciphertext ct1[], struct Ciphertext ct2[], element_t temp_pk1[], element_t temp_pk2[], element_t ttk1[], element_t ttk2[]) {
    start = pbc_get_time();

    for (i = 0; i < num; i++)
    {
        element_pairing(temp_e[0], temp_pk1[0], dpk);  // e(pk1(1), dpk)
        element_pairing(temp_e[1], g, ttk1[0]);  // e(g, dpk^s)
        element_pairing(temp_e[2], h, ttk1[1]);  // e(h, dpk^t)
        element_mul(temp_e[3], temp_e[1], temp_e[2]);

        int b1 = !element_cmp(temp_e[0], temp_e[3]); // cmp returns 0 if a and b are the same, nonzero otherwise

        element_pairing(temp_e[4], temp_pk1[1], dpk);
        element_pairing(temp_e[5], g, ttk1[2]);
        element_pairing(temp_e[6], h, ttk1[3]);
        element_mul(temp_e[7], temp_e[5], temp_e[6]);

        int b2 = !element_cmp(temp_e[4], temp_e[7]);

        element_pairing(temp_e[8], temp_pk1[2], dpk);
        element_pairing(temp_e[9], g, ttk1[4]);
        element_pairing(temp_e[10], h, ttk1[5]);
        element_mul(temp_e[11], temp_e[9], temp_e[10]);

        int b3 = !element_cmp(temp_e[8], temp_e[11]);

        element_pairing(temp_e[12], temp_pk2[0], dpk);
        element_pairing(temp_e[13], g, ttk2[0]);
        element_pairing(temp_e[14], h, ttk2[1]);
        element_mul(temp_e[15], temp_e[13], temp_e[14]);

        int b4 = !element_cmp(temp_e[12], temp_e[15]);

        element_pairing(temp_e[16], temp_pk2[1], dpk);
        element_pairing(temp_e[17], g, ttk2[2]);
        element_pairing(temp_e[18], h, ttk2[3]);
        element_mul(temp_e[19], temp_e[18], temp_e[17]);

        int b5 = !element_cmp(temp_e[16], temp_e[19]);

        element_pairing(temp_e[20], temp_pk2[2], dpk);
        element_pairing(temp_e[21], g, ttk2[4]);
        element_pairing(temp_e[22], h, ttk2[5]);
        element_mul(temp_e[23], temp_e[22], temp_e[21]);

        int b6 = !element_cmp(temp_e[20], temp_e[23]);

        if (b1 && b2 && b3 && b4 && b5 && b6)
        {
            IVgen(i, ct1, ct2, iv, ttk1, ttk2);
        }
        else
        {
            printf("PTest failed!\n");
            break;
        }
    }
    
    
    double pTest_time = pbc_get_time() - start;
    printf("PTest %d Time is: %.2f\n", num, pTest_time);
    
}

int dTest(int ii, struct IV vt[]) {
    element_t temp[2];

    for (i = 0; i < 2; i++)
    {
        element_init_GT(temp[i], pairing);
    }

    element_pow_zn(temp[0], vt[ii].v3, alpha);  // v3^alpha

    if (element_cmp(vt[ii].v4, temp[0]) != 0)
        return -1;
    else
    {
        element_pow_zn(temp[1], vt[ii].v1, alpha);
        int res = !element_cmp(vt[ii].v2, temp[1]);

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

    element_printf("g is: %B\n\n", g);

    double setup_time = pbc_get_time() - start;
    printf("Setup time: %.2fs\n", setup_time);

    total_time += setup_time;

    init();

    dkg();

    ukg(pk1, sk1);
    tkg(tk1, sk1);

    ukg(pk2, sk2);
    tkg(tk2, sk2);

    enc(10, pk1, m_a, c_a);
    enc(10, pk2, m_a, c_b);

    pTest(n, c_a, c_b, pk1, pk2, tk1, tk2);

    printf("DTest result: %d\n", dTest(0, iv));

    printf("PKEOET Finish!\n");

    return 0;
}
