/*
This file is part of BitPunch
Copyright (C) 2014-2015 Frantisek Uhrecky <frantisek.uhrecky[what here]gmail.com>

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
#ifndef BPUT_GF2XTYPES_H
#define BPUT_GF2XTYPES_H

#include <stdint.h>

/**
* Polynomial representation over GF2, max deg f < 16
*/
typedef uint16_t BPU_T_GF2_16x;

/**
* Polynomial representation over GF2, max deg f < 32
*/
typedef uint32_t BPU_T_GF2_32x;

/**
 * GF2_16x Vector representation
 */
typedef struct _BPU_T_GF2_16x_Vector {
  BPU_T_GF2_16x *elements;
  uint8_t len; ///< number of elements
}BPU_T_GF2_16x_Vector;

/**
* Matrix representation over GF2_16x.
*/
typedef struct _BPU_T_GF2_16x_Matrix {
  BPU_T_GF2_16x **elements; ///< all element of matrix
  uint16_t k; ///< rows
  uint16_t n; ///< cols
}BPU_T_GF2_16x_Matrix;

/**
* Representation of polynomial.
*/
typedef struct _BPU_T_GF2_16x_Poly{
  BPU_T_GF2_16x *coef; ///< Polynomial over GF2m
  int16_t deg; ///< degree
  int16_t max_deg; ///< degree
}BPU_T_GF2_16x_Poly;

#endif // BPUT_GF2XTYPES_H
