# libdither
Library and tools for various dithering effects on images

![dither](https://github.com/johnboiles/libdither/assets/218876/b1ac1c28-4f50-40c8-8a5e-eb3f44987a3b)

## Compiling

Simply run

```sh
make
```

The resulting binary file will be in `./bin/dither`.

## Usage

Run it like this:

```sh
./bin/dither -i input.png -p bw -d at -o output.png
```

Options are as follows:
- `-i <inputFileName>`: Specifies the input file path. Supports the following file extensions: `.png`, `.ppm`
- `-o <outputFileName>`: Specifies the output file path. Supports the following file extensions: `.xbm`, `.3b`, `.8b`, `.pbm`, `.1b`, `.gray`, `.bmp`.
- `-p <paletteString>`: Specifies the palette to be used for dithering. The palette determines the set of colors available for representing the image. The available palette options are:
  - `bw` or `1bit`: Black and white palette (2 colors).
  - `bg`: Black and green palette (2 colors).
  - `amber`: Black and amber palette (2 colors).
  - `cga`: CGA (Color Graphics Adapter) palette (4 colors).
  - `cga2`: Alternate CGA palette (4 colors).
  - `xmas`: Christmas palette (4 colors).
  - `rgb`: RGB (Red, Green, Blue) palette (5 colors).
  - `3bit`: 3-bit palette (8 colors).
  - `8bit`: 8-bit palette (256 colors).
- `-d <dithererString>`: Specifies the dithering algorithm to be used. Dithering is a technique used to simulate additional colors and shades by using patterns of different colored pixels. The available ditherer options are:
  - `at`: Atkinson dithering algorithm.
  - `c64`: Commodore 64 dithering algorithm.
  - `floyd`: Floyd-Steinberg dithering algorithm.
