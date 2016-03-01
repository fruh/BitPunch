/*
This file is part of BitPunch
Copyright (C) 2013-2016 Frantisek Uhrecky <frantisek.uhrecky[what here]gmail.com>
Copyright (C) 2013-2014 Andrej Gulyas <andrej.guly[what here]gmail.com>
Copyright (C) 2013-2016 Marek Klein  <kleinmrk[what here]gmail.com>
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
#ifndef UNITYPES_H
#define UNITYPES_H

#include <stdint.h>
#include <stdio.h>
#include <string.h>

typedef uint16_t BPU_T_Element;

/**
 * Represents elements bigger than standard C types.
 */
typedef struct _BPU_T_Element_Array {
    BPU_T_Element *elements; ///< element stored in the array
    uint8_t element_bit_size; ///< element size e.g., 16 bits
    uint16_t array_length; ///< number of elements in the array
    uint32_t len; ///< bit length of the element
}BPU_T_Element_Array;

#endif // UNITYPES_H
