#include <Adafruit_GFX.h>
#include <Adafruit_SSD1306.h>

#define OLED_RESET 0  // GPIO0

// Shield for Wemos D1 mini (0.66', see https://docs.wemos.cc/en/latest/d1_mini_shiled/oled_0_66.html)
// uses D1 and D2
class Oled64x48Display : public MatrDisplayDevice {
  public:
    Oled64x48Display();
    void displayBitmap(uint8_t* bmp) const override;
    void displayChar(char ch) const override;
    void displayPercent(int val) const override;
    void clear() const override;
  private:
    Adafruit_SSD1306* mx;
};

Oled64x48Display::Oled64x48Display() : mx(new Adafruit_SSD1306(OLED_RESET)) {
  mx->begin(SSD1306_SWITCHCAPVCC, 0x3C);
  mx->clearDisplay();
  mx->display();  
}

void Oled64x48Display::displayBitmap(uint8_t* bmp) const {
  mx->drawBitmap(0, 0, bmp, 64, 48, WHITE);
  mx->display();
}

void Oled64x48Display::displayChar(char ch) const {
    mx->setTextSize(2);
    mx->setTextColor(WHITE);
    mx->setCursor(10,0);
    mx->clearDisplay();
    mx->println(ch);
    mx->display();
}

void Oled64x48Display::displayPercent(int val) const {

}

void Oled64x48Display::clear() const {
  mx->clearDisplay();
  mx->display();
}
