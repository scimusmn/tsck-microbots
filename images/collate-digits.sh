#!/bin/bash

num="_PNG/_Numbers"
cd $num

# take care of colon image
magick Numbers-01.png -gravity center -crop 14x48+0+0 colon.png

# crop number images
for img in Numbers-{0[^1],10,11}.png; do
	echo $img
	magick $img -gravity center -crop 32x48+0+0 "_$img"
	identify "_$img"
done


# combine number images
magick _* +append "digits.png"


# convert to white
magick digits.png -channel RGB -white-threshold 1% digits.png
magick colon.png -channel RGB -white-threshold 1% colon.png


# clean up
rm _*
