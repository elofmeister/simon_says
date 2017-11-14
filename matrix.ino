/*
   LED - Matrix pin configuration
*/

//#define DEBUG_MODE 1

#define ROW_1 10
#define ROW_2 7
#define ROW_3 12
#define ROW_4 6
#define ROW_5 2
#define ROW_6 13
#define ROW_7 A1
#define ROW_8 5

#define COL_1 A0
#define COL_2 8
#define COL_3 9
#define COL_4 4
#define COL_5 11
#define COL_6 3
#define COL_7 A2
#define COL_8 A3

const byte rows[] = {
  ROW_1, ROW_2, ROW_3, ROW_4, ROW_5, ROW_6, ROW_7, ROW_8
};

const byte columns[] = {
  COL_1, COL_2, COL_3, COL_4, COL_5, COL_6, COL_7, COL_8
};

#ifdef DEBUG_MODE
const byte bytedef_topleft[] = {
  B10000000,
  B00000000,
  B00000000,
  B00000000,
  B00000000,
  B00000000,
  B00000000,
  B00000000
};
const byte bytedef_topright[] = {
  B00000001,
  B00000000,
  B00000000,
  B00000000,
  B00000000,
  B00000000,
  B00000000,
  B00000000
};
const byte bytedef_bottomleft[] = {
  B00000000,
  B00000000,
  B00000000,
  B00000000,
  B00000000,
  B00000000,
  B00000000,
  B10000000
};
const byte bytedef_bottomright[] = {
  B00000000,
  B00000000,
  B00000000,
  B00000000,
  B00000000,
  B00000000,
  B00000000,
  B00000001
};
const byte bytedef_all[] = {
  B11111111,
  B11111111,
  B11111111,
  B11111111,
  B11111111,
  B11111111,
  B11111111,
  B11111111
};
#endif

const byte numbers[][8] = {
  {
    B00111000,
    B01000100,
    B01000100,
    B01000100,
    B01000100,
    B01000100,
    B01000100,
    B00111000
  },
  {
    B00010000,
    B00110000,
    B00010000,
    B00010000,
    B00010000,
    B00010000,
    B00010000,
    B00111000
  }, {
    B00111000,
    B01000100,
    B00000100,
    B00000100,
    B00001000,
    B00010000,
    B00100000,
    B01111100
  }, {
    B00111000,
    B01000100,
    B00000100,
    B00011000,
    B00000100,
    B00000100,
    B01000100,
    B00111000
  }, {
    B00000100,
    B00001100,
    B00010100,
    B00100100,
    B01000100,
    B01111100,
    B00000100,
    B00000100
  }, {
    B01111100,
    B01000000,
    B01000000,
    B01111000,
    B00000100,
    B00000100,
    B01000100,
    B00111000
  }, {
    B00111000,
    B01000100,
    B01000000,
    B01111000,
    B01000100,
    B01000100,
    B01000100,
    B00111000
  }, {
    B01111100,
    B00000100,
    B00000100,
    B00001000,
    B00010000,
    B00100000,
    B00100000,
    B00100000
  }, {
    B00111000,
    B01000100,
    B01000100,
    B00111000,
    B01000100,
    B01000100,
    B01000100,
    B00111000
  }, {
    B00111000,
    B01000100,
    B01000100,
    B01000100,
    B00111100,
    B00000100,
    B01000100,
    B00111000
  }
};

const byte bytedef_clear[] = {
  B00000000,
  B00000000,
  B00000000,
  B00000000,
  B00000000,
  B00000000,
  B00000000,
  B00000000
};

uint16_t timerCnt = 0;

void setup() {
  // Open serial port
  Serial.begin(9600);

  // Set all used pins to OUTPUT
  // This is very important! If the pins are set to input
  // the display will be very dim.
  for (uint8_t i = 0; i < 8; i++)
  {
    pinMode(rows[i], OUTPUT);
    pinMode(columns[i], OUTPUT);
  }
}

void loop() {
#ifdef DEBUG_MODE
  // This could be rewritten to not use a delay, which would make it appear brighter
  delay(5);
  timerCnt++;
  if (timerCnt <  70) {
    drawScreen(bytedef_topleft);
  }
  else if (timerCnt <  150)
  {
    drawScreen(bytedef_topright);
  }
  else if (timerCnt <  270)
  {
    drawScreen(bytedef_bottomleft);
  }
  else if (timerCnt <  350)
  {
    drawScreen(bytedef_bottomright);
  }
  else if (timerCnt <  420)
  {
    drawScreen(bytedef_all);
  }
  else if (timerCnt <  720)
  {
    timerCnt = 0;
  }
#else
  delay(5);
  timerCnt++;
  if (timerCnt / 100 + 1 > sizeof(numbers) / sizeof(numbers[0])) {    // 100 * 5ms = 500ms .. display time per number
    timerCnt = 0;
  }
  if (timerCnt % 100) {
    drawScreen(numbers[timerCnt / 100]);
  }
  else {
    drawScreen(bytedef_clear);
    delay(50);  // 50ms .. gaps between animations
  }  
#endif


}

void  drawScreen(const byte buf[]) {


  // Turn on each row in series
  for (byte i = 0; i < 8; i++) {
    setColumns(buf[i]); // Set columns for this specific row

    digitalWrite(rows[i], HIGH);
    delay(1); // Set this to 50 or 100 if you want to see the multiplexing effect!
    digitalWrite(rows[i], LOW);

  }
}


void setColumns(const byte b) {
  digitalWrite(COL_1, (~b >> 0) & 0x01); // Get the 1st bit: 10000000
  digitalWrite(COL_2, (~b >> 1) & 0x01); // Get the 2nd bit: 01000000
  digitalWrite(COL_3, (~b >> 2) & 0x01); // Get the 3rd bit: 00100000
  digitalWrite(COL_4, (~b >> 3) & 0x01); // Get the 4th bit: 00010000
  digitalWrite(COL_5, (~b >> 4) & 0x01); // Get the 5th bit: 00001000
  digitalWrite(COL_6, (~b >> 5) & 0x01); // Get the 6th bit: 00000100
  digitalWrite(COL_7, (~b >> 6) & 0x01); // Get the 7th bit: 00000010
  digitalWrite(COL_8, (~b >> 7) & 0x01); // Get the 8th bit: 00000001

}

