//
//  Image.cpp
//  imconv
//
//  Created by Michael Hill on 2/20/17.
//  Copyright © 2017 Michael Hill. All rights reserved.
//

#include <stdio.h>
#include <stdlib.h>
#include "Image.h"
#include "math.h"
#include "../upng/upng.h"
#include <string.h>
#include <map>
#include <string>
#include <list>
#include "../BMP-C/src/bmpfile.h"

using std::map;
using std::string;
using std::list;

Color::Color()
{
    for (int i = 0; i < 3; i++)
    {
        rgb[i] = 0.0;
    }
}

Color::Color(float r, float g, float b)
{
    rgb[0] = r;
    rgb[1] = g;
    rgb[2] = b;
}

float Color::distanceFromColor(const Color &color)
{
    float error = 0;
    for (int i = 0; i < 3; i++)
    {
        float c = (color.rgb[i] - rgb[i]);
        error += (c*c);
    }
    
    return error;
}

void Color::fromPixel(const Pixel &pixel)
{
    for (int i = 0; i < 3; i++)
    {
        rgb[i] = pixel.rgb[i];
    }
}

void Color::fromColor(const Color &color)
{
    for (int i = 0; i < 3; i++)
    {
        rgb[i] = color.rgb[i];
    }
}

void Color::getGrayAndColorComponents(Color &gray, Color &col)
{
    float minVal = 999.9;
    for (int i = 0; i < 3; i++)
    {
        if (rgb[i] < minVal)
        {
            minVal = rgb[i];
        }
    }
    
    gray.set(minVal, minVal, minVal);
    col.set(rgb[0]-minVal, rgb[1]-minVal, rgb[2]-minVal);
}

Palette::Palette()
: Palette(NULL, 0)
{
    
}

Palette::Palette(int nc)
: Palette(NULL, nc)
{
    
}

// Palette
Palette::Palette(unsigned char* colorValues, int nc)
: numColors(nc)
{
    // create colors from init values
    if (numColors > 0)
    {
        colors = new Color[numColors];
    }
    
    if (colorValues)
    {
        unsigned char* cc = colorValues;
        for (int i = 0; i < numColors; i++)
        {
            for (int c = 0; c < 3; c++)
            {
                colors[i].rgb[c] = ((float)(*cc) / 255.0);
                cc++;
            }
        }
    }
}

Palette::Palette(const Palette& p)
{
    numColors = p.getNumColors();
    if (numColors > 0)
    {
        colors = new Color[numColors];
    }
    
    for (int i = 0; i < numColors; i++)
    {
        Color* c = p.colorAtIndex(i);
        colors[i].fromColor(*c);
    }
}

void Palette::setNumColors(int numColors)
{
    this->numColors = numColors;
    if (colors)
    {
        delete colors;
        colors = NULL;
    }
    
    colors = new Color[this->numColors];
}

void Palette::getClosestColorTo(const Color& inColor, Color& outColor, int& index, bool includeGrayscale, int excludeColorIndex) const
{
    float lowestError = 999999.9;
    int lowestErrorIndex = -1;
    
    for (int i = 0; i < numColors; i++)
    {
        bool useColor = true;
        Color* c = colorAtIndex(i);
        
        if (!includeGrayscale)
        {
            if (c->rgb[0] == c->rgb[1] && c->rgb[0] == c->rgb[2])
            {
                useColor = false;
            }
        }
        
        if (excludeColorIndex == i)
        {
            useColor = false;
        }
            
        if (useColor)
        {
            float error = c->distanceFromColor(inColor);
            if (error < lowestError)
            {
                lowestError = error;
                lowestErrorIndex = i;
            }
        }
    }
    
    index = lowestErrorIndex;
    outColor.fromColor(*colorAtIndex(lowestErrorIndex));
}

Color* Palette::colorAtIndex(int index) const
{
    return &colors[index];
}

void Palette::setColorAtIndex(const Color &color, int index) const
{
    Color* c = colorAtIndex(index);
    c->fromColor(color);
}

Palette::~Palette()
{
    if (colors)
    {
        delete colors;
    }
}

// Pixel

Pixel::Pixel() {}

Image::Image(int w, int h, unsigned char* pixels)
: Image(w,h)
{
    initWithData(pixels, w*3, 3, 0, 1, 2);
}

Image::Image(int w, int h, int bytesPerRow, int bytesPerPixel, unsigned char* pixels)
: Image(w,h)
{
    initWithData(pixels, bytesPerRow, bytesPerPixel, 2, 1, 0);
}

void Image::initWithData(unsigned char *pixels, int bytesPerRow, int bytesPerPixel, int redIndex, int greenIndex, int blueIndex)
{
    unsigned char* pp = pixels;
    int rgbindices[3];
    rgbindices[0] = redIndex;
    rgbindices[1] = greenIndex;
    rgbindices[2] = blueIndex;
    
    for (int hh = 0; hh < height; hh++)
    {
        pp = pixels + (hh * bytesPerRow);
        for (int ww = 0; ww < width; ww++)
        {
            Pixel* p = pixelAt(ww, hh);
            for (int c = 0; c < 3; c++)
            {
                float pxval = (float)(pp[rgbindices[c]]) / 255.0;
                p->rgb[c] = pxval;
            }
            
            pp += bytesPerPixel;
            
            // apply boost to color portion
            // find min rgb val
            float min = 999.0;
            for (int c = 0; c < 3; c++)
            {
                if (p->rgb[c] < min)
                {
                    min = p->rgb[c];
                }
            }
            
            // remove gray portion and boost color
            for (int c = 0; c < 3; c++)
            {
                p->rgb[c] -= min;
                p->rgb[c] *= boost;
                p->rgb[c] += (min / boost);
                
                if (p->rgb[c] > 1.0)
                    p->rgb[c] = 1.0;
            }
        }
    }
}

Image::Image(const Image& im)
: Image(im.getWidth(), im.getHeight())
{
    for (int hh = 0; hh < height; hh++)
    {
        for (int ww = 0; ww < width; ww++)
        {
            Pixel* p = pixelAt(ww, hh);
            Pixel* in_p = im.pixelAt(ww, hh);
            for (int c = 0; c < 3; c++)
            {
                p->rgb[c] = in_p->rgb[c];
            }
        }
    }
}

Image::Image(const Image& im, int w, int h)
: Image(w, h)
{
    // stretch / shrink input image to fit this image
    float xscale = (float)im.getWidth() / (float)w;
    float yscale = (float)im.getHeight() / (float)h;
    
    for (int y = 0; y < height; y++)
    {
        for (int x = 0; x < width; x++)
        {
            int xcoord = floor((float)x * xscale);
            int ycoord = floor((float)y * yscale);
            
            Pixel* p = pixelAt(x, y);
            Pixel* srcp = im.pixelAt(xcoord, ycoord);
            
            p->fromPixel(*srcp);
        }
    }
}

Image::Image(const char* fname)
{
    // check file name
    int len = strlen(fname);
    if (len > 4 && strcmp(&fname[len-4], ".png") == 0) {
        initWithPNG(fname);
    } else {
        FILE* fp = fopen(fname, "rb");
        initWithPPM(fp);
    }
}

Image::Image(FILE* fp)
{
    initWithPPM(fp);
}

void Image::initWithPPM(FILE *fp)
{
    unsigned char temp[256];
    fread(temp, 1, 2, fp);
    if (temp[0] == 'P' && temp[1] == '6')
    {
        unsigned char ch = ' ';
        unsigned char* strptr;
        while (isspace(ch))
        {
            fread(&ch, 1, 1, fp);
        }
        
        // check for comment
        strptr = temp;
        //fread(strptr, 1, 1, fp);
        temp[0] = ch;
        if (*strptr == '#')
        {
            // skip until newline
            while (*strptr != '\n')
            {
                fread(strptr, 1, 1, fp);
            }
            
            // read one more character
            fread(strptr, 1, 1, fp);
        }
        
        // read width, until whitespace
        while (!isspace(*strptr))
        {
            strptr++;
            fread(strptr, 1, 1, fp);
        }
        // terminate string
        *strptr = 0;
        
        int ww = atoi((const char*)temp);
        
        // read height
        strptr = temp;
        memset(temp, 0, 256);
        
        fread(strptr, 1, 1, fp);
        while (!isspace(*strptr))
        {
            strptr++;
            fread(strptr, 1, 1, fp);
        }
        *strptr = 0;
        
        int hh = atoi((const char*)temp);
        
        // read maxval
        strptr = temp;
        memset(temp, 0, 256);
        
        fread(strptr, 1, 1, fp);
        while (!isspace(*strptr))
        {
            strptr++;
            fread(strptr, 1, 1, fp);
        }
        *strptr = 0;
        
        int maxval = atoi((const char*)temp);
        
        width = ww;
        height = hh;
        // allocate pixels
        pixels = new Pixel[width*height];
        
        unsigned char* pixels = (unsigned char*)malloc(sizeof(unsigned char) * width * height * 3);
        if (maxval <= 255) {
            fread(pixels, 1, width * height * 3, fp);
        } else {
            unsigned char* px = pixels;
            uint16_t r,g,b;
            for (int y = 0; y < height; y++) {
                for (int x = 0; x < width; x++) {
                    fread(&r, 2, 1, fp);
                    fread(&g, 2, 1, fp);
                    fread(&b, 2, 1, fp);
                    
                    px[0] = r >> 8;
                    px[1] = g >> 8;
                    px[2] = b >> 8;
                    px += 3;
                }
            }
        }
            
        initWithData(pixels, width*3, 3, 0, 1, 2);
        free(pixels);
    }
    
    if (fp != stdin)
    {
        fclose(fp);
    }
}

void Image::initWithPNG(const char* fname) {
    upng_t* upng;

    upng = upng_new_from_file(fname);
    if (upng == NULL) {
        printf("error: could not load png %s\n", fname);
        return;
    }

    upng_decode(upng);

    if (upng_get_error(upng) != UPNG_EOK) {
        printf("error: could not decode png %s\n", fname);
        return;
    }

    width = upng_get_width(upng);
    height = upng_get_height(upng);
    upng_format f = upng_get_format(upng);

    uint8_t* buffer = (uint8_t*)upng_get_buffer(upng);
    // allocate pixels
    pixels = new Pixel[width*height];

    // handle different pixel formats
    if (f == UPNG_RGB8) {
        initWithData(buffer, width*3, 3, 0, 1, 2);
    } else if (f == UPNG_RGBA8) {
        initWithData(buffer, width*4, 4, 0, 1, 2);
    } else if (f == UPNG_LUMINANCE8) {
        initWithData(buffer, width, 1, 0, 0, 0);
    } else if (f == UPNG_LUMINANCE_ALPHA8) {
        initWithData(buffer, width*2, 2, 0, 0, 0);
    } else if (f == UPNG_RGBA16) {
        // 16 bit color
        // just use the higher byte (for now)
        initWithData(buffer, width*8, 8, 0, 2, 4);
    } else if (f == UPNG_RGB16) {
        initWithData(buffer, width*6, 6, 0, 2, 4);
    } else {
        printf("format %d unsupported (for now)\n", f); 
    }

    upng_free(upng);
}

Image::Image(int w, int h)
: width(w)
, height(h)
{
    // allocate pixels
    pixels = new Pixel[width*height];
}

Image::~Image()
{
    if (pixels)
    {
        delete pixels;
        pixels = NULL;
    }
}

void Image::fromSubImage(const Image &im, int xOffset, int width, int yOffset, int height)
{
    // copy pixels
    for (int y = 0; y < height; y++)
    {
        for (int x = 0; x < width; x++)
        {
            Pixel* srcPixel = im.pixelAt(xOffset+x, yOffset+y);
            Pixel* dstPixel = pixelAt(x, y);
            
            for (int c = 0; c < 3; c++)
            {
                dstPixel->rgb[c] = srcPixel->rgb[c];
            }
        }
    }
}

void Image::getAvgColor(Color &color)
{
    color.set(0, 0, 0);
    float divisor = float(height*width);
    
    for (int h = 0; h < height; h++)
    {
        for (int w = 0; w < width; w++)
        {
            Pixel* p = pixelAt(w, h);
            for (int c = 0; c < 3; c++)
            {
                color.rgb[c] += p->rgb[c] / divisor;
            }
        }
    }
}

void Image::getSecondaryColor(Color &firstColor, Color &secondaryColor, bool useColorVector)
{
    // get an ordered list of pixels, by descending error
    typedef struct
    {
        Pixel* p;
        float error;
    } PixelError;
    
    Color fromColor;
    if (useColorVector)
    {
        Color gray;
        firstColor.getGrayAndColorComponents(gray, fromColor);
    }
    else
    {
        fromColor.fromColor(firstColor);
    }
    
    list<PixelError> pixelIndicesDescendingError;
    for (int h = 0; h < height; h++)
    {
        for (int w = 0; w < width; w++)
        {
            Pixel* p = pixelAt(w, h);
            Color toColor;
            if (useColorVector)
            {
                Color gray;
                Color c;
                c.fromPixel(*p);
                c.getGrayAndColorComponents(gray, toColor);
            }
            else
            {
                toColor.fromPixel(*p);
            }
            
            float error = 0;
            float diff = 0;
            for (int c = 0; c < 3; c++)
            {
                diff = firstColor.rgb[c] - p->rgb[c];
                error += (diff*diff);
            }
            
            PixelError thisError;
            thisError.p = p;
            thisError.error = error;
            bool found = false;
            for (list<PixelError>::iterator it = pixelIndicesDescendingError.begin();
                 it != pixelIndicesDescendingError.end();
                 it++)
            {
                if (it->error < error)
                {
                    pixelIndicesDescendingError.insert(it, thisError);
                    found = true;
                    break;
                }
            }
            
            if (!found)
            {
                pixelIndicesDescendingError.push_back(thisError);
            }
        }
    }
    
    int halfPixelCount = (height*width) / 2;
    int i = 0;
    float divisor = float(halfPixelCount);
    secondaryColor.set(0, 0, 0);
    for (list<PixelError>::iterator it = pixelIndicesDescendingError.begin();
         it != pixelIndicesDescendingError.end() && i < halfPixelCount;
         it++, i++)
    {
        for (int c = 0; c < 3; c++)
        {
            secondaryColor.rgb[c] += it->p->rgb[c] / divisor;
        }
    }
}

void Image::colorHistogram()
{
    map<int, int> colorPixels;
    for (int h = 0; h < height; h++)
    {
        for (int w = 0; w < width; w++)
        {
            Pixel* p = pixelAt(w, h);
            //printf("hh pixel %d %d %f %f %f\n", w, h, p->rgb[0], p->rgb[1], p->rgb[2]);
            /*
            sprintf(tmp, "%d_%d_%d__%d",
                    (int)(p->rgb[0]*255.0),
                    (int)(p->rgb[1]*255.0),
                    (int)(p->rgb[2]*255.0),
                    p->palette_index);
            
            string s(tmp);
            int n = colorPixels[s];
            n++;
            colorPixels[s] = n;
            */
            
            int n = colorPixels[p->palette_index];
            colorPixels[p->palette_index] = ++n;
        }
    }
    
    std::list<ColorRank> ranks;
    
    // sort
    while (colorPixels.size() > 0)
    {
        int maxVal = -1;
        int maxIndex = -1;
        for (std::map<int, int>::iterator it = colorPixels.begin(); it != colorPixels.end(); it++)
        {
            //printf("## %d: %d\n", it->first, it->second);
            if (it->second > maxVal)
            {
                maxVal = it->second;
                maxIndex = it->first;
            }
        }
        
        //printf("## %d: %d\n", maxIndex, maxVal);
        colorPixels.erase(maxIndex);
        
        ColorRank r;
        r.paletteIndex = maxIndex;
        r.numPixels = maxVal;
        
        ranks.push_back(r);
    }
    
    for (std::list<ColorRank>::iterator it = ranks.begin(); it != ranks.end(); it++)
    {
        printf("##@ %d: %d\n", it->paletteIndex, it->numPixels);
    }
    
    /*
    for (it = colorPixels.begin(); it != colorPixels.end(); it++)
    {
        printf("## %d: %d\n", it->first, it->second);
    }
    */
    
    
}

float Image::getErrorFromImage(const Image &im)
{
    float grayError = 0.0;
    float colorError = 0.0;
    
    float err = 0.0;
    Color pixColor[2];
    Color gray[2];
    Color col[2];
    for (int h = 0; h < height; h++)
    {
        for (int w = 0; w < width; w++)
        {
            //Pixel* p1 = pixelAt(w, h);
            //Pixel* p2 = im.pixelAt(w, h);
            
            /*
            for (int c = 0; c < 3; c++)
            {
                err += p1->rgb[c] * p2->rgb[c];
            }
            */
            
            Pixel* p[2];
            p[0] = pixelAt(w, h);
            p[1] = im.pixelAt(w, h);
            
            for (int i = 0; i < 2; i++)
            {
                pixColor[i].fromPixel(*p[i]);
                pixColor[i].getGrayAndColorComponents(gray[i], col[i]);
            }
            
            float cdiff;
            for (int c = 0; c < 3; c++)
            {
                cdiff = col[0].rgb[c] - col[1].rgb[c];
                colorError += cdiff*cdiff;
            }
            
            cdiff = gray[0].rgb[0] - gray[1].rgb[0];
            grayError += 3.0 * (cdiff*cdiff);
        }
    }
    
    err = (1.4 * colorError) + (0.6 * grayError);
    return err;
}

Pixel* Image::pixelAt(int w, int h) const
{
    if (w >= 0 && w < width &&
        h >= 0 && h < height)
        return &pixels[h*width + w];
    else
        return NULL;
}

void Image::copyFromImageAtPosition(const Image& im, int xOffset, int yOffset)
{
    for (int y = 0; y < im.getHeight(); y++)
    {
        for (int x = 0; x < im.getWidth(); x++)
        {
            Pixel *dstPix = pixelAt(xOffset + x, yOffset + y);
            Pixel *srcPix = im.pixelAt(x, y);
            
            dstPix->fromPixel(*srcPix);
        }
    }
}

void Image::writePPM(const char *fname)
{
    FILE* fp = fopen(fname, "wb");
    writePPM(fp);
}

void Image::writePPM(FILE *fp)
{
    // write ppm header
    fprintf(fp, "P6 %d %d 255\n", width, height);
    for (int hh = 0; hh < height; hh++)
    {
        for (int ww = 0; ww < width; ww++)
        {
            Pixel* p = pixelAt(ww, hh);
            for (int c = 0; c < 3; c++)
            {
                unsigned char clr = (unsigned char)(p->rgb[c] * 255.0);
                fwrite(&clr, 1, 1, fp);
            }
        }
    }
    
    if (fp != stdout)
    {
        fclose(fp);
    }
}

void Image::writeXBM(const char *fname)
{
    FILE* fp = fopen(fname, "wb");
    writeXBM(fp);
}

// Write out the image as an XBM file
void Image::writeXBM(FILE *out) {

    fprintf(out, "#define image_width %d\n", width);
    fprintf(out, "#define image_height %d\n", height);
    fprintf(out, "static unsigned char image_bits[] = {\n");

    int rowWidth = width / 8;
    if (width % 8 != 0) {
        //rowWidth++;
    }

    
    for (int y = 0; y < height; y++) {
        for (int x = 0; x < rowWidth; x++) {
            uint8_t byte = 0;
            for (int bit = 0; bit < 8; bit++) {
                Pixel* pixel = pixelAt(x * 8 + bit, y);
                if (pixel->rgb[0] < 0.5f || pixel->rgb[1] < 0.5f || pixel->rgb[2] < 0.5f) {
                    byte |= 1 << bit;
                }
            }


            fprintf(out, "0x%02x,", byte);
        }
        uint8_t leftoverByte = 0;
        for (int i = 0; i < width % 8; i++) {
            Pixel* pixel = pixelAt(width - 8 + i, y);
                if (pixel->rgb[0] < 0.5f || pixel->rgb[1] < 0.5f || pixel->rgb[2] < 0.5f) {
                    leftoverByte |= 1 << i;
                }
            
        }

        if (width % 8 != 0) {
            fprintf(out, "0x%02x,", leftoverByte);
        }
        fprintf(out, "\n");
    }

    fprintf(out, "};\n");
    if (out != stdout) {
      fclose(out);
    }
}

void writeBE(uint16_t val, FILE* fp) {
    uint8_t byte;
    byte = ((uint16_t)val & 0xFF00) >> 8;
    fwrite(&byte, 1, 1, fp);

    byte = (uint16_t)val & 0x00FF;
    fwrite(&byte, 1, 1, fp);
}

void Image::writeRGB3(const char *fname)
{
    FILE* fp = fopen(fname, "wb");
    writeRGB3(fp);
}

// Write out the image as an rgb3 file
void Image::writeRGB3(FILE *out) {
    // write out palette indices
    // first write 2-byte width, height (big endian)
    //writeBE((uint16_t)width, out);
    //writeBE((uint16_t)height, out);

    uint8_t byte = 0;
    int bitsInByte = 0;
    for (int hh = 0; hh < height; hh++)
    {
        for (int ww = 0; ww < width; ww++)
        {
            Pixel* p = pixelAt(ww, hh);
            uint8_t index = (uint8_t)p->palette_index;

            uint8_t bits = index & 0b00000111;
            if (bitsInByte == 1) {
                bits <<= 4;
            }

            byte |= bits;
            bitsInByte++;

            if (bitsInByte == 2) {
                fwrite(&byte, 1, 1, out);
                byte = 0;
                bitsInByte = 0;
            }
        }
    }
}

void Image::writeRGB8(const char *fname)
{
    FILE* fp = fopen(fname, "wb");
    writeRGB8(fp);
}

// Write out the image as an rgb3 file
void Image::writeRGB8(FILE *out) {
    // write out palette indices
    // first write 2-byte width, height (big endian)
    //writeBE((uint16_t)width, out);
    //writeBE((uint16_t)height, out);

    for (int hh = 0; hh < height; hh++)
    {
        for (int ww = 0; ww < width; ww++)
        {
            Pixel* p = pixelAt(ww, hh);
            uint8_t index = (uint8_t)p->palette_index;
            fwrite(&index, 1, 1, out);
        }
    }
}


void Image::writePBM(const char *fname)
{
    FILE* fp = fopen(fname, "wb");
    writePBM(fp);
}

// Write out the image as an rgb3 file
void Image::writePBM(FILE *out) {
    // write ppm header
    fprintf(out, "P4\n%d %d\n", width, height);

    int byte = 0;
    int bitsInByte = 0;
    for (int hh = 0; hh < height; hh++)
    {
        for (int ww = 0; ww < width; ww++)
        {
            Pixel* p = pixelAt(ww, hh);
            //uint8_t index = (uint8_t)p->palette_index;
            byte <<= 1;
            if (p->rgb[0] <= 0.5 && p->rgb[1] <= 0.5 && p->rgb[2] <= 0.5) {
                byte |= 0b1;
            }
            bitsInByte++;

            if (bitsInByte == 8) {
                fwrite(&byte, 1, 1, out);
                bitsInByte = 0;
                byte = 0;
            }
        }
    }
}

void Image::writeBMP(const char *fname)
{
    /*
    // write ppm header
    fprintf(out, "P4\n%d %d\n", width, height);

    int byte = 0;
    int bitsInByte = 0;
    for (int hh = 0; hh < height; hh++)
    {
        for (int ww = 0; ww < width; ww++)
        {
            Pixel* p = pixelAt(ww, hh);
            //uint8_t index = (uint8_t)p->palette_index;
            byte <<= 1;
            if (p->rgb[0] <= 0.5 && p->rgb[1] <= 0.5 && p->rgb[2] <= 0.5) {
                byte |= 0b1;
            }
            bitsInByte++;

            if (bitsInByte == 8) {
                fwrite(&byte, 1, 1, out);
                bitsInByte = 0;
                byte = 0;
            }
        }
    }*/

    bmpfile_t* out;
    out = bmp_create(width, height, 1);

    rgb_pixel_t black, white;
    black.blue = 0;
    black.green = 0;
    black.red = 0;
    black.alpha = 255;

    white.blue = 255;
    white.green = 255;
    white.red = 255;
    white.alpha = 255;

    for (int hh = 0; hh < height; hh++)
    {
        for (int ww = 0; ww < width; ww++)
        {
            Pixel* p = pixelAt(ww, hh);
            if (p->rgb[0] <= 0.5 && p->rgb[1] <= 0.5 && p->rgb[2] <= 0.5) {
                bmp_set_pixel(out, ww, hh, black);
            } else {
                bmp_set_pixel(out, ww, hh, white);
            }
        }
    }

    bmp_save(out, fname);
    bmp_destroy(&out);
}

void Image::writeBW(const char *fname)
{
    FILE* fp = fopen(fname, "wb");
    writeBW(fp);
}

// 1 bit mono
void Image::writeBW(FILE *out) {
    int byte = 0;
    int bitsInByte = 0;
    for (int hh = 0; hh < height; hh++)
    {
        for (int ww = 0; ww < width; ww++)
        {
            Pixel* p = pixelAt(ww, hh);
            byte <<= 1;
            if (p->rgb[0] > 0.5 && p->rgb[1] > 0.5 && p->rgb[2] > 0.5) {
                byte |= 0b1;
            }
            bitsInByte++;

            if (bitsInByte == 8) {
                fwrite(&byte, 1, 1, out);
                bitsInByte = 0;
                byte = 0;
            }
        }
    }
}

void Image::writeRawMono(const char* fname) {
    FILE* fp = fopen(fname, "wb");
    writeRawMono(fp);
}

// write out image 
void Image::writeRawMono(FILE* fp)
{
    for (int hh = 0; hh < height; hh++)
    {
        for (int ww = 0; ww < width; ww++)
        {
            Pixel* p = pixelAt(ww, hh);
            unsigned char clr = (unsigned char)(p->rgb[0] * 255.0);
            fwrite(&clr, 1, 1, fp);
        }
    }
}
