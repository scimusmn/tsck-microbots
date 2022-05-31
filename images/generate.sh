#!/bin/bash

gci \
	--label "background" -i gradient-bg.png \
	--label "digit0" --background "gradient-bg.png" --offset 192x297 --array 10x1 -i digits.png \
	--label "digit1" --background "gradient-bg.png" --offset 273x297 --array 10x1 -i digits.png \
	--label "colon"  --background "gradient-bg.png" --offset 354x297 -i colon.png \
	--label "digit2" --background "gradient-bg.png" --offset 435x297 --array 10x1 -i digits.png \
	--label "digit3" --background "gradient-bg.png" --offset 516x297 --array 10x1 -i digits.png \
	--label "health_good" -i health-good.png \
	--label "health_bad" -i health-bad.png \
	--label "heart" --background "gradient-bg.png" --offset 340x61 -i heart.png \
	--gci images.gci --header ../software/microbots-arduino/offsets.h
