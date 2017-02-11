/*
 This file is part of BitPunch
 Copyright (C) 2014-2015 Frantisek Uhrecky <frantisek.uhrecky[what here]gmail.com>

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
#include <bitpunch/bitpunch.h>

#include <stdio.h>
#include <stdlib.h>
#include <time.h>
#include <sys/time.h>

#include <bitpunch/crypto/hash/sha512.h>
#include <bitpunch/asn1/asn1.h>

int testCmpMecsCtx(const BPU_T_Mecs_Ctx * ctx1, const BPU_T_Mecs_Ctx * ctx2) {
    int i, j, rc = 0;

    if (ctx1->type != ctx2->type) {
        BPU_printError("type");
    }
    if (ctx1->ct_len != ctx2->ct_len) {
        BPU_printError("ct_len");
    }
    if (ctx1->pt_len != ctx2->pt_len) {
        BPU_printError("pt_len");
    }
    if (ctx1->_decrypt != ctx2->_decrypt) {
        BPU_printError("_decrypt");
    }
    if (ctx1->_encrypt != ctx2->_encrypt) {
        BPU_printError("_encrypt");
    }
    if (ctx1->code_ctx->code_len != ctx2->code_ctx->code_len) {
        BPU_printError("code_len");
    }
    if (ctx1->code_ctx->msg_len != ctx2->code_ctx->msg_len) {
        BPU_printError("msg_len");
    }
    if (ctx1->code_ctx->t != ctx2->code_ctx->t) {
        BPU_printError("t");
    }
    if (ctx1->code_ctx->type != ctx2->code_ctx->type) {
        BPU_printError("code type");
    }
    if (ctx1->code_ctx->_decode != ctx2->code_ctx->_decode) {
        BPU_printError("_decode");
    }
    if (ctx1->code_ctx->_encode != ctx2->code_ctx->_encode) {
        BPU_printError("_encode");
    }
    if (BPU_gf2xPolyCmp
        (ctx1->code_ctx->code_spec->goppa->g,
         ctx2->code_ctx->code_spec->goppa->g)) {
        BPU_printError("g poly");
    }
    if (ctx1->code_ctx->code_spec->goppa->support_len !=
        ctx2->code_ctx->code_spec->goppa->support_len) {
        BPU_printError("support len");
    }
    if (ctx1->code_ctx->code_spec->goppa->permutation->size !=
        ctx2->code_ctx->code_spec->goppa->permutation->size) {
        BPU_printError("perm size");
    }
    if (ctx1->code_ctx->code_spec->goppa->g_mat->elements_in_row !=
        ctx2->code_ctx->code_spec->goppa->g_mat->elements_in_row) {
        BPU_printError("g_mat elements_in_row");
    }
    if (ctx1->code_ctx->code_spec->goppa->g_mat->element_bit_size !=
        ctx2->code_ctx->code_spec->goppa->g_mat->element_bit_size) {
        BPU_printError("g_mat element_bit_size");
    }
    if (ctx1->code_ctx->code_spec->goppa->g_mat->k !=
        ctx2->code_ctx->code_spec->goppa->g_mat->k) {
        BPU_printError("g_mat k");
    }
    if (ctx1->code_ctx->code_spec->goppa->g_mat->n !=
        ctx2->code_ctx->code_spec->goppa->g_mat->n) {
        BPU_printError("g_mat n");
    }
    for (i = 0; i < ctx1->code_ctx->code_spec->goppa->permutation->size; i++) {
        if (ctx1->code_ctx->code_spec->goppa->permutation->elements[i] !=
            ctx2->code_ctx->code_spec->goppa->permutation->elements[i]) {
            BPU_printError("perm diff");
            break;
        }
    }
    if (ctx1->code_ctx->code_spec->goppa->h_mat->k !=
        ctx2->code_ctx->code_spec->goppa->h_mat->k) {
        BPU_printError("h_mat k");
    }
    if (ctx1->code_ctx->code_spec->goppa->h_mat->n !=
        ctx2->code_ctx->code_spec->goppa->h_mat->n) {
        BPU_printError("h_mat n");
    }
    for (i = 0; i < ctx1->code_ctx->code_spec->goppa->g_mat->elements_in_row;
         i++) {
        for (j = 0; j < ctx1->code_ctx->code_spec->goppa->g_mat->k; j++) {
            if (ctx1->code_ctx->code_spec->goppa->g_mat->elements[j][i] !=
                ctx2->code_ctx->code_spec->goppa->g_mat->elements[j][i]) {
                BPU_printError("g_mat diff");
                j = -1;
                break;
            }
        }
        if (j == -1) {
            break;
        }
    }
    for (i = 0; i < ctx1->code_ctx->code_spec->goppa->h_mat->n; i++) {
        for (j = 0; j < ctx1->code_ctx->code_spec->goppa->h_mat->k; j++) {
            if (ctx1->code_ctx->code_spec->goppa->h_mat->elements[j][i] !=
                ctx2->code_ctx->code_spec->goppa->h_mat->elements[j][i]) {
                BPU_printError("h_mat diff");
                j = -1;
                break;
            }
        }
        if (j == -1) {
            break;
        }
    }
    return rc;
}

int testKeyGenEncDec(BPU_T_Mecs_Ctx * ctx) {
    BPU_T_GF2_Vector *ct = NULL;
    BPU_T_GF2_Vector *pt_in = NULL;
    BPU_T_GF2_Vector *pt_out = NULL;
    BPU_T_GF2_Vector *error = NULL;
    int rc = 1;

    fprintf(stderr, "Key generation...\n");
    if (BPU_mecsGenKeyPair(ctx)) {
        BPU_printError("Key generation error");
        goto err;
    }

    // prepare plain text, allocate memory and init random plaintext
    if (NULL == (pt_in = BPU_gf2VecNew(ctx->pt_len))) {
        BPU_printError("PT initialisation error");
        goto err;
    }

    BPU_gf2VecRand(pt_in, 0);

    // alocate cipher text vector
    if (NULL == (ct = BPU_gf2VecNew(ctx->ct_len))) {
        BPU_printError("CT vector allocation error");
        goto err;
    }
    // prepare plain text, allocate memory and init random plaintext
    if (NULL == (pt_out = BPU_gf2VecNew(ctx->pt_len))) {
        BPU_printError("PT out initialisation error");
        goto err;
    }

    BPU_gf2VecRand(pt_out, 0);

    fprintf(stderr, "Encryption...\n");
    if (BPU_mecsEncrypt(ct, pt_in, ctx, NULL)) {
        BPU_printError("Encryption error");
        goto err;
    }

    fprintf(stderr, "Decryption...\n");
    if (BPU_mecsDecrypt(pt_out, error, ct, ctx)) {
        BPU_printError("Decryption error");
        goto err;
    }

    // check for correct decryption
    if (BPU_gf2VecCmp(pt_in, pt_out)) {
        BPU_printError("\nOutput plain text differs from input");
        goto err;
    } else {
        fprintf(stderr,
                "\nSUCCESS: Input plain text is equal to output plain text.\n");
    }

    rc = 0;
err:
    fprintf(stderr, "\nCleaning up...\n");
    BPU_SAFE_FREE(BPU_gf2VecFree, pt_in);
    BPU_SAFE_FREE(BPU_gf2VecFree, pt_out);
    BPU_SAFE_FREE(BPU_gf2VecFree, ct);
    BPU_SAFE_FREE(BPU_gf2VecFree, error);
    return rc;
}

#ifdef BPU_CONF_ASN1
int testKeyGenAsn1() {
    int rc = 1;
    BPU_T_Mecs_Ctx *ctx = NULL;
    BPU_T_Mecs_Ctx *ctx_2 = NULL;
    BPU_T_UN_Mecs_Params *params = NULL;

    fprintf(stderr, "Basic GOPPA Initialisation...\n");
    if (NULL == (params = BPU_mecsParamsGoppaNew(11, 50, 0))) {
        BPU_printError("\"%s\" failed", "BPU_mecsParamsGoppaNew");
        goto err;
    }

    if (NULL == (ctx = BPU_mecsCtxNew(params, BPU_EN_MECS_BASIC_GOPPA))) {
        BPU_printError("\"%s\" failed", "BPU_mecsCtxNew");
        goto err;
    }

    fprintf(stderr, "Key generation...\n");
    if (BPU_mecsGenKeyPair(ctx)) {
        BPU_printError("\"%s\" failed", "BPU_mecsGenKeyPair");
        goto err;
    }

    if (0 != BPU_asn1SaveKeyPair(ctx, "prikey.der", "pubkey.der")) {
        asn1_perror(-1);
    }

    if (0 != BPU_asn1LoadKeyPair(&ctx_2, "prikey.der", "pubkey.der")) {
        asn1_perror(-1);
    }

    testCmpMecsCtx(ctx, ctx_2);

    rc = 0;
err:
    BPU_SAFE_FREE(BPU_mecsFreeCtx, ctx);
    BPU_SAFE_FREE(BPU_mecsFreeCtx, ctx_2);
    BPU_SAFE_FREE(BPU_mecsParamsGoppaFree, params);
    return rc;
}

#endif

int testGoppa() {
    int rv = 1;
    BPU_T_Mecs_Ctx *ctx = NULL;
    BPU_T_UN_Mecs_Params *params = NULL;

    fprintf(stderr, "Basic GOPPA Initialisation...\n");
    if (NULL == (params = BPU_mecsParamsGoppaNew(11, 50, 0))) {
        goto err;
    }

    if (NULL == (ctx = BPU_mecsCtxNew(params, BPU_EN_MECS_BASIC_GOPPA))) {
        goto err;
    }

    if (0 != testKeyGenEncDec(ctx)) {
        BPU_printError("\"%s\" failed", "testKeyGenEncDec");
        goto err;
    }

    rv = 0;
err:
    BPU_SAFE_FREE(BPU_mecsFreeCtx, ctx);
    BPU_SAFE_FREE(BPU_mecsParamsGoppaFree, params);
    return rv;
}

#ifdef BPU_CONF_MECS_CCA2_POINTCHEVAL_GOPPA
int testGoppaPointcheval() {
    int rv = 1;
    BPU_T_Mecs_Ctx *ctx = NULL;
    BPU_T_UN_Mecs_Params *params = NULL;

    fprintf(stderr, "Basic GOPPA Initialisation...\n");
    if (NULL == (params = BPU_mecsParamsGoppaNew(11, 50, 0))) {
        goto err;
    }

    fprintf(stderr, "\nCCA2 Pointcheval GOPPA Initialisation...\n");
    if (NULL ==
        (ctx = BPU_mecsCtxNew(params, BPU_EN_MECS_CCA2_POINTCHEVAL_GOPPA))) {
        goto err;
    }

    if (0 != testKeyGenEncDec(ctx)) {
        BPU_printError("\"%s\" failed", "testKeyGenEncDec");
        goto err;
    }

    rv = 0;
err:
    BPU_SAFE_FREE(BPU_mecsFreeCtx, ctx);
    BPU_SAFE_FREE(BPU_mecsParamsGoppaFree, params);
    return rv;
}
#endif

int testQcmdpc() {
    int rv = 1;
    BPU_T_Mecs_Ctx *ctx = NULL;
    BPU_T_UN_Mecs_Params *params = NULL;

    // mce initialisation of 80-bit security
    fprintf(stderr, "Basic QC-MDPC Initialisation...\n");
    if (NULL == (params = BPU_mecsInitParamsQcmdpc(4801, 2, 90, 84))) {
        goto err;
    }

    if (NULL == (ctx = BPU_mecsCtxNew(params, BPU_EN_MECS_BASIC_QCMDPC))) {
        goto err;
    }

    if (0 != testKeyGenEncDec(ctx)) {
        BPU_printError("\"%s\" failed", "testKeyGenEncDec");
        goto err;
    }

    rv = 0;
err:
    BPU_SAFE_FREE(BPU_mecsFreeCtx, ctx);
    BPU_SAFE_FREE(BPU_mecsParamsGoppaFree, params);
    return rv;
}

#ifdef BPU_CONF_MECS_CCA2_POINTCHEVAL_QCMDPC
int testQcmdpcPointcheval() {
    int rv = 1;
    BPU_T_Mecs_Ctx *ctx = NULL;
    BPU_T_UN_Mecs_Params *params = NULL;

    fprintf(stderr, "\nCCA2 Pointcheval QC-MDPC Initialisation...\n");
    if (NULL == (params = BPU_mecsInitParamsQcmdpc(4801, 2, 90, 84))) {
        goto err;
    }
    if (NULL ==
        (ctx = BPU_mecsCtxNew(params, BPU_EN_MECS_CCA2_POINTCHEVAL_QCMDPC))) {
        goto err;
    }

    if (0 != testKeyGenEncDec(ctx)) {
        BPU_printError("\"%s\" failed", "testKeyGenEncDec");
        goto err;
    }

    rv = 0;
err:
    BPU_SAFE_FREE(BPU_mecsFreeCtx, ctx);
    BPU_SAFE_FREE(BPU_mecsParamsGoppaFree, params);
    return rv;
}
#endif

int main(int argc, char **argv) {
    int rc = 1;

    srand(time(NULL));
#if !defined(BPU_CONF_GOPPA_WO_H) && defined(BPU_CONF_ASN1)
    if (0 != testKeyGenAsn1()) {
        fprintf(stderr, "\"%s\" failed\n", "testKeyGenAsn1");
        goto err;
    }
#endif
    if (0 != testGoppa()) {
        fprintf(stderr, "\"%s\" failed\n", "testGoppa");
        goto err;
    }
#ifdef BPU_CONF_MECS_CCA2_POINTCHEVAL_GOPPA
    if (0 != testGoppaPointcheval()) {
        fprintf(stderr, "\"%s\" failed\n", "testGoppaPointcheval");
        goto err;
    }
#endif
    if (0 != testQcmdpc()) {
        fprintf(stderr, "\"%s\" failed\n", "testQcmdpc");
        goto err;
    }
#ifdef BPU_CONF_MECS_CCA2_POINTCHEVAL_QCMDPC
    if (0 != testQcmdpcPointcheval()) {
        fprintf(stderr, "\"%s\" failed\n", "testQcmdpcPointcheval");
        goto err;
    }
#endif

    rc = 0;
err:
    return rc;
}
