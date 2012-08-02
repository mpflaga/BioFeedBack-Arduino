/*
  This library is distributed in the hope that it will be useful,
  but WITHOUT ANY WARRANTY; without even the implied warranty of
  MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the GNU
  Lesser General Public License for more details.

  You should have received a copy of the GNU Lesser General Public
  License along with this library; if not, write to the Free Software
  Foundation, Inc., 51 Franklin St, Fifth Floor, Boston, MA  02110-1301  USA
*/

#include <BioFeedBack.h>
#include <inttypes.h>
#include "Arduino.h"
#include <digitalWriteFast.h>
#include <SPI.h>
#include <SdFat.h>
#include <SdFatUtil.h> 
#include <Bounce.h>

Sd2Card card;
SdVolume volume;
SdFile root;
SdFile track;
volatile StoreStruct_t EEPROM_configuration = {
  // The default values
  40, 40,
  'c',
  10000,
  {4.5, 5.5, 7, 8.5, 10, 12},
  CONFIG_VERSION
};

float fmap(float x, float in_min, float in_max, float out_min, float out_max)
{
 return (x - in_min) * (out_max - out_min) / (in_max - in_min) + out_min;
} 

float Sensor::GetBatteryVoltage(uint8_t channel)
{
	return fmap(analogRead(channel), 0, (2*(BATTERY_CALIBRATION))-1, 0.0, (2*(REGULATOR_VOLTAGE)));
}

float Sensor::GetTMPVoltage(uint8_t channel)
{
	return fmap(analogRead(channel), 0, (2*(BATTERY_CALIBRATION))-1, 0.0, ((REGULATOR_VOLTAGE)));
}

void DigitalPOT::PulseDigitalPOT(uint8_t chipSelectPin, int8_t pulses) {
  pinModeFast(SPI_MOSI, OUTPUT); // Condition the MOSI as Output to U/D pin
	if (pulses > 0) {
		digitalWriteFast(SPI_MOSI, UP); // Prime U/D pin on direction
	} else {
		digitalWriteFast(SPI_MOSI, DOWN);
	}
	delayMicroseconds(1);
	digitalWriteFast(chipSelectPin, LOW); //Enable Command to Target Device
	delayMicroseconds(1);
	for (uint8_t  thisPin = 0; thisPin < abs(pulses); thisPin++)  {
		digitalWriteFast(SPI_MOSI, HIGH); //Pulse Command High
		delayMicroseconds(1);
		digitalWriteFast(SPI_MOSI, LOW);  //Then Low and repeat if needed.
		delayMicroseconds(1);
	}
	digitalWriteFast(chipSelectPin, HIGH); //Negate Command to Device
}

void DigitalPOT::SetDigitalPOT(uint8_t chipSelectPin, int8_t value) {
	// Step 1; Force Digital POT Value to a known value of ZERO.
	PulseDigitalPOT(chipSelectPin, -65); // POT's range is 64
	delayMicroseconds(1);
	// Step 2; Place Digital POT Value to desired Value.
	PulseDigitalPOT(chipSelectPin, value);
}	
void DigitalPGA::WriteRegister(uint8_t chipSelectPin, int8_t Gain) {
//	pinModeFast(chipSelectPin, OUTPUT);
	digitalWriteFast(chipSelectPin, LOW); //Select control
	SPI.transfer(B01000000); //Write instruction byte
	SPI.transfer(Gain); //Write Data byte
	digitalWrite(chipSelectPin, HIGH); //Select control
#ifdef DEBUG
	Serial.print("PGA GAIN = 8");
	Serial.println(Gain, DEC);
#endif
}

void PreOperatingSelfTest::BlinkEachButtonLeds() {
	for (uint8_t  thisPin = 0; thisPin < button_leds_count; thisPin++) {
		// turn the pin on:
		digitalWrite(button_leds[thisPin], LOW);
		delay(POST_TIMER);
		// turn the pin off:
		digitalWrite(button_leds[thisPin], HIGH);
	}
	// loop from the highest pin to the lowest:
	for (int8_t  thisPin = button_leds_count - 1; thisPin >= 0; thisPin--) {
		// turn the pin on:
		digitalWrite(button_leds[thisPin], LOW);
		delay(POST_TIMER);
		// turn the pin off:
		digitalWrite(button_leds[thisPin], HIGH);
	}
}

void PreOperatingSelfTest::CylonEye() {
	// loop from the lowest pin to the highest:
	for (uint8_t  thisPin = 0; thisPin < pwm_led_bar_count; thisPin++) {
		// turn the pin on:
		digitalWrite(pwm_led_bar[thisPin], LOW);
		delay(CYLON_DELAY);
		// turn the pin off:
		digitalWrite(pwm_led_bar[thisPin], HIGH);
	}
}

void PreOperatingSelfTest::FadeLedBar() {
	// fade in from min to max in increments of 5 points:
	for(uint8_t  fadeValue = 255 ; fadeValue >= 200; fadeValue -=5) {
		// sets the value (range from 0 to 255):
		for (uint8_t  thisPin = 0; thisPin < pwm_led_bar_count; thisPin++)  {
			analogWrite(pwm_led_bar[thisPin], fadeValue);
		}
		// wait for 30 milliseconds to see the dimming effect
		delay(FADE_DELAY);
	}
	// fade out from max to min in increments of 5 points:
	for(int  fadeValue = 0 ; fadeValue <= 255; fadeValue +=5) {
		// sets the value (range from 0 to 255):
		for (uint8_t  thisPin = 0; thisPin < pwm_led_bar_count; thisPin++)  {
			analogWrite(pwm_led_bar[thisPin], fadeValue);
		}
		// wait for 30 milliseconds to see the dimming effect
		delay(FADE_DELAY);
	}
}

void PreOperatingSelfTest::BlinkAllButtonLeds() {
	// BLINK all Button LED's at once, twice
	for (uint8_t  thisPin = 0; thisPin < button_leds_count; thisPin++)  {
		digitalWrite(button_leds[thisPin], LOW);
	}
	delay(POST_TIMER);
	for (uint8_t  thisPin = 0; thisPin < button_leds_count; thisPin++)  {
		digitalWrite(button_leds[thisPin], HIGH);
	}
}

void PreOperatingSelfTest::PulseLedBar () {
	for (uint8_t  thisPin = 0; thisPin < pwm_led_bar_count; thisPin++)  {
		digitalWrite(pwm_led_bar[thisPin], LOW);
	}
	delay(POST_TIMER);
	for (uint8_t  thisPin = 0; thisPin < pwm_led_bar_count; thisPin++)  {
		digitalWrite(pwm_led_bar[thisPin], HIGH);
	}
}

void PreOperatingSelfTest::post () {

#ifdef DEBUG
	PulseLedBar();
	delay(POST_TIMER);
	PulseLedBar();
	CylonEye();
#endif

	FadeLedBar();

#ifdef DEBUG
	BlinkAllButtonLeds();
	delay(POST_TIMER);
#endif

	BlinkAllButtonLeds();

#ifdef DEBUG
	BlinkEachButtonLeds();
#endif

} //	PreOperatingSelfTest();


//void PreOperatingSelfTest() {
//
//#ifdef DEBUG
//	irsend.PulseLedBar();
//	delay(POST_TIMER);
//	irsend.PulseLedBar();
//	CylonEye();
//#endif
//
//	FadeLedBar();
//
//#ifdef DEBUG
//	BlinkAllButtonLeds();
//	delay(POST_TIMER);
//#endif
//
//	BlinkAllButtonLeds();
//
//#ifdef DEBUG
//	BlinkEachButtonLeds();
//#endif
//
//	// Report Battery Voltage
//	Serial.print("Battery Voltage = ");
//	Serial.print(GetBatteryVoltage(ANA_BATTERY), 2); // two decimal places
//	Serial.println(" volts");
//
//} // PreOperatingSelfTest()

void HW_configuration::BoardsPinMode () {
	pinMode(LD_DIAG, OUTPUT);
	pinMode(PS_MODE, INPUT);      //Pin #55(PC2) PS_MODE
	pinMode(P_ONOFF_CTRL, OUTPUT);     //Pin #54(PC1) P_ONOFF_CTRL

  pinMode(MP3_XCS, OUTPUT);
  pinMode(MP3_XDCS, OUTPUT);
  pinMode(MP3_DREQ, INPUT);
  pinMode(MP3_RESET, OUTPUT);
  pinMode(SDCARD_CS, OUTPUT);
  
	pinMode(AUDIO_AMP_SHTDWN, OUTPUT);
// May not need to set PinMode of these UART pins - MPF
//	pinMode(MIDI_RX ,OUTPUT );   // Send to RX of MIDI
//	pinMode(MIDI_TX ,INPUT );    // Send from RX of MIDI
	pinMode(BT_RX ,INPUT );     // Send to RX of BlueTooth
	pinMode(BT_TX ,INPUT );      // Send from RX of BlueTooth
	pinMode(BT_CTS ,OUTPUT );
	pinMode(BT_RTS ,INPUT );
	pinMode(BT_CD ,INPUT );
	digitalWrite(BT_CD, LOW);    //Disable internal pull up resistor.
	pinMode(BT_RST ,OUTPUT );
	pinMode(TMPOFFSET_CS ,OUTPUT );
	pinMode(GSROFFSET_CS ,OUTPUT );
	pinMode(TMPPGA_CS ,OUTPUT );
	pinMode(GSRPGA_CS ,OUTPUT );
// May not need to set PinMode of these Analog Input- MPF
//	pinMode(ANA_GSR ,INPUT );
//	pinMode(ANA_TMP ,INPUT );
//	pinMode(ANA_BATTERY ,INPUT );

	// Set the Initial Value of All Input Pins.
	digitalWriteFast(P_ONOFF_CTRL, HIGH);  // set the Power On!
	digitalWriteFast(PS_MODE, HIGH);  // Disable the Power Save Mode
// Should not need to set Initial Value of UARTS - MPF
//  digitalWriteFast(MIDI_RX, HIGH);
//  digitalWriteFast(BT_RX, HIGH);
  digitalWriteFast(AUDIO_AMP_SHTDWN, LOW); //Initially Disable(HIGH) Audio Output
//  digitalWriteFast(BT_CTS, HIGH);   //Initially Disable(HIGH) Radio Transmission
  digitalWriteFast(BT_CTS, LOW);   //Leave Enabled, as to span resets for Debug.
  digitalWriteFast(BT_RST, LOW);   //Initially Hold Radio in Reset
  digitalWriteFast(TMPOFFSET_CS, HIGH); //Initially Deselect DAC
  digitalWriteFast(GSROFFSET_CS, HIGH); //Initially Deselect DAC
  digitalWriteFast(TMPPGA_CS, HIGH);   //Initially Deselect PGA
  digitalWriteFast(GSRPGA_CS, HIGH);   //Initially Deselect PGA

  digitalWriteFast(MP3_XCS, HIGH);   //Initially Deselect Control
  digitalWriteFast(MP3_XDCS, HIGH);  //Initially Deselect Data
  digitalWriteFast(SDCARD_CS, HIGH); //Initially Deselect SD Card
  digitalWriteFast(TMPOFFSET_CS, HIGH);  //Initially Deselect Data
  digitalWriteFast(MP3_RESET, LOW);  //Initially Put VS1053 into hardware reset

	// PinMode Buttons
	for (uint8_t  thisPin = 0; thisPin < button_input_count; thisPin++)  {
		pinMode(button_input[thisPin], OUTPUT);
		digitalWrite(button_input[thisPin], HIGH);
	}

	// PinMode and Clear LEDs
	for (uint8_t  thisPin = 0; thisPin < button_leds_count; thisPin++)  {
		pinMode(button_leds[thisPin], OUTPUT);
		digitalWrite(button_leds[thisPin], HIGH);
	}

	// PinMode and Clear LED Bar
	for (uint8_t  thisPin = 0; thisPin < pwm_led_bar_count; thisPin++)  {
		pinMode(pwm_led_bar[thisPin], OUTPUT);
		digitalWrite(pwm_led_bar[thisPin], HIGH);
	}
}

//Write to VS10xx register
//SCI: Data transfers are always 16bit. When a new SCI operation comes in 
//DREQ goes low. We then have to wait for DREQ to go high again.
//XCS should be low for the full duration of operation.
void Mp3::WriteRegister(unsigned char addressbyte, unsigned char highbyte, unsigned char lowbyte){
  while(!digitalRead(MP3_DREQ)) ; //Wait for DREQ to go high indicating IC is available
  digitalWriteFast(MP3_XCS, LOW); //Select control

  //SCI consists of instruction byte, address byte, and 16-bit data word.
  SPI.transfer(0x02); //Write instruction
  SPI.transfer(addressbyte);
  SPI.transfer(highbyte);
  SPI.transfer(lowbyte);
  while(!digitalRead(MP3_DREQ)) ; //Wait for DREQ to go high indicating command is complete
  digitalWriteFast(MP3_XCS, HIGH); //Deselect Control
}

//Read the 16-bit value of a VS10xx register
unsigned int Mp3::ReadRegister (unsigned char addressbyte){
  while(!digitalRead(MP3_DREQ)) ; //Wait for DREQ to go high indicating IC is available
  digitalWriteFast(MP3_XCS, LOW); //Select control

  //SCI consists of instruction byte, address byte, and 16-bit data word.
  SPI.transfer(0x03);  //Read instruction
  SPI.transfer(addressbyte);

  char response1 = SPI.transfer(0xFF); //Read the first byte
  while(!digitalRead(MP3_DREQ)) ; //Wait for DREQ to go high indicating command is complete
  char response2 = SPI.transfer(0xFF); //Read the second byte
  while(!digitalRead(MP3_DREQ)) ; //Wait for DREQ to go high indicating command is complete

  digitalWriteFast(MP3_XCS, HIGH); //Deselect Control

  int resultvalue = response1 << 8;
  resultvalue |= response2;
  return resultvalue;
}

//Set VS10xx Volume Register
void Mp3::SetVolume(unsigned char leftchannel, unsigned char rightchannel){
  WriteRegister(SCI_VOL, leftchannel, rightchannel);
}

void Mp3::Initialize() {
  //Setup SD card interface
  pinMode(SS_PIN, OUTPUT);       //Pin 10 must be set as an output for the SD communication to work.
  if (!card.init(SPI_FULL_SPEED, SDCARD_CS))  Serial.println("Error: Card init"); //Initialize the SD card and configure the I/O pins.
  if (!volume.init(&card)) Serial.println("Error: Volume ini"); //Initialize a volume on the SD card.
  if (!root.openRoot(&volume)) Serial.println("Error: Opening root"); //Open the root directory in the volume. 

  Serial.println("MP3 Testing");

  //We have no need to setup SPI for VS1053 because this has already been done by the SDfatlib

  //From page 12 of datasheet, max SCI reads are CLKI/7. Input clock is 12.288MHz. 
  //Internal clock multiplier is 1.0x after power up. 
  //Therefore, max SPI speed is 1.75MHz. We will use 1MHz to be safe.
  SPI.setClockDivider(SPI_CLOCK_DIV16); //Set SPI bus speed to 1MHz (16MHz / 16 = 1MHz)
  SPI.transfer(0xFF); //Throw a dummy byte at the bus
  //Initialize VS1053 chip 
  delay(10);
  digitalWriteFast(MP3_RESET, HIGH); //Bring up VS1053
  digitalWriteFast(AUDIO_AMP_SHTDWN, HIGH); //Enable Audio Output

  //delay(10); //We don't need this delay because any register changes will check for a high DREQ

  //SetVolume(10, 10); //Set initial volume (20 = -10dB) LOUD
  SetVolume(40, 40); //Set initial volume (20 = -10dB) Manageable
  SetVolume(EEPROM_configuration.vol_l, EEPROM_configuration.vol_r); //Set initial volume (20 = -10dB) Manageable
  //SetVolume(80, 80); //Set initial volume (20 = -10dB) More quiet

  //Let's check the status of the VS1053
  int MP3Mode = ReadRegister(SCI_MODE);
  int MP3Status = ReadRegister(SCI_STATUS);
  int MP3Clock = ReadRegister(SCI_CLOCKF);

  Serial.print("SCI_Mode (0x4800) = 0x");
  Serial.println(MP3Mode, HEX);

  Serial.print("SCI_Status (0x48) = 0x");
  Serial.println(MP3Status, HEX);

  int vsVersion = (MP3Status >> 4) & 0x000F; //Mask out only the four version bits
  Serial.print("VS Version (VS1053 is 4) = ");
  Serial.println(vsVersion, DEC); //The 1053B should respond with 4. VS1001 = 0, VS1011 = 1, VS1002 = 2, VS1003 = 3

  Serial.print("SCI_ClockF = 0x");
  Serial.println(MP3Clock, HEX);

  //Now that we have the VS1053 up and running, increase the internal clock multiplier and up our SPI rate
  WriteRegister(SCI_CLOCKF, 0x60, 0x00); //Set multiplier to 3.0x

  //From page 12 of datasheet, max SCI reads are CLKI/7. Input clock is 12.288MHz. 
  //Internal clock multiplier is now 3x.
  //Therefore, max SPI speed is 5MHz. 4MHz will be safe.
  SPI.setClockDivider(SPI_CLOCK_DIV4); //Set SPI bus speed to 4MHz (16MHz / 4 = 4MHz)

  MP3Clock = ReadRegister(SCI_CLOCKF);
  Serial.print("SCI_ClockF = 0x");
  Serial.println(MP3Clock, HEX);
	UCSR2C = UCSR2C | B00001000;
  //MP3 IC setup complete
}

Bounce mp3_dwn       = Bounce( B_DWN      , BUTTON_DEBOUNCE_PERIOD ); 
Bounce mp3_up        = Bounce( B_UP       , BUTTON_DEBOUNCE_PERIOD ); 


//PlayMP3 pulls 32 byte chunks from the SD card and throws them at the VS1053
//We monitor the DREQ (data request pin). If it goes low then we determine if
//we need new data or not. If yes, pull new from SD card. Then throw the data
//at the VS1053 until it is full.
void Mp3::Play(char* fileName) {
	char errorMsg[100]; //This is a generic array used for sprintf of error messages

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
      
      
			if (mp3_dwn.update()) {
				if (mp3_dwn.fallingEdge())	{
					Serial.println("mp3_dwn pressed");
				}	
			}	
			if (mp3_up.update()) {
				if (mp3_up.fallingEdge())	{
					Serial.println("mp3_up pressed");
				}	
			}	
      

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