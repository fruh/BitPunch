/*
This file is part of BitPunch
Copyright (C) 2015 Frantisek Uhrecky <frantisek.uhrecky[what here]gmail.com>

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
#ifndef MECSPOINTCHEVAL_H
#define MECSPOINTCHEVAL_H

#include <bitpunch/config.h>

#if defined(BPU_CONF_MECS_CCA2_POINTCHEVAL_GOPPA) || defined(BPU_CONF_MECS_CCA2_POINTCHEVAL_QCMDPC)

#include <bitpunch/config.h>
#include <bitpunch/math/gf2.h>
#include <bitpunch/crypto/mecsctx.h>
#include <bitpunch/crypto/hash/sha512.h>

#ifdef BPU_CONF_ENCRYPTION
/**
 * @brief BPU_mecsPointchevalCCA2Encrypt
 * @param out
 * @param in
 * @param ctx
 * @return
 */
int BPU_mecsPointchevalCCA2Encrypt(BPU_T_GF2_Vector * out,
                                   const BPU_T_GF2_Vector * in,
                                   const BPU_T_Mecs_Ctx * ctx);
#endif

#ifdef BPU_CONF_DECRYPTION
/**
 * @brief BPU_mecsPointchevalCCA2Decrypt
 * @param out
 * @param in
 * @param ctx
 * @return
 */
int BPU_mecsPointchevalCCA2Decrypt(BPU_T_GF2_Vector * out,
                                   const BPU_T_GF2_Vector * in,
                                   const BPU_T_Mecs_Ctx * ctx);
#endif

#endif // BPU_CONF_MECS_CCA2_POINTCHEVAL_GOPPA
#endif // MECSPOINTCHEVAL_H
