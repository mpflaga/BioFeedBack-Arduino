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

class GraphPatterns
{
public:
	void Fill_from_Fill_from_Left(int8_t, int8_t);
	void Fill_from_Fill_from_Right(int8_t, int8_t);
	void Fill_from_CenterOut(int8_t, int8_t);
	void spot_from_left(int8_t, int8_t, int8_t);
	void all_on();
	void all_clear();
	void CylonEye(uint16_t, uint8_t);
};

#endif  // End GraphPatterns_h
