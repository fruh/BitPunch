/*
This file is part of BitPunch
Copyright (C) 2016 Frantisek Uhrecky <frantisek.uhrecky[what here]gmail.com>
Copyright (C) 2016 Marek Klein  <kleinmrk[what here]gmail.com>

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
#include "uni.h"

typedef BPU_T_Element_Array BPU_T_Bigint;

int BPU_bigintMultiply(BPU_T_Bigint * out, BPU_T_Bigint * a, BPU_T_Bigint * b);

int BPU_bigintAdd(BPU_T_Bigint * out, BPU_T_Bigint * a, BPU_T_Bigint * b);

#define BPU_bigintNull(b_pointer) BPU_elementArrayNull(b_pointer)

#define BPU_bigintMalloc(b_double_pointer, len) BPU_elementArrayMalloc(b_double_pointer, len)

#define BPU_bigintFree(b_double_pointer) BPU_elementArrayFree(b_double_pointer);

/**
* @brief BPU_gf2VecResize Resize vecor.
* @param v
* @param len
* @return
*/
#define BPU_bigintResize(b_pointer, len) BPU_elementArrayResize(b_pointer, len)
