#ifndef MAIN_H_INCLUDED
#define MAIN_H_INCLUDED


#include <stdint.h>
// MyCIEXYZTRIPLE stuff
typedef uint32_t MyFXPT2DOT30;
#pragma pack(push, 1)
typedef struct {
    MyFXPT2DOT30 ciexyzX;
    MyFXPT2DOT30 ciexyzY;
    MyFXPT2DOT30 ciexyzZ;
} MyCIEXYZ;

typedef struct {
    MyCIEXYZ  ciexyzRed; 
    MyCIEXYZ  ciexyzGreen; 
    MyCIEXYZ  ciexyzBlue; 
} MyCIEXYZTRIPLE;
 
// bitmap file header

typedef struct {
    uint16_t bfType;    ////  11  22 00 00 
    uint32_t  bfSize;  
    uint16_t bfReserved1;
    uint16_t bfReserved2;
    uint32_t   bfOffBits;
} MyBITMAPFILEHEADER;
 
// bitmap info header

typedef struct {
    uint32_t   biSize;
    uint32_t   biWidth;
    uint32_t   biHeight;
    uint16_t biPlanes;
    uint16_t biBitCount;
    uint32_t   biCompression;
    uint32_t   biSizeImage;
    uint32_t   biXPelsPerMeter;
    uint32_t   biYPelsPerMeter;
    uint32_t   biClrUsed;
    uint32_t   biClrImportant;
    uint32_t   biRedMask;
    uint32_t   biGreenMask;
    uint32_t   biBlueMask;
    uint32_t   biAlphaMask;
    uint32_t   biCSType;
    MyCIEXYZTRIPLE   biEndpoints;
    uint32_t   biGammaRed;
    uint32_t   biGammaGreen;
    uint32_t   biGammaBlue;
    uint32_t   biIntent;
    uint32_t   biProfileData;
    uint32_t   biProfileSize;
    uint32_t   biReserved;
} MyBITMAPINFOHEADER;
 
// rgb quad

typedef struct {
    unsigned char  rgbBlue;
    unsigned char  rgbGreen;
    unsigned char  rgbRed;
    unsigned char  rgbReserved;
} MyRGBQUAD;
#pragma pack(pop)
// read bytes
template <typename Type>
void read(std::ifstream &fp, Type &result, std::size_t size) {
    fp.read(reinterpret_cast<char*>(&result), size);
}
 
// bit extract
unsigned char bitextract(const uint32_t byte, const uint32_t mask);
 
#endif // MAIN_H_INCLUDEDs