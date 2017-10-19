#include <Wire.h>
#include <LiquidCrystal_I2C.h>

LiquidCrystal_I2C lcd(0x27, 16, 2);

const uint8_t BUTTON_PIN = 3;

const uint8_t LCD_WELCOME = 0x00;
const uint8_t LCD_NEW_GAME = 0x01;
const uint8_t LCD_PLAYER_SELECT = 0x02;
const uint8_t LCD_PLAYER_TURN = 0x03;
const uint8_t LCD_GAME_OVER = 0x04;
const uint8_t LCD_WINNING = 0x05;

uint8_t heart[8] = {0x0, 0xa, 0x1f, 0x1f, 0xe, 0x4, 0x0};
uint8_t lcd_status = LCD_WELCOME;

void setup() {
  lcd.begin();  
  lcd.backlight();
  lcd.createChar(3, heart);

  pinMode(BUTTON_PIN, INPUT);
  pinMode(BUTTON_PIN, INPUT_PULLUP);
}


void lcd_welcome() {
  lcd.print("Simon Says V.1.3");
}

void lcd_new_game() {
  lcd.print("New Game");
  lcd.setCursor(0, 1);
  lcd.print("Press A Button");
}

void lcd_player_select() {
  lcd.print("Player Select");
}

void lcd_player_turn() {
  lcd.setCursor(0, 0);
  lcd.setCursor(13, 0);
  lcd.write(3);
  lcd.write(3);
  lcd.write(3);
}

void lcd_refresh() {
  lcd.clear();
  lcd.setCursor(0, 0);
  switch(lcd_status) {
    case LCD_WELCOME:
      lcd_welcome();
      break;
    case LCD_NEW_GAME:
      lcd_new_game();
      break;
    case LCD_PLAYER_SELECT:
      lcd_player_select();
      lcd_status++; // skip this
      break;
    case LCD_PLAYER_TURN:
      lcd_player_turn();
      break;
    default:
      break;
  }
}
  
void loop() {
  lcd_refresh();
  delay(500);

  if(lcd_status == LCD_WELCOME) {
    delay(3000);
    lcd_status = LCD_NEW_GAME;
  }

  if(digitalRead(BUTTON_PIN) == LOW && lcd_status == LCD_NEW_GAME) { // Button pressed
    lcd_status = LCD_PLAYER_SELECT;
  }
}
