#ifndef OXS_VOLTAGE_h
#define OXS_VOLTAGE_h

#include "Arduino.h"
#include "oXs_config_basic.h"
#include "oXs_config_advanced.h"
#include "oXs_config_macros.h"

#define NUM_VOLTAGES 1

struct VOLTAGEDATA {
//  bool available;    // to remove afterward
  uint16_t vrefMilliVolts;          // in mV the internal measured voltage Reference ; to remove afterward

  struct ONE_MEASUREMENT mVolt[NUM_VOLTAGES] ;  // in mV 
//  int32_t mVolt[NUM_VOLTAGES] ;             // in mV 
//  bool mVoltAvailable[NUM_VOLTAGES] ;
  
  byte mVoltPin[NUM_VOLTAGES] ;            // Arduino pin number to use to read each voltage (See hardware setting in oXs_config.h)  
  int offset[NUM_VOLTAGES] ;               // offset to apply while converting ADC to millivolt (See setting in oXs_config.h)  
  float mVoltPerStep[NUM_VOLTAGES] ;       // rate to apply while converting ADC to millivolt (See setting in oXs_config.h)  

  bool atLeastOneVolt ;         // true if there is at least one voltage to measure (added because otherwise a while in cpp never end)
  
  int32_t sumVoltage[NUM_VOLTAGES] ;       // used to calculate average voltage     

//  uint8_t maxNumberOfCells ;    // used to fill in the max number of cells
//  uint32_t mVoltCell[NUM_VOLTAGES] ;
//  bool mVoltCell_Available [NUM_VOLTAGES];
//  uint32_t mVoltCellMin ;
//  bool mVoltCellMin_Available ;
//   uint32_t mVoltCellTot ;
 // bool mVoltCellTot_Available ;
 
#if defined(PROTOCOL) && ( (PROTOCOL == FRSKY_SPORT) || ( PROTOCOL == FRSKY_HUB ) || (PROTOCOL == FRSKY_SPORT_HUB ) ) //if Frsky protocol is used  
//struct ONE_MEASUREMENT mVoltCell_1_2 ; 
//struct ONE_MEASUREMENT mVoltCell_3_4 ;  
//struct ONE_MEASUREMENT mVoltCell_5_6 ;  
#endif
};


class OXS_VOLTAGE {
  public:
#ifdef DEBUG  
    OXS_VOLTAGE(HardwareSerial &print);
#else
    OXS_VOLTAGE( uint8_t x );
#endif
    VOLTAGEDATA voltageData ;
	void setupVoltage( void );
	void readSensor();
	void resetValues();
  void convertNtcVoltToTemp (int32_t &voltage ) ;
    
  private:
#ifdef DEBUG  
     HardwareSerial* printer;
#endif
     int readVoltage( int value) ;  // read the voltage from the sensor specify by value
     void voltageNrIncrease() ; 
     uint32_t calculateCell(int32_t V0 , int32_t V1 , int32_t V2 , uint8_t cellId , uint8_t  maxNumberOfCells) ;  
};

extern bool lowVoltage ;

#endif
