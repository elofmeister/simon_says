#include <boarddefs.h>
#include <IRremote.h>
#include <IRremoteInt.h>
#include <ir_Lego_PF_BitStreamEncoder.h>
#include <avr/wdt.h>

#include <Wire.h>
#include <LiquidCrystal_I2C.h>

#if defined(ARDUINO) && ARDUINO >= 100
#define printByte(args)  write(args);
#else
#define printByte(args)  print(args,BYTE);
#endif



LiquidCrystal_I2C lcd(0x27, 16, 2);
//display variables
const uint8_t LCD_DISPLAY_SIZE = 16;
const uint8_t LCD_ANIMATION_OFFSET = 6;
const uint8_t LCD_FAST_ANIMATION = 50; //time in miliseconds
const uint8_t LCD_SLOW_ANIMATION = 200;
const int     LCD_NO_ANIMATION = 1000;

//pin layout
const uint8_t IR_PIN = 11;
const uint8_t LED_PIN = A3;
const uint8_t BUTTON_PIN = 5;
const uint8_t R_PIN = A0;
const uint8_t G_PIN = A1;
const uint8_t B_PIN = A2;

//menu screen flag variables
const uint8_t LCD_WELCOME = 0x00;
const uint8_t LCD_NEW_GAME = 0x01;
const uint8_t LCD_PLAYER_SELECT = 0x02;
const uint8_t LCD_PLAYER_TURN = 0x03;
const uint8_t LCD_GAME_OVER = 0x04;
const uint8_t LCD_WINNING = 0x05;
const uint8_t IR_TEST = 0x06;
const uint8_t LED_TEST = 0x07;

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
uint8_t star[8] = {0x04, 0x15, 0x0E, 0x0E, 0x1F, 0x0E, 0x0E, 0x15};

//loop variables
int i = 0;
int j = 0;
int k = 0;
int l = 1;

//random number generator variables 
long randalf = 0; //global random number and seedvalue
const long MULTIPLIER = 1103515245; //
const int INCREMENT = 12345; //stepwidth
const uint8_t MAX_VALUE = 10;  //maximal random value

//global display variable
uint8_t lcd_status = LCD_WELCOME;

// uint8_t player_lifes[][]; //keeps track of the number of turns and the specific number in sequence
const uint8_t STARTING_LIFES = 3;
const uint8_t MAX_PLAYERS = 9;  
const unsigned int GAMESPEED = 3000;  //speed of the numbers shown
long player_life_turns[MAX_PLAYERS][STARTING_LIFES];   
long simon_said[50];
uint8_t player_number;
uint8_t player_alive;
int turn_player = 0;  // turn player
int global_turns = 1;  //global turns
int number_count = 0; //counting said numbers 
uint8_t input = 0;

//IrRemote Part
IRrecv irrecv(IR_PIN);
decode_results results;

int refresh=0;

//Timer Part
uint8_t timer1_toggle = 0;
long timer1_cnt = 0;
long timer1_old_cnt=0; 

void setup() {  
  pinMode(BUTTON_PIN, INPUT);   //is not necessary
  pinMode(BUTTON_PIN, INPUT_PULLUP);
  lcd.begin();  
  lcd.backlight();
  Serial.begin(9600); //for IR, Dataconnection and other stuff
  irrecv.enableIRIn(); // Start the receiver
  pinMode(LED_PIN, OUTPUT);
  pinMode(R_PIN, OUTPUT);
  pinMode(G_PIN, OUTPUT);
  pinMode(B_PIN, OUTPUT);
  timer_setup();

  wdt_disable(); //disable the watchdog
  wdt_enable(WDTO_8S); //enable watchdog = 20s
}
  

void timer_setup()
{
 cli();  //disallow interrupts
 
  TCCR1A = 0;// set Timer1 Registers to 0
  TCCR1B = 0;
  TCNT1  = 0;//initialize counter value to 0
  // compare register set to 20Hz values
  OCR1A = 12500;// = (16000000)/(12500*64) = 20Hz (must be <65536)
  // turn on CTC mode --> no process point evaluation necessary anymore
  TCCR1B |= (1 << WGM12);
  // Set CS12 bits for 64-bit prescaler
  TCCR1B |= (1 << CS11)|(1 << CS10); 
  // enable timer compare interrupt
  TIMSK1 |= (1 << OCIE1A);

 sei(); //allow interrupts 
}


ISR(TIMER1_COMPA_vect)  //interupt sercvice routine for timer 1 
{
  
  timer1_cnt++;  
  
  if (timer1_toggle == 1){
    digitalWrite(LED_PIN,HIGH);  
    timer1_toggle = 0;  //shows that device is on
    }
  else if(timer1_toggle == 0){
    digitalWrite(LED_PIN,LOW);
    timer1_toggle = 1;  //shows that device is on
  }
}


void rgb_color( int red, int green, int blue) //sets the RGB-colors
{
  analogWrite(R_PIN, red);
  analogWrite(G_PIN, green);
  analogWrite(B_PIN, blue);
}

void ir_input()
{
  if (irrecv.decode(&results))
    {
     irrecv.resume(); // Receive the next value
    }
}

void randomer() //linear conguential generator , generating pseudorandom numbers = [0;9]
{   
  randalf = abs((MULTIPLIER * randalf + INCREMENT )% MAX_VALUE);   
}

void lcd_welcome() 
{ 
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
           switch (j)
           {       
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

void lcd_new_game() 
{
  lcd.createChar(0, block);  
  i = LCD_DISPLAY_SIZE + LCD_ANIMATION_OFFSET;
  j = LCD_DISPLAY_SIZE;
  k = 0;

      while (i >= -LCD_ANIMATION_OFFSET)    //Print moving Blocks
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
  
    while (lcd_status == LCD_NEW_GAME)
    {
    lcd.setCursor(0, 1);
    lcd.print(" Press A Button");  
      if (irrecv.decode(&results))
       { 
         lcd_status = LCD_PLAYER_SELECT;
         randalf = micros();   //initializing randalf with seed value (~ 110.000.000)
         irrecv.resume(); // ready to receive the next value from IR-Control
       }
     }          // Watchdog triggers here after ~3 sec inactivity
}

 

void lcd_player_select() 
{
  while (lcd_status == LCD_PLAYER_SELECT)
  {
  wdt_reset();  
  lcd.setCursor(0,0);
  lcd.print("Please select Number");
  lcd.setCursor(0,1);
  lcd.print("of Players (1-9)");
  delay(LCD_NO_ANIMATION);
  
    if (irrecv.decode(&results))
       { 
         set_input();             
         irrecv.resume(); // ready to receive the next value from IR-Control
       }
       
     if (input >= 1 && input <= 9) 
     {
         //setting up players and lifepoints
        player_number = input;
        player_alive = player_number;
        for(i=0;i<=input;i++)
        {
           player_life_turns[i][1]=STARTING_LIFES;
        }
        lcd.clear();
        lcd.setCursor(2,0);
        lcd.print("You Choose:"); 
        lcd.setCursor(0,1);
        
        if (input == 1)
         {
           lcd.print("Single Player");
         }
        else
         {
          lcd.print(" ");
          lcd.print(input);
          lcd.print("-Player Mode");
         }
        delay(LCD_NO_ANIMATION*3);  
        lcd_status = LCD_PLAYER_TURN;    
     }

  }   
}

void lcd_player_turn() 
{

 lcd.createChar(0, skull);  
 lcd.createChar(1, heart); 
 lcd.clear();
 
    timer1_cnt=0; //reseting timer
    wdt_disable(); //turning of watchdog 
  
  while (1)
  {
    // alternatively = sizeof(player_life_turns) / sizeof(player_life_turns[0][0])/3;
    
   
    lcd.setCursor(0, 0);
    switch (player_life_turns[turn_player][1])
    { 
      case (0): lcd.write(0);
                lcd.write(0);
                lcd.write(0);
                break;
      case (1): lcd.write(0);
                lcd.write(0);
                lcd.write(1);
                break;
      case (2): lcd.write(0);
                lcd.write(1);
                lcd.write(1);
                break;
      case (3): lcd.write(1);
                lcd.write(1);
                lcd.write(1);
                break;                
    }

    lcd.setCursor(0, 1);
    switch (player_life_turns[turn_player][1])
    { 
      case (0): 
                lcd.print("Player:");
                lcd.print(turn_player+1);  
                lcd.print("DEAD:");
                lcd.print(player_life_turns[turn_player][2]);
                delay(LCD_SLOW_ANIMATION*5);  // Marker für maddin
                break;
      default: 
               lcd.print("Player:");
               lcd.print(turn_player+1);  
               lcd.print("Turn:");
               lcd.print(global_turns+1);
               delay(LCD_SLOW_ANIMATION*20);  // Marker für maddin
               game();
               player_input();  
               break;
    }
         
    
    
    turn_player++;
    
    if (turn_player==player_number)
    {
      turn_player=0;
      global_turns++;
      randomer(); 
      number_count++;   
      simon_said[number_count]=randalf;      
    }   
  }
 lcd.setCursor(0, 0);
    lcd.print("TIMER DOWN");
}

void game()
{
   if (global_turns == 0 && turn_player == 0) //first 3 numbers for the first turn
   {
    while(number_count<3)  
    {
     randomer(); 
     number_count++;
     simon_said[number_count]=randalf;
    }
   }
   
  lcd.clear();  
  for(int x=0; x<=number_count; x++)
  {
    lcd.setCursor(4,0);
    lcd.print(simon_said[x]);
    delay(LCD_NO_ANIMATION); //speedvalue / number of chars +- some modifier maybe
  }
  lcd.setCursor(0,1);
  lcd.print("go! go! go!");
  delay(LCD_NO_ANIMATION); 
}


void player_input()
{
  int input_count = 0;
 
  while(input_count<number_count)
  {
  lcd.setCursor(0,0);
  lcd.clear();
    if (irrecv.decode(&results))
         {          
           set_input();           
              
           if (simon_said[input_count]==input)
           {                
              lcd.print("right!!!");
              lcd.setCursor(1,1);
              lcd.print("right!!!"); 
              rgb_color(0, 255, 0); // green 
              delay(LCD_SLOW_ANIMATION);
              input_count++;
              player_life_turns[turn_player][2]++;    //correct entries counted for highscore     

             if (player_number - player_alive == 1) lcd_winning();
           }
           else 
           {
              lcd.print("wrong!!!");
              lcd.setCursor(1,1);
              lcd.print("wrong!!!");              
              rgb_color(255, 0, 0); // red
              delay(LCD_SLOW_ANIMATION);
              player_life_turns[turn_player][1]--;  //wrong entry = lesser life
              input_count=number_count;             //wrong entry = turn over
              
              if (player_life_turns[turn_player][1] == 0) lcd_game_over();
                  
           }
         lcd.clear();  
         irrecv.resume(); // ready to receive the next value from IR-Control          
        } 
  } 
}

void lcd_game_over()  
{
 k=0; 
 lcd.createChar(0, skull);
 
 wdt_enable(WDTO_8S);

  while (k<=1)
    { 
      k++;
       i=0;
       j=0;
       
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
      delay(LCD_FAST_ANIMATION*2);
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
      delay(LCD_FAST_ANIMATION*2);
      }
   lcd.setCursor(0, 0);
   lcd.clear();
   lcd.print("    G A M E "); 
   lcd.setCursor(0, 1);
   lcd.print("    O V E R ");
   delay(LCD_SLOW_ANIMATION*10);
   wdt_reset();
   }

player_alive--;                        //one player died 
if (player_number == 1) soft_reset();
if (player_number - player_alive == 0) soft_reset();
}


void lcd_winning()
{
 i = 0;
 j = 0;  
 k = 0; 
 lcd.createChar(0, crown);
 lcd.createChar(1, heart);
 lcd.createChar(2, star);
 
 wdt_enable(WDTO_8S);
 
  while (k<10)
  {
    lcd.setCursor(0, 0);
    lcd.write(i);
    lcd.write(i);
    lcd.print(" YOU WIN!!! ");
    lcd.write(i);
    lcd.write(i); 
    lcd.setCursor(0, 1);
    lcd.write(i);
    lcd.write(i);
    lcd.write(i);
    lcd.print("SCORE:");
    lcd.print(player_life_turns[turn_player][2]); 
    lcd.write(i);
    lcd.write(i);
    lcd.write(i);
    delay(LCD_SLOW_ANIMATION*5); 
    if (i==2) i=i-3;
    i++;
    j++;
    k++;  
  }
 if (player_number - player_alive == 1) soft_reset; 
}


void soft_reset() 
{
 delay(8001); //if delay is greater then 8seconds , immidiate watchdog reset
}


void set_input() //returns the pressed input
{
  switch(results.value)
  {
    
    case 0xFFC23D: input=11; // PLAY or PAUSE
                   break;    
    case 0xFF6897: input=0; //NULL
                   break;   
    case 0xFF30CF: input=1;  //1               
                   break; 
    case 0xFF18E7: input=2;  //2
                   break;  
    case 0xFF7A85: input=3;
                   break; 
    case 0xFF10EF: input=4;
                   break;
    case 0xFF38C7: input=5;
                   break; 
    case 0xFF5AA5: input=6;
                   break; 
    case 0xFF42BD: input=7;
                   break; 
    case 0xFF4AB5: input=8;
                   break;  
    case 0xFF52AD: input=9;
                   break; 
    case 0xFFFFFF: break;   //if button is pressed continuously  
    default:  break; 
  }
}


void lcd_refresh() 
{
  lcd.clear();
  lcd.setCursor(0, 0);
  switch(lcd_status) 
  {
    case LCD_WELCOME:
      rgb_color(0, 0, 255); // blue
      lcd_welcome();
      break;
    case LCD_NEW_GAME:
      lcd_new_game();
      break;
    case LCD_PLAYER_SELECT:
      lcd_player_select();
      rgb_color(0, 255, 0); // green
      break;
    case LCD_PLAYER_TURN:      
      rgb_color(255, 0, 255); // purple
      lcd_player_turn();
      break;
    case LCD_GAME_OVER:
      rgb_color(255, 255, 0); // yellow
      lcd_game_over();
      break;
    case LCD_WINNING:
      rgb_color(0, 255, 255); // aqua
      lcd_winning();
      break; 
    case IR_TEST:
      irtest();
      break;
    case LED_TEST:
     ledtest();
      break;  
    default:
      lcd.print("No Valid Input");
      lcd.setCursor(0,1);
      lcd.print("Try again!!!");
      delay(1000);
      break;
 
  }
}

  
void loop() {
  
  wdt_reset();  //resetting watchdog  
 lcd_refresh();


  if(lcd_status == LCD_WELCOME) {
     lcd_status = LCD_NEW_GAME;   
  }
  

/*
  if(digitalRead(BUTTON_PIN) == LOW && lcd_status == LCD_NEW_GAME) { // Button pressed
    lcd_status = LCD_PLAYER_SELECT;
  }
*/  
}


// -- TESTFUNCTIONS -- TESTFUNCTIONS -- TESTFUNCTIONS -- TESTFUNCTIONS -- TESTFUNCTIONS -- TESTFUNCTIONS -- TESTFUNCTIONS -- TESTFUNCTIONS -- 
void ledtest()                     // Lets the single LED-Blink
{
  digitalWrite(LED_PIN, HIGH);   // sets the LED on
  delay(500);                  // waits for a second
  digitalWrite(LED_PIN, LOW);    // sets the LED off
  delay(500);                  // waits for a second
}



void irtest()  //function is for IR_Control testing
{
  if (irrecv.decode(&results))
    {
    Serial.println(results.value, HEX); //send HexCode of pressed Button to SerialPort 
    buttontest();
    irrecv.resume(); // Receive the next value
    }
}

void buttontest() //displays the pressed Button on IR-Control on LCD-Screen
{
  switch(results.value)
  {
    case 0xFFA25D: lcd.print("CH-");  //CH-
                   delay(LCD_SLOW_ANIMATION*3);
                   break;
    case 0xFF629D: lcd.print("CH"); //CH
                   delay(LCD_SLOW_ANIMATION*3);
                   break; 
    case 0xFFE21D: lcd.print("CH+");  //CH+
                   delay(LCD_SLOW_ANIMATION*3);
                   break;   
    case 0xFF22DD: lcd.print("PREV"); //PREV
                   delay(LCD_SLOW_ANIMATION*3);
                   break;  
    case 0xFF02FD: lcd.print("NEXT");  //NEXT 
                   delay(LCD_SLOW_ANIMATION*3);
                   break; 
    case 0xFFC23D: lcd.print("PLAY"); //PLAY
                   delay(LCD_SLOW_ANIMATION*3);
                   break;  
    case 0xFFE01F: lcd.print("VOL-"); //VOL-
                   delay(LCD_SLOW_ANIMATION*3);
                   break;  
    case 0xFFA857: lcd.print("VOL+"); //VOL+
                   delay(LCD_SLOW_ANIMATION*3);
                   break;  
    case 0xFF906F: lcd.print("EQ"); //EQ
                   delay(LCD_SLOW_ANIMATION*3);
                   break;  
    case 0xFF6897: lcd.print("NULL"); //NULL
                   delay(LCD_SLOW_ANIMATION*3);
                   break;  
    case 0xFF9867: lcd.print("100+"); //100+
                   delay(LCD_SLOW_ANIMATION*3);
                   break;  
    case 0xFFB04F: lcd.print("200+"); //200+
                   delay(LCD_SLOW_ANIMATION*3);
                   break;  
    case 0xFF30CF: lcd.print("1");  //1 
                   delay(LCD_SLOW_ANIMATION*3);
                   break; 
    case 0xFF18E7: lcd.print("2"); //2 
                   delay(LCD_SLOW_ANIMATION*3);
                   break;  
    case 0xFF7A85: lcd.print("3"); //3
                   delay(LCD_SLOW_ANIMATION*3);
                   break; 
    case 0xFF10EF: lcd.print("4");  //4
                   delay(LCD_SLOW_ANIMATION*3);
                   break;
    case 0xFF38C7: lcd.print("5");  //5
                   delay(LCD_SLOW_ANIMATION*3);
                   break; 
    case 0xFF5AA5: lcd.print("6"); //6 
                   delay(LCD_SLOW_ANIMATION*3);
                   break; 
    case 0xFF42BD: lcd.print("7"); //7
                   delay(LCD_SLOW_ANIMATION*3);
                   break; 
    case 0xFF4AB5: lcd.print("8"); //8 
                   delay(LCD_SLOW_ANIMATION*3);
                   break;  
    case 0xFF52AD: lcd.print("9"); //9
                   delay(LCD_SLOW_ANIMATION*3);
                   break; 
    case 0xFFFFFF: lcd.print("zippp"); //BUTTON HOLD doesnt work due too delay
                   delay(LCD_SLOW_ANIMATION*3);
                   break;    
    default: break; 
  }
}
