/*
 * MPU6050.h
 *
 *  Created on: May 18, 2025
 *      Author: capting
 */

#ifndef INC_MPU6050_H_
#define INC_MPU6050_H_
#include <stdint.h>

void MPU6050_WriteReg(uint8_t RegAdd,uint8_t DataAdd);
uint8_t MPU6050_ReadReg(uint8_t RegAdd);
void MPU6050_Init(void);
void MPU6050_GetData(int16_t* Accx, int16_t* Accy, int16_t* Accz,
                     int16_t* Gyrox, int16_t* Gyroy, int16_t* Gyroz);

#endif /* INC_MPU6050_H_ */
