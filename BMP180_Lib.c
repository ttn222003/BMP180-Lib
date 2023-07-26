#include "main.h"
#include "math.h"

extern I2C_HandleTypeDef hi2c1;
#define BMP180_Address	0XEE

/***********************************/
short AC1;
short AC2;
short AC3;
unsigned short AC4;
unsigned short AC5;
unsigned short AC6;
short B1;
short B2;
short MB;
short MC;
short MD;
/***********************************/
/*** Variable used to calculate ****/
long X1;
long X2;
long B5;
long B6;
long X3;
long B3;
unsigned long B4;
unsigned long B7;
long p;
/***********************************/
void BMP180_Init()
{
	uint8_t dataRead[22];
	uint16_t startMem = 0XAA;
	HAL_I2C_Mem_Read(&hi2c1,BMP180_Address,startMem,1,dataRead,22,HAL_MAX_DELAY);
	AC1 = (dataRead[0]<<8 | dataRead[1]);
	AC2 = (dataRead[2]<<8 | dataRead[3]);
	AC3 = (dataRead[4]<<8 | dataRead[5]);
	AC4 = (dataRead[6]<<8 | dataRead[7]);
	AC5 = (dataRead[8]<<8 | dataRead[9]);
	AC6 = (dataRead[10]<<8 | dataRead[11]);
	B1 = (dataRead[12]<<8 | dataRead[13]);
	B2 = (dataRead[14]<<8 | dataRead[15]);
	MB = (dataRead[16]<<8 | dataRead[17]);
	MC = (dataRead[18]<<8 | dataRead[19]);
	MD = (dataRead[20]<<8 | dataRead[21]);
}

long UN_Temp()
{
	uint8_t memAdd = 0XF4;
	uint8_t dataWrite = 0X2E;
	uint8_t startMem = 0XF6;
	uint8_t dataRead[2];
	HAL_I2C_Mem_Write(&hi2c1,BMP180_Address,memAdd,1,&dataWrite,1,HAL_MAX_DELAY);
	HAL_Delay(5);
	HAL_I2C_Mem_Read(&hi2c1,BMP180_Address,startMem,1,dataRead,2,HAL_MAX_DELAY);
	return ((dataRead[0]<<8) + dataRead[1]);
}

/*******************************
There are 4 mode to read pressure value of BMP180:
0: Ultra low power
1: Standard
2: High resolution
3: Ultra high resolution
You can choose one of those mode
*******************************/
long UN_Pressure(uint8_t mode)
{
	uint8_t memAdd = 0XF4;
	uint8_t dataWrite = 0X34 + (mode << 6);
	uint8_t startMem = 0XF6;
	uint8_t dataRead[3];
	HAL_I2C_Mem_Write(&hi2c1,BMP180_Address,memAdd,1,&dataWrite,1,HAL_MAX_DELAY);
	HAL_Delay(5);
	HAL_I2C_Mem_Read(&hi2c1,BMP180_Address,startMem,1,dataRead,3,HAL_MAX_DELAY);
	return ((dataRead[0]<<16) + (dataRead[1]<<8) + (dataRead[2])) >> (8 - mode);
}

long True_Temp()
{
	X1 = (UN_Temp() - AC6) * AC5 / pow(2.0,15);
	X2 = MC * pow(2.0,11) / (X1 + MD);
	B5 = X1 + X2;
	return (((B5 + 8) / pow(2.0,4)) / 10);
}

/*************** Use mode which used at UN_Pressure function ******************/
long True_Pressure(uint8_t mode)
{
	X1 = (UN_Temp() - AC6) * AC5 / pow(2.0,15);
	X2 = MC * pow(2.0,11) / (X1 + MD);
	B5 = X1 + X2;
	B6 = B5 - 4000;
	X1 = (B2 * (B6 * B6 / pow(2.0,12))) / pow(2.0,11);
	X2 = AC2 * B6 / pow(2.0,11);
	X3 = X1 + X2;
	B3 = ((((long)AC1 * 4 + X3) << mode) + 2) / 4;
	X1 = AC3 * B6  / pow(2.0,19);
	X2 = (B1 * (B6 * B6 / pow(2.0,12))) / pow(2.0,16);
	X3 = ((X1 + X2) + 2) / pow(2.0,2);
	B4 = AC4 * (unsigned long)(X3 + 32768) / pow(2.0,15);
	B7 = ((unsigned long)UN_Pressure(mode) - B3) * (50000 >> mode);
	if(B7	< 0X80000000)	p = (B7 * 2) / B4;
	else p = (B7 / B4) * 2;
	X1 = (p / pow(2.0,8)) * (p / pow(2.0,8));
	X1 = (X1 * 3038) / pow(2.0,16);
	X2 = (-7357 * p) / pow(2.0,16);
	p = p + (X1 +X2 + 3791) * pow(2.0,4);
	return p;
}
