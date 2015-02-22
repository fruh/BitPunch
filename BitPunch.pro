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
    lib/src/tmp/arithmetics.c \
    lib/src/tmp/debugio.c \
    lib/src/tmp/init.c \
    lib/src/tmp/main.c \
    lib/src/tmp/mceliece.c \
    lib/src/tmp/operations.c \
    lib/src/tmp/process.c \
    lib/src/arithmetics.c \
    lib/src/init.c \
    lib/src/main.c \
    lib/src/operations.c \
    lib/src/process.c \
    lib/src/test-speed.c \
    lib/src/bitpunch/code/codectx.c \
    lib/src/bitpunch/crypto/mecsctx.c \
    lib/src/bitpunch/prng/prng.c \
    lib/src/bitpunch/math/gf2types.c \
    lib/src/bitpunch/math/gf2xtypes.c \
    lib/src/bitpunch/code/goppa/goppa.c \
    lib/src/bitpunch/crypto/mecsbasic/mecsbasic.c \
    lib/src/bitpunch/crypto/padding/padding.c \
    lib/src/bitpunch/code/goppa/goppatypes.c

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
    lib/src/tmp/arithmetics.h \
    lib/src/tmp/debugio.h \
    lib/src/tmp/globals.h \
    lib/src/tmp/init.h \
    lib/src/tmp/mceliece.h \
    lib/src/tmp/operations.h \
    lib/src/tmp/process.h \
    lib/src/tmp/types.h \
    lib/src/arithmetics.h \
    lib/src/debugio.h \
    lib/src/globals.h \
    lib/src/init.h \
    lib/src/mceliece.h \
    lib/src/operations.h \
    lib/src/process.h \
    lib/src/types.h \
    lib/src/version.h \
    lib/src/bitpunch/crypto/mecsctx.h \
    lib/src/bitpunch/code/goppatypes.h \
    lib/src/bitpunch/prng/prng.h \
    lib/src/bitpunch/errorcodes.h \
    lib/src/bitpunch/code/goppa/goppa.h \
    lib/src/bitpunch/code/goppa/goppatypes.h \
    lib/src/bitpunch/crypto/mecsbasic/mecsbasic.h \
    lib/src/bitpunch/crypto/padding/padding.h \
    lib/src/bitpunch/version.h \
    lib/src/bitpunch/bitpunch.h

OTHER_FILES += \
    lib/makefile
