#include <float.h>
#include "oXs_curr.h"
#include "EEPROMAnything.h"
#include "EEPROMConfig.h"

#if defined( ARDUINO_MEASURES_A_CURRENT) && (ARDUINO_MEASURES_A_CURRENT == YES)

#ifdef DEBUG
#define DEBUGCURRENT
#endif

extern unsigned long micros( void ) ;
extern unsigned long millis( void ) ;
extern void delay(unsigned long ms) ;

///////////////////////////////////////////////////////////////////////////////
float LinTab[5][3] = {
	{ 0,	     0, 0 },
	{ 1,	     0, 0 },
	{ 2.5,	    78, 0 },
	{ 20.8,   1000, 0 },
	{ 108.15, 5000, 0 },
	//{ FLT_MAX, 0   , 0 },
};

///////////////////////////////////////////////////////////////////////////////
float GetCurrent(float ad_val)
{
	int i = -1;
	while (i <= 2)
	{
		if (ad_val < LinTab[++i][0])
			break;
	}

	float Y = LinTab[i][1] + (ad_val - LinTab[i][0]) * LinTab[i][2];

	if (Y < 0)		Y = 0;

	return Y;
}

///////////////////////////////////////////////////////////////////////////////
void OXS_CURRENT::setCurrentTabStart(uint8_t pos, float current)
{
	ad_value = 0;
	ad_value_cnt = 100;
	set_current_for_pos = pos;
	set_current_for_current = current;
}

///////////////////////////////////////////////////////////////////////////////
void OXS_CURRENT::setCurrentTabFinish()
{
  uint8_t pos = set_current_for_pos * (EE_CURRENT_AD_VALUE_1 - EE_CURRENT_AD_VALUE_0);
	EEPROM_writeAnything(EE_CURRENT_AD_VALUE_0 + pos , ad_value / 100);
	EEPROM_writeAnything(EE_CURRENT_CURRENT_0 + pos, set_current_for_current);
  
    printer->print("Write pos = ");
    printer->print(set_current_for_pos);
    printer->print(", AD = ");
    printer->print(ad_value / 100, 3);
    printer->print(", I = ");
    printer->println(set_current_for_current, 3);
      
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
// **************** Setup the Current sensor *********************
void OXS_CURRENT::setupCurrent( )
{
	for(int i = 0; i <= 4; i++)
	{
    uint8_t pos = i * (EE_CURRENT_AD_VALUE_1 - EE_CURRENT_AD_VALUE_0);
    float value;
    EEPROM_readAnything(EE_CURRENT_AD_VALUE_0 + pos, value);
    LinTab[i][0] = value;
    EEPROM_readAnything(EE_CURRENT_CURRENT_0 + pos, value); 
    LinTab[i][1] = value;
  
    if(i > 0)
		  LinTab[i-1][2] = (LinTab[i][1] - LinTab[i-1][1]) / (LinTab[i][0] - LinTab[i-1][0]);
	}
 for(int i = 0; i <= 4; i++)
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
	ad_value_cnt = 0;

	currentData.milliAmps.available = false;
	currentData.consumedMilliAmps.available = false;

	filtered = 0.0;
  filtered_cnt = 0;
	resetValues();
}

///////////////////////////////////////////////////////////////////////////////////////////////////
// **************** Read the Current sensor *********************
#if defined(ARDUINO_MEASURES_A_CURRENT) && (ARDUINO_MEASURES_A_CURRENT == YES)
void OXS_CURRENT::readSensor() 
{
	static int cnt = 0;
	static unsigned long lastCurrentMillis = millis() ; 
	/*static*/ unsigned long milliTmp = millis() ;
#ifdef USE_INTERNAL_REFERENCE
	ADMUX = _BV(REFS1) | _BV(REFS0) | _BV(MUX3) | _BV(MUX2) | _BV(MUX1) | _BV(MUX0); // binary = 11 00 1111 (11 = use internal VRef as max, 1111 = measure ground level)
#elif defined(USE_EXTERNAL_REFERENCE)
	ADMUX =  _BV(MUX3) | _BV(MUX2) | _BV(MUX1) | _BV(MUX0);                          // binary = 00 00 1111 (00 = use external VRef as max, 1111 = measure ground level)
#else
	ADMUX =  _BV(REFS0) | _BV(MUX3) | _BV(MUX2) | _BV(MUX1) | _BV(MUX0);             // binary = 01 00 1111 (01 = use Vcc as max, 1111 = measure ground level)
#endif
	delayMicroseconds(200); // Wait for Vref to settle 
	ADCSRA |= _BV(ADSC); // Start conversion
	while (bit_is_set(ADCSRA,ADSC)); // wait that conversion is done


  analogRead(_pinCurrent) ; // make a first read to let ADCMux to set up
  delayMicroseconds(200) ; // wait to be sure
  //currentData.sumCurrent += analogRead(_pinCurrent) ; 
  //sumCurrent += analogRead(_pinCurrent) ; 

  filtered += analogRead(_pinCurrent);
  filtered_cnt++;
  //filtered = filtered * 0.98 + analogRead(_pinCurrent) * 0.02;
  
  cnt++ ;
  //milliTmp = millis() ;
  if(  ( milliTmp - lastCurrentMillis) > 200 && filtered_cnt > 0 ) 
  {
    filtered /= filtered_cnt;
    
  	if(ad_value_cnt > 0)
  	{
  		ad_value += filtered;
  		if(--ad_value_cnt == 0)
  			setCurrentTabFinish();
  	}
      // calculate average once per 200 millisec
	  //currentData.milliAmps.value = filtered * mAmpPerStep + offsetCurrentSteps;
      //if (currentData.milliAmps.value < 0) currentData.milliAmps.value = 0 ;
	  currentData.milliAmps.value = GetCurrent(filtered);

    
	    currentData.milliAmps.available = true ;
      
      sumCurrent = 0;
      floatConsumedMilliAmps += ((float) currentData.milliAmps.value) * (milliTmp - lastCurrentMillis ) / 3600.0 /1000.0 ;   // Mike , is this ok when millis() overrun????
      currentData.consumedMilliAmps.value = (int32_t) floatConsumedMilliAmps;// / 10 ;
      currentData.consumedMilliAmps.available = true ;
      lastCurrentMillis =  milliTmp ;
#ifdef DEBUGCURRENT
      printer->print("ad_value_cnt = ");
      printer->print(ad_value_cnt);
      
      printer->print(" ,ad_value = ");
      printer->print(ad_value / (100 - ad_value_cnt));
      printer->print(", filtered = ");
      printer->print(filtered);
//      printer->print(", Cnt = ");
//      printer->print(cnt);
      printer->print(" average current =  ");
      printer->print(currentData.milliAmps.value);
//      printer->print(", Consumed milliAmph =  ");
//      printer->print(currentData.consumedMilliAmps.value);
      printer->println("");
#endif
      cnt = 0;

    filtered = 0;   
    filtered_cnt = 0;
      
  }  
}
#endif // end of readSensor

///////////////////////////////////////////////////////////////////////////////////////////////////
void OXS_CURRENT::resetValues()
{
	currentData.consumedMilliAmps.value=0;
	floatConsumedMilliAmps=0;
}


#endif // end of #if defined( ARDUINO_MEASURES_A_CURRENT) && (ARDUINO_MEASURES_A_CURRENT == YES)
