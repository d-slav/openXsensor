#ifndef OXS_EAGLETREE_h
#define OXS_EAGLETREE_h

#include <Arduino.h>
#include "oXs_config_basic.h"
#include "oXs_config_advanced.h"
#include "oXs_config_macros.h"
#include "I2C.h"
#include "oXs_curr.h"
#include "oXs_voltage.h"


class OXS_EAGLETREE 
{
public:
#ifdef DEBUG  
	OXS_EAGLETREE(HardwareSerial &print);
#else
	OXS_EAGLETREE(uint8_t x);
#endif
  
	void setup();
	boolean  readSensor( void ); //return true if an averaged has just been calculated  
	struct CURRENTDATA currentData;
	struct VOLTAGEDATA voltageData;
//  float floatConsumedMilliAmps ; // in mA
  
private: 
/*#ifdef DEBUG
	static HardwareSerial* printer;
#endif*/
	static void receiveEvent(int bytes);
	uint16_t powint10(uint16_t exponent);
	char buffer[33];

}; // end class OXS_EAGLETREE


#endif // OXS_EAGLETREE


