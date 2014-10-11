/**
 This file is part of BitPunch
 Copyright (C) 2014 Frantisek Uhrecky <frantisek.uhrecky[at]gmail.com>
 Copyright (C) 2014 Andrej Gulyas <andrej.guly[at]gmail.com>
 Copyright (C) 2014 Marek Klein  <kleinmrk[at]gmail.com>
 Copyright (C) 2014 Filip Machovec  <filipmachovec[at]yahoo.com>
 Copyright (C) 2014 Jozef Kudlac <jozef[at]kudlac.sk>

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
/**
* Here are defined extern (global) variables, structures etc.
*/
#ifndef GLOBALS_H
#define GLOBALS_H

#include "types.h"

#define BPU_ONE 1

/************** DEBUG RULES *****************/

// ################### process.h
// if set it prints debug output in decode adn decodeA function
// #define BPU_DEBUG_DECODE
// #define BPU_DEBUG_ENCRYPT
// #define BPU_DEBUG_DECRYPT

// ################### init.h
// #define BPU_DEBUG_MALLOC_VECTOR_GF2
// #define BPU_DEBUG_INIT_MCE_CTX

/********************************************/

/*************** TESTS **********************/
// equivalent code status
// #define BPU_TEST_DET_EQ_CODE

/// McEliece context, extern variable
extern BPU_T_McEliece_Ctx mce_ctx;
extern int __test_was_eq_code;

#endif // GLOBALS_H