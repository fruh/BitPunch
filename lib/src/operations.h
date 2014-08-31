#ifndef OPERATIONS_H
#define OPERATIONS_H

#include <string.h>
#include "types.h"

/**
 * Check if is set bit at n-th index makro.
 * @param w bit word to check
 * @param n index of bit to check, starts from 0
 * @return 1 - is set, else zero
 */
 /// Check if is set bit at n-th index.
#define getBit(w, n) ((int)((((uint64_t)w) >> (n)) & 1ul))

/**
 * Check if is set bit at s, t position in matrix.
 * @param m_pointer GF2 matrix address
 * @param s is row
 * @param t is col
 * @return 1 - is set, else zero
 */
 /// Check if is set bit at s, t position in matrix.
#define gf2MatGetBit(m_pointer, s, t) (getBit((m_pointer)->elements[s][(t) / (m_pointer)->element_bit_size], (t) % (m_pointer)->element_bit_size))

/**
* Check if is set bit at s, t position in vector.
* @param v_pointer GF2 vector address
* @param t is col
* @return 1 - is set, else zero
*/
/// Check if is set bit at i-th position in vector.
#define gf2VecGetBit(v_pointer, i) (getBit((v_pointer)->elements[(i) / (v_pointer)->element_bit_size], (i) % (v_pointer)->element_bit_size))

/**
 * Set bit in matrix GF2 at index s, t
 * @param  m_pointer address to matrix GF2
 * @param  s         row
 * @param  i         col index
 * @param  bit       bit value 0 or 1
 */
#define gf2MatSetBit(m_pointer, s, t, bit)  if (bit) { \
 												(m_pointer)->elements[s][t / (m_pointer)->element_bit_size] |= ((GF2) 1) << ((t) % (m_pointer)->element_bit_size);\
 										    } \
 										    else { \
 										    	/* this is like: 00101111 ^ 00000100 = 00101011 */\
 										    	(m_pointer)->elements[s][(t) / (m_pointer)->element_bit_size] &= ((GF2) (0xFFFFFFFFFFFFFFFFul)) ^ (((GF2) 1) << ((t) % (m_pointer)->element_bit_size));\
 										    }

/**
 * Set bit in vector GF2 at index i
 * @param  v_pointer address to matrix GF2
 * @param  i         col index
 * @param  bit       bit value 0 or 1
 */
#define gf2VecSetBit(v_pointer, i, bit)  if (bit) { \
 												(v_pointer)->elements[(i) / (v_pointer)->element_bit_size] |= ((GF2) 1) << ((i) % (v_pointer)->element_bit_size);\
 										    } \
 										    else { \
 										    	/* this is like: 00101111 ^ 00000100 = 00101011 */\
 										    	(v_pointer)->elements[(i) / (v_pointer)->element_bit_size] &= ((GF2) (0xFFFFFFFFFFFFFFFFul)) ^ (((GF2) 1) << ((i) % (v_pointer)->element_bit_size));\
 										    }

/**
 * Null GF2 matrix row.
 * @param[out]  m_pointer pointer to GF2 matrix
 * @param[in]  row       row to null
 */
/// Null GF2 matrix row.
#define gf2MatNullRow(m_pointer, row) memset((void *) ((m_pointer)->elements[row]), 0, (m_pointer)->element_bit_size / 8 * (m_pointer)->elements_in_row)

/**
 * Null GF2 vector.
 * @param[out]  v_pointer pointer to GF2 vector
 */
/// Null GF2 vector.
#define gf2VecNull(v_pointer) memset((void *) ((v_pointer)->elements), 0, (v_pointer)->element_bit_size / 8 * (v_pointer)->elements_in_row)

/**
 * Copy Matrix GF2 row to Vector GF2.
 * @param v_pointer[out] pointer to GF2 vector	
 * @param m_pointer[in] pointer to GF2 matrix	
 * @param row[in] row index
 */
 /// Copy Matrix GF2 row to Vector GF2.
#define gf2MatCopyRowToVec(v_pointer, m_pointer, row) memcpy((void *) ((v_pointer)->elements), (void *) ((m_pointer)->elements[row]), (v_pointer)->element_bit_size / 8 * (v_pointer)->elements_in_row)

/**
 * Set Polynomial values to 0.
 * @param d_pointer[out] pointer to GF2_16x polynomial		
 */
 /// Copy Polynomial.
#define gf2xPolyNull(d_pointer) memset((void *) ((d_pointer)->coef), 0, sizeof(GF2_16x)*((d_pointer)->max_deg + 1));\
 	(d_pointer)->deg = -1

/**
 * Get inverse element of galois field.
 * @param  gf2_16x_e find inverse element to this element
 * @param  a_data_p pointer to Arithmetic_Data structure
 * @return inverse element
 */
/// Get inverse element of galois field.
#define gf2xInvElement(gf2_16x_e, a_data_p) ((gf2_16x_e) == 1 ? 1 : (a_data_p)->exp_table[((a_data_p)->ord - (a_data_p)->log_table[gf2_16x_e])])

/**
 * Get leading coefitient of Polynomal over GF2x.
 * @param poly_gf2_16x_p pointer to Polynomial_GF2_16x structure
 * @return leading coef (element from GF2^16) or 0
 */
/// * Get leading coefitient of Polynomal over GF2x.
#define gf2xPolyLeadCoef(poly_gf2_16x_p) ((poly_gf2_16x_p)->deg > -1 ? (poly_gf2_16x_p)->coef[(poly_gf2_16x_p)->deg] : 0)

int gf2MatCopy(Matrix_GF2 *out, const Matrix_GF2 *in);

/**
 * Copy Polynomial.
 * @param dest[out] pointer to GF2_16x polynomial	
 * @param src[in] pointer to GF2_16x polynomial	
 */
 /// Copy Polynomial.
void gf2xPolyCopy(Polynomial_GF2_16x *dest, const Polynomial_GF2_16x *src);

/**
 * Copy VectorGF2.
 * @param dest[out] pointer to VectorGF2
 * @param src[in] pointer to VectorGF2	
 */
 /// Copy VectorGF2.
void gf2VecCopy(Vector_GF2 *dest, const Vector_GF2 *src);

/**
* Get degree of polynomial over GF2.
* @param pol input polynomial
* @return degree of pol or -1 if it is pol = 0
*/
/// Get degree of polynomial over GF2
int gf2xGetDeg(GF2_64x pol);

/**
* Get degree of polynomial over GF2x.
* @param pol input polynomial
* @return degree of pol or -1 if it is pol = 0
*/
/// Get degree of polynomial over GF2x
int gf2xPolyGetDeg(Polynomial_GF2_16x *poly);

/**
 * Permute matrix GF2^m with permutation.
 * @param m matrix GF2^m
 * @param permutation permutation vector
 * @return on succes 0, on size error -1, on allocation error -2
 */
int gf2xMatPermute(Matrix_GF2_16x *out, const Matrix_GF2_16x *m, const Permutation_Vector *permutation);

/**
 * Permute matrix GF2 with permutation.
 * @param m matrix GF2
 * @param permutation permutation vector
 * @return on succes 0, on size error -1, on allocation error -2
 */
int gf2MatPermute(Matrix_GF2 *m, const Permutation_Vector *permutation);

/**
 * Permute vector GF2 with permutation.
 * @param vec vector GF2
 * @param permutation permutation vector
 * @return on succes 0, on size error -1, on allocation error -2
 */
int gf2VecPermute(Vector_GF2 *vec, const Permutation_Vector *permutation);

/**
 * Get masked bit from exact row of Matrix GF2
 * @param m matrix GF2
 * @param row row
 * @param bit bit
 * @return masked bit
 */
GF2 gf2MatGetMaskedBit(const Matrix_GF2 *m, uint32_t row, uint32_t bit);

/**
 * Get masked bit from Vector GF2
 * @param vec vector GF2
 * @param bit bit
 * @return masked bit
 */
GF2 gf2VecGetMaskedBit(const Vector_GF2 *vec, uint32_t bit);

/**
 * Generate random permutation, with structure allocation of given size.  After work you have to free memory using call freePerm.
 * @param permutation pointer to structure Permutation_Vector 
 * @param size size of permutation vector
 * @return on succes 0, else error
 */
int permGenA(Permutation_Vector* permutation, uint32_t size);

/**
 * Converts matrix GF2m to matrix GF2. Allocates memory for GF2 matrix.  After work you have to free memory using call freeMatGF2.
 * @param m matrix GF2m
 * @param deg degree of elements from GF2m
 * @param out output matrix GF2
 * @return on succes 0, else error
 */
int gf2xMatConvertToGf2MatA(Matrix_GF2 *out, const Matrix_GF2_16x *m, int deg);

/**
 * Get inverse permutation to permutation. It allocates new permutation elements using malloc. After work you have to free memory using call freePerm.
 * @param  out inverted permutation
 * @param  in  permutation
 * @return     on succes 0, else error
 */
int permGetInvA(Permutation_Vector *out, const Permutation_Vector *in);

/**
 * Transpose GF2 Matrix. After work you have to free memory using call freeMatGF2.
 * @param  out transposed GF2 matrix
 * @param  in  matrix to transpose
 * @return     on success 0, else error
 */
int gf2MatTranspA(Matrix_GF2 *out, const Matrix_GF2 *in);

/**
 * Swaps GF2 elements.
 * @param a pointer to GF2 element
 * @param b pointer to GF2 element 
 * @return
 */
void gf2Swap(GF2 *a, GF2 *b);

/**
 * It swaps i-th and j-th rows of Matrix GF2.
 * @param  mat Matrix GF2
 * @param  int i
 * @param  int j
 * @return permutation needed for equivalent codes
 */
void gf2MatSwapRows(Matrix_GF2 *mat, int i, int j);

/**
 * Finds row in Matrix GF2 where 1 is at i-th position (from left). Looking for row starts at start_index.
 * @param  mat Matrix GF2
 * @param  i int
 * @param  start_index int
 * @return number of row on success, else -1 
 */
int gf2MatFindRow(const Matrix_GF2 *mat, int i, int start_index);

/**
 * Finds column in Matrix GF2 where 1 is at i-th position (from top). Looking for column starts at start_index.
 * @param  mat Matrix GF2
 * @param  i int
 * @param  start_index int
 * @return number of row on success, else -1 
 */
int gf2MatFindCol(const Matrix_GF2 *mat, int i, int start_index);

/**
 * It brings Matrix GF2 into the systematic form -> with I on the left side. After work, return value should be freed, it is allocated dynamically.
 * @param  inout Matrix GF2
 * @return       on success pointer to Permutation_Vector
 */
/// It brings Matrix GF2 into the systematic form -> with I on the left side.
Permutation_Vector* gf2MatMakeSystematicA(Matrix_GF2 *inout);

/**
 * Compose permutations.
 * @param  to_permute  row vector to be permuted
 * @param  permutation
 * @return 0 success, -1 when size of vectors is not the same, -2 on memory allocation error 
 */
int permPermute(Permutation_Vector *to_permute, const Permutation_Vector *permutation);

/**
 * Append identity matrix, so out matrix should be (M | I). After work should be called freeMatGF2.
 * @param  out oupput matrix of size k x (n + k), will be allocated
 * @param  in  input matrix of size k x n
 * @return     0 - succes, else error
 */
/// Append identity matrix, so out matrix should be (M | I).
int gf2MatAppendIdenityA(Matrix_GF2 *out, const Matrix_GF2 *in);

/**
 * Concats two vectors it allocates ouptu inside.
 * @param  vec1 first vector
 * @param  vec2 second vector
 * @param  out 	output vector - concat of vec1 and vec2, it allocates vector inside function, should be freed after use freeVecGF2()
 * @return    0 - succes, else error
 */
/// Concats two vectors it allocates ouptu inside.
int gf2VecConcatA(Vector_GF2 *out, const Vector_GF2 *vec1, const Vector_GF2 *vec2);

/**
 * Concats two vectors without allocation ouput.
 * @param  vec1 first vector
 * @param  vec2 second vector
 * @param  out 	output vector - concat of vec1 and vec2, it allocates vector inside function, should be freed after use freeVecGF2()
 * @return    0 - succes, else error
 */
/// Concats two vectors without allocation ouput.
int gf2VecConcat(Vector_GF2 *out, const Vector_GF2 *vec1, const Vector_GF2 *vec2);

/**
 * Crop the vector
 * @param  out 	  output vector
 * @param  in 	  input vector
 * @param  start  starting position in vector
 * @param  length count of bits of cropped vector
 * @return    0 - succes, else error
 */
int gf2VecCropA(Vector_GF2 *out, const Vector_GF2 *in, const int start, const int length);

/**
 * Crop matrix GF2 from left.  After work you have to free memory using call freeMatGF2.
 * @param in input matrix GF2
 * @param out croped output matrix GF2 
 * @param width width of cropped matrix
 * @return on succes 0, on higher or equal width as actual width of in matrix -1, on allocation of new matrix error -2
 */
/// Crop matrix GF2 from left.
int gf2MatCropA(Matrix_GF2 *out, const Matrix_GF2 *in, uint16_t width);

/**
 * Get matrix GF2 row as vector gf2. You can set alloc param, to allocate it dynamically inside function, or use allocated vector yet. When set alloc, after work call freeVecGF2
 * @param  out   vector
 * @param  in    matrix
 * @param  row   row index
 * @param  alloc if set to 0, jsut copy row to allocated vector yet, or also allocate output vector
 * @return       0 - success, else error
 */
int gf2MatGetRowAsGf2Vec(Vector_GF2 *out, const Matrix_GF2 *in, int row, int alloc);

/**
 * Evaluate polynomial over GF2^m with x.
 * @param a_data pointer to Aritmetic_Data structure, used by calculations
 * @param poly polynomial to evaluate over GF2x
 * @param x value to evaluate, GF2x element
 * @return evaluated polynomial, GF2x element
 */
/// Evaluate polynomial over GF2^m with x.
GF2_16x gf2xPolyEval(const Polynomial_GF2_16x *poly, const GF2_16x x, const Arithmetic_Data *a_data);

/**
 * Get random unsigned int 32 value from given range (from <= return <= to)
 * @param from minimum
 * @param to maximum
 * @return random value
 */
/// Get random unsigned int 32 value from given range (from <= return <= to)
uint32_t getRandValue(int from, int to);

/**
 * Extended euclidian to find greatest common divisor and Bézout coefficients s, t, where gcd(a, b) = d = a*s + b*t.
 * You can specify degegree, when will it ends, deg(d) > end_deg. Polynomials d, s, t will be allocated inside function. After work should be freed using freePoly().
 * @param  d[out] greatest common divisor
 * @param  s[out] Bézout coefficients
 * @param  t[out] Bézout coefficients
 * @param  a[in] input polynomial
 * @param  b[in] input polynomial
 * @param  end_deg[in] it will ends, when deg(devider) <= end_deg
 * @param a_data[in] arithmetic data structure, initialized galois field
 * @return  0
 */
/// Extended Euclidean to find greatest common divisor and Bézout coefficients s, t, where gcd(a, b) = d = a*s + b*t.
int gf2xPolyExtEuclidA(Polynomial_GF2_16x *d, Polynomial_GF2_16x *s, Polynomial_GF2_16x *t, const Polynomial_GF2_16x *a, const Polynomial_GF2_16x *b, int end_deg, const Arithmetic_Data *a_data);

/**
 * Function compares two polynomials
 * @param  p1 [description]
 * @param  p2 [description]
 * @return    0 if different, 1 otherwise.
 */
int gf2xPolyCmp(const Polynomial_GF2_16x *p1, const Polynomial_GF2_16x *p2);

/**
 * Function tests if polynomial is irreducible or not.
 * @param  p      [description]
 * @param  a_data [description]
 * @return        [description]
 */
int gf2xPolyIrredTest(const Polynomial_GF2_16x *p, const Arithmetic_Data *a_data);
/**
 * Get inverse polynomial over GF2_16x. It is using Extended Euclidean alg. Polynomial out will be allocated inside function. After work should be freed using freePoly().
 * @param a[in]    input polynomial
 * @param mod[in]  modulus polynomial
 * @param out[out] output polynomial
 * @param a_data[in] arithmetic data structure
 */
/// Get inverse polynomial over GF2_16x.
void gf2xPolyInvA(Polynomial_GF2_16x *out, const Polynomial_GF2_16x *a, const Polynomial_GF2_16x *mod, const Arithmetic_Data *a_data);

/**
 * Make from polynomial monic polynomial. It means that it will be multiplicated with (leading coef)^-1. It will change source polynomial.
 * @param  a      polynomial to be monic
 * @param  a_data arithmetic data structure
 * @return        inverse leading coefitient or 0
 */
/// Make from polynomial monic polynomial.
GF2_16x gf2xPolyMakeMonic(Polynomial_GF2_16x *a, const Arithmetic_Data *a_data);

/**
 * Check if number is prime number.
 * @param  n [description]
 * @return   [description]
 */
int isPrime(int n);

/**
 * Hash function with output digest length of 512bits
 * @param  in   input vector to hash
 * @param  out 	output hashed vector of 512 bits
 * @return        	0 - success, else error
 */
int gf2VecHashA(Vector_GF2 *out, const Vector_GF2 *in, int len);

void gf2xMatNull(Matrix_GF2_16x *mat);

/**
 * Insert coefitients from poly into matrix mat to i-th row
 * @param poly [description]
 * @param mat  [description]
 * @param i    [description]
 */
void gf2xMatInsertPoly(Matrix_GF2_16x *mat, const Polynomial_GF2_16x *poly, int i);

/**
 * save poly coefitients to vector, it allocates vec inside function. After use use freeVectorGF2_16x
 * @param poly [description]
 * @param vec  [description]
 */
void gf2xPolyToVecA(Vector_GF2_16x *vec, const Polynomial_GF2_16x *poly, int len);

/**
 * GEM applied on matrix with coefitients from GF2^m
 * @param mat    [description]
 * @param a_data [description]
 */
void gf2xMatGEM(Matrix_GF2_16x *mat, const Arithmetic_Data *a_data);

/**
 * Swap
 * @param a [description]
 * @param b [description]
 */
void gf2xSwap(GF2_16x *a, GF2_16x *b);

/**
 * in matrix mat swap rows i and j
 * @param mat [description]
 * @param i   [description]
 * @param j   [description]
 */
void gf2xMatSwapRows(Matrix_GF2_16x *mat, int i, int j);

/**
 * Multiply whole row of matrix mat by element from finite field
 * @param mat     [description]
 * @param element [description]
 * @param i       [description]
 * @param a_data  [description]
 */
void gf2xMatMulElRow(Matrix_GF2_16x *mat, const GF2_16x element, int i, const Arithmetic_Data *a_data);

/**
 * find row where pivot is at index index
 * @param  mat   [description]
 * @param  index [description]
 * @return       [description]
 */
int gf2xMatFindPivot(const Matrix_GF2_16x *mat, int index);

/**
 * Get GF2_16x element which is a * return = res.
 * @param  a   input element
 * @param  res input element
 * @return     element a * return = res
 */
/// Get GF2_16x element which is a * return = res.
GF2_16x gf2xGetPseudoInv(const GF2_16x a, const GF2_16x res, const Arithmetic_Data *a_data);

/**
 * If at mat->elements[j][index] is not zero, then
 * mat->row[j] = mat->row[j] ^ (element * (mat->row[index]))
 * @param mat [description]
 * @param index   [description]
 * @param j   [description]
 */
void gf2xMatXorRows(Matrix_GF2_16x *mat, int index, int j, const Arithmetic_Data *a_data);

/**
 * vec = element*vec
 * @param vec     [description]
 * @param element [description]
 * @param a_data  [description]
 */
void gf2xVecMulEl(Vector_GF2_16x *vec, GF2_16x element, const Arithmetic_Data *a_data);

void gf2xVecToPoly(Polynomial_GF2_16x *poly, const Vector_GF2_16x *vec);

void gf2xMatDetermineSyndromeA(Polynomial_GF2_16x *syndrome, const Vector_GF2 *z, const Matrix_GF2_16x *H, const Arithmetic_Data *a_data);

void gf2xMatClearCol(Matrix_GF2_16x *mat, int index, const Arithmetic_Data *a_data);

int permIsValid(const Permutation_Vector *p);

int gf2VecCmp(const Vector_GF2 *v1, const Vector_GF2 *v2);

/**
 * Generate monic polynomial of degree t and with non-zero coefficient a_0
 * @param p      [description]
 * @param t      [description]
 * @param a_data [description]
 */
void gf2xPolyGenRandom(Polynomial_GF2_16x *p, int t, const Arithmetic_Data *a_data);

#endif // OPERATIONS_H
