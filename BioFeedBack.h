/*
  This library is distributed in the hope that it will be useful,
  but WITHOUT ANY WARRANTY; without even the implied warranty of
  MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the GNU
  Lesser General Public License for more details.

  You should have received a copy of the GNU Lesser General Public
  License along with this library; if not, write to the Free Software
  Foundation, Inc., 51 Franklin St, Fifth Floor, Boston, MA  02110-1301  USA
*/

#ifndef BioFeedBack_h
#define BioFeedBack_h
#include "Arduino.h"
#include <inttypes.h>

#define DEBUG

// Power Control output Pins
#define PS_MODE      35  /* Drive Power Save Mode on TPS61200 */
#define P_ONOFF_CTRL 36  /* Drive Power Save Mode on TPS61200 */

// Solid LED output Pins
#define LD_DIAG     13  /* LD3   to PB7 */
#define LD_GSR2     38  /* LD104 to PD7 */
#define LD_THR_BLW  24  /* LD101 to PG2 */
#define LD_THR_ABV  39  /* LD101 to PA2 */
#define LD_UP       48  /* LD106 to PL1 */
#define LD_TMP1     A8  /* LD105 to PK0 */
#define LD_DWN      A10 /* LD100 to PK2 */
#define LD_GSR1     83  /* LD108 to PE7(not MegaPro/Seeduino) */
#define LD_TMP2     25  /* LD103 to PA3 */

// Button Input Pins
#define B_CH1       A9   /* S107 to PK1 */
#define B_DWN       A15  /* S105 to PK7 */
#define B_UP        49   /* S106 to PL0 */
#define B_CNTR      28   /* S102 to PA6 */
#define B_THR       22   /* S100 to PA0 */
#define B_CH2       77   /* S103 to PJ7(not MegaPro/Seeduino) */
#define B_AUDIO     80   /* S104 to PD6(not MegaPro/Seeduino) */
#define B_DISP      70   /* S108 to PH2(not MegaPro/Seeduino) */
#define B_ONOFF_SNS 37   /* S101 to PC0 */

// SPI Pins
#define SPI_MISO    50      //PB3 input
#define SPI_MOSI    51      //PB2 output
#define SPI_SCK     52      //PB1 output
#define SPI_SS      53      //PB0 input // Used Elseware !!!

// I2C Pins
// Not Used in this Design
#define I2C_SDA    20      //PD1 input
#define I2C_SCL    21      //PD0 input

#if defined(__AVR_ATmega2560__) || defined(__AVR_ATmega1280__)
#define MP3_XCS     67      //PK5 Output, Active Low,  Control Chip Select Pin (for accessing SPI Control/Status registers)
#define MP3_XDCS    68      //PK6 Output, Active Low,  Data Chip Select / BSYNC Pin
#define MP3_DREQ    66      //PK4 Input , Active High, Data Request Pin: Player asks for more data
#define MP3_RESET   65      //PK3 Output, Active Low,  Reset is active low
#define SDCARD_CS   76      //PJ6 Output, Active Low
#else
#define MP3_XCS      6      //Control Chip Select Pin (for accessing SPI Control/Status registers)
#define MP3_XDCS     7      //Data Chip Select / BSYNC Pin
#define MP3_DREQ     2      //Data Request Pin: Player asks for more data
#define MP3_RESET    8      //Reset is active low
#define SDCARD_CS    9      //Output, Active Low
#endif

#define AUDIO_AMP_SHTDWN 85 //PE2 Output, Active Low, Disables the Amp

#define MIDI_RX     16      //PH1 Output, USART2_TX
#define MIDI_TX     17      //PH0 Input,  USART2_RX

#define BT_RX       14      //PJ1 Output, USART3_TX
#define BT_TX       15      //PJ0 Input,  USART3_RX
#define BT_CTS      27      //PA5 Output, Active Low,  Enable Device Transmission, Not Sure - MPF
#define BT_RTS      26      //PA4 Input , Active Low,  Requesting Data, Not Sure - MPF
#define BT_RST      23      //PA1 Output, Active Low,  Resets BlueTooth Transciever
#define BT_CD       72      //PJ2 Input, 

#define TMPOF_CS    71      //PH7 Output, Active Low,  Enables MOSI of Offset DAC of Tempurature
#define GSROF_CS    53      //PB0 Output, Active Low,  Enables MOSI of Offset DAC of GSR
#define TMP_CS      82      //PG3 Output, Active Low,  Enables MOSI of PGA of TMP
#define GSR_CS      84      //PE6 Output, Active Low,  Enables MOSI of PGA of GSR

#define ANA_GSR     A0      //(54)PF0 Analog Input, from GSR Sensor.
#define ANA_TMP     A1      //(55)PF1 Analog Input, from TMP Sensor.
#define ANA_BATTERY A7      //(61)PF7 Analog Input, from Battery Voltage 50%.

#define UP          HIGH
#define DOWN        LOW
#define MCP4013_FULL_SCALE 64

#define BATTERY_CALIBRATION 525
#define REGULATOR_VOLTAGE 3.33

#define FADE_DELAY 10
#define CYLON_DELAY 100
#define POST_TIMER 200

const int pwm_led_bar[] = {
4, 5, 2, 3, 6, 7, 8, 9, 10, 11, 12, 46, 45, 44 };
#define pwm_led_bar_count (uint8_t) sizeof(pwm_led_bar) / sizeof(int)

const int button_leds[] = {
LD_TMP1, LD_GSR1, LD_DWN, LD_UP, LD_TMP2, LD_GSR2, LD_THR_BLW, LD_THR_ABV }; //
#define button_leds_count (uint8_t) sizeof(button_leds) / sizeof(int)

const int button_input[] = {
B_CH1, B_DISP, B_DWN, B_UP, B_CH2, B_AUDIO, B_CNTR, B_THR, B_ONOFF_SNS };
#define button_input_count (uint8_t) sizeof(button_input) / sizeof(int)

const int button_test_leds[] = {
	B_CH1,       LD_TMP1,
	B_DISP,      LD_GSR1,
	B_DWN,       LD_DWN,
	B_UP,        LD_UP,
	B_CH2,       LD_TMP2,
	B_AUDIO,     LD_GSR2,
	B_CNTR,      LD_THR_BLW,
	B_THR,       LD_THR_ABV,
	B_ONOFF_SNS, pwm_led_bar[1]
};
#define button_test_leds_count (uint8_t) sizeof(button_test_leds) / sizeof(int)

float fmap(float, float, float, float, float);
void PulseDigitalPOT(uint8_t, int8_t);
void SetDigitalPOT(uint8_t, int8_t);
void configure_pins();


class Sensor
{
public:
	float GetBatteryVoltage(uint8_t);
}
;

class PreOperatingSelfTest
{
public:
	void PulseLedBar();
	void CylonEye();
	void FadeLedBar();
	void BlinkAllButtonLeds();
	void BlinkEachButtonLeds();
}
;

#endif  // End BioFeedBack_h
