#include <MD_MAX72xx.h>

// Connection for Arduino Uno:
// DISP_CLK_PIN   13  // or SCK
// DISP_DATA_PIN  11  // or MOSI
// DISP_CS_PIN    see ctor  // or SS
class MdMax8x8Display : public MatrDisplayDevice {
  public: 
  MdMax8x8Display(uint8_t csPin);
  void displayBitmap(uint8_t* bmp) const override;
  void displayChar(char ch) const override;
  void displayPercent(int val) const override;
  void clear() const override;
  private:
  MD_MAX72XX* mx;
};

MdMax8x8Display::MdMax8x8Display(uint8_t csPin) : mx( new MD_MAX72XX(MD_MAX72XX::FC16_HW, csPin)) {
  mx->begin();
}

void MdMax8x8Display::displayBitmap(uint8_t* bmp) const {
  for (uint8_t i=0; i<8; i++) mx->setRow(i, bmp[i]);
}

void MdMax8x8Display::displayChar(char ch) const {
  mx->clear();
  mx->setChar(7, ch);
}

void MdMax8x8Display::displayPercent(int val) const {
      byte dotsNo = 64 * val / 100;
      for (uint8_t col=0; col<8; col++) {  
        for (uint8_t row=0; row<8; row++) {    
          mx->setPoint(7-col, 7-row, col * 8 + row <= dotsNo);
        }
      }
}

void MdMax8x8Display::clear() const {
  mx -> clear();
}
