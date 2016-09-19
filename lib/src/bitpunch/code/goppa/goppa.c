/**
This file is part of BitPunch
Copyright (C) 2013-2015 Frantisek Uhrecky <frantisek.uhrecky[what here]gmail.com>
Copyright (C) 2013-2014 Andrej Gulyas <andrej.guly[what here]gmail.com>
Copyright (C) 2013-2014 Marek Klein  <kleinmrk[what here]gmail.com>
Copyright (C) 2013-2014 Filip Machovec  <filipmachovec[what here]yahoo.com>
Copyright (C) 2013-2014 Jozef Kudlac <jozef[what here]kudlac.sk>

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
#include "goppa.h"

#include <stdlib.h>

#include <bitpunch/math/gf2.h>
#include <bitpunch/prng/prng.h>
#include <bitpunch/math/perm.h>
#include <bitpunch/debugio.h>
#include <bitpunch/bitpunch.h>

int BPU_goppaEncode(BPU_T_GF2_Vector *out, const BPU_T_GF2_Vector *in, const struct _BPU_T_Code_Ctx *ctx) {
	int rc = 0;
	BPU_T_GF2_Vector tmp;

	BPU_gf2VecNull(out);

	// test the size of message and g_m
	fprintf(stderr, "%d\n", in->len);
	if (in->len != ctx->code_spec->goppa->g_mat->k) {
		BPU_printError("message length have to be of length %d", ctx->code_spec->goppa->g_mat->k);

		return -1;
	}
	BPU_gf2VecMalloc(&tmp, ctx->code_spec->goppa->g_mat->n);
	rc += BPU_gf2VecMulMat(&tmp, in, ctx->code_spec->goppa->g_mat);
	rc += BPU_gf2VecConcat(out, &tmp, in);
	BPU_gf2VecFree(&tmp, 0);

	return rc;
}

int BPU_goppaEncodeM(BPU_T_GF2_Vector *out, const BPU_T_GF2_Vector *in, const struct _BPU_T_Code_Ctx *ctx) {
	int rc = 0;
	int i, j;
	uint8_t out_bit = 0;
	BPU_T_GF2 out_dword;
	BPU_T_GF2_Vector tmp;

	BPU_gf2VecNull(out);

	// test the size of message and g_m
	if (in->len != ctx->code_spec->goppa->g_mat->n) {
		BPU_printError("message length have to be of length %d", ctx->code_spec->goppa->g_mat->n);

		return -1;
	}
	BPU_gf2VecMalloc(&tmp, ctx->code_spec->goppa->g_mat->k);
	for (j = 0; j < ctx->code_spec->goppa->g_mat->k; j++) {
		out_dword = 0;
		for (i = 0; i < ctx->code_spec->goppa->g_mat->elements_in_row; i++) {
			out_dword ^= in->elements[i] & ctx->code_spec->goppa->g_mat->elements[j][i];
		}
		out_bit = BPU_getParity(out_dword);
		BPU_gf2VecSetBit(&tmp, j, out_bit);
	}

	rc += BPU_gf2VecConcat(out, &tmp, in);
	BPU_gf2VecFree(&tmp, 0);

	return rc;
}

int BPU_goppaDecode(BPU_T_GF2_Vector *out, const BPU_T_GF2_Vector *in, const struct _BPU_T_Code_Ctx *ctx) {
	BPU_T_GF2_Vector orig_enc;
	BPU_T_GF2_Vector error_vec;

	// get error vector
	BPU_gf2VecMalloc(&error_vec, in->len);
	BPU_goppaGetError(&error_vec, in, ctx);

	// remove error
	BPU_gf2VecMalloc(&orig_enc, in->len);
	BPU_gf2VecCopy(&orig_enc, in);
	BPU_gf2VecXor(&orig_enc, &error_vec);
	BPU_gf2VecFree(&error_vec, 0);
	// get message
	BPU_gf2VecCrop(out, &orig_enc, in->len - ctx->msg_len, ctx->msg_len);
	BPU_gf2VecFree(&orig_enc, 0);

	return 0;
}

int BPU_goppaGetError(BPU_T_GF2_Vector *error, const BPU_T_GF2_Vector *encoded, const BPU_T_Code_Ctx *ctx) {
	BPU_T_Perm_Vector inv_perm;
	BPU_T_GF2_16x_Poly syndrome, tau, a, b, sigma, inv_syndrome, tmp, tmp2;
	int l;
	BPU_T_GF2_16x tmp_eval;
	BPU_T_GF2_Vector enc_permuted;


	// permute code word
	BPU_gf2VecMalloc(&enc_permuted, encoded->len);
	BPU_gf2VecCopy(&enc_permuted, encoded);

	BPU_permMalloc(&inv_perm, ctx->code_spec->goppa->permutation->size);
	BPU_permGetInv(&inv_perm, ctx->code_spec->goppa->permutation);
	BPU_gf2VecPermute(&enc_permuted, &inv_perm);
	BPU_permFree(&inv_perm, 0);

	// Beginning of patterson
	BPU_gf2xPolyMalloc(&syndrome, ctx->code_spec->goppa->g->deg - 1);
	BPU_goppaDetSyndromeM(&syndrome, &enc_permuted, ctx);
	BPU_gf2VecFree(&enc_permuted, 0);

	BPU_gf2xPolyMalloc(&inv_syndrome, (syndrome.deg > ctx->code_spec->goppa->g->deg) ? syndrome.deg : ctx->code_spec->goppa->g->deg);


	BPU_gf2xPolyInv(&inv_syndrome, &syndrome, ctx->code_spec->goppa->g, ctx->math_ctx);

	BPU_gf2xPolyFree(&syndrome, 0);
	inv_syndrome.coef[1] = inv_syndrome.coef[1] ^ 1;

	// get square root
	BPU_gf2xPolyMalloc(&tau, ctx->code_spec->goppa->g->deg);
	BPU_gf2xPolyRoot(&tau, &inv_syndrome, ctx->code_spec->goppa->g, ctx->math_ctx);
	BPU_gf2xPolyFree(&inv_syndrome, 0);
	/**************** FROM NOW WE ARE NOT USING MODULUS g for a, b ********************/
	BPU_gf2xPolyMalloc(&a, (tau.deg > ctx->code_spec->goppa->g->deg) ? tau.deg : ctx->code_spec->goppa->g->deg);
	BPU_gf2xPolyMalloc(&b, a.max_deg);

	BPU_goppaFindPolyAB(&a, &b, &tau, ctx->code_spec->goppa->g, ctx->math_ctx);
	BPU_gf2xPolyFree(&tau, 0);

	BPU_gf2xPolyMalloc(&tmp2, 2 * ctx->code_spec->goppa->g->deg);
	BPU_gf2xPolyMalloc(&tmp, 2 * ctx->code_spec->goppa->g->deg);

	// a^2, b^2
	BPU_gf2xPolyMul(&tmp, &a, &a, ctx->math_ctx);
	BPU_gf2xPolyMul(&tmp2, &b, &b, ctx->math_ctx);

	// copy a^2, b^2 to a, b
	BPU_gf2xPolyCopy(&a, &tmp);
	BPU_gf2xPolyFree(&tmp, 0);

	BPU_gf2xPolyCopy(&b, &tmp2);
	BPU_gf2xPolyFree(&tmp2, 0);

	// b^2 * x
	BPU_gf2xPolyShl(&b, 1);
	BPU_gf2xPolyMalloc(&sigma, ctx->code_spec->goppa->g->deg);
	
	// calculate sigma = a^2 + x * b^2
	BPU_gf2xPolyAdd(&sigma, &a, &b);
	BPU_gf2xPolyFree(&a, 0);
	BPU_gf2xPolyFree(&b, 0);
	// check if there is enough space
	if (error->len < ctx->code_spec->goppa->support_len) {
		BPU_gf2VecFree(error, 0);

		BPU_gf2VecMalloc(error, ctx->code_spec->goppa->support_len);
	}
	else {
		BPU_gf2VecNull(error);
	}

	sigma.deg = ctx->t;
	for (l = 0; l < ctx->code_spec->goppa->support_len; l++) {
		tmp_eval = BPU_gf2xPolyEvalC(&sigma, ctx->math_ctx->exp_table[l], ctx->math_ctx);
		BPU_gf2VecSetBit(error, l, !tmp_eval);
	}

	// permute error vector
	BPU_gf2VecPermute(error, ctx->code_spec->goppa->permutation);
	BPU_gf2xPolyFree(&sigma, 0);
	return 0;
}

void BPU_goppaDetSyndromeM(BPU_T_GF2_16x_Poly *syndrome, const BPU_T_GF2_Vector *z, const BPU_T_Code_Ctx *ctx) {
    int row, column;
#ifndef BPU_GOPPA_WITH_H
    int k, e, bit;
	BPU_T_GF2_16x element, divider;
#endif
    BPU_gf2xPolyNull(syndrome);
#ifdef BPU_GOPPA_WITH_H
	for (column = 0; column < z->len; column++) {
		for (row = 0; row < ctx->code_spec->goppa->h_mat->k; row++) {
			syndrome->coef[syndrome->max_deg - row] ^= BPU_gf2VecGetBit(z, column) * ctx->code_spec->goppa->h_mat->elements[row][column];
		}
	}
#else
	for(column = 0; column < z->len; column++) {
		bit = BPU_gf2VecGetBit(z, column);
		divider = BPU_gf2xPowerModT(BPU_gf2xPolyEval(ctx->code_spec->goppa->g, ctx->math_ctx->exp_table[column], ctx->math_ctx), -1, ctx->math_ctx);
			for(row = 0; row < ctx->code_spec->goppa->g->deg; row++) {
				element = 0;
				for(k = ctx->code_spec->goppa->g->deg - row, e = 0; k <= ctx->code_spec->goppa->g->deg; k++, e++) {
					element ^= BPU_gf2xMulMod(ctx->code_spec->goppa->g->coef[k], BPU_gf2xPowerModT (ctx->math_ctx->exp_table[column], e, ctx->math_ctx), ctx->math_ctx->mod);
				}
				element = BPU_gf2xMulMod(element, divider, ctx->math_ctx->mod);
				syndrome->coef[syndrome->max_deg - row] ^= bit * element;
			}
	}
#endif
	syndrome->deg = BPU_gf2xPolyGetDeg(syndrome);  	
}

void BPU_goppaFindPolyAB(BPU_T_GF2_16x_Poly *a, BPU_T_GF2_16x_Poly *b, const BPU_T_GF2_16x_Poly *tau, const BPU_T_GF2_16x_Poly *mod, const BPU_T_Math_Ctx *math_ctx) {
    BPU_T_GF2_16x_Poly tmp;
    int end_deg = mod->deg / 2;

    BPU_gf2xPolyMalloc(&tmp, (tau->deg > mod->deg) ? tau->deg : mod->deg);
//    BPU_gf2xPolyExtEuclidC(a, &tmp, b, mod, tau, end_deg, math_ctx);
    BPU_gf2xPolyExtEuclid(a, &tmp, b, mod, tau, end_deg, math_ctx);
    BPU_gf2xPolyFree(&tmp, 0);
}

int BPU_goppaInitMatH2(BPU_T_GF2_Matrix *h2, BPU_T_GF2_16x_Matrix *hx, const BPU_T_Code_Ctx *ctx) {
	int bit, bit_in_element = -1, act_element = 0;
	int element_bit_size = ctx->math_ctx->mod_deg;
	int k, row, column, e;
	BPU_T_GF2_16x element, divider;

	if (h2->k != ctx->code_spec->goppa->g->deg * element_bit_size || h2->n != ctx->code_spec->goppa->support_len) {
		BPU_printError("Matrix h2 dimension should be %dx%d, current is %dx%d", ctx->code_spec->goppa->g->deg * element_bit_size, ctx->code_spec->goppa->support_len, h2->k, h2->n);

		return -1;
	}
#ifdef BPU_GOPPA_WITH_H
	if (hx->k != ctx->code_spec->goppa->g->deg || hx->n != ctx->code_spec->goppa->support_len) {
		BPU_printError("Matrix hx dimension should be %dx%d, current is %dx%d", ctx->code_spec->goppa->g->deg, ctx->math_ctx->ord, hx->k, hx->n);

		return -1;
	}
#endif
	for(column = 0; column < h2->n; column++) {
		divider = BPU_gf2xPowerModT(BPU_gf2xPolyEval(ctx->code_spec->goppa->g, ctx->math_ctx->exp_table[column], ctx->math_ctx), -1, ctx->math_ctx);
		if ((column - act_element * h2->element_bit_size) >= h2->element_bit_size) { // next elemenet, first bit
			act_element++;
			bit_in_element = 0;
		}
		else // same element, next bit
			bit_in_element++;
		for(row = 0; row < ctx->code_spec->goppa->g->deg; row++) {
			element = 0;
			for(k = ctx->code_spec->goppa->g->deg - row, e = 0; k <= ctx->code_spec->goppa->g->deg; k++, e++) {
				element ^= BPU_gf2xMulModT(ctx->code_spec->goppa->g->coef[k], BPU_gf2xPowerModT(ctx->math_ctx->exp_table[column], e, ctx->math_ctx), ctx->math_ctx);
			}
			element = BPU_gf2xMulModT(element, divider, ctx->math_ctx);
#ifdef BPU_GOPPA_WITH_H
			hx->elements[row][column] = element;
#endif
			for (bit = 0; bit < element_bit_size; bit++) { // bit loop through element of matrix
				h2->elements[row*element_bit_size + bit][act_element] ^= BPU_getBit(element, bit) << (bit_in_element); // get bit from element and shift it
			}
		}
	}
	return 0;
}

int BPU_goppaGenCode(BPU_T_Code_Ctx *ctx) {
	int rc = 0;
	int permute = -1; // needed for equivalent codes
	BPU_T_Perm_Vector temp;

	ctx->code_spec->goppa->g = (BPU_T_GF2_16x_Poly *) calloc(1, sizeof(BPU_T_GF2_16x_Poly));
	BPU_gf2xPolyMalloc(ctx->code_spec->goppa->g, ctx->t);
	BPU_gf2xPolyGenGoppa(ctx->code_spec->goppa->g, ctx->t, ctx->math_ctx);
	ctx->code_spec->goppa->g_mat = (BPU_T_GF2_Matrix *) calloc(1, sizeof(BPU_T_GF2_Matrix));
#ifdef BPU_GOPPA_WITH_H
	ctx->code_spec->goppa->h_mat = (BPU_T_GF2_16x_Matrix*) calloc(1, sizeof(BPU_T_GF2_16x_Matrix));
	BPU_gf2xMatMalloc(ctx->code_spec->goppa->h_mat, ctx->code_spec->goppa->g->deg, ctx->code_spec->goppa->support_len);
#else
	ctx->code_spec->goppa->h_mat = NULL;
#endif
	ctx->code_spec->goppa->permutation = (BPU_T_Perm_Vector *) calloc(1, sizeof(BPU_T_Perm_Vector));

	BPU_gf2MatMalloc(ctx->code_spec->goppa->g_mat, ctx->code_spec->goppa->g->deg * ctx->math_ctx->mod_deg, ctx->code_spec->goppa->support_len);
	rc = BPU_goppaInitMatH2(ctx->code_spec->goppa->g_mat, ctx->code_spec->goppa->h_mat, ctx);
	if (rc) {
		BPU_printError("Can not initialize H matrix.");

		return rc;
	}
	// prepare permutations
	BPU_permMalloc(ctx->code_spec->goppa->permutation, ctx->code_spec->goppa->support_len);
	BPU_permMalloc(&temp, ctx->code_spec->goppa->permutation->size);
	BPU_permRandomize(ctx->code_spec->goppa->permutation);
	BPU_gf2MatPermute(ctx->code_spec->goppa->g_mat, ctx->code_spec->goppa->permutation);

	while (permute != 0) {
		permute = BPU_gf2MatMakeSystematic(ctx->code_spec->goppa->g_mat);
		if (permute != 0) {
			BPU_permRandomize(&temp);
			BPU_permPermute(ctx->code_spec->goppa->permutation, &temp);
			BPU_gf2MatPermute(ctx->code_spec->goppa->g_mat, &temp);
		}
	}
	rc = BPU_gf2MatCropMemory(ctx->code_spec->goppa->g_mat, (ctx->code_spec->goppa->g_mat->n - ctx->code_spec->goppa->g_mat->k));
	BPU_permFree(&temp, 0);

	if (rc != 0) {
		BPU_printGf2Mat(ctx->code_spec->goppa->g_mat);
		BPU_printError("BPU_genKeyPair: can not crop matrix");

		return -1;
	}
	return rc;
}
