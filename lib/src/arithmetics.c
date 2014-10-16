/**
 This file is part of BitPunch
 Copyright (C) 2014 Frantisek Uhrecky <frantisek.uhrecky[what here]gmail.com>
 Copyright (C) 2014 Andrej Gulyas <andrej.guly[what here]gmail.com>
 Copyright (C) 2014 Marek Klein  <kleinmrk[what here]gmail.com>
 Copyright (C) 2014 Filip Machovec  <filipmachovec[what here]yahoo.com>
 Copyright (C) 2014 Jozef Kudlac <jozef[what here]kudlac.sk>

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

BPU_T_GF2_16x BPU_gf2xMulMod(BPU_T_GF2_16x a, BPU_T_GF2_16x b, BPU_T_GF2_16x mod) {
  BPU_T_GF2_16x tmp, tmp2;

  if (a == 0) {
    return 0;
  }
  // (a/x * b)
  tmp = BPU_gf2xMulMod(a >> 1, b, mod);

  // tmp * x
  tmp = (tmp << 1);

  // tmp + mod
  tmp2 = tmp ^ mod;

  if (tmp2 < tmp) {
    tmp = tmp2;
  }
  if ((a & BPU_ONE) == BPU_ONE) {
    tmp ^= b;
  }
  return tmp;
}

BPU_T_GF2_16x BPU_gf2xMulModT(const BPU_T_GF2_16x a, const BPU_T_GF2_16x b, const BPU_T_Arithmetic_Data *a_data) {
  if (a == 0 || b == 0) {
    return 0;
  }
  // look into tables
  return a_data->exp_table[(a_data->log_table[a] + a_data->log_table[b]) % a_data->ord];
}

BPU_T_GF2_16x BPU_gf2xPowerModT(BPU_T_GF2_16x a, int e, const BPU_T_Arithmetic_Data *a_data) {
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
int BPU_gf2xMatrixMulA(BPU_T_Matrix_GF2_16x *x, const BPU_T_Matrix_GF2_16x *a, const BPU_T_Matrix_GF2_16x *b, const BPU_T_Arithmetic_Data *a_data) {
  uint32_t i, j, k;
  GF2_16x tmp;

  if (a->n != b->k)
    return -1;

  if (BPU_mallocMatrix(x, a->k, b->n) != 0) {
    BPU_printError("BPU_gf2xMatrixMulA: allocation error");

    return -2;
  }
      
  for (i = 0; i < a->k; i++) {
    for (j = 0; j < b->n; j++) {
      tmp = 0;
      for (k = 0; k < a->n; k++) {
        tmp ^= BPU_gf2xMulModT(a->elements[i][k], b->elements[k][j], a_data);
      }
      x->elements[i][j] = tmp;
    }
  }
  return 0;
}
*/

/*** PZ: speedup critical instructions ***/
int BPU_gf2xMatrixMulA(BPU_T_Matrix_GF2_16x *x, const BPU_T_Matrix_GF2_16x *a, const BPU_T_Matrix_GF2_16x *b, const BPU_T_Arithmetic_Data *a_data) {
  uint32_t i, j, k;
  int loga; 

  if (a->n != b->k)
    return -1;

  if (BPU_mallocMatrix(x, a->k, b->n) != 0) {
    BPU_printError("BPU_gf2xMatrixMulA: allocation error");

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


int BPU_gf2VecMulMatA(BPU_T_Vector_GF2 *out, const BPU_T_Vector_GF2 *v, const BPU_T_Matrix_GF2 *b) {
  if (BPU_mallocVectorGF2(out, b->n)) {
    return -2;
  }
  return BPU_gf2VecMulMat(out, v, b);
}

int BPU_gf2VecMulMat(BPU_T_Vector_GF2 *out, const BPU_T_Vector_GF2 *v, const BPU_T_Matrix_GF2 *b) {
  int i, j;

  if ((v->len != b->k) || (out->len != b->n)) {
    BPU_printError("BPU_gf2VecMulMat: wrong vector and matrix dimension v->len = %d, b->k = %d", v->len, b->k);
    BPU_printError("BPU_gf2VecMulMat: wrong vector and matrix dimension out->len = %d, b->n = %d", out->len, b->n);

    return -1;
  }
  // null elements
  BPU_gf2VecNull(out);

  for (i = 0; i < v->len; i++) {
    if (BPU_gf2VecGetBit(v, i)) {
      // xor rows
      for (j = 0; j < out->elements_in_row; j++) {
        out->elements[j] ^= b->elements[i][j];
      }
    }
  }
  return 0;
}

void BPU_gf2MatXorRows(BPU_T_Matrix_GF2 *mat, int i, int j) {
  int k;
  
  for (k = 0; k < mat->elements_in_row; k++) {
    mat->elements[i][k] ^= mat->elements[j][k];
  }
}

int BPU_gf2VecXor(BPU_T_Vector_GF2 *out, const BPU_T_Vector_GF2 *in) {
  int i;

  if (out->elements_in_row != in->elements_in_row) {
    BPU_printError("BPU_gf2VecXor: length error %d != %d", out->elements_in_row, in->elements_in_row);

    return -1;
  }
  for (i = 0; i < out->elements_in_row; i++) {
    out->elements[i] ^= in->elements[i];
  }
  return 0;
}

void BPU_gf2xPolyAdd(BPU_T_Poly_GF2_16x *out, const BPU_T_Poly_GF2_16x *a, const BPU_T_Poly_GF2_16x *b) {
  int16_t out_deg;
  int i = 0;

  out_deg = a->deg > b->deg ? a->deg : b->deg;

  if (out->max_deg < out_deg) {
    BPU_freePoly(out, 0);
    BPU_mallocPoly(out, out_deg);
  }
  else {
    BPU_gf2xPolyNull(out);
  }
  for (i = 0; i <= out_deg; i++) {
    if (i <= a->deg) {
      out->coef[i] ^= a->coef[i];
    }
    if (i <= b->deg) {
      out->coef[i] ^= b->coef[i];
    }
  }
  out->deg = BPU_gf2xPolyGetDeg(out);
}

void BPU_gf2xPolyDiv(BPU_T_Poly_GF2_16x *q, BPU_T_Poly_GF2_16x *r, const BPU_T_Poly_GF2_16x *a, const BPU_T_Poly_GF2_16x *b, const BPU_T_Arithmetic_Data *a_data) {
  // a:b = q+r
  BPU_T_Poly_GF2_16x tmp;
  BPU_T_GF2_16x leader;
  BPU_T_Poly_GF2_16x dividend;
  const BPU_T_Poly_GF2_16x *divider = b;
  int exponent;
  int i;
  int max_deg_q;

  BPU_mallocPoly(&dividend, a->max_deg);
  BPU_gf2xPolyCopy(&dividend, a);
  
  max_deg_q = a->deg - b->deg;

  // check size of outputs
  if (q->max_deg < max_deg_q) {
    BPU_freePoly(q, 0);
    BPU_mallocPoly(q, max_deg_q);
  }
  else {
    // here was mordor #2 - forgotten to null outputs :/
    // null outputs
    BPU_gf2xPolyNull(q);
  }
  if (r->max_deg < (b->max_deg - 1)) {
    BPU_freePoly(r, 0);
    BPU_mallocPoly(r, b->max_deg - 1);
  }
  else {
    BPU_gf2xPolyNull(r);
  }
  BPU_mallocPoly(&tmp, a->max_deg);

  for (i = a->deg; i >= 0; i--) {
    if (dividend.deg < divider->deg) {
      BPU_gf2xPolyCopy(r, &dividend);
      break;
    }
    BPU_gf2xPolyNull(&tmp);
    leader = BPU_gf2xMulModT(dividend.coef[i], BPU_gf2xPowerModT(divider->coef[divider->deg], -1, a_data), a_data);
    exponent = dividend.deg - divider->deg;
    q->coef[exponent] = leader;

    if(q->deg == -1) {
      q->deg = BPU_gf2xPolyGetDeg(q);            
    }
    BPU_gf2xPolyMul(&tmp, divider, q, a_data);

    BPU_gf2xPolyAdd(&dividend, a, &tmp);
  }
  BPU_freePoly(&dividend, 0);
  BPU_freePoly(&tmp, 0);
}

void BPU_gf2xPolyMul(BPU_T_Poly_GF2_16x *out, const BPU_T_Poly_GF2_16x *a, const BPU_T_Poly_GF2_16x *b, const BPU_T_Arithmetic_Data *a_data) {
  int i;
  int j;
  int max_deg = a->deg + b->deg;
  
  // MAYBE HERE WAS #### mordor #### - it does
  // if there is not enough space, resize it
  if (out->max_deg < max_deg) {
    BPU_freePoly(out, 0);
    BPU_mallocPoly(out, max_deg);
  }
  else {
    BPU_gf2xPolyNull(out);
  }

  for (i = a->deg; i >= 0; i--) {
    for (j = b->deg; j >= 0; j--) {
      out->coef[i+j] ^= BPU_gf2xMulModT(a->coef[i], b->coef[j], a_data);
    }
  }
  out->deg = BPU_gf2xPolyGetDeg(out);
}

void BPU_gf2xPolyShr(BPU_T_Poly_GF2_16x *a, int n) {
  BPU_T_Poly_GF2_16x tmp;

  // if there is nothing to shift, return
  if (a->deg == -1 || n <= 0) {
    return;
  }
  BPU_mallocPoly(&tmp, a->deg);
  BPU_gf2xPolyCopy(&tmp, a);
  BPU_gf2xPolyNull(a);

  if (n < tmp.deg + 1) {
    memcpy((void *)(a->coef), (void *)(tmp.coef + n), (tmp.deg + 1 - n) * sizeof(BPU_T_GF2_16x));

    a->deg = BPU_gf2xPolyGetDeg(a);
  }
  BPU_freePoly(&tmp, 0);
}

void BPU_gf2xPolyShl(BPU_T_Poly_GF2_16x *a, int n) {
  BPU_T_Poly_GF2_16x tmp;

  // if there is nothing to shift, return
  if (a->deg == -1 || n <= 0) {
    return;
  }
  BPU_mallocPoly(&tmp, a->deg);
  BPU_gf2xPolyCopy(&tmp, a);

  if (a->max_deg < a->deg + n) {
    BPU_freePoly(a, 0);

    BPU_mallocPoly(a, a->deg + n);
  }
  else {
    BPU_gf2xPolyNull(a);
  }
  memcpy((void *)(a->coef + n), (void *)tmp.coef, (tmp.deg + 1) * sizeof(BPU_T_GF2_16x));
  a->deg = BPU_gf2xPolyGetDeg(a);

  BPU_freePoly(&tmp, 0);
}

void BPU_gf2xPolyPower(BPU_T_Poly_GF2_16x *a, int e, const BPU_T_Arithmetic_Data *a_data) {
  int i;
  BPU_T_Poly_GF2_16x tmp, tmp_2;

  if (e < 0) {
    BPU_printError("gf2xPolyPower: e < 0, NOT IMPLEMENTED YET");

    return;
  }
  if (e == 0) {
    BPU_gf2xPolyNull(a);
    a->coef[0] = 1;
    a->deg = 0;
  }
  else if (e == 1 || a->deg < 0) {
    return;
  }
  else {
    BPU_mallocPoly(&tmp, a->deg * e);
    BPU_mallocPoly(&tmp_2, a->deg * e);

    BPU_gf2xPolyCopy(&tmp, a);

    for (i = 1; i < e; i++) {
      BPU_gf2xPolyCopy(&tmp_2, &tmp);

      BPU_gf2xPolyMul(&tmp, &tmp_2, a, a_data);
    }
    BPU_gf2xPolyCopy(a, &tmp);

    BPU_freePoly(&tmp, 0);
    BPU_freePoly(&tmp_2, 0);
  }
}

void BPU_gf2xPolyMulEl(BPU_T_Poly_GF2_16x *a, BPU_T_GF2_16x el, const BPU_T_Arithmetic_Data *a_data) {
  int i;

  for (i = a->deg; i >= 0; i--) {
    a->coef[i] = BPU_gf2xMulModT(a->coef[i], el, a_data);
  }
  a->deg = BPU_gf2xPolyGetDeg(a);
}

void BPU_gf2xPolyMod(BPU_T_Poly_GF2_16x *out, const BPU_T_Poly_GF2_16x *a, const BPU_T_Poly_GF2_16x *mod, const BPU_T_Arithmetic_Data *a_data) {
  int i;
  BPU_T_Poly_GF2_16x tmp_out, tmp_mod;
  BPU_T_GF2_16x lead;

  if (mod->deg < 0) {
    return;
  }
  if (out->max_deg < a->deg) {
    BPU_freePoly(out, 0);

    BPU_mallocPoly(out, a->deg);
  }
  else {
    BPU_gf2xPolyNull(out);
  }
  // if there is nothing to do
  if (a->deg < mod->deg) {
    BPU_gf2xPolyCopy(out, a);

    return;
  }
  // prepare tmp variables
  BPU_mallocPoly(&tmp_mod, a->deg);
  BPU_mallocPoly(&tmp_out, a->deg);
  BPU_gf2xPolyCopy(&tmp_out, a);

  for (i = a->deg; i >= mod->deg; i--) {
    BPU_gf2xPolyCopy(&tmp_mod, mod);

    lead = BPU_gf2xGetPseudoInv(BPU_gf2xPolyLeadCoef(mod), tmp_out.coef[i], a_data);
    
    BPU_gf2xPolyMulEl(&tmp_mod, lead, a_data);
    BPU_gf2xPolyShl(&tmp_mod, tmp_out.deg - mod->deg);
    BPU_gf2xPolyAdd(out, &tmp_out, &tmp_mod);

    if (i > mod->deg) {
      BPU_gf2xPolyCopy(&tmp_out, out);
    }
  }
  BPU_freePoly(&tmp_mod, 0);
  BPU_freePoly(&tmp_out, 0);
}

void BPU_gf2xMatRootA(BPU_T_Matrix_GF2_16x *out, const BPU_T_Poly_GF2_16x *mod, const BPU_T_Arithmetic_Data *a_data) {
  int i, j;
  BPU_T_Poly_GF2_16x row, tmp;
  BPU_T_Matrix_GF2_16x bigMat;//, test;//, test_out; // matrix (S | I)

  // create square matrix
  BPU_mallocMatrix(&bigMat, mod->deg, mod->deg * 2);
  BPU_mallocMatrix(out, mod->deg, mod->deg);
  // BPU_mallocMatrix(&test, mod->deg, mod->deg);
  BPU_gf2xMatNull(out);
  BPU_gf2xMatNull(&bigMat);
  BPU_mallocPoly(&tmp, 0);
  
  BPU_mallocPoly(&row, (2 * out->k));

  for (i = 0; i < out->k; i++) {
    BPU_gf2xPolyNull(&row);
    row.coef[2*i] = 1;
    row.deg = 2*i;
    // compute line
    BPU_gf2xPolyMod(&tmp, &row, mod, a_data);
    // copy elements from polynomial into matrix 
    BPU_gf2xMatInsertPoly(out, &tmp, i);
  }
  BPU_freePoly(&row, 0);

  for (i = 0; i < out->k; i++) {
    for (j = 0; j < out->k; j++) {
      bigMat.elements[i][j] = out->elements[i][j];
    }
    bigMat.elements[i][out->n + i] = 1;
  }

  BPU_gf2xMatGEM(&bigMat, a_data);

  for (i = 0; i < out->k; i++) {
    for (j = 0; j < out->k; j++) {
      // test.elements[i][j] = out->elements[i][j];
      out->elements[i][j] = BPU_gf2xRoot(bigMat.elements[i][out->k + j], a_data);
    }
  }
  // BPU_gf2xMatrixMulA(&test_out, &test, out, a_data);
  // BPU_printGf2xMat(&test_out);
  BPU_freeMat(&bigMat, 0);
}

void BPU_gf2xVecMulMat(BPU_T_Vector_GF2_16x *out, const BPU_T_Vector_GF2_16x *x, const BPU_T_Matrix_GF2_16x *mat, const BPU_T_Arithmetic_Data *a_data) {
  int i, j;
  BPU_T_GF2_16x element;
  
  for (i = 0; i < x->len; i++) {
    element = 0;
    for (j = 0; j < mat->n; j++) {
      element = element ^ BPU_gf2xMulModT(x->elements[j], mat->elements[j][i], a_data);
    }
    out->elements[i] = element;
  }
}

BPU_T_GF2_16x BPU_gf2xRoot(BPU_T_GF2_16x element, const BPU_T_Arithmetic_Data *a_data) {
  BPU_T_GF2_16x sqr;
  BPU_T_GF2_16x sqr_alpha;
  int exponent;
  if (element == 0) {
    return 0;
  }
  exponent = a_data->log_table[element];
  sqr = a_data->exp_table[exponent >> 1];
  if ((exponent & BPU_ONE) == BPU_ONE) {
    sqr_alpha = a_data->exp_table[(a_data->ord + 1) / 2];
    sqr = BPU_gf2xMulModT(sqr, sqr_alpha, a_data);
  }
  return sqr;
}

void BPU_gf2xPolyRoot(BPU_T_Poly_GF2_16x *out, const BPU_T_Poly_GF2_16x *poly, const BPU_T_Poly_GF2_16x *mod, const BPU_T_Arithmetic_Data *a_data) {
  BPU_T_Vector_GF2_16x tmp, tmp_out;
  BPU_T_Matrix_GF2_16x squareMat;
  int i;

  BPU_mallocVectorGF2_16x(&tmp_out, mod->deg);

  if (out->deg < mod->deg) {
    BPU_freePoly(out, 0);
    BPU_mallocPoly(out, mod->deg);
  }
  BPU_gf2xPolyToVecA(&tmp, poly, mod->deg);
  BPU_gf2xMatRootA(&squareMat, mod, a_data);

  // BPU_printGf2xMat(&squareMat);

  for (i = 0; i < tmp.len; i++) {
    tmp.elements[i] = BPU_gf2xRoot(tmp.elements[i], a_data);
  }

  BPU_gf2xVecMulMat(&tmp_out, &tmp, &squareMat, a_data);
  BPU_gf2xPolyNull(out);
  BPU_gf2xVecToPoly(out, &tmp_out);

  BPU_freeMat(&squareMat, 0);
  BPU_freeVectorGF2_16x(&tmp, 0);
  BPU_freeVectorGF2_16x(&tmp_out, 0);
}