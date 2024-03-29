//
//  Image.hpp
//  imconv
//
//  Created by Michael Hill on 2/20/17.
//  Copyright © 2017 Michael Hill. All rights reserved.
//

#ifndef Image_hpp
#define Image_hpp

#include <stdio.h>

typedef struct
{
    int paletteIndex;
    int numPixels;
} ColorRank;

class Pixel
{
public:
    Pixel();
    void fromPixel(const Pixel& pixel)
    {
        for (int c = 0; c < 3; c++)
        {
            rgb[c] = pixel.rgb[c];
        }
        palette_index = pixel.palette_index;
    }
    
    float rgb[3];
    int palette_index = -1;
};

class Color
{
public:
    Color();
    Color(float r, float g, float b);
    float distanceFromColor(const Color& color);
    void fromPixel(const Pixel& pixel);
    void fromColor(const Color& color);
    void set(float r, float g, float b)
    {
        rgb[0] = r;
        rgb[1] = g;
        rgb[2] = b;
    }

    void clip()
    {
        for (int i = 0; i < 3; i++)
        {
            if (rgb[i] > 1.0)
            {
                rgb[i] = 1.0;
            }
            else if (rgb[i] < 0.0)
            {
                rgb[i] = 0.0;
            }
        }
    }
    
    void getGrayAndColorComponents(Color& gray, Color& col);
    
    float rgb[3];
};

class Palette
{
public:
    Palette();
    Palette(int numColors);
    Palette(unsigned char* colorValues, int numColors);
    Palette(const Palette& p);
    
    void setNumColors(int numColors);
    void getClosestColorTo(const Color& inColor, Color& outColor, int& index, bool includeGrayscale = true, int excludeColorIndex = -1) const;
    Color* colorAtIndex(int index) const;
    void setColorAtIndex(const Color& color, int index) const;
    int getNumColors() const
    {
        return numColors;
    }
    
    ~Palette();
private:
    int numColors;
    Color* colors = NULL;
};

class Image
{
public:
    Image(int w, int h);
    Image(int w, int h, unsigned char* pixels);
    Image(int w, int h, int bytesPerRow, int bytesPerPixel, unsigned char* pixels);
    Image(const Image& im);
    Image(const Image& im, int w, int h);
    Image(const char* fname);
    Image(FILE* fp);
    
    void fromSubImage(const Image& im, int xOffset, int width, int yOffset, int height);
    Pixel* pixelAt (int w, int h) const;
    void writePPM(const char* fname);
    void writePPM(FILE* fp);
    void writeXBM(const char* fname);
    void writeXBM(FILE* fp);
    void writeRGB3(const char* fname);
    void writeRGB3(FILE* fp);
    void writeRGB8(const char* fname);
    void writeRGB8(FILE* fp);
    void writePBM(const char* fname);
    void writePBM(FILE* fp);
    void writeBMP(const char* fname);
    void writeBW(const char* fname);
    void writeBW(FILE* fp);
    void writeRawMono(const char* fname);
    void writeRawMono(FILE* fp);
    int getWidth() const { return width; }
    int getHeight() const { return height; }
    void getAvgColor(Color& color);
    void getSecondaryColor(Color& firstColor, Color& secondaryColor, bool useColorVector);
    
    void colorHistogram();
    float getErrorFromImage(const Image& im);
    void copyFromImageAtPosition(const Image& im, int xOffset, int yOffset);
    
    ~Image();
private:
    float boost = 1.0;
    int width;
    int height;
    Pixel* pixels = NULL;
    
    void initWithData(unsigned char* pixels, int bytesPerRow, int bytesPerPixel, int redIndex, int greenIndex, int blueIndex);
    void initWithPPM(FILE* fp);
    void initWithPNG(const char* fname);
};

#endif /* Image_hpp */
