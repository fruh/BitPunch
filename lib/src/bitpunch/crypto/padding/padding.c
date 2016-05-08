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
#include "padding.h"

#include <bitpunch/debugio.h>
#include <bitpunch/math/gf2.h>

int BPU_padAdd(BPU_T_GF2_Vector * padded_message,
               const BPU_T_GF2_Vector * message, const uint16_t padding_len) {
    int i;

    if (message->len + padding_len != padded_message->len) {
        BPU_printError("Wrong message len");
        return -1;
    }
    // copy message into padded message
    for (i = 0; i < message->array_length; i++) {
        padded_message->elements[i] = message->elements[i];
    }
    // add padding - first padded bit set to 1, rest keep 0
    BPU_gf2VecSetBit(padded_message, message->len, 1);

    return 0;
}

int BPU_padDel(BPU_T_GF2_Vector * message,
               const BPU_T_GF2_Vector * padded_message) {
    int i, message_size = 0, blocks = 0;


    // count the message size
    for (i = padded_message->len - 1; i >= 0; i--) {
        // nula - padding
        if (BPU_gf2VecGetBit(padded_message, i) == 1) {

            // ci bola aspon jedna 0 pred 1
            //if (i <= padded_message->len-3) {
            message_size = i;
            break;
            //}
            // inak zly padding
            /*else {
               BPU_printError("del_padding: message padding is incorrect");
               return -1;
               } */
        }
    }
    if (message->len < message_size) {
        BPU_printError("Wrong message size.");
        return -1;
    }
    message->len = message_size;
    blocks = message->len / padded_message->element_bit_size;

    // copy n-1 elements of padded message into message
    for (i = 0; i < blocks - 1; i++) {
        message->elements[i] = padded_message->elements[i];
    }
    // copy the rest of message
    for (i =
         (blocks - 1) * padded_message->element_bit_size;
         i < message->len; i++) {
        BPU_gf2VecSetBit(message, i, BPU_gf2VecGetBit(padded_message, i));
    }
    return 0;
}
