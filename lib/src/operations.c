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
#include <string.h>
#include <math.h>
#include <time.h>
// TODO: remove dependency
#include <openssl/sha.h>

#include "arithmetics.h"
#include "globals.h"
#include "debugio.h"
#include "operations.h"
#include "init.h"

int BPU_gf2xGetDeg(BPU_T_GF2_64x poly) {
  int i = 63;

  while (i >= 0) {
    if (BPU_getBit(poly, i)) {
      return i;
    }
    i--;
  }
  return -1;
}

int BPU_gf2xPolyGetDeg(BPU_T_Poly_GF2_16x *poly) {
  int i = poly->max_deg;

  while (i >= 0) {
    if (poly->coef[i] != 0) {
      return i;
    }
    i--;
  }
  return -1;
}

int BPU_gf2VecHashA(BPU_T_Vector_GF2 *out, const BPU_T_Vector_GF2 *in, int len){
//sha512 output constant - SHA512_DIGEST_LENGTH
  const int DIGEST_LENGTH = 512;
  int i, element = -1;
  unsigned char* hashed = (unsigned char*) malloc(DIGEST_LENGTH/8);
  BPU_T_Vector_GF2 hash_vector;

  // TODO: remove dependency
  if(SHA512((unsigned char*)in->elements, in->elements_in_row, hashed) == NULL){
    BPU_printError("hash: SHA512");
    return -1;
  }

  if (BPU_mallocVectorGF2(&hash_vector, DIGEST_LENGTH) != 0) {
    BPU_printError("hash: BPU_mallocVectorGF2");
    return -2;
  }

  for (i = 0; i < hash_vector.len/8; i++) {
    if (i % (hash_vector.element_bit_size/8) == 0)
      element++;
    hash_vector.elements[element] ^= hashed[i] << ((i % (hash_vector.element_bit_size/8))*8);
  }
  BPU_gf2VecCropA(out, &hash_vector, 0, len);

  BPU_freeVecGF2(&hash_vector, 0);
  free(hashed);

  return 0;
}

int BPU_gf2xMatPermute(BPU_T_Matrix_GF2_16x *out, const BPU_T_Matrix_GF2_16x *m, const BPU_T_Perm_Vector *permutation) {
  int i, j;

  // check if the size is correct
  if (m->n != permutation->size) {
    BPU_printError("BPU_gf2MatPermute: permutation size not correct m->n = %d, p->size = %d", m->n, permutation->size);

    return -1;
  }
  // permute
  for (j = 0; j < m->n; j++) { // column loop
    for (i = 0; i < m->k; i++) { // row loop
      out->elements[i][j] = m->elements[i][permutation->elements[j]]; // permute the columns
    }
  }
  return 0;
}

int BPU_gf2MatCopy(BPU_T_Matrix_GF2 *out, const BPU_T_Matrix_GF2 *in) {
  int i, j;

  if (out->k != in->k || out->n != in->n) {
    BPU_printError("BPU_gf2MatCopy: wrong matrix size");

    return -1;
  }

  // copy the matrix
  for (i = 0; i < in->k; i++) {
    for (j = 0; j < in->elements_in_row; j++) {
      out->elements[i][j] = in->elements[i][j];
    }
  }
  return 0;
}

int BPU_gf2MatPermute(BPU_T_Matrix_GF2 *m, const BPU_T_Perm_Vector *permutation) {
  int i, j, act_element, shift;
  BPU_T_Matrix_GF2 new_matrix;

  // check if the size is correct
  if (m->n != permutation->size)
    return -1;

  // allocate new matrix
  if (BPU_mallocMatrixGF2(&new_matrix, m->k ,m->n) != 0)
    return -2;

  // copy the matrix
  BPU_gf2MatCopy(&new_matrix, m);

  // erase the matrix
  for (i = 0; i < m->k; i++) {
    for (j = 0; j < m->elements_in_row; j++) {
      m->elements[i][j] = 0;
    }
  }

  // permute
  act_element = 0;
  for (j = 0; j < m->n; j++) { // column loop
    if (j >= m->element_bit_size + act_element * m->element_bit_size)
      act_element++;
    for (i = 0; i < m->k; i++) { // row loop
      if ((int)(permutation->elements[j]-j) >= 0) {
        shift = (permutation->elements[j]-j) % new_matrix.element_bit_size;
        m->elements[i][act_element] ^= BPU_gf2MatGetMaskedBit(&new_matrix, (uint64_t)i, (uint64_t)permutation->elements[j]) >> (shift); // permute the columns
      }
      else {
        shift = (j-permutation->elements[j]) % new_matrix.element_bit_size;
        m->elements[i][act_element] ^= BPU_gf2MatGetMaskedBit(&new_matrix, (uint64_t)i, (uint64_t)permutation->elements[j]) << (shift); // permute the columns
      }
    }
  }
  BPU_freeMatGF2(&new_matrix, 0);

  return 0;
}

int BPU_gf2VecPermute(BPU_T_Vector_GF2 *vec, const BPU_T_Perm_Vector *permutation) {
  int i;
  BPU_T_Vector_GF2 tmp;

  BPU_mallocVectorGF2(&tmp, vec->len);

  for (i = 0; i < permutation->size; i++) {
    BPU_gf2VecSetBit(&tmp, i, BPU_gf2VecGetBit(vec, permutation->elements[i]));
  }
  BPU_gf2VecCopy(vec, &tmp);

  BPU_freeVecGF2(&tmp, 0);

  return 0;
}

BPU_T_GF2 BPU_gf2MatGetMaskedBit(const BPU_T_Matrix_GF2 *m, uint32_t row, uint32_t bit) {
  int segment, bit_in_segment;

  segment = bit / (m->element_bit_size);
  bit_in_segment = bit % (m->element_bit_size);
  
  return m->elements[row][segment] & ((uint64_t)1 << bit_in_segment);
}

BPU_T_GF2 BPU_gf2VecGetMaskedBit(const BPU_T_Vector_GF2 *vec, uint32_t bit) {
  int segment, bit_in_segment;

  segment = bit / (vec->element_bit_size);
  bit_in_segment = bit % (vec->element_bit_size);
  
  return vec->elements[segment] & ((uint64_t)1 << bit_in_segment);
}

int BPU_permGenA(BPU_T_Perm_Vector* permutation, uint32_t size) {
  int i, j, tested;
  uint32_t rand_value;

  if (BPU_mallocPerm(permutation, size)) {
    return -1;
  }
  // // TEST: NSA
  // return 0;
  for (i = 0; i < size; i++) {
    while (1) {
      rand_value = BPU_getRandValue(0,size);
      tested = 1;
      for (j = 0; j < i; j++) {
        if (permutation->elements[j] == rand_value){
          tested = 0;
          break;
        }
      }
      if (tested == 1){
        permutation->elements[i] = rand_value;
        break;
      }
    }
  }
  return 0;
}

int BPU_gf2xMatConvertToGf2MatA(BPU_T_Matrix_GF2 *out, const BPU_T_Matrix_GF2_16x *m, int element_bit_size) {
  int i, j, bit, bit_in_element = -1, act_element = 0;

  // allocate memory for new matrix
  if (BPU_mallocMatrixGF2(out, m->k * element_bit_size, m->n)!= 0)
    return -1;

  // converting
  for (j = 0; j < m->n; j++) { // column loop
    // check if there is shift through elements
    if ((j - act_element * out->element_bit_size) >= out->element_bit_size) { // next elemenet, first bit
      act_element++;
      bit_in_element = 0;
    }
    else // same element, next bit
      bit_in_element++;
    for (i = 0; i < m->k; i++) { // row loop
      for (bit = 0; bit < element_bit_size; bit++) { // bit loop through element of matrix
        out->elements[i*element_bit_size + bit][act_element] ^= (uint64_t)BPU_getBit(m->elements[i][j], bit) << (bit_in_element); // get bit from element and shift it
      }
    }
  }
  return 0;
}


int BPU_permGetInvA(BPU_T_Perm_Vector *out, const BPU_T_Perm_Vector *in) {
  int i;

  // allocate memory
  if (BPU_mallocPerm(out, in->size)) {
    BPU_printError("inverted permutation malloc error");

    return -1;
  }
  for (i = 0; i < in->size; i++) {
    out->elements[in->elements[i]] = i;
  }
  return 0;
}

int BPU_gf2MatTranspA(BPU_T_Matrix_GF2 *out, const BPU_T_Matrix_GF2 *in) {
  int i, j;

  if (BPU_mallocMatrixGF2(out, in->n, in->k)) {
    BPU_printError("BPU_gf2MatTranspA allocation error");

    return -1;
  }
  for (i = 0; i < in->k; i++) {
    for (j = 0; j < in->n; j++) {
      BPU_gf2MatSetBit(out, j, i, BPU_gf2MatGetBit(in, i, j));
    } // col loop
  } // row loop

  return 0;
}

void BPU_gf2Swap(BPU_T_GF2 *a, BPU_T_GF2 *b) {
  BPU_T_GF2 tmp;
  tmp = *a;
  *a = *b;
  *b = tmp;
}

void BPU_gf2MatSwapRows(BPU_T_Matrix_GF2 *mat, int i, int j) {
  int k;
  for (k = 0; k < mat->elements_in_row; k++) {
    BPU_gf2Swap(&(mat->elements[i][k]), &(mat->elements[j][k]));
  }
}



int BPU_gf2MatFindRow(const BPU_T_Matrix_GF2 *mat, int i, int start_index) {
  int k;
  for (k = start_index; k < mat->k; k++) {
    if ( BPU_gf2MatGetMaskedBit(mat, k, i) ){
      return k;
    }
  }
  return -1;
}

int BPU_gf2MatFindCol(const BPU_T_Matrix_GF2 *mat, int i, int start_index) {
  int k;
  for (k = start_index; k < mat->n; k++) {
    if ( BPU_gf2MatGetMaskedBit(mat, i, k) ){
      return k;
    }
  }
  return -1;
}

BPU_T_Perm_Vector* BPU_gf2MatMakeSystematicA(BPU_T_Matrix_GF2 *inout) {
  int act_position = 0;
  int i;
  int row, col;
  BPU_T_Perm_Vector perm;
  BPU_T_Perm_Vector *permOut;
  permOut = (BPU_T_Perm_Vector*) malloc (sizeof(BPU_T_Perm_Vector));

  BPU_mallocPerm(&perm, inout->n);
  BPU_mallocPerm(permOut, inout->n);

  for (i = 0; i < perm.size; i++) {
    perm.elements[i] = i;
    permOut->elements[i] = i;
  }
#ifdef BPU_TEST_DET_EQ_CODE
  __test_was_eq_code = 0;
#endif
  
  for (act_position = 0; act_position < inout->k; act_position++) {
    row = BPU_gf2MatFindRow(inout, act_position, act_position);
    if (row == -1){
      BPU_printWarning("Not systematic");
      // TODO: find bug in this if branch
      #ifdef BPU_TEST_DET_EQ_CODE
        __test_was_eq_code = 1;
      #endif
        BPU_freePerm(permOut, 1);
        BPU_freePerm(&perm, 0);

        return NULL;

      row = act_position;
      col = BPU_gf2MatFindCol(inout, act_position, act_position);
      if (col == -1) {
        BPU_printError("unable to create systematic form");
        
        exit(-1);
      }
      BPU_gf2Swap(&(perm.elements[act_position]), &(perm.elements[col])); 
      // printf("swap col: %d\t%d\n", act_position, col);
      BPU_permPermute(permOut, &perm);
      // BPU_printPerm(permOut);
      BPU_gf2MatPermute(inout, &perm);
      BPU_gf2Swap(&(perm.elements[act_position]), &(perm.elements[col])); // reset temporary permutation
    }
    BPU_gf2MatSwapRows(inout, act_position, row);

    // xor with the rest of rows if needed
    for (i = 0; i < inout->k; i++) {
      if ( BPU_gf2MatGetMaskedBit(inout, i, act_position) && act_position != i) {
        BPU_gf2MatXorRows(inout, i, act_position);
      }
    }
  }
  BPU_freePerm(&perm, 0);
  return permOut;
}

int BPU_permPermute(BPU_T_Perm_Vector *to_permute, const BPU_T_Perm_Vector *permutation) {

  int i;
  BPU_T_Perm_Vector new_permutation;
  // check if the size is correct
  if (to_permute->size != permutation->size)
    return -1;

  // allocate new permutation
  if (BPU_mallocPerm(&new_permutation, to_permute->size) != 0)
    return -2;

  // copy the permutation
  for (i = 0; i < to_permute->size; i++) {
    new_permutation.elements[i] = to_permute->elements[i];
  }

  // permute
  for (i = 0; i < permutation->size; i++) { // row loop
    to_permute->elements[i] = new_permutation.elements[permutation->elements[i]];
  }

  BPU_freePerm(&new_permutation, 0);

  return 0;
}

int BPU_gf2MatAppendIdenityA(BPU_T_Matrix_GF2 *out, const BPU_T_Matrix_GF2 *in) {
  int i, j;

  if (BPU_mallocMatrixGF2(out, in->k, in->n + in->k)) {
    BPU_printError("gf2MatAppendIdenityA: allocation error");

    return -1;
  }
  // first copy old rows
  for (i = 0; i < in->k; i++) {
    memcpy((void *) out->elements[i], (void *)in->elements[i], in->elements_in_row * (in->element_bit_size / 8));
  } // row loop
  // now append identity matrix
  for (i = 0; i < out->k; i++) {
      for (j = in->n; j < out->n; j++) {
        BPU_gf2MatSetBit(out, i, j, i == (j - in->n) ? 1 : 0);
    } // col loop
  } // row loop
  return 0;
}

int BPU_gf2VecConcat(BPU_T_Vector_GF2 *out, const BPU_T_Vector_GF2 *vec1, const BPU_T_Vector_GF2 *vec2) {
  int len = vec1->len + vec2->len;
  int i;

  if (out->len != len) {
    BPU_freeVecGF2(out, 0);
    BPU_mallocVectorGF2(out, vec1->len + vec2->len);
  }
  else {
    BPU_gf2VecNull(out);
  }
  // copy first vector
  BPU_gf2VecCopy(out, vec1);

  // copy second vector
  for (i = 0; i < vec2->len; i++) {
    BPU_gf2VecSetBit(out, i + vec1->len, BPU_gf2VecGetBit(vec2, i));
  }
  out->len = len;

  return 0;
}

int BPU_gf2VecConcatA(BPU_T_Vector_GF2 *out, const BPU_T_Vector_GF2 *vec1, const BPU_T_Vector_GF2 *vec2) {
  int len = vec1->len + vec2->len;

  if (BPU_mallocVectorGF2(out, len)) {
    BPU_printError("BPU_gf2VecConcat: allocation error");

    return -1;
  }
  return BPU_gf2VecConcat(out, vec1, vec2);
}

int BPU_gf2VecCropA(BPU_T_Vector_GF2 *out, const BPU_T_Vector_GF2 *in, const int start, const int length) {

  //input test
  if (start < 0 || (length+start) > in->len) {
    BPU_printError("cropVector: bad params");
    return -1;
  }

  //malloc output vector
  if (BPU_mallocVectorGF2(out, length)) {
    BPU_printError("cropVector: allocation error");
    return -1;
  }

  int i;

  //calc the shifts
  /*int crop_start = (start % in->element_bit_size);
  int crop_len = (length % in->element_bit_size);
  int shift1 = in->element_bit_size - crop_start - crop_len;
  int shift2 = in->element_bit_size - crop_len;
  int shift3 = in->element_bit_size - crop_start;
 
  //start elements   
  int start_element = start / in->element_bit_size;*/

  //element loop
  /*for(i = 0; i < out->elements_in_row; i++) {
    //crop from only one element
    if (((start / in->element_bit_size) == ((start + length) / in->element_bit_size)) || (i == out->elements_in_row-1 && crop_start == 0)) {
      printf("first way: \n");
      out->elements[i] = (in->elements[i+start_element] << shift1) >> shift2;
    }
    //concat of elements
    else { 
      printf("second way: \n");
      out->elements[i] = (in->elements[i-start_element] >> crop_start) ^ ((in->elements[i+start_element+1]) << shift1 >> (shift1 - shift3));
    }
  }*/
  int counter = 0;
  for (i = start; i < start+length; i++) {
    BPU_gf2VecSetBit(out, counter, BPU_gf2VecGetBit(in, i));
    counter++;
  }

  return 0;
}

int BPU_gf2MatCropA(BPU_T_Matrix_GF2 *out, const BPU_T_Matrix_GF2 *in, uint16_t width) {
  int i, j, startElement;
  uint8_t cropBits;

  // if is new width lower than old width of matrix
  if (in->n >= width) {
    // allocate memory for new matrix
    if (BPU_mallocMatrixGF2(out, in->k, width)!= 0) {
      return -2;
    }

    startElement = (in->n - width) / in->element_bit_size;
    cropBits = (in->n - width) % in->element_bit_size;

    // fill new matrix
    for (i = 0; i < out->k; i++) { // row loop
      for (j = 0; j < out->elements_in_row; j++) { // column loop
        
        // if there is no need to make shift through elements
        if (j+startElement >= in->elements_in_row-1)
          out->elements[i][j] = (in->elements[i][j+startElement] >> cropBits);
        // with shift through elements
        else 
          out->elements[i][j] = (in->elements[i][j+startElement] >> cropBits) ^ (in->elements[i][j+startElement+1] << (in->element_bit_size - cropBits));
      }
    }
    return 0;

  }
  // else error
  else {
    BPU_printError("BPU_gf2MatCropA in->n <= width, (%d <= %d)", in->n, width);

    return -1;
  }
}

int BPU_gf2MatGetRowAsGf2Vec(BPU_T_Vector_GF2 *out, const BPU_T_Matrix_GF2 *in, int row, int alloc) {
  if (!alloc && out->len != in->n) {
    BPU_printError("gf2MatGetRowAsGf2Vec: dimension is wrong out->len %d != in->n %d", out->len, in->n);

    return -1;
  }
  if (alloc) {
    if (BPU_mallocVectorGF2(out, in->n)) {
      BPU_printError("gf2MatGetRowAsGf2Vec: can not allocate vector");

      return -2;
    }
  }
  BPU_gf2MatCopyRowToVec(out, in, row);

  return 0;
}

int BPU_gf2xPolyExtEuclidA(BPU_T_Poly_GF2_16x *d, BPU_T_Poly_GF2_16x *s, BPU_T_Poly_GF2_16x *t, const BPU_T_Poly_GF2_16x *a, const BPU_T_Poly_GF2_16x *b, const int end_deg, const BPU_T_Arithmetic_Data *a_data) {
  BPU_T_Poly_GF2_16x tmp, tmp_2, old_s, old_t, old_r, r, q;
  BPU_T_GF2_16x inv_lead;
  int deg;

  deg = (a->deg > b->deg) ? a->deg : b->deg;
  
  // allocate GCD qoutient
  BPU_mallocPoly(s, deg);
  BPU_mallocPoly(t, deg);
  BPU_mallocPoly(d, deg);

  BPU_mallocPoly(&tmp, deg);
  BPU_mallocPoly(&tmp_2, deg);
  BPU_mallocPoly(&old_s, deg);
  BPU_mallocPoly(&old_t, deg);
  BPU_mallocPoly(&old_r, deg);
  BPU_mallocPoly(&r, deg);
  BPU_mallocPoly(&q, deg);

  BPU_gf2xPolyCopy(&r, b);
  BPU_gf2xPolyCopy(&old_r, a);

  if (a->deg == -1) {
    BPU_gf2xPolyCopy(&old_r, b);
    old_t.coef[0] = 1;
    old_t.deg = 0;
  }
  else if (b->deg == -1) {
    BPU_gf2xPolyCopy(&old_r, a);
    old_s.coef[0] = 1;
    old_s.deg = 0;
  }
  else {
    old_s.coef[0] = 1;
    old_s.deg = 0;

    t->coef[0] = 1;
    t->deg = 0;

    while (old_r.deg > end_deg && r.deg > -1) {   
      BPU_gf2xPolyDiv(&q, &tmp, &old_r, &r, a_data);

      // save old reminder
      BPU_gf2xPolyCopy(&old_r, &r);
      // save current reminder
      BPU_gf2xPolyCopy(&r, &tmp);

      // save s quocient
      BPU_gf2xPolyCopy(&tmp, &old_s);
      BPU_gf2xPolyCopy(&old_s, s);
      BPU_gf2xPolyMul(&tmp_2, &q, s, a_data);
      BPU_gf2xPolyAdd(s, &tmp, &tmp_2);

      // save t quocient
      BPU_gf2xPolyCopy(&tmp, &old_t);
      BPU_gf2xPolyCopy(&old_t, t);
      BPU_gf2xPolyMul(&tmp_2, &q, t, a_data);
      BPU_gf2xPolyAdd(t, &tmp, &tmp_2);
    }
  } 
  // prepare return values
  BPU_gf2xPolyCopy(d, &old_r);
  BPU_gf2xPolyCopy(s, &old_s);
  BPU_gf2xPolyCopy(t, &old_t);

  // make monic, if it is not
  inv_lead = BPU_gf2xPolyMakeMonic(d, a_data);

  if (inv_lead != 0) {
    BPU_gf2xPolyMulEl(s, inv_lead, a_data);
    BPU_gf2xPolyMulEl(t, inv_lead, a_data);
  }
  BPU_freePoly(&tmp, 0);
  BPU_freePoly(&tmp_2, 0);
  BPU_freePoly(&old_s, 0);
  BPU_freePoly(&old_t, 0);
  BPU_freePoly(&old_r, 0);
  BPU_freePoly(&r, 0);
  BPU_freePoly(&q, 0);

  return 0;
}

BPU_T_GF2_16x BPU_gf2xPolyEval(const BPU_T_Poly_GF2_16x *poly, const BPU_T_GF2_16x x, const BPU_T_Arithmetic_Data *a_data) {
  int i;
  BPU_T_GF2_16x ret = 0;
  ret = poly->coef[0];

  for (i = 1; i <= poly->deg; i++) {
    ret = ret ^ BPU_gf2xMulModT(poly->coef[i], BPU_gf2xPowerModT(x, i, a_data), a_data);
  }
  return ret;
}

uint32_t BPU_getRandValue(int from, int to) {
  if (from <= to){
    return rand()%(to-from)+from;
  }
  return 0;
}


int BPU_gf2xPolyCmp(const BPU_T_Poly_GF2_16x *p1, const BPU_T_Poly_GF2_16x *p2) {
  int i;    
  if (p1->deg != p2->deg) {
    return 0;
  }
  for (i = 0; i <= p1->deg; i++) {
    if (p1->coef[i] != p2->coef[i]) {
      return 0;
    }    
  }
  return 1;
}

int BPU_gf2xPolyIrredTest(const BPU_T_Poly_GF2_16x *p, const BPU_T_Arithmetic_Data *a_data) {
  // x^(q^n) = x^((2^m)^n) = x^(2^(m*n))
  // q = 2^m
  // check if gcd(x^(q^n) - x, f)==f. if not, f is reducible
  // find all prime factors of n = deg(f)
  // for every prime factor t of n check if gcd(x^(q^(n/t)), f) == 1. if not f is reducible
  // otherwise f is ireducible
  int i, j;
  int is_irred = 1;
  int m = a_data->mod_deg;
  int n = p->deg;
  int exponent = m*n;
  BPU_T_Poly_GF2_16x tmp, out, qr, x, gcd, s, t, one;

  // test if some alpha is root
  for (i = 0; i < a_data->ord; i++) {
    if (BPU_gf2xPolyEval(p, a_data->exp_table[i], a_data) == 0) {
      is_irred = 0;
      return is_irred;
    }
  }
  BPU_mallocPoly(&out, 0);
  BPU_mallocPoly(&one, 0);
  BPU_mallocPoly(&qr, 0);
  BPU_mallocPoly(&tmp, 1);
  BPU_mallocPoly(&x, 1);
  // gcd, s, t, will be allocated inside gf2xPolyExtEuclidA

  // set tmp polynomial tmp(x) = x
  tmp.coef[0] = 0;
  tmp.coef[1] = 1;
  tmp.deg = BPU_gf2xPolyGetDeg(&tmp);

  x.coef[0] = 0;
  x.coef[1] = 1;
  x.deg = BPU_gf2xPolyGetDeg(&x);

  one.coef[0] = 1;
  one.deg = 0;

  // simplify polynomial with big coeffitient
  for (i = 0; i < exponent; i++) {
    BPU_gf2xPolyMul(&out, &tmp, &tmp, a_data);
    BPU_gf2xPolyMod(&tmp, &out, p, a_data);
  }
  BPU_gf2xPolyAdd(&qr, &tmp, &x);
  BPU_gf2xPolyExtEuclidA(&gcd, &s, &t, &qr, p, -1, a_data);

  if (BPU_gf2xPolyCmp(p, &gcd) != 1) {
    BPU_freePoly(&out, 0);
    BPU_freePoly(&one, 0);
    BPU_freePoly(&qr, 0);
    BPU_freePoly(&tmp, 0);
    BPU_freePoly(&x, 0);
    BPU_freePoly(&gcd, 0);
    BPU_freePoly(&s, 0);
    BPU_freePoly(&t, 0);

    is_irred = 0;
    return is_irred;
  }

  for (j = 2; j <= p->deg; j++) {
    if (p->deg % j != 0 || BPU_isPrime(j) == 0)
      continue;

    BPU_gf2xPolyNull(&tmp);
    tmp.coef[0] = 0;
    tmp.coef[1] = 1;
    tmp.deg = BPU_gf2xPolyGetDeg(&tmp);
    
    exponent = m*(n/j);

    for (i = 0; i < exponent; i++) {
      BPU_gf2xPolyMul(&out, &tmp, &tmp, a_data);
      BPU_gf2xPolyMod(&tmp, &out, p, a_data);
    }
    BPU_freePoly(&gcd, 0);
    BPU_freePoly(&s, 0);
    BPU_freePoly(&t, 0);

    BPU_gf2xPolyExtEuclidA(&gcd, &s, &t, &tmp, p, -1, a_data);

    if (BPU_gf2xPolyCmp(&one, &gcd) != 1) {
      is_irred = 0;
      break;
    }
  }
  BPU_freePoly(&out, 0);
  BPU_freePoly(&one, 0);
  BPU_freePoly(&qr, 0);
  BPU_freePoly(&tmp, 0);
  BPU_freePoly(&x, 0);
  BPU_freePoly(&gcd, 0);
  BPU_freePoly(&s, 0);
  BPU_freePoly(&t, 0);

  return is_irred;
}

void BPU_gf2xPolyCopy(BPU_T_Poly_GF2_16x *dest, const BPU_T_Poly_GF2_16x *src) {
  // if there is not enough space resize
  if (dest->max_deg < src->max_deg) {
    BPU_freePoly(dest, 0);
    BPU_mallocPoly(dest, src->max_deg);
  }
  else {
    BPU_gf2xPolyNull(dest);
  }
  memcpy((void *) (dest->coef), (void *) (src->coef), sizeof(BPU_T_GF2_16x) * (src->max_deg + 1));
  
  dest->deg = src->deg;
}

void BPU_gf2VecCopy(BPU_T_Vector_GF2 *dest, const BPU_T_Vector_GF2 *src) {
  // if there is not enough space resize
  if (dest->elements_in_row < src->elements_in_row) {
    BPU_freeVecGF2(dest, 0);

    BPU_mallocVectorGF2(dest, src->elements_in_row * src->element_bit_size * sizeof(BPU_T_GF2));
  }
  else {
    BPU_gf2VecNull(dest);
  }
  memcpy((void *) (dest->elements), (void *) (src->elements), sizeof(BPU_T_GF2) * (src->elements_in_row));
  
  dest->len = src->len;
}

void BPU_gf2xPolyInvA(BPU_T_Poly_GF2_16x *out, const BPU_T_Poly_GF2_16x *a, const BPU_T_Poly_GF2_16x *mod, const BPU_T_Arithmetic_Data *a_data) {
  BPU_T_Poly_GF2_16x d, t;
  
  BPU_gf2xPolyExtEuclidA(&d, out, &t, a, mod, 0, a_data);

  if (d.deg != 0 || d.coef[0] != 1) {
    BPU_printDebug("inverse polynomial NOT found");
    BPU_printError("degree: %d\nelement: %d\n", d.deg, d.coef[0]);
    BPU_printGf2xPoly(out, a_data);
    BPU_gf2xPolyNull(out);
  }
  BPU_freePoly(&d, 0);
  BPU_freePoly(&t, 0);
}

BPU_T_GF2_16x BPU_gf2xPolyMakeMonic(BPU_T_Poly_GF2_16x *a, const BPU_T_Arithmetic_Data *a_data) {
  BPU_T_GF2_16x inv_lead = 0;

  // if it is already monic do nothing
  if ((a->deg > -1 && a->coef[a->deg] == 1) || a->deg == -1) {
    return inv_lead;
  }
  inv_lead = BPU_gf2xInvElement(BPU_gf2xPolyLeadCoef(a), a_data);
  BPU_gf2xPolyMulEl(a, inv_lead, a_data);

  return inv_lead;
}

int BPU_isPrime(int n) {
  int i;
  if (n == 2)
    return 1;
  for (i = 2; i <= n/2.0; i++) {   //TODO: import math.h and use sqrt(n)
    if (n % i == 0)
      return 0;
  }

  return 1;
}

int BPU_gf2VecToPoly(BPU_T_Vector_GF2 *in, BPU_T_Poly_GF2_16x *out, uint8_t ord) {
  if (BPU_mallocPoly(out, in->len / ord + 1) != 0) {
    BPU_printError("gf2VecToPoly: BPU_mallocPoly");
    return -1;
  }

  int i;
  int actual_coef = -1;

  for (i = 0; i<in->len; i++) {
    //move to next coef
    if (i % ord == 0) {
      actual_coef++;
      //fprintf(stderr, "next coef\n");
    }

    out->coef[actual_coef] ^= BPU_gf2VecGetBit(in,i) << (i%ord);
    //fprintf(stderr, "i:%d  adding:%d coef", i,BPU_gf2VecGetBit(in,i));
    //BPU_printBinaryLe(out->coef[actual_coef], ord);
    //fprintf(stderr, "\n");
  }

  out->deg = BPU_gf2xPolyGetDeg(out);

  return 0;
}

void BPU_gf2xMatDetermineSyndromeA(BPU_T_Poly_GF2_16x *syndrome, const BPU_T_Vector_GF2 *z, const BPU_T_Matrix_GF2_16x *H, const BPU_T_Arithmetic_Data *a_data) {
  int i, j;
  BPU_mallocPoly(syndrome, H->k - 1);
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

BPU_T_GF2_16x BPU_gf2xGetPseudoInv(const BPU_T_GF2_16x a, const BPU_T_GF2_16x res, const BPU_T_Arithmetic_Data *a_data) {
  return BPU_gf2xMulModT(BPU_gf2xInvElement(a, a_data), res, a_data);
}

void BPU_gf2xMatNull(BPU_T_Matrix_GF2_16x *mat) {
  int i, j;
  for (i = 0; i < mat->k; i++) {
    for (j = 0; j < mat->n; j++) {
      mat->elements[i][j] = 0;
    }
  }
}

void BPU_gf2xMatInsertPoly(BPU_T_Matrix_GF2_16x *mat, const BPU_T_Poly_GF2_16x *poly, int i)  {
  int j;
  for (j = 0; j <= poly->deg; j++) {
    mat->elements[i][j] = poly->coef[j];
  }
}

void BPU_gf2xPolyToVecA(BPU_T_Vector_GF2_16x *vec, const BPU_T_Poly_GF2_16x *poly, int len) {
  int i;

  if (poly->deg >= len) {
    BPU_printError("dimension missmatch");

    exit(-1);
  }
  BPU_mallocVectorGF2_16x(vec, len);

  for (i = 0; i <= poly->deg; i++) {
    vec->elements[i] = poly->coef[i];
  }
}

void BPU_gf2xVecToPoly(BPU_T_Poly_GF2_16x *poly, const BPU_T_Vector_GF2_16x *vec) {
  int i;
  
  for (i = 0; i < vec->len; i++) {
    poly->coef[i] = vec->elements[i];
  }
  poly->deg = BPU_gf2xPolyGetDeg(poly);
}

void BPU_gf2xSwap(BPU_T_GF2_16x *a, BPU_T_GF2_16x *b) {
  BPU_T_GF2_16x tmp;
  tmp = *a;
  *a = *b;
  *b = tmp;
}

void BPU_gf2xMatSwapRows(BPU_T_Matrix_GF2_16x *mat, int i, int j) {
  int k;
  for (k = 0; k < mat->n; k++) {
    BPU_gf2xSwap(&(mat->elements[i][k]), &(mat->elements[j][k]));
  }
}

void BPU_gf2xMatMulElRow(BPU_T_Matrix_GF2_16x *mat, const BPU_T_GF2_16x element, int row, const BPU_T_Arithmetic_Data *a_data) {
  int i;
  for (i = 0; i < mat->n; i++) {
    mat->elements[row][i] = BPU_gf2xMulModT(element, mat->elements[row][i], a_data);
  }
}

int BPU_gf2xMatFindPivot(const BPU_T_Matrix_GF2_16x *mat, int index) {
  int i;
  for (i = index; i < mat->k; i++) {
    if(mat->elements[i][index] != 0) {
      return i;
    }
  }
  return -1;
}

void BPU_gf2xVecMulEl(BPU_T_Vector_GF2_16x *vec, BPU_T_GF2_16x element, const BPU_T_Arithmetic_Data *a_data) {
  int i;
  for (i = 0; i < vec->len; i++) {
    vec->elements[i] = BPU_gf2xMulModT(element, vec->elements[i], a_data);
  }
}

void BPU_gf2xMatXorRows(BPU_T_Matrix_GF2_16x *mat, int index, int j, const BPU_T_Arithmetic_Data *a_data) {
  int k;
  BPU_T_GF2_16x element;
  BPU_T_Vector_GF2_16x tmp;

  BPU_mallocVectorGF2_16x(&tmp, mat->n);
  for (k = 0; k < tmp.len; k++) {
    tmp.elements[k] = mat->elements[index][k];
  }
  element = mat->elements[j][index];

  BPU_gf2xVecMulEl(&tmp, element, a_data);

  for (k = 0; k < tmp.len; k++) {
    mat->elements[j][k] = mat->elements[j][k] ^ tmp.elements[k];
  }
  BPU_freeVectorGF2_16x(&tmp, 0);
}

void BPU_gf2xMatClearCol(BPU_T_Matrix_GF2_16x *mat, int index, const BPU_T_Arithmetic_Data *a_data) {
  int i;
  for (i = 0; i < mat->k; i++) {
    if (i == index) {
      continue;
    }
    BPU_gf2xMatXorRows(mat, index, i, a_data);
  }
}

void BPU_gf2xMatGEM(BPU_T_Matrix_GF2_16x *mat, const BPU_T_Arithmetic_Data *a_data) {
  int i, pivot;
  BPU_T_GF2_16x element;
  for (i = 0; i < mat->k; i++) {
    pivot = BPU_gf2xMatFindPivot(mat, i);
    if (pivot == -1)
      BPU_printError("################# unbeliviable 'badness' ###########");
    if (pivot != i) {
      BPU_gf2xMatSwapRows(mat, i, pivot);
    }
    if (mat->elements[i][i] != 1) {
      element = BPU_gf2xPowerModT(mat->elements[i][i], -1, a_data);
      BPU_gf2xMatMulElRow(mat, element, i, a_data);
    }
    BPU_gf2xMatClearCol(mat, i, a_data);
  }
}

int BPU_permIsValid(const BPU_T_Perm_Vector *p) {
  int i, j;

  for (i = 0; i < p->size; i++) {
    for (j = 0; j < p->size; j++) {
      if (i != j && p->elements[i] == p->elements[j]) {
        BPU_printError("permutation is not valid");
        BPU_printPerm(p);

        return 1;
      }
    }
  }
  return 0;
}

int BPU_gf2VecCmp(const BPU_T_Vector_GF2 *v1, const BPU_T_Vector_GF2 *v2) {
  int i;

  if (v1->len != v2->len) {
    return 1;
  }
  for (i = 0; i < v1->len; i++) {
    if (BPU_gf2VecGetBit(v1, i) != BPU_gf2VecGetBit(v2, i)) {
      return 2;
    }
  }
  return 0;
}

void BPU_gf2xPolyGenRandom(BPU_T_Poly_GF2_16x *p, int t, const BPU_T_Arithmetic_Data *a_data) {
  int i;
  p->coef[t] = 1;
  do {
    p->coef[0] = BPU_getRandValue(1, a_data->ord);
  } while (p->coef[0] == 0);
  
  for (i = 1; i < t; i++) {
    p->coef[i] = BPU_getRandValue(0, a_data->ord);
  }
  p->deg = t;
}