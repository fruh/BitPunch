#include <bitpunch/bitpunch.h>

#include <stdio.h>

int main(int argc, char **argv) {
        int rc = 0;
        // MUST BE NULL
        BPU_T_Mecs_Ctx *ctx = NULL;
        BPU_T_UN_Mecs_Params params;

        /***************************************/
        // mce initialisation t = 50, m = 11
        if (BPU_mecsInitParamsGoppa(&params, 11, 50, 0)) {
                return 1;
        }
        fprintf(stderr, "Basic GOPPA Initialisation...\n");
        if (BPU_mecsInitCtx(&ctx, &params, BPU_EN_MECS_BASIC_GOPPA)) {
                return 1;
        }
        BPU_mecsFreeCtx(&ctx);
        BPU_mecsFreeParamsGoppa(&params);

        return rc;
}