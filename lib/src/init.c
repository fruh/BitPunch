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
#include "init.h"
#include "arithmetics.h"
#include "globals.h"
#include "debugio.h"
#include "operations.h"
#include "process.h"


int BITP_initLogExpTable(McEliece_Ctx *mce_ctx, GF2_16x g, GF2_16x mod) {
    GF2_16x b = 1;
    int i = 0;

    // get group ord, number of elements
    GF2_16x ord = ((1 << BITP_gf2xGetDeg(mod)) - 1);

    // alocate memory for tables
    mce_ctx->a_data.mod = mod;
    mce_ctx->a_data.mod_deg = BITP_gf2xGetDeg(mod);
    mce_ctx->a_data.log_table = (uint32_t*) malloc(sizeof(uint32_t) * (ord + 1));
    mce_ctx->a_data.exp_table = (GF2_16x*) malloc(sizeof(GF2_16x) * (ord + 1));

    // set ord
    mce_ctx->a_data.ord = ord;

    do {
        mce_ctx->a_data.exp_table[i] = b;
        mce_ctx->a_data.log_table[b] = i;    

        b = gf2xMulMod(b, g, mod);
        i++;
    } while (b != 1);

    mce_ctx->a_data.exp_table[ord] = 0;
    mce_ctx->a_data.log_table[0] = ord;

    if (i != ord) {
        printError("element 0x%x is not generator", g);

        return 1;
    }
    return 0;
}

int BITP_initMatX(Matrix_GF2_16x *m, Polynomial_GF2_16x *poly) {
    int i, j;

    // allocate memory
    if (BITP_mallocMatrix(m, poly->deg, poly->deg)) {
        printError("BITP_initMatX: allocation error");
        
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

int BITP_initMatY(Matrix_GF2_16x *m, uint8_t t, Arithmetic_Data *a_data) {
    int i, j;

    // allocate memory
    if (BITP_mallocMatrix(m, t, a_data->ord)) {
        printError("BITP_initMatY: allocation error");
        
        return 1;
    }
    // insert data
    for (i = 0; i < m->k; i++) {
        for (j = 0; j < m->n; j++) {
            // first row are ones
            if (i == 0) {
                m->elements[i][j] = (GF2_16x)1;
            }
            else {
                // field elements powered on j
                m->elements[i][j] = gf2xPowerModT(a_data->exp_table[j], i, a_data);
            }
        } // col loop
    } // row loop
    return 0;
}

int BITP_initMatZ(Matrix_GF2_16x *m, Polynomial_GF2_16x *poly, Arithmetic_Data *a_data) {
    int i, j;

    // allocate memory
    if (BITP_mallocMatrix(m, a_data->ord, a_data->ord)) {
        printError("BITP_initMatZ: allocation error");        
        return 1;
    }
    // insert data
    for (i = 0; i < m->k; i++) {
        for (j = 0; j < m->n; j++) {
            
            if (i == j) {
                // diagonal
                m->elements[i][j] = gf2xPowerModT(gf2xPolyEval(poly, a_data->exp_table[i], a_data), -1, a_data);
            }
            else {
                // if it is not diagonal, there are zeros
                m->elements[i][j] = (GF2_16x)0;
            }
        } // col loop
    } // row loop
    return 0;
}

void BITP_freeMat(Matrix_GF2_16x *m, int is_dyn) {
    int i;
    
    // first free cols
    for (i = 0; i < m->k; i++) {
        free(m->elements[i]);
    }
    // then free rows
    free(m->elements);
    
    if (is_dyn) {
        free(m);
    }
}

void BITP_freeArithmeticData(Arithmetic_Data *a, int is_dyn) {
    free(a->exp_table);
    free(a->log_table);

    if (is_dyn) {
        free(a);
    }
}

void BITP_freePoly(Polynomial_GF2_16x *p, int is_dyn) {
    free(p->coef);

    if (is_dyn) {
        free(p);
    }
    else {
        p->coef = NULL;
    }
}

void BITP_freePerm(Permutation_Vector *p, int is_dyn) {
    free(p->elements);

    if (is_dyn) {
        free(p);
    }
}

void BITP_freeMatGF2(Matrix_GF2 *m, int is_dyn) {
    int i;
    
    // first free cols
    for (i = 0; i < m->k; i++) {
        free(m->elements[i]);
    }
    // then free rows
    free(m->elements);
    
    if (is_dyn) {
        free(m);
    }
}

void BITP_freeVecGF2(Vector_GF2 *v, int is_dyn) {  
    // then free rows
    free(v->elements);
    
    if (is_dyn) {
        free(v);
    }
}

void freePrivateKey(McEliece_Priv_Key *priv_key, int is_dyn) {
    // free permutation
    BITP_freePerm(&priv_key->permutation, is_dyn);

    BITP_freePoly(&(priv_key->g), 0);

    BITP_freeMat(&(priv_key->h_mat), 0);

    if (is_dyn) {
        free(priv_key);
    }
}

void freePublicKey(McEliece_Pub_Key *pub_key, int is_dyn) {
    // free matrix
    BITP_freeMatGF2(&pub_key->g_mat, is_dyn);

    if (is_dyn) {
        free(pub_key);
    }
}

void BITP_freeMcElieceCtx(McEliece_Ctx *mce_ctx) {
    // free Aritmetics tables
    BITP_freeArithmeticData(&(mce_ctx->a_data), 0);

    // free Private key
    freePrivateKey(&(mce_ctx->priv_key), 0);

    // free Private key
    freePublicKey(&(mce_ctx->pub_key), 0);

    // TODO: free other structures
}

int BITP_mallocPoly(Polynomial_GF2_16x *p, int16_t max_deg) {
    // allocate memory
    p->deg = -1;
    p->max_deg = max_deg;

    if (p->max_deg < 0) {
        printError("BITP_mallocPoly: max_deg must be at least 0");

        return -1;
    }
    p->coef = (GF2_16x*) calloc(max_deg + 1, sizeof(GF2_16x));

    if (!p->coef) {
        printError("BITP_mallocPoly: can not allocate polynomial");

        return -1;
    }
    return 0;
}

int BITP_mallocPerm(Permutation_Vector *p, int size) {
    // allocate memory
    int i;
    p->size = size;
    p->elements = (uint32_t*) malloc(sizeof(uint32_t) * size);

    if (!p->elements) {
        printError("BITP_mallocPerm: can not allocate permutation vector");

        return -1;
    }
    for (i = 0; i < p->size; i++) {
        p->elements[i] = i;
    }
    return 0;
}

int BITP_mallocMatrix(Matrix_GF2_16x *m, int rows, int cols) {
    int i;
    // rows
    m->k = rows;
    // cols
    m->n = cols;
    // allocate rows
    m->elements = (GF2_16x **) malloc(sizeof(GF2_16x*) * m->k);
    
    if (!m->elements) {
        printError("BITP_mallocMatrix: can not allocate memory for matrix rows");
        
        return 1;
    }
    // allocate cols
    for (i = 0; i < m->k; i++) {
        m->elements[i] = (GF2_16x*) malloc(sizeof(GF2_16x) * m->n);
    }

    return 0;
}

int BITP_mallocMatrixGF2(Matrix_GF2 *m, int rows, int cols) {
    int i;

    // element size
    m->element_bit_size = sizeof(GF2)*8;

    // rows
    m->k = rows;
    // cols
    m->n = cols;
    
    // calc how many elements of set size will be in one row
    int modul = 0;
    if ( cols % m->element_bit_size > 0) {
        modul = 1;
    }
    m->elements_in_row = cols/m->element_bit_size + modul;

    // allocate rows
    m->elements = (GF2**) malloc(sizeof(GF2*) * m->k);

    if (!m->elements) {
        printError("BITP_mallocMatrixGF2: can not allocate memory for matrix rows");
        
        return 1;
    }

    // allocate cols
    for (i = 0; i < m->k; i++) {
        m->elements[i] = (GF2*) malloc(sizeof(GF2) * m->elements_in_row);
    }

    // fill matrix with zeros
    for (i = 0; i < m->k; i++) {
        gf2MatNullRow(m, i);
    }
    return 0;
}

int BITP_mallocVectorGF2(Vector_GF2 *v, int len) {
    // element size in bits
    v->element_bit_size = sizeof(GF2) * 8;

    // len
    v->len = len;
    
    // calc how many elements of set size will be in one row
    int modul = 0;

    if ( len % v->element_bit_size > 0) {
        modul = 1;
    }
    v->elements_in_row = len / v->element_bit_size + modul;

    // allocate elemtens
    v->elements = (GF2*) malloc(sizeof(GF2) * v->elements_in_row);

#ifdef DEBUG_MALLOC_VECTOR_GF2
    printDebug("vector len: %d, element_bit_size: %d, elements_in_row: %d", v->len, v->element_bit_size, v->elements_in_row);
#endif
    if (!v->elements) {
        printError("BITP_mallocVectorGF2: can not allocate memory for vector of len %d", len);
        
        return 1;
    }
    // null vector
    gf2VecNull(v);

    return 0;
}

int BITP_genKeyPair(McEliece_Ctx *mce_ctx, uint8_t deg) {
	int rc = 0;
    Matrix_GF2 temp, temp2;
    Permutation_Vector *permutation = NULL; // needed for equivalent codes
    Matrix_GF2_16x perm_matrix;

    mce_ctx->pub_key.t = deg;
    mce_ctx->pub_key.m = mce_ctx->a_data.mod_deg;

    BITP_gf2xPolyGenGoppaA(&(mce_ctx->priv_key.g), deg, &(mce_ctx->a_data));

	rc = BITP_initMatH(&(mce_ctx->priv_key.h_mat), &(mce_ctx->priv_key.g), &(mce_ctx->a_data));

#ifdef DEBUG_INIT_MCE_CTX
    printDebug("Matrix H:");
    BITP_printGf2xMat(&(mce_ctx->priv_key.h_mat));
    printf("\n");
#endif
    while (permutation == NULL) {
        BITP_permGenA(&(mce_ctx->priv_key.permutation), mce_ctx->priv_key.h_mat.n);
        permIsValid(&(mce_ctx->priv_key.permutation));

    #ifdef DEBUG_INIT_MCE_CTX
        printDebug("Permutation:");
        BITP_printPerm(&mce_ctx->priv_key.permutation);
        printf("\n");
    #endif
        BITP_mallocMatrix(&perm_matrix, mce_ctx->priv_key.h_mat.k, mce_ctx->priv_key.h_mat.n);
        rc = BITP_gf2xMatPermute(&perm_matrix, &(mce_ctx->priv_key.h_mat), &(mce_ctx->priv_key.permutation));

    #ifdef DEBUG_INIT_MCE_CTX
        printDebug("Permuted Matrix H:");
        BITP_printGf2xMat(&(mce_ctx->priv_key.h_mat));
    #endif
        rc = BITP_gf2xMatConvertToGf2MatA(&temp, &perm_matrix, mce_ctx->a_data.mod_deg);
        BITP_freeMat(&perm_matrix, 0);

    #ifdef DEBUG_INIT_MCE_CTX
        printDebug("Converted matrix H2:");
        BITP_printGf2Mat(&temp);
    #endif
        permutation = gf2MatMakeSystematicA(&temp);

        if (permutation == NULL) {
            BITP_freePerm(&(mce_ctx->priv_key.permutation), 0);
            BITP_freeMatGF2(&temp, 0);
        }
        else {
            permIsValid(permutation);
        }
    }
    BITP_permPermute(&(mce_ctx->priv_key.permutation), permutation);
    permIsValid(&(mce_ctx->priv_key.permutation));

#ifdef DEBUG_INIT_MCE_CTX
    printDebug("Permutation (systematic form):");
    BITP_printPerm(permutation);
    printDebug("Matrix H2 in systematic form:");
    BITP_printGf2Mat(&temp);
#endif
    BITP_freePerm(permutation, 1);

    rc = BITP_gf2MatCropA(&temp2, &temp, (temp.n - temp.k));
    
    if (rc != 0) {
        BITP_printGf2Mat(&temp);
        printError("BITP_genKeyPair: can not crop matrix");

        return -1;
    }
    BITP_freeMatGF2(&temp, 0);

#ifdef DEBUG_INIT_MCE_CTX
    printDebug("Croped Matrix H2:");
    BITP_printGf2Mat(&temp2);
    printf("\n");
#endif
    rc = BITP_gf2MatTranspA(&mce_ctx->pub_key.g_mat, &temp2);
    BITP_freeMatGF2(&temp2, 0);  

    // BITP_mallocMatrixGF2(&temp2, mce_ctx->pub_key.g_mat.k, mce_ctx->pub_key.g_mat.n);
    // BITP_gf2MatCopy(&temp2, &(mce_ctx->pub_key.g_mat));
    // BITP_freeMatGF2(&(mce_ctx->pub_key.g_mat), 0);
    // gf2MatAppendIdenityA(&(mce_ctx->pub_key.g_mat), &temp2);
    // BITP_freeMatGF2(&temp2, 0);

#ifdef DEBUG_INIT_MCE_CTX
    printDebug("Transposed croped Matrix H2:");
    BITP_printGf2Mat(&mce_ctx->pub_key.g_mat);
    printf("\n");
#endif
    return rc;
}

int BITP_initMatH(Matrix_GF2_16x *m, Polynomial_GF2_16x *poly, Arithmetic_Data *a_data) {
    int rc = 0;
    Matrix_GF2_16x tmp_mat, x_m, y_m, z_m;
    
    rc += BITP_initMatX(&(x_m), poly);

#ifdef DEBUG_INIT_MAT_H
    printDebug("Matrix X:");
    BITP_printGf2xMat(&(x_m));
#endif
	rc += BITP_initMatY(&(y_m), poly->deg, a_data);

#ifdef DEBUG_INIT_MAT_H
    printDebug("Matrix Y:");
    BITP_printGf2xMat(&(y_m));
#endif
	rc += BITP_initMatZ(&(z_m), poly, a_data);

#ifdef DEBUG_INIT_MAT_H
    printDebug("Matrix Z:");
    BITP_printGf2xMat(&(z_m));
#endif
    rc += BITP_gf2xMatrixMulA(&tmp_mat, &(x_m), &(y_m), a_data);
    rc += BITP_gf2xMatrixMulA(m, &tmp_mat, &(z_m),a_data);

    BITP_freeMat(&tmp_mat, 0);
    BITP_freeMat(&x_m, 0);
    BITP_freeMat(&y_m, 0);
    BITP_freeMat(&z_m, 0);
    
	return rc;
}


int initRandVector(Vector_GF2 *out, int l, int w) {
	int i, j;

	if (BITP_mallocVectorGF2(out, l)) {
        printError("initRandVector: allocation error");
        return -1;
    }

	if (w > l) {
        printError("initRandVector: weight error w > l");
        return -2;
    }

	//vector of random weight
	if(w == 0) {
		for(i = 0; i < l; i++) {
			gf2VecSetBit(out, i, getRandValue(0, 2));
		}
	}
	//vector of required weight
	else {
		for(i = 0; i < w; i++) {
			j = getRandValue(0, l);

			if(gf2VecGetBit(out, j) == 0) {
				gf2VecSetBit(out, j, 1);
			}
			else{
				i--;
			}
		}
	}
	return 0;
}

void BITP_mallocVectorGF2_16x(Vector_GF2_16x *vec, int size) {
    vec->len = size;
    vec->elements = (GF2_16x*)malloc(sizeof(GF2_16x) * size);

    memset((void *)(vec->elements), 0, sizeof(GF2_16x) * size);
}

void BITP_freeVectorGF2_16x(Vector_GF2_16x *vec, int is_dyn) {
    free(vec->elements);

    if (is_dyn) {
        free(vec);
    }
}