#ifndef _EEPROM_CONFIG_H_
#define _EEPROM_CONFIG_H_

#include <inttypes.h>

#define EE_VOLTAGE_RESISTOR_TO_GROUND	0
#define EE_VOLTAGE_RESISTOR_TO_VOLTAGE	4
#define EE_VOLTAGE_OFFSET_VOLTAGE		8
#define EE_VOLTAGE_SCALE_VOLTAGE		12

//#define EE_CURRENT_COUNT				16 // nepouziva se
#define EE_CURRENT_AD_VALUE_0			20
#define EE_CURRENT_CURRENT_0			24
#define EE_CURRENT_AD_VALUE_1			28
#define EE_CURRENT_CURRENT_1			32
#define EE_CURRENT_AD_VALUE_2			36
#define EE_CURRENT_CURRENT_2			40
#define EE_CURRENT_AD_VALUE_3			44
#define EE_CURRENT_CURRENT_3			48
#define EE_TEMP_CORRECTION				52
#define EE_TEMP_CORR_COEF				56



//////////////////////////////////////////////////////////
void EEPROMConfig_char(int c);



#endif // _EEPROM_CONFIG_H_
