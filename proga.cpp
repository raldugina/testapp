#include "stdafx.h"
#include <iostream.h>
#include <windows.h>

unsigned int* fun(char* fin)
{
BITMAPFILEHEADER bfh; // в hfh хранятся данные из блока HEADER
BITMAPINFOHEADER bih; // в bih хранятся данные из блока INFO
HANDLE hIn; // hIn - поток, в котором читается переданный файл BMP
unsigned int width, // переменная для хранения ширины растра
height, // переменная для хранения высоты растра
position=2, // переменная, указывающая на свободное место в массиве
i, j, // переменные-счётчики для циклов for
*massiv; // массив, в котором будут храниться цвета пикселей
RGBQUAD palette[256]; // массив для хранения палитры (ТОЛЬКО ДЛЯ 256-ЦВЕТНЫХ КАРТИНОК)
BYTE *inBuf; // массив, в который будет считываться строка пикселей
DWORD RW; //

hIn=CreateFile(fin, GENERIC_READ, FILE_SHARE_READ, NULL, OPEN_EXISTING, 0, NULL); // в hIn открывается переданный файл

if(hIn==INVALID_HANDLE_VALUE) // если возникает ошибка, то
{
CloseHandle(hIn); // закрываем поток hIn
return NULL; // и функция fun возвращает указатель равный NULL
}
else // если ошибки чтения файла не возникает
{
ReadFile(hIn, &bfh, sizeof(bfh), &RW, NULL); // прочитаем из файла блок HEADER в bfh
ReadFile(hIn, &bih, sizeof(bih), &RW, NULL); // прочитаем из файла блок INFO в bih
ReadFile(hIn, palette, 256*sizeof(RGBQUAD), &RW, NULL); // прочитаем палитру в массив palette

width=bih.biWidth; // ширина растра заносится в width
height=bih.biHeight; // высота растра заносится в height

SetFilePointer(hIn, bfh.bfOffBits, NULL, FILE_BEGIN); // читаем файл через bfh.bfOffBits от начала файла

inBuf=new BYTE[width]; // создаём динамический массив для хранения строки растра
massiv = new unsigned int [3*height*width+2]; // создаём динамический массив для хранения значений цветовых
//компонент пикселей
massiv[0]=width; // нулевой элемент массива хранит количество пикселей в строке
massiv[1]=height; // первый элемент массива хранит количество строк
// отсюда можно рассчитать длину массива как 3*height*width+2
for(i=0; i<height; i++) // перебираем все строки растра
{
ReadFile(hIn, inBuf, width, &RW, NULL); // читаем значения пикселей строки i в массив inBuf
for(j=0; j<width; j++)
{
massiv[position]=palette[inBuf[j]].rgbRed; // сохраняем значение красной компоненты
massiv[position+1]=palette[inBuf[j]].rgbGreen; // сохраняем значение зелёной компоненты
massiv[position+2]=palette[inBuf[j]].rgbBlue; // сохраняем значение синей компоненты
position+=3; // смещаем номер свободного элемента в массиве на 3 позиции
}
SetFilePointer(hIn, (3*width)%4, NULL, FILE_CURRENT);// начинаем читать из файла со следующей строки
}
delete inBuf; // удаляем динамический массив inBuf
CloseHandle(hIn); // закрываем поток hIn

return massiv; // функция fun возвращает указатель на нулевой элемент массива
}
}