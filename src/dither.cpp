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
    while ((opt = getopt(argc, argv, "i:o:")) != -1)
    {
        switch (opt)
        {
            case 'i':
                strcpy(inputFileName, optarg);
                break;
            case 'o':
                strcpy(outputFileName, optarg);
                break;
        }
    }

    // open input file
    Image inputImage(inputFileName);

    Ditherer* fsDitherer = Ditherer::createFloydSteinbergDitherer();

    /*
    Palette p(2);

    Color black;
    black.rgb[0] = 0.0;
    black.rgb[1] = 0.0;
    black.rgb[2] = 0.0;

    Color white;
    black.rgb[0] = 1.0;
    black.rgb[1] = 1.0;
    black.rgb[2] = 1.0;

    p.setColorAtIndex(black, 0);
    p.setColorAtIndex(white, 1);
    */

    Palette p(4);

    Color black;
    black.rgb[0] = 0.0;
    black.rgb[1] = 0.0;
    black.rgb[2] = 0.0;

    Color white;
    white.rgb[0] = 1.0;
    white.rgb[1] = 1.0;
    white.rgb[2] = 1.0;

    Color red;
    red.rgb[0] = 1.0;
    red.rgb[1] = 0.0;
    red.rgb[2] = 0.0;

    Color green;
    green.rgb[0] = 0.0;
    green.rgb[1] = 1.0;
    green.rgb[2] = 0.0;

    Color blue;
    blue.rgb[0] = 0.0;
    blue.rgb[1] = 0.0;
    blue.rgb[2] = 1.0;

    printf("a\n");

    p.setColorAtIndex(black, 0);
    //p.setColorAtIndex(white, 1);
    p.setColorAtIndex(red, 1);
    p.setColorAtIndex(green, 2);
    p.setColorAtIndex(blue, 3);

    printf("here\n");

    fsDitherer->ditherImageInPlaceWithPalette(inputImage, p);

    inputImage.writePPM(outputFileName);



    /*
    Image im(10,10);
    for (int i = 0; i < 10; i++)
    {
        Pixel* p = im.pixelAt(i,i);
        p->rgb[0] = 1.0;
        p->rgb[1] = 0.0;
        p->rgb[2] = 0.0;
    }

    im.writePPM("test.ppm");
    printf("done.\n");
    */
}