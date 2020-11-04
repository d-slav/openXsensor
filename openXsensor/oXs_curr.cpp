#include <float.h>
#include "oXs_curr.h"
#include "EEPROMAnything.h"
#include "EEPROMConfig.h"

#ifdef DEBUG
#define DEBUGCURRENT
#endif

extern unsigned long micros( void ) ;
extern unsigned long millis( void ) ;
extern void delay(unsigned long ms) ;

float iir(float NewSample);

///////////////////////////////////////////////////////////////////////////////
float LinTab[][3] = {
	{ 2,	   0, 0 },
	{ 5,	 300, 0 },
	{ 16,	1000, 0 },
	{ 400, 20000, 0 },
};

#define TAB_ROWS (sizeof(LinTab) / sizeof(LinTab[0]))

///////////////////////////////////////////////////////////////////////////////
float GetCurrent(float ad_val)
{
	int i = 0;
	while (ad_val > LinTab[i][0] && i < (TAB_ROWS-1)) i++;
	if (i > 0) i--;
	float Y = LinTab[i][1] + (ad_val - LinTab[i][0]) * LinTab[i][2];
	return (Y >= 0) ? Y : 0;
}

///////////////////////////////////////////////////////////////////////////////
#ifdef DEBUG  
OXS_CURRENT::OXS_CURRENT(uint8_t pinCurrent, HardwareSerial &print)
#else
OXS_CURRENT::OXS_CURRENT(uint8_t pinCurrent)
#endif
{
  // constructor
#ifdef DEBUG  
  printer = &print; //operate on the address of print
#endif
  _pinCurrent=pinCurrent;
  pinMode(pinCurrent,INPUT); 
}

///////////////////////////////////////////////////////////////////////////////////////////////////
void OXS_CURRENT::setupCurrent( )
{
	for(int i = 0; i < TAB_ROWS; i++)
	{
		uint8_t pos = i * (EE_CURRENT_AD_VALUE_1 - EE_CURRENT_AD_VALUE_0);
		float value;
		EEPROM_readAnything(EE_CURRENT_AD_VALUE_0 + pos, value);
		LinTab[i][0] = value;
		EEPROM_readAnything(EE_CURRENT_CURRENT_0 + pos, value); 
		LinTab[i][1] = value;
	  
		if(i >= 1)
			  LinTab[i-1][2] = (LinTab[i][1] - LinTab[i-1][1]) / (LinTab[i][0] - LinTab[i-1][0]);
	}
	
	for(int i = 0; i < TAB_ROWS; i++)
	{
		printer->print("Pos = ");
		printer->print(i);
		printer->print(", AD = ");
		printer->print(LinTab[i][0], 3);
		printer->print(", I = ");
		printer->print(LinTab[i][1], 3);
		printer->print(", X = ");
		printer->println(LinTab[i][2], 3);
	}
	
	//ad_value_cnt = 0;

	currentData.milliAmps.available = false;
	currentData.consumedMilliAmps.available = false;

	filtered = 0.0;
	TempCorrection - 0.0;

	currentData.consumedMilliAmps.value=0;
	floatConsumedMilliAmps=0;
}

#define TEMP_CORRECTION 0.009						// cas korekce (vetsi cislo = mensi cas)
#define TEMP_CORR_FCOEF (3.8E-5 * TEMP_CORRECTION)	// velikost korekce

///////////////////////////////////////////////////////////////////////////////////////////////////
void OXS_CURRENT::readSensor() 
{
	static unsigned long lastCurrentMillis = millis(); 
	unsigned long milliTmp = millis() ;

	ADMUX = _BV(REFS1) | _BV(REFS0) | _BV(MUX3) | _BV(MUX2) | _BV(MUX1) | _BV(MUX0); // binary = 11 00 1111 (11 = use internal VRef as max, 1111 = measure ground level)
	delayMicroseconds(200); // Wait for Vref to settle 
	ADCSRA |= _BV(ADSC); // Start conversion
	while (bit_is_set(ADCSRA, ADSC)); // wait that conversion is done


	analogRead(_pinCurrent) ; // make a first read to let ADCMux to set up
	delayMicroseconds(200) ; // wait to be sure

	float f1 = analogRead(_pinCurrent);
	AD_curr = iir(f1);
  
	if(  ( milliTmp - lastCurrentMillis) > 200) 
	{
		TempCorrection = TempCorrection * (1.0-TEMP_CORRECTION) + (AD_curr * AD_curr * TEMP_CORR_FCOEF);
		filtered = GetCurrent(AD_curr - TempCorrection);
		currentData.milliAmps.value = filtered;
		currentData.milliAmps.available = true ;

		floatConsumedMilliAmps += ((float) currentData.milliAmps.value) * (milliTmp - lastCurrentMillis ) / 3600.0 /1000.0 ;   // Mike , is this ok when millis() overrun????
		currentData.consumedMilliAmps.value = (int32_t) floatConsumedMilliAmps;// / 10 ;
		currentData.consumedMilliAmps.available = true ;
		lastCurrentMillis =  milliTmp ;
#ifdef DEBUGCURRENT
		//printer->print("T = "); printer->println(GetTemp());
		//printer->print("ad_value_cnt = ");
		//printer->print(ad_value_cnt);

		//printer->print(" ,ad_value = ");
		//printer->print(ad_value / (100 - ad_value_cnt));
		//printer->print(", filtered = ");
		//printer->print(filtered);

		//printer->print((uint16_t)(filtered * 100));
		//printer->print(",");
		//printer->print((uint16_t)(f2 * 100));
		//printer->print(",");
		//printer->print((uint16_t)(f1 * 1000));
		//printer->print(", filtered_cnt = ");
		//printer->print(filtered_cnt);
		//printer->print(" average current =  ");
		//printer->print(currentData.milliAmps.value);
		//printer->print(", Consumed milliAmph =  ");
		//printer->print(currentData.consumedMilliAmps.value);
		//printer->println("");
#endif
	}  
}

///////////////////////////////////////////////////////////////////////////////////////////////////
