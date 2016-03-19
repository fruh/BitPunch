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
#ifndef ASN1_H
#define ASN1_H

#include <bitpunch/config.h>

#ifdef BPU_CONF_ASN1
#include <bitpunch/crypto/mecsctx.h>

#include <stdint.h>
#include <libtasn1.h>

#define BPU_STR_ASN1_MECS_PRI_KEY "asn1/MecsPriKey.asn"
#define BPU_STR_ASN1_MECS_PUB_KEY "asn1/MecsPubKey.asn"
#define BPU_STR_ASN1_MECS_PRI_KEY_NAME "BPU_ASN1.MecsPriKey"
#define BPU_STR_ASN1_MECS_PUB_KEY_NAME "BPU_ASN1.MecsPubKey"

/// src: http://www.hrz.tu-darmstadt.de/itsicherheit/object_identifier/oids_der_informatik__cdc/oids_des_fachgebietes_cdc_der_informatik.de.jsp
#define BPU_STR_ASN1_OID_MECS_BASIC_GOPPA "1.3.6.1.4.1.8301.3.1.3.4.1"
/// src: http://www.hrz.tu-darmstadt.de/itsicherheit/object_identifier/oids_der_informatik__cdc/oids_des_fachgebietes_cdc_der_informatik.de.jsp
#define BPU_STR_ASN1_OID_MECS_CCA2_POINTCHEVAL_GOPPA "1.3.6.1.4.1.8301.3.1.3.4.2.2"

/**
 * @brief BPU_asn1EncodePriKey dsadsad
 * @param buffer fds
 * @param size ddd
 * @param ctx dda
 * @return 0
 */
/// sasa
int BPU_asn1EncodePriKey(char **buffer, int *size, const BPU_T_Mecs_Ctx * ctx);

int BPU_asn1DecodePriKey(BPU_T_Mecs_Ctx ** ctx, const char *buffer,
                         const int size);

int BPU_asn1EncodePubKey(char **buffer, int *size, const BPU_T_Mecs_Ctx * ctx);

int BPU_asn1DecodePubKey(BPU_T_Mecs_Ctx ** ctx, const char *buffer,
                         const int size);

int BPU_asn1ReadValue(char **buffer, int *len, const int max_len,
                      const char *name, const asn1_node node);

BPU_T_EN_Mecs_Types BPU_asn1GetMecsTypeFromOid(asn1_node node);

int BPU_asn1MallocBuffer(char **buffer, int *len, const int max_len,
                         const asn1_node node, const char *name);

int BPU_asn1LoadFileToBuffer(char **buffer, long *size, const char *file_name);

int BPU_asn1LoadKeyPair(BPU_T_Mecs_Ctx ** ctx, const char *pri_key_file,
                        const char *pub_key_file);

int BPU_asn1LoadPriKey(BPU_T_Mecs_Ctx ** ctx, const char *pri_key_file);

int BPU_asn1LoadPubKey(BPU_T_Mecs_Ctx ** ctx, const char *pub_key_file);

int BPU_asn1SaveKeyPair(const BPU_T_Mecs_Ctx * ctx, const char *pri_key_file,
                        const char *pub_key_file);

int BPU_asn1SavePriKey(const BPU_T_Mecs_Ctx * ctx, const char *pri_key_file);

int BPU_asn1SavePubKey(const BPU_T_Mecs_Ctx * ctx, const char *pub_key_file);

int BPU_asn1WriteBufferToFile(const char *buffer, const long size,
                              const char *file_name);

#endif // BPU_CONF_ASN1
#endif // ASN1_H
