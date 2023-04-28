#ifndef DS18B20_H_
#define DS18B20_H_

//
// Podstawowe polecenia ukladu DS18B20
//
#define cmd_DS18x20_SearchROM		0xF0
#define cmd_DS18x20_ReadROM			0x33
#define cmd_DS18x20_MatchROM		0x55
#define cmd_DS18x20_SkipROM			0xCC
#define cmd_DS18x20_AlarmSearch		0xEC
#define cmd_DS18x20_ConvertT		0x44
#define cmd_DS18x20_ReadScratchpad	0xBE
#define cmd_DS18x20_WrireScratchpad	0x4E
#define cmd_DS18x20_CopyScratchpad	0x48
#define cmd_DS18x20_RecallEE		0xB8
#define cmd_DS18x20_ReadPowerSupply	0xB4

#endif //DS18B20_H_