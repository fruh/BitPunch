/**
* Here are defined basic types used by our McEliece implementation.
*/
#ifndef TYPES_H
#define TYPES_H

#include <stdint.h>

/**
* Binary representation GF2
*/
typedef uint32_t GF2;

/**
* Polynomial representation over GF2, max deg f < 8
*/
typedef uint8_t GF2_8x;

/**
* Polynomial representation over GF2, max deg f < 16
*/
typedef uint16_t GF2_16x;

/**
* Polynomial representation over GF2, max deg f < 32
*/
typedef uint32_t GF2_32x;

/**
* Polynomial representation over GF2, max deg f < 64
*/
typedef uint64_t GF2_64x;

/**
* Representation of permutation.
* Elements are numbers from 0 to size-1.
* Size corresponds to count of columns in matrix.
*/
typedef struct _Permutation_Vector {
	uint32_t *elements; ///< permutation vector
	uint32_t size; ///< permutation size
}Permutation_Vector;


/**
* Matrix representation over GF2.
* Every element is one bit.
*/
typedef struct _Matrix_GF2 {
	GF2 **elements; ///< all element of matrix
	uint8_t element_bit_size; ///< element size, is sizeof(GF2) i.e. 64 bits
	uint8_t elements_in_row; ///< number of elements in one row
	uint32_t k; ///< rows
	uint32_t n; ///< cols
}Matrix_GF2;

/**
 * Row vector GF2 representation.
 * Every element is one bit.
 */
typedef struct _Vector_GF2 {
	GF2 *elements; ///< all element of matrix
	uint8_t element_bit_size; ///< element size, is sizeof(GF2) i.e. 64 bits
	uint8_t elements_in_row; ///< number of elements in one row
	uint32_t len; ///< cols
}Vector_GF2;

typedef struct _Vector_GF2_16x {
	GF2_16x *elements;
	uint8_t len; ///< number of elements
}Vector_GF2_16x;

/**
* Matrix representation over GF2_8x
*/
typedef struct _Matrix_GF2_8x {
	GF2_8x **elements; ///< elements of matrix
	uint8_t k; ///< rows
	uint8_t n; ///< cols
}Matrix_GF2_8x;

/**
* Matrix representation over GF2_16x.
*/
typedef struct _Matrix_GF2_16x {
	GF2_16x **elements; ///< all element of matrix
	uint16_t k; ///< rows
	uint16_t n; ///< cols
}Matrix_GF2_16x;

/**
* Representation of polynomial.
*/
typedef struct _Polynomial_GF2_16x{
	GF2_16x *coef; ///< Polynomial over GF2m
	int16_t deg; ///< degree
	int16_t max_deg; ///< degree
}Polynomial_GF2_16x;

/**
* McEliece public key representation.
*/
typedef struct _McEliece_Pub_Key{
	Matrix_GF2 g_mat; ///< public key matrix
	uint8_t t; ///< number of errors
	uint8_t m; ///< galois finite field GF(2^m)
}McEliece_Pub_Key;

/**
* McEliece private key representation.
*/
typedef struct _McEliece_Priv_Key{
	Permutation_Vector permutation; ///< permutation "matrix"
	Polynomial_GF2_16x g;
	Matrix_GF2_16x h_mat; ///< Control matrix H
}McEliece_Priv_Key;

/**
* Representation of aritmetics data.
*/
typedef struct _Aritmetic_Data {
	GF2_16x *exp_table; ///< there are all elements referenced by i, so at i-th index is g^i element, g - generator
	uint32_t *log_table; ///< there are all indexes referenced by element, so alpha elemnet (g^i) -> i
	GF2_16x mod; ///< polynomial modulus
	int mod_deg; ///< modulo degree, galois finite field GF(2^m)
	int ord; ///< group ord, number of elements
}Arithmetic_Data;

/**
* Representation of McEliece context. All necessary structures, data should be here obtained.
*/
typedef struct _McEliece_Ctx {
	Arithmetic_Data a_data; ///< aritmetic date
	McEliece_Pub_Key pub_key; ///< public key
	McEliece_Priv_Key priv_key; ///< private key
	uint16_t max_pt_len_bit; ///< max plaintext len in bits
	uint16_t max_ct_len_bit; ///< max ciphertext len in bits
}McEliece_Ctx;

#endif // TYPES_H
