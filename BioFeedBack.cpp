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
#include <Bounce.h>
#include <GraphPatterns.h>

GraphPatterns	BarGraph;

volatile StoreStruct_t EEPROM_configuration = {
	// The default values
	40, 40,
	CONFIG_VERSION
};

float fmap(float x, float in_min, float in_max, float out_min, float out_max) {
	return (x - in_min) * (out_max - out_min) / (in_max - in_min) + out_min;
}

float Sensor::GetBatteryVoltage(uint8_t channel) {
	return fmap(analogRead(channel), 0, (2*(BATTERY_CALIBRATION))-1, 0.0, (2*(REGULATOR_VOLTAGE)));
}

float Sensor::GetTMPVoltage(uint8_t channel) {
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
	for (uint8_t  thisPin = 0; thisPin < abs(pulses); thisPin++) {
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
	Serial.print(F("PGA GAIN = 8"));
	Serial.println(Gain, DEC);
	#endif
}

void PreOperatingSelfTest::BlinkEachButtonLeds() {
	for (uint8_t  thisPin = 0; thisPin < button_leds_count; thisPin++) {
		// turn the pin on:
		digitalWrite(button_leds[thisPin], LED_ON);
		delay(POST_TIMER);
		// turn the pin off:
		digitalWrite(button_leds[thisPin], LED_OFF);
	}
	// loop from the highest pin to the lowest:
	for (int8_t  thisPin = button_leds_count - 1; thisPin >= 0; thisPin--) {
		// turn the pin on:
		digitalWrite(button_leds[thisPin], LED_ON);
		delay(POST_TIMER);
		// turn the pin off:
		digitalWrite(button_leds[thisPin], LED_OFF);
	}
}

void PreOperatingSelfTest::Cylon(uint16_t cylon_delay, uint8_t repetitions) {
	for (uint8_t  cycles = 0; ((cycles < repetitions) || (repetitions == 0)); cycles++) {
		//BarGraph.tscale = BarGraph.full;
		BarGraph.setscale(BarGraph.full);
		for (uint8_t  cycles = 1; cycles <= 100; cycles++) {
			BarGraph.spot_from_left(cycles, 0);
			delay(cylon_delay);
		}
		for (uint8_t  cycles = 100; cycles >= 1; cycles--) {
			BarGraph.spot_from_left(cycles, 0);
			delay(cylon_delay);
		}
	}
}

void PreOperatingSelfTest::FadeLedBar() {
	// fade in from min to max in increments of 5 points:
	for(uint8_t  fadeValue = 255 ; fadeValue >= 200; fadeValue -=5) {
		// sets the value (range from 0 to 255):
		for (uint8_t  thisPin = 0; thisPin < pwm_led_bar_count; thisPin++) {
			analogWrite(pwm_led_bar[thisPin], fadeValue);
		}
		// wait for 30 milliseconds to see the dimming effect
		delay(FADE_DELAY);
	}
	// fade out from max to min in increments of 5 points:
	for(int  fadeValue = 0 ; fadeValue <= 255; fadeValue +=5) {
		// sets the value (range from 0 to 255):
		for (uint8_t  thisPin = 0; thisPin < pwm_led_bar_count; thisPin++) {
			analogWrite(pwm_led_bar[thisPin], fadeValue);
		}
		// wait for 30 milliseconds to see the dimming effect
		delay(FADE_DELAY);
	}
}

void PreOperatingSelfTest::BlinkAllButtonLeds() {
	// BLINK all Button LED's at once, twice
	for (uint8_t  thisPin = 0; thisPin < button_leds_count; thisPin++) {
		digitalWrite(button_leds[thisPin], LED_ON);
	}
	delay(POST_TIMER);
	for (uint8_t  thisPin = 0; thisPin < button_leds_count; thisPin++) {
		digitalWrite(button_leds[thisPin], LED_OFF);
	}
}

void PreOperatingSelfTest::PulseLedBar () {
	BarGraph.all_on();
	delay(POST_TIMER);
	BarGraph.all_clear();
}

void PreOperatingSelfTest::post () {

	#ifdef DEBUG
	PulseLedBar();
	delay(POST_TIMER);
	PulseLedBar();
	Cylon(CYLON_DELAY, 1);
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

} // PreOperatingSelfTest();

void HW_configuration::CylonEye () {
	BarGraph.CylonEye(CYLON_DELAY, 1);
	BarGraph.all_clear();
}

void HW_configuration::BoardsPinMode () {
	wdt_disable();

	pinMode(LD_DIAG, OUTPUT);
	pinMode(PS_MODE, INPUT);      //Pin #55(PC2) PS_MODE
	pinMode(P_ONOFF_CTRL, OUTPUT);     //Pin #54(PC1) P_ONOFF_CTRL

	pinMode(MP3_XCS, OUTPUT);
	pinMode(MP3_XDCS, OUTPUT);
	pinMode(MP3_DREQ, INPUT);
	pinMode(MP3_RESET, OUTPUT);
	pinMode(SD_SEL, OUTPUT);

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
	digitalWriteFast(BT_CTS, BT_CTS_Disabled);   //Leave Enabled, as to span resets for Debug.
	digitalWriteFast(BT_RST, BT_RST_Disabled);   //Initially Hold Radio in Reset
	digitalWriteFast(TMPOFFSET_CS, HIGH); //Initially Deselect DAC
	digitalWriteFast(GSROFFSET_CS, HIGH); //Initially Deselect DAC
	digitalWriteFast(TMPPGA_CS, HIGH);   //Initially Deselect PGA
	digitalWriteFast(GSRPGA_CS, HIGH);   //Initially Deselect PGA

	digitalWriteFast(MP3_XCS, HIGH);   //Initially Deselect Control
	digitalWriteFast(MP3_XDCS, HIGH);  //Initially Deselect Data
	digitalWriteFast(SD_SEL, HIGH); //Initially Deselect SD Card
	digitalWriteFast(TMPOFFSET_CS, HIGH);  //Initially Deselect Data
	digitalWriteFast(MP3_RESET, LOW);  //Initially Put VS1053 into hardware reset

	// PinMode Buttons
	for (uint8_t  thisPin = 0; thisPin < button_input_count; thisPin++) {
		pinMode(button_input[thisPin], OUTPUT);
		digitalWrite(button_input[thisPin], HIGH);
	}

	// PinMode and Clear LEDs
	for (uint8_t  thisPin = 0; thisPin < button_leds_count; thisPin++) {
		pinMode(button_leds[thisPin], OUTPUT);
		digitalWrite(button_leds[thisPin], HIGH);
	}

	// PinMode and Clear LED Bar
	for (uint8_t  thisPin = 0; thisPin < pwm_led_bar_count; thisPin++) {
		pinMode(pwm_led_bar[thisPin], OUTPUT);
		digitalWrite(pwm_led_bar[thisPin], HIGH);
	}
}

void HW_configuration::Reset () {
	digitalWriteFast(AUDIO_AMP_SHTDWN, LOW); //Turn Off Audio AMP to prevent POP
	digitalWriteFast(P_ONOFF_CTRL, LOW); // turn off
	delay(1000);           // this prevents next WDTO from bouncing system before above line settles, in off
	Serial.println("Using the WatchDog to Soft Reset");
	wdt_enable(WDTO_15MS); // provides a Soft Reset when connected to FDTI Port, that provides power
	for(;;)
	{
	}
}
