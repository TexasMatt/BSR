#include <ps2.h>
#include <SPI.h>

/*
 * Pin 5 is the mouse data pin, pin 6 is the clock pin
 */
PS2 mouse(6, 5);
/*
 * SPI pins:
 * MOSI - Pin 11
 * SCK  - Pin 13
 */
const int CS_PIN = 9; // POT Chip Select
const int SPI_SS_PIN = 10; 
const float POT_MULTIPLIER = 25.6; // The bigger this number the smaller the stitches will be

/*
 * Define the number of samples to keep track of.  The higher the number,
 * the more the readings will be smoothed, but the slower the output will
 * respond to the input.  Using a constant rather than a normal variable lets
 * use this value to determine the size of the readings array.
*/
const int NUM_READINGS_TO_AVERAGE = 3;

float readings[NUM_READINGS_TO_AVERAGE]; // the readings from the analog input
int readIndex = 0;                       // the index of the current reading
float total = 0;                         // the running total
float average = 0;                       // the average
/*
 * initialize the mouse. Reset it, and place it into remote
 * mode, so we can get the encoder data on demand.
 */
void mouse_init()
{
  mouse.write(0xff);  // reset
  mouse.read();  // ack byte
  mouse.read();  // blank */
  mouse.read();  // blank */
  mouse.write(0xf0);  // remote mode
  mouse.read();  // ack
  delayMicroseconds(100);
}



void setup()
{
  Serial.begin(9600);
  mouse_init();

  // digital POT setup
  SPI.begin();
  pinMode(CS_PIN, OUTPUT);
  pinMode(SPI_SS_PIN, OUTPUT);
  digitalWrite(CS_PIN, LOW);
  SPI.transfer(0);
  SPI.transfer(0);

  // initialize all the readings to 0:
  for (int thisReading = 0; thisReading < NUM_READINGS_TO_AVERAGE; thisReading++) {
    readings[thisReading] = 0;
  }
}

/*
 * get a reading from the mouse and report it back to the
 * host via the serial line.
 */
void loop()
{
  char mstat;
  char mx;
  char my;

  /* get a reading from the mouse */
  mouse.write(0xeb);  // give me data!
  mouse.read();      // ignore ack
  mstat = mouse.read();
  mx = mouse.read();
  my = mouse.read();

  /* calculate distance traveled , pythagoras theorem */
  float distance;
  int potValue;
  int unconstrainedPotValue;
  int numNonZeroReadings;

  distance = sqrt(sq(abs(mx)) + sq(abs(my))); 

  // subtract the last reading:
  total = total - readings[readIndex];
  // read from the sensor:
  readings[readIndex] = distance;
  // add the reading to the total:
  total = total + readings[readIndex];
  // advance to the next position in the array:
  readIndex = readIndex + 1;

  // if we're at the end of the array...
  if (readIndex >= NUM_READINGS_TO_AVERAGE) {
    // ...wrap around to the beginning:
    readIndex = 0;
  }

  numNonZeroReadings = 0;
  for (int i = 0; i < NUM_READINGS_TO_AVERAGE; i++) {
    if (readings[i] > 0) {
      numNonZeroReadings++;
    }
  }
  if (numNonZeroReadings == 0) {
    average = 0;
  } else {
    average = total / numNonZeroReadings;
  }
  unconstrainedPotValue = round(average * POT_MULTIPLIER); // Compare this to the potValue and sound the buzzer if the difference is greater than something.
  potValue = constrain(unconstrainedPotValue, 0, 128);

  SPI.transfer(0);
  SPI.transfer(potValue);

  /* send the data back up */
  /*
  Serial.print(mstat, BIN);
  Serial.print("\tX=");
  Serial.print(mx, DEC);
  Serial.print("\tY=");
  Serial.print(my, DEC);
  */
  Serial.print("\tD=");
  Serial.print(distance, DEC);
  Serial.print("\t#=");
  Serial.print(numNonZeroReadings, DEC);
  Serial.print("\tAVG_D=");
  Serial.print(average, DEC);
  Serial.print("\tPOT=");
  Serial.print(potValue, DEC);
  Serial.println();
  delay(5); // The POT IC need time to settle
}
