
//
// pumpkinbot 2014
//
// hack o'lantern 2014
//
// author: @jefforulez
//

//
// parallax rfid code borrowed from
// https://github.com/ril3y/Make-Projects/blob/master/Arduino_Parallax_RFID_Reader/Arduino_Parallax_RFID_Reader.pde
//

#include "pumpkinbot2014.h"

//
// pin assignments
//

const int EMIC2_RX     =  2 ;
const int EMIC2_TX     =  3 ;

const int RFID_RX      =  6 ;
const int RFID_TX      =  7 ;
const int RFID_ENABLE  =  8 ;

const int LED_PIN      = 13 ;

//
// rfid globals
//

const int CODE_LEN         = 10   ; // max length of RFID tag
const int VALIDATE_LENGTH  = 200  ; // maximum reads b/w tag read and validate
const char START_BYTE      = 0x0A ;
const char STOP_BYTE       = 0x0D ;

const char candySourPatchKids[] = "35021E906B" ;

char rfid_tag[ CODE_LEN + 1 ] ;

SoftwareSerial serialRFID( 6, 7) ; // RX, TX

//
// tts globals
//

SoftwareSerial emic2Serial = SoftwareSerial( EMIC2_RX, EMIC2_TX ) ;
Emic2TtsModule emic2TtsModule = Emic2TtsModule( &emic2Serial ) ;

//
// methods
//

void setup()
{
	Serial.begin( 9600 ) ;

	//
	// initialize pins
	//

	// tts
	pinMode( EMIC2_RX, INPUT ) ;
	pinMode( EMIC2_TX, OUTPUT ) ;

	// rfid
	pinMode( RFID_ENABLE, OUTPUT ) ;
	pinMode( RFID_RX, INPUT ) ;
	pinMode( RFID_TX, OUTPUT ) ;
	
	// leds
	pinMode( LED_PIN, OUTPUT ) ;

	//
	// initialize devices
	//

	// turn the rfid reader on
	digitalWrite( RFID_ENABLE, LOW ) ;
	delay( 1000 ) ;

}

void loop()
{
	Serial.println( "loop()" ) ;

	// blocks while an RFID tag is present
	blockWhileRFIDPresent() ;

	sayGiveMeCandy() ;

	delay( 1000 ) ;
}

//
// TTS
//

int voice_index = 0 ;

void sayGiveMeCandy()
{
	Serial.println( "sayGiveMeCandy()" ) ;

	emic2Serial.begin( 9600 ) ;
	emic2TtsModule.init() ;
	emic2TtsModule.setVolume( 40 ) ;
	emic2TtsModule.setWordsPerMinute( 250 ) ;
	emic2TtsModule.setVoice( (EmicVoice)( voice_index++ % 9 ) ) ;
	// emic2TtsModule.setVoice( HugeHarry ) ;
	emic2TtsModule.say( F("Yo, witches. Where's my candy") ) ;
}

//
// RFID
//

void blockWhileRFIDPresent() 
{
	Serial.println( "blockWhileRFIDPresent()" ) ;

	int no_rfid_count = 0 ;
	
	// start the serial connections
	serialRFID.begin( 2400 ) ;

	while ( 42 )
	{
		// start the loop with a clean input buffer
		serialRFID.flush() ;

		// give the reader time to generate new data
		delay( 400 ) ;		

		// clear the rfid_tag
		for ( int i = 0 ; i < CODE_LEN + 1 ; i++ ) {
			rfid_tag[i] = 0x0 ;
		}
			
		byte byte_read = 0x0 ;
	
		// throw away garbage bytes and wait for the start_byte
		while ( serialRFID.available() > 0 )
		{
			byte_read = serialRFID.read() ;
			
			if ( byte_read == START_BYTE ) { 
				break ;
			}
			else if ( byte_read == STOP_BYTE ) {
				Serial.println( "received STOP_BYTE when expecting START_BYTE" ) ;
				delay( 200 ) ;
				continue ;			
			}
			delay( 200 ) ;
		}
	
		// break, if we haven't gotten a start byte by now
		if ( byte_read != START_BYTE ) 
		{
			Serial.println( "error: expecting START_BYTE" ) ;

			// allow for one error to account for noise and timing issues
			if ( no_rfid_count++ < 1 ) {
				continue ;
			}

			// turn the LEDs on when the rfid is missing
			ledsOn() ;
			
			break ;
		}

		// turn the LEDs off when the rfid is present
		ledsOff() ;

		byte bytesread = 0x0 ;

		while ( bytesread < CODE_LEN ) 
		{
			// Serial.println( "reading RFID tag" ) ;
			
			if ( serialRFID.available() <= 0 ) {
				Serial.println( "error: expecting more bytes" ) ;
				break ;
			}
			
			if ( ( byte_read = serialRFID.read() ) == STOP_BYTE ) {
				Serial.println( "error: expecting STOP_BYTE" ) ;
				break ;
			}
			
			rfid_tag[ bytesread++ ] = byte_read ;
			
			delay( 200 ) ;
		}
		
		Serial.println( "tag: " ) ;
		Serial.println( rfid_tag ) ;			
	}

	return ;
}

//
// LEDs
//

void ledsOn()
{
	digitalWrite( LED_PIN, HIGH ) ;
}

void ledsOff()
{
	digitalWrite( LED_PIN, LOW ) ;
}

