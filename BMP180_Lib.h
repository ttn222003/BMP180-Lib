#include "main.h"
#include "math.h"
extern I2C_HandleTypeDef hi2c1;

void BMP180_Init();
long UN_Temp();
long UN_Pressure(uint8_t mode);
long True_Temp();
long True_Pressure(uint8_t mode);
