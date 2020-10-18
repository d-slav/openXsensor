#ifndef _EEPROM_CONFIG_H_
#define _EEPROM_CONFIG_H_

#include <inttypes.h>

#define EE_VOLTAGE_RESISTOR_TO_GROUND	0
#define EE_VOLTAGE_RESISTOR_TO_VOLTAGE	4
#define EE_VOLTAGE_OFFSET_VOLTAGE		8
#define EE_VOLTAGE_SCALE_VOLTAGE		12

#define EE_CURRENT_MVOLT_AT_ZERO_AMP	16
#define EE_CURRENT_MVOLT_PER_AMP		20
#define EE_CURRENT_SCALE_CURRENT		24


//////////////////////////////////////////////////////////
void EEPROMConfig_char(int c);



#endif // _EEPROM_CONFIG_H_
