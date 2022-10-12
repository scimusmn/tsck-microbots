path-encoder
============

Convert human-readable (and craftable) path files to custom binary format.


build
-----

```bash
mkdir build
cd build
cmake ..
make
```


usage
-----

Make a 128x128 image file. Any black pixels are "banned" areas; any other pixels are "permitted" areas. I typically use B&W for my paths.

Next, place a single red (#ff0000) pixel at the path start, a single blue (#0000ff) pixel at the path end, and dot purple (#ff00ff) pixels along the path so that each purple pixel is closest to only the two other pixels in the correct pathing order. Try to minimize purple pixel count.

Finally, just do `path-encoder [INPUT_IMAGE] [OUTPUT_FILENAME]`.
