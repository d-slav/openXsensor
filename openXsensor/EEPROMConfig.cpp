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
float value = 0.0;
int32_t i_value = 1;
char buffer[15];
int8_t buf_cnt;

extern OXS_VOLTAGE oXs_Voltage;
extern OXS_CURRENT oXs_Current;

/*
	v - zapis integer value
	V - zapis float value
	r - read integer value
	R - read float value
	w - zapis integer value
	W - zapis float value
	I - init ( 0 = napeti, 1 = proud )
	C - zapise float value do registru a spusti mereni proudu pro nastavenou hodnotu proudu
		V[skutecny proud]; C[pozicev tabulce proudu];
*/

///////////////////////////////////////////////////////////////////////////////////////////////////
void EEPROMConfig_char(int c)
{
	switch(c)
	{
		//case 'r':
		case 'R':
		//case 'w':
		case 'W':
		//case 'v':
		case 'V':
		case 'I':
			cmd = c;
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
		case 'E':		
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
					Serial.println(value, 7);
					break;
					
				case 'W':
					reg = atoi(buffer);
					EEPROM_writeAnything(reg, value);
					break;
					
				/*case 'v':
					i_value = atoi(buffer);
					Serial.print(F("set value = "));
					Serial.println(value);*/
					
				case 'V':
					value = atof(buffer);
					Serial.print(F("set value = "));
					Serial.println(value);
					break;
					
				case 'I':
					reg = atoi(buffer);
					switch(reg)
					{
					case 0: oXs_Voltage.setupVoltage(); break;
					case 1: oXs_Current.setupCurrent(); break;
					}
					break;
			}
		
		break;
	}
	
	
	
}

///////////////////////////////////////////////////////////////////////////////////////////////////
