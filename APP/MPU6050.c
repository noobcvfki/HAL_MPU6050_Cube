/*
 * MPU6050.c
 *
 *  Created on: May 18, 2025
 *      Author: capting
 */
#include "i2c_hal.h"
#include "main.h"

#define MPU6050_ADD 0x68

#define	MPU6050_SMPLRT_DIV		0x19
#define	MPU6050_CONFIG			0x1A
#define	MPU6050_GYRO_CONFIG		0x1B
#define	MPU6050_ACCEL_CONFIG	0x1C
#define	MPU6050_ACCEL_XOUT_H	0x3B
#define	MPU6050_ACCEL_XOUT_L	0x3C
#define	MPU6050_ACCEL_YOUT_H	0x3D
#define	MPU6050_ACCEL_YOUT_L	0x3E
#define	MPU6050_ACCEL_ZOUT_H	0x3F
#define	MPU6050_ACCEL_ZOUT_L	0x40
#define	MPU6050_TEMP_OUT_H		0x41
#define	MPU6050_TEMP_OUT_L		0x42
#define	MPU6050_GYRO_XOUT_H		0x43
#define	MPU6050_GYRO_XOUT_L		0x44
#define	MPU6050_GYRO_YOUT_H		0x45
#define	MPU6050_GYRO_YOUT_L		0x46
#define	MPU6050_GYRO_ZOUT_H		0x47
#define	MPU6050_GYRO_ZOUT_L		0x48
#define	MPU6050_PWR_MGMT_1		0x6B
#define	MPU6050_PWR_MGMT_2		0x6C
#define	MPU6050_WHO_AM_I		0x75

void MPU6050_WriteReg(uint8_t RegAdd, uint8_t DataAdd)
{
    I2CStart();
    I2CSendByte(MPU6050_ADD);
    I2CWaitAck();
    I2CSendByte(RegAdd);
    I2CWaitAck();
    I2CSendByte(DataAdd);
    I2CWaitAck();
    I2CStop();
}

void MPU_Write_Len(uint8_t addr,uint8_t reg,uint8_t len,uint8_t *buf)
{
    uint8_t i;
    I2CStart();
    I2CSendByte(addr<<1|0x00);//发送器件地址+写命令
    I2CWaitAck();
    I2CSendByte(reg);	//写寄存器地址
    I2CWaitAck();		//等待应答
    for(i=0;i<len;i++)
    {
        I2CSendByte(buf[i]);	//发送数据
        I2CWaitAck();		//等待ACK
    }
    I2CStop();
}

uint8_t MPU6050_ReadReg(uint8_t RegAdd)
{
    uint8_t Data;
    I2CStart();
    I2CSendByte(MPU6050_ADD);
    I2CWaitAck();
    I2CSendByte(RegAdd);
    I2CWaitAck();

    I2CStart();
    I2CSendByte(MPU6050_ADD | 0x01);
    I2CWaitAck();
    Data = I2CReceiveByte();
    I2CSendNotAck();
    I2CStop();

    return Data;
}

void MPU_Read_Len(uint8_t addr,uint8_t reg,uint8_t len,uint8_t *buf)
{
    I2CStart();
    I2CSendByte((addr<<1)|0x00);//发送器件地址+写命令
    I2CWaitAck();	//等待应答
    I2CSendByte(reg);	//写寄存器地址
    I2CWaitAck();		//等待应答
    I2CStart();
    I2CSendByte((addr)<<1|0x01);//发送器件地址+读命令
    I2CWaitAck();		//等待应答
    for(int i=0;i<len;i++)
    {
        buf[i] = I2CReceiveByte();	//发送数据
        I2CWaitAck();		//等待ACK
    }
    I2CStop();	//产生一个停止条件
}

void MPU6050_Init(void)
{
    I2CInit();

    MPU6050_WriteReg(MPU6050_PWR_MGMT_1, 0x01); //电源管理寄存器1
    MPU6050_WriteReg(MPU6050_PWR_MGMT_2, 0x00);
    MPU6050_WriteReg(MPU6050_SMPLRT_DIV, 0x09);
    MPU6050_WriteReg(MPU6050_CONFIG, 0x06);
    MPU6050_WriteReg(MPU6050_GYRO_CONFIG, 0x18);
    MPU6050_WriteReg(MPU6050_ACCEL_CONFIG, 0x18);
}

void MPU6050_GetData(int16_t* Accx, int16_t* Accy, int16_t* Accz,
                     int16_t* Gyrox, int16_t* Gyroy, int16_t* Gyroz)
{
    uint8_t DataH = 0, DataL = 0;
    DataH = MPU6050_ReadReg(MPU6050_ACCEL_XOUT_H);
    DataL = MPU6050_ReadReg(MPU6050_ACCEL_XOUT_L);
    *Accx = (DataH << 8) | DataL;

    DataH = MPU6050_ReadReg(MPU6050_ACCEL_YOUT_H);
    DataL = MPU6050_ReadReg(MPU6050_ACCEL_YOUT_L);
    *Accy = (DataH << 8) | DataL;

    DataH = MPU6050_ReadReg(MPU6050_ACCEL_ZOUT_H);
    DataL = MPU6050_ReadReg(MPU6050_ACCEL_ZOUT_L);
    *Accz = (DataH << 8) | DataL;

    DataH = MPU6050_ReadReg(MPU6050_GYRO_XOUT_H);
    DataL = MPU6050_ReadReg(MPU6050_GYRO_XOUT_L);
    *Gyrox = (DataH << 8) | DataL;

    DataH = MPU6050_ReadReg(MPU6050_GYRO_YOUT_H);
    DataL = MPU6050_ReadReg(MPU6050_GYRO_YOUT_L);
    *Gyroy = (DataH << 8) | DataL;

    DataH = MPU6050_ReadReg(MPU6050_GYRO_ZOUT_H);
    DataL = MPU6050_ReadReg(MPU6050_GYRO_ZOUT_L);
    *Gyroz = (DataH << 8) | DataL;
}
