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

void GraphPatterns::Fill_from_Fill_from_Left(int8_t value, int8_t offset) {
	int8_t graphsize = pwm_led_bar_count/2;
	//if (graphsize % 2) graphsize--;
	value = map(value, 1, 100, 0, graphsize-1);
	for (uint8_t thisPin = 0; thisPin < graphsize; thisPin++) {
		if (thisPin <= value) {
			digitalWrite(pwm_led_bar[thisPin + offset], LED_ON);
		} else {
			digitalWrite(pwm_led_bar[thisPin + offset], LED_OFF);
		}
	}
}

void GraphPatterns::Fill_from_Fill_from_Right(int8_t value, int8_t offset) {
	int8_t graphsize = pwm_led_bar_count/2;
	//if (graphsize % 2) graphsize--;
	value = map(value, 1, 100, graphsize-1, 0);
	for (uint8_t thisPin = 0; thisPin < graphsize; thisPin++) {
		if (thisPin >= value) {
			digitalWrite(pwm_led_bar[thisPin + offset], LED_ON);
		} else {
			digitalWrite(pwm_led_bar[thisPin + offset], LED_OFF);
		}
	}
}

void GraphPatterns::spot_from_left(int8_t value, int8_t offset, int8_t scale) {
	int8_t graphsize = pwm_led_bar_count / scale;
	//if (graphsize % 2) graphsize--;
	value = map(value, 1, 100, 0, graphsize-1);
	for (uint8_t thisPin = 0; thisPin < graphsize; thisPin++) {
		if (thisPin == value) {
			digitalWrite(pwm_led_bar[thisPin + offset], LED_ON);
		} else {
			digitalWrite(pwm_led_bar[thisPin + offset], LED_OFF);
		}
	}
}

void GraphPatterns::all_clear() {
	for (uint8_t  thisPin = 0; thisPin < pwm_led_bar_count; thisPin++) {
		digitalWrite(pwm_led_bar[thisPin], LED_OFF);
	}	
}

void GraphPatterns::all_on() {
	for (uint8_t  thisPin = 0; thisPin < pwm_led_bar_count; thisPin++) {
		digitalWrite(pwm_led_bar[thisPin], LED_ON);
	}	
}

void GraphPatterns::Fill_from_CenterOut(int8_t value, int8_t offset) {
	Fill_from_Fill_from_Left(value, pwm_led_bar_count/2 + offset);
	Fill_from_Fill_from_Right(value, offset);
}

void GraphPatterns::CylonEye(uint16_t cylon_delay, uint8_t repetitions) {
	for (uint8_t  cycles = 0; ((cycles < repetitions) || (repetitions == 0)); cycles++) {
		for (int8_t  thisPin = 0; thisPin < 101; thisPin++) {
			Fill_from_CenterOut(thisPin, 0);
			delay(cylon_delay);
		}
	
		for (int8_t  thisPin = 101; thisPin > 0; thisPin--) {
			Fill_from_CenterOut(thisPin, 0);
			delay(cylon_delay);
		}
	}
}