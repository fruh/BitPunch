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
#ifndef PADDING_H
#define PADDING_H

#include <bitpunch/math/gf2.h>

/**
 * Add padding to message. Padding begins with 1.
 * @param  padded_message[out] padded message
 * @param  message[in]        input message
 * @param  padding_len[in]    padding len
 * @return                0 on succes else, non-zero value
 */
/// Add padding to message.
int BPU_padAdd(BPU_T_GF2_Vector * padded_message,
               const BPU_T_GF2_Vector * message, const uint16_t padding_len);

/**
 * Delete padding from message. Padding begins with 1.
 * @param  message[out]        output message without padding
 * @param  padded_message[in] input message with padding
 * @return                0 on succes else, non-zero value
 */
/// Delete padding from message.
int BPU_padDel(BPU_T_GF2_Vector * message,
               const BPU_T_GF2_Vector * padded_message);

#endif // PADDING_H
