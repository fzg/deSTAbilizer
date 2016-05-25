#!/bin/sh -x
set -x

#skip 64b (uImage header) -> zImage

dd if=$1 of=$1.zimage bs=64 skip=1

#
# then find out the gzip-compressed initrd offset
#arm-none-eabi-objdump -EL -b binary -D -m armv5t 0 | grep 00088b1f

img_offset=$(arm-none-eabi-objdump -EL -b binary -D -m armv5t $1.zimage | grep 00088b1f -m 1 | cut -d ":" -f 1)
img_offset=$(expr $img_offset + 0)
img_offset=$(echo "ibase=16;"$img_offset | bc)

dd if=$1.zimage of=$1.kernel_loader bs=1 count=$img_offset

dd if=$1.zimage of=$1.kernel.gz bs=1 skip=$img_offset
gunzip -k $1.kernel.gz

# Second uImage
dd if=$1.initrd.gz of=$1.initrd.uim2 bs=1 skip=1559596
# Squashfs Image
dd if=$1.initrd.uim2 of=$1.initrd.squashfs bs=64 skip=1

#  -> Todo: figure out the custom squashfs params,
#  in order to be able to repack
#

sasquatch initrd.kern

