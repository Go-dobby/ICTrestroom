#ifndef _PAINT_H
#define _PAINT_H
#include "Font.h"

// Color Codes
#define WHITE         0xFFFF
#define BLACK         0x0000
#define BLUE          0x001F
#define BRED          0XF81F
#define GRED          0XFFE0
#define GBLUE         0X07FF
#define RED           0xF800
#define MAGENTA       0xF81F
#define GREEN         0x07E0
#define CYAN          0x7FFF
#define YELLOW        0xFFE0
#define BROWN         0XBC40
#define BRRED         0XFC07
#define GRAY          0X8430
#define DARKBLUE      0X01CF
#define LIGHTBLUE     0X7D7C
#define GRAYBLUE      0X5458
#define LIGHTGREEN    0X841F
#define LGRAY         0XC618
#define LGRAYBLUE     0XA651
#define LBBLUE        0X2B12

// raw   : CustomLCD 클래스
// x     : 좌표x
// y     : 센터y
// Color : 색상
// size  : 크기
void DrawPoint(CustomLCD raw, uint16_t x, uint16_t y, uint16_t Color, uint16_t size = 1)
{
  if(size == 1) raw.SetPointColor(x, y, Color);
  else
  {
    int p = size/2;
    raw.ClearWindow(x-p, y-p, x+p, y+p, Color);
  }
}

// raw   : CustomLCD 클래스
// x     : 좌표x
// y     : 센터y
// to_x  : 가로
// to_y  : 목표y
// Color : 색상
// thick : 굵기
void DrawLine(CustomLCD raw, uint16_t x, uint16_t y, uint16_t to_x, uint16_t to_y, uint16_t Color, uint16_t thick = 1)
{
  int dx = abs((int)to_x - (int)x);
  int dy = -abs((int)to_y - (int)y);
  int add_x = x < to_x ? 1:-1;
  int add_y = y < to_y ? 1:-1;
  int esp = dx + dy;
  int len = 0;
  for(;;)
  {
    len++;
    DrawPoint(raw, x, y, Color, thick);
    if(esp * 2 >= dy)
    {
      if(x == to_x) break;
      esp += dy;
      x += add_x;
    }
    if(esp * 2 <= dx)
    {
      if(y == to_y) break;
      esp += dx;
      y += add_y;
    }
  }
}

// raw   : CustomLCD 클래스
// x     : 좌표x
// y     : 센터y
// width : 가로
// height: 세로
// Color : 색상
// centor: 중앙정렬 (true, false)
void DrawRect(CustomLCD raw, uint16_t x, uint16_t y, uint16_t width, uint16_t height, uint16_t Color, bool center = 0)
{
  if(center) raw.ClearWindow(x, y, x+width, y+height, Color);
  else
  {
    int w = width/2;
    int h = height/2;
    raw.ClearWindow(x-w, y-h, x+w, y+h, Color);
  }
}

// raw   : CustomLCD 클래스
// cx    : 센터x
// cy    : 센터y
// rad   : 반지름
// Color : 색상
// fill  : 채우기
// 선굵기 : 테두리 굵기 (채우기X 일때)
void DrawCircle(CustomLCD raw, uint16_t cx, uint16_t cy, uint16_t rad, uint16_t Color, bool fill = 1, uint16_t thick = 1)
{
  int x = 0, y = rad, esp = 3 - (rad << 1), s;
  if(fill)
  {
    while(x <= y)
    {
      for(s = x; s <= y; s++)
      {
        DrawPoint(raw, cx + x, cy + s, Color); // 1
        DrawPoint(raw, cx - x, cy + s, Color); // 2
        DrawPoint(raw, cx - s, cy + x, Color); // 3
        DrawPoint(raw, cx - s, cy - x, Color); // 4
        DrawPoint(raw, cx - x, cy - s, Color); // 5
        DrawPoint(raw, cx + x, cy - s, Color); // 6
        DrawPoint(raw, cx + s, cy - x, Color); // 7
        DrawPoint(raw, cx + s, cy + x, Color); // 0
      }
      if(esp < 0) esp += 4 * x + 6;
      else
      {
        esp += 10 + 4 * (x - y);
        y--;
      }
      x++;
    }
  }
  else
  {
    while(x <= y)
    {
      DrawPoint(raw, cx + x, cy + y, Color, thick); // 1
      DrawPoint(raw, cx - x, cy + y, Color, thick); // 2
      DrawPoint(raw, cx - y, cy + x, Color, thick); // 3
      DrawPoint(raw, cx - y, cy - x, Color, thick); // 4
      DrawPoint(raw, cx - x, cy - y, Color, thick); // 5
      DrawPoint(raw, cx + x, cy - y, Color, thick); // 6
      DrawPoint(raw, cx + y, cy - x, Color, thick); // 7
      DrawPoint(raw, cx + y, cy + x, Color, thick); // 0
      if(esp < 0) esp += 4 * x + 6;
      else
      {
        esp += 10 + 4 * (x - y);
        y--;
      }
      x++;
    }
  }
}

// raw   : CustomLCD 클래스
// x     : 좌표x
// y     : 센터y
// text  : 문자
// Color : 색상
// size  : 글자크기
void DrawChar(CustomLCD raw, uint16_t x, uint16_t y, char text, uint16_t Color, uint16_t size = 1)
{
  int con_size = size * 8;
  char convert[con_size][con_size] = {0,};
  // Convert
  for(int i = 0; i < 8; i++)
    for(int j = 0; j < 8; j++)
      for(int cx = 0; cx < size; cx++)
        for(int cy = 0; cy < size; cy++)
          convert[i*size + cx][j*size + cy] = (FontTable[text][i] >> j) & 0x1;
  // Print
  for(int i = 0; i < con_size; i++)
    for(int j = 0; j < con_size; j++)
      if(convert[i][j] != 0)
        raw.SetPointColor(x+j, y+i, Color);
}

// raw   : CustomLCD 클래스
// x     : 좌표x
// y     : 센터y
// text  : 문자열
// Color : 색상
// centor: 중앙정렬 (true, false)
// size  : 글자크기
void DrawString(CustomLCD raw, uint16_t x, uint16_t y, String text, uint16_t Color, bool center = 0, uint16_t size = 1)
{
  int cx = x - (text.length() * size * 4);
  for(int len = 0; len < text.length(); len++)
  {
    if(center) DrawChar(raw, len*size*8 + cx, y, text[len], Color, size);
    else DrawChar(raw, len*size*8 + x, y, text[len], Color, size);
  }
}

#endif
