#ifndef PROCESS_H
#define PROCESS_H

#include "types.h"

/**
 * Makes CCA2 safe plaintext. out = r1 || gf2VecHashA(m || r2) NEED TO IMPLEMENT HASH FUNCTION!!!!
 * @param  r1 random (k-l)-bit vector r1
 * @param  r2 random (l)-bit vector r2
 * @param  in (l)-bit vector - plaintext
 * @param  out CCA2 safe plaintext
 * @return     0 - succes, else error
 */
int makeCCA2safePT(Vector_GF2 *r1, Vector_GF2 *r2, Vector_GF2 *in, Vector_GF2 *out);

/**
 * Add padding to message. Padding begins with 1. I t allocates ouput inside, so must be freed after use.
 * @param  padded_message[out] padded message
 * @param  message[in]        input message
 * @param  padding_len[in]    padding len
 * @return                0 on succes else, non-zero value
 */
/// Add padding to message.
int addPaddingA(Vector_GF2 *padded_message, const Vector_GF2 *message, const uint16_t padding_len);

/**
 * Delete padding from message. Padding begins with 1. I t allocates ouput inside, so must be freed after use.
 * @param  message[out]        output message without padding
 * @param  padded_message[in] input message with padding
 * @return                0 on succes else, non-zero value
 */
/// Delete padding from message.
int delPaddingA(Vector_GF2 *message, const Vector_GF2 *padded_message);

/**
 * Encrypt plaintext with CCA2 safe Kobara-Imai method. NEED TO IMPLEMENT HASH FUNCTION!!!!
 * @param  out    CCA2-safe cyphertext
 * @param  message  plaintext
 * @param  ctx  McEliece context
 * @return     0 - succes, else error
 */
int encryptCCA2KobaraImai(Vector_GF2 *out, const Vector_GF2 *message, const McEliece_Ctx *ctx);

/**
 * Encrypt plaintext with CCA2 safe Kobara-Imai method. NEED TO IMPLEMENT HASH FUNCTION!!!!
 * @param  out    CCA2-safe cyphertext, will be allocated inside function
 * @param  message  plaintext
 * @param  ctx  McEliece context
 * @return     0 - succes, else error
 */
int encryptCCA2KobaraImaiA(Vector_GF2 *out, const Vector_GF2 *message, const McEliece_Ctx *ctx);

/**
 * Encrypt plaintext with CCA2 safe Kobara-Imai method. NEED TO IMPLEMENT HASH FUNCTION!!!!
 * @param  out 		CCA2-safe cyphertext
 * @param  message 	plaintext
 * @param  ctx 	McEliece context
 * @return     0 - succes, else error
 */
int encrypt(Vector_GF2 *out, const Vector_GF2 *message, const McEliece_Ctx *ctx);

/**
 * Encrypt plaintext with CCA2 safe Kobara-Imai method. NEED TO IMPLEMENT HASH FUNCTION!!!!
 * @param  out 		CCA2-safe cyphertext, will be allocated inside function
 * @param  message 	plaintext
 * @param  ctx 	McEliece context
 * @return     0 - succes, else error
 */
int encryptA(Vector_GF2 *out, const Vector_GF2 *message, const McEliece_Ctx *ctx);

/**
 * Generate monic irreducible polynopmial of degree t. Polynomial p must be afreed after use.
 * @param p      [description]
 * @param t      [description]
 * @param a_data [description]
 */
void gf2xPolyGenGoppaA(Polynomial_GF2_16x *p, int t, const Arithmetic_Data *a_data);

/**
 * Find polynomials a, b of degree <= (t div 2). We are using GCD to find them, gcd(tau, mod) = a, so: a = tau * b + mod * (some s***s). Out a and b are allocated inside, after use must be preed using freePoly().
 * @param tau[in]    [description]
 * @param mod[in]    [description]
 * @param a[out]      polynomial is allocated inside, must be then freed using freePoly()
 * @param b[out]      polynomial is allocated inside, must be then freed using freePoly()
 * @param a_data[in] [description]
 */
/// Find polynomials a, b of degree <= (t div 2)
void findPolynomialsAB(const Polynomial_GF2_16x *tau, const Polynomial_GF2_16x *mod, Polynomial_GF2_16x *a, Polynomial_GF2_16x *b, const Arithmetic_Data *a_data);

int decrypt2(Vector_GF2 *cipher, Vector_GF2 *plain, const McEliece_Ctx *ctx);

int decrypt(Vector_GF2 *message, Vector_GF2 *cipher, const McEliece_Ctx *ctx);

int decryptA(Vector_GF2 *message, Vector_GF2 *cipher, const McEliece_Ctx *ctx);

int decryptCCA2KobaraImai(Vector_GF2 *message, const Vector_GF2 *cipher, const McEliece_Ctx *ctx);

int decryptCCA2KobaraImaiA(Vector_GF2 *message, const Vector_GF2 *cipher, const McEliece_Ctx *ctx);

/**
 * Decode message. Decoded must be allocated before use, or use function decodeA
 * @param encoded[in] encoded message
 * @param decoded[out] decoded message, allocated inside
 * @param ctx[out] McEliece context
 * 
 * @return still 0
 */
int decode(Vector_GF2 *encoded, Vector_GF2 *decoded, const McEliece_Ctx *ctx);

/**
 * Decode message. Decoded is allocated inside function, use freeVectorGF2() afer use.
 * @param encoded[in] encoded message
 * @param decoded[out] decoded message, allocated inside
 * @param ctx[out] McEliece context
 * 
 * @return succes - 0, else non-zero
 */
int decodeA(Vector_GF2 *encoded, Vector_GF2 *decoded, const McEliece_Ctx *ctx);

#endif // PROCESS_H