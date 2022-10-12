gci-encoder
===========

Convert normal images to GCI files.

This program requires CMake to build, but has no other dependencies.


build
-----

```bash
mkdir build
cd build
cmake ..
make
```

Optionally, you can also build the tests with `make test-image-encoder`.


usage
-----

```
Usage: gci [OPTIONS]
Convert images into GCI data for use on 4D Systems displays
  -i, --image=FILENAME      Add an image to the outputs.
  --label=LABEL             Set an explicit name for the next image (defaults to the image filename)
  --array=WIDTHxHEIGHT      Set the next image to be an array of WIDTH * HEIGHT sub-images
  --background=FILENAME     Set the next image to use FILENAME as a background image.
  --offset=XxY              When replacing background pixels, start at (X, Y) in the background image rather than (0, 0).
  --gci=FILENAME            Set the binary output file. (default: output.gci)
  --header=FILENAME         Set the C constants output file. If omitted, output to stdout.
  -h, --help                Show help information
```
