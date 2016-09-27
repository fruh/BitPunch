#ifndef TOOLS_H
#define TOOLS_H
#include <sys/time.h>
/**
 * @brief timeDiff returns time difference ts - te in mikroseconds
 * @param te
 * @param ts
 * @return
 */
float timeDiff(struct timespec *te, struct timespec *ts);
void removeErrorBit(BPU_T_GF2_Vector *out, BPU_T_GF2_Vector *error, int number_of_errors);
void addErrorBit(BPU_T_GF2_Vector *out, BPU_T_GF2_Vector *error, int number_of_errors);
unsigned long long int rdtsc(void);
void xorBit(BPU_T_GF2_Vector *out, int position);
#endif // TOOLS_H
