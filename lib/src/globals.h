/**
* Here are defined extern (global) variables, structures etc.
*/
#ifndef GLOBALS_H
#define GLOBALS_H

#include "types.h"

#define ONE 1
#define debug 0

/************** DEBUG RULES *****************/

// ################### process.h
// if set it prints debug output in decode adn decodeA function
// #define DEBUG_DECODE
// #define DEBUG_ENCRYPT
// #define DEBUG_DECRYPT

// ################### init.h
// #define DEBUG_MALLOC_VECTOR_GF2
// #define DEBUG_INIT_MCE_CTX

/********************************************/

/*************** TESTS **********************/
// equivalent code status
// #define TEST_DET_EQ_CODE

/// McEliece context, extern variable
extern McEliece_Ctx mce_ctx;
extern int __test_was_eq_code;

#endif // GLOBALS_H