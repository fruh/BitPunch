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
#ifndef MCELLIECE_H
#define MCELLIECE_H

#include "types.h"
#include "init.h"

/**
 * Initialize McEliece context to be able working with it.
 * @param  ctx  pointer to BPU_T_McEliece_Ctx structure, it is a context
 * @param  t_errors number of errors, i.e. 50
 * @return          0 on succes, else non-zero value
 */
/// Initialize McEliece context to be able working with it
int BPU_mecsInitCtx(BPU_T_McEliece_Ctx *ctx, uint8_t t_errors);

/**
 * Key generation, first must be initialized context using BPU_mecsInitCtx().
 * @param  ctx McEliece context
 * @return     0 - success, else error
 */
/// Key generation, first must be initialized context using BPU_mecsInitCtx.
int BPU_mecsGenKeyPair(BPU_T_McEliece_Ctx *ctx);

int BPU_mecsSaveKeyPair(const BPU_T_McEliece_Ctx *ctx, unsigned char file_path);

int BPU_mecsLoadKeyPair(BPU_T_McEliece_Ctx *ctx, unsigned char file_path);

int BPU_mecsSaveKeyPub(const BPU_T_McEliece_Ctx *ctx, unsigned char file_path);

int BPU_mecsLoadKeyPub(BPU_T_McEliece_Ctx *ctx, unsigned char file_path);

int BPU_mecsSaveKeyPriv(const BPU_T_McEliece_Ctx *ctx, unsigned char file_path);

int BPU_mecsLoadKeyPriv(BPU_T_McEliece_Ctx *ctx, unsigned char file_path);

/**
 * Encrypt plaintext (pt) and save it to cipher text. All strctures must be allocated before use.
 * @param  ct  cipher text
 * @param  pt  plain text
 * @param  ctx McEliece context
 * @return     0 - succes, else error
 */
/// Encrypt plaintext (pt) and save it to cipher text.
int BPU_mecsEncrypt(BPU_T_Vector_GF2 *ct, const BPU_T_Vector_GF2 *pt, const BPU_T_McEliece_Ctx *ctx);

/**
 * Encrypt plaintext (pt) and save it to cipher text. Output cipher text is allocated inside, must be freed after use BPU_freeVecGF2().
 * @param  ct  cipher text
 * @param  pt  plain text
 * @param  ctx McEliece context
 * @return     0 - succes, else error
 */
/// Encrypt plaintext (pt) and save it to cipher text. Output plain text is allocated inside, must be freed after use BPU_freeVecGF2().
int BPU_mecsEncryptA(BPU_T_Vector_GF2 *ct, const BPU_T_Vector_GF2 *pt, const BPU_T_McEliece_Ctx *ctx);

/**
 * Decrypt cipher text (ct) and save it to plain text. All strctures must be allocated before use.
 * @param  pt  plain text
 * @param  ct  cipher text
 * @param  ctx McEliece context
 * @return     0 - succes, else error
 */
/// Decrypt cipher text (ct) and save it to plain text.
int BPU_mecsDecrypt(BPU_T_Vector_GF2 *pt, BPU_T_Vector_GF2 *ct, const BPU_T_McEliece_Ctx *ctx);

/**
 * Decrypt cipher text (ct) and save it to plain text. Output cipher text is allocated inside, must be freed after use BPU_freeVecGF2().
 * @param  pt  plain text
 * @param  ct  cipher text
 * @param  ctx McEliece context
 * @return     0 - succes, else error
 */
/// Decrypt cipher text (ct) and save it to plain text. Output plain text is allocated inside, must be freed after use BPU_freeVecGF2().
int BPU_mecsDecryptA(BPU_T_Vector_GF2 *pt, BPU_T_Vector_GF2 *ct, const BPU_T_McEliece_Ctx *ctx);

int BPU_mecsMallocPtBlock(BPU_T_Vector_GF2 *pt);

int BPU_mecsMallocCtBlock(BPU_T_Vector_GF2 *ct);

int BPU_mecsFreePtBlock(BPU_T_Vector_GF2 *pt);

int BPU_mecsFreeCtBlock(BPU_T_Vector_GF2 *ct);

int BPU_mecsUpdatePtBlock(BPU_T_Vector_GF2 *pt, unsigned char *data);

int BPU_mecsUpdateCtBlock(BPU_T_Vector_GF2 *ct, unsigned char *data);

int BPU_mecsGetPtBlockSizeBits(BPU_T_McEliece_Ctx *ctx);

int BPU_mecsGetCtBlockSizeBits(BPU_T_McEliece_Ctx *ctx);

/**
 * Free McEliece context after use.
 * @param ctx pointer to BPU_T_McEliece_Ctx structure
 */
/// Free McEliece context after use.
void BPU_mecsFreeCtx(BPU_T_McEliece_Ctx *ctx);

#endif // MCELLIECE_H