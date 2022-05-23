#!/bin/bash

gci \
	--label "background" -i gradient-bg.png \
	--label "digit0" --background "gradient-bg.png" --offset 212x297 --array 10x1 -i digits.png \
	--label "digit1" --background "gradient-bg.png" --offset 293x297 --array 10x1 -i digits.png \
	--label "digit2" --background "gradient-bg.png" --offset 374x297 --array 10x1 -i digits.png \
	--label "digit3" --background "gradient-bg.png" --offset 455x297 --array 10x1 -i digits.png \
	--gci images.gci --header ../software/microbots-arduino/offsets.h
