#include <time.h>
#include <bitpunch/bitpunch.h>
#include "tools.h"

float timeDiff(struct timespec *te, struct timespec *ts) {
	float seconds;
	float nanoseconds;

	seconds = te->tv_sec - ts->tv_sec;
	if (te->tv_nsec - ts->tv_nsec < 0) {
		seconds--;
		nanoseconds = (1000000000L + te->tv_nsec - ts->tv_nsec);
	}
	else
		nanoseconds = (te->tv_nsec - ts->tv_nsec);

	return (seconds * 1000000000L + nanoseconds) / 1000.0;
}

void removeErrorBit(BPU_T_GF2_Vector *out, BPU_T_GF2_Vector *error, int number_of_errors) {
	int i;
	for(i = 0; i < error->len; i++) {
		if (BPU_gf2VecGetBit(error, i)) {

			if (BPU_gf2VecGetBit(out, i)) {
				BPU_gf2VecSetBit(out, i, 0);
			}
			else {
				BPU_gf2VecSetBit(out, i, 1);
			}
			number_of_errors--;
			if(!(number_of_errors)){
				break;
			}
		}


	}
}

void xorBit(BPU_T_GF2_Vector *out, int position) {
        if (!BPU_gf2VecGetBit(out, position)) {
            BPU_gf2VecSetBit(out, position, 1);
        }
        else {
            BPU_gf2VecSetBit(out, position, 0);
        }
}

void addErrorBit(BPU_T_GF2_Vector *out, BPU_T_GF2_Vector *error, int number_of_errors) {
	int i;
	for(i = 0; i < error->len; i++) {
		if (!BPU_gf2VecGetBit(error, i)) {
			if (BPU_gf2VecGetBit(out, i)) {
				BPU_gf2VecSetBit(out, i, 0);
			}
			else {
				BPU_gf2VecSetBit(out, i, 1);
			}
			number_of_errors--;
			if(!(number_of_errors)){
				break;
			}
		}


	}
}

unsigned long long int rdtsc(void)
{
	unsigned long lo, hi;
	asm( "rdtsc" : "=a" (lo), "=d" (hi) );
	return( lo | (hi << 32) );
}
