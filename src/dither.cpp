#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <string.h>
#include "Image.h"
#include "Ditherer.h"

unsigned char c64_colors[] =
{
    0,      0,      0,
    255,    255,    255,
    136,    0,      0,
    170,    255,    238,
    204,    68,     204,
    0,      204,    85,
    0,      0,      170,
    238,    238,    119,
    221,    136,    85,
    102,    68,     0,
    255,    119,    119,
    51,     51,     51,
    119,    119,    119,
    170,    255,    102,
    0,      136,    255,
    187,    187,    187
};

int num_64_colors = 16;

typedef enum {
    PPM,
    XBM
} output_file_type_t;

bool has_suffix(char* str, const char* suffix) {
    int str_len = strlen(str);
    int suffix_len = strlen(suffix);

    if (str_len < suffix_len) {
        return false;
    }

    if (strcmp(&str[str_len - suffix_len], suffix) == 0) {
        return true;
    }

    return false;
}

int main(int argc, char** argv)
{
    // get command line options
    int opt;
    char inputFileName[256];
    char outputFileName[256];
    char paletteString[256];
    char dithererString[256];

    memset(inputFileName, 0, 256);
    memset(outputFileName, 0, 256);
    memset(paletteString, 0, 256);
    memset(dithererString, 0, 256);

    output_file_type_t outputFileType = PPM;

    while ((opt = getopt(argc, argv, "i:o:p:d:")) != -1)
    {
        switch (opt)
        {
            case 'i':
                strcpy(inputFileName, optarg);
                break;
            case 'o':
                strcpy(outputFileName, optarg);
                break;
            case 'p':
                strcpy(paletteString, optarg);
                break;
            case 'd':
                strcpy(dithererString, optarg);
                break;
        }
    }

    if (strlen(inputFileName) == 0) {
        printf("error: no input file name specified.\n");
        exit(1);
    }

    if (strlen(outputFileName) == 0) {
        printf("error: no output file name specified.\n");
        exit(1);
    }

    if (has_suffix(outputFileName, ".xbm") || has_suffix(outputFileName, ".xbm")) {
        outputFileType = XBM;
    }

    // open input file
    Image inputImage(inputFileName);
    Image* outputImage;
    Ditherer* ditherer;

    if (strcmp(dithererString, "c64") == 0)
    {
        Ditherer* c64Ditherer = Ditherer::createC64Ditherer();
        Palette c64palette(c64_colors, num_64_colors);

        outputImage = c64Ditherer->createDitheredImageFromImageWithPalette(inputImage, c64palette);
    }
    else
    {
        if (strcmp(dithererString, "at") == 0)
        {
            ditherer = Ditherer::createAtkinsonDitherer();
        }
        else
        {
            ditherer = Ditherer::createFloydSteinbergDitherer();
        }

        // check chosen palette
        Palette* p;
        if (strcmp(paletteString, "bw") == 0)
        {
            p = new Palette(2);
            Color black(0,0,0);
            Color white(1,1,1);

            p->setColorAtIndex(black, 0);
            p->setColorAtIndex(white, 1);
        }
        else if (strcmp(paletteString, "bg") == 0)
        {
            p = new Palette(2);
            Color black(0,0,0);
            Color green(0,1,0);

            p->setColorAtIndex(black, 0);
            p->setColorAtIndex(green, 1);
        }
        else if (strcmp(paletteString, "amber") == 0)
        {
            p = new Palette(2);
            Color black(0,0,0);
            Color amber(1,0.75,0);

            p->setColorAtIndex(black, 0);
            p->setColorAtIndex(amber, 1);
        }
        else if (strcmp(paletteString, "cga") == 0)
        {
            p = new Palette(4);

            Color black(0,0,0);
            Color white(1,1,1);
            Color magenta(1.0 ,85.0/255.0, 1.0);
            Color cyan(85.0/255.0, 1.0, 1.0);
            
            p->setColorAtIndex(black, 0);
            p->setColorAtIndex(white, 1);
            p->setColorAtIndex(cyan, 2);
            p->setColorAtIndex(magenta, 3);
        }
        else if (strcmp(paletteString, "cga2") == 0)
        {
            p = new Palette(4);

            Color black(0,0,0);
            Color green(85.0/255.0, 1.0, 85.0/255.0);
            Color red(1.0 ,85.0/255.0, 85.0/255.0);
            Color yellow(1.0, 1.0, 85.0/255.0);
            
            p->setColorAtIndex(black, 0);
            p->setColorAtIndex(green, 1);
            p->setColorAtIndex(red, 2);
            p->setColorAtIndex(yellow, 3);
        }
        else if (strcmp(paletteString, "xmas") == 0)
        {
            p = new Palette(4);

            Color black(0,0,0);
            Color green(0, 1.0, 0.0);
            Color red(1.0 , 0.0, 0.0);
            Color gold(1.0, 215.0/255.0, 0.0);

            p->setColorAtIndex(black, 0);
            p->setColorAtIndex(green, 1);
            p->setColorAtIndex(red, 2);
            p->setColorAtIndex(gold, 3);
        }
        else if (strcmp(paletteString, "rgb") == 0)
        {
            p = new Palette(5);

            Color black(0,0,0);
            Color red(1,0,0);
            Color green(0,1.0,0);
            Color blue(0,0,1.0);
            Color white(1,1,1);

            p->setColorAtIndex(black, 0);
            p->setColorAtIndex(red, 1);
            p->setColorAtIndex(green, 2);
            p->setColorAtIndex(blue, 3);
            p->setColorAtIndex(white, 4);
        }

        outputImage = ditherer->createDitheredImageFromImageWithPalette(inputImage, *p);
    }

    if (outputFileType == PPM) {
        outputImage->writeXBM(outputFileName);
    } else if (outputFileType == XBM) {
        outputImage->writeXBM(outputFileName);
    }
}