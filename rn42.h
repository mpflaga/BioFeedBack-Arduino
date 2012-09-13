/*
  This library is distributed in the hope that it will be useful,
  but WITHOUT ANY WARRANTY; without even the implied warranty of
  MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the GNU
  Lesser General Public License for more details.

  You should have received a copy of the GNU Lesser General Public
  License along with this library; if not, write to the Free Software
  Foundation, Inc., 51 Franklin St, Fifth Floor, Boston, MA  02110-1301  USA
*/

#ifndef rn42_h
#define rn42_h

class RN42
{
public:
	void begin();
	void end();
	void TxOn();
	void TxOff();
	
private:
	String command (String, char, long);
	void flush(long);
	String read_version;
	String read_serial;
	String read_connection;
//	String get_parameter;
};

#endif  // End rn42_h
