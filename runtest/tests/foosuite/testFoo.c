#include <bitpunch/bitpunch.h>

#include <stdio.h>

int main(int argc, char **argv)
{
    int rc = 0;
    BPU_T_Mecs_Ctx *ctx = NULL;
    BPU_T_UN_Mecs_Params *params = NULL;

    if (NULL == (params = BPU_mecsParamsGoppaNew(11, 50, 0))) {
        return 1;
    }

    fprintf(stderr, "Basic GOPPA Initialisation...\n");
    if (NULL == (ctx = BPU_mecsCtxNew(params, BPU_EN_MECS_BASIC_GOPPA))) {
        return 1;
    }
    BPU_mecsFreeCtx(ctx);
    BPU_mecsParamsGoppaFree(params);

    return rc;
}
