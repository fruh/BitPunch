/*
This file is part of BitPunch
Copyright (C) 2015 Frantisek Uhrecky <frantisek.uhrecky[what here]gmail.com>
Copyright (C) 2015 Andrej Gulyas <andrej.guly[what here]gmail.com>

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

#include "qcmdpc.h"

#ifdef BPU_CONF_ENCRYPTION
int BPU_mecsQcmdpcEncode(BPU_T_GF2_Vector * out, const BPU_T_GF2_Vector * in,
                         const struct _BPU_T_Code_Ctx *ctx) {

    BPU_T_GF2_Poly temp_ct, temp_rot_row;
    int ele, bit, i, bit_in_msg = 0;

    // copy message into cipher text
    for (i = 0; i < in->array_length; i++)
        out->elements[i] = in->elements[i];

    // prolong ciphertext
    BPU_gf2PolySetDeg(out, ctx->code_len);

    // calc rest of cipher text (Q x m)
    BPU_gf2PolyMalloc(&temp_ct, ctx->code_spec->qcmdpc->G.element_size);
    // for all matrices in G
    for (ele = 0; ele < ctx->code_spec->qcmdpc->G.element_count; ele++) {
        // get matrix
        BPU_gf2PolyCopy(&temp_rot_row,
                        &ctx->code_spec->qcmdpc->G.matrices[ele]);
        // for all bits in matrix
        for (bit = 0; bit < ctx->code_spec->qcmdpc->G.element_size; bit++) {
            // if is set bit in message
            if (BPU_gf2PolyGetBit(in, bit_in_msg)) {
                for (i = 0; i < temp_rot_row.array_length; i++)
                    temp_ct.elements[i] ^= temp_rot_row.elements[i];
            }
            bit_in_msg++;
            // get next row by shift
            BPU_gf2PolyMulX(&temp_rot_row);
        }
        BPU_gf2PolyFree(&temp_rot_row, 0);
    }

    // join ciphertext
    BPU_gf2PolyShiftLeft(&temp_ct, ctx->code_spec->qcmdpc->G.element_size);
    BPU_gf2PolyAdd(out, &temp_ct, 0);
    BPU_gf2PolyFree(&temp_ct, 0);

    return 0;
}
#endif

#ifdef BPU_CONF_DECRYPTION
int BPU_mecsQcmdpcDecrypt(BPU_T_GF2_Vector * out, const BPU_T_GF2_Vector * in,
                          const struct _BPU_T_Code_Ctx *ctx) {

    int ret = 0, delta = BPU_QCMDPC_PARAM_DELTA;
    int i;

    // BPU_printGf2Vec(ctx->e);
    // null error vector 
    BPU_gf2VecNull(ctx->e);

    // try to decode with faster algorithm
    if (!BPU_mecsQcmdpcDecode2(ctx->e, in, ctx)) {
        // free decoded
        BPU_gf2VecNull(ctx->e);
        while (1) {
            // if not decoded, try algorithm with lower DFR
            if (!BPU_mecsQcmdpcDecode1(ctx->e, in, delta, ctx)) {
                // free decoded
                BPU_gf2VecNull(ctx->e);
                // if not decoded decrease threshold tolerance param
                delta--;
                if (delta < 0) {
                    ret = -1;
                    break;
                }
            }
            else
                break;
        }
    }

    // if decoded, get message from first param_m bits
    if (ret == 0) {
        // decrypt message
        for (i = 0; i < out->array_length; i++)
            out->elements[i] = ctx->e->elements[i] ^ in->elements[i];
        // crop last element
        out->elements[out->array_length - 1] <<= out->element_bit_size -
            (out->len % out->element_bit_size);
        out->elements[out->array_length - 1] >>= out->element_bit_size -
            (out->len % out->element_bit_size);
    }
    else
        BPU_gf2VecNull(ctx->e);

    return ret;
}

int BPU_mecsQcmdpcDecode1(BPU_T_GF2_Vector * error_vec,
                          const BPU_T_GF2_Vector * cipher_text, int delta,
                          const struct _BPU_T_Code_Ctx *ctx) {
    BPU_T_GF2_Poly syndrom;
    BPU_T_GF2_Sparse_Poly row;
    int iter = -1, max, bit, upc, upc_counts[cipher_text->len], isSyndromZero =
        0;
    int flipped_bits_iter = 0;
    uint8_t bit_value;

    // allocate output error vector
    // BPU_gf2PolyMalloc(error_vec, cipher_text->len);

    // calc the syndrom
    BPU_mecsQcmdpcCalcSyndrom(&syndrom, cipher_text, ctx);
    // check syndrom
    if (!BPU_gf2PolyIsZero(&syndrom)) {
        // for max iterations
        for (iter = 0; iter < BPU_QCMDPC_PARAM_MAX_ITER; iter++) {
            max = 0;
            // for every bit of cipher text
            for (bit = 0; bit < error_vec->len; bit++) {
                // calc #UPC
                BPU_gf2SparseQcMatrixGetRow(&row, &ctx->code_spec->qcmdpc->H,
                                            bit);
                upc = BPU_gf2SparsePolyAndHW(&syndrom, &row);
                upc_counts[bit] = upc;
                if (upc > max)
                    max = upc;
                BPU_gf2SparsePolyFree(&row, 0);
            }

            if (max == 0) {
                isSyndromZero = 0;
                break;
            }


            flipped_bits_iter = 0;
            // check which bits to flip
            for (bit = 0; bit < error_vec->len; bit++) {
                if (upc_counts[bit] > 0 && upc_counts[bit] >= (max - delta)) {
                    flipped_bits_iter++;
                    // flip bit
                    bit_value = !BPU_gf2VecGetBit(error_vec, bit);
                    BPU_gf2VecSetBit(error_vec, bit, bit_value);
                    // update syndrom
                    BPU_gf2SparseQcMatrixGetRow(&row,
                                                &ctx->code_spec->qcmdpc->H,
                                                bit);
                    BPU_gf2SparsePolyAdd(&syndrom, &row);
                    BPU_gf2SparsePolyFree(&row, 0);
                    // check the syndrom
                    if (BPU_gf2PolyIsZero(&syndrom)) {
                        isSyndromZero = 1;
                        break;
                    }
                }
            }

            if (isSyndromZero)
                break;
        }
    }
    else {
        isSyndromZero = 1;
    }
    //free
    BPU_gf2PolyFree(&syndrom, 0);

    return isSyndromZero;
}

int BPU_mecsQcmdpcDecode2(BPU_T_GF2_Vector * error_vec,
                          const BPU_T_GF2_Vector * cipher_text,
                          const struct _BPU_T_Code_Ctx *ctx) {
    BPU_T_GF2_Poly syndrom;
    BPU_T_GF2_Sparse_Poly row;
    int iter = -1, bit, upc, isSyndromZero = 0;
    int flipped_bits_iter = 0;
    const uint16_t B_store[BPU_QCMDPC_MAX_B_VALUES] = { 28, 26, 24, 22, 20 };
    uint8_t bit_value;

    // calc the syndrom
    BPU_mecsQcmdpcCalcSyndrom(&syndrom, cipher_text, ctx);

    // check syndrom
    if (!BPU_gf2PolyIsZero(&syndrom)) {
        // for max iterations
        for (iter = 0; iter < BPU_QCMDPC_PARAM_MAX_ITER; iter++) {
            flipped_bits_iter = 0;
            // for every bit of cipher text
            for (bit = 0; bit < error_vec->len; bit++) {
                // calc #UPC
                BPU_gf2SparseQcMatrixGetRow(&row, &ctx->code_spec->qcmdpc->H,
                                            bit);
                upc = BPU_gf2SparsePolyAndHW(&syndrom, &row);

                // check which bits to flip
                if (upc > 0
                    && upc >= B_store[iter <
                                      BPU_QCMDPC_MAX_B_VALUES ? iter
                                      : (BPU_QCMDPC_MAX_B_VALUES - 1)] -
                    BPU_QCMDPC_PARAM_DELTA_B) {
                    flipped_bits_iter++;
                    // flip bit
                    bit_value = !BPU_gf2VecGetBit(error_vec, bit);
                    BPU_gf2VecSetBit(error_vec, bit, bit_value);
                    // update syndrom
                    BPU_gf2SparsePolyAdd(&syndrom, &row);
                    // check the syndrom
                    if (BPU_gf2PolyIsZero(&syndrom)) {
                        isSyndromZero = 1;
                        BPU_gf2SparsePolyFree(&row, 0);
                        break;
                    }
                }
                BPU_gf2SparsePolyFree(&row, 0);
            }

            if (flipped_bits_iter < 1) {
                isSyndromZero = 0;
                break;
            }

            if (isSyndromZero)
                break;
        }
    }
    else {
        isSyndromZero = 1;
    }
    //free
    BPU_gf2PolyFree(&syndrom, 0);

    return isSyndromZero;
}

void BPU_mecsQcmdpcCalcSyndrom(BPU_T_GF2_Vector * syndrom,
                               const BPU_T_GF2_Vector * cipher_text,
                               const struct _BPU_T_Code_Ctx *ctx) {
    BPU_T_GF2_Sparse_Poly row;
    int i;

    BPU_gf2PolyMalloc(syndrom, ctx->code_spec->qcmdpc->H.n);
    for (i = 0; i < cipher_text->len; i++) {
        if (BPU_gf2VecGetBit(cipher_text, i) == 1ul) {
            BPU_gf2SparseQcMatrixGetRow(&row, &ctx->code_spec->qcmdpc->H, i);
            BPU_gf2SparsePolyAdd(syndrom, &row);
            BPU_gf2SparsePolyFree(&row, 0);
        }
    }
}
#endif

#ifdef BPU_CONF_KEY_GEN
int BPU_mecsQcmdpcGenKeys(BPU_T_Code_Ctx * ctx) {

    BPU_T_GF2_Poly H_temp[ctx->code_spec->qcmdpc->n0];
    BPU_T_GF2_Poly G_temp[ctx->code_spec->qcmdpc->n0 - 1];
    BPU_T_GF2_Poly H_last_inv, mod, test_inv;
    BPU_T_GF2_QC_Matrix G_temp_mat, H_temp_mat;
    BPU_T_GF2_Sparse_Qc_Matrix H_temp_sparse;
    int wi[ctx->code_spec->qcmdpc->n0];
    int ret = 0, i, err = 0;

    // init modulus like 1000000...0001
    BPU_gf2PolyMalloc(&mod, ctx->code_spec->qcmdpc->m + 1);
    BPU_gf2VecSetBit(&mod, ctx->code_spec->qcmdpc->m, 1ul);
    BPU_gf2VecSetBit(&mod, 0, 1ul);

#if defined(DEBUG_L)
    BPU_printDebug("modulus: ");
//    BPU_printGf2Poly(&mod);
    BPU_printDebug("generating H vectors");
#endif

    // alloc parity-check matrix
    for (i = 0; i < ctx->code_spec->qcmdpc->n0; i++) {
        // calc weight of polynomials (last poly must be odd)
        if ((ctx->code_spec->qcmdpc->w / ctx->code_spec->qcmdpc->n0) % 2 == 1)
            wi[i] =
                ctx->code_spec->qcmdpc->w / ctx->code_spec->qcmdpc->n0 +
                (int) (i <
                       (ctx->code_spec->qcmdpc->w %
                        ctx->code_spec->qcmdpc->n0));
        else
            wi[i] =
                ctx->code_spec->qcmdpc->w / ctx->code_spec->qcmdpc->n0 +
                (int) (i <
                       (ctx->code_spec->qcmdpc->w %
                        ctx->code_spec->qcmdpc->n0)) + (int) (i ==
                                                              0) - (int) (i ==
                                                                          ctx->code_spec->qcmdpc->n0
                                                                          - 1);

        // generate random polynomials of given weight
        err +=
            BPU_gf2PolyInitRand(&H_temp[i], ctx->code_spec->qcmdpc->m, wi[i],
                                1);
#if defined(DEBUG_L)
        BPU_printDebug("H[%i]: ", i);
//      BPU_printGf2Poly(&H_temp[i]);
#endif
    }

    if (!err) {
        BPU_printDebug("generation successful");
    }
    else {
        BPU_printError("generation failed");
    }

    BPU_printDebug("finding inversion to H[%i]",
                   ctx->code_spec->qcmdpc->n0 - 1);
    // check if H[n0-1] has inversion
    ret = 0;
    while (!ret) {
        BPU_gf2PolySetDeg(&H_temp[ctx->code_spec->qcmdpc->n0 - 1], -1);
        // find inversion using XGCD
        ret =
            BPU_gf2PolyInv(&H_last_inv,
                           &H_temp[ctx->code_spec->qcmdpc->n0 - 1], &mod);

        // if inversion exists, test it (poly x inversion modulo = 1)
        if (ret) {
            BPU_printDebug("testing inversion");
            BPU_gf2PolyMulMod(&H_last_inv,
                              &H_temp[ctx->code_spec->qcmdpc->n0 - 1],
                              &test_inv, &mod, 1);
            if (test_inv.len != 1 || test_inv.elements[0] != 1ul) {
                ret = 0;
                BPU_printWarning("inversion failed");
            }
            else {
                BPU_printDebug("inversion OK");
            }
            BPU_gf2PolyFree(&test_inv, 0);
        }

        // inversion not found, regenerate last poly and try to find inversion again
        if (!ret) {
            BPU_printDebug("inversion not found");
            BPU_printDebug("generating new H[%i]",
                           ctx->code_spec->qcmdpc->n0 - 1);
            BPU_gf2PolyFree(&H_temp[ctx->code_spec->qcmdpc->n0 - 1], 0);
            ret +=
                BPU_gf2PolyInitRand(&H_temp[ctx->code_spec->qcmdpc->n0 - 1],
                                    ctx->code_spec->qcmdpc->m,
                                    wi[ctx->code_spec->qcmdpc->n0 - 1], 1);
#if defined(DEBUG_L)
            BPU_printGf2Poly(&H_temp[ctx->code_spec->qcmdpc->n0 - 1]);
#endif
            BPU_gf2PolyFree(&H_last_inv, 0);
        }
    }

#if defined(DEBUG_L)
    BPU_printDebug("inversion to H[%i] found ", ctx->code_spec->qcmdpc->n0 - 1);
//    BPU_printGf2Poly(&H_last_inv);
    BPU_printDebug("creating H matrix");
#endif

    // create H temp matrix
    BPU_gf2QcMatrixMalloc(&H_temp_mat, ctx->code_spec->qcmdpc->n0,
                          ctx->code_spec->qcmdpc->m, 0, 0);
    for (i = 0; i < ctx->code_spec->qcmdpc->n0; i++) {
        H_temp[i].len = ctx->code_spec->qcmdpc->m;
        BPU_gf2PolyCopy(&H_temp_mat.matrices[i], &H_temp[i]);
    }

    BPU_gf2QcMatrixToSparse(&H_temp_sparse, &H_temp_mat, wi);

#if defined(DEBUG_L)
    BPU_printDebug("H: ");
//    BPU_printGf2QcMatrix(&H_temp_mat);
    BPU_printDebug("H sparse: ");
//    BPU_printGf2SparseQcMatrix(&H_temp_sparse);
    BPU_printDebug("creating G matrix");
#endif

    // create G temp matrix
    for (i = 0; i < ctx->code_spec->qcmdpc->n0 - 1; i++) {
        BPU_printDebug("multiplicating vectors H[%i]^-1 x H[%i]",
                       ctx->code_spec->qcmdpc->n0 - 1, i);
        BPU_gf2PolyMulMod(&H_last_inv, &H_temp[i], &G_temp[i], &mod, 0);
#if defined(DEBUG_L)
//      BPU_printGf2Poly(&G_temp[i]);
#endif
    }

    BPU_printDebug("creating temp G for GH^T test");
    BPU_gf2QcMatrixMalloc(&G_temp_mat, ctx->code_spec->qcmdpc->n0 - 1,
                          ctx->code_spec->qcmdpc->m, 0, 1);
    for (i = 0; i < ctx->code_spec->qcmdpc->n0 - 1; i++) {
        BPU_gf2PolyCopy(&G_temp_mat.matrices[i], &G_temp[i]);
    }

    ret = 0;

    BPU_printDebug("testing GH^T");

    // test if G x H^T = 0
    if (BPU_mecsQcmdpcTestGHmatrices(&G_temp_mat, &H_temp_sparse) != 0) {
        BPU_printError("generator x parity check matrix ERROR");
        ret = -1;
    }
    else {
        BPU_printDebug("GH^t = 0");
    }

    // transpose G matrix
    if (ret == 0) {
        BPU_gf2QcMatrixTransp(&ctx->code_spec->qcmdpc->G, &G_temp_mat);
#if defined(DEBUG_L)
        BPU_printDebug("transposing G matrix");
//      BPU_printGf2QcMatrix(&ctx->code_spec->qcmdpc->G);
#endif
    }

    // transpose H matrix
    if (ret == 0) {
        BPU_gf2SparseQcMatrixTransp(&ctx->code_spec->qcmdpc->H, &H_temp_sparse);
#if defined(DEBUG_L)
        BPU_printDebug("transposing H matrix");
//      BPU_printGf2SparseQcMatrix(&ctx->code_spec->qcmdpc->H);
#endif
    }

    BPU_printDebug("free and exit");

    // free
    for (i = 0; i < ctx->code_spec->qcmdpc->n0 - 1; i++)
        BPU_gf2PolyFree(&G_temp[i], 0);

    for (i = 0; i < ctx->code_spec->qcmdpc->n0; i++)
        BPU_gf2PolyFree(&H_temp[i], 0);

    BPU_gf2PolyFree(&H_last_inv, 0);
    BPU_gf2PolyFree(&mod, 0);
    BPU_gf2QcMatrixFree(&G_temp_mat, 0);
    BPU_gf2QcMatrixFree(&H_temp_mat, 0);
    BPU_gf2SparseQcMatrixFree(&H_temp_sparse, 0);

    return ret;
}

int BPU_mecsQcmdpcTestGHmatrices(const BPU_T_GF2_QC_Matrix * G,
                                 const BPU_T_GF2_Sparse_Qc_Matrix * H) {
    int i, element, err = 0;
    BPU_T_GF2_Poly temp;
    BPU_T_GF2_Sparse_Poly row;
    BPU_T_Element tmp;

    // get I * H[0] + ... + I * H[n0-2]
    BPU_gf2PolyMalloc(&temp, G->n);
    for (i = 0; i < G->element_count; i++) {
        BPU_gf2SparsePolyAdd(&temp, &H->matrices[i]);
    }

    // get other rows
    for (element = 0; element < G->element_count; element++) {
        for (i = 0; i < G->element_size; i++) {
            if (BPU_gf2VecGetBit(&G->matrices[element], i) == 1ul) {
                BPU_gf2SparseQcMatrixGetRow(&row, H,
                                            i +
                                            G->element_size *
                                            (G->element_count));
                BPU_gf2SparsePolyAdd(&temp, &row);
                BPU_gf2SparsePolyFree(&row, 0);
            }
        }
    }
    // check result poly
    for (i = 0; i < temp.array_length; i++) {
        if ((tmp = temp.elements[i]) != 0ul) {
            err++;
            break;
        }
    }

#if defined(DEBUG_L)
    BPU_T_GF2_QC_Matrix GH_result;

    BPU_gf2QcMatrixMalloc(&GH_result, 1, G->element_size, 0, 0);
    BPU_gf2PolyCopy(&GH_result.matrices[0], &temp);
//    BPU_printGf2QcMatrix(&GH_result);
    BPU_gf2QcMatrixFree(&GH_result, 0);
#endif

    BPU_gf2PolyFree(&temp, 0);

    return err;
}

#endif
