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
float LinTab[][3] = {
	{ 1,	     0,    0 },
	{ 2.5,	   78,   0 },
	{ 20.8,    1000, 0 },
	{ 108.15,	   5000, 0 },
	{ FLT_MAX, 0   , 0 },
};

///////////////////////////////////////////////////////////////////////////////
float GetCurrent(float ad_val)
{
	int i = 0;
	while (true)
	{
		if (ad_val < LinTab[++i][0] || LinTab[i][0] == FLT_MAX)
			break;
	}

	while (LinTab[(--i) + 1][0] == FLT_MAX)
		;

	float Y = LinTab[i][1] + (ad_val - LinTab[i][0]) * LinTab[i][2];

	if (Y < 0)
		Y = 0;

	return Y;
}

///////////////////////////////////////////////////////////////////////////////
void OXS_CURRENT::setCurrentTab(uint8_t pos, float current)
{
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

// **************** Setup the Current sensor *********************
void OXS_CURRENT::setupCurrent( )
 {
  uint16_t tempRef ; 
  //float currentDivider = 1.0 ;
#ifdef USE_INTERNAL_REFERENCE   
  analogReference(INTERNAL) ;
#elif defined(USE_EXTERNAL_REFERENCE)
    analogReference(EXTERNAL) ;
#endif
#if defined(USE_INTERNAL_REFERENCE) && defined(REFERENCE_VOLTAGE) && REFERENCE_VOLTAGE < 2000
  tempRef = REFERENCE_VOLTAGE  ;
#elif defined(USE_INTERNAL_REFERENCE) && defined(REFERENCE_VOLTAGE)
  #error REFERENCE_VOLTAGE must be less than 2000 when USE_INTERNAL_REFERENCE is defined
#elif defined(USE_EXTERNAL_REFERENCE)
#ifndef REFERENCE_VOLTAGE
  #error REFERENCE_VOLTAGE must be defined when USE_EXTERNAL_REFERENCE is defined
#else
  tempRef = REFERENCE_VOLTAGE  ;
#endif
#elif defined(USE_INTERNAL_REFERENCE)
  tempRef = 1100 ;
#elif defined(REFERENCE_VOLTAGE) && REFERENCE_VOLTAGE > 2000
  tempRef = REFERENCE_VOLTAGE  ;
#elif defined(REFERENCE_VOLTAGE)
  #error REFERENCE_VOLTAGE must be greater than 2000 when USE_INTERNAL_REFERENCE is not defined
#else 
  tempRef = 5000 ;
#endif  
/*#if defined(RESISTOR_TO_GROUND_FOR_CURRENT) && defined(RESISTOR_TO_CURRENT_SENSOR)
  if ( RESISTOR_TO_GROUND_FOR_CURRENT > 0 && RESISTOR_TO_CURRENT_SENSOR > 0) {
    currentDivider = 1.0 * (RESISTOR_TO_GROUND_FOR_CURRENT + RESISTOR_TO_CURRENT_SENSOR ) / RESISTOR_TO_GROUND_FOR_CURRENT ;
  }
#endif*/ 
	for(int i = 0; i <= 3; i++)
	{
		LinTab[i][2] = (LinTab[i+1][1] - LinTab[i][1]) / (LinTab[i+1][0] - LinTab[i][0]);
		printer->print("AD = ");
		printer->print(LinTab[i][0], 3);
		printer->print(", I = ");
		printer->print(LinTab[i][1], 3);
		printer->print(", X = ");
		printer->println(LinTab[i][2], 3);
	}


  float value;
  
  EEPROM_readAnything(EE_CURRENT_MVOLT_AT_ZERO_AMP, value);
  offsetCurrentSteps =  /*1023.0 * */value/* / tempRef*/ * 1000.0;
  
  EEPROM_readAnything(EE_CURRENT_SCALE_CURRENT, value);
  mAmpScale =  value; 

  EEPROM_readAnything(EE_CURRENT_MVOLT_PER_AMP, value);
  // mAmpPerStep =   (tempRef / 1.023) / (value * mAmpScale); // orig
//     mAmpPerStep =   (value * mAmpScale) / (tempRef / 1.023); 
  mAmpPerStep =   (value * mAmpScale);

  currentData.milliAmps.available = false;
  currentData.consumedMilliAmps.available = false;
//  currentData.sumCurrent = 0 ;
  filtered = 0.0;
  resetValues();
#ifdef DEBUG  
  printer->print("Current sensor on pin:");
  printer->print(_pinCurrent);
  printer->print(", Reference voltage:");
  printer->print(tempRef);
  printer->print(", Offset for current:");
  printer->print(offsetCurrentSteps);
  printer->print(", mAmp per step:");
  printer->println(mAmpPerStep);
//  printer->print(" milli=");  
//  printer->println(millis());
#endif
  
}


// **************** Read the Current sensor *********************
#if defined(ARDUINO_MEASURES_A_CURRENT) && (ARDUINO_MEASURES_A_CURRENT == YES)
void OXS_CURRENT::readSensor() 
{
  static int cnt = 0;
//  static int cntMAmp =0;
  static unsigned long lastCurrentMillis = millis() ; 
//  static unsigned long UpdateMs=0;
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
  
  filtered = filtered * 0.98 + analogRead(_pinCurrent) * 0.02;
  
  cnt++ ;
  //milliTmp = millis() ;
  if(  ( milliTmp - lastCurrentMillis) > 200 ) 
  {   
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
      printer->print("filtered = ");
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
  }  
}
#endif // end of readSensor

void OXS_CURRENT::resetValues(){
  currentData.consumedMilliAmps.value=0;
  floatConsumedMilliAmps=0;
}


#endif // end of #if defined( ARDUINO_MEASURES_A_CURRENT) && (ARDUINO_MEASURES_A_CURRENT == YES)
