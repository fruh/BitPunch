/**
 This file is part of BitPunch
 Copyright (C) 2014 Frantisek Uhrecky <frantisek.uhrecky[at]gmail.com>
 Copyright (C) 2014 Andrej Gulyas <andrej.guly[at]gmail.com>
 Copyright (C) 2014 Marek Klein  <kleinmrk[at]gmail.com>
 Copyright (C) 2014 Filip Machovec  <filipmachovec[at]yahoo.com>
 Copyright (C) 2014 Jozef Kudlac Uhrecky <kudalc.jozef[at]gmail.com>

 This program is free software: you can redistribute it and/or modify
 it under the terms of the GNU General Public License as published by
 the Free Software Foundation, either version 3 of the License, or
 (at your option) any later version.

 This program is distributed in the hope that it will be useful,
 but WITHOUT ANY WARRANTY; without even the implied warranty of
 MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 GNU General Public License for more details.

 You should have received a copy of the GNU General Public License
 along with this program.  If not, see <http://www.gnu.org/licenses/>.
*/
#include <stdlib.h>

#include "process.h"
#include "arithmetics.h"
#include "globals.h"
#include "debugio.h"
#include "init.h"
#include "operations.h"
#include "types.h"

#include <sys/time.h>

#ifdef TEST_DET_EQ_CODE
    int __test_was_eq_code;
#endif

int makeCCA2safePT(Vector_GF2 *r1, Vector_GF2 *r2, Vector_GF2 *in, Vector_GF2 *out) {

    Vector_GF2 temp, temp2;
    int rc = 0;

    rc = gf2VecConcatA(&temp, in, r2);
    rc = gf2VecHashA(&temp2, &temp, in->len);
    rc = gf2VecConcatA(out, r1, &temp2);

    freeVecGF2(&temp, 0);
    freeVecGF2(&temp2, 0);

    return rc;
}

int addPaddingA(Vector_GF2 *padded_message, const Vector_GF2 *message, const uint16_t padding_len) {
    int i;

    // malloc space for padded message
    if (mallocVectorGF2(padded_message, message->len + padding_len) != 0) {
        printError("addPaddingA: mallocVectorGF2");
        return -1;
    }

    // copy message into padded message
    for (i = 0; i < message->elements_in_row; i++)
        padded_message->elements[i] = message->elements[i];

    // add padding - first padded bit set to 1, rest keep 0
    gf2VecSetBit(padded_message, message->len, 1);

    return 0;
}

int delPaddingA(Vector_GF2 *message, const Vector_GF2 *padded_message) {
    int i, message_size = 0;

    // count the message size
    for (i = padded_message->len-1; i >= 0; i--) {
        // nula - padding
        if (gf2VecGetBit(padded_message, i) == 1) {
            
            // ci bola aspon jedna 0 pred 1
            //if (i <= padded_message->len-3) {
                message_size = i;
                break;
            //}
            // inak zly padding
            /*else {
                printError("del_padding: message padding is incorrect");
                return -1;
            }*/
        }
    }

    // malloc space for padded message
    if (mallocVectorGF2(message, message_size) != 0) {
        printError("del_padding: mallocVectorGF2");
        return -1;
    }

    // copy n-1 elements of padded message into message
    for (i = 0; i < message->elements_in_row - 1; i++)
        message->elements[i] = padded_message->elements[i];

    for (i = (message->elements_in_row - 1) * message->element_bit_size; i < message->len; i++)
        gf2VecSetBit(message, i, gf2VecGetBit(padded_message, i));

    return 0;
}

int encryptCCA2KobaraImaiA(Vector_GF2 *out, const Vector_GF2 *message, const McEliece_Ctx *ctx) {
    if (mallocVectorGF2(out, ctx->max_ct_len_bit)) {
        printError("encryptA: can not allocate ouput");

        return 1;
    }
    return encryptCCA2KobaraImai(out, message, ctx);
}

int encryptCCA2KobaraImai(Vector_GF2 *out, const Vector_GF2 *message, const McEliece_Ctx *ctx) {
    int rc = 0;
    Vector_GF2 r1, r2, e, padded_message;
    Vector_GF2 cca2SafePT, m, temp, temp2, temp3;

    // FU
    Matrix_GF2 G;
    const int MINIMAL_PADDING_SIZE = 1;

    // 2 ^ (degree of extension field - 2)
    uint16_t max_message_len;

    gf2VecNull(out);
    
    // PRE MENSIE POLIA - deg 5 - TENTO VYPOCET NESEDI (preto ctx->pub_key.m - 3)
    max_message_len = ctx->max_pt_len_bit;

#ifdef DEBUG_ENCRYPT
    printDebug("###### ENCRYPT ######");
    printDebug("max_message_len: %d", max_message_len);
#endif
    // minimal padding
    max_message_len -= MINIMAL_PADDING_SIZE;

#ifdef DEBUG_ENCRYPT
    printDebug("max_message_len without padding: %d", max_message_len);
#endif
    // test the size of message and g_m
    if (!(message->len > 0 && message->len <= max_message_len)) {
        printError("message length have to be of max length %d", max_message_len);

        return -1;
    }

    // test the size of message and g_m
    if (message->len > 512) {
        printError("encryption is supported for max message length 512 - hash limitation");
        return -1;
    }

#ifdef DEBUG_ENCRYPT
    printDebug("plaintext w/o padding:");
    printGf2Vec(message);
#endif
    // make padding to the correct message size with 1000... added to the end
    rc += addPaddingA(&padded_message, message, (max_message_len+MINIMAL_PADDING_SIZE) - message->len);

#ifdef DEBUG_ENCRYPT
    printDebug("plaintext with padding:");
    printGf2Vec(&padded_message);
#endif
    // generate random vector r1 = k-l bits = 986 bits
    rc += initRandVector(&r1, ctx->pub_key.g_mat.k - padded_message.len, 0);

    // generate random vector r2 = l bits = 512 bits
    rc += initRandVector(&r2, padded_message.len, 0);

    // expand the public key matrix g_m
    // FU
    rc += gf2MatAppendIdenityA(&G, &(ctx->pub_key.g_mat));

    // make cca2 safe plaintext
    rc += makeCCA2safePT(&r1, &r2, &padded_message, &cca2SafePT);

#ifdef DEBUG_ENCRYPT
    printDebug("cca2 safe pt:");
    printGf2Vec(&cca2SafePT);
#endif
    // calculate cca2 safe plaintext * g_m = z'
    // FU
    rc += gf2VecMulMatA(&m, &cca2SafePT, &G);
    // rc += gf2VecMulMatA(&m, &cca2SafePT, &(ctx->pub_key.g_mat));

#ifdef DEBUG_ENCRYPT
    printDebug("encoded plaintext:");
    printGf2Vec(&m);
#endif
    // generate random error vector e
    // FU
    rc += initRandVector(&e, G.n, ctx->pub_key.t);
    // rc += initRandVector(&e, ctx->pub_key.g_mat.n, ctx->pub_key.t);

#ifdef DEBUG_ENCRYPT
    printDebug("error vector:");
    printGf2Vec(&e);
#endif
    // z' XOR e 
    rc += gf2VecXor(&m, &e);

    // hash of r1
    rc += gf2VecHashA(&temp, &r1, padded_message.len);

    // hash of r1 XOR message
    rc += gf2VecXor(&temp, &padded_message);

    // hash of e
    gf2VecHashA(&temp2, &e, padded_message.len);    

    // hash of e XOR r2
    rc += gf2VecXor(&temp2, &r2);

    // z' XOR e || hash of r1 XOR message
    rc += gf2VecConcatA(&temp3, &m, &temp);

    // z' XOR e || hash of r1 XOR message || hash of e XOR r2
    rc += gf2VecConcat(out, &temp3, &temp2);

#ifdef DEBUG_ENCRYPT
    printDebug("cipher text:");
    printGf2Vec(out);
    printDebug("###### ENCRYPT DONE ######\n");
#endif
    // FU
    freeMatGF2(&G, 0);
    freeVecGF2(&r1, 0);
    freeVecGF2(&r2, 0);
    freeVecGF2(&e, 0);
    freeVecGF2(&cca2SafePT, 0);
    freeVecGF2(&m, 0);
    freeVecGF2(&temp, 0);
    freeVecGF2(&temp2, 0);
    freeVecGF2(&temp3, 0);
    freeVecGF2(&padded_message, 0);
    return rc;
}

int encryptA(Vector_GF2 *out, const Vector_GF2 *message, const McEliece_Ctx *ctx) {
    if (mallocVectorGF2(out, ctx->priv_key.h_mat.n)) {
        printError("encryptA: can not allocate ouput");

        return 1;
    }
    return encrypt(out, message, ctx);
}

int encrypt(Vector_GF2 *out, const Vector_GF2 *message, const McEliece_Ctx *ctx) {
    int rc = 0;
    Vector_GF2 e;
    Vector_GF2 tmp;
    // FU
    // Matrix_GF2 G;
    // struct timeval tv, tv_end;

    // gettimeofday(&tv, NULL);
    gf2VecNull(out);
    // gettimeofday(&tv_end, NULL);
    // fprintf(stderr, "null %0.6f\n", (tv_end.tv_sec - tv.tv_sec + ((tv_end.tv_usec - tv.tv_usec) / (double)1000000)));
    
    // test the size of message and g_m
    if (message->len != ctx->pub_key.g_mat.k) {
        printError("message length have to be of length %d", ctx->pub_key.g_mat.k);

        return -1;
    }

#ifdef DEBUG_ENCRYPT
    printDebug("plaintext:");
    printGf2Vec(message);
#endif
    // expand the public key matrix g_m
    // gettimeofday(&tv, NULL);
    // FU
    // rc += gf2MatAppendIdenityA(&G, &(ctx->pub_key.g_mat));
    // gettimeofday(&tv_end, NULL);
    // fprintf(stderr, "append %0.6f\n", (tv_end.tv_sec - tv.tv_sec + ((tv_end.tv_usec - tv.tv_usec) / (double)1000000)));

    // printError("%d", ctx->pub_key.g_mat.k);
    // printError("%d", ctx->priv_key.h_mat.n);
    // printGf2Vec(&cca2SafePT);
    // calculate cca2 safe plaintext * g_m = z'
    // gettimeofday(&tv, NULL);
    // FU
    // rc += gf2VecMulMatA(out, message, &G);
    rc += gf2VecMulMatA(&tmp, message, &(ctx->pub_key.g_mat));
    gf2VecConcat(out, &tmp, message);
    // gettimeofday(&tv_end, NULL);
    // fprintf(stderr, "mul %0.6f\n", (tv_end.tv_sec - tv.tv_sec + ((tv_end.tv_usec - tv.tv_usec) / (double)1000000)));

#ifdef DEBUG_ENCRYPT
    printDebug("encoded plaintext:");
    printGf2Vec(out);
#endif
    // generate random error vector e
    // 
    // gettimeofday(&tv, NULL);
    // FU
    // rc += initRandVector(&e, G.n, ctx->pub_key.t);
    rc += initRandVector(&e, ctx->pub_key.g_mat.k + ctx->pub_key.g_mat.n, ctx->pub_key.t);
    // gettimeofday(&tv_end, NULL);
    // fprintf(stderr, "init rand %0.6f\n", (tv_end.tv_sec - tv.tv_sec + ((tv_end.tv_usec - tv.tv_usec) / (double)1000000)));
#ifdef DEBUG_ENCRYPT
    printDebug("error vector:");
    printGf2Vec(&e);
#endif
    // z' XOR e 
    // printGf2VecOnes(&e)
    // gettimeofday(&tv, NULL);
    rc += gf2VecXor(out, &e);
    // gettimeofday(&tv_end, NULL);
    // fprintf(stderr, "xor %0.6f\n", (tv_end.tv_sec - tv.tv_sec + ((tv_end.tv_usec - tv.tv_usec) / (double)1000000)));

#ifdef DEBUG_ENCRYPT
    printDebug("cipher text:");
    printGf2Vec(out);
    printDebug("###### ENCRYPT DONE ######\n");
#endif
    // FU
    // freeMatGF2(&G, 0);
    freeVecGF2(&e, 0);
    freeVecGF2(&tmp, 0);

    return rc;
}

void findPolynomialsAB(const Polynomial_GF2_16x *tau, const Polynomial_GF2_16x *mod, Polynomial_GF2_16x *a, Polynomial_GF2_16x *b, const Arithmetic_Data *a_data) {
    Polynomial_GF2_16x tmp;
    int end_deg = mod->deg / 2;

    gf2xPolyExtEuclidA(a, b, &tmp, tau, mod, end_deg, a_data);
    freePoly(&tmp, 0);
}

int decodeA(Vector_GF2 *encoded, Vector_GF2 *decoded, const McEliece_Ctx *ctx) {
    if (mallocVectorGF2(decoded, ctx->a_data.ord)) {
        printError("decodeA:");

        return 1;
    }
    return decode(encoded, decoded, ctx);
}

int decode(Vector_GF2 *encoded, Vector_GF2 *decoded, const McEliece_Ctx *ctx) {
    Permutation_Vector inv_perm;
    Polynomial_GF2_16x syndrome, tau, a, b, sigma, inv_syndrome, tmp, tmp2;
    // Vector_GF2 error;
    int l;
    GF2_16x tmp_eval;

    // it allocated inv_perm inside
    permGetInvA(&inv_perm, &(ctx->priv_key.permutation));

#ifdef DEBUG_DECODE
    printDebug("###### DEBUG_DECODE - BEGIN ######");
    printDebug("inverse permutation:");
    printPerm(&inv_perm);

    printDebug("before:");
    printGf2Vec(encoded);
#endif
    // permute code word
    gf2VecPermute(encoded, &inv_perm);

#ifdef DEBUG_DECODE
    printDebug("after:");
    printGf2Vec(encoded);
#endif
    gf2xMatDetermineSyndromeA(&syndrome, encoded, &(ctx->priv_key.h_mat), &(ctx->a_data));

#ifdef DEBUG_DECODE
    printDebug("Syndrome");
    printGf2xPoly(&syndrome, &(ctx->a_data));
#endif
    gf2xPolyInvA(&inv_syndrome, &syndrome, &(ctx->priv_key.g), &(ctx->a_data));
    inv_syndrome.coef[1] = inv_syndrome.coef[1] ^ ONE;

    // get square root
    mallocPoly(&tau, ctx->priv_key.g.deg);
    gf2xPolyRoot(&tau, &inv_syndrome, &(ctx->priv_key.g), &(ctx->a_data));

#ifdef DEBUG_DECODE
    printDebug("inv_syndrome:");
    printGf2xPoly(&inv_syndrome, &(ctx->a_data));
    printDebug("tau:");
    printGf2xPoly(&tau, &(ctx->a_data));

    gf2xPolyNull(&inv_syndrome);
    gf2xPolyMul(&inv_syndrome, &tau, &tau, &(ctx->a_data));
    gf2xPolyMod(&syndrome, &inv_syndrome, &(ctx->priv_key.g), &(ctx->a_data));

    printDebug("tau^2 mod g:");
    printGf2xPoly(&syndrome, &(ctx->a_data));
#endif
    /**************** FROM NOW WE ARE NOT USING MODULUS g for a, b ********************/
    findPolynomialsAB(&tau, &(ctx->priv_key.g), &a, &b, &(ctx->a_data));

#ifdef DEBUG_DECODE
    mallocPoly(&tmp2, 2 * ctx->priv_key.g.deg);
    mallocPoly(&tmp, 2 * ctx->priv_key.g.deg);
    // b * tau == a mod g
    gf2xPolyMul(&tmp, &tau, &b, &(ctx->a_data));
    gf2xPolyMod(&tmp2, &tmp, &(ctx->priv_key.g), &(ctx->a_data));
    
    printDebug("b * tau == a mod g:");
    printGf2xPoly(&tmp2, &(ctx->a_data));

    freePoly(&tmp2, 0);
    freePoly(&tmp, 0);
#endif    
    mallocPoly(&tmp2, 2 * ctx->priv_key.g.deg);
    mallocPoly(&tmp, 2 * ctx->priv_key.g.deg);

    // a^2, b^2
    gf2xPolyMul(&tmp, &a, &a, &(ctx->a_data));
    gf2xPolyMul(&tmp2, &b, &b, &(ctx->a_data));

#ifdef DEBUG_DECODE
    printDebug("a:");
    printGf2xPoly(&a, &(ctx->a_data));
    printDebug("b:");
    printGf2xPoly(&b, &(ctx->a_data));
#endif
    // copy a^2, b^2 to a, b
    gf2xPolyCopy(&a, &tmp);
    gf2xPolyCopy(&b, &tmp2);

#ifdef DEBUG_DECODE
    printDebug("a^2:");
    printGf2xPoly(&a, &(ctx->a_data));
    printDebug("b^2:");
    printGf2xPoly(&b, &(ctx->a_data));
#endif
    // b^2 * x
    gf2xPolyShl(&b, 1);

#ifdef DEBUG_DECODE
    printDebug("x * b^2:");
    printGf2xPoly(&b, &(ctx->a_data));    
#endif
    mallocPoly(&sigma, ctx->priv_key.g.deg);
    
    // calculate sigma = a^2 + x * b^2
    gf2xPolyAdd(&sigma, &a, &b);

#ifdef DEBUG_DECODE
    printDebug("sigma");
    printGf2xPoly(&sigma, &(ctx->a_data));
#endif
    // check if there is enough space
    if (decoded->len < ctx->a_data.ord) {
        freeVecGF2(decoded, 0);

        mallocVectorGF2(decoded, ctx->a_data.ord);
    }
    else {
        gf2VecNull(decoded);
    }
    
    for (l = 0; l < ctx->a_data.ord; l++) {
        tmp_eval = gf2xPolyEval(&sigma, ctx->a_data.exp_table[l], &(ctx->a_data));

        if (tmp_eval == 0) {
            gf2VecSetBit(decoded, l, 1);
        }
    }
    // permute decoded vector
    gf2VecPermute(decoded, &(ctx->priv_key.permutation));

    freePerm(&inv_perm, 0);
    freePoly(&syndrome, 0);
    freePoly(&tau, 0);
    freePoly(&inv_syndrome, 0);
    freePoly(&tmp, 0);
    freePoly(&tmp2, 0);
    freePoly(&sigma, 0);
    freePoly(&a, 0);
    freePoly(&b, 0);

#ifdef DEBUG_DECODE
    printDebug("###### DEBUG_DECODE - END ######");
#endif  
    return 0;
}

int decrypt(Vector_GF2 *message, Vector_GF2 *cipher, const McEliece_Ctx *ctx) {
    // Vector_GF2 tmp;
    int rc = decryptA(message, cipher, ctx);

    // if (!rc) {
    //     gf2VecCopy(message, &tmp);
    //     freeVecGF2(&tmp, 0);
    // }
    return rc;
}

int decryptA(Vector_GF2 *message, Vector_GF2 *cipher, const McEliece_Ctx *ctx) {
    int rc = 0;
    Vector_GF2 temp, decoded, temp_2;
    // Matrix_GF2 mm, mmm;

    mallocVectorGF2(&temp, cipher->len);
    gf2VecCopy(&temp, cipher);
    mallocVectorGF2(&temp_2, cipher->len);
    gf2VecCopy(&temp_2, cipher);

    decodeA(&temp, &decoded, ctx);

#ifdef DEBUG_DECRYPT
    printDebug("decoded error:");
    printGf2Vec(&decoded);
#endif
    gf2VecXor(&temp_2, &decoded);
    // printDebug("Ecoded message:");
    // printGf2Vec(&temp_2);
    gf2VecCropA(message, &temp_2, temp_2.len - ctx->pub_key.g_mat.k, ctx->pub_key.g_mat.k);
    // printGf2Vec(message);

    // gf2xMatConvertToGf2MatA(&mm, &ctx->priv_key.h_mat, ctx->priv_key.g.deg);
    // gf2MatTranspA(&mmm, &mm);
    // gf2VecMulMatA(message, &temp_2, &mmm);
    // printGf2Vec(message);
    freeVecGF2(&temp, 0);

#ifdef DEBUG_DECRYPT
    printDebug("###### DECRYPT DONE ######");
#endif
    return rc;
}

int decrypt2(Vector_GF2 *cipher, Vector_GF2 *plain, const McEliece_Ctx *ctx) {
    Vector_GF2 z1, z2, z3, decoded, temp;
    Vector_GF2 error;   // len kvoli testom
    Vector_GF2 plain_text;   // len kvoli testom
    int l;
    // FU
    // Matrix_GF2 G;

    // FU
    // gf2MatAppendIdenityA(&G, &(ctx->pub_key.g_mat));
    // initRandVector(&error, ctx->pub_key.g_mat.n + ctx->pub_key.g_mat.k, ctx->pub_key.t - 1);
    // mallocVectorGF2(&error, ctx->pub_key.g_mat.n + ctx->pub_key.g_mat.k);
    initRandVector(&error, ctx->pub_key.g_mat.n + ctx->pub_key.g_mat.k, ctx->pub_key.t);

    l = (cipher->len - ctx->priv_key.h_mat.n) / 2;
    mallocVectorGF2(&z1, ctx->pub_key.g_mat.n + ctx->pub_key.g_mat.k);
    mallocVectorGF2(&z2, l);
    mallocVectorGF2(&z3, l);
    // mallocVectorGF2(&decoded, ctx->pub_key.g_mat.n + ctx->pub_key.g_mat.k);

    printGf2xPoly(&(ctx->priv_key.g), &(ctx->a_data));
    // TODO: split cipher text to z1, z2, z3
    
    initRandVector(&plain_text, ctx->pub_key.g_mat.k, 0);
    // printGf2Mat(&(G));
    // mallocVectorGF2(&plain_text, G.k);
    // gf2VecSetBit(&plain_text, 0, 1);
    
    // fprintf(stderr, "plain:\n");
    // printGf2Vec(&plain_text);
    // fprintf(stderr, "z1:\n");

    // mallocVectorGF2(&plain_text, G.k);
    // FU
    // gf2VecMulMat(&z1, &plain_text, &G);
    gf2VecMulMat(&z1, &plain_text, &(ctx->pub_key.g_mat));
    printGf2Vec(&z1);

    gf2VecXor(&z1, &error); // len kvoli testom
    
    // fprintf(stderr, "z1 XOR error:\n");
    // printGf2Vec(&z1);

    // printGf2Vec(cipher);
    // decodeA(&z1, &decoded, ctx);
    // Vector_GF2 temp;

    mallocVectorGF2(&temp, cipher->len);
    gf2VecCopy(&temp, cipher);
    decodeA(&temp, &decoded, ctx);
    // fprintf(stderr, "error:\n");
    // printGf2Vec(&error);

    // fprintf(stderr, "decoded error\n");
    // printGf2Vec(&decoded);

    printGf2VecOnes(&error);
    printGf2VecOnes(&decoded);

#ifdef TEST_DET_EQ_CODE
    if (gf2VecCmp(&error, &decoded)) {
        freeMcElieceCtx(ctx);

        if (__test_was_eq_code) {
            exit(2);
        }
        exit(1);
    }
#endif
    
    freeVecGF2(&decoded, 0);
    freeVecGF2(&plain_text, 0);
    freeVecGF2(&z1, 0);
    freeVecGF2(&z2, 0);
    freeVecGF2(&z3, 0);

    freeVecGF2(&error, 0);
    // FU
    // freeMatGF2(&G, 0);

    return 0;
}

int decryptCCA2KobaraImai(Vector_GF2 *message, const Vector_GF2 *cipher, const McEliece_Ctx *ctx) {
    Vector_GF2 tmp;
    int rc = decryptCCA2KobaraImaiA(&tmp, cipher, ctx);

    if (!rc) {
        gf2VecCopy(message, &tmp);
        freeVecGF2(&tmp, 0);
    }
    return rc;
}

int decryptCCA2KobaraImaiA(Vector_GF2 *message, const Vector_GF2 *cipher, const McEliece_Ctx *ctx) {
    uint16_t message_len;
    int rc = 0;
    Vector_GF2 z1, z1_, z2, z3, decoded, r, h, hash_r, hash_e, hh, temp;
    int n, k;

    // calculate n, and k 
    n = ctx->pub_key.g_mat.n;
    k = ctx->pub_key.g_mat.k;

#ifdef DEBUG_DECRYPT
    printDebug("###### DECRYPT ######");
#endif
    // PRE MENSIE POLIA - deg 5 a menej - TENTO VYPOCET NESEDI (preto pub_key->m - 3)
    message_len = ctx->max_pt_len_bit;

#ifdef DEBUG_DECRYPT
    printDebug("cipher text:");
    printGf2Vec(cipher);
#endif
    rc = gf2VecCropA(&z1, cipher, 0, (0x1 << (ctx->a_data.mod_deg)) -1);
    rc = gf2VecCropA(&z1_, cipher, 0, (0x1 << (ctx->a_data.mod_deg)) -1);
    rc = gf2VecCropA(&z2, cipher, (0x1 << (ctx->a_data.mod_deg)) -1, message_len);
    rc = gf2VecCropA(&z3, cipher, (0x1 << (ctx->a_data.mod_deg)) -1 + message_len, message_len);

#ifdef DEBUG_DECRYPT
    printDebug("z1:");
    printGf2Vec(&z1);
    printDebug("z2:");
    printGf2Vec(&z2);
    printDebug("z3:");
    printGf2Vec(&z3);
#endif
    decodeA(&z1_, &decoded, ctx);

#ifdef DEBUG_DECRYPT
    printDebug("decoded error:");
    printGf2Vec(&decoded);
#endif
    rc = gf2VecXor(&z1, &decoded);
    // m: m*G = z1 ===> z1 = m

    /*Split m ′ to ( r , h , m ′′ ).
    k−l bits l bits n−k bits
    Reconstruct plaintext candidate m = z 2 ⊕ hash (r).
    Determine check value h ′ = hash (m ∥ hash (e) ⊕ z 3 ).
    if h ′ ≡ h then
    15:
    return m
    16: end if
    17: return “Error”*/
    rc = gf2VecCropA(&r, &z1, n, k - message_len);
    rc = gf2VecCropA(&h, &z1, n + k - message_len, message_len);

    rc = gf2VecHashA(&hash_r, &r, message_len);
    rc = gf2VecXor(&z2, &hash_r);

    // Determine check value h′ = hash (m || hash (e) ⊕ z3 )
    rc = gf2VecHashA(&hash_e, &decoded, message_len);
    rc = gf2VecXor(&hash_e, &z3);
    rc = gf2VecConcatA(&temp, &z2, &hash_e);
    rc = gf2VecHashA(&hh, &temp, message_len);

#ifdef DEBUG_DECRYPT
    printDebug("plaintext with padding:");
    printGf2Vec(&z2);
#endif
    if (gf2VecCmp(&h, &hh)) {
        printError("check value not correct");

        rc = 1;
    }
    else {
        // delete padding
        rc = delPaddingA(message, &z2);

    #ifdef DEBUG_DECRYPT
        printDebug("plaintext w/o padding:");
        printGf2Vec(message);
    #endif
    }
    freeVecGF2(&z1, 0);
    freeVecGF2(&z2, 0);
    freeVecGF2(&z3, 0);
    freeVecGF2(&decoded, 0);
    freeVecGF2(&r, 0);
    freeVecGF2(&h, 0);
    freeVecGF2(&hash_r, 0);
    freeVecGF2(&hash_e, 0);
    freeVecGF2(&hh, 0);
    freeVecGF2(&temp, 0);

#ifdef DEBUG_DECRYPT
    printDebug("###### DECRYPT DONE ######");
#endif
    return rc;
}

void gf2xPolyGenGoppaA(Polynomial_GF2_16x *p, int t, const Arithmetic_Data *a_data) {
#if defined(DEBUG_L) || defined(WARNING_L)
    int i = 1;
#endif
    mallocPoly(p, t);

    while(1) {
        gf2xPolyGenRandom(p, t, a_data);
        
        if (gf2xPolyIrredTest(p, a_data) == 1) {
            break;
        }
        printWarning("new irreducibility test #%d", i++);
    }
}
