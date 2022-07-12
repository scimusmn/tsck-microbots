#!/bin/bash

mount /dev/mmcblk0p1 /mnt
cp images.gci /mnt/
sync
umount /mnt
