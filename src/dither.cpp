#include <stdio.h>
#include <unistd.h>
#include <string.h>
#include "Image.h"
#include "Ditherer.h"


int main(int argc, char** argv)
{
    // get command line options
    int opt;
    char inputFileName[256];
    char outputFileName[256];
    char paletteString[256];
    while ((opt = getopt(argc, argv, "i:o:p:")) != -1)
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
        }
    }

    // open input file
    Image inputImage(inputFileName);
    Ditherer* fsDitherer = Ditherer::createFloydSteinbergDitherer();

    // check chosen palette
    Palette* p;
    if (strcmp(paletteString, "bw") == 0)
    {
        p = new Palette(2);
        Color black;
        black.rgb[0] = 0.0;
        black.rgb[1] = 0.0;
        black.rgb[2] = 0.0;

        Color white;
        black.rgb[0] = 1.0;
        black.rgb[1] = 1.0;
        black.rgb[2] = 1.0;

        p->setColorAtIndex(black, 0);
        p->setColorAtIndex(white, 1);
    }
    else if (strcmp(paletteString, "bg") == 0)
    {
        p = new Palette(2);
        Color black;
        black.rgb[0] = 0.0;
        black.rgb[1] = 0.0;
        black.rgb[2] = 0.0;

        Color green;
        black.rgb[0] = 0.0;
        black.rgb[1] = 1.0;
        black.rgb[2] = 0.0;

        p->setColorAtIndex(black, 0);
        p->setColorAtIndex(green, 1);
    }
    else if (strcmp(paletteString, "cga") == 0)
    {
        p = new Palette(4);

        Color black;
        black.rgb[0] = 0.0;
        black.rgb[1] = 0.0;
        black.rgb[2] = 0.0;

        Color white;
        white.rgb[0] = 1.0;
        white.rgb[1] = 1.0;
        white.rgb[2] = 1.0;

        Color magenta;
        magenta.rgb[0] = 1.0;
        magenta.rgb[1] = 85.0/255.0;
        magenta.rgb[2] = 1.0;

        Color cyan;
        cyan.rgb[0] = 85.0/255.0;
        cyan.rgb[1] = 1.0;
        cyan.rgb[2] = 1.0;

        p->setColorAtIndex(black, 0);
        p->setColorAtIndex(white, 1);
        p->setColorAtIndex(cyan, 2);
        p->setColorAtIndex(magenta, 3);
    }
    
    Image* outputImage = fsDitherer->createDitheredImageFromImageWithPalette(inputImage, *p);
    outputImage->writePPM(outputFileName);
}