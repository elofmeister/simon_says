#include <Wire.h>
#include <LiquidCrystal_I2C.h>

#if defined(ARDUINO) && ARDUINO >= 100
#define printByte(args)  write(args);
#else
#define printByte(args)  print(args,BYTE);
#endif

LiquidCrystal_I2C lcd(0x27, 16, 2);

const uint8_t LCD_DISPLAY_SIZE = 16;
const uint8_t LCD_ANIMATION_OFFSET = 6;
const uint8_t LCD_FAST_ANIMATION = 50; //time in miliseconds
const uint8_t LCD_SLOW_ANIMATION = 200;


const uint8_t BUTTON_PIN = 3;

const uint8_t LCD_WELCOME = 0x00;
const uint8_t LCD_NEW_GAME = 0x01;
const uint8_t LCD_PLAYER_SELECT = 0x02;
const uint8_t LCD_PLAYER_TURN = 0x03;
const uint8_t LCD_GAME_OVER = 0x04;
const uint8_t LCD_WINNING = 0x05;

//custom characters
uint8_t block[8] = {0x1F, 0x1F, 0x1F, 0x1F, 0x1F, 0x1F, 0x1F, 0x1F};
uint8_t S[8] = {0x11, 0x16, 0x17, 0x19, 0x1E, 0x0E, 0x01, 0x1F};
uint8_t I[8] = {0x0C, 0x06, 0x08, 0x0C, 0x0C, 0x0C, 0x0C, 0x0C};
uint8_t M[8] = {0x04, 0x1F, 0x1D, 0x15, 0x15, 0x15, 0x15, 0x15};
uint8_t O[8] = {0x0E, 0x0A, 0x11, 0x11, 0x11, 0x11, 0x13, 0x0E};
uint8_t N[8] = {0x05, 0x0D, 0x1D, 0x15, 0x17, 0x13, 0x13, 0x11};
uint8_t A[8] = {0x02, 0x05, 0x05, 0x09, 0x1F, 0x11, 0x11, 0x11};
uint8_t Y[8] = {0x11, 0x11, 0x19, 0x07, 0x06, 0x04, 0x04, 0x1C};
uint8_t heart[8] = {0x0, 0xa, 0x1f, 0x1f, 0xe, 0x4, 0x0};
uint8_t skull[8] = {0x0E, 0x1F, 0x15, 0x1F, 0x1B, 0x0E, 0x0E, 0x00};
uint8_t crown[8] = {0x04, 0x0E, 0x04, 0x15, 0x1F, 0x1F, 0x1F, 0x00};

//loop variables
int i = 0;
int j = 0;
int k = 0;
int l = 1;

uint8_t randalf = 0; //global Seedvalue

uint8_t lcd_status = LCD_WINNING;


void setup() {
  lcd.begin();  
  lcd.backlight();
  Serial.begin(9600);

  pinMode(BUTTON_PIN, INPUT);   //is not necessary
  pinMode(BUTTON_PIN, INPUT_PULLUP);
}


void lcd_welcome() {
   
  lcd.createChar(0, block);  
  lcd.createChar(1, S);
  lcd.createChar(2, I);
  lcd.createChar(3, M);
  lcd.createChar(4, O);
  lcd.createChar(5, N);
  lcd.createChar(6, A);
  lcd.createChar(7, Y);

while (k <= 4)
 {
    while (i <= LCD_DISPLAY_SIZE + LCD_ANIMATION_OFFSET)
    {
    lcd.setCursor(i, 0);
    lcd.printByte(0);  
    lcd.setCursor(i, 1);
    lcd.printByte(0);
     
    i++;
  
    if (i >= 8)
        {
        lcd.setCursor(j, 0);
        lcd.print(" ");    
        lcd.setCursor(j, 1);
        lcd.print(" ");      
        j++;
        }

     if (j>3 && j<9 && k>=1)  //print Simon
     {
      lcd.setCursor(j-1, 0);
      lcd.printByte(l);
      l++;  
     }

     if (k >= 2)                //print Says
     { 
       switch (j){
       
       case 10:lcd.setCursor(j-1, 0);
               lcd.printByte(1);
               break;
       case 11:lcd.setCursor(j-1, 0);
               lcd.printByte(6);
               break;
       case 12:lcd.setCursor(j-1, 0);
               lcd.printByte(7);
               break;
       case 13:lcd.setCursor(j-1, 0);
               lcd.printByte(1);
               break;
       default: break;        
       }      
     }
     
    if (j > LCD_ANIMATION_OFFSET && k==3)        //print version
    {
      lcd.setCursor(j-7, 1);
      lcd.print(" V.1.3 ");
    }
    else;
          
    delay(LCD_FAST_ANIMATION);
    }
    
    i=0;
    j=0;
    l=1;
    k++;
 }
}

void lcd_new_game() {
lcd.createChar(0, block);  
i = LCD_DISPLAY_SIZE + LCD_ANIMATION_OFFSET;
j = LCD_DISPLAY_SIZE;
k = 0;
while (k <= 2)
 {
    while (i >= -LCD_ANIMATION_OFFSET)
    {
    lcd.setCursor(i, 0);
    lcd.write(0);  
    lcd.setCursor(i, 1);
    lcd.printByte(0);
     
    i--;
  
    if (i <= LCD_DISPLAY_SIZE - LCD_ANIMATION_OFFSET)
        {
        lcd.setCursor(j, 0);
        lcd.print(" ");    
        lcd.setCursor(j, 1);
        lcd.print(" ");      
        j--;
        }

     if (j <= 12)
     {
       lcd.setCursor(j+1, 0);   
       lcd.print("    NEW GAME ");
     }
        delay(LCD_FAST_ANIMATION);      
     }

 
  lcd.setCursor(0, 1);
  lcd.print(" Press A Button");
  delay(LCD_SLOW_ANIMATION);  
  k++;
 }
 
 // Button interruptzz und so
  randalf = micros();   
}

 

void lcd_player_select() {
  lcd.print("Player Select");
}

void lcd_player_turn() 
{
  lcd.createChar(0, skull);  
  lcd.createChar(1, heart); 
  
  lcd.setCursor(0, 0);
  lcd.write(0);
  lcd.write(0);
  lcd.write(0);
  lcd.setCursor(13, 0);
  lcd.write(1);
  lcd.write(1);
  lcd.write(1);
  lcd.setCursor(0, 1);
  lcd.print("Player:  Turn:"); 
}

void lcd_game_over()  
{
 lcd.createChar(0, skull);
 i=0;
 j=0;
 k=0;    
 while (k <= 64) {     //disable the loop and set delay to LCD_FAST_ANIMATION to have normal effect
    while (i <= LCD_DISPLAY_SIZE + LCD_ANIMATION_OFFSET)
    {
    lcd.setCursor(i, 0);
    lcd.write(0);        
    i++;
  
    if (i >= LCD_ANIMATION_OFFSET)
        {
        lcd.setCursor(j, 0);
        lcd.print(" ");               
        j++;
        }     
    delay(2);
    } 
   
    while (i > -LCD_ANIMATION_OFFSET +1)
    {
    lcd.setCursor(i, 1);
    lcd.write(0);        
    i--;
  
    if (i <= LCD_DISPLAY_SIZE - LCD_ANIMATION_OFFSET +1)
        {
        lcd.setCursor(j, 1);
        lcd.print(" ");               
        j--;
        }     
    delay(2);
    }
   k++;
 }
 lcd.setCursor(0, 0);
 lcd.clear();
 lcd.print("    G A M E "); 
 lcd.setCursor(0, 1);
 lcd.print("    O V E R ");
 delay(LCD_SLOW_ANIMATION*10); 
}


void lcd_winning()
{
  lcd.createChar(1, crown);
  lcd.setCursor(0, 0);
  lcd.write(1);
  lcd.write(1);
  lcd.print("   YOU WIN  ");
  lcd.write(1);
  lcd.write(1); 
  lcd.setCursor(0, 1);
  lcd.write(1);
  lcd.write(1);
  lcd.write(1);
  lcd.print("  BEST!!! "); 
  lcd.write(1);
  lcd.write(1);
  lcd.write(1);
  delay(LCD_SLOW_ANIMATION*10); 
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
    case LCD_GAME_OVER:
      lcd_game_over();
      break;
    case LCD_WINNING:
      lcd_winning();
      break;  
    default:
      break;
  }
}
  
void loop() {
  lcd_refresh();
 // delay(500);

  if(lcd_status == LCD_WELCOME) {
     lcd_status = LCD_NEW_GAME;   
  }


  if(digitalRead(BUTTON_PIN) == LOW && lcd_status == LCD_NEW_GAME) { // Button pressed
    lcd_status = LCD_PLAYER_SELECT;
  }
}
