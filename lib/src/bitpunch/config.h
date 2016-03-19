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
#ifndef CONFIG_H
#define CONFIG_H

// constraints
#ifdef BPU_CONF_FULL
#ifndef BPU_CONF_ASN1
#define BPU_CONF_ASN1
#endif
#ifndef BPU_CONF_MECS_CCA2_POINTCHEVAL_GOPPA
#define BPU_CONF_MECS_CCA2_POINTCHEVAL_GOPPA
#endif
#ifndef BPU_CONF_MECS_CCA2_POINTCHEVAL_QCMDPC
#define BPU_CONF_MECS_CCA2_POINTCHEVAL_QCMDPC
#endif
#ifndef BPU_CONF_SHA_512
#define BPU_CONF_SHA_512
#endif
#ifndef BPU_CONF_FULL_MECS
#define BPU_CONF_FULL_MECS
#endif
#if !defined(BPU_CONF_NO_PRINT) && !defined(BPU_CONF_PRINT)
#define BPU_CONF_PRINT
#endif
#endif

#ifdef BPU_CONF_FULL_MECS
#ifndef BPU_CONF_ENCRYPTION
#define BPU_CONF_ENCRYPTION
#endif
#ifndef BPU_CONF_DECRYPTION
#define BPU_CONF_DECRYPTION
#endif
#ifndef BPU_CONF_KEY_GEN
#define BPU_CONF_KEY_GEN
#endif
#endif

#if defined(BPU_CONF_MECS_CCA2_POINTCHEVAL_GOPPA) || defined(BPU_CONF_MECS_CCA2_POINTCHEVAL_QCMDPC)
#ifndef BPU_CONF_SHA_512
#define BPU_CONF_SHA_512
#endif
#endif

#endif // CONFIG_H
