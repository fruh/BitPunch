#ifndef MCELLIECE_H
#define MCELLIECE_H

#include "types.h"
#include "init.h"

/**
 * Initialize McEliece context to be able working with it.
 * @param  ctx  pointer to McEliece_Ctx structure, it is a context
 * @param  t_errors number of errors, i.e. 50
 * @return          0 on succes, else non-zero value
 */
/// Initialize McEliece context to be able working with it
int mceInitCtx(McEliece_Ctx *ctx, uint8_t t_errors);

/**
 * Key generation, first must be initialized context using mceInitCtx().
 * @param  ctx McEliece context
 * @return     0 - success, else error
 */
/// Key generation, first must be initialized context using mceInitCtx.
int mceGenKeyPair(McEliece_Ctx *ctx);

int mceSaveKeyPair(const McEliece_Ctx *ctx, unsigned char file_path);

int mceLoadKeyPair(McEliece_Ctx *ctx, unsigned char file_path);

int mceSaveKeyPub(const McEliece_Ctx *ctx, unsigned char file_path);

int mceLoadKeyPub(McEliece_Ctx *ctx, unsigned char file_path);

int mceSaveKeyPriv(const McEliece_Ctx *ctx, unsigned char file_path);

int mceLoadKeyPriv(McEliece_Ctx *ctx, unsigned char file_path);

/**
 * Encrypt plaintext (pt) and save it to cipher text. All strctures must be allocated before use.
 * @param  ct  cipher text
 * @param  pt  plain text
 * @param  ctx McEliece context
 * @return     0 - succes, else error
 */
/// Encrypt plaintext (pt) and save it to cipher text.
int mceEncrypt(Vector_GF2 *ct, const Vector_GF2 *pt, const McEliece_Ctx *ctx);

/**
 * Encrypt plaintext (pt) and save it to cipher text. Output cipher text is allocated inside, must be freed after use freeVecGF2().
 * @param  ct  cipher text
 * @param  pt  plain text
 * @param  ctx McEliece context
 * @return     0 - succes, else error
 */
/// Encrypt plaintext (pt) and save it to cipher text. Output plain text is allocated inside, must be freed after use freeVecGF2().
int mceEncryptA(Vector_GF2 *ct, const Vector_GF2 *pt, const McEliece_Ctx *ctx);

/**
 * Decrypt cipher text (ct) and save it to plain text. All strctures must be allocated before use.
 * @param  pt  plain text
 * @param  ct  cipher text
 * @param  ctx McEliece context
 * @return     0 - succes, else error
 */
/// Decrypt cipher text (ct) and save it to plain text.
int mceDecrypt(Vector_GF2 *pt, Vector_GF2 *ct, const McEliece_Ctx *ctx);

/**
 * Decrypt cipher text (ct) and save it to plain text. Output cipher text is allocated inside, must be freed after use freeVecGF2().
 * @param  pt  plain text
 * @param  ct  cipher text
 * @param  ctx McEliece context
 * @return     0 - succes, else error
 */
/// Decrypt cipher text (ct) and save it to plain text. Output plain text is allocated inside, must be freed after use freeVecGF2().
int mceDecryptA(Vector_GF2 *pt, Vector_GF2 *ct, const McEliece_Ctx *ctx);

int mceMallocPtBlock(Vector_GF2 *pt);

int mceMallocCtBlock(Vector_GF2 *ct);

int mceFreePtBlock(Vector_GF2 *pt);

int mceFreeCtBlock(Vector_GF2 *ct);

int mceUpdatePtBlock(Vector_GF2 *pt, unsigned char *data);

int mceUpdateCtBlock(Vector_GF2 *ct, unsigned char *data);

int mceGetPtBlockSizeBits(McEliece_Ctx *ctx);

int mceGetCtBlockSizeBits(McEliece_Ctx *ctx);

/**
 * Free McEliece context after use.
 * @param ctx pointer to McEliece_Ctx structure
 */
/// Free McEliece context after use.
void mceFreeCtx(McEliece_Ctx *ctx);

#endif // MCELLIECE_H