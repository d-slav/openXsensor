#ifndef OXS_CONFIG_ADVANCED_h
#define OXS_CONFIG_ADVANCED_h

#define PIN_SERIALTX 4

//#define REFERENCE_VOLTAGE 3300
#define USE_INTERNAL_REFERENCE

#define PIN_VOLTAGE  1
#define RESISTOR_TO_GROUND 5.1
#define RESISTOR_TO_VOLTAGE 100.0
#define OFFSET_VOLTAGE 0.0
#define SCALE_VOLTAGE 1.000

#define PIN_CURRENTSENSOR 2
#define MVOLT_AT_ZERO_AMP 0.0
#define MVOLT_PER_AMP     25.0
//#define RESISTOR_TO_GROUND_FOR_CURRENT 0.0
//#define RESISTOR_TO_CURRENT_SENSOR     0.0

//#define         DATA_ID_VARIO  0x00
//#define         DATA_ID_FLVSS  0xA1
//#define         DATA_ID_FAS    0x22
//#define         DATA_ID_GPS    0x83
//#define         DATA_ID_RPM    0xE4
//#define         DATA_ID_ACC    0x67
//#define         DATA_ID_TX     0x0D

#define SECOND_BARO_SENSOR_USE     NO_BARO

#define SENSITIVITY_MIN 80
#define SENSITIVITY_MAX 300
#define SENSITIVITY_MIN_AT 100
#define SENSITIVITY_MAX_AT 1000

#define VARIOHYSTERESIS 5

#endif
