// libraries
#include <LiquidCrystal_I2C.h>  // lcd display
#include <LedControl.h>         // led matrix
#include <avr/wdt.h>            // watchdog
#include <EEPROM.h>             // eeprom
#include <IRremote.h>           // ir

// macros
#if defined(ARDUINO) && ARDUINO >= 100
#define printByte(args)  write(args);
#else
#define printByte(args)  print(args,BYTE);
#endif

// lcd display
#define LCD_DISPLAY_ROWS      2
#define LCD_DISPLAY_COLUMNS   16
#define LCD_ANIMATION_OFFSET  6
#define LCD_FAST_ANIMATION    50    //time in miliseconds
#define LCD_SLOW_ANIMATION    200
#define LCD_NO_ANIMATION      1000

//pin layout
#define IR_PIN                13
#define LED_PIN               4
#define BUTTON_PIN            5
#define R_PIN                 5
#define G_PIN                 6
#define B_PIN                 7
#define DIN_PIN               12
#define CLK_PIN               10
#define CS_PIN                11

// menu screen flag variables
#define LCD_WELCOME           0
#define LCD_NEW_GAME          1
#define LCD_PLAYER_SELECT     2
#define LCD_PLAYER_TURN       3
#define LCD_GAME_OVER         4
#define LCD_WINNING           5
#define LCD_IR_TEST           6
#define LCD_GET_HIGHSCORE     7

// led matrix
#define LC_DIGIT_SHOW         400
#define LC_DIGIT_HIDE         200
#define LC_BRIGHTNESS         8     // 0~15

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

// digits
const byte DIGITS[][8] = {
{
  B00111000,
  B01000100,
  B01000100,
  B01000100,
  B01000100,
  B01000100,
  B01000100,
  B00111000
},{
  B00010000,
  B00110000,
  B00010000,
  B00010000,
  B00010000,
  B00010000,
  B00010000,
  B00111000
},{
  B00111000,
  B01000100,
  B00000100,
  B00000100,
  B00001000,
  B00010000,
  B00100000,
  B01111100
},{
  B00111000,
  B01000100,
  B00000100,
  B00011000,
  B00000100,
  B00000100,
  B01000100,
  B00111000
},{
  B00000100,
  B00001100,
  B00010100,
  B00100100,
  B01000100,
  B01111100,
  B00000100,
  B00000100
},{
  B01111100,
  B01000000,
  B01000000,
  B01111000,
  B00000100,
  B00000100,
  B01000100,
  B00111000
},{
  B00111000,
  B01000100,
  B01000000,
  B01111000,
  B01000100,
  B01000100,
  B01000100,
  B00111000
},{
  B01111100,
  B00000100,
  B00000100,
  B00001000,
  B00010000,
  B00100000,
  B00100000,
  B00100000
},{
  B00111000,
  B01000100,
  B01000100,
  B00111000,
  B01000100,
  B01000100,
  B01000100,
  B00111000
},{
  B00111000,
  B01000100,
  B01000100,
  B01000100,
  B00111100,
  B00000100,
  B01000100,
  B00111000
}};

const byte CLEAR_LED_MATRIX[] = {
  B00000000,
  B00000000,
  B00000000,
  B00000000,
  B00000000,
  B00000000,
  B00000000,
  B00000000
};

//loop variables
int i = 0;
int j = 0;
int k = 0;
int l = 1;

//random number generator variables
long randalf = 0;                    //global random number and seedvalue
const long MULTIPLIER = 1103515245;  //c standard value
const int INCREMENT = 12345;         //stepwidth
const uint8_t MAX_VALUE = 10;        //maximal random value

//global display variable
uint8_t lcd_status = LCD_WELCOME;

//global game and player vatiables
const uint8_t STARTING_LIFES = 3;
const uint8_t MAX_PLAYERS = 9;
const uint8_t ILLEGAL_INPUT_VALUE = 12;   //is used for the case, that the non-turn-player presses a button
const unsigned int GAMESPEED = 3000;      //not used yet
unsigned int player_life_turns[MAX_PLAYERS][STARTING_LIFES];   //  life = [x][1]  turn = [x][2]
unsigned int simon_said[50];                                   //keeps track of the specific number-sequence
uint8_t player_number;
uint8_t player_alive;
uint8_t turn_player = 0;   // turn player
uint8_t global_turns = 0;  //global turns
uint8_t number_count = 0;  //counting said numbers
uint8_t input = 0;

//IrRemote
IRrecv irrecv(IR_PIN);
decode_results results;
int refresh = 0;

//LedMatrix
LedControl lc = LedControl(DIN_PIN, CLK_PIN, CS_PIN, 1); // Pins: DIN,CLK,CS, # of Display connected

//LcdDisplay
LiquidCrystal_I2C lcd(0x27, LCD_DISPLAY_COLUMNS, LCD_DISPLAY_ROWS);

//global timer variables
uint8_t timer1_toggle = 0;
long timer1_cnt = 0;
long timer1_old_cnt = 0;

//EEPROM variables
unsigned int address = 0; //adress 0 = version, 1 = flagscore, 2 = higscore, 3 = playermode, 4 = maxrounds
byte rom_version = B00000; //Version 0
byte value;

void setLedMatrix(const uint8_t image[])
{
  for (uint8_t i = 0; i < 8; i++)
  {
    lc.setRow(0, i, image[i]);
  }
}

void setup() {
  pinMode(BUTTON_PIN, INPUT);   //is not necessary
  pinMode(BUTTON_PIN, INPUT_PULLUP);
  lcd.begin();
  lcd.backlight();
  Serial.begin(9600);    //starting serial connection for IR,Debugging etc.
  irrecv.enableIRIn();   // Start the receiver
  pinMode(LED_PIN, OUTPUT);
  pinMode(R_PIN, OUTPUT);
  pinMode(G_PIN, OUTPUT);
  pinMode(B_PIN, OUTPUT);
  timer_setup();
  get_version();
  //rom_reset();       //if a corrupted version is found or highscores should be reseted

  wdt_disable();       //disable the watchdog
  wdt_enable(WDTO_8S); //enable watchdog = 20s
  lc.shutdown(0, false); // turn off power saving, enables display
  lc.setIntensity(0, LC_BRIGHTNESS); // sets brightness (0~15 possible values)
  lc.clearDisplay(0);// clear screen
}


void timer_setup()
{
  cli();           //disallow interrupts

  TCCR1A = 0;      // set Timer1 Registers to 0
  TCCR1B = 0;
  TCNT1  = 0;      //initialize counter value to 0
  // compare register set to 20Hz values
  OCR1A = 12500;   // = (16000000)/(12500*64) = 20Hz (must be <65536)
  // turn on CTC mode --> no process point evaluation necessary anymore
  TCCR1B |= (1 << WGM12);
  // Set CS12 bits for 64-bit prescaler
  TCCR1B |= (1 << CS11) | (1 << CS10);
  // enable timer compare interrupt
  TIMSK1 |= (1 << OCIE1A);

  sei();           //allow interrupts
}


ISR(TIMER1_COMPA_vect)  //interupt sercvice routine for timer 1
{

  timer1_cnt++;

  if (timer1_toggle == 1)
  {
    digitalWrite(LED_PIN, HIGH); //shows that device is on
    timer1_toggle = 0;
  }
  else if (timer1_toggle == 0)
  {
    digitalWrite(LED_PIN, LOW);
    timer1_toggle = 1;
  }
}

void get_version()
{
  value = EEPROM.read(address);
  if (rom_version != value)
  {
    lcd.print("EEPROM Version");
    lcd.setCursor(0, 1);
    lcd.print("Corrupted!");
    delay(LCD_SLOW_ANIMATION * 10);
    lcd.clear();
    lcd.print("Please Update");
    lcd.setCursor(0, 1);
    lcd.print("to Version:");
    lcd.print(value, DEC);
    delay(LCD_SLOW_ANIMATION * 10);
    soft_reset();
  }
  /***  Can be used for automted changing second screen after softreboot.
        But its not good for the ROM-lifecycle...

       else
      {
        address++;
        value = EEPROM.read(address);   //changing between newgame/highscore screen
        if (value = 0)
        {
            EEPROM.write(address, 1);
        }
        else EEPROM.write(address, 0);
      }

  ***/
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

void randomer()     //linear conguential generator , generating pseudorandom numbers within the limits [0;9]
{
  randalf = abs((MULTIPLIER * randalf + INCREMENT ) % MAX_VALUE);
}

void get_highscore()
{

  lcd.clear();
  address = 2;
  value = EEPROM.read(address);   //reads higscore from eeprom as byte
  lcd.print("Highscore");
  lcd.setCursor(0, 1);
  lcd.print("BEST:");
  lcd.print(value, DEC);          //prints highscore in decimal value
  delay(LCD_SLOW_ANIMATION * 10);

  address++;
  value = EEPROM.read(address);   //reads playermode
  Serial.print(address);
  lcd.clear();
  lcd.print("Max Rounds");
  lcd.setCursor(0, 1);
  lcd.print(value, DEC);
  lcd.print("-Player:");
  address++;
  value = EEPROM.read(address);   //reads max rounds played
  lcd.print(value, DEC);
  delay(LCD_SLOW_ANIMATION * 10);

  if (address == 5)
  {
    address = 1;
  }
  soft_reset();
}

void set_highscore()
{
  address = 2;
  value = EEPROM.read(address);
  if (player_life_turns[turn_player][2] >= value)
  {
    lcd.clear();
    lcd.setCursor(0, 0);
    lcd.print("NEW HIGHSCORE!");
    lcd.setCursor(0, 1);
    lcd.print(player_life_turns[turn_player][2]);
    lcd.print(" (old:");
    lcd.print(value, DEC);
    lcd.print(")");
    delay(LCD_SLOW_ANIMATION * 10);
    value = (uint8_t) player_life_turns[turn_player][2];  //sets new higscore using cast-function
    EEPROM.write(address, value);
  }
  address = 4;
  value = EEPROM.read(address);
  if (global_turns >= value)
  {
    lcd.clear();
    lcd.setCursor(0, 0);
    lcd.print("LONGESTGAME!");
    lcd.setCursor(0, 1);
    lcd.print(global_turns);
    lcd.print(" Rounds");
    delay(LCD_SLOW_ANIMATION * 10);
    value = global_turns;
    EEPROM.write(address, value);    //sets new max rounds
    address--;                       //get the playermode
    value = player_number;
    EEPROM.write(address, value);    //sets playermode
  }
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
    while (i <= LCD_DISPLAY_COLUMNS + LCD_ANIMATION_OFFSET)
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

      if (j > 3 && j < 9 && k >= 1) //prints Simon
      {
        lcd.setCursor(j - 1, 0);
        lcd.printByte(l);
        l++;
      }

      if (k >= 2)                   //prints Says
      {
        switch (j)
        {
          case 10: lcd.setCursor(j - 1, 0);
            lcd.printByte(1);
            break;
          case 11: lcd.setCursor(j - 1, 0);
            lcd.printByte(6);
            break;
          case 12: lcd.setCursor(j - 1, 0);
            lcd.printByte(7);
            break;
          case 13: lcd.setCursor(j - 1, 0);
            lcd.printByte(1);
            break;
          default: break;
        }
      }

      if (j > LCD_ANIMATION_OFFSET && k == 3)      //prints version
      {
        lcd.setCursor(j - 7, 1);
        lcd.print(" V.1.3 ");
      }
      else;

      delay(LCD_FAST_ANIMATION);
    }

    i = 0;
    j = 0;
    l = 1;
    k++;
  }
}

void lcd_new_game()
{
  lcd.createChar(0, block);
  i = LCD_DISPLAY_COLUMNS + LCD_ANIMATION_OFFSET;
  j = LCD_DISPLAY_COLUMNS;
  k = 0;

  while (i >= -LCD_ANIMATION_OFFSET)    //prints moving Blocks
  {
    lcd.setCursor(i, 0);
    lcd.write(0);
    lcd.setCursor(i, 1);
    lcd.printByte(0);

    i--;

    if (i <= LCD_DISPLAY_COLUMNS - LCD_ANIMATION_OFFSET)
    {
      lcd.setCursor(j, 0);
      lcd.print(" ");
      lcd.setCursor(j, 1);
      lcd.print(" ");
      j--;
    }

    if (j <= 12)
    {
      lcd.setCursor(j + 1, 0);
      lcd.print("    NEW GAME ");
    }
    delay(LCD_FAST_ANIMATION);
  }

  while (lcd_status == LCD_NEW_GAME)
  {
    lcd.setCursor(1, 1);
    lcd.print("Press A Button");
    if (irrecv.decode(&results))
    {
      set_input();
      if (input == 11)
      {
        lcd_status = LCD_GET_HIGHSCORE;
      }
      else
      {
        lcd_status = LCD_PLAYER_SELECT;
        input = ILLEGAL_INPUT_VALUE;            //seting input to illegal value
      }

      randalf = micros();    //initializing randalf with seed value (~ 110.000.000)
      irrecv.resume();       // ready to receive the next value from IR-Control
    }
    // Watchdog triggers here after ~3 sec inactivity --> lcd_welcome screen
  }
}

void lcd_player_select()
{
  wdt_reset();

  while (lcd_status == LCD_PLAYER_SELECT)
  {
    lcd.setCursor(0, 0);
    lcd.print("Please select");
    lcd.setCursor(0, 1);
    lcd.print("Players (1-9)");
    delay(LCD_NO_ANIMATION);

    if (irrecv.decode(&results))
    {
      set_input();
      irrecv.resume();            // ready to receive the next value from IR-Control
    }

    if (input >= 1 && input <= 9)
    {
      player_number = input;      //setting up players and lifepoints
      player_alive = player_number;
      for (i = 0; i <= input; i++)
      {
        player_life_turns[i][1] = STARTING_LIFES;
      }
      lcd.clear();
      lcd.setCursor(2, 0);
      lcd.print("You Choose:");
      lcd.setCursor(0, 1);

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
      delay(LCD_NO_ANIMATION * 3);
      input = ILLEGAL_INPUT_VALUE;            //seting input to illegal value
      lcd_status = LCD_PLAYER_TURN;
    }

  }
}

void lcd_player_turn()
{
  lcd.createChar(0, skull);
  lcd.createChar(1, heart);
  lcd.clear();
  timer1_cnt = 0;                    //reseting timer
  while (1)                          //the main game loop
  {
    rgb_color(255, 0, 255);          // purple
    wdt_disable();                   //turning of watchdog while game is in progress

    lcd.setCursor(0, 0);
    switch (player_life_turns[turn_player][1])  //prints lives/deaths of turn player
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
      case (0): lcd.print("Player:");
        lcd.print(turn_player + 1);
        lcd.print("DEAD:");
        lcd.print(player_life_turns[turn_player][2]);
        delay(LCD_SLOW_ANIMATION * 5); // Marker für maddin
        break;
      default: lcd.print("Player:");
        lcd.print(turn_player + 1);
        lcd.print("Turn:");
        lcd.print(global_turns + 1);
        delay(LCD_SLOW_ANIMATION * 20); // Marker für maddin
        game();
        player_input();
        break;
    }

    turn_player++;

    if (turn_player == player_number)
    {
      turn_player = 0;
      global_turns++;
      randomer();
      number_count++;
      simon_said[number_count] = randalf;
    }
  }
  lcd.setCursor(0, 0);
  lcd.print("TIMER DOWN");
}

void game()
{
  if (global_turns == 0 && turn_player == 0)         //first 3 numbers for the first turn
  {
    while (number_count < 3)
    {
      randomer();
      simon_said[number_count] = randalf;
      number_count++;
    }
  }

  lcd.clear();
  for (int x = 0; x < number_count; x++)
  {
    lcd.setCursor(4, 0);
    lcd.print(simon_said[x]);
    
    setLedMatrix(CLEAR_LED_MATRIX);
    delay(LC_DIGIT_HIDE);
    setLedMatrix(DIGITS[simon_said[x]]);
    delay(LC_DIGIT_SHOW);
    
    delay(LCD_NO_ANIMATION);   //speedvalue / number of chars +- some modifier maybe
  }
  lcd.setCursor(0, 1);
  lcd.print("go! go! go!");
  lc.clearDisplay(0);
  delay(LCD_NO_ANIMATION);
}


void player_input()
{
  int input_count = 0;

  while (input_count < number_count)
  {
    lcd.setCursor(0, 0);
    lcd.clear();
    if (irrecv.decode(&results) && input == ILLEGAL_INPUT_VALUE)
    {
      set_input();
      if (simon_said[input_count] == input)
      {
        lcd.print("right!!!");
        lcd.setCursor(1, 1);
        lcd.print("right!!!");
        rgb_color(0, 255, 0);                   // green
        delay(LCD_SLOW_ANIMATION);
        input_count++;
        player_life_turns[turn_player][2]++;    //correct entries counted for highscore

        if (player_number > 1 && player_alive == 1) lcd_winning();
      }
      else
      {
        lcd.print("wrong!!!");
        lcd.setCursor(1, 1);
        lcd.print("wrong!!!");
        rgb_color(255, 0, 0);                 // red
        delay(LCD_SLOW_ANIMATION);
        player_life_turns[turn_player][1]--;  //wrong entry = lesser life
        input_count = number_count;           //wrong entry = turn over
        if (player_life_turns[turn_player][1] == 0) lcd_game_over();
      }
      lcd.clear();
      irrecv.resume();                        // ready to receive the next value from IR-Control
      input = ILLEGAL_INPUT_VALUE;
    }
  }
}

void lcd_game_over()
{
  rgb_color(255, 255, 0);     // yellow
  k = 0;
  lcd.createChar(0, skull);

  while (k <= 1)
  {
    k++;
    i = 0;
    j = 0;

    while (i <= LCD_DISPLAY_COLUMNS + LCD_ANIMATION_OFFSET)
    {
      lcd.setCursor(i, 0);    //prints moving skulls
      lcd.write(0);
      i++;

      if (i >= LCD_ANIMATION_OFFSET)
      {
        lcd.setCursor(j, 0);
        lcd.print(" ");
        j++;
      }
      delay(LCD_FAST_ANIMATION * 2);
    }

    while (i > -LCD_ANIMATION_OFFSET + 1)
    {
      lcd.setCursor(i, 1);
      lcd.write(0);
      i--;

      if (i <= LCD_DISPLAY_COLUMNS - LCD_ANIMATION_OFFSET + 1)
      {
        lcd.setCursor(j, 1);
        lcd.print(" ");
        j--;
      }
      delay(LCD_FAST_ANIMATION * 2);
    }
    
    lcd.setCursor(0, 0);
    lcd.clear();
    lcd.print("    G A M E ");
    lcd.setCursor(0, 1);
    lcd.print("    O V E R ");
    delay(LCD_SLOW_ANIMATION * 10);
  }

  player_alive--;                        //one player died
  wdt_enable(WDTO_8S);                   //reactivate watchdog to reset game

  if (player_number == 1)
  {
    set_highscore();
    soft_reset();
  }
  if (player_number - player_alive == 0)
  {
    set_highscore();
    soft_reset();
  }
}


void lcd_winning()
{
  rgb_color(0, 255, 255);    // turquoise
  i = 0;
  j = 0;
  k = 0;
  lcd.createChar(0, crown);
  lcd.createChar(1, heart);
  lcd.createChar(2, star);
  
  while (k < 20)
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
    lcd.setCursor(13, 1);
    lcd.write(i);
    lcd.write(i);
    lcd.write(i);
    delay(LCD_SLOW_ANIMATION * 5);
    if (i == 2) i = i - 3;
    i++;
    j++;
    k++;
  }
  wdt_enable(WDTO_8S);              //reactivate watchdog to reset game
  if (player_number - player_alive == 1)
  {
    set_highscore();
    soft_reset;
  }
}


void soft_reset()
{
  delay(8001);     //if delay is greater then 8seconds , immidiate watchdog reset
}

void set_input()   //assigns the value of pressed button to input
{
  switch (results.value)
  {
    case 0xFFC23D: input = 11; // PLAY/PAUSE button
      break;
    case 0xFF6897: input = 0;
      break;
    case 0xFF30CF: input = 1;
      break;
    case 0xFF18E7: input = 2;
      break;
    case 0xFF7A85: input = 3;
      break;
    case 0xFF10EF: input = 4;
      break;
    case 0xFF38C7: input = 5;
      break;
    case 0xFF5AA5: input = 6;
      break;
    case 0xFF42BD: input = 7;
      break;
    case 0xFF4AB5: input = 8;
      break;
    case 0xFF52AD: input = 9;
      break;
    case 0xFFFFFF: break;   //if button is pressed continuously
    default:  break;
  }
}


void lcd_refresh()
{
  lcd.clear();
  lcd.setCursor(0, 0);
  switch (lcd_status)
  {
    case LCD_WELCOME:
      rgb_color(0, 0, 255);   // blue
      lcd_welcome();
      break;
    case LCD_NEW_GAME:
      lcd_new_game();
      break;
    case LCD_PLAYER_SELECT:
      lcd_player_select();
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
    //case LCD_IR_TEST:       part of unit test
    //  ir_test();
    //  break;
    case LCD_GET_HIGHSCORE:
      get_highscore();
      break;
    default:
      lcd.print("No Valid Input");
      lcd.setCursor(0, 1);
      lcd.print("Try again!!!");
      delay(LCD_NO_ANIMATION);
      break;
  }
}


void loop()
{
  wdt_reset();  //resetting watchdog
  lcd_refresh();

  if (lcd_status == LCD_WELCOME)
  {
    lcd_status = LCD_NEW_GAME;
  }
}
