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
#ifndef BPU_INT_H
#define BPU_INT_H

#include <stdint.h>

/**
 * Check if number is prime number.
 * @param  n [description]
 * @return   [description]
 */
int BPU_isPrime(int n);

/**
 * Get recursively index of highest set bit in unsigned integer.
 * @param  x        integer
 * @param  start    start position in recursion
 * @param  len      actual processing length in recursion
 * @param  ele_size size of integer
 * @return          index of highest set bit
 */
int msb32(uint32_t x, int start, int len, int ele_size);

#endif // BPU_INT_H
