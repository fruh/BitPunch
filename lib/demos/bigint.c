/*
 This file is part of BitPunch
 Copyright (C) 2014-2016 Marek Klein <kleinmrk@gmail.com>

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

#include <bitpunch/math/bigint.h>
#include <bitpunch/math/uni.h>

int main(int argc, char **argv) {
    BPU_T_Bigint *a, *b, *c;

    BPU_bigintMalloc(&a, 32);
    BPU_bigintMalloc(&b, 16);
    BPU_bigintMalloc(&c, 32);

    a->elements[0] = 65535;
    a->elements[1] = 65535;
    b->elements[0] = 65535;

    BPU_bigintMultiply(c, a, b);
#ifdef BPU_CONF_PRINT
    BPU_printElementArray(a);
    BPU_printElementArray(b);
    BPU_printElementArray(c);
#endif

    BPU_bigintAdd(c, a, b);
#ifdef BPU_CONF_PRINT
    BPU_printElementArray(a);
    BPU_printElementArray(b);
    BPU_printElementArray(c);
#endif
    BPU_bigintFree(&a);
    BPU_bigintFree(&b);
    BPU_bigintFree(&c);

    return 0;
}
