/*
This file is part of BitPunch
Copyright (C) 2013-2016 Frantisek Uhrecky <frantisek.uhrecky[what here]gmail.com>
Copyright (C) 2013-2015 Andrej Gulyas <andrej.guly[what here]gmail.com>
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
#include "gf2.h"
#include "perm.h"
#include "uni.h"

#include <stdlib.h>
#include <bitpunch/debugio.h>
#include <bitpunch/prng/prng.h>

#ifdef BPU_CONF_PRINT
/* ==================================== Print functions ==================================== */
void BPU_printGf2Mat(const BPU_T_GF2_Matrix * m) {
    int i, j, bits_to_print;

    fprintf(stderr, "Matrix size: %dx%d\n", m->k, m->n);

    for (i = 0; i < m->k; i++) {
        fprintf(stderr, "%4d: ", i);

        for (j = 0; j <= m->elements_in_row - 1; j++) {
            if (j == m->elements_in_row - 1) {
                if (m->n % (m->element_bit_size) != 0) {
                    bits_to_print = m->n % m->element_bit_size;
                }
                else {
                    bits_to_print = m->element_bit_size;
                }
            }
            else {
                bits_to_print = m->element_bit_size;
            }
            BPU_printBinaryLsb(m->elements[i][j], bits_to_print);
            // fprintf(stderr, " "); // medzera medzi elementami
        }
        fprintf(stderr, "\n");
    }
}

void BPU_printGf2SparsePoly(const BPU_T_GF2_Sparse_Poly * v) {
    int i;

    fprintf(stderr, "Sparse poly (%i): ", v->weight);
    for (i = 0; i < v->weight; i++) {
        fprintf(stderr, "%3i ", v->index[i]);
    }
    fprintf(stderr, "\n");
}

void BPU_printGf2PolyForMatrix(const BPU_T_GF2_Poly * v) {
    int j, bits_to_print;

    for (j = 0; j < v->array_length; j++) {
        if (j == v->array_length - 1) {
            if (v->len % (v->element_bit_size) != 0) {
                bits_to_print = v->len % v->element_bit_size;
            }
            else {
                bits_to_print = v->element_bit_size;
            }
        }
        else {
            bits_to_print = v->element_bit_size;
        }
        BPU_printBinaryLsb(v->elements[j], bits_to_print);
    }
}

void BPU_printGf2Poly(const BPU_T_GF2_Poly * v) {
    int j, bits_to_print;

    fprintf(stderr, "Poly (%4d): ", v->len - 1);
    for (j = v->array_length - 1; j >= 0; j--) {
        if (j == v->array_length - 1) {
            if (v->len % (v->element_bit_size) != 0) {
                bits_to_print = v->len % v->element_bit_size;
            }
            else {
                bits_to_print = v->element_bit_size;
            }
        }
        else {
            bits_to_print = v->element_bit_size;
        }
        BPU_printBinaryMsb(v->elements[j], bits_to_print);
    }
    fprintf(stderr, "\n");
}

void BPU_printGf2QcMatrix(const BPU_T_GF2_QC_Matrix * v) {
    int ele, i;
    BPU_T_GF2_Poly temp;

    fprintf(stderr, "%s QC Matrix(%i x %i) with %i elements",
            v->isVertical ? "VERTICAL" : "HORIZONTAL",
            (v->is_I_appended ? v->k : 0) + v->n, v->k, v->element_count);
    if (v->is_I_appended)
        fprintf(stderr, " and Identity matrix");
    fprintf(stderr, "\n");

    for (ele = 0; ele < v->element_count; ele++) {
        BPU_gf2PolyCopy(&temp, &v->matrices[ele]);
        for (i = 0; i < v->element_size; i++) {
            BPU_printGf2PolyForMatrix(&temp);
            fprintf(stderr, "\n");
            BPU_gf2PolyMulX(&temp);
        }
        BPU_gf2PolyFree(&temp, 0);
    }
}

void BPU_printGf2SparseQcMatrix(const BPU_T_GF2_Sparse_Qc_Matrix * v) {
    int i;
    BPU_T_GF2_Sparse_Poly row;

    fprintf(stderr, "%s QC Matrix(%i x %i) with %i elements\n",
            v->isVertical ? "VERTICAL" : "HORIZONTAL", v->n, v->k,
            v->element_count);

    for (i = 0; i < v->k; i++) {
        BPU_gf2SparseQcMatrixGetRow(&row, v, i);
        BPU_printGf2SparsePoly(&row);
        BPU_gf2SparsePolyFree(&row, 0);
    }
}

/* ------------------------------------ Print functions ------------------------------------ */
#endif // BPU_CONF_PRINT

void BPU_gf2MatFree(BPU_T_GF2_Matrix ** m) {
    int i;

    if (!*m) {
        return;
    }
    // first free cols
    for (i = 0; i < (*m)->k; i++) {
        free((*m)->elements[i]);
    }
    // then free rows
    free((*m)->elements);
    free((*m));
    *m = NULL;
}

int BPU_gf2MatMalloc(BPU_T_GF2_Matrix ** m, int rows, int cols) {
    int i;

    *m = (BPU_T_GF2_Matrix *) calloc(sizeof(BPU_T_GF2_Matrix), 1);

    if (!*m) {
        BPU_printError("allocation error");
        return -1;
    }
    // element size
    (*m)->element_bit_size = sizeof(BPU_T_GF2) * 8;

    // rows
    (*m)->k = rows;
    // cols
    (*m)->n = cols;

    // calc how many elements of set size will be in one row
    int modul = 0;

    if (cols % (*m)->element_bit_size > 0) {
        modul = 1;
    }
    (*m)->elements_in_row = cols / (*m)->element_bit_size + modul;

    // allocate rows
    (*m)->elements = (BPU_T_GF2 **) malloc(sizeof(BPU_T_GF2 *) * (*m)->k);

    if (!(*m)->elements) {
        BPU_printError("can not allocate memory for matrix rows");
        return -1;
    }
    // allocate cols
    for (i = 0; i < (*m)->k; i++) {
        (*m)->elements[i] =
            (BPU_T_GF2 *) calloc(1, sizeof(BPU_T_GF2) * (*m)->elements_in_row);

        if (!(*m)->elements[i]) {
            BPU_printError("can not allocate memory for matrix cols");
            return -2;
        }
    }
    return 0;
}

void BPU_gf2SparsePolyMalloc(BPU_T_GF2_Sparse_Poly * p, int weight) {
    // allocate indexes
    p->index = (uint32_t *) malloc(weight * sizeof(uint32_t));
    // set weight
    p->weight = weight;
}

void BPU_gf2SparsePolyFree(BPU_T_GF2_Sparse_Poly * p, int is_dyn) {
    free(p->index);

    if (is_dyn)
        free(p);
}

void BPU_gf2SparseQcMatrixMalloc(BPU_T_GF2_Sparse_Qc_Matrix * v,
                                 int element_count, int element_size,
                                 int isVertical) {
    // allocate matrices
    v->matrices =
        (BPU_T_GF2_Sparse_Poly *) malloc(element_count *
                                         sizeof(BPU_T_GF2_Sparse_Poly));

    // set sizes depended on orientation
    if (isVertical) {
        v->k = element_count * element_size;
        v->n = element_size;
    }
    else {
        v->k = element_size;
        v->n = element_count * element_size;
    }

    // set others
    v->element_count = element_count;
    v->element_size = element_size;
    v->isVertical = isVertical;
}

void BPU_gf2SparseQcMatrixFree(BPU_T_GF2_Sparse_Qc_Matrix * v, int is_dyn) {
    int i;

    // free matrices
    for (i = 0; i < v->element_count; i++)
        BPU_gf2SparsePolyFree(&v->matrices[i], is_dyn);

    free(v->matrices);

    if (is_dyn)
        free(v);
}

void BPU_gf2PolyFree(BPU_T_GF2_Poly * p, int is_dyn) {
    free(p->elements);

    if (is_dyn) {
        free(p);
    }
}

int BPU_gf2PolyMalloc(BPU_T_GF2_Poly * p, int len) {
    // element size in bits
    p->element_bit_size = sizeof(BPU_T_GF2) * 8;

    // len
    p->len = len;

    // calc how many elements of set size will be in one row
    int modul = 0;

    if (len % p->element_bit_size > 0) {
        modul = 1;
    }
    p->array_length = len / p->element_bit_size + modul;

    // allocate elemtens
    p->elements = (BPU_T_GF2 *) calloc(1, sizeof(BPU_T_GF2) * p->array_length);

    if (!p->elements) {
        BPU_printError("can not allocate memory for vector of len %d", len);
        return 1;
    }
    return 0;
}

int BPU_gf2QcMatrixMalloc(BPU_T_GF2_QC_Matrix * v, int element_count,
                          int element_size, int isVertical, int is_I_appended) {
    int err = 0;

    // check isVertical
    if (isVertical != 0 && isVertical != 1) {
        return -1;
    }

    // allocate matrices
    v->matrices =
        (BPU_T_GF2_Poly *) malloc(element_count * sizeof(BPU_T_GF2_Poly));

    // set sizes depended on orientation
    if (isVertical) {
        v->k = element_count * element_size;
        v->n = element_size;
    }
    else {
        v->k = element_size;
        v->n = element_count * element_size;
    }

    // set others
    v->element_count = element_count;
    v->element_size = element_size;
    v->is_I_appended = is_I_appended;
    v->isVertical = isVertical;

    return err;
}

// free QC binary matrix
void BPU_gf2QcMatrixFree(BPU_T_GF2_QC_Matrix * v, int is_dyn) {
    int i;

    // free matrices
    for (i = 0; i < v->element_count; i++) {
        BPU_gf2PolyFree(&v->matrices[i], 0);
    }

    free(v->matrices);

    if (is_dyn)
        free(v);
}

int BPU_gf2VecRand(BPU_T_GF2_Vector * out, int w) {
    int i, j;

    if (w > out->len) {
        BPU_printError("weight error w > l");
        return -2;
    }
    //vector of random weight
    if (w == 0) {
        for (i = 0; i < out->len; i++) {
            BPU_gf2VecSetBit(out, i, BPU_prngGetRand(0, 2));
        }
    }
    //vector of required weight
    else {
        BPU_gf2VecNull(out);

        for (i = 0; i < w; i++) {
            j = BPU_prngGetRand(0, out->len);

            if (BPU_gf2VecGetBit(out, j) == 0) {
                BPU_gf2VecSetBit(out, j, 1);
            }
            else {
                i--;
            }
        }
    }
    return 0;
}

int BPU_gf2MatCopy(BPU_T_GF2_Matrix * out, const BPU_T_GF2_Matrix * in) {
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

int BPU_gf2VecPermute(BPU_T_GF2_Vector * vec,
                      const BPU_T_Perm_Vector * permutation) {
    int i;
    BPU_T_GF2_Vector *tmp;

    BPU_gf2VecMalloc(&tmp, vec->len);

    for (i = 0; i < permutation->size; i++) {
        BPU_gf2VecSetBit(tmp, i,
                         BPU_gf2VecGetBit(vec, permutation->elements[i]));
    }
    BPU_gf2VecCopy(vec, tmp);

    BPU_gf2VecFree(&tmp);

    return 0;
}

BPU_T_GF2 BPU_gf2MatGetMaskedBit(const BPU_T_GF2_Matrix * m, uint32_t row,
                                 uint32_t bit) {
    int segment, bit_in_segment;

    segment = bit / (m->element_bit_size);
    bit_in_segment = bit % (m->element_bit_size);
    // TODO: consider repleacing di literal 1u
    return m->elements[row][segment] & ((uint32_t) 1 << bit_in_segment);
}

BPU_T_GF2 BPU_gf2VecGetMaskedBit(const BPU_T_GF2_Vector * vec, uint32_t bit) {
    int segment, bit_in_segment;

    segment = bit / (vec->element_bit_size);
    bit_in_segment = bit % (vec->element_bit_size);
    // TODO: consider repleacing di literal 1u
    return vec->elements[segment] & ((uint32_t) 1 << bit_in_segment);
}

int BPU_gf2MatTransp(BPU_T_GF2_Matrix * out, const BPU_T_GF2_Matrix * in) {
    int i, j;

    if (out->k != in->n || out->n != in->k) {
        BPU_printError("Wrong matrix dimenzion");

        return -1;
    }
    for (i = 0; i < in->k; i++) {
        for (j = 0; j < in->n; j++) {
            BPU_gf2MatSetBit(out, j, i, BPU_gf2MatGetBit(in, i, j));
        }                       // col loop
    }                           // row loop

    return 0;
}

void BPU_gf2Swap(BPU_T_GF2 * a, BPU_T_GF2 * b) {
    BPU_T_GF2 tmp;

    tmp = *a;
    *a = *b;
    *b = tmp;
}

void BPU_gf2MatSwapRows(BPU_T_GF2_Matrix * mat, int i, int j) {
    int k;

    for (k = 0; k < mat->elements_in_row; k++) {
        BPU_gf2Swap(&(mat->elements[i][k]), &(mat->elements[j][k]));
    }
}



int BPU_gf2MatFindRow(const BPU_T_GF2_Matrix * mat, int i, int start_index) {
    int k;

    for (k = start_index; k < mat->k; k++) {
        if (BPU_gf2MatGetMaskedBit(mat, k, i)) {
            return k;
        }
    }
    return -1;
}

int BPU_gf2MatFindCol(const BPU_T_GF2_Matrix * mat, int i, int start_index) {
    int k;

    for (k = start_index; k < mat->n; k++) {
        if (BPU_gf2MatGetMaskedBit(mat, i, k)) {
            return k;
        }
    }
    return -1;
}

int BPU_gf2MatMakeSystematic(BPU_T_GF2_Matrix * inout) {
    int act_position = 0;
    int i;
    int row;

    for (act_position = 0; act_position < inout->k; act_position++) {
        row = BPU_gf2MatFindRow(inout, act_position, act_position);
        if (row == -1) {
            BPU_printWarning("Not systematic");
            return -1;
        }
        BPU_gf2MatSwapRows(inout, act_position, row);

        // xor with the rest of rows if needed
        for (i = 0; i < inout->k; i++) {
            if (BPU_gf2MatGetMaskedBit(inout, i, act_position)
                && act_position != i) {
                BPU_gf2MatXorRows(inout, i, act_position);
            }
        }
    }
    return 0;
}

int BPU_gf2VecConcat(BPU_T_GF2_Vector * out, const BPU_T_GF2_Vector * vec1,
                     const BPU_T_GF2_Vector * vec2) {
    int len = vec1->len + vec2->len;
    int i;

    if (out->len != len) {
        if (BPU_gf2VecResize(out, len)) {
            BPU_printError("resize error");
            return -1;
        }
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

int BPU_gf2VecCrop(BPU_T_GF2_Vector * out, const BPU_T_GF2_Vector * in,
                   const int start, const int length) {
    int i;
    int counter = 0;

    //input test
    if (start < 0 || (length + start) > in->len) {
        BPU_printError("cropVector: bad params");
        return -1;
    }
    if (out->len < length) {
        BPU_printError("cropVector: out vector is smaller then needed");
        return -1;
    }
    for (i = start; i < start + length; i++) {
        BPU_gf2VecSetBit(out, counter, BPU_gf2VecGetBit(in, i));
        counter++;
    }
    return 0;
}

int BPU_gf2MatGetRowAsGf2Vec(BPU_T_GF2_Vector * out,
                             const BPU_T_GF2_Matrix * in, int row) {
    if (out->len != in->n) {
        BPU_printError("dimension is wrong out->len %d != in->n %d", out->len,
                       in->n);
        return -1;
    }
    BPU_gf2MatCopyRowToVec(out, in, row);

    return 0;
}

void BPU_gf2VecCopy(BPU_T_GF2_Vector * dest, const BPU_T_GF2_Vector * src) {
    // if there is not enough space resize
    if (dest->array_length < src->array_length) {
        BPU_gf2VecResize(dest,
                         src->array_length * src->element_bit_size *
                         sizeof(BPU_T_GF2));
    }
    else {
        BPU_gf2VecNull(dest);
    }
    memcpy((void *) (dest->elements), (void *) (src->elements),
           sizeof(BPU_T_GF2) * (src->array_length));

    dest->len = src->len;
}

int BPU_gf2VecCmp(const BPU_T_GF2_Vector * v1, const BPU_T_GF2_Vector * v2) {
    int i;

    if (v1->len != v2->len) {
        return -1;
    }
    for (i = 0; i < v1->len; i++) {
        if (BPU_gf2VecGetBit(v1, i) != BPU_gf2VecGetBit(v2, i)) {
            return i + 1;
        }
    }
    return 0;
}

int BPU_gf2VecXor(BPU_T_GF2_Vector * out, const BPU_T_GF2_Vector * in) {
    int i;

    if (out->len != in->len) {
        BPU_printError
            ("BPU_gf2VecXor: length error (el. in row) %d != %d, len %d != %d",
             out->array_length, in->array_length, out->len, in->len);

        return -1;
    }
    for (i = 0; i < out->array_length; i++) {
        out->elements[i] ^= in->elements[i];
    }
    return 0;
}

int BPU_gf2VecMulMat(BPU_T_GF2_Vector * out, const BPU_T_GF2_Vector * v,
                     const BPU_T_GF2_Matrix * b) {
    int i, j;

    if ((v->len != b->k) || (out->len != b->n)) {
        BPU_printError
            ("wrong vector and matrix dimension v->len = %d, b->k = %d", v->len,
             b->k);
        BPU_printError
            ("wrong vector and matrix dimension out->len = %d, b->n = %d",
             out->len, b->n);

        return -1;
    }
    // null elements
    BPU_gf2VecNull(out);

    for (i = 0; i < v->len; i++) {
        if (BPU_gf2VecGetBit(v, i)) {
            // xor rows
            for (j = 0; j < out->array_length; j++) {
                out->elements[j] ^= b->elements[i][j];
            }
        }
    }
    return 0;
}

void BPU_gf2MatXorRows(BPU_T_GF2_Matrix * mat, int i, int j) {
    int k;

    for (k = 0; k < mat->elements_in_row; k++) {
        mat->elements[i][k] ^= mat->elements[j][k];
    }
}

int BPU_gf2MatPermute(BPU_T_GF2_Matrix * inout, BPU_T_Perm_Vector * permutation) {
    int i, j;
    int bit = 0;
    BPU_T_GF2_Vector *vector;
    int length;

    if (inout->n != permutation->size) {
        BPU_printError("permutation size not correct m->n = %d, p->size = %d",
                       inout->n, permutation->size);

        return -1;
    }
    length = inout->elements_in_row * (inout->element_bit_size / 8);

    BPU_gf2VecMalloc(&vector, permutation->size);

    for (i = 0; i < inout->k; i++) {
        memcpy((vector->elements), inout->elements[i], length);
        for (j = 0; j < permutation->size; j++) {
            bit = BPU_gf2VecGetBit(vector, permutation->elements[j]);
            BPU_gf2MatSetBit(inout, i, j, bit);
        }
    }
    BPU_gf2VecFree(&vector);
    return 0;
}

int BPU_gf2MatCrop(BPU_T_GF2_Matrix * m, uint16_t width) {
    BPU_T_GF2_Vector *row, *cropped_row;
    int length, i, new_length;

    length = m->elements_in_row * (m->element_bit_size / 8);
    BPU_gf2VecMalloc(&row, m->n);
    BPU_gf2VecMalloc(&cropped_row, width);
    new_length = cropped_row->array_length * (m->element_bit_size / 8);
    for (i = 0; i < m->k; i++) {
        memcpy(row->elements, m->elements[i], length);
        BPU_gf2VecCrop(cropped_row, row, m->k, width);
        free(m->elements[i]);
        m->elements[i] = (BPU_T_GF2 *) malloc(new_length);
        memcpy(m->elements[i], cropped_row->elements, new_length);
    }
    m->n = cropped_row->len;
    m->elements_in_row = cropped_row->array_length;

    BPU_gf2VecFree(&row);
    BPU_gf2VecFree(&cropped_row);

    return 0;
}

uint8_t BPU_getParity(BPU_T_GF2 dword) {
    uint32_t tmp = dword;

    tmp = (tmp >> 16) ^ tmp;
    tmp = (tmp >> 8) ^ tmp;
    tmp = (tmp >> 4) ^ tmp;
    tmp = (tmp >> 2) ^ tmp;
    tmp = (tmp >> 1) ^ tmp;
    return tmp & 1;
}

void BPU_gf2PolyCopy(BPU_T_GF2_Poly * out, const BPU_T_GF2_Poly * in) {

    int i;

    // allocate output poly
    BPU_gf2PolyMalloc(out, in->len);

    // copy all elements
    if (in->len != 0)
        for (i = 0; i < in->array_length; i++) {
            out->elements[i] = in->elements[i];
        }
}

int BPU_gf2QcMatrixToSparse(BPU_T_GF2_Sparse_Qc_Matrix * out,
                            const BPU_T_GF2_QC_Matrix * in, const int wi[]) {
    int i, counter, bit;

    // allocate output matrix
    BPU_gf2SparseQcMatrixMalloc(out, in->element_count, in->element_size, 1);

    // transpose polynoms
    for (i = 0; i < in->element_count; i++) {
        counter = 0;
        // allocate sparse poly
        BPU_gf2SparsePolyMalloc(&out->matrices[i], wi[i]);
        // set bits
        for (bit = 0; bit < in->matrices[i].len; bit++) {
            if (BPU_gf2VecGetBit(&in->matrices[i], bit) == 1ul) {
                out->matrices[i].index[counter] = (uint32_t) (bit);
                counter++;
            }
        }
        // weight error
        if (counter != wi[i]) {
            BPU_printError("weight error. Weight should be %i, but is %i.\n",
                           wi[i], counter);
            return -1;
        }
    }
    return 0;
}

int BPU_gf2PolyInitRand(BPU_T_GF2_Poly * out, int l, int w, int set_deg) {
    int ret;

    // allocate output poly
    ret = BPU_gf2PolyMalloc(out, l);

    // set random bits
    if (w >= 0)
        ret = BPU_gf2VecRand((BPU_T_GF2_Vector *) out, w);

    // set poly deg
    if (set_deg)
        BPU_gf2PolySetDeg(out, -1);

    return ret;
}

void BPU_gf2SparsePolyCopy(BPU_T_GF2_Sparse_Poly * out,
                           const BPU_T_GF2_Sparse_Poly * in) {
    int i;

    // allocate output poly
    BPU_gf2SparsePolyMalloc(out, in->weight);
    // copy all indexes
    for (i = 0; i < in->weight; i++) {
        out->index[i] = in->index[i];
    }
}

void BPU_gf2SparseQcMatrixTransp(BPU_T_GF2_Sparse_Qc_Matrix * out,
                                 const BPU_T_GF2_Sparse_Qc_Matrix * in) {
    int counter, coeff, ele, zero_coeff_is_set;

    // allocate memory for output matrix
    BPU_gf2SparseQcMatrixMalloc(out, in->element_count, in->element_size, 1);

    // for all polynoms
    for (ele = 0; ele < in->element_count; ele++) {
        counter = 0;
        zero_coeff_is_set = 0;

        // alloc new matrix of same weight
        BPU_gf2SparsePolyMalloc(&out->matrices[ele], in->matrices[ele].weight);

        // is zero coeff set?
        zero_coeff_is_set = (in->matrices[ele].index[0] == 0ul);
        // set zero coeff
        if (zero_coeff_is_set) {
            out->matrices[ele].index[counter] = 0ul;
            counter++;
        }

        // for other coeffs
        for (coeff = in->matrices[ele].weight - 1;
             coeff >= (1 - !zero_coeff_is_set); coeff--) {
            out->matrices[ele].index[counter] =
                in->element_size - in->matrices[ele].index[coeff];
            counter++;
        }
    }
}

void BPU_gf2PolyShiftLeft(BPU_T_GF2_Poly * a, int shift_count) {
    int i;
    int diff, bit_shift, start, shift_right, shift_left;
    uint32_t ele1, ele2;

    // allocate new poly, with additional bits
    // BPU_gf2PolyMalloc(&poly, a->len+shift_count);
    BPU_gf2PolySetDeg(a, a->len + shift_count);

    // calc element shift and bit shift
    diff = shift_count / a->element_bit_size;
    bit_shift = shift_count % a->element_bit_size;

    start = diff;
    // shift elements
    for (i = a->array_length - 1; i >= start; i--) {
        // not the first element, concat two elements
        if (i - start != 0) {

            // get first element
            if ((a->element_bit_size - bit_shift) >= a->element_bit_size) {
                ele1 = 0ul;
                shift_right = 0;
            }
            else {
                ele1 = a->elements[i - start - 1];
                shift_right = a->element_bit_size - bit_shift;
            }

            // get second element
            if ((i - start) >= a->array_length) {
                ele2 = 0ul;
                shift_left = 0;
            }
            else {
                ele2 = a->elements[i - start];
                shift_left = bit_shift;
            }

            // set element
            a->elements[i] = (ele1 >> shift_right) ^ (ele2 << shift_left);
        }
        // first element, just shift
        else
            a->elements[i] = a->elements[i - start] << bit_shift;
    }

    // set zeros in the beginning
    for (i = 0; i < diff; i++)
        a->elements[i] = 0ul;
}

int BPU_gf2PolyGetHighestBitPos(BPU_T_GF2_Poly * a) {
    int ele;

    // scan all elements and found highest non zero element
    for (ele = a->array_length - 1; ele >= 0; ele--) {
        if (a->elements[ele] != 0ul)
            // find highest bit in highest non zero element
            return msb32(a->elements[ele], 1, a->element_bit_size,
                         a->element_bit_size) + ele * a->element_bit_size;
    }

    // poly is zero
    return -1;
}

void BPU_gf2PolySetDeg(BPU_T_GF2_Poly * a, int deg) {
    int j, orig_elements_in_row = a->array_length;

    // find max degree
    if (deg == -1) {
        deg = BPU_gf2PolyGetHighestBitPos(a);
    }

    if (deg != -1) {
        // set degree and element count
        a->len = deg;
        a->array_length =
            a->len / a->element_bit_size + ((a->len % a->element_bit_size) !=
                                            0 ? 1 : 0);
        // reallocate elements
        a->elements =
            (BPU_T_GF2 *) realloc(a->elements,
                                  sizeof(BPU_T_GF2) * a->array_length);
        // null new elements
        for (j = orig_elements_in_row; j < a->array_length; j++)
            a->elements[j] = 0ul;
    }
    // poly is zero
    else {
        a->len = 0;
        a->array_length = 0;
    }
}

void BPU_gf2PolyMulX(BPU_T_GF2_Poly * a) {
    int ele;
    uint8_t shift = a->element_bit_size - 1;

    // save highest bit
    uint32_t msb = BPU_gf2VecGetBit(a, a->len - 1);

    // null highest bit
    BPU_gf2VecSetBit(a, a->len - 1, 0ul);

    // for all elements
    for (ele = a->array_length - 1; ele >= 1; ele--) {
        a->elements[ele] =
            (a->elements[ele] << 1) ^ (a->elements[ele - 1] >> shift);
    }

    // last element just shift
    a->elements[0] <<= 1;
    // and set lowest bit
    a->elements[0] ^= msb;
}

void BPU_gf2PolyShiftRightOne(BPU_T_GF2_Poly * a) {
    int i;

    // for all elements
    for (i = 0; i < a->array_length - 1; i++) {
        // shift right by one and add lowest bit from next element
        a->elements[i] =
            (a->elements[i] >> 1) ^ ((a->elements[i + 1] & 1ul) <<
                                     (a->element_bit_size - 1));
    }

    // last element just shift
    a->elements[a->array_length - 1] >>= 1;

}

void BPU_gf2PolyAdd(BPU_T_GF2_Poly * out, const BPU_T_GF2_Poly * in, int crop) {
    int i;

    // if out poly is zero, just copy in into out
    if (out->len == 0) {
        BPU_gf2PolyFree(out, 0);
        BPU_gf2PolyCopy(out, in);
    }
    // if in is non zero
    else if (in->len != 0) {
        // if deg(in) > deg(out)
        if (in->len > out->len)
            // set degree
            BPU_gf2PolySetDeg(out, in->len);
        // make add
        for (i = 0; i < in->array_length; i++)
            out->elements[i] ^= in->elements[i];
    }

    // if set new degree
    if (crop)
        BPU_gf2PolySetDeg(out, -1);
}

// operation add with binary polynomials with high degree
void BPU_gf2SparsePolyAdd(BPU_T_GF2_Poly * out,
                          const BPU_T_GF2_Sparse_Poly * in) {
    int i;
    uint8_t bit;

    // for all coefficients
    for (i = 0; i < in->weight; i++) {
        bit = BPU_gf2VecGetBit(out, in->index[i]) ^ 1ul;
        BPU_gf2VecSetBit(out, in->index[i], bit);
    }
}

int BPU_gf2SparsePolyAndHW(const BPU_T_GF2_Poly * a,
                           const BPU_T_GF2_Sparse_Poly * b) {
    int i, hw = 0;

    // for all coefficients
    for (i = 0; i < b->weight; i++)
        // if both poly has set coeff 
        if (BPU_gf2VecGetBit(a, b->index[i]) == 1ul)
            // increase hamming weight
            hw++;

    return hw;
}

void BPU_gf2SparseQcMatrixGetRow(BPU_T_GF2_Sparse_Poly * p,
                                 const BPU_T_GF2_Sparse_Qc_Matrix * m,
                                 int row_num) {
    int i;

    // if rownum is in matrix
    if (row_num < m->k) {
        // allocate output polynom
        BPU_gf2SparsePolyCopy(p, &m->matrices[row_num / m->element_size]);
        // VERTICAL QC matrix
        if (m->isVertical)
            // for all coefficients
            for (i = 0; i < p->weight; i++)
                // shift coefficients 
                p->index[i] = ((p->index[i]) + row_num) % m->element_size;
        // horizontal matrix is not supported
        else {
            BPU_printError
                ("BPU_QcMatrixGetRow: HORIZONTAL matrix not supported\n");
        }
    }
    // row num is out of the matrix
    else {
        BPU_printError
            ("BPU_QcMatrixGetRow: row with index %i does not exist\n", row_num);
    }
}

void BPU_gf2PolyMulMod(const BPU_T_GF2_Poly * a, const BPU_T_GF2_Poly * b,
                       BPU_T_GF2_Poly * c, const BPU_T_GF2_Poly * m, int crop) {
    int i;
    BPU_T_GF2_Poly temp_b;

    // if one of factors is zero, product will also be zero
    if (a->len == 0 || b->len == 0) {
        BPU_gf2PolyMalloc(c, 0);
        return;
    }

    // copy b to temp_b and prolong to modulo lenght - 1
    BPU_gf2PolyCopy(&temp_b, b);
    BPU_gf2PolySetDeg(&temp_b, m->len - 1);

    // malloc multiplication product
    BPU_gf2PolyMalloc(c, m->len);

    // for length of a
    for (i = 0; i < a->len; i++) {
        // 1 in a -> multiply
        if (BPU_gf2VecGetBit(a, i) == 1ul)
            BPU_gf2PolyAdd(c, &temp_b, 0);

        // multiply b by 2
        BPU_gf2PolyMulX(&temp_b);
    }

    // if do not crop the result length
    if (!crop)
        BPU_gf2PolySetDeg(c, m->len - 1);
    else
        BPU_gf2PolySetDeg(c, -1);

    BPU_gf2PolyFree(&temp_b, 0);
}

void BPU_gf2PolyDiv(BPU_T_GF2_Poly * q, BPU_T_GF2_Poly * r,
                    const BPU_T_GF2_Poly * a, const BPU_T_GF2_Poly * b) {
    BPU_T_GF2_Poly divisor, dividend;
    int limit_deg = a->len - b->len;
    int i = 0;

    // copy a, b
    BPU_gf2PolyCopy(&divisor, b);
    BPU_gf2PolyCopy(&dividend, a);

    // allocate quotient
    BPU_gf2PolyMalloc(q, a->len);

    // divisor shift to deg of dividend
    BPU_gf2PolyShiftLeft(&divisor, a->len - b->len);

    while (dividend.len >= b->len) {
        if (dividend.len == divisor.len) {
            BPU_gf2VecSetBit(q, limit_deg - i, 1ul);

            // dividend - divisor
            BPU_gf2PolyAdd(&dividend, &divisor, 1);
        }

        // divisor degree decreased by 1
        BPU_gf2PolyShiftRightOne(&divisor);
        // set actual degree
        BPU_gf2PolySetDeg(&divisor, -1);
        i++;
    }

    BPU_gf2PolyCopy(r, &dividend);
    BPU_gf2PolySetDeg(q, -1);

    // free
    BPU_gf2PolyFree(&dividend, 0);
    BPU_gf2PolyFree(&divisor, 0);
}

// XGCD with binary polynomial with high degree
void BPU_gf2PolyExtEuclidA(BPU_T_GF2_Poly * d, BPU_T_GF2_Poly * s,
                           BPU_T_GF2_Poly * t, const BPU_T_GF2_Poly * a,
                           const BPU_T_GF2_Poly * b, const BPU_T_GF2_Poly * m) {
    BPU_T_GF2_Poly tmp, tmp_2, old_s, old_t, old_r, r, q;
    int deg = (a->len > b->len) ? a->len : b->len;

    // allocate Bezout coeffitients
    BPU_gf2PolyMalloc(s, 0);
    BPU_gf2PolyMalloc(t, deg);
    BPU_gf2PolyMalloc(&old_s, deg);
    BPU_gf2PolyMalloc(&old_t, 0);

    // set initial values
    BPU_gf2PolyCopy(&r, a);
    BPU_gf2PolyCopy(&old_r, b);

    if (a->len == 0) {
        old_t.elements[0] = 1ul;
        BPU_gf2PolySetDeg(&old_t, -1);
    }
    else if (b->len == 0) {
        BPU_gf2PolyFree(&old_r, 0);
        BPU_gf2PolyCopy(&old_r, a);
        old_s.elements[0] = 1ul;
        BPU_gf2PolySetDeg(&old_s, -1);
    }
    // run algoritm, if everything is OK
    else {
        // set initial values
        old_s.elements[0] = 1ul;
        BPU_gf2PolySetDeg(&old_s, -1);
        t->elements[0] = 1ul;
        BPU_gf2PolySetDeg(t, -1);

        // while loop until r is not zero
        while (r.len > 0) {
            // divide
            BPU_gf2PolyDiv(&q, &tmp, &old_r, &r);

            // save old reminder
            BPU_gf2PolyFree(&old_r, 0);
            BPU_gf2PolyCopy(&old_r, &r);

            // save current reminder
            BPU_gf2PolyFree(&r, 0);
            BPU_gf2PolyCopy(&r, &tmp);

            // free
            BPU_gf2PolyFree(&tmp, 0);

            // save s quocient
            BPU_gf2PolyCopy(&tmp, &old_s);
            BPU_gf2PolyFree(&old_s, 0);
            BPU_gf2PolyCopy(&old_s, s);

            BPU_gf2PolyMulMod(&q, s, &tmp_2, m, 1);
            BPU_gf2PolyAdd(&tmp, &tmp_2, 1);
            BPU_gf2PolyFree(s, 0);
            BPU_gf2PolyCopy(s, &tmp);

            // free
            BPU_gf2PolyFree(&tmp, 0);
            BPU_gf2PolyFree(&tmp_2, 0);

            // save t quocient
            BPU_gf2PolyCopy(&tmp, &old_t);
            BPU_gf2PolyFree(&old_t, 0);
            BPU_gf2PolyCopy(&old_t, t);
            BPU_gf2PolyMulMod(&q, t, &tmp_2, m, 1);
            BPU_gf2PolyAdd(&tmp, &tmp_2, 1);
            BPU_gf2PolyFree(t, 0);
            BPU_gf2PolyCopy(t, &tmp);

            // free
            BPU_gf2PolyFree(&tmp_2, 0);
            BPU_gf2PolyFree(&tmp, 0);
            BPU_gf2PolyFree(&q, 0);
        }
    }
    // prepare return values
    BPU_gf2PolyFree(t, 0);
    BPU_gf2PolyFree(s, 0);
    BPU_gf2PolyCopy(d, &old_r);
    BPU_gf2PolyCopy(s, &old_s);
    BPU_gf2PolyCopy(t, &old_t);

    // free
    BPU_gf2PolyFree(&old_s, 0);
    BPU_gf2PolyFree(&old_t, 0);
    BPU_gf2PolyFree(&old_r, 0);
    BPU_gf2PolyFree(&r, 0);
}

int BPU_gf2PolyInv(BPU_T_GF2_Poly * out, const BPU_T_GF2_Poly * a,
                   const BPU_T_GF2_Poly * m) {
    BPU_T_GF2_Poly d, s;
    int ret = 1;

    // call XGCD
    BPU_gf2PolyExtEuclidA(&d, &s, out, a, m, m);

    // if GCD (a,m) is not 1
    if (d.len != 1 || d.elements[0] != 1ul) {
        BPU_printDebug("inverse polynomial NOT found");
        ret = 0;
    }

    // free
    BPU_gf2PolyFree(&d, 0);
    BPU_gf2PolyFree(&s, 0);
    return ret;
}

void BPU_gf2PolyTransp(BPU_T_GF2_Poly * out, const BPU_T_GF2_Poly * in) {
    int i;

    // allocate output poly
    BPU_gf2PolyMalloc(out, in->len);

    // copy zero coefficient
    BPU_gf2VecSetBit(out, 0, BPU_gf2VecGetBit(in, 0));

    // swap other coefficients
    for (i = 1; i < out->len; i++) {
        BPU_gf2VecSetBit(out, out->len - i, BPU_gf2VecGetBit(in, i));
    }
}

void BPU_gf2QcMatrixTransp(BPU_T_GF2_QC_Matrix * out,
                           const BPU_T_GF2_QC_Matrix * in) {
    int i;

    // allocate output matrix
    BPU_gf2QcMatrixMalloc(out, in->element_count, in->element_size,
                          !in->isVertical, 0);

    // transpose all polynoms
    for (i = 0; i < in->element_count; i++)
        BPU_gf2PolyTransp(&out->matrices[i], &in->matrices[i]);
}

int BPU_gf2PolyIsZero(const BPU_T_GF2_Poly * a) {
    int i;

    // scan all elements
    for (i = 0; i < a->array_length; i++)
        // if there is non zero element, poly is not zero
        if (a->elements[i] != 0ul)
            return 0;

    // all elements are zero, so also poly is zero
    return 1;
}
