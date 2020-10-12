#include <Wire.h>
#include "oXs_eagletree.h"

#ifdef DEBUG
#endif

extern unsigned long micros( void ) ;
extern unsigned long millis( void ) ;
extern void delay(unsigned long ms) ;

#if defined(EAGLETREE_CONNECTED) && (EAGLETREE_CONNECTED == YES )// && defined( ADS_MEASURE) 


#ifdef DEBUG  
OXS_EAGLETREE::OXS_EAGLETREE(HardwareSerial &print)
#else
OXS_EAGLETREE::OXS_EAGLETREE(uint8_t x)
#endif
{  // constructor
#ifdef DEBUG  
  printer = &print; 
#endif
}

/*#ifdef DEBUG  
HardwareSerial* OXS_EAGLETREE::printer;
#endif*/


extern struct ONE_MEASUREMENT sport_rpm;
OXS_EAGLETREE *pThis = NULL;

// **************** Setup the EAGLETREE elogger *********************
void OXS_EAGLETREE::setup() 
{
	pThis = this;
#ifdef DEBUG  
	printer->println(F("OXS_EAGLETREE::setup"));
#endif
	memset(buffer, '-', sizeof(buffer));
	buffer[sizeof(buffer)-1] = 0;
	Wire.begin(0x3B); 	// Start the I2C Bus as Slave on address
	Wire.onReceive(OXS_EAGLETREE::receiveEvent);	// Attach a function to trigger when something is received.
}  //end of setup


/****************************************************************************/
boolean OXS_EAGLETREE::readSensor() 
{
//  0         1         2         3
//  01234567890123456789012345678901
//  PkV11.05Amp 0.0 mAH 0   TpA 23  

	static unsigned long lastCurrentMillis = millis() ; 
	static unsigned long milliTmp = millis() ;

	milliTmp = millis() ;
	if(  ( milliTmp - lastCurrentMillis) > 200 ) 
	{
		lastCurrentMillis =  milliTmp ;

		// Voltage float [3-7] 00.00
		int pos = 0;
		int point = 0;
		char s_data[5];
		for(int i = 3; i <= 7; i++)
		{
			if(isdigit(buffer[i]))
				s_data[pos++] = buffer[i];
			else if(buffer[i] == '.')
				point = pos;
			else if(buffer[i] == ' ')
				s_data[pos++] = '0';
		}
		s_data[pos] = 0;
		voltageData.mVoltCellTot = atol(s_data) * powint10(3 - (pos - point));
		voltageData.mVoltCellTot_Available = true;

		// Current float [11-15] 00.00
		pos = point = 0;
		for(int i = 11; i <= 15; i++)
		{
			if(isdigit(buffer[i]))
				s_data[pos++] = buffer[i];
			else if(buffer[i] == '.')
				point = pos;
			else if(buffer[i] == ' ')
				s_data[pos++] = '0';
		}
		s_data[pos] = 0;
		currentData.milliAmps.value = atol(s_data) * powint10(3 - (pos - point));
		currentData.milliAmps.available = true;

		// Capacity int [19-23] 
		pos = 0;
		for(int i = 19; i <= 23; i++)
		{
			if(isdigit(buffer[i]))
				s_data[pos++] = buffer[i];
			else if(buffer[i] == ' ' && pos > 0)
				break;
		}
		s_data[pos] = 0;
		/*currentData.consumedMilliAmps*/sport_rpm.value = atol(s_data);
		/*currentData.consumedMilliAmps*/sport_rpm.available = true;

#ifdef DEBUG  
		printer->print(buffer);
		printer->print("| mV = ");
		printer->print(voltageData.mVoltCellTot);
		printer->print(" mA = ");
		printer->print(currentData.milliAmps.value);
		printer->print(" mAH = ");
		printer->print(/*currentData.consumedMilliAmps*/sport_rpm.value);
		printer->println(' ');
#endif
	}
} // end of readSensor

///////////////////////////////////////////////////////////////////////////////
void OXS_EAGLETREE::receiveEvent(int bytes)
{
	static uint8_t pos = 0;
	static enum I2C_COMMAND {I2cNone, I2cCtrl, I2cData} I2cCommand;

	while(Wire.available() > 0)
	{
		int s = Wire.read();
		if(s == 0x00)
		{
			I2cCommand = I2cCtrl;
		}
		else if(s == 0x40)
		{
			I2cCommand = I2cData;
		}
		else
		{
			if(I2cCommand == I2cCtrl)
			{
				if((s & 0xC0) == 0x80)
					pos = (s & 0x1F);
				else if((s & 0xC0) == 0xC0)
					pos = 16 + (s & 0x1F);
				else
					memset(pThis->buffer, ':', sizeof(buffer)-1);
			}
			else if(I2cCommand == I2cData && pos < sizeof(buffer)-1)
			{
				pThis->buffer[pos++] = (s & 0x7F);
			}

			I2cCommand = I2cNone;
		}		
	}
}


///////////////////////////////////////////////////////////////////////////////
uint16_t OXS_EAGLETREE::powint10(uint16_t exponent)
{
   uint16_t product = 1;
   while (exponent--)
      product *= 10;
   return product;
}

#endif // end of #if defined(EAGLETREE_CONNECTED) && (EAGLETREE_CONNECTED == YES )
