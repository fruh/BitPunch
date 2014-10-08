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

#include "arithmetics.h"
#include "globals.h"
#include "debugio.h"
#include "init.h"
#include "operations.h"
#include "types.h"

GF2_16x gf2xMulMod(GF2_16x a, GF2_16x b, GF2_16x mod) {
    GF2_16x tmp, tmp2;

    if (a == 0) {
        return 0;
    }
    // (a/x * b)
    tmp = gf2xMulMod(a >> 1, b, mod);

    // tmp * x
    tmp = (tmp << 1);

    // tmp + mod
    tmp2 = tmp ^ mod;

    if (tmp2 < tmp) {
        tmp = tmp2;
    }
    if ((a & ONE) == ONE) {
        tmp ^= b;
    }
    return tmp;
}

GF2_16x gf2xMulModT(const GF2_16x a, const GF2_16x b, const Arithmetic_Data *a_data) {
    if (a == 0 || b == 0) {
        return 0;
    }
    // look into tables
    return a_data->exp_table[(a_data->log_table[a] + a_data->log_table[b]) % a_data->ord];
}

GF2_16x gf2xPowerModT(GF2_16x a, int e, const Arithmetic_Data *a_data) {
    if (a == 0) {
        return 0;
    }
    // look into log table to find i (b^i) = a
    e = e * a_data->log_table[a];
    e = e % a_data->ord;

    if (e < 0) {
        e = e + a_data->ord;
    }
    // look into exp table
    return a_data->exp_table[e];
}

/*** PZ: old version
int BITP_gf2xMatrixMulA(Matrix_GF2_16x *x, const Matrix_GF2_16x *a, const Matrix_GF2_16x *b, const Arithmetic_Data *a_data) {
    uint32_t i, j, k;
    GF2_16x tmp;

    if (a->n != b->k)
        return -1;

    if (BITP_mallocMatrix(x, a->k, b->n) != 0) {
        printError("BITP_gf2xMatrixMulA: allocation error");

        return -2;
    }
          
    for (i = 0; i < a->k; i++) {
        for (j = 0; j < b->n; j++) {
            tmp = 0;
            for (k = 0; k < a->n; k++) {
                tmp ^= gf2xMulModT(a->elements[i][k], b->elements[k][j], a_data);
            }
            x->elements[i][j] = tmp;
        }
    }
    return 0;
}
*/

/*** PZ: speedup critical instructions ***/
int BITP_gf2xMatrixMulA(Matrix_GF2_16x *x, const Matrix_GF2_16x *a, const Matrix_GF2_16x *b, const Arithmetic_Data *a_data) {
    uint32_t i, j, k;
    int loga; 

    if (a->n != b->k)
        return -1;

    if (BITP_mallocMatrix(x, a->k, b->n) != 0) {
        printError("BITP_gf2xMatrixMulA: allocation error");

        return -2;
    }

    for (i = 0; i < a->k; i++) {
        for (j = 0; j < b->n; j++) {
            x->elements[i][j] = 0;
        }
    }

          
    for (i = 0; i < a->k; i++) {
      for (k = 0; k < a->n; k++) {
        if (a->elements[i][k] == 0)
          continue;
        loga = a_data->log_table[a->elements[i][k]];
        for (j = 0; j < b->n; j++) {
            if (b->elements[k][j] == 0)
               continue;
            x->elements[i][j] ^= a_data->exp_table[(loga + a_data->log_table[b->elements[k][j]]) % a_data->ord];
        }
      }
    }
    return 0;
}


int BITP_gf2VecMulMatA(Vector_GF2 *out, const Vector_GF2 *v, const Matrix_GF2 *b) {
    if (BITP_mallocVectorGF2(out, b->n)) {
        return -2;
    }
    return BITP_gf2VecMulMat(out, v, b);
}

int BITP_gf2VecMulMat(Vector_GF2 *out, const Vector_GF2 *v, const Matrix_GF2 *b) {
    int i, j;

    if ((v->len != b->k) || (out->len != b->n)) {
        printError("BITP_gf2VecMulMat: wrong vector and matrix dimension v->len = %d, b->k = %d", v->len, b->k);
        printError("BITP_gf2VecMulMat: wrong vector and matrix dimension out->len = %d, b->n = %d", out->len, b->n);

        return -1;
    }
    // null elements
    gf2VecNull(out);

    for (i = 0; i < v->len; i++) {
        if (gf2VecGetBit(v, i)) {
            // xor rows
            for (j = 0; j < out->elements_in_row; j++) {
                out->elements[j] ^= b->elements[i][j];
            }
        }
    }
    return 0;
}

void gf2MatXorRows(Matrix_GF2 *mat, int i, int j) {
    int k;
    
    for (k = 0; k < mat->elements_in_row; k++) {
        mat->elements[i][k] ^= mat->elements[j][k];
    }
}

int BITP_gf2VecXor(Vector_GF2 *out, const Vector_GF2 *in) {
    int i;

    if (out->elements_in_row != in->elements_in_row) {
        printError("BITP_gf2VecXor: length error %d != %d", out->elements_in_row, in->elements_in_row);

        return -1;
    }
    for (i = 0; i < out->elements_in_row; i++) {
        out->elements[i] ^= in->elements[i];
    }
    return 0;
}

void gf2xPolyAdd(Polynomial_GF2_16x *out, const Polynomial_GF2_16x *a, const Polynomial_GF2_16x *b) {
    int16_t out_deg;
    int i = 0;

    out_deg = a->deg > b->deg ? a->deg : b->deg;

    if (out->max_deg < out_deg) {
        BITP_freePoly(out, 0);
        BITP_mallocPoly(out, out_deg);
    }
    else {
        gf2xPolyNull(out);
    }
    for (i = 0; i <= out_deg; i++) {
        if (i <= a->deg) {
            out->coef[i] ^= a->coef[i];
        }
        if (i <= b->deg) {
            out->coef[i] ^= b->coef[i];
        }
    }
    out->deg = BITP_gf2xPolyGetDeg(out);
}

void BITP_gf2xPolyDiv(Polynomial_GF2_16x *q, Polynomial_GF2_16x *r, const Polynomial_GF2_16x *a, const Polynomial_GF2_16x *b, const Arithmetic_Data *a_data) {
    // a:b = q+r
    Polynomial_GF2_16x tmp;
    GF2_16x leader;
    Polynomial_GF2_16x dividend;
    const Polynomial_GF2_16x *divider = b;
    int exponent;
    int i;
    int max_deg_q;

    BITP_mallocPoly(&dividend, a->max_deg);
    BITP_gf2xPolyCopy(&dividend, a);
    
    max_deg_q = a->deg - b->deg;

    // check size of outputs
    if (q->max_deg < max_deg_q) {
        BITP_freePoly(q, 0);
        BITP_mallocPoly(q, max_deg_q);
    }
    else {
        // here was mordor #2 - forgotten to null outputs :/
        // null outputs
        gf2xPolyNull(q);
    }
    if (r->max_deg < (b->max_deg - 1)) {
        BITP_freePoly(r, 0);
        BITP_mallocPoly(r, b->max_deg - 1);
    }
    else {
        gf2xPolyNull(r);
    }
    BITP_mallocPoly(&tmp, a->max_deg);

    for (i = a->deg; i >= 0; i--) {
        if (dividend.deg < divider->deg) {
            BITP_gf2xPolyCopy(r, &dividend);
            break;
        }
        gf2xPolyNull(&tmp);
        leader = gf2xMulModT(dividend.coef[i], gf2xPowerModT(divider->coef[divider->deg], -1, a_data), a_data);
        exponent = dividend.deg - divider->deg;
        q->coef[exponent] = leader;

        if(q->deg == -1) {
            q->deg = BITP_gf2xPolyGetDeg(q);            
        }
        BITP_gf2xPolyMul(&tmp, divider, q, a_data);

        gf2xPolyAdd(&dividend, a, &tmp);
    }
    BITP_freePoly(&dividend, 0);
    BITP_freePoly(&tmp, 0);
}

void BITP_gf2xPolyMul(Polynomial_GF2_16x *out, const Polynomial_GF2_16x *a, const Polynomial_GF2_16x *b, const Arithmetic_Data *a_data) {
    int i;
    int j;
    int max_deg = a->deg + b->deg;
    
    // MAYBE HERE WAS #### mordor #### - it does
    // if there is not enough space, resize it
    if (out->max_deg < max_deg) {
        BITP_freePoly(out, 0);
        BITP_mallocPoly(out, max_deg);
    }
    else {
        gf2xPolyNull(out);
    }

    for (i = a->deg; i >= 0; i--) {
        for (j = b->deg; j >= 0; j--) {
            out->coef[i+j] ^= gf2xMulModT(a->coef[i], b->coef[j], a_data);
        }
    }
    out->deg = BITP_gf2xPolyGetDeg(out);
}

void BITP_gf2xPolyShr(Polynomial_GF2_16x *a, int n) {
    Polynomial_GF2_16x tmp;

    // if there is nothing to shift, return
    if (a->deg == -1 || n <= 0) {
        return;
    }
    BITP_mallocPoly(&tmp, a->deg);
    BITP_gf2xPolyCopy(&tmp, a);
    gf2xPolyNull(a);

    if (n < tmp.deg + 1) {
        memcpy((void *)(a->coef), (void *)(tmp.coef + n), (tmp.deg + 1 - n) * sizeof(GF2_16x));

        a->deg = BITP_gf2xPolyGetDeg(a);
    }
    BITP_freePoly(&tmp, 0);
}

void BITP_gf2xPolyShl(Polynomial_GF2_16x *a, int n) {
    Polynomial_GF2_16x tmp;

    // if there is nothing to shift, return
    if (a->deg == -1 || n <= 0) {
        return;
    }
    BITP_mallocPoly(&tmp, a->deg);
    BITP_gf2xPolyCopy(&tmp, a);

    if (a->max_deg < a->deg + n) {
        BITP_freePoly(a, 0);

        BITP_mallocPoly(a, a->deg + n);
    }
    else {
        gf2xPolyNull(a);
    }
    memcpy((void *)(a->coef + n), (void *)tmp.coef, (tmp.deg + 1) * sizeof(GF2_16x));
    a->deg = BITP_gf2xPolyGetDeg(a);

    BITP_freePoly(&tmp, 0);
}

void gf2xPolyPower(Polynomial_GF2_16x *a, int e, const Arithmetic_Data *a_data) {
    int i;
    Polynomial_GF2_16x tmp, tmp_2;

    if (e < 0) {
        printError("gf2xPolyPower: e < 0, NOT IMPLEMENTED YET");

        return;
    }
    if (e == 0) {
        gf2xPolyNull(a);
        a->coef[0] = 1;
        a->deg = 0;
    }
    else if (e == 1 || a->deg < 0) {
        return;
    }
    else {
        BITP_mallocPoly(&tmp, a->deg * e);
        BITP_mallocPoly(&tmp_2, a->deg * e);

        BITP_gf2xPolyCopy(&tmp, a);

        for (i = 1; i < e; i++) {
            BITP_gf2xPolyCopy(&tmp_2, &tmp);

            BITP_gf2xPolyMul(&tmp, &tmp_2, a, a_data);
        }
        BITP_gf2xPolyCopy(a, &tmp);

        BITP_freePoly(&tmp, 0);
        BITP_freePoly(&tmp_2, 0);
    }
}

void BITP_gf2xPolyMulEl(Polynomial_GF2_16x *a, GF2_16x el, const Arithmetic_Data *a_data) {
    int i;

    for (i = a->deg; i >= 0; i--) {
        a->coef[i] = gf2xMulModT(a->coef[i], el, a_data);
    }
    a->deg = BITP_gf2xPolyGetDeg(a);
}

void gf2xPolyMod(Polynomial_GF2_16x *out, const Polynomial_GF2_16x *a, const Polynomial_GF2_16x *mod, const Arithmetic_Data *a_data) {
    int i;
    Polynomial_GF2_16x tmp_out, tmp_mod;
    GF2_16x lead;

    if (mod->deg < 0) {
        return;
    }
    if (out->max_deg < a->deg) {
        BITP_freePoly(out, 0);

        BITP_mallocPoly(out, a->deg);
    }
    else {
        gf2xPolyNull(out);
    }
    // if there is nothing to do
    if (a->deg < mod->deg) {
        BITP_gf2xPolyCopy(out, a);

        return;
    }
    // prepare tmp variables
    BITP_mallocPoly(&tmp_mod, a->deg);
    BITP_mallocPoly(&tmp_out, a->deg);
    BITP_gf2xPolyCopy(&tmp_out, a);

    for (i = a->deg; i >= mod->deg; i--) {
        BITP_gf2xPolyCopy(&tmp_mod, mod);

        lead = gf2xGetPseudoInv(gf2xPolyLeadCoef(mod), tmp_out.coef[i], a_data);
        
        BITP_gf2xPolyMulEl(&tmp_mod, lead, a_data);
        BITP_gf2xPolyShl(&tmp_mod, tmp_out.deg - mod->deg);
        gf2xPolyAdd(out, &tmp_out, &tmp_mod);

        if (i > mod->deg) {
            BITP_gf2xPolyCopy(&tmp_out, out);
        }
    }
    BITP_freePoly(&tmp_mod, 0);
    BITP_freePoly(&tmp_out, 0);
}

void BITP_gf2xMatRootA(Matrix_GF2_16x *out, const Polynomial_GF2_16x *mod, const Arithmetic_Data *a_data) {
    int i, j;
    Polynomial_GF2_16x row, tmp;
    Matrix_GF2_16x bigMat;//, test;//, test_out; // matrix (S | I)

    // create square matrix
    BITP_mallocMatrix(&bigMat, mod->deg, mod->deg * 2);
    BITP_mallocMatrix(out, mod->deg, mod->deg);
    // BITP_mallocMatrix(&test, mod->deg, mod->deg);
    BITP_gf2xMatNull(out);
    BITP_gf2xMatNull(&bigMat);
    BITP_mallocPoly(&tmp, 0);
    
    BITP_mallocPoly(&row, (2 * out->k));

    for (i = 0; i < out->k; i++) {
        gf2xPolyNull(&row);
        row.coef[2*i] = 1;
        row.deg = 2*i;
        // compute line
        gf2xPolyMod(&tmp, &row, mod, a_data);
        // copy elements from polynomial into matrix 
        BITP_gf2xMatInsertPoly(out, &tmp, i);
    }
    BITP_freePoly(&row, 0);

    for (i = 0; i < out->k; i++) {
        for (j = 0; j < out->k; j++) {
            bigMat.elements[i][j] = out->elements[i][j];
        }
        bigMat.elements[i][out->n + i] = 1;
    }

    BITP_gf2xMatGEM(&bigMat, a_data);

    for (i = 0; i < out->k; i++) {
        for (j = 0; j < out->k; j++) {
            // test.elements[i][j] = out->elements[i][j];
            out->elements[i][j] = gf2xRoot(bigMat.elements[i][out->k + j], a_data);
        }
    }
    // BITP_gf2xMatrixMulA(&test_out, &test, out, a_data);
    // BITP_printGf2xMat(&test_out);
    BITP_freeMat(&bigMat, 0);
}

void BITP_gf2xVecMulMat(Vector_GF2_16x *out, const Vector_GF2_16x *x, const Matrix_GF2_16x *mat, const Arithmetic_Data *a_data) {
    int i, j;
    GF2_16x element;
    
    for (i = 0; i < x->len; i++) {
        element = 0;
        for (j = 0; j < mat->n; j++) {
            element = element ^ gf2xMulModT(x->elements[j], mat->elements[j][i], a_data);
        }
        out->elements[i] = element;
    }
}

GF2_16x gf2xRoot(GF2_16x element, const Arithmetic_Data *a_data) {
    GF2_16x sqr;
    GF2_16x sqr_alpha;
    int exponent;
    if (element == 0) {
        return 0;
    }
    exponent = a_data->log_table[element];
    sqr = a_data->exp_table[exponent >> 1];
    if ((exponent & ONE) == ONE) {
        sqr_alpha = a_data->exp_table[(a_data->ord + 1) / 2];
        sqr = gf2xMulModT(sqr, sqr_alpha, a_data);
    }
    return sqr;
}

void BITP_gf2xPolyRoot(Polynomial_GF2_16x *out, const Polynomial_GF2_16x *poly, const Polynomial_GF2_16x *mod, const Arithmetic_Data *a_data) {
    Vector_GF2_16x tmp, tmp_out;
    Matrix_GF2_16x squareMat;
    int i;

    BITP_mallocVectorGF2_16x(&tmp_out, mod->deg);

    if (out->deg < mod->deg) {
        BITP_freePoly(out, 0);
        BITP_mallocPoly(out, mod->deg);
    }
    BITP_gf2xPolyToVecA(&tmp, poly, mod->deg);
    BITP_gf2xMatRootA(&squareMat, mod, a_data);

    // BITP_printGf2xMat(&squareMat);

    for (i = 0; i < tmp.len; i++) {
        tmp.elements[i] = gf2xRoot(tmp.elements[i], a_data);
    }

    BITP_gf2xVecMulMat(&tmp_out, &tmp, &squareMat, a_data);
    gf2xPolyNull(out);
    BITP_gf2xVecToPoly(out, &tmp_out);

    BITP_freeMat(&squareMat, 0);
    BITP_freeVectorGF2_16x(&tmp, 0);
    BITP_freeVectorGF2_16x(&tmp_out, 0);
}