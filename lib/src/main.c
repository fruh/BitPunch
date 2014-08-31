#include <stdlib.h>
#include <time.h>
#include "arithmetics.h"
#include "debugio.h"
#include "init.h"
#include "operations.h"
#include "process.h"
#include "types.h"
#include "mceliece.h"

McEliece_Ctx mce_ctx;

void testPolyEval() {
    GF2_16x i;
    printDebug("##### TEST EVALUATION");

    printGf2xPoly(&mce_ctx.priv_key.g, &mce_ctx.a_data);

    for(i = 0; i < mce_ctx.a_data.ord; i++) {
        printDebug("alpha^(%d)", i);
        printBinaryLn(gf2xPolyEval(&mce_ctx.priv_key.g, mce_ctx.a_data.exp_table[i], &mce_ctx.a_data), 6);
    }
}

void testLogExpTables() {
    GF2_16x i;
    printf("TEST TABLES\n");
    for(i = 0; i < mce_ctx.a_data.ord; i++) {
        printf("%d: ", i);
        printBinary(mce_ctx.a_data.exp_table[i], 10);
        printf(" ");
        printBinaryLn(mce_ctx.a_data.log_table[i], 10);
    }   
}

void testMatrices() {
    printf("TEST MATRICES\n");
    // printf("matrix X:\n");
    // printGf2xMat(&(mce_ctx.h.x_m));
    // printf("\n\n");
    // printf("matrix Y:\n");
    // printGf2xMat(&(mce_ctx.h.y_m));
    // printf("\n\n");
    printf("matrix Z:\n");
    printGf2xMat(&(mce_ctx.priv_key.h_mat));
    printf("\n\n");
    // printf("matrix H:\n");
    // printGf2xMat(&(mce_ctx.priv_key.h_mat));
}

void testPermutationGF2m() {
    int size = 15;
    // int ret;
    Permutation_Vector permutation;
    permGenA(&permutation, size);
    fprintf(stderr, "\nTEST PERMUTATION GF2m\n");

    fprintf(stderr, "pseudonahodna permutacia:");
    printPerm(&permutation);
    fprintf(stderr, "\n\n");

    fprintf(stderr, "matica Z: \n\n");
    printGf2xMat(&(mce_ctx.priv_key.h_mat));

    // ret = gf2xMatPermute(&(mce_ctx.h.z_m), &permutation);
    // fprintf(stderr, "return z gf2MatPermute: %i\n\n",ret);
    fprintf(stderr, "spermutovana matica Z: \n\n");
    printGf2xMat(&(mce_ctx.priv_key.h_mat));
}

void testPermutationGF2() {
    fprintf(stderr, "\n\nTEST PERMUTATION GF2\n\n\n");
    int size = 6;
    int i, j, ret = -1, bit, max_bits;
    Matrix_GF2 m;
    Permutation_Vector permutation;
    Permutation_Vector permutation2;
    mallocPerm(&permutation, size);
    mallocPerm(&permutation2, size);
    permGenA(&permutation, size);
    

    // allocate matrix
    mallocMatrixGF2(&m, size, size);
    for (i = 0; i < size; i++) {
        permutation2.elements[i] = i;
    }
    permutation.elements[0] = 0;
    permutation.elements[1] = 2;
    permutation.elements[2] = 1;
    permutation.elements[3] = 3;
    permutation.elements[4] = 4;
    permutation.elements[5] = 5;

    // generate pseudorandom permutation
    fprintf(stderr, "pseudonahodna permutacia:");
    printPerm(&permutation);
    printPerm(&permutation2);
    fprintf(stderr, "\n");
    permPermute(&permutation2, &permutation);
    printPerm(&permutation2);
    fprintf(stderr, "\n");
    permPermute(&permutation2, &permutation);
    printPerm(&permutation2);

    // randomize the matrix
    for (i = 0; i < m.k; i++){ // row loop
        for (j = 0; j < m.elements_in_row; j++){ // column element loop
            // if it is the last element, it doesnt need to be full
            if (j == m.elements_in_row-1)
                max_bits = m.n % (m.element_bit_size);
            else
                max_bits = m.element_bit_size;
            for (bit = 0; bit < max_bits; bit++){ // element bit loop
                m.elements[i][j] ^= (((uint64_t)rand()%2 * 0x1) << bit);
            }
        }
    }

    // print matrix
    fprintf(stderr, "\nMatica:\n\n");
    printGf2Mat(&m);
    // transposeMatrixGF2(&m);
    // printf("Matica trans:\n\n");
    // printGf2Mat(&m);

    //permute
    // ret = gf2MatPermute(&m, permutation);
    printf("return z gf2MatPermute: %i\n\n",ret);

    // print matrix
    printf("spermutovana matica: \n\n");
    printGf2Mat(&m);
}

void testPermutationVectorGF2() {
    fprintf(stderr, "\n\nTEST PERMUTATION VECTOR GF2\n\n\n");
    int size = 6;
    Vector_GF2 vec;
    Permutation_Vector permutation;
    mallocPerm(&permutation, size);
    permGenA(&permutation, size);

    // generate pseudorandom permutation
    fprintf(stderr, "pseudonahodna permutacia:");
    printPerm(&permutation);
    fprintf(stderr, "\n");

    initRandVector(&vec, size, size/2);
    fprintf(stderr, "vektor: \n");
    printGf2Vec(&vec);

    gf2VecPermute(&vec, &permutation);


    fprintf(stderr, "spermutovany vektor: \n");
    printGf2Vec(&vec);
}

void testConvertGF2m_to_GF2() {
    // int i, j;

    fprintf(stderr, "\n\nTEST CONVERT MATRIX GF2m TO GF2\n\n\n");

    Matrix_GF2 convertedGF2;
    // Matrix_GF2_16x GF2m;

    // printf("matrix GF2m:\n");
    // mallocMatrix(&GF2m, 10,10);

    // for (i = 0; i < GF2m.k; i++) {
    //     for (j = 0; j < GF2m.n; j++) {
    //         GF2m.elements[i][j] = (uint16_t)rand()%0xf;
    //     }
    // }

    printGf2xMat(&(mce_ctx.priv_key.h_mat));
    printf("\n");

    gf2xMatConvertToGf2MatA(&convertedGF2, &(mce_ctx.priv_key.h_mat), 4);

    // printf("converted matrix GF2:\n");
    printGf2Mat(&convertedGF2);
}

void testCropMatrix() {
    printf("\n\nTEST CROP MATRIX\n\n\n");

    int i, j, ret, bit, max_bits;
    Matrix_GF2 m, cropped;

    // allocate matrix
    mallocMatrixGF2(&m, 5, 128);

    // randomize the matrix
    for (i = 0; i < m.k; i++){ // row loop
        for (j = 0; j < m.elements_in_row; j++){ // column element loop
            // if it is the last element, it doesnt need to be full
            if (j == m.elements_in_row-1)
                if (m.n % (m.element_bit_size) != 0)
                    max_bits = m.n % (m.element_bit_size);
                else
                    max_bits = m.element_bit_size;
            else
                max_bits = m.element_bit_size;
            for (bit = 0; bit < max_bits; bit++){ // element bit loop
                m.elements[i][j] ^= (((uint64_t)rand()%2 * 0x1) << bit);
            }
        }
    }

    // print matrix
    fprintf(stderr, "Matica:\n\n");
    printGf2Mat(&m);

    //permute
    ret = gf2MatCropA(&cropped, &m, 10);
    fprintf(stderr, "return z gf2MatCropA: %i\n\n",ret);

    // print matrix
    fprintf(stderr, "orezana matica: \n\n");
    printGf2Mat(&cropped);

}

void testGf2VecMatMul() {
    Matrix_GF2 convertedGF2;
    Vector_GF2 out, v;
    printDebug("Vector * Matrix multiplication over GF2");
    gf2xMatConvertToGf2MatA(&convertedGF2, &(mce_ctx.priv_key.h_mat), mce_ctx.pub_key.t);
    // gf2xMatConvertToGf2MatA(&convertedGF2, &(mce_ctx.priv_key.h_mat), 10);

    mallocVectorGF2(&v, convertedGF2.k);

    gf2VecNull(&v);
    v.elements[0] ^= 0x5;
    
    printDebug("GF2 vector v:");
    printGf2Vec(&v);

    printDebug("GF2 matrix M:");
    printGf2Mat(&convertedGF2);

    gf2VecMulMatA(&out, &v, &convertedGF2);

    printDebug("result 'v * M':");
    printGf2Vec(&out);
    fprintf(stderr, "\n");
}

void testIsSetBitMat() {
    Matrix_GF2 convertedGF2;
    gf2xMatConvertToGf2MatA(&convertedGF2, &(mce_ctx.priv_key.h_mat), mce_ctx.pub_key.t);
    // gf2xMatConvertToGf2MatA(&convertedGF2, &(mce_ctx.priv_key.h_mat), 10);

    fprintf(stderr, "converted matrix GF2:\n");
    printGf2Mat(&convertedGF2);
    printDebug("bit set (0, 2): %d", gf2MatGetBit(&convertedGF2, 0, 2));
    printDebug("bit set (5, 1): %d", gf2MatGetBit(&convertedGF2, 5, 1));
    printDebug("bit set (5, 3): %d", gf2MatGetBit(&convertedGF2, 5, 3));
    fprintf(stderr, "\n");
    printDebug("bit set (2, 2): %d", gf2MatGetBit(&convertedGF2, 2, 2));
    printDebug("set bit at (2, 2)");
    gf2MatSetBit(&convertedGF2, 2, 2, 1);
    printDebug("bit set (2, 2): %d", gf2MatGetBit(&convertedGF2, 2, 2));
    printDebug("unset bit at (2, 2)");
    gf2MatSetBit(&convertedGF2, 2, 2, 0);
    printDebug("bit set (2, 2): %d", gf2MatGetBit(&convertedGF2, 2, 2));

    freeMatGF2(&convertedGF2, 0);

    fprintf(stderr, "\n");
}

void testGetInvPerm() {
    Permutation_Vector p, inv;

    permGenA(&p, 5);

    printDebug("Permutation:");
    printPerm(&p);

    permGetInvA(&inv, &p);
    printDebug("Inverted permutation:");
    printPerm(&inv);

    freePerm(&p, 0);
    freePerm(&inv, 0);
    fprintf(stderr, "\n");
}

void testTransMatGF2() {
    Matrix_GF2 convertedGF2, tran;
    gf2xMatConvertToGf2MatA(&convertedGF2, &(mce_ctx.priv_key.h_mat), mce_ctx.pub_key.t);

    fprintf(stderr, "converted matrix GF2:\n");
    printGf2Mat(&convertedGF2);
    
    gf2MatTranspA(&tran, &convertedGF2);
    fprintf(stderr, "\n");
    printDebug("transposed matrix");
    printGf2Mat(&tran);

    freeMatGF2(&convertedGF2, 0);
    freeMatGF2(&tran, 0);

    fprintf(stderr, "\n");
}

void testAppendIdentityMatrix() {
    Matrix_GF2 convertedGF2, appended;
    gf2xMatConvertToGf2MatA(&convertedGF2, &(mce_ctx.priv_key.h_mat), mce_ctx.pub_key.t);

    printDebug("matrix GF2:\n");
    printGf2Mat(&convertedGF2);

    gf2MatAppendIdenityA(&appended, &convertedGF2);

    fprintf(stderr, "\n");
    printDebug("Appended identity matrix:");
    printGf2Mat(&appended);

    freeMatGF2(&convertedGF2, 0);
    freeMatGF2(&appended, 0);

    fprintf(stderr, "\n");
}

void testGH() {
    Matrix_GF2 g_mat, h_matat, trans;
    Vector_GF2 tmp_g_row, result;
    Permutation_Vector *perm;
    Permutation_Vector inv_perm;
    int i;

    gf2MatAppendIdenityA(&g_mat, &(mce_ctx.pub_key.g_mat)); 
    // fprintf(stderr, "Matrix G:\n");
    // printGf2Mat(&tmp);

    gf2xMatConvertToGf2MatA(&h_matat, &(mce_ctx.priv_key.h_mat), gf2xGetDeg(mce_ctx.a_data.mod));
    perm = gf2MatMakeSystematicA(&h_matat);
    permGetInvA(&inv_perm, perm);

    gf2MatPermute(&g_mat, &inv_perm);
    fprintf(stderr, "Permuted Matrix G:\n");
    printGf2Mat(&g_mat);


    // fprintf(stderr, "Matrix H:\n");
    // printGf2Mat(&h_matat);
    gf2xMatConvertToGf2MatA(&h_matat, &(mce_ctx.priv_key.h_mat), gf2xGetDeg(mce_ctx.a_data.mod));
    gf2MatTranspA(&trans, &h_matat);

    fprintf(stderr, "Transposed Matrix H:\n");
    printGf2Mat(&trans);

    fprintf(stderr, "\n");
    printDebug("G: %dx%d H: %dx%d", g_mat.k, g_mat.n, trans.k, trans.n);

    // G x H
    mallocVectorGF2(&tmp_g_row, g_mat.n);
    mallocVectorGF2(&result, trans.n);

    // printGf2Vec(&tmp_g_row);
    // printGf2Vec(&result);

    for (i = 0; i < g_mat.k; i++) {
        gf2MatGetRowAsGf2Vec(&tmp_g_row, &g_mat, i, 0);

        // printGf2Vec(&tmp_g_row);
        gf2VecMulMat(&result, &tmp_g_row, &trans);
        printGf2Vec(&result);
    }
    freeMatGF2(&g_mat, 0);
    freeMatGF2(&h_matat, 0);
    freeMatGF2(&trans, 0);
    freeVecGF2(&tmp_g_row, 0);
    freeVecGF2(&result, 0);
    freePerm(perm, 0);
    freePerm(&inv_perm, 0);

    fprintf(stderr, "\n");
}

void testEncrypt() {
    Vector_GF2 plaintext;
    mallocVectorGF2(&plaintext, 3);
    plaintext.elements[0] = 0x7;

    Vector_GF2 ciphertext;

    fprintf(stderr, "plaintext:\n");
    printGf2Vec(&plaintext);
    
    if (mceEncryptA(&ciphertext, &plaintext, &mce_ctx) == 0) {
        fprintf(stderr, "ciphertext:\n");
        printGf2Vec(&ciphertext);
        freeVecGF2(&ciphertext, 0);
    }
    else
        fprintf(stderr, "encryption error\n");
    freeVecGF2(&plaintext, 0);
}

void testCrop() {
    Vector_GF2 in;
    Vector_GF2 out;

    initRandVector(&in, 1024, 512);
    fprintf(stderr, "vstupny vektor: ");
    printGf2Vec(&in);
    gf2VecCropA(&out, &in, 1, 1022);
    fprintf(stderr, "orezany vektor: ");
    printGf2Vec(&out);
}

void testPoly() {
    Polynomial_GF2_16x p1;
    Polynomial_GF2_16x p2;
    // Polynomial_GF2_16x quotient, reminder;
    Polynomial_GF2_16x out;
    Polynomial_GF2_16x gcd;
    Polynomial_GF2_16x s;
    Polynomial_GF2_16x t;
    Polynomial_GF2_16x tmp_1;
    Polynomial_GF2_16x tmp_2;
    Polynomial_GF2_16x tmp_3;
    Polynomial_GF2_16x a, b;

    mallocPoly(&p1, 10);
    p1.coef[3] = mce_ctx.a_data.exp_table[0];
    p1.coef[2] = mce_ctx.a_data.exp_table[20];
    p1.coef[1] = mce_ctx.a_data.exp_table[8];
    p1.coef[0] = mce_ctx.a_data.exp_table[2];
    p1.deg = gf2xPolyGetDeg(&p1);

    mallocPoly(&p2, 10);
    p2.coef[4] = 1;//mce_ctx.a_data.exp_table[6];
    p2.coef[0] = mce_ctx.a_data.exp_table[6];

    p2.deg = gf2xPolyGetDeg(&p2);

    mallocPoly(&out, 1);
    gf2xPolyAdd(&out, &p1, &p2);

    // fprintf(stderr, "\n******nasobenie*******\n");
    // gf2xPolyMul(&out, &p2, &p1, &mce_ctx.a_data);
    // fprintf(stderr, "f:\n");
    // printGf2xPoly(&p1, &mce_ctx.a_data);
    // fprintf(stderr, "g:\n");
    // printGf2xPoly(&p2, &mce_ctx.a_data);
    // fprintf(stderr, "f x g:\n");
    // printGf2xPoly(&out, &mce_ctx.a_data);

    // mallocPoly(&quotient, 50);
    // mallocPoly(&reminder, 50);

    // fprintf(stderr, "\n******delenie*******\n");
    // gf2xPolyDiv(&quotient, &reminder, &p2, &p1, &mce_ctx.a_data);
    // fprintf(stderr, "f:\n");
    // printGf2xPoly(&p2, &mce_ctx.a_data);
    // fprintf(stderr, "g:\n");
    // printGf2xPoly(&p1, &mce_ctx.a_data);
    // fprintf(stderr, "f : g = q + r:\n");
    // fprintf(stderr, "q\n");
    // printGf2xPoly(&quotient, &mce_ctx.a_data);
    // fprintf(stderr, "r\n");
    // printGf2xPoly(&reminder, &mce_ctx.a_data);




    fprintf(stderr, "*********Euclidean 1**********\n");
    // mallocPoly(&s, mce_ctx.priv_key.g.deg);
    // mallocPoly(&t, mce_ctx.priv_key.g.deg);
    // mallocPoly(&gcd, mce_ctx.priv_key.g.deg);
    gf2xPolyExtEuclidA(&gcd, &s, &t, &mce_ctx.priv_key.g, &p1, 0, &mce_ctx.a_data);
    
    fprintf(stderr, "\nGoppa:\n");
    printGf2xPoly(&mce_ctx.priv_key.g, &mce_ctx.a_data);
    fprintf(stderr, "\np1:\n");
    printGf2xPoly(&p1, &mce_ctx.a_data);
    fprintf(stderr, "\ns:\n");
    printGf2xPoly(&s, &mce_ctx.a_data);
    fprintf(stderr, "\nt:\n");
    printGf2xPoly(&t, &mce_ctx.a_data);
    fprintf(stderr, "\ngcd:\n");
    printGf2xPoly(&gcd, &mce_ctx.a_data);

    mallocPoly(&tmp_1, 30);
    mallocPoly(&tmp_2, 30);
    mallocPoly(&tmp_3, 30);

    gf2xPolyMul(&tmp_1, &mce_ctx.priv_key.g, &s, &mce_ctx.a_data);
    gf2xPolyMul(&tmp_2, &p1, &t, &mce_ctx.a_data);

    fprintf(stderr, "\nmce x s:\n");
    printGf2xPoly(&tmp_1, &mce_ctx.a_data);


    fprintf(stderr, "\np1 x t:\n");
    printGf2xPoly(&tmp_2, &mce_ctx.a_data);

    gf2xPolyAdd(&tmp_3, &tmp_1, &tmp_2);
    fprintf(stderr, "\noverenie\n");
    printGf2xPoly(&tmp_3, &mce_ctx.a_data);

    freePoly(&gcd, 0);
    freePoly(&s, 0);
    freePoly(&t, 0);


    fprintf(stderr, "*********Euclidean 2**********\n");
    // mallocPoly(&s, mce_ctx.priv_key.g.deg);
    // mallocPoly(&t, mce_ctx.priv_key.g.deg);
    // mallocPoly(&gcd, mce_ctx.priv_key.g.deg);
    fprintf(stderr, "\nGoppa:\n");
    printGf2xPoly(&mce_ctx.priv_key.g, &mce_ctx.a_data);
    fprintf(stderr, "\np1:\n");
    printGf2xPoly(&p1, &mce_ctx.a_data);
    gf2xPolyExtEuclidA(&gcd, &s, &t, &p1, &mce_ctx.priv_key.g, 0, &mce_ctx.a_data);
    
    fprintf(stderr, "\nGoppa:\n");
    printGf2xPoly(&mce_ctx.priv_key.g, &mce_ctx.a_data);
    fprintf(stderr, "\np1:\n");
    printGf2xPoly(&p1, &mce_ctx.a_data);
    fprintf(stderr, "\ns:\n");
    printGf2xPoly(&s, &mce_ctx.a_data);
    fprintf(stderr, "\nt:\n");
    printGf2xPoly(&t, &mce_ctx.a_data);
    fprintf(stderr, "\ngcd:\n");
    printGf2xPoly(&gcd, &mce_ctx.a_data);

    gf2xPolyMul(&tmp_1, &p1, &s, &mce_ctx.a_data);
    gf2xPolyMul(&tmp_2, &mce_ctx.priv_key.g, &t, &mce_ctx.a_data);

    fprintf(stderr, "\np1 x s:\n");
    printGf2xPoly(&tmp_1, &mce_ctx.a_data);


    fprintf(stderr, "\nmce x t:\n");
    printGf2xPoly(&tmp_2, &mce_ctx.a_data);

    gf2xPolyAdd(&tmp_3, &tmp_1, &tmp_2);
    fprintf(stderr, "\noverenie\n");
    printGf2xPoly(&tmp_3, &mce_ctx.a_data);

    freePoly(&gcd, 0);
    freePoly(&s, 0);
    freePoly(&t, 0);

    fprintf(stderr, "*********Euclidean 3**********\n");
    mallocPoly(&a, 10);
    a.coef[4] = mce_ctx.a_data.exp_table[0];
    a.deg = gf2xPolyGetDeg(&a);      

    mallocPoly(&b, 10);
    b.coef[2] = mce_ctx.a_data.exp_table[2];
    b.deg = gf2xPolyGetDeg(&b);
    gf2xPolyExtEuclidA(&gcd, &s, &t, &a, &b, 0, &mce_ctx.a_data);
    
    fprintf(stderr, "a:\n");
    printGf2xPoly(&a, &mce_ctx.a_data);
    fprintf(stderr, "b:\n");
    printGf2xPoly(&b, &mce_ctx.a_data);
    fprintf(stderr, "s:\n");
    printGf2xPoly(&s, &mce_ctx.a_data);
    fprintf(stderr, "t:\n");
    printGf2xPoly(&t, &mce_ctx.a_data);
    fprintf(stderr, "gcd:\n");
    printGf2xPoly(&gcd, &mce_ctx.a_data);

    gf2xPolyMul(&tmp_1, &a, &s, &mce_ctx.a_data);
    gf2xPolyMul(&tmp_2, &b, &t, &mce_ctx.a_data);

    fprintf(stderr, "a x s:\n");
    printGf2xPoly(&tmp_1, &mce_ctx.a_data);


    fprintf(stderr, "b x t:\n");
    printGf2xPoly(&tmp_2, &mce_ctx.a_data);

    gf2xPolyAdd(&tmp_3, &tmp_1, &tmp_2);
    fprintf(stderr, "overenie\n");
    printGf2xPoly(&tmp_3, &mce_ctx.a_data);

    freePoly(&p1, 0);
    freePoly(&p2, 0);
    freePoly(&out, 0);
    freePoly(&gcd, 0);
    freePoly(&s, 0);
    freePoly(&t, 0);
    freePoly(&tmp_1, 0);
    freePoly(&tmp_2, 0);
    freePoly(&tmp_3, 0);

    return;
}

void printLogExpTable() {
    int i;
    printDebug("Exponential table:");

    for (i = 0; i < mce_ctx.a_data.ord; i++) {
        fprintf(stderr, "%4d: exp = %4x  (dec) %4d\n", i, mce_ctx.a_data.exp_table[i], mce_ctx.a_data.exp_table[i]);//, mce_ctx.a_data.exp_table[i], mce_ctx.a_data.log_table[mce_ctx.a_data.exp_table[i]]);
// \tlog[%4x] = %4x\n
    }
}

void testPolyDiv() {
    Polynomial_GF2_16x a, b, q, r;

    printDebug("############# Polynomial division test: a / b = q + r ################");

    mallocPoly(&a, 10);
    mallocPoly(&b, 10);
    mallocPoly(&q, 10);
    mallocPoly(&r, 10);

    printDebug("Test #1");
    a.coef[0] = mce_ctx.a_data.exp_table[11];
    a.coef[1] = mce_ctx.a_data.exp_table[26];
    a.coef[2] = mce_ctx.a_data.exp_table[8];
    a.deg = gf2xPolyGetDeg(&a);      

    b.coef[0] = mce_ctx.a_data.exp_table[9];
    b.coef[1] = mce_ctx.a_data.exp_table[3];
    b.deg = gf2xPolyGetDeg(&b);

    gf2xPolyDiv(&q, &r, &a, &b, &mce_ctx.a_data);
    printGf2xPoly(&a, &mce_ctx.a_data);
    printGf2xPoly(&b, &mce_ctx.a_data);
    printGf2xPoly(&q, &mce_ctx.a_data);
    printGf2xPoly(&r, &mce_ctx.a_data);

    printDebug("Test #2");
    gf2xPolyDiv(&q, &r, &mce_ctx.priv_key.g, &b, &mce_ctx.a_data);
    printGf2xPoly(&mce_ctx.priv_key.g, &mce_ctx.a_data);
    printGf2xPoly(&b, &mce_ctx.a_data);
    printGf2xPoly(&q, &mce_ctx.a_data);
    printGf2xPoly(&r, &mce_ctx.a_data);

    printDebug("Test #3");
    gf2xPolyNull(&a);
    gf2xPolyNull(&b);
    a.coef[4] = mce_ctx.a_data.exp_table[0];
    a.deg = gf2xPolyGetDeg(&a);      

    b.coef[2] = mce_ctx.a_data.exp_table[2];
    b.deg = gf2xPolyGetDeg(&b);

    gf2xPolyDiv(&q, &r, &a, &b, &mce_ctx.a_data);
    printGf2xPoly(&a, &mce_ctx.a_data);
    printGf2xPoly(&b, &mce_ctx.a_data);
    printGf2xPoly(&q, &mce_ctx.a_data);
    printGf2xPoly(&r, &mce_ctx.a_data);

    freePoly(&a, 0);
    freePoly(&b, 0);
    freePoly(&q, 0);
    freePoly(&r, 0);
}

void genPolyTest() {
    gf2xPolyGenGoppaA(&mce_ctx.priv_key.g, 50, &mce_ctx.a_data);
    fprintf(stderr, "generovany polynom\n");
    printGf2xPoly(&mce_ctx.priv_key.g, &mce_ctx.a_data); 
}

void testPolyShift() {
    Polynomial_GF2_16x a, b;

    printDebug("################ Polynomial shift test: #################");

    mallocPoly(&a, 10);
    mallocPoly(&b, 10);

    a.coef[0] = mce_ctx.a_data.exp_table[11];
    a.coef[1] = mce_ctx.a_data.exp_table[26];
    a.coef[2] = mce_ctx.a_data.exp_table[8];
    a.deg = gf2xPolyGetDeg(&a);      

    b.coef[0] = mce_ctx.a_data.exp_table[9];
    b.coef[1] = mce_ctx.a_data.exp_table[3];
    b.deg = gf2xPolyGetDeg(&b);

    printDebug("max degree: %d", a.max_deg);
    printDebug("shift polynomial 3 times left and then 7 times right and then 2 times left");
    printGf2xPoly(&a, &mce_ctx.a_data);
        
    gf2xPolyShl(&a, 3);
    printGf2xPoly(&a, &mce_ctx.a_data);

    gf2xPolyShr(&a, 7);
    printGf2xPoly(&a, &mce_ctx.a_data);


    gf2xPolyShl(&a, 2);
    printGf2xPoly(&a, &mce_ctx.a_data);
    

    printDebug("shift polynomial 15 times left");
    printGf2xPoly(&b, &mce_ctx.a_data);

    gf2xPolyShl(&b, 15);
    printGf2xPoly(&b, &mce_ctx.a_data);
    
    freePoly(&a, 0);
    freePoly(&b, 0);
}

void testPolyMod() {
    Polynomial_GF2_16x a, b, r;

    printDebug("########### Polynomial modulo test: a mod b = r #############");

    mallocPoly(&a, 10);
    mallocPoly(&b, 10);
    mallocPoly(&r, 10);

    printDebug("Test #1:");
    gf2xPolyNull(&a);
    gf2xPolyNull(&b);
    a.coef[0] = mce_ctx.a_data.exp_table[11];
    a.coef[1] = mce_ctx.a_data.exp_table[26];
    a.coef[2] = mce_ctx.a_data.exp_table[8];
    a.deg = gf2xPolyGetDeg(&a);      

    b.coef[0] = mce_ctx.a_data.exp_table[9];
    b.coef[1] = mce_ctx.a_data.exp_table[3];
    b.deg = gf2xPolyGetDeg(&b);

    gf2xPolyMod(&r, &a, &b, &mce_ctx.a_data);
    printGf2xPoly(&a, &mce_ctx.a_data);
    printGf2xPoly(&b, &mce_ctx.a_data);
    printGf2xPoly(&r, &mce_ctx.a_data);

    printDebug("Test #2:");
    gf2xPolyNull(&a);
    gf2xPolyNull(&b);
    a.coef[0] = mce_ctx.a_data.exp_table[5];
    a.coef[1] = mce_ctx.a_data.exp_table[2];
    a.coef[2] = mce_ctx.a_data.exp_table[8];
    a.coef[3] = mce_ctx.a_data.exp_table[20];
    a.coef[4] = mce_ctx.a_data.exp_table[0];
    a.deg = gf2xPolyGetDeg(&a);      

    b.coef[0] = mce_ctx.a_data.exp_table[5];
    b.coef[3] = mce_ctx.a_data.exp_table[5];
    b.deg = gf2xPolyGetDeg(&b);

    gf2xPolyMod(&r, &a, &b, &mce_ctx.a_data);
    printGf2xPoly(&a, &mce_ctx.a_data);
    printGf2xPoly(&b, &mce_ctx.a_data);
    printGf2xPoly(&r, &mce_ctx.a_data);

    printDebug("Test #3:");
    gf2xPolyNull(&a);
    gf2xPolyNull(&b);
    a.coef[0] = mce_ctx.a_data.exp_table[5];
    a.coef[3] = mce_ctx.a_data.exp_table[5];
    a.deg = gf2xPolyGetDeg(&a);      

    b.coef[0] = mce_ctx.a_data.exp_table[11];
    b.coef[1] = mce_ctx.a_data.exp_table[26];
    b.coef[2] = mce_ctx.a_data.exp_table[8];
    b.deg = gf2xPolyGetDeg(&b);

    gf2xPolyMod(&r, &a, &b, &mce_ctx.a_data);
    printGf2xPoly(&a, &mce_ctx.a_data);
    printGf2xPoly(&b, &mce_ctx.a_data);
    printGf2xPoly(&r, &mce_ctx.a_data);

    printDebug("Test #4:");
    gf2xPolyNull(&a);
    gf2xPolyNull(&b);
    a.coef[0] = mce_ctx.a_data.exp_table[5];
    a.coef[3] = mce_ctx.a_data.exp_table[5];
    a.deg = gf2xPolyGetDeg(&a);      

    b.coef[0] = mce_ctx.a_data.exp_table[11];
    b.coef[1] = mce_ctx.a_data.exp_table[26];
    b.coef[5] = mce_ctx.a_data.exp_table[8];
    b.deg = gf2xPolyGetDeg(&b);

    gf2xPolyMod(&r, &a, &b, &mce_ctx.a_data);
    printGf2xPoly(&a, &mce_ctx.a_data);
    printGf2xPoly(&b, &mce_ctx.a_data);
    printGf2xPoly(&r, &mce_ctx.a_data);

    printDebug("Test #5:");
    gf2xPolyNull(&b);   

    b.coef[0] = mce_ctx.a_data.exp_table[11];
    b.coef[1] = mce_ctx.a_data.exp_table[26];
    b.coef[4] = mce_ctx.a_data.exp_table[8];
    b.deg = gf2xPolyGetDeg(&b);

    gf2xPolyMod(&r, &mce_ctx.priv_key.g, &b, &mce_ctx.a_data);
    printGf2xPoly(&mce_ctx.priv_key.g, &mce_ctx.a_data);
    printGf2xPoly(&b, &mce_ctx.a_data);
    printGf2xPoly(&r, &mce_ctx.a_data);

    printDebug("Test #6:");
    gf2xPolyNull(&b);   

    b.coef[0] = mce_ctx.a_data.exp_table[11];
    b.deg = gf2xPolyGetDeg(&b);

    gf2xPolyMod(&r, &mce_ctx.priv_key.g, &b, &mce_ctx.a_data);
    printGf2xPoly(&mce_ctx.priv_key.g, &mce_ctx.a_data);
    printGf2xPoly(&b, &mce_ctx.a_data);
    printGf2xPoly(&r, &mce_ctx.a_data);

    freePoly(&a, 0);
    freePoly(&b, 0);
    freePoly(&r, 0);
}

void testPolyPower() {
    Polynomial_GF2_16x a;

    printDebug("Polynomial power test:");

    mallocPoly(&a, 10);

    a.coef[0] = mce_ctx.a_data.exp_table[11];
    a.coef[1] = mce_ctx.a_data.exp_table[26];
    a.coef[2] = mce_ctx.a_data.exp_table[8];
    a.deg = gf2xPolyGetDeg(&a);   

    printGf2xPoly(&a, &mce_ctx.a_data);
    gf2xPolyPower(&a, 2, &mce_ctx.a_data);
    printGf2xPoly(&a, &mce_ctx.a_data);

    freePoly(&a, 0);
}

void testPolyInv() {
    Polynomial_GF2_16x a, b, inv_a;

    printDebug("########### Polynomial inverse test: a * inv_a = 1 mod m #############");

    mallocPoly(&a, 10);
    mallocPoly(&b, 10);

    printDebug("Test #1:");
    gf2xPolyNull(&a);
    gf2xPolyNull(&b);
    a.coef[0] = mce_ctx.a_data.exp_table[11];
    a.coef[1] = mce_ctx.a_data.exp_table[26];
    a.coef[2] = mce_ctx.a_data.exp_table[8];
    a.deg = gf2xPolyGetDeg(&a);

    b.coef[0] = mce_ctx.a_data.exp_table[14];
    b.coef[1] = mce_ctx.a_data.exp_table[28];
    b.coef[2] = mce_ctx.a_data.exp_table[5];
    // b.coef[3] = mce_ctx.a_data.exp_table[15];
    b.deg = gf2xPolyGetDeg(&b);

    gf2xPolyInvA(&inv_a, &a, &mce_ctx.priv_key.g, &mce_ctx.a_data);

    fprintf(stderr, "polynomial a:\n");
    printGf2xPoly(&a, &mce_ctx.a_data);

    fprintf(stderr, "polynomial b:\n");
    printGf2xPoly(&mce_ctx.priv_key.g, &mce_ctx.a_data);

    fprintf(stderr, "inverse polynomial\n");
    printGf2xPoly(&inv_a, &mce_ctx.a_data);
    printGf2xPoly(&b, &mce_ctx.a_data);

    freePoly(&a, 0);
    freePoly(&inv_a, 0);
    freePoly(&b, 0);
}

void testIrredPoly() {
    Polynomial_GF2_16x p1;
    Polynomial_GF2_16x p2;
    Polynomial_GF2_16x gcd;
    Polynomial_GF2_16x s;
    Polynomial_GF2_16x t;
    int i, j, k, l, m, n, o, p, q;

    mallocPoly(&p1, 4);
    // p1.coef[3] = mce_ctx.a_data.exp_table[15];
    // p1.coef[0] = mce_ctx.a_data.exp_table[10];
    // p1.deg = gf2xPolyGetDeg(&p1);

    mallocPoly(&p2, 4);
    p2.coef[4] = 1;
    p2.deg = 4;

    q = 0;

    fprintf(stderr, "*********Find irred poly (brute force)**********\n");
    for (i = 0; i < mce_ctx.a_data.ord; i++){
        p2.coef[3] = i;
        for (j = 0; j < mce_ctx.a_data.ord; j++){
            p2.coef[2] = j;
            printDebug("Step: %d", ++q);
            for (k = 0; k < mce_ctx.a_data.ord; k++){
                p2.coef[1] = k;
                for (l = 0; l < mce_ctx.a_data.ord; l++){
                    p2.coef[0] = l;

                    for (m = 0; m < mce_ctx.a_data.ord; m++){
                        p1.coef[3] = m;
                        for (n = 0; n < mce_ctx.a_data.ord; n++){
                            p1.coef[2] = n;
                            for (o = 0; o < mce_ctx.a_data.ord; o++){
                                p1.coef[1] = o;
                                for (p = 0; p < mce_ctx.a_data.ord; p++){
                                    p1.coef[0] = p;
                                    p1.deg = gf2xPolyGetDeg(&p1);
                                    if (p1.deg == -1) {
                                        continue;
                                    }
                                    
                                    gf2xPolyExtEuclidA(&gcd, &s, &t, &p2, &p1, 0, &mce_ctx.a_data);

                                    if (gcd.deg != 0 || gcd.coef[0] != 1) {
                                        // printGf2xPoly(&p2, &mce_ctx.a_data);
                                        // printGf2xPoly(&p1, &mce_ctx.a_data);
                                        // printGf2xPoly(&gcd, &mce_ctx.a_data);
                                        // printGf2xPoly(&s, &mce_ctx.a_data);
                                        // printGf2xPoly(&t, &mce_ctx.a_data);

                                        freePoly(&gcd, 0);
                                        freePoly(&s, 0);
                                        freePoly(&t, 0);
                                        goto other;
                                    }else if (!(m == mce_ctx.a_data.ord - 1 &&
                                                n == mce_ctx.a_data.ord - 1 &&
                                                    o == mce_ctx.a_data.ord - 1 &&
                                                        p == mce_ctx.a_data.ord - 1 )){
                                        freePoly(&gcd, 0);
                                        freePoly(&s, 0);
                                        freePoly(&t, 0);
                                    }
                                }
                            }
                        }
                    }
                    printDebug("found irred:");
                    printGf2xPoly(&p2, &mce_ctx.a_data);
                    printGf2xPoly(&p1, &mce_ctx.a_data);
                    printGf2xPoly(&gcd, &mce_ctx.a_data);
                    // printGf2xPoly(&s, &mce_ctx.a_data);
                    // printGf2xPoly(&t, &mce_ctx.a_data);
                    freePoly(&gcd, 0);
                    freePoly(&s, 0);
                    freePoly(&t, 0);
                    other:
                    ;
                }
            }
        }
    }
    freePoly(&p1, 0);
    freePoly(&p2, 0);
    return;
}

void testIrred() {
    
}

void testDetermineSyndrome() {
    /*
    Polynomial_GF2_16x syndrome, syndrome2, squared;
    // Matrix_GF2_16x sqrtMatrix;
    Vector_GF2 z;
    // Vector_GF2_16x vec, vec_out;
    // int i, j, k;
    int i;

    // printGf2Mat(&mce_ctx.priv_key.H);

    mallocVectorGF2(&z, mce_ctx.priv_key.H.k);
    for (i = 0; i < z.len; i++)
        gf2VecSetBit(&z, i, 1);
    // printDebug("Z:");
    // printGf2Vec(&z);

    //mising determine
    // printDebug("syndrome:");
    // printGf2xPoly(&syndrome, &mce_ctx.a_data);

    // printDebug("syndrome by elements:");
    // for (i = 0; i <= syndrome.deg; i++) {
    //     printBinary(syndrome.coef[i], mce_ctx.m);//mce_ctx.m);
    //     fprintf(stderr, "\n");
    // }



    // SQRT part
    mallocPoly(&syndrome2, 10);
    mallocPoly(&squared, 10);


    gf2xPolyNull(&syndrome);
    gf2xPolyNull(&syndrome2);
    syndrome.coef[2] = 2;
    syndrome.coef[1] = 1;
    syndrome.coef[0] = 1;
    syndrome.deg = gf2xPolyGetDeg(&syndrome);

    fprintf(stderr, "SQUARE part\n");
    gf2xPolyRoot(&squared, &syndrome, &(mce_ctx.priv_key.g), &(mce_ctx.a_data));

    fprintf(stderr, "\ngoppa:\n");
    printGf2xPoly(&(mce_ctx.priv_key.g), &(mce_ctx.a_data));

    fprintf(stderr, "\nsyndrome:\n");    
    printGf2xPoly(&syndrome, &(mce_ctx.a_data));
    
    fprintf(stderr, "\nsquared syndrome:\n");
    printGf2xPoly(&squared, &(mce_ctx.a_data));


    gf2xPolyMul(&syndrome2, &squared, &squared, &(mce_ctx.a_data));
    // gf2xPolyCopy(&syndrome, &syndrome2);
    gf2xPolyMod(&syndrome, &syndrome2, &mce_ctx.priv_key.g, &(mce_ctx.a_data));
    fprintf(stderr, "\n(squared syndrome)^2:\n");
    printGf2xPoly(&syndrome, &(mce_ctx.a_data));


    // gf2xPolyNull(&syndrome2);
    // gf2xPolyNull(&syndrome);
    // fprintf(stderr, "real square:\n");
    // for(i = 0; i < mce_ctx.a_data.ord; i++) {
    //     gf2xPolyNull(&syndrome);
    //     gf2xPolyNull(&syndrome2);
    //     for (j = 0; j < mce_ctx.a_data.ord; j++) {
    //         syndrome.coef[1] = i;
    //         syndrome.coef[0] = j;
    //         syndrome.deg = 1;
    //         gf2xPolyMul(&syndrome2, &syndrome, &syndrome, &(mce_ctx.a_data));
    //         gf2xPolyMod(&syndrome, &syndrome2, &(mce_ctx.priv_key.g), &(mce_ctx.a_data));
    //         if (syndrome.coef[1] == 1 && syndrome.coef[0] == 0) {
    //         printGf2xPoly(&syndrome, &(mce_ctx.a_data));
    //             fprintf(stderr, "%x, %x\n", i, j);
    //         }
    //     }
    // }

    freeVecGF2(&z, 0);
    freePoly(&syndrome, 0);
    freePoly(&syndrome2, 0);
    freePoly(&squared, 0);*/
}

void testHash() {
    Vector_GF2 in, out;
    initRandVector(&in, 512, 256);
    gf2VecHashA(&out, &in, 512);

}

void testDecrypt() {
    Vector_GF2 cipher;
    Vector_GF2 plain;
    mallocVectorGF2(&cipher, mce_ctx.priv_key.h_mat.n + 2*512);
    mallocVectorGF2(&plain, 512);

    // initRandVector(&plain, mce_ctx., int w)
    // cipher.len = mce_ctx.priv_key.h_mat.n + 2*512;
    decrypt(&cipher, &plain, &mce_ctx);

    freeVecGF2(&cipher, 0);
    freeVecGF2(&plain, 0);
}

void testPadding() {
    Vector_GF2 message, padded_message, unpadded_message;
    
    initRandVector(&message, 50, 25);
    printGf2Vec(&message);

    addPaddingA(&padded_message, &message, 2);
    printGf2Vec(&padded_message);

    delPaddingA(&unpadded_message, &padded_message);
    printGf2Vec(&unpadded_message);

    freeVecGF2(&message, 0);
    freeVecGF2(&padded_message, 0);
    freeVecGF2(&unpadded_message, 0);
}

void testEncDec() {
    Vector_GF2 ciphertext, plaintext, plaintext2;

    initRandVector(&plaintext, mce_ctx.max_pt_len_bit - 1, 0);

    if (mceEncryptA(&ciphertext, &plaintext, &mce_ctx) != 0) {
        perror("encrypt");
        return;
    }

    if (mceDecryptA(&plaintext2, &ciphertext, &mce_ctx) != 0) {
        perror("mceDecryptA");
        return;
    }
    printDebug("plaintext: ");
    printGf2Vec(&plaintext);
    printDebug("plaintext2: ");
    printGf2Vec(&plaintext2);

    freeVecGF2(&ciphertext, 0);
    freeVecGF2(&plaintext, 0);
    freeVecGF2(&plaintext2, 0);
}

void testConCatVec() {
    Vector_GF2 v1, v2, v3;
    int i;

    mallocVectorGF2(&v1, 16);
    mallocVectorGF2(&v2, 15);

    gf2VecSetBit(&v1, 5, 1);
    gf2VecSetBit(&v2, 3, 1);

    printGf2Vec(&v1);
    printGf2Vec(&v2);

    for (i = 0; i < v1.len; i++) {
        gf2VecSetBit(&v1, i, 1);
    }

    gf2VecConcatA(&v3, &v1, &v2);
    printGf2Vec(&v3);

    freeVecGF2(&v1, 0);
    freeVecGF2(&v2, 0);
    freeVecGF2(&v3, 0);
}

int main(int argc, char const *argv[])
{
    srand(time(0));

    // testConCatVec();
    // return 0;

    if (mceInitCtx(&mce_ctx, 7)) {
        exit(1);
    }
    mceGenKeyPair(&mce_ctx);
    // printGf2xPoly(&mce_ctx.priv_key.g, &mce_ctx.a_data);

    // printBinary64Ln(59);
    // return 1;
    // printMceCtx(&mce_ctx);
    // testIrredPoly();
    // printLogExpTable();

    // fprintf(stderr, "\n");
    // testPolyDiv();
    // testPolyMod();
    // testPolyPower();

    // testPoly();
    // testPolyInv();
    // testPolyShift();
    // testIsSetBitMat();
    // testGf2VecMatMul();
    // testGetInvPerm();
    // testTransMatGF2();
    // testLogExpTables();
    // testPolyEval();
    // testMatrices();
    // testPermutationGF2m();
    // testPermutationGF2();
    // testConvertGF2m_to_GF2();
    // testCropMatrix();
    
    // fprintf(stderr, "\n");
    // printDebug("test: %s", "Print binary in BE and LE:");
    // printBinaryLn(21, 16);
    // printBinaryLnLe(21, 16);
    // 
    // testAppendIdentityMatrix();
    // testAppendIdentityMatrix();
    // testGH();
    // testEncrypt();
    // testPadding();
    // printMceCtx(&mce_ctx);
    // testEncrypt();
    // testDecrypt();
    testEncDec();
    // testCrop();
    // testPermutationVectorGF2();
    // testDetermineSyndrome();
    // testHash();

    mceFreeCtx(&mce_ctx);

    return 0;
}
