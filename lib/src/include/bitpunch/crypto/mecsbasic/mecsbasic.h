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
#ifndef MECSBASIC_H
#define MECSBASIC_H

#include <bitpunch/config.h>
#include <bitpunch/crypto/mecsctx.h>

#ifdef BPU_CONF_ENCRYPTION
/**
 * Encrypt plaintext.
 * @param  out    cyphertext
 * @param  message  plaintext
 * @param  ctx  McEliece context
 * @return     0 - succes, else error
 */
int BPU_mecsBasicEncrypt(BPU_T_GF2_Vector * out, const BPU_T_GF2_Vector * in,
                         const BPU_T_Mecs_Ctx * ctx);
#endif

#ifdef BPU_CONF_DECRYPTION
int BPU_mecsBasicDecrypt(BPU_T_GF2_Vector * out, const BPU_T_GF2_Vector * in,
                         const BPU_T_Mecs_Ctx * ctx);
#endif

#endif // MECSBASIC_H
