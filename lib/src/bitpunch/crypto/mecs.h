/*
This file is part of BitPunch
Copyright (C) 2013-2015 Frantisek Uhrecky <frantisek.uhrecky[what here]gmail.com>
Copyright (C) 2013-2014 Andrej Gulyas <andrej.guly[what here]gmail.com>
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
#ifndef BPU_MECS_H
#define BPU_MECS_H

#include "mecsctx.h"

#include <bitpunch/config.h>
#include <bitpunch/math/gf2.h>
#include <bitpunch/math/gf2x.h>
#include <bitpunch/code/codectx.h>

/******************************************************************/
#ifdef BPU_CONF_ENCRYPTION

int BPU_mecsEncrypt(BPU_T_GF2_Vector * ct, const BPU_T_GF2_Vector * pt,
                    const BPU_T_Mecs_Ctx * ctx);
#endif

#ifdef BPU_CONF_DECRYPTION

int BPU_mecsDecrypt(BPU_T_GF2_Vector * pt, BPU_T_GF2_Vector * ct,
                    const BPU_T_Mecs_Ctx * ctx);
#endif

#ifdef BPU_CONF_KEY_GEN

int BPU_mecsGenKeyPair(BPU_T_Mecs_Ctx * ctx);
#endif

#endif // BPU_MECS_H
