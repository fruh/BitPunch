/*
 This file is part of BitPunch
 Copyright (C) 2015 Frantisek Uhrecky <frantisek.uhrecky[what here]gmail.com>

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
#include "asn1.h"

#ifdef BPU_CONF_ASN1
#include <bitpunch/debugio.h>
#include <bitpunch/math/perm.h>

#include <libtasn1.h>
#include <stdlib.h>

int BPU_asn1EncodePriKey(char **buffer, int *size, const BPU_T_Mecs_Ctx * ctx) {
    int rc = 0;
    asn1_node definitions = NULL;
    asn1_node asn1_element = NULL;
    char error_desc[ASN1_MAX_ERROR_DESCRIPTION_SIZE];
    BPU_T_GF2_16x *h_mat = NULL;
    uint32_t h_mat_bs;
    int i, j;

    *size = 0;
    // TODO: structure cleanup on error
    rc = asn1_parser2tree(BPU_STR_ASN1_MECS_PRI_KEY, &definitions, error_desc);

    if (rc != ASN1_SUCCESS) {
        BPU_printError("ErrorDescription = %s\n\n", error_desc);
        return rc;
    }
    rc = asn1_create_element(definitions, BPU_STR_ASN1_MECS_PRI_KEY_NAME,
                             &asn1_element);
    if (rc != ASN1_SUCCESS) {
        BPU_printError("asn1_create_element(): %d", rc);
        return rc;
    }
    // write elements
    rc = -1;
    if (ctx->type == BPU_EN_MECS_BASIC_GOPPA) {
        rc = asn1_write_value(asn1_element, "oid",
                              BPU_STR_ASN1_OID_MECS_BASIC_GOPPA,
                              strlen(BPU_STR_ASN1_OID_MECS_BASIC_GOPPA));
    }
#ifdef BPU_CONF_MECS_CCA2_POINTCHEVAL_GOPPA
    if (ctx->type == BPU_EN_MECS_CCA2_POINTCHEVAL_GOPPA) {
        rc = asn1_write_value(asn1_element, "oid",
                              BPU_STR_ASN1_OID_MECS_CCA2_POINTCHEVAL_GOPPA,
                              strlen
                              (BPU_STR_ASN1_OID_MECS_CCA2_POINTCHEVAL_GOPPA));
    }
#endif
    if (rc != ASN1_SUCCESS) {
        BPU_printError("asn1_write_value(): oid %d", rc);
        return rc;
    }
    rc = asn1_write_value(asn1_element, "m",
                          (void *) &(ctx->code_ctx->math_ctx->mod_deg),
                          sizeof((ctx->code_ctx->math_ctx->mod_deg)));
    if (rc != ASN1_SUCCESS) {
        BPU_printError("asn1_write_value(): m %d", rc);
        return rc;
    }
    rc = asn1_write_value(asn1_element, "t", (void *) &(ctx->code_ctx->t),
                          sizeof((ctx->code_ctx->t)));
    if (rc != ASN1_SUCCESS) {
        BPU_printError("asn1_write_value(): t %d", rc);
        return rc;
    }
    rc = asn1_write_value(asn1_element, "mod",
                          (void *) &(ctx->code_ctx->math_ctx->mod),
                          sizeof(ctx->code_ctx->math_ctx->mod));
    if (rc != ASN1_SUCCESS) {
        BPU_printError("asn1_write_value(): mod %d", rc);
        return rc;
    }
    rc = asn1_write_value(asn1_element, "g",
                          (void *) (ctx->code_ctx->code_spec->goppa->g->coef),
                          sizeof(ctx->code_ctx->code_spec->goppa->g->coef[0]) *
                          (ctx->code_ctx->code_spec->goppa->g->deg + 1));
    if (rc != ASN1_SUCCESS) {
        BPU_printError("asn1_write_value(): g %d", rc);
        return rc;
    }
    rc = asn1_write_value(asn1_element, "p",
                          (void *) (ctx->code_ctx->code_spec->
                                    goppa->permutation->elements),
                          sizeof(ctx->code_ctx->code_spec->goppa->permutation->
                                 elements[0]) *
                          ctx->code_ctx->code_spec->goppa->permutation->size);
    if (rc != ASN1_SUCCESS) {
        BPU_printError("asn1_write_value(): p %d", rc);
        return rc;
    }
    if (ctx->code_ctx->type == BPU_EN_CODE_GOPPA) {
        if (!ctx->code_ctx->code_spec->goppa->h_mat) {
            BPU_printError("not supported w/o H");
            return -1;
        }
        // prepare H matrix
        h_mat_bs =
            sizeof(BPU_T_GF2_16x) *
            (ctx->code_ctx->code_spec->goppa->h_mat->k *
             ctx->code_ctx->code_spec->goppa->h_mat->n);
        h_mat = (BPU_T_GF2_16x *) malloc(h_mat_bs);

        if (!h_mat) {
            BPU_printError("Can not allocate memory for H matrix export");
            return -1;
        }
        j = 0;
        for (i = 0; i < ctx->code_ctx->code_spec->goppa->h_mat->k; i++) {
            memcpy(h_mat + j,
                   ctx->code_ctx->code_spec->goppa->h_mat->elements[i],
                   ctx->code_ctx->code_spec->goppa->h_mat->n *
                   sizeof(BPU_T_GF2_16x));
            j += ctx->code_ctx->code_spec->goppa->h_mat->n;
        }
    }
    else {
        BPU_printError("Not supported for this CODE type.");
        return -1;
    }
    rc = asn1_write_value(asn1_element, "h_mat", (void *) (h_mat), h_mat_bs);
    free(h_mat);

    if (rc != ASN1_SUCCESS) {
        BPU_printError("asn1_write_value(): h_mat %d", rc);

        return rc;
    }
    /* Clear the definition structures */
    asn1_delete_structure(&definitions);

    // get size of data
    asn1_der_coding(asn1_element, "", NULL, size, NULL);
    *buffer = (char *) malloc(*size);

    rc = asn1_der_coding(asn1_element, "", *buffer, size, NULL);
    if (rc != ASN1_SUCCESS) {
        BPU_printError("asn1_der_coding(): %d", rc);
        return rc;
    }
    /* Clear the definition structures */
    asn1_delete_structure(&asn1_element);
    return rc;
}

int BPU_asn1DecodePriKey(BPU_T_Mecs_Ctx ** ctx, const char *buffer,
                         const int size) {
    int rc = 0;
    asn1_node definitions = NULL;
    asn1_node asn1_element = NULL;
    int tmp_len;
    char error_desc[ASN1_MAX_ERROR_DESCRIPTION_SIZE];
    char *tmp_buf;
    BPU_T_EN_Mecs_Types type;
    BPU_T_UN_Mecs_Params params;
    uint8_t m, t;
    int i, j;

    // TODO:structure cleanup on error
    rc = asn1_parser2tree(BPU_STR_ASN1_MECS_PRI_KEY, &definitions, error_desc);

    if (rc != ASN1_SUCCESS) {
        BPU_printError("ErrorDescription = %s", error_desc);
        return rc;
    }
    rc = asn1_create_element(definitions, BPU_STR_ASN1_MECS_PRI_KEY_NAME,
                             &asn1_element);
    if (rc != ASN1_SUCCESS) {
        BPU_printError("asn1_create_element(): %d", rc);
        return rc;
    }
    /* Clear the definition structures */
    asn1_delete_structure(&definitions);

    // decode buffer
    rc = asn1_der_decoding(&asn1_element, buffer, size, error_desc);
    if (rc != ASN1_SUCCESS) {
        BPU_printError("Decoding error %d %s", rc, error_desc);
        return rc;
    }
    // section of data decoding
    type = BPU_asn1GetMecsTypeFromOid(asn1_element);
    if (type == -1) {
        BPU_printError("Wrong OID");
        return -1;
    }
    if (BPU_asn1ReadValue(&tmp_buf, &tmp_len, 1, "m", asn1_element)) {
        return -1;
    }
    m = *(uint8_t *) tmp_buf;
    free(tmp_buf);

    if (BPU_asn1ReadValue(&tmp_buf, &tmp_len, 1, "t", asn1_element)) {
        return -1;
    }
    t = *(uint8_t *) tmp_buf;
    free(tmp_buf);

    if (BPU_asn1ReadValue
        (&tmp_buf, &tmp_len, sizeof(BPU_T_GF2_16x), "mod", asn1_element)) {
        return -1;
    }
    if (type == BPU_EN_MECS_BASIC_GOPPA
        || type == BPU_EN_MECS_CCA2_POINTCHEVAL_GOPPA) {
        if (BPU_mecsInitParamsGoppa
            (&params, m, t, *((BPU_T_GF2_16x *) tmp_buf))) {
            return -1;
        }
        free(tmp_buf);
        if (BPU_mecsInitCtx(ctx, &params, type)) {
            return -1;
        }
        BPU_mecsFreeParamsGoppa(&params);
        if (BPU_asn1ReadValue
            (&tmp_buf, &tmp_len, sizeof(BPU_T_GF2_16x) * (t + 1), "g",
             asn1_element)) {
            return -1;
        }
        if (BPU_gf2xPolyMalloc(&((*ctx)->code_ctx->code_spec->goppa->g), t)) {
            return -1;
        }
        memcpy((*ctx)->code_ctx->code_spec->goppa->g->coef, tmp_buf, tmp_len);
        (*ctx)->code_ctx->code_spec->goppa->g->deg = t;
        free(tmp_buf);
    }
    else {
        BPU_printError("Not supported for this code type.");
        return -1;
    }
    if (BPU_asn1ReadValue
        (&tmp_buf, &tmp_len,
         sizeof(BPU_T_Perm_Element) * (*ctx)->code_ctx->code_len, "p",
         asn1_element)) {
        return -1;
    }
    if (BPU_permMalloc
        (&((*ctx)->code_ctx->code_spec->goppa->permutation),
         (*ctx)->code_ctx->code_len)) {
        return -1;
    }
    memcpy((*ctx)->code_ctx->code_spec->goppa->permutation->elements, tmp_buf,
           tmp_len);
    free(tmp_buf);

    if (BPU_asn1ReadValue
        (&tmp_buf, &tmp_len,
         sizeof(BPU_T_GF2_16x) * (*ctx)->code_ctx->t *
         (*ctx)->code_ctx->code_len, "h_mat", asn1_element)) {
        return -1;
    }
    if (BPU_gf2xMatMalloc
        (&((*ctx)->code_ctx->code_spec->goppa->h_mat), (*ctx)->code_ctx->t,
         (*ctx)->code_ctx->code_len)) {
        return -1;
    }
    j = 0;
    for (i = 0; i < (*ctx)->code_ctx->code_spec->goppa->h_mat->k; i++) {
        memcpy((*ctx)->code_ctx->code_spec->goppa->h_mat->elements[i],
               tmp_buf + j,
               (*ctx)->code_ctx->code_spec->goppa->h_mat->n *
               sizeof(BPU_T_GF2_16x));
        j += (*ctx)->code_ctx->code_spec->goppa->h_mat->n *
            sizeof(BPU_T_GF2_16x);
    }
    free(tmp_buf);

    asn1_delete_structure(&asn1_element);
    return rc;
}

int BPU_asn1EncodePubKey(char **buffer, int *size, const BPU_T_Mecs_Ctx * ctx) {
    int rc = 0;
    asn1_node definitions = NULL;
    asn1_node asn1_element = NULL;
    char error_desc[ASN1_MAX_ERROR_DESCRIPTION_SIZE];
    BPU_T_GF2 *g_mat = NULL;
    uint32_t g_mat_bs;
    int i, j;

    *size = 0;
    // TODO:structure cleanup on error
    rc = asn1_parser2tree(BPU_STR_ASN1_MECS_PUB_KEY, &definitions, error_desc);

    if (rc != ASN1_SUCCESS) {
        BPU_printError("ErrorDescription = %s", error_desc);
        return rc;
    }
    rc = asn1_create_element(definitions, BPU_STR_ASN1_MECS_PUB_KEY_NAME,
                             &asn1_element);
    if (rc != ASN1_SUCCESS) {
        BPU_printError("asn1_create_element(): %d", rc);
        return rc;
    }
    // write elements
    rc = -1;
    if (ctx->type == BPU_EN_MECS_BASIC_GOPPA) {
        rc = asn1_write_value(asn1_element, "oid",
                              BPU_STR_ASN1_OID_MECS_BASIC_GOPPA,
                              strlen(BPU_STR_ASN1_OID_MECS_BASIC_GOPPA));
    }
#ifdef BPU_CONF_MECS_CCA2_POINTCHEVAL_GOPPA
    if (ctx->type == BPU_EN_MECS_CCA2_POINTCHEVAL_GOPPA) {
        rc = asn1_write_value(asn1_element, "oid",
                              BPU_STR_ASN1_OID_MECS_CCA2_POINTCHEVAL_GOPPA,
                              strlen
                              (BPU_STR_ASN1_OID_MECS_CCA2_POINTCHEVAL_GOPPA));
    }
#endif
    if (rc != ASN1_SUCCESS) {
        BPU_printError("asn1_write_value(): oid %d", rc);
        return rc;
    }
    rc = asn1_write_value(asn1_element, "m",
                          (void *) &(ctx->code_ctx->math_ctx->mod_deg),
                          sizeof((ctx->code_ctx->math_ctx->mod_deg)));
    if (rc != ASN1_SUCCESS) {
        BPU_printError("asn1_write_value(): m %d", rc);
        return rc;
    }
    rc = asn1_write_value(asn1_element, "t", (void *) &(ctx->code_ctx->t),
                          sizeof((ctx->code_ctx->t)));
    if (rc != ASN1_SUCCESS) {
        BPU_printError("asn1_write_value(): t %d", rc);
        return rc;
    }
    if (ctx->code_ctx->type == BPU_EN_CODE_GOPPA) {
        g_mat_bs =
            sizeof(BPU_T_GF2) * (ctx->code_ctx->code_spec->goppa->g_mat->k *
                                 ctx->code_ctx->code_spec->goppa->
                                 g_mat->elements_in_row);

        // prepare G matrix
        g_mat = (BPU_T_GF2 *) malloc(g_mat_bs);

        if (!g_mat) {
            BPU_printError("Can not allocate memory for G matrix export");
            return -1;
        }
        j = 0;
        for (i = 0; i < ctx->code_ctx->code_spec->goppa->g_mat->k; i++) {
            memcpy(g_mat + j,
                   ctx->code_ctx->code_spec->goppa->g_mat->elements[i],
                   ctx->code_ctx->code_spec->goppa->g_mat->elements_in_row *
                   sizeof(BPU_T_GF2));
            j += ctx->code_ctx->code_spec->goppa->g_mat->elements_in_row;
        }
    }
    else {
        BPU_printError("Not supported for this MECS type.");
        return -1;
    }
    rc = asn1_write_value(asn1_element, "g_mat", (void *) (g_mat), g_mat_bs);
    free(g_mat);

    if (rc != ASN1_SUCCESS) {
        BPU_printError("asn1_write_value(): g_mat %d", rc);
        return rc;
    }
    /* Clear the definition structures */
    asn1_delete_structure(&definitions);

    // get size of data
    asn1_der_coding(asn1_element, "", NULL, size, NULL);
    *buffer = (char *) malloc(*size);

    rc = asn1_der_coding(asn1_element, "", *buffer, size, NULL);
    if (rc != ASN1_SUCCESS) {
        BPU_printError("asn1_der_coding(): %d", rc);
    }
    /* Clear the definition structures */
    asn1_delete_structure(&asn1_element);
    return rc;
}

int BPU_asn1DecodePubKey(BPU_T_Mecs_Ctx ** ctx, const char *buffer,
                         const int size) {
    int rc = 0;
    asn1_node definitions = NULL;
    asn1_node asn1_element = NULL;
    int tmp_len;
    char error_desc[ASN1_MAX_ERROR_DESCRIPTION_SIZE];
    char *tmp_buf = NULL;
    BPU_T_EN_Mecs_Types type;
    BPU_T_UN_Mecs_Params params;
    uint8_t m, t;
    int i, j, tmp;

    // TODO:structure cleanup on error
    rc = asn1_parser2tree(BPU_STR_ASN1_MECS_PUB_KEY, &definitions, error_desc);

    if (rc != ASN1_SUCCESS) {
        BPU_printError("ErrorDescription = %s", error_desc);
        return rc;
    }
    rc = asn1_create_element(definitions, BPU_STR_ASN1_MECS_PUB_KEY_NAME,
                             &asn1_element);
    if (rc != ASN1_SUCCESS) {
        BPU_printError("asn1_create_element(): %d", rc);
        return rc;
    }
    /* Clear the definition structures */
    asn1_delete_structure(&definitions);

    // decode buffer
    rc = asn1_der_decoding(&asn1_element, buffer, size, error_desc);
    if (rc != ASN1_SUCCESS) {
        BPU_printError("Decoding error %d, %s", rc, error_desc);
        return rc;
    }
    // section of data decoding
    type = BPU_asn1GetMecsTypeFromOid(asn1_element);
    if (type == -1) {
        BPU_printError("Wrong OID");
        return -1;
    }
    if (BPU_asn1ReadValue(&tmp_buf, &tmp_len, 1, "m", asn1_element)) {
        return -1;
    }
    m = *(uint8_t *) tmp_buf;
    free(tmp_buf);

    if (BPU_asn1ReadValue(&tmp_buf, &tmp_len, 1, "t", asn1_element)) {
        return -1;
    }
    t = *(uint8_t *) tmp_buf;
    free(tmp_buf);

    // init mecs context without mod
    if (type == BPU_EN_MECS_BASIC_GOPPA
        || type == BPU_EN_MECS_CCA2_POINTCHEVAL_GOPPA) {
        if (BPU_mecsInitParamsGoppa(&params, m, t, -1)) {
            return -1;
        }
    }
    else {
        BPU_printError("Type not supported");
        return -1;
    }
    if ((rc = BPU_mecsInitCtx(ctx, &params, type))) {
        BPU_printError("can not initialize mecs ctx");

        return rc;
    }
    BPU_mecsFreeParamsGoppa(&params);

    if (BPU_asn1ReadValue
        (&tmp_buf, &tmp_len,
         (*ctx)->code_ctx->code_len * (*ctx)->code_ctx->msg_len, "g_mat",
         asn1_element)) {
        return -1;
    }
    if (BPU_gf2MatMalloc
        (&((*ctx)->code_ctx->code_spec->goppa->g_mat),
         (*ctx)->code_ctx->code_len - (*ctx)->code_ctx->msg_len,
         (*ctx)->code_ctx->msg_len)) {
        return -1;
    }
    j = 0;
    tmp =
        (*ctx)->code_ctx->code_spec->goppa->g_mat->elements_in_row *
        ((*ctx)->code_ctx->code_spec->goppa->g_mat->element_bit_size / 8);
    for (i = 0; i < (*ctx)->code_ctx->code_spec->goppa->g_mat->k; i++) {
        memcpy((*ctx)->code_ctx->code_spec->goppa->g_mat->elements[i],
               tmp_buf + j, tmp);
        j += tmp;
    }
    free(tmp_buf);

    asn1_delete_structure(&asn1_element);
    return rc;
}

int BPU_asn1ReadValue(char **buffer, int *len, const int max_len,
                      const char *name, const asn1_node node) {
    int rc;

    // get size and malloc buffer
    if ((rc = BPU_asn1MallocBuffer(buffer, len, max_len, node, name))) {
        return rc;
    }
    // decode data
    rc = asn1_read_value(node, name, *buffer, len);
    BPU_printDebug("data len: %d", *len);
    if (rc != ASN1_SUCCESS) {
        BPU_printError("asn1_read_value(): %s %d", name, rc);
        return -1;
    }
    BPU_printDebug("data %s: %02x", name, **buffer);
    return rc;
}

BPU_T_EN_Mecs_Types BPU_asn1GetMecsTypeFromOid(asn1_node node) {
    int rc, tmp_len;
    char *tmp_buf = NULL;

    if (BPU_asn1ReadValue(&tmp_buf, &tmp_len, 60, "oid", node)) {
        return -1;
    }
    tmp_buf[tmp_len - 1] = '\0';
    BPU_printDebug("data oid: %s", tmp_buf);

    if (!strcmp(tmp_buf, BPU_STR_ASN1_OID_MECS_BASIC_GOPPA)) {
        rc = BPU_EN_MECS_BASIC_GOPPA;
    }
#ifdef BPU_CONF_MECS_CCA2_POINTCHEVAL_GOPPA
    else if (!strcmp(tmp_buf, BPU_STR_ASN1_OID_MECS_CCA2_POINTCHEVAL_GOPPA)) {
        rc = BPU_EN_MECS_CCA2_POINTCHEVAL_GOPPA;
    }
#endif
    else {
        rc = -1;
    }
    free(tmp_buf);

    return rc;
}

int BPU_asn1MallocBuffer(char **buffer, int *len, const int max_len,
                         const asn1_node node, const char *name) {
    int rc;

    rc = asn1_read_value(node, name, NULL, len);

    if (*len > max_len) {
        BPU_printError("requested length %d is bigger than max expected %d",
                       *len, max_len);
        return -1;
    }
    else if (*len > 0) {
        // malloc data
        *buffer = (char *) malloc(*len);

        if (!(*buffer)) {
            BPU_printError("can not allocate memory.");
            return -1;
        }
    }
    else {
        BPU_printError("can not get size of data");
        return rc;
    }
    return 0;
}

int BPU_asn1LoadFileToBuffer(char **buffer, long *size, const char *file_name) {
    FILE *fp = fopen(file_name, "rb");
    long tmp;

    if (!fp) {
        BPU_printError("can not open private key file: %s", file_name);
        return -1;
    }
    fseek(fp, 0L, SEEK_END);
    *size = ftell(fp);
    fseek(fp, 0L, SEEK_SET);

    if (*size == -1L) {
        BPU_printError("can get file size");
        fclose(fp);

        return -1;
    }
    *buffer = (char *) malloc(*size);

    if (!*buffer) {
        BPU_printError("can malloc buffer");
        fclose(fp);

        return -1;
    }
    tmp = fread(*buffer, 1, *size, fp);

    if (tmp != *size) {
        BPU_printError
            ("can not read data from file, read only %ld B, instead of %ld B",
             tmp, *size);
        free(*buffer);
        fclose(fp);

        return -1;
    }
    fclose(fp);
    return 0;
}

int BPU_asn1LoadKeyPair(BPU_T_Mecs_Ctx ** ctx, const char *pri_key_file,
                        const char *pub_key_file) {
    if (BPU_asn1LoadPriKey(ctx, pri_key_file)
        || BPU_asn1LoadPubKey(ctx, pub_key_file)) {
        BPU_printError("can not load key pair");
        return -1;
    }
    return 0;
}

int BPU_asn1LoadPriKey(BPU_T_Mecs_Ctx ** ctx, const char *pri_key_file) {
    long size;
    char *buffer;

    if (BPU_asn1LoadFileToBuffer(&buffer, &size, pri_key_file)) {
        return -1;
    }
    if (BPU_asn1DecodePriKey(ctx, buffer, size)) {
        free(buffer);

        return -1;
    }
    free(buffer);

    return 0;
}

int BPU_asn1LoadPubKey(BPU_T_Mecs_Ctx ** ctx, const char *pub_key_file) {
    long size;
    char *buffer;

    if (BPU_asn1LoadFileToBuffer(&buffer, &size, pub_key_file)) {
        return -1;
    }
    if (BPU_asn1DecodePubKey(ctx, buffer, size)) {
        free(buffer);

        return -1;
    }
    free(buffer);

    return 0;
}

int BPU_asn1SaveKeyPair(const BPU_T_Mecs_Ctx * ctx, const char *pri_key_file,
                        const char *pub_key_file) {
    if (BPU_asn1SavePriKey(ctx, pri_key_file)
        || BPU_asn1SavePubKey(ctx, pub_key_file)) {
        BPU_printError("can not save key pair");
        return -1;
    }
    return 0;
}

int BPU_asn1SavePriKey(const BPU_T_Mecs_Ctx * ctx, const char *pri_key_file) {
    char *buffer = NULL;
    int size;

    if (BPU_asn1EncodePriKey(&buffer, &size, ctx)) {
        return -1;
    }
    if (BPU_asn1WriteBufferToFile(buffer, size, pri_key_file)) {
        BPU_printError("can not write pri key data");
        free(buffer);

        return -1;
    }
    free(buffer);

    return 0;
}

int BPU_asn1SavePubKey(const BPU_T_Mecs_Ctx * ctx, const char *pub_key_file) {
    char *buffer = NULL;
    int size;

    if (BPU_asn1EncodePubKey(&buffer, &size, ctx)) {
        return -1;
    }
    if (BPU_asn1WriteBufferToFile(buffer, size, pub_key_file)) {
        BPU_printError("can not write pub key data");
        free(buffer);

        return -1;
    }
    free(buffer);

    return 0;
}

int BPU_asn1WriteBufferToFile(const char *buffer, const long size,
                              const char *file_name) {
    FILE *fp;
    long tmp;

    fp = fopen(file_name, "wb");

    if (!fp) {
        BPU_printError("can not open file %s", file_name);

        return -1;
    }
    tmp = fwrite(buffer, 1, size, fp);

    if (tmp != size) {
        BPU_printError
            ("can not write buffer to file, written %ld B, instead of %ld B",
             tmp, size);
        fclose(fp);

        return -1;
    }
    fclose(fp);

    return 0;
}
#endif // BPU_CONF_ASN1
