#
# Generating a proper header for running Xinu on a U-Boot router
#
# $Id$

MKIMAGEUTIL   = mkimage
XINUPATH  = ../../

HOSTCC    = gcc -Wall -pedantic -DTEXT_BASE=0xBFC00000 -DUSE_HOSTCC -O -I.

include ${XINUPATH}/compile/mipsVars

${MKIMAGEUTIL}: crc32.c mkimage.c
	${HOSTCC} ${HOSTCCFLAGS} $^ -o $@

clean:
	rm -rf ${MKIMAGEUTIL}
