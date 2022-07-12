#!/bin/bash

BG="_PNG/_Screens/Main_Screen.png"

gci \
	--label "game_over_screen" -i _PNG/_Screens/Gameover_Screen.png \
	--label "resetting_screen" -i _PNG/_Screens/Resetting_Screen.png \
	--label "attract_screen"   -i _PNG/_Screens/Start_Screen.png \
	--label "success_screen"   -i _PNG/_Screens/Sucess_Screen.png \
	--label "background"       -i _PNG/_Screens/Main_Screen.png \
	--label "digit0" --background $BG --offset  22x415 --array 10x1 -i _PNG/_Numbers/digits.png \
	--label "digit1" --background $BG --offset  59x415 --array 10x1 -i _PNG/_Numbers/digits.png \
	--label "colon"  --background $BG --offset  95x415 -i _PNG/_Numbers/colon.png \
	--label "digit2" --background $BG --offset 116x415 --array 10x1 -i _PNG/_Numbers/digits.png \
	--label "digit3" --background $BG --offset 153x415 --array 10x1 -i _PNG/_Numbers/digits.png \
	--label "health9" --background $BG --offset 232x97 -i _PNG/_HeartBar/FullHeart.png \
	--label "health8" --background $BG --offset 232x97 -i _PNG/_HeartBar/1.9_Heart.png \
	--label "health7" --background $BG --offset 232x97 -i _PNG/_HeartBar/2.9_Heart.png \
	--label "health6" --background $BG --offset 232x97 -i _PNG/_HeartBar/3.9_Heart.png \
	--label "health5" --background $BG --offset 232x97 -i _PNG/_HeartBar/4.9_Heart.png \
	--label "health4" --background $BG --offset 232x97 -i _PNG/_HeartBar/5.9_Heart.png \
	--label "health3" --background $BG --offset 232x97 -i _PNG/_HeartBar/6.9_Heart.png \
	--label "health2" --background $BG --offset 232x97 -i _PNG/_HeartBar/7.9_Heart.png \
	--label "health1" --background $BG --offset 232x97 -i _PNG/_HeartBar/8.9_Heart.png \
	--label "health0" --background $BG --offset 232x97 -i _PNG/_HeartBar/9.9_Heart.png \
	--gci images.gci --header ../software/microbots-arduino/offsets.h
