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
#define DEFAULT_FLUSH_TIMEOUT 500 // ms

void RN42::begin() {
	digitalWrite(BT_RST, BT_RST_Enabled);   //Take Radio out of Reset
	digitalWrite(BT_CTS, BT_CTS_Enabled);   //Enable Transmitter
	
	delay(1000); // need to wait for the radio to stablize.

	Serial.print("$$$ = ");
	Serial.print (command("$$$", '\n', DEFAULT_FLUSH_TIMEOUT));
	Serial.println();
	delay(1000); // delay as it avoids problems, with flush.
	
	read_version = command("v\n", '\n', DEFAULT_FLUSH_TIMEOUT);
	read_serial = command("GB\n", '\n', DEFAULT_FLUSH_TIMEOUT);
	read_connection = command("GK\n", '\n', DEFAULT_FLUSH_TIMEOUT);

	Serial.print("Online = ");
	Serial.print(command("---\n", '\n', DEFAULT_FLUSH_TIMEOUT));
	Serial.println();
	
	Serial.print("version1 = ");
	Serial.println(read_version);
	
	Serial.print("Serial = ");
	Serial.print(read_serial);
	Serial.println();
	
	Serial.print("Connection = ");
	Serial.print(read_connection);
	Serial.println();
}

void RN42::end() {
	//turn radio of and put into reset.
	digitalWrite(BT_RST, BT_RST_Disabled); 
	digitalWrite(BT_CTS, BT_CTS_Disabled);
}	

void RN42::TxOff() {
	// turn Transmitter off to save power
	digitalWrite(BT_CTS, BT_CTS_Disabled);
}	

void RN42::TxOn() {
	// turn Transmitter on, more power
	digitalWrite(BT_CTS, BT_CTS_Enabled);
}

void RN42::flush(long timeout) {
		long start_time = millis();
		int state = 1;
		char c;
		while (state)
		{
			int i = Serial3.available();
			for (int  thischar = 0; thischar < i; thischar++) {
				c = Serial3.read();
			}
			if ( (millis() - start_time) > timeout ) 
			{
				state = 0;
			}
		}
}

//enum parameter {gVERSION, gSERIAL, gCONNECTION};
//String RN42::get_parameter (parameter currentWeather)
//{
//    switch(currentWeather) {
//    case gVERSION:
//        return read_version;
//        break;
//    case gSERIAL:
//        return read_serial;
//        break;
//    case gCONNECTION:
//        return read_connection;
//        break;
//    }
//}

String RN42::command (String sCommand, char cTerminator, long timeout)
{
	String	inData="";
	long start_time = millis();
	int state = 0;
	Serial3.print(sCommand);
	while (!state)
	{
		int len = Serial3.available();		
		for (int  thischar = 0; thischar < len; thischar++) {
				char c = (char) Serial3.read();
//				Serial.print("0x");
//				Serial.println((int) c, HEX);
				if ((c == '\n')) {// || (c == '\r')) {
					state = 1;
					break;
				} else {
					if (state != 1) {
						inData += c;
					}
				}
		}
		
		if ( (millis() - start_time) > timeout ) 
		{
			state = 2;
			//Serial.println ("TimedOut");
		}
	}
	inData.trim();
	if ((state = 1)) flush(timeout);
	return inData;
}