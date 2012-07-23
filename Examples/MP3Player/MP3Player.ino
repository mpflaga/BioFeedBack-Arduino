/*
 4-28-2011
 Spark Fun Electronics 2011
 Nathan Seidle
 
 This code is public domain but you buy me a beer if you use this and we meet someday (Beerware license).
 
 This example code plays a MP3 from the SD card called 'track001.mp3'. The theory is that you can load a 
 microSD card up with a bunch of MP3s and then play a given 'track' depending on some sort of input such 
 as which pin is pulled low.
 
 It relies on the sdfatlib from Bill Greiman: 
 http://code.google.com/p/sdfatlib/
 You will need to download and install his library. To compile, you MUST change Sd2PinMap.h of the SDfatlib! 
 The default SS_PIN = 10;. You must change this line under the ATmega328/Arduino area of code to 
 uint8_t const SS_PIN = 9;. This will cause the sdfatlib to use pin 9 as the 'chip select' for the 
 microSD card on pin 9 of the Arduino so that the layout of the shield works.
 
 Attach the shield to an Arduino. Load code (after editing Sd2PinMap.h) then open the terminal at 57600bps. This 
 example shows that it takes ~30ms to load up the VS1053 buffer. We can then do whatever we want for ~100ms 
 before we need to return to filling the buffer (for another 30ms).
 
 This code is heavily based on the example code I wrote to control the MP3 shield found here:
 http://www.sparkfun.com/products/9736
 This example code extends the previous example by reading the MP3 from an SD card and file rather than from internal
 memory of the ATmega. Because the current MP3 shield does not have a microSD socket, you will need to add the microSD 
 shield to your Arduino stack.
 
 The main gotcha from all of this is that you have to make sure your CS pins for each device on an SPI bus is carefully
 declared. For the SS pin (aka CS) on the SD FAT libaray, you need to correctly set it within Sd2PinMap.h. The default 
 pin in Sd2PinMap.h is 10. If you're using the SparkFun microSD shield with the SparkFun MP3 shield, the SD CS pin 
 is pin 9. 
 
 Four pins are needed to control the VS1503:
 DREQ
 CS
 DCS
 Reset (optional but good to have access to)
 Plus the SPI bus
 
 Only the SPI bus pins and another CS pin are needed to control the microSD card.
 
 What surprised me is the fact that with a normal MP3 we can do other things for up to 100ms while the MP3 IC crunches
 through it's fairly large buffer of 2048 bytes. As long as you keep your sensor checks or serial reporting to under 
 100ms and leave ~30ms to then replenish the MP3 buffer, you can do quite a lot while the MP3 is playing glitch free.
 
 */

#include <SPI.h>

//Add the SdFat Libraries
#include <digitalWriteFast.h>
#include <SdFat.h>
#include <SdFatUtil.h> 
#include <BioFeedBack.h>
#include <math.h>

PreOperatingSelfTest PreOperatingSelfTest;
Sensor Sensor;
DigitalPOT	DigitalPOT;
DigitalPGA	DigitalPGA;
HW_configuration	HW_configuration;

//Create the variables to be used by SdFat Library
Sd2Card card;
SdVolume volume;
SdFile root;
SdFile track;
//String readString;
Mp3	Mp3;

//This is the name of the file on the microSD card you would like to play
//Stick with normal 8.3 nomeclature. All lower-case works well.
//Note: you must name the tracks on the SD card with 001, 002, 003, etc. 
//For example, the code is expecting to play 'track002.mp3', not track2.mp3.
char trackName[] = "track001.mp3";
int trackNumber = 1;

char errorMsg[100]; //This is a generic array used for sprintf of error messages
String inBytestring; 

#define TRUE  0
#define FALSE  1

////MP3 Player Shield pin mapping. See the schematic
//#define MP3_XCS 6 //Control Chip Select Pin (for accessing SPI Control/Status registers)
//#define MP3_XDCS 7 //Data Chip Select / BSYNC Pin
//#define MP3_DREQ 2 //Data Request Pin: Player asks for more data
//#define MP3_RESET 8 //Reset is active low
////Remember you have to edit the Sd2PinMap.h of the sdfatlib library to correct control the SD card.
//#define SDCARD_CS   9      //PJ6 Output, Active Low,  Remember you have to edit the Sd2PinMap.h of the sdfatlib library to correct control the SD card.


void setup() {

	HW_configuration.BoardsPinMode();

//#ifdef DEBUG
//	DigitalPOT.SetDigitalPOT(TMPOFFSET_CS, map(25, 0, 100, 0, MCP4013_FULL_SCALE));
//	DigitalPOT.SetDigitalPOT(GSROFFSET_CS, map(75, 0, 100, 0, MCP4013_FULL_SCALE));
//	delay(5000);
//	DigitalPOT.SetDigitalPOT(TMPOFFSET_CS, map(75, 0, 100, 0, MCP4013_FULL_SCALE));
//	DigitalPOT.SetDigitalPOT(GSROFFSET_CS, map(25, 0, 100, 0, MCP4013_FULL_SCALE));
//	delay(5000);
//#endif

	DigitalPOT.SetDigitalPOT(TMPOFFSET_CS, map(50, 0, 100, 0, MCP4013_FULL_SCALE));
	DigitalPOT.SetDigitalPOT(GSROFFSET_CS, map(50, 0, 100, 0, MCP4013_FULL_SCALE));

  Serial.begin(115200); //Use serial for debugging 
  Serial3.begin(115200); //Use serial for debugging 

	PreOperatingSelfTest.post();

	// Report Battery Voltage
	Serial.print("Battery Voltage = ");
	Serial.print(Sensor.GetBatteryVoltage(ANA_BATTERY), 2); // two decimal places
	Serial.println(" volts");

//  pinMode(MP3_DREQ, INPUT);
//  pinMode(MP3_XCS, OUTPUT);
//  pinMode(MP3_XDCS, OUTPUT);
//  pinMode(MP3_RESET, OUTPUT);
//
//  digitalWriteFast(MP3_XCS, HIGH); //Deselect Control
//  digitalWriteFast(MP3_XDCS, HIGH); //Deselect Data
//  digitalWriteFast(MP3_RESET, LOW); //Put VS1053 into hardware reset

  //Setup SD card interface
  pinMode(SS_PIN, OUTPUT);       //Pin 10 must be set as an output for the SD communication to work.
  if (!card.init(SPI_FULL_SPEED, SDCARD_CS))  Serial.println("Error: Card init"); //Initialize the SD card and configure the I/O pins.
  if (!volume.init(&card)) Serial.println("Error: Volume ini"); //Initialize a volume on the SD card.
  if (!root.openRoot(&volume)) Serial.println("Error: Opening root"); //Open the root directory in the volume. 

	Mp3.Initialize();
}

void loop(){

	while (trackNumber <= 10) {
	  //Let's play a track of a given number
	  sprintf(trackName, "track%03d.mp3", trackNumber); //Splice the new file number into this file name
	  playMP3(trackName); //Go play trackXXX.mp3
	
	  //Once we are done playing or have exited the playback for some reason, decide what track to play next
	  trackNumber++; //When we loop, advance to next track!
	}
  Serial.println("MP3 playing has ended, moving onto other stuff");
  Serial.println("");

  Serial.println("Moving on to Keyboard Test");
	Serial.println("UART3 Loop Back Test");
	Serial3.println("BlueTooth Test, Type anything.");
	digitalWriteFast(BT_RST, LOW);   //Take Radio out of Reset
	digitalWriteFast(BT_CTS, LOW);    //Enable Transmitter

	delay(1000);
//	Serial3.print("$$$");
	Serial.print("BT_CD = ");
	Serial.println(digitalReadFast(BT_CD));
//	while (!digitalReadFast(BT_CD));
//	Serial.println("BT Connect Detected.");
//	Serial.print("BT_CD = ");
//	Serial.println(digitalReadFast(BT_CD));
	
//	for (uint8_t  thisGain = 0; thisGain < PGA_Gains_count; thisGain++) {
//		DigitalPGA.WriteRegister(GSRPGA_CS, thisGain);
//		Serial.println("Sensor Voltage = ");
//		for (uint8_t  thisCount = 0; thisCount < 5; thisCount += 1)  {
//			Serial.print(Sensor.GetTMPVoltage(ANA_GSR), 3); // two decimal places
//			Serial.println(" volts");
//			delay(100);
//		}
//		delay(2000);
//	}

  while(1) {
		// Keyboard Test
		if ( (digitalRead(B_CH1) == LOW) ||
		(digitalReadFast(B_DISP) == LOW) ||
		(digitalReadFast(B_DWN) == LOW) ||
		(digitalReadFast(B_UP) == LOW) ||
		(digitalReadFast(B_CH2) == LOW) ||
		(digitalReadFast(B_AUDIO) == LOW) ||
		(digitalReadFast(B_CNTR) == LOW) ||
		(digitalReadFast(B_THR) == LOW) ||
		(digitalReadFast(B_ONOFF_SNS) == LOW) )
		{
			digitalWriteFast(pwm_led_bar[0], LOW);
		}
		else {
			digitalWriteFast(pwm_led_bar[0], HIGH);
		}
	
		for (uint8_t  thisPin = 0; thisPin < button_test_leds_count; thisPin += 2)  {
			if (digitalReadFast(button_test_leds[thisPin]) == HIGH) {
				digitalWriteFast(button_test_leds[thisPin+1], HIGH);
			}
			else {
				digitalWriteFast(button_test_leds[thisPin+1], LOW);
			}
			if (digitalReadFast(B_ONOFF_SNS) == LOW)	{
				digitalWriteFast(P_ONOFF_CTRL, LOW); // turn off
			}		
			if (digitalReadFast(B_CNTR) == LOW)	{
				Serial.print("Battery Voltage = ");
				Serial.print(Sensor.GetBatteryVoltage(ANA_BATTERY), 2); // two decimal places
				Serial.println(" volts");
			}		
		}


		// End Keyboard Test

//MPF STILL TRYING TO WORK OUT THE BT
		while (Serial3.available()) {
		  Serial.write((uint8_t ) Serial3.read()); 
		}	
		while (Serial.available()) {
		  Serial3.write((uint8_t ) Serial.read()); 
		}	
	}
}

//PlayMP3 pulls 32 byte chunks from the SD card and throws them at the VS1053
//We monitor the DREQ (data request pin). If it goes low then we determine if
//we need new data or not. If yes, pull new from SD card. Then throw the data
//at the VS1053 until it is full.
void playMP3(char* fileName) {

  if (!track.open(&root, fileName, O_READ)) { //Open the file in read mode.
    sprintf(errorMsg, "Failed to open %s", fileName);
    Serial.println(errorMsg);
    return;
  }

  Serial.println("Track open");

  uint8_t mp3DataBuffer[32]; //Buffer of 32 bytes. VS1053 can take 32 bytes at a go.
  //track.read(mp3DataBuffer, sizeof(mp3DataBuffer)); //Read the first 32 bytes of the song
  int need_data = TRUE; 
  long replenish_time = millis();

  Serial.println("Start MP3 decoding");

  while(1) {
    while(!digitalRead(MP3_DREQ)) { 
      //DREQ is low while the receive buffer is full
      //You can do something else here, the buffer of the MP3 is full and happy.
      //Maybe set the volume or test to see how much we can delay before we hear audible glitches

      //If the MP3 IC is happy, but we need to read new data from the SD, now is a great time to do so
      if(need_data == TRUE) {
        if(!track.read(mp3DataBuffer, sizeof(mp3DataBuffer))) { //Try reading 32 new bytes of the song
          //Oh no! There is no data left to read!
          //Time to exit
          break;
        }
        need_data = FALSE;
      }

      //Serial.println("."); //Print a character to show we are doing nothing

      //This is here to show how much time is spent transferring new bytes to the VS1053 buffer. Relies on replenish_time below.
      Serial.print("Time to replenish buffer: ");
      Serial.print(millis() - replenish_time, DEC);
      Serial.print("ms");

      //Test to see just how much we can do before the audio starts to glitch
      long start_time = millis();
      //delay(150); //Do NOTHING - audible glitches
      //delay(135); //Do NOTHING - audible glitches
      //delay(120); //Do NOTHING - barely audible glitches
      delay(100); //Do NOTHING - sounds fine
      Serial.print(" Idle time: ");
      Serial.print(millis() - start_time, DEC);
      Serial.println("ms");
      //Look at that! We can actually do quite a lot without the audio glitching

      //Now that we've completely emptied the VS1053 buffer (2048 bytes) let's see how much 
      //time the VS1053 keeps the DREQ line high, indicating it needs to be fed
      replenish_time = millis();
    }


    if(need_data == TRUE){ //This is here in case we haven't had any free time to load new data
      if(!track.read(mp3DataBuffer, sizeof(mp3DataBuffer))) { //Go out to SD card and try reading 32 new bytes of the song
        //Oh no! There is no data left to read!
        //Time to exit
        break;
      }
      need_data = FALSE;
    }

    //Once DREQ is released (high) we now feed 32 bytes of data to the VS1053 from our SD read buffer
    digitalWriteFast(MP3_XDCS, LOW); //Select Data
    for(uint8_t  y = 0 ; y < sizeof(mp3DataBuffer) ; y++) {
      SPI.transfer(mp3DataBuffer[y]); // Send SPI byte
    }

    digitalWriteFast(MP3_XDCS, HIGH); //Deselect Data
    need_data = TRUE; //We've just dumped 32 bytes into VS1053 so our SD read buffer is empty. Set flag so we go get more data
  }

  while(!digitalRead(MP3_DREQ)) ; //Wait for DREQ to go high indicating transfer is complete
  digitalWriteFast(MP3_XDCS, HIGH); //Deselect Data
  
  track.close(); //Close out this track

  sprintf(errorMsg, "Track %s done!", fileName);
  Serial.println(errorMsg);
}