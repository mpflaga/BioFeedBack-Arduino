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
#include <BioFeedBack.h>
#include <math.h>

PreOperatingSelfTest PreOperatingSelfTest;
Sensor Sensor;
DigitalPOT	DigitalPOT;
DigitalPGA	DigitalPGA;
HW_configuration	HW_configuration;

//Create the variables to be used by SdFat Library
//String readString;
Mp3	Mp3;

//This is the name of the file on the microSD card you would like to play
//Stick with normal 8.3 nomeclature. All lower-case works well.
//Note: you must name the tracks on the SD card with 001, 002, 003, etc. 
//For example, the code is expecting to play 'track002.mp3', not track2.mp3.
char trackName[] = "track001.mp3";
int trackNumber = 1;

String inBytestring; 

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

	Mp3.Initialize();
}

void loop(){

	while (trackNumber <= 10) {
	  //Let's play a track of a given number
	  sprintf(trackName, "track%03d.mp3", trackNumber); //Splice the new file number into this file name
	  Mp3.Play(trackName); //Go play trackXXX.mp3
	
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

