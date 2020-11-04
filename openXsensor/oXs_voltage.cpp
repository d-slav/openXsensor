#include "oXs_voltage.h"
#include "EEPROMAnything.h"
#include "EEPROMConfig.h"
 
#ifdef DEBUG
//#define DEBUGNEWVALUE
//#define DEBUGDELAY
//#define DEBUGCELLCALCULATION
//#define DEBUGLOWVOLTAGE
//#define DEBUGNTC
#endif


extern unsigned long micros( void ) ;
extern unsigned long millis( void ) ;
extern void delay(unsigned long ms) ;


#ifdef DEBUG
OXS_VOLTAGE::OXS_VOLTAGE(HardwareSerial &print) 
#else
OXS_VOLTAGE::OXS_VOLTAGE(uint8_t x) 
#endif
{
#ifdef DEBUG  
  printer = &print; //operate on the address of print   
#endif
}

#define tempRef 1100

///////////////////////////////////////////////////////////////////////////////////////////////////
void OXS_VOLTAGE::setupVoltage( void ) 
{
	analogReference(INTERNAL) ;

	uint8_t tempPin[NUM_VOLTAGES] = { PIN_VOLTAGE };

	float tempResistorToGround;// = { RESISTOR_TO_GROUND } ;
	float tempResistorToVoltage;// = { RESISTOR_TO_VOLTAGE } ;
	float tempOffsetVoltage;// = { OFFSET_VOLTAGE} ;
	float tempScaleVoltage;// = { SCALE_VOLTAGE }  ;

	EEPROM_readAnything(EE_VOLTAGE_RESISTOR_TO_GROUND, tempResistorToGround);
	EEPROM_readAnything(EE_VOLTAGE_RESISTOR_TO_VOLTAGE, tempResistorToVoltage);
	EEPROM_readAnything(EE_VOLTAGE_OFFSET_VOLTAGE, tempOffsetVoltage);
	EEPROM_readAnything(EE_VOLTAGE_SCALE_VOLTAGE, tempScaleVoltage);

	voltageData.atLeastOneVolt = false ;
	voltageData.mVoltPin[0] =  tempPin[0] ;
	voltageData.atLeastOneVolt = true ;
	
    voltageData.offset[0] = tempOffsetVoltage * 1000 ;
    voltageData.mVoltPerStep[0] = tempRef / 1023.0 * ( tempResistorToGround + tempResistorToVoltage ) / tempResistorToGround  * tempScaleVoltage;
    voltageData.sumVoltage[0] = 0 ;
    voltageData.mVolt[0].available = false ; 
#ifdef DEBUG  
    printer->println("Voltage:");
	printer->print("ResistorToGround = "); 	printer->println(tempResistorToGround);
    printer->print("ResistorToVoltage = "); printer->println(tempResistorToVoltage);
    printer->print("VoltPerStep = "); 		printer->println( voltageData.mVoltPerStep[0] );
    printer->print("Offset = "); 			printer->println( voltageData.offset[0] );
#endif
}
#define voltageNr 0
#define cntVolt 0

///////////////////////////////////////////////////////////////////////////////////////////////////
void OXS_VOLTAGE::readSensor() {

    if (voltageData.atLeastOneVolt) 
	{ 
#ifdef DEBUGDELAY
        long milliVoltBegin = micros() ;
#endif
        voltageData.sumVoltage[voltageNr] += readVoltage(voltageNr) ;   // read voltage 
/*#ifdef DEBUGDELAY
        milliVoltBegin = milliVoltBegin - micros() ;
        printer->print("VoltageNr ");
        printer->print(voltageNr);
        printer->print(" in ");
        printer->println(milliVoltBegin);
#endif*/

        voltageNrIncrease();                          // Find next voltage to be read; if overlap, calculate average for each voltage
    }
}      

///////////////////////////////////////////////////////////////////////////////////////////////////
void OXS_VOLTAGE::voltageNrIncrease() 
{
	static int cnt = 0;
	static unsigned long lastVoltMillis = millis() ;

	//voltageNr = 0 ;
	cnt++;
	if(millis() > ( lastVoltMillis + 500) )
	{
		voltageData.mVolt[cntVolt].value = round( ((float) voltageData.sumVoltage[cntVolt] * voltageData.mVoltPerStep[cntVolt]   / cnt ) + voltageData.offset[cntVolt] );
		voltageData.mVolt[cntVolt].available = true ;
		voltageData.sumVoltage[cntVolt] = 0 ;
		
#ifdef DEBUGNEWVALUE
		printer->print("At ");
		printer->print(millis());
		printer->print(" Cnt = ");
		printer->print(cnt);
		printer->print(" mVolt ");
		printer->print(cntVolt);
		printer->print(" = ");
		printer->println( voltageData.mVolt[cntVolt].value );
#endif

        cnt=0;
        lastVoltMillis = millis() ;
        
	}
}

///////////////////////////////////////////////////////////////////////////////////////////////////////////
int OXS_VOLTAGE::readVoltage( int value ) 
{ 
	// value is the index in an aray giving the pin to read
	//******** First discharge the capacitor of ADCMux to ground in order to avoid that measurement from another pin has an impact on this measurement  
	ADMUX = _BV(REFS1) | _BV(REFS0) | _BV(MUX3) | _BV(MUX2) | _BV(MUX1) | _BV(MUX0); // binary = 11 00 1111 (11 = use internal VRef as max, 1111 = measure ground level)
	delayMicroseconds(200); // Wait for Vref to settle 
	ADCSRA |= _BV(ADSC); // Start conversion
	while (bit_is_set(ADCSRA,ADSC)); // wait that conversion is done ; this takes 13 cycles of 125 khz (clock set in oXs_general.ccp so 104 usec

	analogRead( voltageData.mVoltPin[value]); // read the value from the sensor ; it requires about 120 usec 
	// discard the first measurement
	delayMicroseconds(100); // Wait for ADMux to settle 
	return analogRead(voltageData.mVoltPin[value]); // use the second measurement ; it requires about 120 usec
}
