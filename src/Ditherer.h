//
//  Ditherer.hpp
//  imconv
//
//  Created by Michael Hill on 2/20/17.
//  Copyright © 2017 Michael Hill. All rights reserved.
//

#ifndef Ditherer_hpp
#define Ditherer_hpp

#include <stdio.h>
#include "Image.h"

class Ditherer
{
public:
    virtual ~Ditherer() {};
    virtual Image* createDitheredImageFromImageWithPalette(const Image& image, const Palette& palette) = 0;
    virtual void ditherImageInPlaceWithPalette(const Image& image, const Palette& palette) {};
    
    static Ditherer* createFloydSteinbergDitherer();
    static Ditherer* createAtkinsonDitherer();
    static Ditherer* createC64Ditherer();
    static Ditherer* createNearestNeighborDitherer();
};

#endif /* Ditherer_hpp */
