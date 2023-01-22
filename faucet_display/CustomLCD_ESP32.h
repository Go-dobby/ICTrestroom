#ifndef _CUSTOM_LCD_ESP32_H
#define _CUSTOM_LCD_ESP32_H
#include <SPI.h>

class CustomLCD
{
  private:
  SPIClass *spi = new SPIClass(VSPI);
  // Port
  // CLK = 18, DIN = 23, SS = CS = 5, BL = N.C (MAX Light)
  int CS = 5, DC = 17, RST = 16;
  // WriteData
  void WriteDataByte(uint8_t data)
  {
    digitalWrite(CS, 0);
    digitalWrite(DC, 1);
    spi->beginTransaction(SPISettings(8000000, MSBFIRST, SPI_MODE3));
    spi->transfer(data);
    spi->endTransaction();
    digitalWrite(CS, 1);
  }

  void WriteDataWord(uint16_t data)
  {
    digitalWrite(CS, 0);
    digitalWrite(DC, 1);
    spi->beginTransaction(SPISettings(8000000, MSBFIRST, SPI_MODE3));
    spi->transfer(data >> 8);
    spi->transfer(data);
    spi->endTransaction();
    digitalWrite(CS, 1);
  }

  void WriteDataReg(uint8_t data)
  {
    digitalWrite(CS, 0);
    digitalWrite(DC, 0);
    spi->beginTransaction(SPISettings(8000000, MSBFIRST, SPI_MODE3));
    spi->transfer(data);
    spi->endTransaction();
    // digitalWrite(CS, 1);
  }
  
  public:
  int Width = 240, Height = 240;

  void begin()
  {
    // Pin Mode
    pinMode(CS, OUTPUT);
    pinMode(DC, OUTPUT);
    pinMode(RST, OUTPUT);
    // Set Mode
    spi->begin();
    // Init Command
    Reset();
    Init();
  }
  
  void Reset()
  {
    digitalWrite(CS, 0);
    delay(100);
    digitalWrite(RST, 0);
    delay(100);
    digitalWrite(RST, 1);
    delay(100);
  }

  void Init()
  {
    WriteDataReg(0xEF);
    WriteDataReg(0xEB);
    WriteDataByte(0x14); 
    
    WriteDataReg(0xFE);      
    WriteDataReg(0xEF); 
  
    WriteDataReg(0xEB); 
    WriteDataByte(0x14); 
  
    WriteDataReg(0x84);     
    WriteDataByte(0x40); 
  
    WriteDataReg(0x85);     
    WriteDataByte(0xFF); 
  
    WriteDataReg(0x86);     
    WriteDataByte(0xFF); 
  
    WriteDataReg(0x87);     
    WriteDataByte(0xFF);
  
    WriteDataReg(0x88);     
    WriteDataByte(0x0A);
  
    WriteDataReg(0x89);     
    WriteDataByte(0x21); 
  
    WriteDataReg(0x8A);     
    WriteDataByte(0x00); 
  
    WriteDataReg(0x8B);     
    WriteDataByte(0x80); 
  
    WriteDataReg(0x8C);     
    WriteDataByte(0x01); 
  
    WriteDataReg(0x8D);     
    WriteDataByte(0x01); 
  
    WriteDataReg(0x8E);     
    WriteDataByte(0xFF); 
  
    WriteDataReg(0x8F);     
    WriteDataByte(0xFF); 
  
  
    WriteDataReg(0xB6);
    WriteDataByte(0x00);
    WriteDataByte(0x20);
  
    WriteDataReg(0x36);
    WriteDataByte(0x08);
  
    WriteDataReg(0x3A);     
    WriteDataByte(0x05); 
  
    WriteDataReg(0x90);     
    WriteDataByte(0x08);
    WriteDataByte(0x08);
    WriteDataByte(0x08);
    WriteDataByte(0x08); 
  
    WriteDataReg(0xBD);     
    WriteDataByte(0x06);
    
    WriteDataReg(0xBC);     
    WriteDataByte(0x00); 
  
    WriteDataReg(0xFF);     
    WriteDataByte(0x60);
    WriteDataByte(0x01);
    WriteDataByte(0x04);
  
    WriteDataReg(0xC3);     
    WriteDataByte(0x13);
    WriteDataReg(0xC4);     
    WriteDataByte(0x13);
  
    WriteDataReg(0xC9);     
    WriteDataByte(0x22);
  
    WriteDataReg(0xBE);     
    WriteDataByte(0x11); 
  
    WriteDataReg(0xE1);     
    WriteDataByte(0x10);
    WriteDataByte(0x0E);
  
    WriteDataReg(0xDF);     
    WriteDataByte(0x21);
    WriteDataByte(0x0c);
    WriteDataByte(0x02);
  
    WriteDataReg(0xF0);   
    WriteDataByte(0x45);
    WriteDataByte(0x09);
    WriteDataByte(0x08);
    WriteDataByte(0x08);
    WriteDataByte(0x26);
    WriteDataByte(0x2A);
  
    WriteDataReg(0xF1);    
    WriteDataByte(0x43);
    WriteDataByte(0x70);
    WriteDataByte(0x72);
    WriteDataByte(0x36);
    WriteDataByte(0x37);  
    WriteDataByte(0x6F);
  
  
    WriteDataReg(0xF2);   
    WriteDataByte(0x45);
    WriteDataByte(0x09);
    WriteDataByte(0x08);
    WriteDataByte(0x08);
    WriteDataByte(0x26);
    WriteDataByte(0x2A);
  
    WriteDataReg(0xF3);   
    WriteDataByte(0x43);
    WriteDataByte(0x70);
    WriteDataByte(0x72);
    WriteDataByte(0x36);
    WriteDataByte(0x37); 
    WriteDataByte(0x6F);
  
    WriteDataReg(0xED); 
    WriteDataByte(0x1B); 
    WriteDataByte(0x0B); 
  
    WriteDataReg(0xAE);     
    WriteDataByte(0x77);
    
    WriteDataReg(0xCD);     
    WriteDataByte(0x63);   
  
  
    WriteDataReg(0x70);     
    WriteDataByte(0x07);
    WriteDataByte(0x07);
    WriteDataByte(0x04);
    WriteDataByte(0x0E); 
    WriteDataByte(0x0F); 
    WriteDataByte(0x09);
    WriteDataByte(0x07);
    WriteDataByte(0x08);
    WriteDataByte(0x03);
  
    WriteDataReg(0xE8);     
    WriteDataByte(0x34);
  
    WriteDataReg(0x62);     
    WriteDataByte(0x18);
    WriteDataByte(0x0D);
    WriteDataByte(0x71);
    WriteDataByte(0xED);
    WriteDataByte(0x70); 
    WriteDataByte(0x70);
    WriteDataByte(0x18);
    WriteDataByte(0x0F);
    WriteDataByte(0x71);
    WriteDataByte(0xEF);
    WriteDataByte(0x70); 
    WriteDataByte(0x70);
  
    WriteDataReg(0x63);     
    WriteDataByte(0x18);
    WriteDataByte(0x11);
    WriteDataByte(0x71);
    WriteDataByte(0xF1);
    WriteDataByte(0x70); 
    WriteDataByte(0x70);
    WriteDataByte(0x18);
    WriteDataByte(0x13);
    WriteDataByte(0x71);
    WriteDataByte(0xF3);
    WriteDataByte(0x70); 
    WriteDataByte(0x70);
  
    WriteDataReg(0x64);     
    WriteDataByte(0x28);
    WriteDataByte(0x29);
    WriteDataByte(0xF1);
    WriteDataByte(0x01);
    WriteDataByte(0xF1);
    WriteDataByte(0x00);
    WriteDataByte(0x07);
  
    WriteDataReg(0x66);     
    WriteDataByte(0x3C);
    WriteDataByte(0x00);
    WriteDataByte(0xCD);
    WriteDataByte(0x67);
    WriteDataByte(0x45);
    WriteDataByte(0x45);
    WriteDataByte(0x10);
    WriteDataByte(0x00);
    WriteDataByte(0x00);
    WriteDataByte(0x00);
  
    WriteDataReg(0x67);     
    WriteDataByte(0x00);
    WriteDataByte(0x3C);
    WriteDataByte(0x00);
    WriteDataByte(0x00);
    WriteDataByte(0x00);
    WriteDataByte(0x01);
    WriteDataByte(0x54);
    WriteDataByte(0x10);
    WriteDataByte(0x32);
    WriteDataByte(0x98);
  
    WriteDataReg(0x74);     
    WriteDataByte(0x10); 
    WriteDataByte(0x85); 
    WriteDataByte(0x80);
    WriteDataByte(0x00); 
    WriteDataByte(0x00); 
    WriteDataByte(0x4E);
    WriteDataByte(0x00);         
    
    WriteDataReg(0x98);     
    WriteDataByte(0x3e);
    WriteDataByte(0x07);
  
    WriteDataReg(0x35); 
    WriteDataReg(0x21);
  
    WriteDataReg(0x11);
    delay(120);
    WriteDataReg(0x29);
    delay(20);
  }

  // Basic Command
  void SetCursor(uint16_t x, uint16_t y, uint16_t x_end, uint16_t y_end)
  {
    if(x < 0) x = 0;
    else if(x > Width) x = Width;
    if(y < 0) y = 0;
    else if(y > Height) y = Height;
    
    WriteDataReg(0x2a);
    WriteDataByte(0x00);
    WriteDataByte(x);
    WriteDataByte(0x00);  
    WriteDataByte(x_end);
    
    WriteDataReg(0x2b);
    WriteDataByte(0x00);
    WriteDataByte(y);
    WriteDataByte(0x00);
    WriteDataByte(y_end);
    
    WriteDataReg(0x2c);
  }

  void Clear(uint16_t Color)
  {
    ClearWindow(0, 0, Width, Height, Color);
  }

  void ClearWindow(uint16_t x, uint16_t y, uint16_t x_end, uint16_t y_end, uint16_t Color)
  {
    uint16_t i, j;
    SetCursor(x, y, x_end-1, y_end-1);
    for(i = x; i < x_end; i++)
      for(j = y; j < y_end; j++)
        WriteDataWord(Color);
  }
  
  void SetWindowColor(uint16_t x, uint16_t y, uint16_t x_end, uint16_t y_end, uint16_t Color)
  {
    SetCursor(x, y, x_end, y_end);
    WriteDataWord(Color);
  }

  void SetPointColor(uint16_t x, uint16_t y, uint16_t Color)
  {
    SetCursor(x, y, x, y);
    WriteDataWord(Color);
  }
};

#endif
