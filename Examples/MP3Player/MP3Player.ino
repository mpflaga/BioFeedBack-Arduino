/*
MPF
 */

#include <SPI.h>

//Add the SdFat Libraries
#include <digitalWriteFast.h>
#include <SdFat.h>
#include <BioFeedBack.h>
#include <math.h>
#include <Bounce.h>
#include <EEPROM.h>

Bounce b_ch1       = Bounce( B_CH1      , BUTTON_DEBOUNCE_PERIOD ); 
Bounce b_dwn       = Bounce( B_DWN      , BUTTON_DEBOUNCE_PERIOD ); 
Bounce b_up        = Bounce( B_UP       , BUTTON_DEBOUNCE_PERIOD ); 
Bounce b_cntr      = Bounce( B_CNTR     , BUTTON_DEBOUNCE_PERIOD ); 
Bounce b_thr       = Bounce( B_THR      , BUTTON_DEBOUNCE_PERIOD ); 
Bounce b_ch2       = Bounce( B_CH2      , BUTTON_DEBOUNCE_PERIOD ); 
Bounce b_audio     = Bounce( B_AUDIO    , BUTTON_DEBOUNCE_PERIOD ); 
Bounce b_disp      = Bounce( B_DISP     , BUTTON_DEBOUNCE_PERIOD ); 
Bounce b_onoff_sns = Bounce( B_ONOFF_SNS, BUTTON_DEBOUNCE_PERIOD ); 

PreOperatingSelfTest PreOperatingSelfTest;
Sensor Sensor;
DigitalPOT	DigitalPOT;
DigitalPGA	DigitalPGA;
HW_configuration	HW_configuration;
Mp3	Mp3;
StoreStruct_t settings = {
  // The default values
  220, 1884,
  'c',
  10000,
  {4.5, 5.5, 7, 8.5, 10, 12},
  CONFIG_VERSION
};

//This is the name of the file on the microSD card you would like to play
//Stick with normal 8.3 nomeclature. All lower-case works well.
//Note: you must name the tracks on the SD card with 001, 002, 003, etc. 
//For example, the code is expecting to play 'track002.mp3', not track2.mp3.
char trackName[] = "track001.mp3";
int trackNumber = 1;

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
	loadConfig();
	
//  for (int i = 0; i < 512; i++)
//    EEPROM.write(i, i);	
	
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
  	
  	int i = settings.c - 'a';
  	
		if (b_cntr.update()) {
			if (b_cntr.fallingEdge())	{
				Serial.println("b_cntr pressed");
				Serial.print("Battery Voltage = ");
				Serial.print(Sensor.GetBatteryVoltage(ANA_BATTERY), 2); // two decimal places
				Serial.println(" volts");
				
				settings.c = 'a';
				saveConfig();
				
			}	
		}	
//		if (b_onoff_sns.update()) {
//			if (b_onoff_sns.fallingEdge())	{
//				Serial.println("b_onoff_sns pressed");
//				digitalWriteFast(P_ONOFF_CTRL, LOW); // turn off
//			}	
//		}	
		if (digitalReadFast(B_ONOFF_SNS) == LOW)	{
			digitalWriteFast(P_ONOFF_CTRL, LOW); // turn off
		}		
		if (b_ch1.update()) {
			if (b_ch1.fallingEdge())	{
				Serial.println("b_ch1 pressed");
			}	
		}	
		if (b_dwn.update()) {
			if (b_dwn.fallingEdge())	{
				Serial.println("b_dwn pressed");
			}	
		}	
		if (b_up.update()) {
			if (b_up.fallingEdge())	{
				Serial.println("b_up pressed");
			}	
		}	
		if (b_thr.update()) {
			if (b_thr.fallingEdge())	{
				Serial.println("b_thr pressed");
//				Serial.println("Printing EEPROM Contents:");
//				for (uint8_t  thisbyte = 0; thisbyte < 20; thisbyte++)  {
//				  Serial.print(thisbyte, DEC);
//				  Serial.print("\t");
//				  Serial.print(EEPROM.read(thisbyte), DEC);
//				  Serial.println();				
//				}
			}	
		}	
		if (b_ch2.update()) {
			if (b_ch2.fallingEdge())	{
				Serial.println("b_ch2 pressed");
			}	
		}	
		if (b_audio.update()) {
			if (b_audio.fallingEdge())	{
				Serial.println("b_audio pressed");
			}	
		}	
		if (b_disp.update()) {
			if (b_disp.fallingEdge())	{
				Serial.println("b_disp pressed");
			}	
		}	

//		// Keyboard Test
//		if ( (digitalRead(B_CH1) == LOW) ||
//		(digitalReadFast(B_DISP) == LOW) ||
//		(digitalReadFast(B_DWN) == LOW) ||
//		(digitalReadFast(B_UP) == LOW) ||
//		(digitalReadFast(B_CH2) == LOW) ||
//		(digitalReadFast(B_AUDIO) == LOW) ||
//		(digitalReadFast(B_CNTR) == LOW) ||
//		(digitalReadFast(B_THR) == LOW) ||
//		(digitalReadFast(B_ONOFF_SNS) == LOW) )
//		{
//			digitalWriteFast(pwm_led_bar[0], LOW);
//		}
//		else {
//			digitalWriteFast(pwm_led_bar[0], HIGH);
//		}
//	
//		for (uint8_t  thisPin = 0; thisPin < button_test_leds_count; thisPin += 2)  {
//			if (digitalReadFast(button_test_leds[thisPin]) == HIGH) {
//				digitalWriteFast(button_test_leds[thisPin+1], HIGH);
//			}
//			else {
//				digitalWriteFast(button_test_leds[thisPin+1], LOW);
//			}
//		}

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

void loadConfig() {
  // To make sure there are settings, and they are YOURS!
  // If nothing is found it will use the default settings.
  if (//EEPROM.read(CONFIG_START + sizeof(settings) - 1) == settings.version_of_program[3] // this is '\0'
      EEPROM.read(CONFIG_START + sizeof(settings) - 2) == settings.version_of_program[2] &&
      EEPROM.read(CONFIG_START + sizeof(settings) - 3) == settings.version_of_program[1] &&
      EEPROM.read(CONFIG_START + sizeof(settings) - 4) == settings.version_of_program[0])
  { // reads settings from EEPROM
  	Serial.println("EEPROM being loaded.");
    for (unsigned int t=0; t<sizeof(settings); t++)
      *((char*)&settings + t) = EEPROM.read(CONFIG_START + t);
  } else {
    // settings aren't valid! will overwrite with default settings
    Serial.println("EEPROM being defaulted.");
    saveConfig();
  }
}

void saveConfig() {
	Serial.println("EEPROM being saved.");
  for (unsigned int t=0; t<sizeof(settings); t++)
  { // writes to EEPROM
    EEPROM.write(CONFIG_START + t, *((char*)&settings + t));
    // and verifies the data
    if (EEPROM.read(CONFIG_START + t) != *((char*)&settings + t))
    {
      // error writing to EEPROM
    }
  }
}