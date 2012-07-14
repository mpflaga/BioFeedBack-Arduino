/*
  This library is distributed in the hope that it will be useful,
  but WITHOUT ANY WARRANTY; without even the implied warranty of
  MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the GNU
  Lesser General Public License for more details.

  You should have received a copy of the GNU Lesser General Public
  License along with this library; if not, write to the Free Software
  Foundation, Inc., 51 Franklin St, Fifth Floor, Boston, MA  02110-1301  USA
*/


#include "BioFeedBack.h"
#include <inttypes.h>
#include "Arduino.h"
#include <digitalWriteFast.h>

float fmap(float x, float in_min, float in_max, float out_min, float out_max)
{
 return (x - in_min) * (out_max - out_min) / (in_max - in_min) + out_min;
} 

float GetBatteryVoltage(uint8_t channel)
{
	return fmap(analogRead(channel), 0, (2*(BATTERY_CALIBRATION))-1, 0.0, (2*(REGULATOR_VOLTAGE)));
}

void PulseDigitalPOT(uint8_t chipSelectPin, int8_t pulses) {
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

void SetDigitalPOT(uint8_t chipSelectPin, int8_t value) {
	// Step 1; Force Digital POT Value to a known value of ZERO.
	PulseDigitalPOT(chipSelectPin, -65); // POT's range is 64
	delayMicroseconds(1);
	// Step 2; Place Digital POT Value to desired Value.
	PulseDigitalPOT(chipSelectPin, value);
}	


void BlinkEachButtonLeds() {
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

void PulseLedBar () {
	for (uint8_t  thisPin = 0; thisPin < pwm_led_bar_count; thisPin++)  {
		digitalWrite(pwm_led_bar[thisPin], LOW);
	}
	delay(POST_TIMER);
	for (uint8_t  thisPin = 0; thisPin < pwm_led_bar_count; thisPin++)  {
		digitalWrite(pwm_led_bar[thisPin], HIGH);
	}
}

void CylonEye() {
	// loop from the lowest pin to the highest:
	for (uint8_t  thisPin = 0; thisPin < pwm_led_bar_count; thisPin++) {
		// turn the pin on:
		digitalWrite(pwm_led_bar[thisPin], LOW);
		delay(CYLON_DELAY);
		// turn the pin off:
		digitalWrite(pwm_led_bar[thisPin], HIGH);
	}
}

void FadeLedBar() {
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

void BlinkAllButtonLeds() {
	// BLINK all Button LED's at once, twice
	for (uint8_t  thisPin = 0; thisPin < button_leds_count; thisPin++)  {
		digitalWrite(button_leds[thisPin], LOW);
	}
	delay(POST_TIMER);
	for (uint8_t  thisPin = 0; thisPin < button_leds_count; thisPin++)  {
		digitalWrite(button_leds[thisPin], HIGH);
	}
}

void PreOperatingSelfTest() {

//	PulseLedBar();
//	delay(POST_TIMER);
//	PulseLedBar();

//	CylonEye();

	FadeLedBar();

//	BlinkAllButtonLeds();
//	delay(POST_TIMER);
	BlinkAllButtonLeds();

//	BlinkEachButtonLeds();

	// Report Battery Voltage
	Serial.print("Battery Voltage = ");
	Serial.print(GetBatteryVoltage(ANA_BATTERY), 2); // two decimal places
	Serial.println(" volts");

} // PreOperatingSelfTest()

void configure_pins() {
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
	pinMode(TMPOF_CS ,OUTPUT );
	pinMode(GSROF_CS ,OUTPUT );
	pinMode(TMP_CS ,OUTPUT );
	pinMode(GSR_CS ,OUTPUT );
// May not need to set PinMode of these Analog Input- MPF
//	pinMode(ANA_GSR ,INPUT );
//	pinMode(ANA_TMP ,INPUT );
//	pinMode(ANA_BATTERY ,INPUT );

	// Set the Initial Value of All Input Pins.
	digitalWriteFast(P_ONOFF_CTRL, HIGH);  // set the Power On!
// Should not need to set Initial Value of UARTS - MPF
//  digitalWriteFast(MIDI_RX, HIGH);
//  digitalWriteFast(BT_RX, HIGH);
  digitalWriteFast(AUDIO_AMP_SHTDWN, LOW); //Initially Disable(HIGH) Audio Output
//  digitalWriteFast(BT_CTS, HIGH);   //Initially Disable(HIGH) Radio Transmission
  digitalWriteFast(BT_CTS, LOW);   //Leave Enabled, as to span resets for Debug.
  digitalWriteFast(BT_RST, LOW);   //Initially Hold Radio in Reset
  digitalWriteFast(TMPOF_CS, HIGH); //Initially Deselect DAC
  digitalWriteFast(GSROF_CS, HIGH); //Initially Deselect DAC
  digitalWriteFast(TMP_CS, HIGH);   //Initially Deselect PGA
  digitalWriteFast(GSR_CS, HIGH);   //Initially Deselect PGA

  digitalWriteFast(MP3_XCS, HIGH);   //Initially Deselect Control
  digitalWriteFast(MP3_XDCS, HIGH);  //Initially Deselect Data
  digitalWriteFast(SDCARD_CS, HIGH); //Initially Deselect SD Card
  digitalWriteFast(TMPOF_CS, HIGH);  //Initially Deselect Data
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