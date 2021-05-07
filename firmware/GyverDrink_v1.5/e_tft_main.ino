/*
 An example analogue meter using a HX8357 TFT LCD screen

 Needs Font 2 (also Font 4 if using large scale label)

 Make sure all the required fonts are loaded by editting the
 User_Setup.h file in the TFT_HX8357 library folder.

  #########################################################################
  ###### DON'T FORGET TO UPDATE THE User_Setup.h FILE IN THE LIBRARY ######
  ######       TO SELECT THE FONTS AND PINS YOU USE, SEE ABOVE       ######
  #########################################################################
 
Updated by Bodmer for variable meter size
 */



void tftClear() {
    tft.setTextColor(TFT_BLACK);
}

void tftTick() {
  
}
