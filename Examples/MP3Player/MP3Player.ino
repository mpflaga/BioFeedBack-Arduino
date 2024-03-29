/*
MPF
*/

#include <SPI.h>

#include <digitalWriteFast.h>
#include <SdFat.h>
#include <BioFeedBack.h>
#include <SFEMP3Shield.h>
#include <math.h>
#include <Bounce.h>
#include <EEPROM.h>
#include <rn42.h>

#define SerialX Serial

SdFat sd;
SFEMP3Shield MP3player;
RN42 Bluetooth;

byte temp;
byte result;

char title[30];
char artist[30];
char album[30];

PreOperatingSelfTest PreOperatingSelfTest;
Sensor Sensor;
DigitalPOT	DigitalPOT;
DigitalPGA	DigitalPGA;
HW_configuration	HW_configuration;

void setup() {
	MCUSR = 0;
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

	Serial.begin(57600); //Use serial for debugging
	Serial3.begin(57600); //Use serial for debugging

	PreOperatingSelfTest.post();

	// Report Battery Voltage
	Serial.print("Battery Voltage = ");
	Serial.print(Sensor.GetBatteryVoltage(ANA_BATTERY), 2); // two decimal places
	Serial.println(" volts");

	loadConfig();
	digitalWriteFast(AUDIO_AMP_SHTDWN, HIGH); //Enable Audio Output

	//Initialize the SdCard.
	if(!sd.begin(SD_SEL, SPI_HALF_SPEED)) sd.initErrorHalt();
	if(!sd.chdir("/")) sd.errorHalt("sd.chdir");

	//boot up the MP3 Player Shield
	result = MP3player.begin();
	//check result, see readme for error codes.
	if(result != 0) {
		Serial.print("Error code: ");
		Serial.print(result);
		Serial.println(" when trying to start MP3 player");
	}
	if (MP3player.ADMixerLoad("admxster.053") == 0) {
		MP3player.ADMixerVol(-3);
	}

	Serial.println("Playing First Track");
	MP3player.playTrack(1);
	Serial.println("Send a number 1-9 to play a track or s to stop playing");
	Serial.print("MP3_DREQINT = ");
	Serial.println(MP3_DREQINT, DEC);

	Bluetooth.begin();
	Serial.println("BlueTooth Test, Type anything.");

	HW_configuration.CylonEye();

}

void loop()
{

	//MPF STILL TRYING TO WORK OUT THE BT
	while (Serial3.available()) {
		Serial.write((uint8_t ) Serial3.read());
	}

//	while (Serial.available()) {
//		Serial3.write((uint8_t ) Serial.read());
//	}

	if(Serial.available()){
		temp = Serial.read();
		Serial3.write(temp);

		Serial.print("Received command: ");
		Serial.write(temp);
		Serial.println(" ");

		//if s, stop the current track
		if (temp == 's') {
			MP3player.stopTrack();
		}

		else if (temp >= '1' && temp <= '9'){
			//convert ascii numbers to real numbers
			temp = temp - 48;

			//tell the MP3 Shield to play a track
			result = MP3player.playTrack(temp);

			//check result, see readme for error codes.
			if(result != 0) {
				Serial.print("Error code: ");
				Serial.print(result);
				Serial.println(" when trying to play track");
			}

			Serial.println("Playing:");

			//we can get track info by using the following functions and arguments
			//the functions will extract the requested information, and put it in the array we pass in
			MP3player.trackTitle((char*)&title);
			MP3player.trackArtist((char*)&artist);
			MP3player.trackAlbum((char*)&album);

			//print out the arrays of track information
			Serial.write((byte*)&title, 30);
			Serial.println();
			Serial.print("by:  ");
			Serial.write((byte*)&artist, 30);
			Serial.println();
			Serial.print("Album:  ");
			Serial.write((byte*)&album, 30);
			Serial.println();

		}

		/* Alterativly, you could call a track by it's file name by using playMP3(filename);
		But you must stick to 8.1 filenames, only 8 characters long, and 3 for the extension */

		else if (temp == 'f') {
			//create a string with the filename
			char trackName[] = "carly0.mid";

			//tell the MP3 Shield to play that file
			result = MP3player.playMP3(trackName);

			//check result, see readme for error codes.
			if(result != 0) {
				Serial.print("Error code: ");
				Serial.print(result);
				Serial.println(" when trying to play track");
			}
		}

	}

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

	keypad();
}

void loadConfig() {
	// To make sure there are EEPROM_configuration, and they are YOURS!
	// If nothing is found it will use the default EEPROM_configuration.
	if ( //EEPROM.read(CONFIG_START + sizeof(EEPROM_configuration) - 1) == EEPROM_configuration.version_of_program[3] // this is '\0'
		EEPROM.read(CONFIG_START + sizeof(EEPROM_configuration) - 2) == EEPROM_configuration.version_of_program[2] &&
		EEPROM.read(CONFIG_START + sizeof(EEPROM_configuration) - 3) == EEPROM_configuration.version_of_program[1] &&
		EEPROM.read(CONFIG_START + sizeof(EEPROM_configuration) - 4) == EEPROM_configuration.version_of_program[0]) { // reads EEPROM_configuration from EEPROM
		Serial.println("EEPROM being loaded.");
		for (unsigned int t=0; t<sizeof(EEPROM_configuration); t++)
		*((char*)&EEPROM_configuration + t) = EEPROM.read(CONFIG_START + t);
	} else {
		// EEPROM_configuration aren't valid! will overwrite with default EEPROM_configuration
		Serial.println("EEPROM being defaulted.");
		saveConfig();
	}

	#ifdef DEBUG
	Serial.print("Volume increamented to -");
	Serial.print((float) EEPROM_configuration.vol_l/2, 1);
	Serial.print("/ -");
	Serial.print((float) EEPROM_configuration.vol_r/2, 1);
	Serial.println("dB");
	#endif
}

void saveConfig() {
	Serial.println("EEPROM being saved.");
	for (unsigned int t=0; t<sizeof(EEPROM_configuration); t++)
	{ // writes to EEPROM
		EEPROM.write(CONFIG_START + t, *((char*)&EEPROM_configuration + t));
		// and verifies the data
		if (EEPROM.read(CONFIG_START + t) != *((char*)&EEPROM_configuration + t)) {
			// error writing to EEPROM
		}
	}
}

Bounce b_ch1       = Bounce( B_CH1      , BUTTON_DEBOUNCE_PERIOD );
Bounce b_dwn       = Bounce( B_DWN      , BUTTON_DEBOUNCE_PERIOD );
Bounce b_up        = Bounce( B_UP       , BUTTON_DEBOUNCE_PERIOD );
Bounce b_cntr      = Bounce( B_CNTR     , BUTTON_DEBOUNCE_PERIOD );
Bounce b_thr       = Bounce( B_THR      , BUTTON_DEBOUNCE_PERIOD );
Bounce b_ch2       = Bounce( B_CH2      , BUTTON_DEBOUNCE_PERIOD );
Bounce b_audio     = Bounce( B_AUDIO    , BUTTON_DEBOUNCE_PERIOD );
Bounce b_disp      = Bounce( B_DISP     , BUTTON_DEBOUNCE_PERIOD );
Bounce b_onoff_sns = Bounce( B_ONOFF_SNS, BUTTON_DEBOUNCE_PERIOD );

void keypad() {
	if (b_cntr.update()) {
		if (b_cntr.fallingEdge())	{
			SerialX.println("b_cntr pressed");
			SerialX.print("Battery Voltage = ");
			SerialX.print(Sensor.GetBatteryVoltage(ANA_BATTERY), 2); // two decimal places
			SerialX.println(" volts");
			//				saveConfig();
			if (Sensor.GetBatteryVoltage(ANA_BATTERY) < 2) {
				HW_configuration.Reset();
			}
		}
	}
	if (b_onoff_sns.update()) {
		if (b_onoff_sns.fallingEdge())	{
			SerialX.println("b_onoff_sns pressed");
			HW_configuration.Reset();
		}
	}
	if (b_ch1.update()) {
		if (b_ch1.fallingEdge())	{
			SerialX.println("b_ch1 pressed");
		}
	}
	if (b_dwn.update()) {
		if (b_dwn.fallingEdge())	{
			SerialX.println("b_dwn pressed");
			EEPROM_configuration.vol_l--;
			if (EEPROM_configuration.vol_l <= 1) {
				EEPROM_configuration.vol_l = 1;
			}
			EEPROM_configuration.vol_r--;
			if (EEPROM_configuration.vol_r <= 1) {
				EEPROM_configuration.vol_r = 1;
			}
			#ifdef DEBUG
			SerialX.print("Volume decreamented to -");
			SerialX.print((float) EEPROM_configuration.vol_l/2, 1);
			SerialX.print("/ -");
			SerialX.print((float) EEPROM_configuration.vol_r/2, 1);
			SerialX.println("dB");
			#endif
			saveConfig();
			MP3player.setVolume(EEPROM_configuration.vol_l, EEPROM_configuration.vol_r);
			MP3player.playMP3("/vs1053/sounds/ding.mp3");
			b_dwn.rebounce(250);
		}
	}
	if (b_up.update()) {
		if (b_up.fallingEdge())	{
			SerialX.println("b_up pressed");
			EEPROM_configuration.vol_l++;
			if (EEPROM_configuration.vol_l >= 254) {
				EEPROM_configuration.vol_l = 254;
			}
			EEPROM_configuration.vol_r++;
			if (EEPROM_configuration.vol_r >= 254) {
				EEPROM_configuration.vol_r = 254;
			}
			#ifdef DEBUG
			SerialX.print("Volume increamented to -");
			SerialX.print((float) EEPROM_configuration.vol_l/2, 1);
			SerialX.print("/ -");
			SerialX.print((float) EEPROM_configuration.vol_r/2, 1);
			SerialX.println("dB");
			#endif
			saveConfig();
			MP3player.setVolume(EEPROM_configuration.vol_l, EEPROM_configuration.vol_r);
			MP3player.playMP3("/vs1053/sounds/ding.mp3");
		}
		b_up.rebounce(250);
	}
	if (b_thr.update()) {
		if (b_thr.fallingEdge())	{
			SerialX.println("b_thr pressed");
			MP3player.playMP3("/vs1053/sounds/womb.mp3");
		}
	}
	if (b_ch2.update()) {
		if (b_ch2.fallingEdge())	{
			SerialX.println("b_ch2 pressed");
		}
	}
	if (b_audio.update()) {
		if (b_audio.fallingEdge())	{
			SerialX.println("b_audio pressed");
		}
	}
	if (b_disp.update()) {
		if (b_disp.fallingEdge())	{
			SerialX.println("b_disp pressed");
		}
	}
}
