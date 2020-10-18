#include <string.h>
#include "EEPROMConfig.h"
#include "EEPROMAnything.h"
#include "oXs_voltage.h"
#include "oXs_curr.h"

// Rreg; - precte registr reg
// Vval; - nastavi promennou value na val
// Wreg; - zapise hodnoty value do registru reg

///////////////////////////////////////////////////////////////////////////////////////////////////
char cmd = 0;
uint8_t reg;
float value;
char buffer[15];
int8_t buf_cnt;

extern OXS_VOLTAGE oXs_Voltage;
extern OXS_CURRENT oXs_Current;

///////////////////////////////////////////////////////////////////////////////////////////////////
void EEPROMConfig_char(int c)
{
	switch(c)
	{
		case 'r':
		case 'R':
		case 'w':
		case 'W':
		case 'v':
		case 'V':
			cmd = toupper(c);
			buf_cnt = 0;
			memset(buffer, 0, sizeof(buffer));
			break;
		
		case '+':
		case '-':
		case '0':
		case '1':
		case '2':
		case '3':
		case '4':
		case '5':
		case '6':
		case '7':
		case '8':
		case '9':
		case '.':
		case ',':		
			if(buf_cnt < (sizeof(buffer)-1))
			{
				if(c == ',')
					buffer[buf_cnt++] = '.';
				else
					buffer[buf_cnt++] = (char)c;
			}
			break;
		
		case ';':
			switch(cmd)
			{
				case 'R':
					reg = atoi(buffer);
					EEPROM_readAnything(reg, value);
					Serial.print(F("reg "));
					Serial.print(reg);
					Serial.print(F(" = "));
					Serial.println(value);
					break;
					
				case 'W':
					reg = atoi(buffer);
					EEPROM_writeAnything(reg, value);
					if(reg == EE_VOLTAGE_RESISTOR_TO_GROUND	|| reg == EE_VOLTAGE_RESISTOR_TO_VOLTAGE || reg == EE_VOLTAGE_OFFSET_VOLTAGE || reg == EE_VOLTAGE_SCALE_VOLTAGE)
						oXs_Voltage.setupVoltage(); 
					else if(reg == EE_CURRENT_MVOLT_AT_ZERO_AMP || reg == EE_CURRENT_MVOLT_PER_AMP || reg == EE_CURRENT_SCALE_CURRENT)
						oXs_Current.setupCurrent();
					break;
					
				case 'V':
					value = atof(buffer);
					Serial.print(F("set value = "));
					Serial.println(value);
					break;
			}
		
		break;
	}
	
	
	
}

///////////////////////////////////////////////////////////////////////////////////////////////////
