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
#include <bitpunch/math/perm.h>
#include <bitpunch/debugio.h>

int BPU_goppaEncode(BPU_T_GF2_Vector *out, const BPU_T_GF2_Vector *in, const struct _BPU_T_Code_Ctx *ctx) {
	int rc = 0;
	BPU_T_GF2_Vector tmp;

	BPU_gf2VecNull(out);

	// test the size of message and g_m
	if (in->len != ctx->code_spec.goppa->g_mat->k) {
		BPU_printError("message length have to be of length %d", ctx->code_spec.goppa->g_mat->k);

		return -1;
	}
	BPU_gf2VecMalloc(&tmp, ctx->code_spec.goppa->g_mat->n);
	rc += BPU_gf2VecMulMat(&tmp, in, ctx->code_spec.goppa->g_mat);
	rc += BPU_gf2VecConcat(out, &tmp, in);
	BPU_gf2VecFree(&tmp, 0);

	return rc;
}

int BPU_goppaDecode(BPU_T_GF2_Vector *out, const BPU_T_GF2_Vector *in, const struct _BPU_T_Code_Ctx *ctx) {
	// TODO: remove space allocation
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
	// Vector_GF2 error;
	int l;
	BPU_T_GF2_16x tmp_eval;
	BPU_T_GF2_Vector enc_permuted;

#ifdef BPU_DEBUG_DECODE
	BPU_printDebug("###### BPU_DEBUG_DECODE - BEGIN ######");
	BPU_printDebug("inverse permutation:");
	BPU_printPerm(&inv_perm);

	BPU_printDebug("before:");
	BPU_printGf2Vec(encoded);
#endif
	// permute code word
	BPU_gf2VecMalloc(&enc_permuted, encoded->len);
	BPU_gf2VecCopy(&enc_permuted, encoded);

	BPU_permMalloc(&inv_perm, ctx->code_spec.goppa->permutation->size);
	BPU_permGetInv(&inv_perm, ctx->code_spec.goppa->permutation);
	BPU_gf2VecPermute(&enc_permuted, &inv_perm);
	BPU_permFree(&inv_perm, 0);

#ifdef BPU_DEBUG_DECODE
	BPU_printDebug("after:");
	BPU_printGf2Vec(encoded);
#endif
	// Beginning of patterson
	BPU_goppaDetSyndromeA(&syndrome, &enc_permuted, ctx->code_spec.goppa->h_mat);
	BPU_gf2VecFree(&enc_permuted, 0);

#ifdef BPU_DEBUG_DECODE
	BPU_printDebug("Syndrome");
	BPU_printGf2xPoly(&syndrome, (ctx->math_ctx));
#endif
	BPU_gf2xPolyInvA(&inv_syndrome, &syndrome, ctx->code_spec.goppa->g, ctx->math_ctx);
	BPU_gf2xPolyFree(&syndrome, 0);
	inv_syndrome.coef[1] = inv_syndrome.coef[1] ^ 1;

	// get square root
	BPU_gf2xPolyMalloc(&tau, ctx->code_spec.goppa->g->deg);
	BPU_gf2xPolyRoot(&tau, &inv_syndrome, ctx->code_spec.goppa->g, ctx->math_ctx);
	BPU_gf2xPolyFree(&inv_syndrome, 0);

#ifdef BPU_DEBUG_DECODE
	BPU_printDebug("inv_syndrome:");
	BPU_printGf2xPoly(&inv_syndrome, (ctx->math_ctx));
	BPU_printDebug("tau:");
	BPU_printGf2xPoly(&tau, (ctx->math_ctx));

	BPU_gf2xPolyNull(&inv_syndrome);
	BPU_gf2xPolyMul(&inv_syndrome, &tau, &tau, (ctx->math_ctx));
	BPU_gf2xPolyMod(&syndrome, &inv_syndrome, &(ctx->priv_key.g), (ctx->math_ctx));

	BPU_printDebug("tau^2 mod g:");
	BPU_printGf2xPoly(&syndrome, (ctx->math_ctx));
#endif
	/**************** FROM NOW WE ARE NOT USING MODULUS g for a, b ********************/
	BPU_goppaFindPolyAB(&a, &b, &tau, ctx->code_spec.goppa->g, ctx->math_ctx);
	BPU_gf2xPolyFree(&tau, 0);

#ifdef BPU_DEBUG_DECODE
	BPU_mallocPoly(&tmp2, 2 * ctx->priv_key->g.deg);
	BPU_mallocPoly(&tmp, 2 * ctx->priv_key->g.deg);
	// b * tau == a mod g
	BPU_gf2xPolyMul(&tmp, &tau, &b, (ctx->math_ctx));
	BPU_gf2xPolyMod(&tmp2, &tmp, &(ctx->priv_key->g), (ctx->math_ctx));
	
	BPU_printDebug("b * tau == a mod g:");
	BPU_printGf2xPoly(&tmp2, (ctx->math_ctx));

	BPU_freePoly(&tmp2, 0);
	BPU_freePoly(&tmp, 0);
#endif    
	BPU_gf2xPolyMalloc(&tmp2, 2 * ctx->code_spec.goppa->g->deg);
	BPU_gf2xPolyMalloc(&tmp, 2 * ctx->code_spec.goppa->g->deg);

	// a^2, b^2
	BPU_gf2xPolyMul(&tmp, &a, &a, ctx->math_ctx);
	BPU_gf2xPolyMul(&tmp2, &b, &b, ctx->math_ctx);

#ifdef BPU_DEBUG_DECODE
	BPU_printDebug("a:");
	BPU_printGf2xPoly(&a, (ctx->math_ctx));
	BPU_printDebug("b:");
	BPU_printGf2xPoly(&b, (ctx->math_ctx));
#endif
	// copy a^2, b^2 to a, b
	BPU_gf2xPolyCopy(&a, &tmp);
	BPU_gf2xPolyFree(&tmp, 0);

	BPU_gf2xPolyCopy(&b, &tmp2);
	BPU_gf2xPolyFree(&tmp2, 0);

#ifdef BPU_DEBUG_DECODE
	BPU_printDebug("a^2:");
	BPU_printGf2xPoly(&a, (ctx->math_ctx));
	BPU_printDebug("b^2:");
	BPU_printGf2xPoly(&b, (ctx->math_ctx));
#endif
	// b^2 * x
	BPU_gf2xPolyShl(&b, 1);

#ifdef BPU_DEBUG_DECODE
	BPU_printDebug("x * b^2:");
	BPU_printGf2xPoly(&b, (ctx->math_ctx));    
#endif
	BPU_gf2xPolyMalloc(&sigma, ctx->code_spec.goppa->g->deg);
	
	// calculate sigma = a^2 + x * b^2
	BPU_gf2xPolyAdd(&sigma, &a, &b);
	BPU_gf2xPolyFree(&a, 0);
	BPU_gf2xPolyFree(&b, 0);

#ifdef BPU_DEBUG_DECODE
	BPU_printDebug("sigma");
	BPU_printGf2xPoly(&sigma, (ctx->math_ctx));
#endif
	// check if there is enough space
	if (error->len < ctx->math_ctx->ord) {
		BPU_gf2VecFree(error, 0);

		BPU_gf2VecMalloc(error, ctx->math_ctx->ord);
	}
	else {
		BPU_gf2VecNull(error);
	}
	
	for (l = 0; l < ctx->math_ctx->ord; l++) {
		tmp_eval = BPU_gf2xPolyEval(&sigma, ctx->math_ctx->exp_table[l], ctx->math_ctx);

		if (tmp_eval == 0) {
			BPU_gf2VecSetBit(error, l, 1);
		}
	}
	// permute error vector
	BPU_gf2VecPermute(error, ctx->code_spec.goppa->permutation);
	BPU_gf2xPolyFree(&sigma, 0);

#ifdef BPU_DEBUG_DECODE
	BPU_printDebug("###### BPU_DEBUG_DECODE - END ######");
#endif  
	return 0;
}

void BPU_goppaDetSyndromeA(BPU_T_GF2_16x_Poly *syndrome, const BPU_T_GF2_Vector *z, const BPU_T_GF2_16x_Matrix *H) {
	int i, j;
	BPU_gf2xPolyMalloc(syndrome, H->k - 1);
	BPU_gf2xPolyNull(syndrome);
	for (i = 0; i < z->len; i++) {
		if (BPU_gf2VecGetBit(z, i)) {
			for (j = 0; j < H->k; j++) {
				syndrome->coef[syndrome->max_deg - j] ^= H->elements[j][i];
			}
		}
	}
	syndrome->deg = BPU_gf2xPolyGetDeg(syndrome);  
}

void BPU_goppaFindPolyAB(BPU_T_GF2_16x_Poly *a, BPU_T_GF2_16x_Poly *b, const BPU_T_GF2_16x_Poly *tau, const BPU_T_GF2_16x_Poly *mod, const BPU_T_Math_Ctx *math_ctx) {
	BPU_T_GF2_16x_Poly tmp;
	int end_deg = mod->deg / 2;

	BPU_gf2xPolyExtEuclidA(a, b, &tmp, tau, mod, end_deg, math_ctx);
	BPU_gf2xPolyFree(&tmp, 0);
}

int BPU_goppaInitMatX(BPU_T_GF2_16x_Matrix *m, BPU_T_GF2_16x_Poly *poly) {
	int i, j;

	// allocate memory
	if (BPU_gf2xMatMalloc(m, poly->deg, poly->deg)) {
		BPU_printError("BPU_initMatX: allocation error");
		
		return 1;
	}
	// insert data
	for (i = 0; i < m->k; i++) {
		for (j = 0; j < m->n; j++) {
			if (j > i) {
				m->elements[i][j] = 0;    
			}
			else {
				m->elements[i][j] = poly->coef[poly->deg - i + j];
			}            
		} // col loop
	} // row loop
	return 0;
}

int BPU_goppaInitMatY(BPU_T_GF2_16x_Matrix *m, uint8_t t, BPU_T_Math_Ctx *math_ctx) {
	int i, j;

	// allocate memory
	if (BPU_gf2xMatMalloc(m, t, math_ctx->ord)) {
		BPU_printError("BPU_initMatY: allocation error");
		
		return 1;
	}
	// insert data
	for (i = 0; i < m->k; i++) {
		for (j = 0; j < m->n; j++) {
			// first row are ones
			if (i == 0) {
				m->elements[i][j] = (BPU_T_GF2_16x)1;
			}
			else {
				// field elements powered on j
				m->elements[i][j] = BPU_gf2xPowerModT(math_ctx->exp_table[j], i, math_ctx);
			}
		} // col loop
	} // row loop
	return 0;
}

int BPU_goppaInitMatZ(BPU_T_GF2_16x_Matrix *m, BPU_T_GF2_16x_Poly *poly, BPU_T_Math_Ctx *math_ctx) {
	int i, j;

	// allocate memory
	if (BPU_gf2xMatMalloc(m, math_ctx->ord, math_ctx->ord)) {
		BPU_printError("BPU_initMatZ: allocation error");        
		return 1;
	}
	// insert data
	for (i = 0; i < m->k; i++) {
		for (j = 0; j < m->n; j++) {
			
			if (i == j) {
				// diagonal
				m->elements[i][j] = BPU_gf2xPowerModT(BPU_gf2xPolyEval(poly, math_ctx->exp_table[i], math_ctx), -1, math_ctx);
			}
			else {
				// if it is not diagonal, there are zeros
				m->elements[i][j] = (BPU_T_GF2_16x)0;
			}
		} // col loop
	} // row loop
	return 0;
}


int BPU_goppaInitMatH(BPU_T_GF2_16x_Matrix *m, BPU_T_GF2_16x_Poly *poly, BPU_T_Math_Ctx *math_ctx) {
	// int rc = 0;
// 	BPU_T_GF2_16x_Matrix tmp_mat, x_m, y_m, z_m;
	
// 	rc += BPU_goppaInitMatX(&(x_m), poly);

// #ifdef DEBUG_INIT_MAT_H
// 	BPU_printDebug("Matrix X:");
// 	BPU_printGf2xMat(&(x_m));
// #endif
	// rc += BPU_goppaInitMatY(&(y_m), poly->deg, math_ctx);

// #ifdef DEBUG_INIT_MAT_H
// 	BPU_printDebug("Matrix Y:");
// 	BPU_printGf2xMat(&(y_m));
// #endif
// 	rc += BPU_goppaInitMatZ(&(z_m), poly, math_ctx);

// #ifdef DEBUG_INIT_MAT_H
// 	BPU_printDebug("Matrix Z:");
// 	BPU_printGf2xMat(&(z_m));
// #endif
// 	rc += BPU_gf2xMatrixMulA(&tmp_mat, &(x_m), &(y_m), math_ctx);
// 	rc += BPU_gf2xMatrixMulA(m, &tmp_mat, &(z_m),math_ctx);

// 	BPU_gf2xMatFree(&tmp_mat, 0);
// 	BPU_gf2xMatFree(&x_m, 0);
// 	BPU_gf2xMatFree(&y_m, 0);
// 	BPU_gf2xMatFree(&z_m, 0);
	int k, rc = 0, row, column, e;
	BPU_T_GF2_16x element, divider;
	rc = BPU_gf2xMatMalloc(m, poly->deg, math_ctx->ord);
	for(column = 0; column < m->n; column++) {
		divider = BPU_gf2xPowerModT(BPU_gf2xPolyEval(poly, math_ctx->exp_table[column], math_ctx), -1, math_ctx);
		for(row = 0; row < m->k; row++) {
			element = 0;
			for(k = poly->deg - row, e = 0; k <= poly->deg; k++, e++) {
				element ^= BPU_gf2xMulMod(poly->coef[k], BPU_gf2xPowerModT (math_ctx->exp_table[column], e, math_ctx), math_ctx->mod);
			}
			m->elements[row][column] = BPU_gf2xMulMod(element, divider, math_ctx->mod);
		}
	}
	return rc;
}

int BPU_goppaGenCode(BPU_T_Code_Ctx *ctx) {
	int rc = 0;
	BPU_T_GF2_Matrix temp, temp2;
	BPU_T_Perm_Vector *permutation = NULL; // needed for equivalent codes
	BPU_T_GF2_16x_Matrix perm_matrix;

	ctx->code_spec.goppa->g = (BPU_T_GF2_16x_Poly *) calloc(1, sizeof(BPU_T_GF2_16x_Poly));
	BPU_gf2xPolyMalloc(ctx->code_spec.goppa->g, ctx->t);
	BPU_gf2xPolyGenGoppa(ctx->code_spec.goppa->g, ctx->t, ctx->math_ctx);

	ctx->code_spec.goppa->h_mat = (BPU_T_GF2_16x_Matrix *) calloc(1, sizeof(BPU_T_GF2_16x_Matrix));
	rc = BPU_goppaInitMatH(ctx->code_spec.goppa->h_mat, ctx->code_spec.goppa->g, ctx->math_ctx);

#ifdef BPU_DEBUG_INIT_MCE_CTX
	BPU_printDebug("Matrix H:");
	BPU_printGf2xMat(mce_ctx->code_ctx->h_mat);
#endif
	ctx->code_spec.goppa->permutation = (BPU_T_Perm_Vector *) calloc(1, sizeof(BPU_T_Perm_Vector));
	while (permutation == NULL) {
		// BPU_permGenA(&(mce_ctx->priv_key->permutation), mce_ctx->code_ctx->h_mat->n);
		BPU_permGenA(ctx->code_spec.goppa->permutation, ctx->code_spec.goppa->h_mat->n);

	#ifdef BPU_DEBUG_INIT_MCE_CTX
		BPU_printDebug("Permutation:");
		BPU_printPerm(&mce_ctx->priv_key.permutation);
		printf("\n");
	#endif
		BPU_gf2xMatMalloc(&perm_matrix, ctx->code_spec.goppa->h_mat->k, ctx->code_spec.goppa->h_mat->n);
		rc = BPU_gf2xMatPermute(&perm_matrix, ctx->code_spec.goppa->h_mat, ctx->code_spec.goppa->permutation);

	#ifdef BPU_DEBUG_INIT_MCE_CTX
		BPU_printDebug("Permuted Matrix H:");
		BPU_printGf2xMat(&(mce_ctx->priv_key->h_mat));
	#endif
		rc = BPU_gf2xMatConvertToGf2MatA(&temp, &perm_matrix, ctx->math_ctx->mod_deg);
		BPU_gf2xMatFree(&perm_matrix, 0);

	#ifdef BPU_DEBUG_INIT_MCE_CTX
		BPU_printDebug("Converted matrix H2:");
		BPU_printGf2Mat(&temp);
	#endif
		permutation = BPU_gf2MatMakeSystematic(&temp);

		if (permutation == NULL) {
			BPU_permFree(ctx->code_spec.goppa->permutation, 0);
			BPU_gf2MatFree(&temp, 0);
		}
	}
	BPU_permPermute(ctx->code_spec.goppa->permutation, permutation);

#ifdef BPU_DEBUG_INIT_MCE_CTX
	BPU_printDebug("Permutation (systematic form):");
	BPU_printPerm(permutation);
	BPU_printDebug("Matrix H2 in systematic form:");
	BPU_printGf2Mat(&temp);
#endif
	BPU_permFree(permutation, 1);

	BPU_gf2MatMalloc(&temp2, temp.k, (temp.n - temp.k));
	rc = BPU_gf2MatCrop(&temp2, &temp, (temp.n - temp.k));
	
	if (rc != 0) {
		BPU_printGf2Mat(&temp);
		BPU_printError("BPU_genKeyPair: can not crop matrix");

		return -1;
	}
	BPU_gf2MatFree(&temp, 0);

#ifdef BPU_DEBUG_INIT_MCE_CTX
	BPU_printDebug("Croped Matrix H2:");
	BPU_printGf2Mat(&temp2);
#endif
	ctx->code_spec.goppa->g_mat = (BPU_T_GF2_Matrix *) calloc(1, sizeof(BPU_T_GF2_Matrix));
	BPU_gf2MatMalloc(ctx->code_spec.goppa->g_mat, temp2.n, temp2.k);
	rc = BPU_gf2MatTransp(ctx->code_spec.goppa->g_mat, &temp2);
	BPU_gf2MatFree(&temp2, 0);

#ifdef BPU_DEBUG_INIT_MCE_CTX
	BPU_printDebug("Transposed croped Matrix H2:");
	BPU_printGf2Mat(&mce_ctx->pub_key->g_mat);
#endif
	return rc;
}
