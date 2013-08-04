/*
  This library is distributed in the hope that it will be useful,
  but WITHOUT ANY WARRANTY; without even the implied warranty of
  MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the GNU
  Lesser General Public License for more details.

  You should have received a copy of the GNU Lesser General Public
  License along with this library; if not, write to the Free Software
  Foundation, Inc., 51 Franklin St, Fifth Floor, Boston, MA  02110-1301  USA
*/

#ifndef GraphPatterns_h
#define GraphPatterns_h


#define full_scale 1
#define half_scale 2

const uint8_t pwm_led_bar[] = {
4, 5, 2, 3, 6, 7, 8, 9, 10, 11, 12, 46, 45, 44 };
const uint8_t pwm_led_bar_count = (uint8_t) (sizeof(pwm_led_bar) / sizeof(* pwm_led_bar));

//	enum scale {
//		full = 1,
//		half
//	} ;

class GraphPatterns
{
	public:
	enum scale {
		full = 1,
		half
	}/*tscale*/ ;
	
	enum led_pattern {
		cyloneye = 0,
		cylon,
		slider
	} pattern;

	scale getscale() {return tscale;};
	void setscale(scale value) {tscale = value;};
	void Fill_from_Fill_from_Left(int8_t, int8_t);
	void Fill_from_Fill_from_Right(int8_t, int8_t);
	void Fill_from_CenterOut(int8_t, int8_t);
	void spot_from_left(int8_t, int8_t);
	void all_on();
	void all_clear();
	void CylonEye(uint16_t, uint8_t);
	void update();
	void begin(uint16_t);
	
	private:
	scale tscale;
	uint8_t led_pos;
	int16_t lastUpdateTime;
	int16_t upDateDelay;
	void printled(char*);
};


#endif  // End GraphPatterns_h
