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

// The display buffer

byte linksoben[] = {B10000000, B00000000, B00000000, B00000000, B00000000, B00000000, B00000000, B00000000};
byte rechtsoben[] = {B00000001, B00000000, B00000000, B00000000, B00000000, B00000000, B00000000, B00000000};
byte linksunten[] = {B00000000, B00000000, B00000000, B00000000, B00000000, B00000000, B00000000, B10000000};
byte rechtsunten[] = {B00000000, B00000000, B00000000, B00000000, B00000000, B00000000, B00000000, B00000001};
byte alles[] = {B11111111,B11111111,B11111111,B11111111,B11111111,B11111111,B11111111,B11111111};


float timeCount = 0;

void setup() {
    // Open serial port
    Serial.begin(9600);
    
    // Set all used pins to OUTPUT
    // This is very important! If the pins are set to input
    // the display will be very dim.
    for (byte i = 2; i <= 13; i++)
    {
    pinMode(i, OUTPUT);
    }
    pinMode(A0, OUTPUT);
    pinMode(A1, OUTPUT);
    pinMode(A2, OUTPUT);
    pinMode(A3, OUTPUT);
}

void loop() {
// This could be rewritten to not use a delay, which would make it appear brighter
delay(5);
timeCount += 1;
if(timeCount <  70) {
drawScreen(linksoben);
} 
else if (timeCount <  150) 
{
drawScreen(rechtsoben);
}
else if (timeCount <  270) 
{
drawScreen(linksunten);
} 
else if (timeCount <  350) 
{
drawScreen(rechtsunten);
}
else if (timeCount <  420) 
{
drawScreen(alles);
}
else if (timeCount <  720) 
{
  timeCount = 0;
}


}

 void  drawScreen(byte buffer2[]){
     
    
   // Turn on each row in series
   for (byte i = 0; i < 8; i++) {
        setColumns(buffer2[i]); // Set columns for this specific row
        
        digitalWrite(rows[i], HIGH);
        delay(1); // Set this to 50 or 100 if you want to see the multiplexing effect!
        digitalWrite(rows[i], LOW);
        
    }
}


void setColumns(byte b) {
    digitalWrite(COL_1, (~b >> 0) & 0x01); // Get the 1st bit: 10000000
    digitalWrite(COL_2, (~b >> 1) & 0x01); // Get the 2nd bit: 01000000
    digitalWrite(COL_3, (~b >> 2) & 0x01); // Get the 3rd bit: 00100000
    digitalWrite(COL_4, (~b >> 3) & 0x01); // Get the 4th bit: 00010000
    digitalWrite(COL_5, (~b >> 4) & 0x01); // Get the 5th bit: 00001000
    digitalWrite(COL_6, (~b >> 5) & 0x01); // Get the 6th bit: 00000100
    digitalWrite(COL_7, (~b >> 6) & 0x01); // Get the 7th bit: 00000010
    digitalWrite(COL_8, (~b >> 7) & 0x01); // Get the 8th bit: 00000001

}

