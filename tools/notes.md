notes
=====

gci file format
---------------

word = 2 bytes
3 word header:
[width], [height], 0x1000

following is raw pixel data, arranged as sequential rows, left to right and top to bottom.
i am not yet sure what format it is exactly (655, 565, or 556)
