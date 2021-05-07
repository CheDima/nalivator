// Define meter size as multiplier ot 240 pixels wide 1.0 and 1.3333 work OK
#define TFT_GREY 0x5AEB

struct analogMeterParam {
  float scale = 2;
  float ltx = 0;    // Saved x coord of bottom of needle
  uint16_t osx = scale*120, osy = scale*120; // Saved x & y coords
  uint32_t updateTime = 0;       // time for next update
  
  int old_analog =  -999; // Value last displayed
  
  int value[6] = {0, 0, 0, 0, 0, 0};
  int old_value[6] = { -1, -1, -1, -1, -1, -1};
  int d = 0;
};

struct analogMeterParam am;

// #########################################################################
//  Draw the analogue meter on the screen
// #########################################################################
void analogMeter()
{

  // Meter outline
  tft.fillRect(0, 0, am.scale*239, am.scale*126, TFT_GREY);
  tft.fillRect(5, 3, am.scale*230, am.scale*119, TFT_WHITE);

  tft.setTextColor(TFT_BLACK);

  // Draw ticks every 5 degrees from -50 to +50 degrees (100 deg. FSD swing)
  for (int i = -50; i < 51; i += 5) {
    // Long scale tick length
    int tl = 15;

    // Coodinates of tick to draw
    float sx = cos((i - 90) * 0.0174532925);
    float sy = sin((i - 90) * 0.0174532925);
    uint16_t x0 = sx * (am.scale*100 + tl) + am.scale*120;
    uint16_t y0 = sy * (am.scale*100 + tl) + am.scale*140;
    uint16_t x1 = sx * am.scale*100 + am.scale*120;
    uint16_t y1 = sy * am.scale*100 + am.scale*140;

    // Coordinates of next tick for zone fill
    float sx2 = cos((i + 5 - 90) * 0.0174532925);
    float sy2 = sin((i + 5 - 90) * 0.0174532925);
    int x2 = sx2 * (am.scale*100 + tl) + am.scale*120;
    int y2 = sy2 * (am.scale*100 + tl) + am.scale*140;
    int x3 = sx2 * am.scale*100 + am.scale*120;
    int y3 = sy2 * am.scale*100 + am.scale*140;

    // Yellow zone limits
    //if (i >= -50 && i < 0) {
    //  tft.fillTriangle(x0, y0, x1, y1, x2, y2, TFT_YELLOW);
    //  tft.fillTriangle(x1, y1, x2, y2, x3, y3, TFT_YELLOW);
    //}

    // Green zone limits
    if (i >= 0 && i < 25) {
      tft.fillTriangle(x0, y0, x1, y1, x2, y2, TFT_GREEN);
      tft.fillTriangle(x1, y1, x2, y2, x3, y3, TFT_GREEN);
    }

    // Orange zone limits
    if (i >= 25 && i < 50) {
      tft.fillTriangle(x0, y0, x1, y1, x2, y2, TFT_ORANGE);
      tft.fillTriangle(x1, y1, x2, y2, x3, y3, TFT_ORANGE);
    }

    // Short scale tick length
    if (i % 25 != 0) tl = 8;

    // Recalculate coords incase tick lenght changed
    x0 = sx * (am.scale*100 + tl) + am.scale*120;
    y0 = sy * (am.scale*100 + tl) + am.scale*140;
    x1 = sx * am.scale*100 + am.scale*120;
    y1 = sy * am.scale*100 + am.scale*140;

    // Draw tick
    tft.drawLine(x0, y0, x1, y1, TFT_BLACK);

    // Check if labels should be drawn, with position tweaks
    if (i % 25 == 0) {
      // Calculate label positions
      x0 = sx * (am.scale*100 + tl + 10) + am.scale*120;
      y0 = sy * (am.scale*100 + tl + 10) + am.scale*140;
      switch (i / 25) {
        case -2: tft.drawCentreString("0", x0, y0 - 12, 2); break;
        case -1: tft.drawCentreString("25", x0, y0 - 9, 2); break;
        case 0: tft.drawCentreString("50", x0, y0 - 7, 2); break;
        case 1: tft.drawCentreString("75", x0, y0 - 9, 2); break;
        case 2: tft.drawCentreString("100", x0, y0 - 12, 2); break;
      }
    }

    // Now draw the arc of the scale
    sx = cos((i + 5 - 90) * 0.0174532925);
    sy = sin((i + 5 - 90) * 0.0174532925);
    x0 = sx * am.scale*100 + am.scale*120;
    y0 = sy * am.scale*100 + am.scale*140;
    // Draw scale arc, don't draw the last part
    if (i < 50) tft.drawLine(x0, y0, x1, y1, TFT_BLACK);
  }

  tft.drawString("%RH", am.scale*(5 + 230 - 40), am.scale*(119 - 20), 2); // Units at bottom right
  tft.drawCentreString("%RH", am.scale*120, am.scale*70, 4); // Comment out to avoid font 4
  tft.drawRect(5, 3, am.scale*230, am.scale*119, TFT_BLACK); // Draw bezel line

  //plotNeedle(0, 0); // Put meter needle at 0
}

// #########################################################################
// Update needle position
// This function is blocking while needle moves, time depends on ms_delay
// 10ms minimises needle flicker if text is drawn within needle sweep area
// Smaller values OK if text not in sweep area, zero for instant movement but
// does not look realistic... (note: 100 increments for full scale deflection)
// #########################################################################
void plotNeedle(int value, byte ms_delay)
{
  tft.setTextColor(TFT_BLACK, TFT_WHITE);
  char buf[8]; dtostrf(value, 4, 0, buf);
  tft.drawRightString(buf, am.scale*40, am.scale*(119 - 20), 2);

  if (value < -10) value = -10; // Limit value to emulate needle end stops
  if (value > 110) value = 110;

  // Move the needle until new value reached
  while (!(value == am.old_analog)) {
    if (am.old_analog < value) am.old_analog++;
    else am.old_analog--;

    if (ms_delay == 0) am.old_analog = value; // Update immediately if delay is 0

    float sdeg = map(am.old_analog, -10, 110, -150, -30); // Map value to angle
    // Calcualte tip of needle coords
    float sx = cos(sdeg * 0.0174532925);
    float sy = sin(sdeg * 0.0174532925);

    // Calculate x delta of needle start (does not start at pivot point)
    float tx = tan((sdeg + 90) * 0.0174532925);

    // Erase old needle image
    tft.drawLine(am.scale*(120 + 20 * am.ltx - 1), am.scale*(140 - 20), am.osx - 1, am.osy, TFT_WHITE);
    tft.drawLine(am.scale*(120 + 20 * am.ltx), am.scale*(140 - 20), am.osx, am.osy, TFT_WHITE);
    tft.drawLine(am.scale*(120 + 20 * am.ltx + 1), am.scale*(140 - 20), am.osx + 1, am.osy, TFT_WHITE);

    // Re-plot text under needle
    tft.setTextColor(TFT_BLACK);
    tft.drawCentreString("%RH", am.scale*120, am.scale*70, 4); // // Comment out to avoid font 4

    // Store new needle end coords for next erase
    am.ltx = tx;
    am.osx = am.scale*(sx * 98 + 120);
    am.osy = am.scale*(sy * 98 + 140);

    // Draw the needle in the new postion, magenta makes needle a bit bolder
    // draws 3 lines to thicken needle
    tft.drawLine(am.scale*(120 + 20 * am.ltx - 1), am.scale*(140 - 20), am.osx - 1, am.osy, TFT_RED);
    tft.drawLine(am.scale*(120 + 20 * am.ltx), am.scale*(140 - 20), am.osx, am.osy, TFT_MAGENTA);
    tft.drawLine(am.scale*(120 + 20 * am.ltx + 1), am.scale*(140 - 20), am.osx + 1, am.osy, TFT_RED);

    // Slow needle down slightly as it approaches new postion
    if (abs(am.old_analog -  value) < 10) ms_delay += ms_delay / 5;

    // Wait before next update
    delay(ms_delay);
  }
}
