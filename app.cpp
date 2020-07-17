#include <windows.h>
#include <tchar.h>
#include <cstdio>
#define  DIB_RGB(r, g, b) \
((DWORD)((r & 0xFF) << 16) | ((g & 0xFF) << 8) | (b & 0xFF)) 

void fill_rect(__int32*, int, int, int, int, int, DWORD);





// 24/32 бит
BOOL  SaveArrFile(const TCHAR* filename, const __int32* arr, 
                         int width, int height, int bpp = 24){

    if((bpp < 24) || (bpp > 32)) // только 24/32 бит
         return FALSE;

    DWORD p_row = (DWORD)((width * bpp + 31) & ~31) / 8uL;
    DWORD size  = (DWORD)(height * p_row);

    // формируем файловый заголовок
    BITMAPFILEHEADER  hdr;
    ZeroMemory(&hdr, sizeof(BITMAPFILEHEADER));
    hdr.bfType    = 0x4D42;
    hdr.bfOffBits = sizeof(BITMAPFILEHEADER) + sizeof(BITMAPINFOHEADER);
    hdr.bfSize    = hdr.bfOffBits + size;

    // заголовок описателя растра
    BITMAPINFO dib;
    ZeroMemory(&dib, sizeof(BITMAPINFO));
    dib.bmiHeader.biSize        = sizeof(BITMAPINFOHEADER);
    dib.bmiHeader.biBitCount  = bpp;
    dib.bmiHeader.biCompression  = BI_RGB;
    dib.bmiHeader.biPlanes  = 1u;
    dib.bmiHeader.biWidth   = (long)width;
    dib.bmiHeader.biHeight  = (long)-height;
    dib.bmiHeader.biSizeImage   = size;
    dib.bmiHeader.biXPelsPerMeter = 11811L;
    dib.bmiHeader.biYPelsPerMeter = 11811L;
    dib.bmiHeader.biClrImportant  = 0uL;
    dib.bmiHeader.biClrUsed  = 0uL;

    // далее запись в файл
    HANDLE fp = CreateFile(filename, GENERIC_WRITE, FILE_SHARE_WRITE, NULL, 
                                   CREATE_ALWAYS, FILE_ATTRIBUTE_NORMAL, NULL);
    if(fp == INVALID_HANDLE_VALUE)
           return FALSE;

    // записываем заголовки...
    DWORD  dwr = 0uL;
    WriteFile(fp, (LPCVOID)&hdr, sizeof(BITMAPFILEHEADER), &dwr, NULL);
    WriteFile(fp, (LPCVOID)&dib.bmiHeader, sizeof(BITMAPINFOHEADER), &dwr, NULL);

    // запись массива пикселей
    if(bpp == 32) // 32-бит
           WriteFile(fp, (LPCVOID)arr, size, &dwr, NULL);
    else if(bpp == 24) { // 24-бит с дополнением до 32-разрядной границы

          BYTE   nil = 0u;
          int   cb  = sizeof(RGBQUAD);
          int  align = ((cb - ((width*bpp + 7) / 8) % cb) % cb);
		
          for(int y = 0; y < height; y++) {
               for(int x = 0; x < width; x++) 
                    WriteFile(fp, (LPCVOID)&arr[y*width+x], sizeof(RGBTRIPLE), &dwr, NULL);
			
                    for(int i = 0; i < align; i++) // до границы DWORD
                          WriteFile(fp, (LPCVOID)&nil, sizeof(BYTE), &dwr, NULL);
               }
   }

   FlushFileBuffers(fp);
   CloseHandle(fp);
   return TRUE;
}





int  main(void) {
    //массив пикселей
    __int32 arr[111*222] = {0};
    int cw = 222; 
    int ch = 111;

    // нарисуем что-нибудь
    DWORD rgb;
    int   sx  = ch / 5;
    int   sy  = cw / 10;
    for(int y = 0; y < 5; y++) {
         for(int x = 0; x < 10; x++) {
              rgb = DIB_RGB(rand()%2*0xFF, rand()%2*0xFF, rand()%2*0xFF);
              fill_rect(arr, cw, x*sx, y*sy, sx, sy, rgb);
        }
    }


    // сохраняем в файл
    if(SaveArrFile(_T("grid.bmp"), arr, cw, ch, 24))
           _putts(_T("Good save file."));
    else
           _putts(_T("Error save file !"));

    _gettchar();
    return 0;
}



// вывод прямоугольника
void fill_rect(__int32* arr, int width, int x, int y, int cx, int cy, DWORD color){
     for(int r = y; r <= (y + cy); r++) {
          for(int c = x; c <= (x + cx); c++)
               arr[r*width + c] = color;
     }
}