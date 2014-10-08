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

#ifdef BPU_TEST_DET_EQ_CODE
  int __test_was_eq_code;
#endif

int BPU_makeCCA2safePT(BPU_T_Vector_GF2 *r1, BPU_T_Vector_GF2 *r2, BPU_T_Vector_GF2 *in, BPU_T_Vector_GF2 *out) {

  BPU_T_Vector_GF2 temp, temp2;
  int rc = 0;

  rc = BPU_gf2VecConcatA(&temp, in, r2);
  rc = BPU_gf2VecHashA(&temp2, &temp, in->len);
  rc = BPU_gf2VecConcatA(out, r1, &temp2);

  BPU_freeVecGF2(&temp, 0);
  BPU_freeVecGF2(&temp2, 0);

  return rc;
}

int addPaddingA(BPU_T_Vector_GF2 *padded_message, const BPU_T_Vector_GF2 *message, const uint16_t padding_len) {
  int i;

  // malloc space for padded message
  if (BPU_mallocVectorGF2(padded_message, message->len + padding_len) != 0) {
    BPU_printError("addPaddingA: BPU_mallocVectorGF2");
    return -1;
  }

  // copy message into padded message
  for (i = 0; i < message->elements_in_row; i++)
    padded_message->elements[i] = message->elements[i];

  // add padding - first padded bit set to 1, rest keep 0
  BPU_gf2VecSetBit(padded_message, message->len, 1);

  return 0;
}

int delPaddingA(BPU_T_Vector_GF2 *message, const BPU_T_Vector_GF2 *padded_message) {
  int i, message_size = 0;

  // count the message size
  for (i = padded_message->len-1; i >= 0; i--) {
    // nula - padding
    if (BPU_gf2VecGetBit(padded_message, i) == 1) {
      
      // ci bola aspon jedna 0 pred 1
      //if (i <= padded_message->len-3) {
        message_size = i;
        break;
      //}
      // inak zly padding
      /*else {
        BPU_printError("del_padding: message padding is incorrect");
        return -1;
      }*/
    }
  }

  // malloc space for padded message
  if (BPU_mallocVectorGF2(message, message_size) != 0) {
    BPU_printError("del_padding: BPU_mallocVectorGF2");
    return -1;
  }

  // copy n-1 elements of padded message into message
  for (i = 0; i < message->elements_in_row - 1; i++)
    message->elements[i] = padded_message->elements[i];

  for (i = (message->elements_in_row - 1) * message->element_bit_size; i < message->len; i++)
    BPU_gf2VecSetBit(message, i, BPU_gf2VecGetBit(padded_message, i));

  return 0;
}

int BPU_encryptCCA2KobaraImaiA(BPU_T_Vector_GF2 *out, const BPU_T_Vector_GF2 *message, const BPU_T_McEliece_Ctx *ctx) {
  if (BPU_mallocVectorGF2(out, ctx->max_ct_len_bit)) {
    BPU_printError("BPU_encryptA: can not allocate ouput");

    return 1;
  }
  return BPU_encryptCCA2KobaraImai(out, message, ctx);
}

int BPU_encryptCCA2KobaraImai(BPU_T_Vector_GF2 *out, const BPU_T_Vector_GF2 *message, const BPU_T_McEliece_Ctx *ctx) {
  int rc = 0;
  BPU_T_Vector_GF2 r1, r2, e, padded_message;
  BPU_T_Vector_GF2 cca2SafePT, m, temp, temp2, temp3;

  // FU
  BPU_T_Matrix_GF2 G;
  const int MINIMAL_PADDING_SIZE = 1;

  // 2 ^ (degree of extension field - 2)
  uint16_t max_message_len;

  BPU_gf2VecNull(out);
  
  // PRE MENSIE POLIA - deg 5 - TENTO VYPOCET NESEDI (preto ctx->pub_key.m - 3)
  max_message_len = ctx->max_pt_len_bit;

#ifdef BPU_DEBUG_ENCRYPT
  BPU_printDebug("###### ENCRYPT ######");
  BPU_printDebug("max_message_len: %d", max_message_len);
#endif
  // minimal padding
  max_message_len -= MINIMAL_PADDING_SIZE;

#ifdef BPU_DEBUG_ENCRYPT
  BPU_printDebug("max_message_len without padding: %d", max_message_len);
#endif
  // test the size of message and g_m
  if (!(message->len > 0 && message->len <= max_message_len)) {
    BPU_printError("message length have to be of max length %d", max_message_len);

    return -1;
  }

  // test the size of message and g_m
  if (message->len > 512) {
    BPU_printError("BPU_encryption is supported for max message length 512 - hash limitation");
    return -1;
  }

#ifdef BPU_DEBUG_ENCRYPT
  BPU_printDebug("plaintext w/o padding:");
  BPU_printGf2Vec(message);
#endif
  // make padding to the correct message size with 1000... added to the end
  rc += addPaddingA(&padded_message, message, (max_message_len+MINIMAL_PADDING_SIZE) - message->len);

#ifdef BPU_DEBUG_ENCRYPT
  BPU_printDebug("plaintext with padding:");
  BPU_printGf2Vec(&padded_message);
#endif
  // generate random vector r1 = k-l bits = 986 bits
  rc += initRandVector(&r1, ctx->pub_key.g_mat.k - padded_message.len, 0);

  // generate random vector r2 = l bits = 512 bits
  rc += initRandVector(&r2, padded_message.len, 0);

  // expand the public key matrix g_m
  // FU
  rc += BPU_gf2MatAppendIdenityA(&G, &(ctx->pub_key.g_mat));

  // make cca2 safe plaintext
  rc += BPU_makeCCA2safePT(&r1, &r2, &padded_message, &cca2SafePT);

#ifdef BPU_DEBUG_ENCRYPT
  BPU_printDebug("cca2 safe pt:");
  BPU_printGf2Vec(&cca2SafePT);
#endif
  // calculate cca2 safe plaintext * g_m = z'
  // FU
  rc += BPU_gf2VecMulMatA(&m, &cca2SafePT, &G);
  // rc += BPU_gf2VecMulMatA(&m, &cca2SafePT, &(ctx->pub_key.g_mat));

#ifdef BPU_DEBUG_ENCRYPT
  BPU_printDebug("encoded plaintext:");
  BPU_printGf2Vec(&m);
#endif
  // generate random error vector e
  // FU
  rc += initRandVector(&e, G.n, ctx->pub_key.t);
  // rc += initRandVector(&e, ctx->pub_key.g_mat.n, ctx->pub_key.t);

#ifdef BPU_DEBUG_ENCRYPT
  BPU_printDebug("error vector:");
  BPU_printGf2Vec(&e);
#endif
  // z' XOR e 
  rc += BPU_gf2VecXor(&m, &e);

  // hash of r1
  rc += BPU_gf2VecHashA(&temp, &r1, padded_message.len);

  // hash of r1 XOR message
  rc += BPU_gf2VecXor(&temp, &padded_message);

  // hash of e
  BPU_gf2VecHashA(&temp2, &e, padded_message.len);    

  // hash of e XOR r2
  rc += BPU_gf2VecXor(&temp2, &r2);

  // z' XOR e || hash of r1 XOR message
  rc += BPU_gf2VecConcatA(&temp3, &m, &temp);

  // z' XOR e || hash of r1 XOR message || hash of e XOR r2
  rc += BPU_gf2VecConcat(out, &temp3, &temp2);

#ifdef BPU_DEBUG_ENCRYPT
  BPU_printDebug("cipher text:");
  BPU_printGf2Vec(out);
  BPU_printDebug("###### ENCRYPT DBPU_ONE ######\n");
#endif
  // FU
  BPU_freeMatGF2(&G, 0);
  BPU_freeVecGF2(&r1, 0);
  BPU_freeVecGF2(&r2, 0);
  BPU_freeVecGF2(&e, 0);
  BPU_freeVecGF2(&cca2SafePT, 0);
  BPU_freeVecGF2(&m, 0);
  BPU_freeVecGF2(&temp, 0);
  BPU_freeVecGF2(&temp2, 0);
  BPU_freeVecGF2(&temp3, 0);
  BPU_freeVecGF2(&padded_message, 0);
  return rc;
}

int BPU_encryptA(BPU_T_Vector_GF2 *out, const BPU_T_Vector_GF2 *message, const BPU_T_McEliece_Ctx *ctx) {
  if (BPU_mallocVectorGF2(out, ctx->priv_key.h_mat.n)) {
    BPU_printError("BPU_encryptA: can not allocate ouput");

    return 1;
  }
  return BPU_encrypt(out, message, ctx);
}

int BPU_encrypt(BPU_T_Vector_GF2 *out, const BPU_T_Vector_GF2 *message, const BPU_T_McEliece_Ctx *ctx) {
  int rc = 0;
  BPU_T_Vector_GF2 e;
  BPU_T_Vector_GF2 tmp;
  // FU
  // Matrix_GF2 G;
  // struct timeval tv, tv_end;

  // gettimeofday(&tv, NULL);
  BPU_gf2VecNull(out);
  // gettimeofday(&tv_end, NULL);
  // fprintf(stderr, "null %0.6f\n", (tv_end.tv_sec - tv.tv_sec + ((tv_end.tv_usec - tv.tv_usec) / (double)1000000)));
  
  // test the size of message and g_m
  if (message->len != ctx->pub_key.g_mat.k) {
    BPU_printError("message length have to be of length %d", ctx->pub_key.g_mat.k);

    return -1;
  }

#ifdef BPU_DEBUG_ENCRYPT
  BPU_printDebug("plaintext:");
  BPU_printGf2Vec(message);
#endif
  // expand the public key matrix g_m
  // gettimeofday(&tv, NULL);
  // FU
  // rc += BPU_gf2MatAppendIdenityA(&G, &(ctx->pub_key.g_mat));
  // gettimeofday(&tv_end, NULL);
  // fprintf(stderr, "append %0.6f\n", (tv_end.tv_sec - tv.tv_sec + ((tv_end.tv_usec - tv.tv_usec) / (double)1000000)));

  // BPU_printError("%d", ctx->pub_key.g_mat.k);
  // BPU_printError("%d", ctx->priv_key.h_mat.n);
  // BPU_printGf2Vec(&cca2SafePT);
  // calculate cca2 safe plaintext * g_m = z'
  // gettimeofday(&tv, NULL);
  // FU
  // rc += BPU_gf2VecMulMatA(out, message, &G);
  rc += BPU_gf2VecMulMatA(&tmp, message, &(ctx->pub_key.g_mat));
  BPU_gf2VecConcat(out, &tmp, message);
  // gettimeofday(&tv_end, NULL);
  // fprintf(stderr, "mul %0.6f\n", (tv_end.tv_sec - tv.tv_sec + ((tv_end.tv_usec - tv.tv_usec) / (double)1000000)));

#ifdef BPU_DEBUG_ENCRYPT
  BPU_printDebug("encoded plaintext:");
  BPU_printGf2Vec(out);
#endif
  // generate random error vector e
  // 
  // gettimeofday(&tv, NULL);
  // FU
  // rc += initRandVector(&e, G.n, ctx->pub_key.t);
  rc += initRandVector(&e, ctx->pub_key.g_mat.k + ctx->pub_key.g_mat.n, ctx->pub_key.t);
  // gettimeofday(&tv_end, NULL);
  // fprintf(stderr, "init rand %0.6f\n", (tv_end.tv_sec - tv.tv_sec + ((tv_end.tv_usec - tv.tv_usec) / (double)1000000)));
#ifdef BPU_DEBUG_ENCRYPT
  BPU_printDebug("error vector:");
  BPU_printGf2Vec(&e);
#endif
  // z' XOR e 
  // BPU_printGf2VecOnes(&e)
  // gettimeofday(&tv, NULL);
  rc += BPU_gf2VecXor(out, &e);
  // gettimeofday(&tv_end, NULL);
  // fprintf(stderr, "xor %0.6f\n", (tv_end.tv_sec - tv.tv_sec + ((tv_end.tv_usec - tv.tv_usec) / (double)1000000)));

#ifdef BPU_DEBUG_ENCRYPT
  BPU_printDebug("cipher text:");
  BPU_printGf2Vec(out);
  BPU_printDebug("###### ENCRYPT DBPU_ONE ######\n");
#endif
  // FU
  // BPU_freeMatGF2(&G, 0);
  BPU_freeVecGF2(&e, 0);
  BPU_freeVecGF2(&tmp, 0);

  return rc;
}

void findPolynomialsAB(const BPU_T_Poly_GF2_16x *tau, const BPU_T_Poly_GF2_16x *mod, BPU_T_Poly_GF2_16x *a, BPU_T_Poly_GF2_16x *b, const BPU_T_Arithmetic_Data *a_data) {
  BPU_T_Poly_GF2_16x tmp;
  int end_deg = mod->deg / 2;

  BPU_gf2xPolyExtEuclidA(a, b, &tmp, tau, mod, end_deg, a_data);
  BPU_freePoly(&tmp, 0);
}

int decodeA(BPU_T_Vector_GF2 *encoded, BPU_T_Vector_GF2 *decoded, const BPU_T_McEliece_Ctx *ctx) {
  if (BPU_mallocVectorGF2(decoded, ctx->a_data.ord)) {
    BPU_printError("decodeA:");

    return 1;
  }
  return decode(encoded, decoded, ctx);
}

int decode(BPU_T_Vector_GF2 *encoded, BPU_T_Vector_GF2 *decoded, const BPU_T_McEliece_Ctx *ctx) {
  BPU_T_Perm_Vector inv_perm;
  BPU_T_Poly_GF2_16x syndrome, tau, a, b, sigma, inv_syndrome, tmp, tmp2;
  // Vector_GF2 error;
  int l;
  BPU_T_GF2_16x tmp_eval;

  // it allocated inv_perm inside
  BPU_permGetInvA(&inv_perm, &(ctx->priv_key.permutation));

#ifdef BPU_DEBUG_DECODE
  BPU_printDebug("###### BPU_DEBUG_DECODE - BEGIN ######");
  BPU_printDebug("inverse permutation:");
  BPU_printPerm(&inv_perm);

  BPU_printDebug("before:");
  BPU_printGf2Vec(encoded);
#endif
  // permute code word
  BPU_gf2VecPermute(encoded, &inv_perm);

#ifdef BPU_DEBUG_DECODE
  BPU_printDebug("after:");
  BPU_printGf2Vec(encoded);
#endif
  BPU_gf2xMatDetermineSyndromeA(&syndrome, encoded, &(ctx->priv_key.h_mat), &(ctx->a_data));

#ifdef BPU_DEBUG_DECODE
  BPU_printDebug("Syndrome");
  BPU_printGf2xPoly(&syndrome, &(ctx->a_data));
#endif
  BPU_gf2xPolyInvA(&inv_syndrome, &syndrome, &(ctx->priv_key.g), &(ctx->a_data));
  inv_syndrome.coef[1] = inv_syndrome.coef[1] ^ BPU_ONE;

  // get square root
  BPU_mallocPoly(&tau, ctx->priv_key.g.deg);
  BPU_gf2xPolyRoot(&tau, &inv_syndrome, &(ctx->priv_key.g), &(ctx->a_data));

#ifdef BPU_DEBUG_DECODE
  BPU_printDebug("inv_syndrome:");
  BPU_printGf2xPoly(&inv_syndrome, &(ctx->a_data));
  BPU_printDebug("tau:");
  BPU_printGf2xPoly(&tau, &(ctx->a_data));

  BPU_gf2xPolyNull(&inv_syndrome);
  BPU_gf2xPolyMul(&inv_syndrome, &tau, &tau, &(ctx->a_data));
  BPU_gf2xPolyMod(&syndrome, &inv_syndrome, &(ctx->priv_key.g), &(ctx->a_data));

  BPU_printDebug("tau^2 mod g:");
  BPU_printGf2xPoly(&syndrome, &(ctx->a_data));
#endif
  /**************** FROM NOW WE ARE NOT USING MODULUS g for a, b ********************/
  findPolynomialsAB(&tau, &(ctx->priv_key.g), &a, &b, &(ctx->a_data));

#ifdef BPU_DEBUG_DECODE
  BPU_mallocPoly(&tmp2, 2 * ctx->priv_key.g.deg);
  BPU_mallocPoly(&tmp, 2 * ctx->priv_key.g.deg);
  // b * tau == a mod g
  BPU_gf2xPolyMul(&tmp, &tau, &b, &(ctx->a_data));
  BPU_gf2xPolyMod(&tmp2, &tmp, &(ctx->priv_key.g), &(ctx->a_data));
  
  BPU_printDebug("b * tau == a mod g:");
  BPU_printGf2xPoly(&tmp2, &(ctx->a_data));

  BPU_freePoly(&tmp2, 0);
  BPU_freePoly(&tmp, 0);
#endif    
  BPU_mallocPoly(&tmp2, 2 * ctx->priv_key.g.deg);
  BPU_mallocPoly(&tmp, 2 * ctx->priv_key.g.deg);

  // a^2, b^2
  BPU_gf2xPolyMul(&tmp, &a, &a, &(ctx->a_data));
  BPU_gf2xPolyMul(&tmp2, &b, &b, &(ctx->a_data));

#ifdef BPU_DEBUG_DECODE
  BPU_printDebug("a:");
  BPU_printGf2xPoly(&a, &(ctx->a_data));
  BPU_printDebug("b:");
  BPU_printGf2xPoly(&b, &(ctx->a_data));
#endif
  // copy a^2, b^2 to a, b
  BPU_gf2xPolyCopy(&a, &tmp);
  BPU_gf2xPolyCopy(&b, &tmp2);

#ifdef BPU_DEBUG_DECODE
  BPU_printDebug("a^2:");
  BPU_printGf2xPoly(&a, &(ctx->a_data));
  BPU_printDebug("b^2:");
  BPU_printGf2xPoly(&b, &(ctx->a_data));
#endif
  // b^2 * x
  BPU_gf2xPolyShl(&b, 1);

#ifdef BPU_DEBUG_DECODE
  BPU_printDebug("x * b^2:");
  BPU_printGf2xPoly(&b, &(ctx->a_data));    
#endif
  BPU_mallocPoly(&sigma, ctx->priv_key.g.deg);
  
  // calculate sigma = a^2 + x * b^2
  BPU_gf2xPolyAdd(&sigma, &a, &b);

#ifdef BPU_DEBUG_DECODE
  BPU_printDebug("sigma");
  BPU_printGf2xPoly(&sigma, &(ctx->a_data));
#endif
  // check if there is enough space
  if (decoded->len < ctx->a_data.ord) {
    BPU_freeVecGF2(decoded, 0);

    BPU_mallocVectorGF2(decoded, ctx->a_data.ord);
  }
  else {
    BPU_gf2VecNull(decoded);
  }
  
  for (l = 0; l < ctx->a_data.ord; l++) {
    tmp_eval = BPU_gf2xPolyEval(&sigma, ctx->a_data.exp_table[l], &(ctx->a_data));

    if (tmp_eval == 0) {
      BPU_gf2VecSetBit(decoded, l, 1);
    }
  }
  // permute decoded vector
  BPU_gf2VecPermute(decoded, &(ctx->priv_key.permutation));

  BPU_freePerm(&inv_perm, 0);
  BPU_freePoly(&syndrome, 0);
  BPU_freePoly(&tau, 0);
  BPU_freePoly(&inv_syndrome, 0);
  BPU_freePoly(&tmp, 0);
  BPU_freePoly(&tmp2, 0);
  BPU_freePoly(&sigma, 0);
  BPU_freePoly(&a, 0);
  BPU_freePoly(&b, 0);

#ifdef BPU_DEBUG_DECODE
  BPU_printDebug("###### BPU_DEBUG_DECODE - END ######");
#endif  
  return 0;
}

int decrypt(BPU_T_Vector_GF2 *message, BPU_T_Vector_GF2 *cipher, const BPU_T_McEliece_Ctx *ctx) {
  // Vector_GF2 tmp;
  int rc = decryptA(message, cipher, ctx);

  // if (!rc) {
  //     BPU_gf2VecCopy(message, &tmp);
  //     BPU_freeVecGF2(&tmp, 0);
  // }
  return rc;
}

int decryptA(BPU_T_Vector_GF2 *message, BPU_T_Vector_GF2 *cipher, const BPU_T_McEliece_Ctx *ctx) {
  int rc = 0;
  BPU_T_Vector_GF2 temp, decoded, temp_2;
  // Matrix_GF2 mm, mmm;

  BPU_mallocVectorGF2(&temp, cipher->len);
  BPU_gf2VecCopy(&temp, cipher);
  BPU_mallocVectorGF2(&temp_2, cipher->len);
  BPU_gf2VecCopy(&temp_2, cipher);

  decodeA(&temp, &decoded, ctx);

#ifdef BPU_DEBUG_DECRYPT
  BPU_printDebug("decoded error:");
  BPU_printGf2Vec(&decoded);
#endif
  BPU_gf2VecXor(&temp_2, &decoded);
  // BPU_printDebug("Ecoded message:");
  // BPU_printGf2Vec(&temp_2);
  BPU_gf2VecCropA(message, &temp_2, temp_2.len - ctx->pub_key.g_mat.k, ctx->pub_key.g_mat.k);
  // BPU_printGf2Vec(message);

  // BPU_gf2xMatConvertToGf2MatA(&mm, &ctx->priv_key.h_mat, ctx->priv_key.g.deg);
  // BPU_gf2MatTranspA(&mmm, &mm);
  // BPU_gf2VecMulMatA(message, &temp_2, &mmm);
  // BPU_printGf2Vec(message);
  BPU_freeVecGF2(&temp, 0);

#ifdef BPU_DEBUG_DECRYPT
  BPU_printDebug("###### DECRYPT DBPU_ONE ######");
#endif
  return rc;
}

int decrypt2(BPU_T_Vector_GF2 *cipher, BPU_T_Vector_GF2 *plain, const BPU_T_McEliece_Ctx *ctx) {
  BPU_T_Vector_GF2 z1, z2, z3, decoded, temp;
  BPU_T_Vector_GF2 error;   // len kvoli testom
  BPU_T_Vector_GF2 plain_text;   // len kvoli testom
  int l;
  // FU
  // Matrix_GF2 G;

  // FU
  // BPU_gf2MatAppendIdenityA(&G, &(ctx->pub_key.g_mat));
  // initRandVector(&error, ctx->pub_key.g_mat.n + ctx->pub_key.g_mat.k, ctx->pub_key.t - 1);
  // BPU_mallocVectorGF2(&error, ctx->pub_key.g_mat.n + ctx->pub_key.g_mat.k);
  initRandVector(&error, ctx->pub_key.g_mat.n + ctx->pub_key.g_mat.k, ctx->pub_key.t);

  l = (cipher->len - ctx->priv_key.h_mat.n) / 2;
  BPU_mallocVectorGF2(&z1, ctx->pub_key.g_mat.n + ctx->pub_key.g_mat.k);
  BPU_mallocVectorGF2(&z2, l);
  BPU_mallocVectorGF2(&z3, l);
  // BPU_mallocVectorGF2(&decoded, ctx->pub_key.g_mat.n + ctx->pub_key.g_mat.k);

  BPU_printGf2xPoly(&(ctx->priv_key.g), &(ctx->a_data));
  // TODO: split cipher text to z1, z2, z3
  
  initRandVector(&plain_text, ctx->pub_key.g_mat.k, 0);
  // BPU_printGf2Mat(&(G));
  // BPU_mallocVectorGF2(&plain_text, G.k);
  // BPU_gf2VecSetBit(&plain_text, 0, 1);
  
  // fprintf(stderr, "plain:\n");
  // BPU_printGf2Vec(&plain_text);
  // fprintf(stderr, "z1:\n");

  // BPU_mallocVectorGF2(&plain_text, G.k);
  // FU
  // BPU_gf2VecMulMat(&z1, &plain_text, &G);
  BPU_gf2VecMulMat(&z1, &plain_text, &(ctx->pub_key.g_mat));
  BPU_printGf2Vec(&z1);

  BPU_gf2VecXor(&z1, &error); // len kvoli testom
  
  // fprintf(stderr, "z1 XOR error:\n");
  // BPU_printGf2Vec(&z1);

  // BPU_printGf2Vec(cipher);
  // decodeA(&z1, &decoded, ctx);
  // Vector_GF2 temp;

  BPU_mallocVectorGF2(&temp, cipher->len);
  BPU_gf2VecCopy(&temp, cipher);
  decodeA(&temp, &decoded, ctx);
  // fprintf(stderr, "error:\n");
  // BPU_printGf2Vec(&error);

  // fprintf(stderr, "decoded error\n");
  // BPU_printGf2Vec(&decoded);

  BPU_printGf2VecOnes(&error);
  BPU_printGf2VecOnes(&decoded);

#ifdef BPU_TEST_DET_EQ_CODE
  if (BPU_gf2VecCmp(&error, &decoded)) {
    BPU_freeMcElieceCtx(ctx);

    if (__test_was_eq_code) {
      exit(2);
    }
    exit(1);
  }
#endif
  
  BPU_freeVecGF2(&decoded, 0);
  BPU_freeVecGF2(&plain_text, 0);
  BPU_freeVecGF2(&z1, 0);
  BPU_freeVecGF2(&z2, 0);
  BPU_freeVecGF2(&z3, 0);

  BPU_freeVecGF2(&error, 0);
  // FU
  // BPU_freeMatGF2(&G, 0);

  return 0;
}

int decryptCCA2KobaraImai(BPU_T_Vector_GF2 *message, const BPU_T_Vector_GF2 *cipher, const BPU_T_McEliece_Ctx *ctx) {
  BPU_T_Vector_GF2 tmp;
  int rc = decryptCCA2KobaraImaiA(&tmp, cipher, ctx);

  if (!rc) {
    BPU_gf2VecCopy(message, &tmp);
    BPU_freeVecGF2(&tmp, 0);
  }
  return rc;
}

int decryptCCA2KobaraImaiA(BPU_T_Vector_GF2 *message, const BPU_T_Vector_GF2 *cipher, const BPU_T_McEliece_Ctx *ctx) {
  uint16_t message_len;
  int rc = 0;
  BPU_T_Vector_GF2 z1, z1_, z2, z3, decoded, r, h, hash_r, hash_e, hh, temp;
  int n, k;

  // calculate n, and k 
  n = ctx->pub_key.g_mat.n;
  k = ctx->pub_key.g_mat.k;

#ifdef BPU_DEBUG_DECRYPT
  BPU_printDebug("###### DECRYPT ######");
#endif
  // PRE MENSIE POLIA - deg 5 a menej - TENTO VYPOCET NESEDI (preto pub_key->m - 3)
  message_len = ctx->max_pt_len_bit;

#ifdef BPU_DEBUG_DECRYPT
  BPU_printDebug("cipher text:");
  BPU_printGf2Vec(cipher);
#endif
  rc = BPU_gf2VecCropA(&z1, cipher, 0, (0x1 << (ctx->a_data.mod_deg)) -1);
  rc = BPU_gf2VecCropA(&z1_, cipher, 0, (0x1 << (ctx->a_data.mod_deg)) -1);
  rc = BPU_gf2VecCropA(&z2, cipher, (0x1 << (ctx->a_data.mod_deg)) -1, message_len);
  rc = BPU_gf2VecCropA(&z3, cipher, (0x1 << (ctx->a_data.mod_deg)) -1 + message_len, message_len);

#ifdef BPU_DEBUG_DECRYPT
  BPU_printDebug("z1:");
  BPU_printGf2Vec(&z1);
  BPU_printDebug("z2:");
  BPU_printGf2Vec(&z2);
  BPU_printDebug("z3:");
  BPU_printGf2Vec(&z3);
#endif
  decodeA(&z1_, &decoded, ctx);

#ifdef BPU_DEBUG_DECRYPT
  BPU_printDebug("decoded error:");
  BPU_printGf2Vec(&decoded);
#endif
  rc = BPU_gf2VecXor(&z1, &decoded);
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
  rc = BPU_gf2VecCropA(&r, &z1, n, k - message_len);
  rc = BPU_gf2VecCropA(&h, &z1, n + k - message_len, message_len);

  rc = BPU_gf2VecHashA(&hash_r, &r, message_len);
  rc = BPU_gf2VecXor(&z2, &hash_r);

  // Determine check value h′ = hash (m || hash (e) ⊕ z3 )
  rc = BPU_gf2VecHashA(&hash_e, &decoded, message_len);
  rc = BPU_gf2VecXor(&hash_e, &z3);
  rc = BPU_gf2VecConcatA(&temp, &z2, &hash_e);
  rc = BPU_gf2VecHashA(&hh, &temp, message_len);

#ifdef BPU_DEBUG_DECRYPT
  BPU_printDebug("plaintext with padding:");
  BPU_printGf2Vec(&z2);
#endif
  if (BPU_gf2VecCmp(&h, &hh)) {
    BPU_printError("check value not correct");

    rc = 1;
  }
  else {
    // delete padding
    rc = delPaddingA(message, &z2);

  #ifdef BPU_DEBUG_DECRYPT
    BPU_printDebug("plaintext w/o padding:");
    BPU_printGf2Vec(message);
  #endif
  }
  BPU_freeVecGF2(&z1, 0);
  BPU_freeVecGF2(&z2, 0);
  BPU_freeVecGF2(&z3, 0);
  BPU_freeVecGF2(&decoded, 0);
  BPU_freeVecGF2(&r, 0);
  BPU_freeVecGF2(&h, 0);
  BPU_freeVecGF2(&hash_r, 0);
  BPU_freeVecGF2(&hash_e, 0);
  BPU_freeVecGF2(&hh, 0);
  BPU_freeVecGF2(&temp, 0);

#ifdef BPU_DEBUG_DECRYPT
  BPU_printDebug("###### DECRYPT DBPU_ONE ######");
#endif
  return rc;
}

void BPU_gf2xPolyGenGoppaA(BPU_T_Poly_GF2_16x *p, int t, const BPU_T_Arithmetic_Data *a_data) {
#if defined(DEBUG_L) || defined(WARNING_L)
  int i = 1;
#endif
  BPU_mallocPoly(p, t);

  while(1) {
    BPU_gf2xPolyGenRandom(p, t, a_data);
    
    if (BPU_gf2xPolyIrredTest(p, a_data) == 1) {
      break;
    }
    BPU_printWarning("new irreducibility test #%d", i++);
  }
}
