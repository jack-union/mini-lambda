/*
   display functions

*/
#define SMALL_FONT u8g2_font_10x20_tf
#define LARGE_FONT u8g2_font_logisoso46_tn

void initDisplay() {
  display.setI2CAddress(0x78);  //0x3c * 2, default
  display.begin();
  display.setDrawColor(1);
  display.setFontMode(0);

  display.firstPage();
  do {
    draw_logo();
  } while (display.nextPage());
}

void draw_lambda(uint16_t readout) {
  char data[4] = "";
  char *info = "";

  // connection valid?
  if (lambdaError == 255) {
    display.setFont(LARGE_FONT);
    display.setCursor(10, 46);
    if (lambdaStatus & 0b00100000) {  // temp reached
      sprintf(data, "%03d", readout);
      // display sensor reading
      display.print(data[0]);
      display.print(".");
      display.print(data[1]);
      display.print(data[2]);
    } else {
      display.print("......");
    }

    // status infos
    if (lambdaStatus & 0b00100000) {  // temp reached
      info = " ";
    }
    if (lambdaStatus & 0b10000000) {  // heating
      info = "H";
    }
    if (lambdaStatus & 0b00001000) {  // voltage too high
      info = "U ++";
    }
    if (lambdaStatus & 0b00000100) {  // voltage too low
      info = "U --";
    }
    if (lambdaStatus & 0b00000010) {  // temp too high
      info = "T ++";
    }

    // show info
    display.setFont(SMALL_FONT);
    display.setCursor(10, 61);
    display.println(info);

    // show hint
    display.setCursor(80, 61);
    if (~lambdaStatus & 0b00100000) {  // temp not reached
      display.println(F("cold"));
    } else {
      if (readout < 100) {
        display.println(F("rich"));
      } else {
        display.println(F("lean"));
      }
    }
  } else {  // no valid connection
    display.setFont(SMALL_FONT);
    display.setCursor(20, 46);
    if (lambdaError == 127) {  // no sensor
      display.println(F("no sensor"));
    } else {  // all other errors
      display.println(F("no data"));
    }
  }
}

void draw_logo() {
  display.drawXBMP(
    (SCREEN_WIDTH - LOGO_WIDTH) / 2,
    (SCREEN_HEIGHT - LOGO_HEIGHT) / 2,
    LOGO_WIDTH, LOGO_HEIGHT,
    logo_bmp);
}
