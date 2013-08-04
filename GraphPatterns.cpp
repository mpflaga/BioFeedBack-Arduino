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

void GraphPatterns::spot_from_left(int8_t value, int8_t offset) {
	int8_t graphsize = pwm_led_bar_count / tscale;
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

void GraphPatterns::begin(uint16_t delay) {
	lastUpdateTime = millis();
	upDateDelay = delay;
	led_pos = 0;
}

void GraphPatterns::update() {
	cli(); 
	int16_t current_millis_value = millis(); 
	sei();
	int8_t pos;
	
	if ((current_millis_value - lastUpdateTime) > upDateDelay) {
		int8_t graphsize = pwm_led_bar_count;
		
//		Serial.print("Tick of LedGraph = ");
//		Serial.print(led_pos, DEC);
//		Serial.print(" pos = ");
//		Serial.println(pos, DEC);

//	char bar1[] = {0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0 };
//	char bar2[] = {0, 0, 0, 0, 0, 0, 1, 1, 0, 0, 0, 0, 0, 0 };
//	char bar3[] = {0, 0, 0, 0, 0, 1, 1, 1, 1, 0, 0, 0, 0, 0 };
//	char bar4[] = {0, 0, 0, 0, 1, 1, 1, 1, 1, 1, 0, 0, 0, 0 };
//	char bar5[] = {0, 0, 0, 1, 1, 1, 1, 1, 1, 1, 1, 0, 0, 0 };
//	char bar6[] = {0, 0, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 0, 0 };
//	char bar7[] = {0, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 0 };
//	char bar8[] = {1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1 };
//	char bar9[] = {0, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 0 };
//	char bara[] = {0, 0, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 0, 0 };
//	char barb[] = {0, 0, 0, 1, 1, 1, 1, 1, 1, 1, 1, 0, 0, 0 };
//	char barc[] = {0, 0, 0, 0, 1, 1, 1, 1, 1, 1, 0, 0, 0, 0 };
//	char bard[] = {0, 0, 0, 0, 0, 1, 1, 1, 1, 0, 0, 0, 0, 0 };
//	char bare[] = {0, 0, 0, 0, 0, 0, 1, 1, 0, 0, 0, 0, 0, 0 };
//

	char bar[3][14][14] = {{
	                 {0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0 },
                   {0, 0, 0, 0, 0, 0, 1, 1, 0, 0, 0, 0, 0, 0 },
                   {0, 0, 0, 0, 0, 1, 1, 1, 1, 0, 0, 0, 0, 0 },
                   {0, 0, 0, 0, 1, 1, 1, 1, 1, 1, 0, 0, 0, 0 },
                   {0, 0, 0, 1, 1, 1, 1, 1, 1, 1, 1, 0, 0, 0 },
                   {0, 0, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 0, 0 },
                   {0, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 0 },
                   {1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1 },
                   {0, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 0 },
                   {0, 0, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 0, 0 },
                   {0, 0, 0, 1, 1, 1, 1, 1, 1, 1, 1, 0, 0, 0 },
                   {0, 0, 0, 0, 1, 1, 1, 1, 1, 1, 0, 0, 0, 0 },
                   {0, 0, 0, 0, 0, 1, 1, 1, 1, 0, 0, 0, 0, 0 },
                   {0, 0, 0, 0, 0, 0, 1, 1, 0, 0, 0, 0, 0, 0 }
                   } , {
	                 {0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0 },
                   {0, 1, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0 },
                   {0, 0, 1, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0 },
                   {0, 0, 0, 1, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0 },
                   {0, 0, 0, 0, 1, 0, 0, 0, 0, 0, 0, 0, 0, 0 },
                   {0, 0, 0, 0, 0, 1, 0, 0, 0, 0, 0, 0, 0, 0 },
                   {0, 0, 0, 0, 0, 0, 1, 0, 0, 0, 0, 0, 0, 0 },
                   {0, 0, 0, 0, 0, 0, 0, 1, 0, 0, 0, 0, 0, 0 },
                   {0, 0, 0, 0, 0, 0, 0, 0, 1, 0, 0, 0, 0, 0 },
                   {0, 0, 0, 0, 0, 0, 0, 0, 0, 1, 0, 0, 0, 0 },
                   {0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 1, 0, 0, 0 },
                   {0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 1, 0, 0 },
                   {0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 1, 0 },
                   {0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 1 }
                   } , {
	                 {0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0 },
                   {0, 0, 0, 0, 0, 0, 1, 1, 0, 0, 0, 0, 0, 0 },
                   {0, 0, 0, 0, 0, 1, 0, 0, 1, 0, 0, 0, 0, 0 },
                   {0, 0, 0, 0, 1, 0, 0, 0, 0, 1, 0, 0, 0, 0 },
                   {0, 0, 0, 1, 0, 0, 0, 0, 0, 0, 1, 0, 0, 0 },
                   {0, 0, 1, 0, 0, 0, 0, 0, 0, 0, 0, 1, 0, 0 },
                   {0, 1, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 1, 0 },
                   {1, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 1 },
                   {0, 1, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 1, 0 },
                   {0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 1, 0, 0 },
                   {0, 0, 0, 1, 0, 0, 0, 0, 0, 0, 1, 0, 0, 0 },
                   {0, 0, 0, 0, 1, 0, 0, 0, 0, 1, 0, 0, 0, 0 },
                   {0, 0, 0, 0, 0, 1, 0, 0, 1, 0, 0, 0, 0, 0 },
                   {0, 0, 0, 0, 0, 0, 1, 1, 0, 0, 0, 0, 0, 0 }
                 }};

			switch (led_pos++) {
			  default: 
			  	led_pos = 0;
			  case 0:
			  	printled((bar[pattern][0]));
					break;
			  case 1:
			  	printled((bar[pattern][1]));
					break;
			  case 2:
			  	printled((bar[pattern][2]));
					break;
			  case 3:
			  	printled((bar[pattern][3]));
					break;
			  case 4:
			  	printled((bar[pattern][4]));
					break;
			  case 5:
			  	printled((bar[pattern][5]));
					break;
			  case 6:
			  	printled((bar[pattern][6]));
					break;
			  case 7:
			  	printled((bar[pattern][7]));
					break;
			  case 8:
			  	printled((bar[pattern][8]));
					break;
			  case 9:
			  	printled((bar[pattern][9]));
					break;
			  case 10:
			  	printled((bar[pattern][10]));
					break;
			  case 11:
			  	printled((bar[pattern][11]));
					break;
			  case 12:
			  	printled((bar[pattern][12]));
					break;
			  case 13:
			  	printled((bar[pattern][13]));
					break;
			}

		cli(); 
		lastUpdateTime = millis();
		sei();
	}
}

void GraphPatterns::printled(char* p) {
	for (uint8_t  thisPin = 0; thisPin < pwm_led_bar_count; thisPin++) {
		if (p[thisPin] == 0) {
			digitalWrite(pwm_led_bar[thisPin], 1);
		} else {
			digitalWrite(pwm_led_bar[thisPin], 0);
		}
	}			  	
}