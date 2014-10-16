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
#include "mceliece.h"
#include "debugio.h"
#include "init.h"
#include "process.h"

int BPU_mecsInitCtx(BPU_T_McEliece_Ctx *ctx, uint8_t t_errors) {
  int rc = 0;

  ctx->pub_key.t = t_errors;

  switch(t_errors) {
    case 5:
      rc = BPU_initLogExpTable(ctx, (BPU_T_GF2_16x)2, (BPU_T_GF2_16x) BPU_GF2_POLY_DEG_5);

      break;
    case 6:
      rc = BPU_initLogExpTable(ctx, (BPU_T_GF2_16x)2, (BPU_T_GF2_16x) BPU_GF2_POLY_DEG_6);
      
      break;
    case 7:
      rc = BPU_initLogExpTable(ctx, (BPU_T_GF2_16x)2, (BPU_T_GF2_16x) BPU_GF2_POLY_DEG_6);
      
      break;
    case 50:
      rc = BPU_initLogExpTable(ctx, (BPU_T_GF2_16x)2, (BPU_T_GF2_16x) BPU_GF2_POLY_DEG_11);
      
      break;
    default:
      BPU_printError("Not supported, only t error: 5, 6, 7, 50");

      rc = 1;
      break;
  }
  if (!rc) {
    ctx->max_pt_len_bit = (ctx->a_data.mod_deg <= 5) ? 1 << (ctx->a_data.mod_deg - 3) : 1 << (ctx->a_data.mod_deg - 2);
    // ctx->max_pt_len = (ctx->max_pt_len_bit >> 3) + (ctx->max_pt_len_bit % 8) ? 1 : 0;

    ctx->max_ct_len_bit = ctx->a_data.ord + 2 * ctx->max_pt_len_bit;
    // ctx->max_pt_len = (ctx->max_ct_len_bit >> 3) + (ctx->max_ct_len_bit % 8) ? 1 : 0;
  }
  return rc;
}

int BPU_mecsEncrypt(BPU_T_Vector_GF2 *ct, const BPU_T_Vector_GF2 *pt, const BPU_T_McEliece_Ctx *ctx) {
  return BPU_encrypt(ct, pt, ctx);
  // return BPU_encryptCCA2KobaraImai(ct, pt, ctx);
}

int BPU_mecsEncryptA(BPU_T_Vector_GF2 *ct, const BPU_T_Vector_GF2 *pt, const BPU_T_McEliece_Ctx *ctx) {
  return BPU_encryptA(ct, pt, ctx);
}

int BPU_mecsDecrypt(BPU_T_Vector_GF2 *pt, BPU_T_Vector_GF2 *ct, const BPU_T_McEliece_Ctx *ctx) {
  return BPU_decrypt(pt, ct, ctx);
  // return BPU_decryptCCA2KobaraImai(pt, ct, ctx);
}

int BPU_mecsDecryptA(BPU_T_Vector_GF2 *pt, BPU_T_Vector_GF2 *ct, const BPU_T_McEliece_Ctx *ctx) {
  return BPU_decryptA(pt, ct, ctx);
}

int BPU_mecsGenKeyPair(BPU_T_McEliece_Ctx *ctx) {
  return BPU_genKeyPair(ctx, ctx->pub_key.t);
}

void BPU_mecsFreeCtx(BPU_T_McEliece_Ctx *ctx) {
  BPU_freeMcElieceCtx(ctx);
}

int BPU_mecsGetPtBlockSizeBits(BPU_T_McEliece_Ctx *ctx) {
  return ctx->max_pt_len_bit;
}

int BPU_mecsGetCtBlockSizeBits(BPU_T_McEliece_Ctx *ctx) {
  return ctx->max_ct_len_bit;
}