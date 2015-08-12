


#define BUTTON_PRESS_TIME 200    // button press time in ms

unsigned long buttonPressTime = 0;

////////////////////////////////////////////


void write(char* text) {
  write(0, 0, text);
}

void write(char* text1, char* text2) {
  write(0, 0, text1);
  overwrite(0, 1, text2);
}

void write(short column, uint8_t row,  char* text) {
  lcd.clear();
  overwrite(column, row, text);
}

void overwrite(short column, uint8_t row, char* text) {
  lcd.setCursor(column, row);
  lcd.print(text);
}

void clearline(short column, uint8_t row) {
  lcd.setCursor(column, row);
  lcd.print("                ");
}

///////////////////////////////////////////////////
//getButton
//returns the button code
///////////////////////////////////////////////////
uint8_t getButton() {
  if (millis() - buttonPressTime >= BUTTON_PRESS_TIME) {
    buttonPressTime = millis(); // reset press time

    uint16_t keyValue = analogRead(0);

    if (keyValue > 790) return BUTTON_NONE;
    if (keyValue < 50) {
      return BUTTON_RIGHT;
    }
    if (keyValue < 195) {
      return BUTTON_UP;
    }
    if (keyValue < 380) {
      return BUTTON_DOWN;
    }
    if (keyValue < 555) {
      return BUTTON_LEFT;
    }
    if (keyValue < 790) {
      return BUTTON_SELECT;
    }
  }
  return BUTTON_NONE;
}
