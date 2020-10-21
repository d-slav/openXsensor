#ifndef OXS_CURRENT_h
#define OXS_CURRENT_h

#include <Arduino.h>
#include "oXs_config_basic.h"
#include "oXs_config_advanced.h"
#include "oXs_config_macros.h"

struct CURRENTDATA {
  struct ONE_MEASUREMENT milliAmps;       // in mA
  struct ONE_MEASUREMENT consumedMilliAmps;       // in mA
  //int32_t consumedMilliAmps; // in mA
  //bool consumedMilliAmpsAvailable;
};



class OXS_CURRENT {
public:
#ifdef DEBUG  
  OXS_CURRENT(uint8_t pinCurrent, HardwareSerial &print);
#else
  OXS_CURRENT(uint8_t pinCurrent) ;
#endif
  CURRENTDATA currentData ;
  void setupCurrent() ;
  void readSensor();
  void resetValues();
  void setCurrentTabStart(uint8_t pos, float current);
  void setCurrentTabFinish();

private:
#ifdef DEBUG  
  HardwareSerial* printer;
#endif
  byte _pinCurrent;
//  float offsetCurrentSteps ;
//  float mAmpPerStep ; 
//  float mAmpScale;
  float floatConsumedMilliAmps; // in mA
  int32_t sumCurrent ;
  float filtered;
  uint32_t filtered_cnt;
  
  uint8_t ad_value_cnt;
  float ad_value;
  uint8_t set_current_for_pos;
  float set_current_for_current;
};

#endif // OXS_CURRENT_h
