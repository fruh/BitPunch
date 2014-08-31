#include <stdlib.h>
#include <string.h>
#include <math.h>
#include <time.h>
#include <openssl/sha.h>
#include "arithmetics.h"
#include "globals.h"
#include "debugio.h"
#include "operations.h"
#include "init.h"

int gf2xGetDeg(GF2_64x poly) {
    int i = 63;

    while (i >= 0) {
        if (getBit(poly, i)) {
            return i;
        }
        i--;
    }
    return -1;
}

int gf2xPolyGetDeg(Polynomial_GF2_16x *poly) {
    int i = poly->max_deg;

    while (i >= 0) {
        if (poly->coef[i] != 0) {
            return i;
        }
        i--;
    }
    return -1;
}


int gf2VecHashA(Vector_GF2 *out, const Vector_GF2 *in, int len){
//sha512 output constant - SHA512_DIGEST_LENGTH
    const int DIGEST_LENGTH = 512;
    int i, element = -1;
    unsigned char* hashed = (unsigned char*) malloc(DIGEST_LENGTH/8);
    Vector_GF2 hash_vector;

    if(SHA512((unsigned char*)in->elements, in->elements_in_row, hashed) == NULL){
        printError("hash: SHA512");
        return -1;
    }

    if (mallocVectorGF2(&hash_vector, DIGEST_LENGTH) != 0) {
        printError("hash: mallocVectorGF2");
        return -2;
    }

    for (i = 0; i < hash_vector.len/8; i++) {
        if (i % (hash_vector.element_bit_size/8) == 0)
            element++;
        hash_vector.elements[element] ^= hashed[i] << ((i % (hash_vector.element_bit_size/8))*8);
    }
    gf2VecCropA(out, &hash_vector, 0, len);

    freeVecGF2(&hash_vector, 0);
    free(hashed);

    return 0;
}

int gf2xMatPermute(Matrix_GF2_16x *out, const Matrix_GF2_16x *m, const Permutation_Vector *permutation) {
    int i, j;

    // check if the size is correct
    if (m->n != permutation->size) {
        printError("gf2MatPermute: permutation size not correct m->n = %d, p->size = %d", m->n, permutation->size);

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

int gf2MatCopy(Matrix_GF2 *out, const Matrix_GF2 *in) {
    int i, j;

    if (out->k != in->k || out->n != in->n) {
        printError("gf2MatCopy: wrong matrix size");

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

int gf2MatPermute(Matrix_GF2 *m, const Permutation_Vector *permutation) {
    int i, j, act_element, shift;
    Matrix_GF2 new_matrix;

    // check if the size is correct
    if (m->n != permutation->size)
        return -1;

    // allocate new matrix
    if (mallocMatrixGF2(&new_matrix, m->k ,m->n) != 0)
        return -2;

    // copy the matrix
    gf2MatCopy(&new_matrix, m);

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
                m->elements[i][act_element] ^= gf2MatGetMaskedBit(&new_matrix, (uint64_t)i, (uint64_t)permutation->elements[j]) >> (shift); // permute the columns
            }
            else {
                shift = (j-permutation->elements[j]) % new_matrix.element_bit_size;
                m->elements[i][act_element] ^= gf2MatGetMaskedBit(&new_matrix, (uint64_t)i, (uint64_t)permutation->elements[j]) << (shift); // permute the columns
            }
        }
    }
    freeMatGF2(&new_matrix, 0);

    return 0;
}

int gf2VecPermute(Vector_GF2 *vec, const Permutation_Vector *permutation) {
    int i;
    Vector_GF2 tmp;

    mallocVectorGF2(&tmp, vec->len);

    for (i = 0; i < permutation->size; i++) {
        gf2VecSetBit(&tmp, i, gf2VecGetBit(vec, permutation->elements[i]));
    }
    gf2VecCopy(vec, &tmp);

    freeVecGF2(&tmp, 0);

    return 0;
}

GF2 gf2MatGetMaskedBit(const Matrix_GF2 *m, uint32_t row, uint32_t bit) {
    int segment, bit_in_segment;

    segment = bit / (m->element_bit_size);
    bit_in_segment = bit % (m->element_bit_size);
    
    return m->elements[row][segment] & ((uint64_t)1 << bit_in_segment);
}

GF2 gf2VecGetMaskedBit(const Vector_GF2 *vec, uint32_t bit) {
    int segment, bit_in_segment;

    segment = bit / (vec->element_bit_size);
    bit_in_segment = bit % (vec->element_bit_size);
    
    return vec->elements[segment] & ((uint64_t)1 << bit_in_segment);
}

int permGenA(Permutation_Vector* permutation, uint32_t size) {
    int i, j, tested;
    uint32_t rand_value;

    if (mallocPerm(permutation, size)) {
        return -1;
    }
    // // TEST: NSA
    // return 0;
    for (i = 0; i < size; i++) {
        while (1) {
            rand_value = getRandValue(0,size);
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

int gf2xMatConvertToGf2MatA(Matrix_GF2 *out, const Matrix_GF2_16x *m, int element_bit_size) {
    int i, j, bit, bit_in_element = -1, act_element = 0;

    // allocate memory for new matrix
    if (mallocMatrixGF2(out, m->k * element_bit_size, m->n)!= 0)
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
                out->elements[i*element_bit_size + bit][act_element] ^= (uint64_t)getBit(m->elements[i][j], bit) << (bit_in_element); // get bit from element and shift it
            }
        }
    }
    return 0;
}


int permGetInvA(Permutation_Vector *out, const Permutation_Vector *in) {
    int i;

    // allocate memory
    if (mallocPerm(out, in->size)) {
        printError("inverted permutation malloc error");

        return -1;
    }
    for (i = 0; i < in->size; i++) {
        out->elements[in->elements[i]] = i;
    }
    return 0;
}

int gf2MatTranspA(Matrix_GF2 *out, const Matrix_GF2 *in) {
    int i, j;

    if (mallocMatrixGF2(out, in->n, in->k)) {
        printError("gf2MatTranspA allocation error");

        return -1;
    }
    for (i = 0; i < in->k; i++) {
        for (j = 0; j < in->n; j++) {
            gf2MatSetBit(out, j, i, gf2MatGetBit(in, i, j));
        } // col loop
    } // row loop

    return 0;
}

void gf2Swap(GF2 *a, GF2 *b) {
    GF2 tmp;
    tmp = *a;
    *a = *b;
    *b = tmp;
}

void gf2MatSwapRows(Matrix_GF2 *mat, int i, int j) {
    int k;
    for (k = 0; k < mat->elements_in_row; k++) {
        gf2Swap(&(mat->elements[i][k]), &(mat->elements[j][k]));
    }
}



int gf2MatFindRow(const Matrix_GF2 *mat, int i, int start_index) {
    int k;
    for (k = start_index; k < mat->k; k++) {
        if ( gf2MatGetMaskedBit(mat, k, i) ){
            return k;
        }
    }
    return -1;
}

int gf2MatFindCol(const Matrix_GF2 *mat, int i, int start_index) {
    int k;
    for (k = start_index; k < mat->n; k++) {
        if ( gf2MatGetMaskedBit(mat, i, k) ){
            return k;
        }
    }
    return -1;
}

Permutation_Vector* gf2MatMakeSystematicA(Matrix_GF2 *inout) {
    int act_position = 0;
    int i;
    int row, col;
    Permutation_Vector perm;
    Permutation_Vector *permOut;
    permOut = (Permutation_Vector*) malloc (sizeof(Permutation_Vector));

    mallocPerm(&perm, inout->n);
    mallocPerm(permOut, inout->n);

    for (i = 0; i < perm.size; i++) {
        perm.elements[i] = i;
        permOut->elements[i] = i;
    }
#ifdef TEST_DET_EQ_CODE
    __test_was_eq_code = 0;
#endif
    
    for (act_position = 0; act_position < inout->k; act_position++) {
        row = gf2MatFindRow(inout, act_position, act_position);
        if (row == -1){
            printWarning("Not systematic");
            // TODO: find bug in this if branch
            #ifdef TEST_DET_EQ_CODE
                __test_was_eq_code = 1;
            #endif
                freePerm(permOut, 1);
                freePerm(&perm, 0);

                return NULL;

            row = act_position;
            col = gf2MatFindCol(inout, act_position, act_position);
            if (col == -1) {
                printError("unable to create systematic form");
                
                exit(-1);
            }
            gf2Swap(&(perm.elements[act_position]), &(perm.elements[col])); 
            // printf("swap col: %d\t%d\n", act_position, col);
            permPermute(permOut, &perm);
            // printPerm(permOut);
            gf2MatPermute(inout, &perm);
            gf2Swap(&(perm.elements[act_position]), &(perm.elements[col])); // reset temporary permutation
        }
        gf2MatSwapRows(inout, act_position, row);

        // xor with the rest of rows if needed
        for (i = 0; i < inout->k; i++) {
            if ( gf2MatGetMaskedBit(inout, i, act_position) && act_position != i) {
                gf2MatXorRows(inout, i, act_position);
            }
        }
    }
    freePerm(&perm, 0);
    return permOut;
}

int permPermute(Permutation_Vector *to_permute, const Permutation_Vector *permutation) {

    int i;
    Permutation_Vector new_permutation;
    // check if the size is correct
    if (to_permute->size != permutation->size)
        return -1;

    // allocate new permutation
    if (mallocPerm(&new_permutation, to_permute->size) != 0)
        return -2;

    // copy the permutation
    for (i = 0; i < to_permute->size; i++) {
        new_permutation.elements[i] = to_permute->elements[i];
    }

    // permute
    for (i = 0; i < permutation->size; i++) { // row loop
        to_permute->elements[i] = new_permutation.elements[permutation->elements[i]];
    }

    freePerm(&new_permutation, 0);

    return 0;
}

int gf2MatAppendIdenityA(Matrix_GF2 *out, const Matrix_GF2 *in) {
    int i, j;

    if (mallocMatrixGF2(out, in->k, in->n + in->k)) {
        printError("gf2MatAppendIdenityA: allocation error");

        return -1;
    }
    // first copy old rows
    for (i = 0; i < in->k; i++) {
        memcpy((void *) out->elements[i], (void *)in->elements[i], in->elements_in_row * (in->element_bit_size / 8));
    } // row loop
    // now append identity matrix
    for (i = 0; i < out->k; i++) {
            for (j = in->n; j < out->n; j++) {
                gf2MatSetBit(out, i, j, i == (j - in->n) ? 1 : 0);
        } // col loop
    } // row loop
    return 0;
}

int gf2VecConcat(Vector_GF2 *out, const Vector_GF2 *vec1, const Vector_GF2 *vec2) {
    int len = vec1->len + vec2->len;
    int i;

    if (out->len != len) {
        freeVecGF2(out, 0);
        mallocVectorGF2(out, vec1->len + vec2->len);
    }
    else {
        gf2VecNull(out);
    }
    // copy first vector
    gf2VecCopy(out, vec1);

    // copy second vector
    for (i = 0; i < vec2->len; i++) {
        gf2VecSetBit(out, i + vec1->len, gf2VecGetBit(vec2, i));
    }
    out->len = len;

    return 0;
}

int gf2VecConcatA(Vector_GF2 *out, const Vector_GF2 *vec1, const Vector_GF2 *vec2) {
    int len = vec1->len + vec2->len;

    if (mallocVectorGF2(out, len)) {
        printError("gf2VecConcat: allocation error");

        return -1;
    }
    return gf2VecConcat(out, vec1, vec2);
}

int gf2VecCropA(Vector_GF2 *out, const Vector_GF2 *in, const int start, const int length) {

    //input test
    if (start < 0 || (length+start) > in->len) {
        printError("cropVector: bad params");
        return -1;
    }

    //malloc output vector
    if (mallocVectorGF2(out, length)) {
        printError("cropVector: allocation error");
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
        gf2VecSetBit(out, counter, gf2VecGetBit(in, i));
        counter++;
    }

    return 0;
}

int gf2MatCropA(Matrix_GF2 *out, const Matrix_GF2 *in, uint16_t width) {
    int i, j, startElement;
    uint8_t cropBits;

    // if is new width lower than old width of matrix
    if (in->n >= width) {
        // allocate memory for new matrix
        if (mallocMatrixGF2(out, in->k, width)!= 0) {
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
        printError("gf2MatCropA in->n <= width, (%d <= %d)", in->n, width);

        return -1;
    }
}

int gf2MatGetRowAsGf2Vec(Vector_GF2 *out, const Matrix_GF2 *in, int row, int alloc) {
    if (!alloc && out->len != in->n) {
        printError("gf2MatGetRowAsGf2Vec: dimension is wrong out->len %d != in->n %d", out->len, in->n);

        return -1;
    }
    if (alloc) {
        if (mallocVectorGF2(out, in->n)) {
            printError("gf2MatGetRowAsGf2Vec: can not allocate vector");

            return -2;
        }
    }
    gf2MatCopyRowToVec(out, in, row);

    return 0;
}

int gf2xPolyExtEuclidA(Polynomial_GF2_16x *d, Polynomial_GF2_16x *s, Polynomial_GF2_16x *t, const Polynomial_GF2_16x *a, const Polynomial_GF2_16x *b, const int end_deg, const Arithmetic_Data *a_data) {
    Polynomial_GF2_16x tmp, tmp_2, old_s, old_t, old_r, r, q;
    GF2_16x inv_lead;
    int deg;

    deg = (a->deg > b->deg) ? a->deg : b->deg;
    
    // allocate GCD qoutient
    mallocPoly(s, deg);
    mallocPoly(t, deg);
    mallocPoly(d, deg);

    mallocPoly(&tmp, deg);
    mallocPoly(&tmp_2, deg);
    mallocPoly(&old_s, deg);
    mallocPoly(&old_t, deg);
    mallocPoly(&old_r, deg);
    mallocPoly(&r, deg);
    mallocPoly(&q, deg);

    gf2xPolyCopy(&r, b);
    gf2xPolyCopy(&old_r, a);

    if (a->deg == -1) {
        gf2xPolyCopy(&old_r, b);
        old_t.coef[0] = 1;
        old_t.deg = 0;
    }
    else if (b->deg == -1) {
        gf2xPolyCopy(&old_r, a);
        old_s.coef[0] = 1;
        old_s.deg = 0;
    }
    else {
        old_s.coef[0] = 1;
        old_s.deg = 0;

        t->coef[0] = 1;
        t->deg = 0;

        while (old_r.deg > end_deg && r.deg > -1) {   
            gf2xPolyDiv(&q, &tmp, &old_r, &r, a_data);

            // save old reminder
            gf2xPolyCopy(&old_r, &r);
            // save current reminder
            gf2xPolyCopy(&r, &tmp);

            // save s quocient
            gf2xPolyCopy(&tmp, &old_s);
            gf2xPolyCopy(&old_s, s);
            gf2xPolyMul(&tmp_2, &q, s, a_data);
            gf2xPolyAdd(s, &tmp, &tmp_2);

            // save t quocient
            gf2xPolyCopy(&tmp, &old_t);
            gf2xPolyCopy(&old_t, t);
            gf2xPolyMul(&tmp_2, &q, t, a_data);
            gf2xPolyAdd(t, &tmp, &tmp_2);
        }
    } 
    // prepare return values
    gf2xPolyCopy(d, &old_r);
    gf2xPolyCopy(s, &old_s);
    gf2xPolyCopy(t, &old_t);

    // make monic, if it is not
    inv_lead = gf2xPolyMakeMonic(d, a_data);

    if (inv_lead != 0) {
        gf2xPolyMulEl(s, inv_lead, a_data);
        gf2xPolyMulEl(t, inv_lead, a_data);
    }
    freePoly(&tmp, 0);
    freePoly(&tmp_2, 0);
    freePoly(&old_s, 0);
    freePoly(&old_t, 0);
    freePoly(&old_r, 0);
    freePoly(&r, 0);
    freePoly(&q, 0);

    return 0;
}

GF2_16x gf2xPolyEval(const Polynomial_GF2_16x *poly, const GF2_16x x, const Arithmetic_Data *a_data) {
    int i;
    GF2_16x ret = 0;
    ret = poly->coef[0];

    for (i = 1; i <= poly->deg; i++) {
        ret = ret ^ gf2xMulModT(poly->coef[i], gf2xPowerModT(x, i, a_data), a_data);
    }
    return ret;
}

uint32_t getRandValue(int from, int to) {
    if (from <= to){
        return rand()%(to-from)+from;
    }
    return 0;
}


int gf2xPolyCmp(const Polynomial_GF2_16x *p1, const Polynomial_GF2_16x *p2) {
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

int gf2xPolyIrredTest(const Polynomial_GF2_16x *p, const Arithmetic_Data *a_data) {
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
    Polynomial_GF2_16x tmp, out, qr, x, gcd, s, t, one;

    // test if some alpha is root
    for (i = 0; i < a_data->ord; i++) {
        if (gf2xPolyEval(p, a_data->exp_table[i], a_data) == 0) {
            is_irred = 0;
            return is_irred;
        }
    }
    mallocPoly(&out, 0);
    mallocPoly(&one, 0);
    mallocPoly(&qr, 0);
    mallocPoly(&tmp, 1);
    mallocPoly(&x, 1);
    // gcd, s, t, will be allocated inside gf2xPolyExtEuclidA

    // set tmp polynomial tmp(x) = x
    tmp.coef[0] = 0;
    tmp.coef[1] = 1;
    tmp.deg = gf2xPolyGetDeg(&tmp);

    x.coef[0] = 0;
    x.coef[1] = 1;
    x.deg = gf2xPolyGetDeg(&x);

    one.coef[0] = 1;
    one.deg = 0;

    // simplify polynomial with big coeffitient
    for (i = 0; i < exponent; i++) {
        gf2xPolyMul(&out, &tmp, &tmp, a_data);
        gf2xPolyMod(&tmp, &out, p, a_data);
    }
    gf2xPolyAdd(&qr, &tmp, &x);
    gf2xPolyExtEuclidA(&gcd, &s, &t, &qr, p, -1, a_data);

    if (gf2xPolyCmp(p, &gcd) != 1) {
        freePoly(&out, 0);
        freePoly(&one, 0);
        freePoly(&qr, 0);
        freePoly(&tmp, 0);
        freePoly(&x, 0);
        freePoly(&gcd, 0);
        freePoly(&s, 0);
        freePoly(&t, 0);

        is_irred = 0;
        return is_irred;
    }

    for (j = 2; j <= p->deg; j++) {
        if (p->deg % j != 0 || isPrime(j) == 0)
            continue;

        gf2xPolyNull(&tmp);
        tmp.coef[0] = 0;
        tmp.coef[1] = 1;
        tmp.deg = gf2xPolyGetDeg(&tmp);
        
        exponent = m*(n/j);

        for (i = 0; i < exponent; i++) {
            gf2xPolyMul(&out, &tmp, &tmp, a_data);
            gf2xPolyMod(&tmp, &out, p, a_data);
        }
        freePoly(&gcd, 0);
        freePoly(&s, 0);
        freePoly(&t, 0);

        gf2xPolyExtEuclidA(&gcd, &s, &t, &tmp, p, -1, a_data);

        if (gf2xPolyCmp(&one, &gcd) != 1) {
            is_irred = 0;
            break;
        }
    }
    freePoly(&out, 0);
    freePoly(&one, 0);
    freePoly(&qr, 0);
    freePoly(&tmp, 0);
    freePoly(&x, 0);
    freePoly(&gcd, 0);
    freePoly(&s, 0);
    freePoly(&t, 0);

    return is_irred;
}

void gf2xPolyCopy(Polynomial_GF2_16x *dest, const Polynomial_GF2_16x *src) {
    // if there is not enough space resize
    if (dest->max_deg < src->max_deg) {
        freePoly(dest, 0);
        mallocPoly(dest, src->max_deg);
    }
    else {
        gf2xPolyNull(dest);
    }
    memcpy((void *) (dest->coef), (void *) (src->coef), sizeof(GF2_16x) * (src->max_deg + 1));
    
    dest->deg = src->deg;
}

void gf2VecCopy(Vector_GF2 *dest, const Vector_GF2 *src) {
    // if there is not enough space resize
    if (dest->elements_in_row < src->elements_in_row) {
        freeVecGF2(dest, 0);

        mallocVectorGF2(dest, src->elements_in_row * src->element_bit_size * sizeof(GF2));
    }
    else {
        gf2VecNull(dest);
    }
    memcpy((void *) (dest->elements), (void *) (src->elements), sizeof(GF2) * (src->elements_in_row));
    
    dest->len = src->len;
}

void gf2xPolyInvA(Polynomial_GF2_16x *out, const Polynomial_GF2_16x *a, const Polynomial_GF2_16x *mod, const Arithmetic_Data *a_data) {
    Polynomial_GF2_16x d, t;
    
    gf2xPolyExtEuclidA(&d, out, &t, a, mod, 0, a_data);

    if (d.deg != 0 || d.coef[0] != 1) {
        printDebug("inverse polynomial NOT found");
        printError("degree: %d\nelement: %d\n", d.deg, d.coef[0]);
        printGf2xPoly(out, a_data);
        gf2xPolyNull(out);
    }
    freePoly(&d, 0);
    freePoly(&t, 0);
}

GF2_16x gf2xPolyMakeMonic(Polynomial_GF2_16x *a, const Arithmetic_Data *a_data) {
    GF2_16x inv_lead = 0;

    // if it is already monic do nothing
    if ((a->deg > -1 && a->coef[a->deg] == 1) || a->deg == -1) {
        return inv_lead;
    }
    inv_lead = gf2xInvElement(gf2xPolyLeadCoef(a), a_data);
    gf2xPolyMulEl(a, inv_lead, a_data);

    return inv_lead;
}

int isPrime(int n) {
    int i;
    if (n == 2)
        return 1;
    for (i = 2; i <= n/2.0; i++) {   //TODO: import math.h and use sqrt(n)
        if (n % i == 0)
            return 0;
    }

    return 1;
}

int gf2VecToPoly(Vector_GF2 *in, Polynomial_GF2_16x *out, uint8_t ord) {
    if (mallocPoly(out, in->len / ord + 1) != 0) {
        printError("gf2VecToPoly: mallocPoly");
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

        out->coef[actual_coef] ^= gf2VecGetBit(in,i) << (i%ord);
        //fprintf(stderr, "i:%d  adding:%d coef", i,gf2VecGetBit(in,i));
        //printBinaryLe(out->coef[actual_coef], ord);
        //fprintf(stderr, "\n");
    }

    out->deg = gf2xPolyGetDeg(out);

    return 0;
}

void gf2xMatDetermineSyndromeA(Polynomial_GF2_16x *syndrome, const Vector_GF2 *z, const Matrix_GF2_16x *H, const Arithmetic_Data *a_data) {
    int i, j;
    mallocPoly(syndrome, H->k - 1);
    gf2xPolyNull(syndrome);
    for (i = 0; i < z->len; i++) {
        if (gf2VecGetBit(z, i)) {
            for (j = 0; j < H->k; j++) {
                syndrome->coef[syndrome->max_deg - j] ^= H->elements[j][i];
            }
        }
    }
    syndrome->deg = gf2xPolyGetDeg(syndrome);  
}

GF2_16x gf2xGetPseudoInv(const GF2_16x a, const GF2_16x res, const Arithmetic_Data *a_data) {
    return gf2xMulModT(gf2xInvElement(a, a_data), res, a_data);
}

void gf2xMatNull(Matrix_GF2_16x *mat) {
    int i, j;
    for (i = 0; i < mat->k; i++) {
        for (j = 0; j < mat->n; j++) {
            mat->elements[i][j] = 0;
        }
    }
}

void gf2xMatInsertPoly(Matrix_GF2_16x *mat, const Polynomial_GF2_16x *poly, int i)  {
    int j;
    for (j = 0; j <= poly->deg; j++) {
        mat->elements[i][j] = poly->coef[j];
    }
}

void gf2xPolyToVecA(Vector_GF2_16x *vec, const Polynomial_GF2_16x *poly, int len) {
    int i;

    if (poly->deg >= len) {
        printError("dimension missmatch");

        exit(-1);
    }
    mallocVectorGF2_16x(vec, len);

    for (i = 0; i <= poly->deg; i++) {
        vec->elements[i] = poly->coef[i];
    }
}

void gf2xVecToPoly(Polynomial_GF2_16x *poly, const Vector_GF2_16x *vec) {
    int i;
    
    for (i = 0; i < vec->len; i++) {
        poly->coef[i] = vec->elements[i];
    }
    poly->deg = gf2xPolyGetDeg(poly);
}

void gf2xSwap(GF2_16x *a, GF2_16x *b) {
    GF2_16x tmp;
    tmp = *a;
    *a = *b;
    *b = tmp;
}

void gf2xMatSwapRows(Matrix_GF2_16x *mat, int i, int j) {
    int k;
    for (k = 0; k < mat->n; k++) {
        gf2xSwap(&(mat->elements[i][k]), &(mat->elements[j][k]));
    }
}

void gf2xMatMulElRow(Matrix_GF2_16x *mat, const GF2_16x element, int row, const Arithmetic_Data *a_data) {
    int i;
    for (i = 0; i < mat->n; i++) {
        mat->elements[row][i] = gf2xMulModT(element, mat->elements[row][i], a_data);
    }
}

int gf2xMatFindPivot(const Matrix_GF2_16x *mat, int index) {
    int i;
    for (i = index; i < mat->k; i++) {
        if(mat->elements[i][index] != 0) {
            return i;
        }
    }
    return -1;
}

void gf2xVecMulEl(Vector_GF2_16x *vec, GF2_16x element, const Arithmetic_Data *a_data) {
    int i;
    for (i = 0; i < vec->len; i++) {
        vec->elements[i] = gf2xMulModT(element, vec->elements[i], a_data);
    }
}

void gf2xMatXorRows(Matrix_GF2_16x *mat, int index, int j, const Arithmetic_Data *a_data) {
    int k;
    GF2_16x element;
    Vector_GF2_16x tmp;

    mallocVectorGF2_16x(&tmp, mat->n);
    for (k = 0; k < tmp.len; k++) {
        tmp.elements[k] = mat->elements[index][k];
    }
    element = mat->elements[j][index];

    gf2xVecMulEl(&tmp, element, a_data);

    for (k = 0; k < tmp.len; k++) {
        mat->elements[j][k] = mat->elements[j][k] ^ tmp.elements[k];
    }
    freeVectorGF2_16x(&tmp, 0);
}

void gf2xMatClearCol(Matrix_GF2_16x *mat, int index, const Arithmetic_Data *a_data) {
    int i;
    for (i = 0; i < mat->k; i++) {
        if (i == index) {
            continue;
        }
        gf2xMatXorRows(mat, index, i, a_data);
    }
}

void gf2xMatGEM(Matrix_GF2_16x *mat, const Arithmetic_Data *a_data) {
    int i, pivot;
    GF2_16x element;
    for (i = 0; i < mat->k; i++) {
        pivot = gf2xMatFindPivot(mat, i);
        if (pivot == -1)
            printError("################# unbeliviable 'badness' ###########");
        if (pivot != i) {
            gf2xMatSwapRows(mat, i, pivot);
        }
        if (mat->elements[i][i] != 1) {
            element = gf2xPowerModT(mat->elements[i][i], -1, a_data);
            gf2xMatMulElRow(mat, element, i, a_data);
        }
        gf2xMatClearCol(mat, i, a_data);
    }
}

int permIsValid(const Permutation_Vector *p) {
    int i, j;

    for (i = 0; i < p->size; i++) {
        for (j = 0; j < p->size; j++) {
            if (i != j && p->elements[i] == p->elements[j]) {
                printError("permutation is not valid");
                printPerm(p);

                return 1;
            }
        }
    }
    return 0;
}

int gf2VecCmp(const Vector_GF2 *v1, const Vector_GF2 *v2) {
    int i;

    if (v1->len != v2->len) {
        return 1;
    }
    for (i = 0; i < v1->len; i++) {
        if (gf2VecGetBit(v1, i) != gf2VecGetBit(v2, i)) {
            return 2;
        }
    }
    return 0;
}

void gf2xPolyGenRandom(Polynomial_GF2_16x *p, int t, const Arithmetic_Data *a_data) {
    int i;
    p->coef[t] = 1;
    do {
        p->coef[0] = getRandValue(1, a_data->ord);
    } while (p->coef[0] == 0);
    
    for (i = 1; i < t; i++) {
        p->coef[i] = getRandValue(0, a_data->ord);
    }
    p->deg = t;
}