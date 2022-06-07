#!/bin/sh

gci="gci" # set to whatever 'image-encoder' is on your PATH

$gci \
	--label "digits" --array 10x1 --background gradient-bg.png -i digits.png \
	--header gci_offsets.h --binary images.gci
