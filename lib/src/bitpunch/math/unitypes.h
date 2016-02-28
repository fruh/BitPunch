#ifndef UNITYPES_H
#define UNITYPES_H

#include <stdint.h>

/**
 * Represents elements bigger than standard C types.
 */
typedef struct _BPU_T_Element_Array {
    uint16_t *elements; ///< element stored in the array
    uint8_t element_bit_size; ///< element size e.g., 16 bits
    uint16_t array_length; ///< number of elements in the array
    uint32_t len; ///< bit length of the element
}BPU_T_Element_Array;

#endif // UNITYPES_H
