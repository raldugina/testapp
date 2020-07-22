#include <iostream>
#include <fstream>
#include <tchar.h>
#include "main.h"
#include  <windows.h> 
// 24/32 бит
BOOL  SaveArrFile(const TCHAR* filename, const __int32* arr,   MyBITMAPFILEHEADER & fileheader, MyBITMAPINFOHEADER & fileinfoheader ) {
   
   

   // DWORD p_row = (DWORD)((width * bpp + 31) & ~31) / 8uL;
  //  DWORD size = (DWORD)(height * p_row);

    // формируем файловый заголовок
    BITMAPFILEHEADER  hdr;
    ZeroMemory(&hdr, sizeof(BITMAPFILEHEADER));
    hdr.bfType = 0x4D42;
    hdr.bfOffBits = sizeof(BITMAPFILEHEADER) + sizeof(BITMAPINFOHEADER);
    hdr.bfSize = hdr.bfOffBits + fileinfoheader.biSize;

    // заголовок описателя растра
    BITMAPINFO dib;
    ZeroMemory(&dib, sizeof(BITMAPINFO));
    dib.bmiHeader.biSize = sizeof(BITMAPINFOHEADER);
   // dib.bmiHeader.biBitCount = bpp;
    dib.bmiHeader.biCompression = BI_RGB;
    dib.bmiHeader.biPlanes = 1u;
   // dib.bmiHeader.biWidth = (long)width;
   // dib.bmiHeader.biHeight = (long)-height;
    //dib.bmiHeader.biSizeImage = size;
    dib.bmiHeader.biXPelsPerMeter = 11811L;
    dib.bmiHeader.biYPelsPerMeter = 11811L;
    dib.bmiHeader.biClrImportant = 0uL;
    dib.bmiHeader.biClrUsed = 0uL;
    
    std::ofstream outfile((const char*)"teststream.bmp", std::ifstream::binary);

   

    
    // далее запись в файл
    HANDLE fp = CreateFile(filename, GENERIC_WRITE, FILE_SHARE_WRITE, NULL,
        CREATE_ALWAYS, FILE_ATTRIBUTE_NORMAL, NULL);
    if (fp == INVALID_HANDLE_VALUE)
        return FALSE;

    // записываем заголовки...
    DWORD  dwr = 0uL;
  WriteFile(fp, (LPCVOID)&hdr, sizeof(BITMAPFILEHEADER), &dwr, NULL);
  WriteFile(fp, (LPCVOID)&dib.bmiHeader, sizeof(BITMAPINFOHEADER), &dwr, NULL);

    outfile.write((const char*)&fileheader, sizeof(MyBITMAPFILEHEADER));
    outfile.write((const char*)&fileinfoheader, sizeof(MyBITMAPINFOHEADER)-sizeof(MyCIEXYZTRIPLE)-48);
    outfile.write((const char*)arr, 735 * 718);


    // запись массива пикселей
    if (fileinfoheader.biBitCount == 32) // 32-бит
        WriteFile(fp, (LPCVOID)arr, fileinfoheader.biSize, &dwr, NULL);
    else if (fileinfoheader.biBitCount == 24) { // 24-бит с дополнением до 32-разрядной границы

        BYTE   nil = 0u;
        int   cb = sizeof(RGBQUAD);
        int  align = ((cb - ((fileinfoheader.biWidth * fileinfoheader.biBitCount + 7) / 8) % cb) % cb);

        for (size_t y = 0; y < fileinfoheader.biHeight; y++) {
            for (size_t x = 0; x < fileinfoheader.biWidth; x++)
                WriteFile(fp, (LPCVOID)&arr[y * fileinfoheader.biWidth + x], sizeof(RGBTRIPLE), &dwr, NULL);

            for (int i = 0; i < align; i++) // до границы DWORD
                WriteFile(fp, (LPCVOID)&nil, sizeof(BYTE), &dwr, NULL);
        }
    }


    FlushFileBuffers(fp);
    CloseHandle(fp);
    outfile.close();
    return TRUE;
}


char adjustBrightness(char pixel, int32_t brightness) {
    uint32_t res = (uint8_t) pixel + brightness; 
    if (res > 255) res = 255;
    if (res < 0) res = 0; 
    return  char(res);
}


int main(int argc, char *argv[])
{
 /*   if (argc < 2) {
        std::cout << "Usage: " << argv[0] << " file_name" << std::endl;
        return 0;
    }
 */
    const char * fileName = "city.bmp";
 
    // открываем файл
    std::ifstream fileStream(fileName, std::ifstream::binary);
    if (!fileStream) {
        std::cout << "Error opening file '" << fileName << "'." << std::endl;
        return 0;
    }
 
    // заголовк изображения
    MyBITMAPFILEHEADER fileHeader;
    read(fileStream, fileHeader.bfType, sizeof(fileHeader.bfType));
    read(fileStream, fileHeader.bfSize, sizeof(fileHeader.bfSize));
    read(fileStream, fileHeader.bfReserved1, sizeof(fileHeader.bfReserved1));
    read(fileStream, fileHeader.bfReserved2, sizeof(fileHeader.bfReserved2));
    read(fileStream, fileHeader.bfOffBits, sizeof(fileHeader.bfOffBits));
 
    if (fileHeader.bfType != 0x4D42) {
        std::cout << "Error: '" << fileName << "' is not BMP file." << std::endl;
        return 0;
    }
 
    // информация изображения
    MyBITMAPINFOHEADER fileInfoHeader;
    read(fileStream, fileInfoHeader.biSize, sizeof(fileInfoHeader.biSize));
 
    // bmp core
    if (fileInfoHeader.biSize >= 12) {
        read(fileStream, fileInfoHeader.biWidth, sizeof(fileInfoHeader.biWidth));
        read(fileStream, fileInfoHeader.biHeight, sizeof(fileInfoHeader.biHeight));
        read(fileStream, fileInfoHeader.biPlanes, sizeof(fileInfoHeader.biPlanes));
        read(fileStream, fileInfoHeader.biBitCount, sizeof(fileInfoHeader.biBitCount));
    }
 
    // получаем информацию о битности
    int colorsCount = fileInfoHeader.biBitCount >> 3;
    if (colorsCount < 3) {
        colorsCount = 3;
    }
 
    int bitsOnColor = fileInfoHeader.biBitCount / colorsCount;
    int maskValue = (1 << bitsOnColor) - 1;
 
    // bmp v1
    if (fileInfoHeader.biSize >= 40) {
        read(fileStream, fileInfoHeader.biCompression, sizeof(fileInfoHeader.biCompression));
        read(fileStream, fileInfoHeader.biSizeImage, sizeof(fileInfoHeader.biSizeImage));
        read(fileStream, fileInfoHeader.biXPelsPerMeter, sizeof(fileInfoHeader.biXPelsPerMeter));
        read(fileStream, fileInfoHeader.biYPelsPerMeter, sizeof(fileInfoHeader.biYPelsPerMeter));
        read(fileStream, fileInfoHeader.biClrUsed, sizeof(fileInfoHeader.biClrUsed));
        read(fileStream, fileInfoHeader.biClrImportant, sizeof(fileInfoHeader.biClrImportant));
    }
 
    // bmp v2
    fileInfoHeader.biRedMask = 0;
    fileInfoHeader.biGreenMask = 0;
    fileInfoHeader.biBlueMask = 0;
 
    if (fileInfoHeader.biSize >= 52) {
        read(fileStream, fileInfoHeader.biRedMask, sizeof(fileInfoHeader.biRedMask));
        read(fileStream, fileInfoHeader.biGreenMask, sizeof(fileInfoHeader.biGreenMask));
        read(fileStream, fileInfoHeader.biBlueMask, sizeof(fileInfoHeader.biBlueMask));
    }
 
    // если маска не задана, то ставим маску по умолчанию
    if (fileInfoHeader.biRedMask == 0 || fileInfoHeader.biGreenMask == 0 || fileInfoHeader.biBlueMask == 0) {
        fileInfoHeader.biRedMask = maskValue << (bitsOnColor * 2);
        fileInfoHeader.biGreenMask = maskValue << bitsOnColor;
        fileInfoHeader.biBlueMask = maskValue;
    }
 
    // bmp v3
    if (fileInfoHeader.biSize >= 56) {
        read(fileStream, fileInfoHeader.biAlphaMask, sizeof(fileInfoHeader.biAlphaMask));
    } else {
        fileInfoHeader.biAlphaMask = maskValue << (bitsOnColor * 3);
    }
 
    // bmp v4
    if (fileInfoHeader.biSize >= 108) {
        read(fileStream, fileInfoHeader.biCSType, sizeof(fileInfoHeader.biCSType));
        read(fileStream, fileInfoHeader.biEndpoints, sizeof(fileInfoHeader.biEndpoints));
        read(fileStream, fileInfoHeader.biGammaRed, sizeof(fileInfoHeader.biGammaRed));
        read(fileStream, fileInfoHeader.biGammaGreen, sizeof(fileInfoHeader.biGammaGreen));
        read(fileStream, fileInfoHeader.biGammaBlue, sizeof(fileInfoHeader.biGammaBlue));
    }
 
    // bmp v5
    if (fileInfoHeader.biSize >= 124) {
        read(fileStream, fileInfoHeader.biIntent, sizeof(fileInfoHeader.biIntent));
        read(fileStream, fileInfoHeader.biProfileData, sizeof(fileInfoHeader.biProfileData));
        read(fileStream, fileInfoHeader.biProfileSize, sizeof(fileInfoHeader.biProfileSize));
        read(fileStream, fileInfoHeader.biReserved, sizeof(fileInfoHeader.biReserved));
    }
 
    // проверка на поддерку этой версии формата
    if (fileInfoHeader.biSize != 12 && fileInfoHeader.biSize != 40 && fileInfoHeader.biSize != 52 &&
        fileInfoHeader.biSize != 56 && fileInfoHeader.biSize != 108 && fileInfoHeader.biSize != 124) {
        std::cout << "Error: Unsupported BMP format." << std::endl;
        return 0;
    }
 
    if (fileInfoHeader.biBitCount != 8 && fileInfoHeader.biBitCount != 16 && fileInfoHeader.biBitCount != 24 && fileInfoHeader.biBitCount != 32) {
        std::cout << "Error: Unsupported BMP bit count." << std::endl;
        return 0;
    }
 
    if (fileInfoHeader.biCompression != 0 && fileInfoHeader.biCompression != 3) {
        std::cout << "Error: Unsupported BMP compression." << std::endl;
        return 0;
    }
 /*
    // rgb info
    MyRGBQUAD **rgbInfo = new MyRGBQUAD*[fileInfoHeader.biHeight];
 
    for (unsigned int i = 0; i < fileInfoHeader.biHeight; i++) {
        rgbInfo[i] = new MyRGBQUAD[fileInfoHeader.biWidth];
    }
  */
    // определение размера отступа в конце каждой строки
    int linePadding = ((fileInfoHeader.biWidth * (fileInfoHeader.biBitCount / 8)) % 4) & 3;

    // чтение
   
    std::cout << fileHeader.bfOffBits;
    char* buffer = new char[735 * 718];

    fileStream.seekg(56);
    fileStream.read(buffer, 735*718);
    /// <summary>
    ///  Здесь 
    /// </summary>
    /// <param name="argc"></param>
    /// <param name="argv"></param>
    /// <returns></returns>
    for (size_t i = 0; i < fileInfoHeader.biSizeImage; i++) {
        buffer[i] =  adjustBrightness(buffer[i],-30);
    }

    const TCHAR* fileOutName =( const TCHAR *) "out.bmp";

    std::cout << " Saving file" << fileOutName << std::endl;

    SaveArrFile(_T("grid.bmp"), (const __int32*)buffer, fileHeader,fileInfoHeader);


 /*
    for (unsigned int i = 0; i < fileInfoHeader.biHeight; i++) {
        for (unsigned int j = 0; j < fileInfoHeader.biWidth; j++) {
            read(fileStream, bufer, fileInfoHeader.biBitCount / 8);
 
            rgbInfo[i][j].rgbRed = bitextract(bufer, fileInfoHeader.biRedMask);
            rgbInfo[i][j].rgbGreen = bitextract(bufer, fileInfoHeader.biGreenMask);
            rgbInfo[i][j].rgbBlue = bitextract(bufer, fileInfoHeader.biBlueMask);
            rgbInfo[i][j].rgbReserved = bitextract(bufer, fileInfoHeader.biAlphaMask);
        }
        fileStream.seekg(linePadding, std::ios_base::cur);
    }
 */
    // вывод
    //for (unsigned int i = 0; i < fileInfoHeader.biHeight; i++) {
    //    for (unsigned int j = 0; j < fileInfoHeader.biWidth; j++) {
    //        std::cout << std::hex
    //                  << +rgbInfo[i][j].rgbRed << " "
    //                  << +rgbInfo[i][j].rgbGreen << " "
    //                  << +rgbInfo[i][j].rgbBlue << " "
    //                  << +rgbInfo[i][j].rgbReserved
    //                  << std::endl;
    //    }
    //    std::cout << std::endl;
    //}
 
    return 0;
}
 
unsigned char bitextract(const uint32_t byte, const uint32_t mask) {
    if (mask == 0) {
        return 0;
    }
 
    // определение количества нулевых бит справа от маски
    int
        maskBufer = mask,
        maskPadding = 0;
 
    while (!(maskBufer & 1)) {
        maskBufer >>= 1;
        maskPadding++;
    }
 
    // применение маски и смещение
    return (byte & mask) >> maskPadding;
}