#ifndef OXS_CURRENT_h
#define OXS_CURRENT_h

#include <Arduino.h>
#include "oXs_config_basic.h"
#include "oXs_config_advanced.h"
#include "oXs_config_macros.h"

///////////////////////////////////////////////////////////////////////////////////////////////////
struct CURRENTDATA {
	struct ONE_MEASUREMENT milliAmps;       	// in mA
	struct ONE_MEASUREMENT consumedMilliAmps;   // in mAh
};

///////////////////////////////////////////////////////////////////////////////////////////////////
class OXS_CURRENT {
public:
	OXS_CURRENT(uint8_t pinCurrent) ;

	CURRENTDATA currentData ;
	void setupCurrent();
	void readSensor();

	float AD_curr;
	float TempCorrection;

private:
	byte _pinCurrent;
	float floatConsumedMilliAmps; // in mA
	float filtered;
	
	float TempCorrTime;		// cas korekce (vetsi cislo = mensi cas)
	float TempCorrKoef;		// velikost korekce
};

///////////////////////////////////////////////////////////////////////////////////////////////////

#endif // OXS_CURRENT_h
