#/*
# This file is part of BitPunch
# Copyright (C) 2015 Frantisek Uhrecky <frantisek.uhrecky[what here]gmail.com>

# This program is free software: you can redistribute it and/or modify
# it under the terms of the GNU General Public License as published by
# the Free Software Foundation, either version 3 of the License, or
# (at your option) any later version.

# This program is distributed in the hope that it will be useful,
# but WITHOUT ANY WARRANTY; without even the implied warranty of
# MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
# GNU General Public License for more details.

# You should have received a copy of the GNU General Public License
# along with this program.  If not, see <http://www.gnu.org/licenses/>.
#*/
#DEFINES += BPU_CONF_FULL_MECS BPU_CONF_NO_PRINT BPU_CONF_MECS_CCA2_POINTCHEVAL_QCMDPC
DEFINES += BPU_CONF_FULL

TEMPLATE = app
CONFIG += console
CONFIG -= app_bundle
CONFIG -= qt

INCLUDEPATH += \
    lib/src/
SOURCES += \
    lib/src/bitpunch/code/goppa.c \
    lib/src/bitpunch/crypto/hash.c \
    lib/src/bitpunch/crypto/mecs.c \
    lib/src/bitpunch/math/gf2.c \
    lib/src/bitpunch/math/gf2x.c \
    lib/src/bitpunch/math/int.c \
    lib/src/bitpunch/math/mathctx.c \
    lib/src/bitpunch/math/perm.c \
    lib/src/bitpunch/math/permtypes.c \
    lib/src/bitpunch/debugio.c \
    lib/src/bitpunch/code/codectx.c \
    lib/src/bitpunch/crypto/mecsctx.c \
    lib/src/bitpunch/prng/prng.c \
    lib/src/bitpunch/math/gf2types.c \
    lib/src/bitpunch/math/gf2xtypes.c \
    lib/src/bitpunch/code/goppa/goppa.c \
    lib/src/bitpunch/crypto/mecsbasic/mecsbasic.c \
    lib/src/bitpunch/crypto/padding/padding.c \
	lib/src/main.c \
    lib/src/bitpunch/code/goppa/goppatypes.c \
    lib/src/test-speed.c \
    lib/src/bitpunch/crypto/hash/polarssl/sha512.c \
    lib/src/bitpunch/crypto/hash/sha512.c \
    lib/src/bitpunch/crypto/hash/polarssl/sha512.c \
    lib/src/bitpunch/crypto/cca2/mecspointcheval.c \
    lib/src/bitpunch/asn1/asn1.c \
    lib/src/bitpunch/code/qcmdpc/qcmdpc.c \
    lib/src/bitpunch/code/qcmdpc/qcmdpctypes.c

#include(deployment.pri)
#qtcAddDeployment()

HEADERS += \
    lib/src/bitpunch/code/codectx.h \
    lib/src/bitpunch/crypto/mecs.h \
    lib/src/bitpunch/crypto/prng.h \
    lib/src/bitpunch/math/gf2.h \
    lib/src/bitpunch/math/gf2types.h \
    lib/src/bitpunch/math/gf2x.h \
    lib/src/bitpunch/math/gf2xtypes.h \
    lib/src/bitpunch/math/int.h \
    lib/src/bitpunch/math/mathctx.h \
    lib/src/bitpunch/math/perm.h \
    lib/src/bitpunch/math/permtypes.h \
    lib/src/bitpunch/debugio.h \
    lib/src/bitpunch/crypto/mecsctx.h \
    lib/src/bitpunch/code/goppatypes.h \
    lib/src/bitpunch/prng/prng.h \
    lib/src/bitpunch/errorcodes.h \
    lib/src/bitpunch/code/goppa/goppa.h \
    lib/src/bitpunch/code/goppa/goppatypes.h \
    lib/src/bitpunch/crypto/mecsbasic/mecsbasic.h \
    lib/src/bitpunch/crypto/padding/padding.h \
    lib/src/bitpunch/version.h \
    lib/src/bitpunch/bitpunch.h \
    lib/src/bitpunch/crypto/hash/polarssl/polarssl/sha512.h \
    lib/src/bitpunch/crypto/hash/sha512.h \
    lib/src/bitpunch/config.h \
    lib/src/bitpunch/crypto/cca2/mecspointcheval.h \
    lib/src/bitpunch/asn1/asn1.h \
    lib/src/bitpunch/code/qcmdpc/qcmdpc.h \
    lib/src/bitpunch/code/qcmdpc/qcmdpctypes.h

OTHER_FILES += \
    lib/makefile

DISTFILES += \
    lib/asn1/MecsPriKey.asn \
    lib/asn1/MecsPubKey.asn
