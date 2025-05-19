/**
 * CT117E-M4 / GPIO - I2C
*/

#include "i2c_hal.h"

#define DELAY_TIME	20

#define SDA_PIN GPIO_PIN_1
#define SDA_Port GPIOB

#define SCL_PIN GPIO_PIN_0
#define SCL_Port GPIOB

//
void SDA_Input_Mode()
{
    GPIO_InitTypeDef GPIO_InitStructure = {0};

    GPIO_InitStructure.Pin = SDA_PIN;
    GPIO_InitStructure.Mode = GPIO_MODE_INPUT;
    GPIO_InitStructure.Pull = GPIO_PULLUP;
    GPIO_InitStructure.Speed = GPIO_SPEED_FREQ_HIGH;
    HAL_GPIO_Init(SDA_Port, &GPIO_InitStructure);
}

//
void SDA_Output_Mode()
{
    GPIO_InitTypeDef GPIO_InitStructure = {0};

    GPIO_InitStructure.Pin = SDA_PIN;
    GPIO_InitStructure.Mode = GPIO_MODE_OUTPUT_OD;
    GPIO_InitStructure.Pull = GPIO_NOPULL;
    GPIO_InitStructure.Speed = GPIO_SPEED_FREQ_HIGH;
    HAL_GPIO_Init(SDA_Port, &GPIO_InitStructure);
}

//
void SDA_Output( uint16_t val )
{
    if ( val )
    {
    	SDA_Port->BSRR |= SDA_PIN;
    }
    else
    {
    	SDA_Port->BRR |= SDA_PIN;
    }
}

//
void SCL_Output( uint16_t val )
{
    if ( val )
    {
    	SCL_Port->BSRR |= SCL_PIN;
    }
    else
    {
    	SCL_Port->BRR |= SCL_PIN;
    }
}

//
uint8_t SDA_Input(void)
{
	if(HAL_GPIO_ReadPin(SDA_Port, SDA_PIN) == GPIO_PIN_SET){
		return 1;
	}else{
		return 0;
	}
}

//
static void delay1(unsigned int n)
{
    uint32_t i;
    for ( i = 0; i < n; ++i);
}

//
void I2CStart(void)
{
    SDA_Output(1);
    delay1(DELAY_TIME);
    SCL_Output(1);
    delay1(DELAY_TIME);
    SDA_Output(0);
    delay1(DELAY_TIME);
    SCL_Output(0);
    delay1(DELAY_TIME);
}

//
void I2CStop(void)
{
    SCL_Output(0);
    delay1(DELAY_TIME);
    SDA_Output(0);
    delay1(DELAY_TIME);
    SCL_Output(1);
    delay1(DELAY_TIME);
    SDA_Output(1);
    delay1(DELAY_TIME);

}

//
unsigned char I2CWaitAck(void)
{
    unsigned short cErrTime = 5;
    SDA_Input_Mode();
    delay1(DELAY_TIME);
    SCL_Output(1);
    delay1(DELAY_TIME);
    while(SDA_Input())
    {
        cErrTime--;
        delay1(DELAY_TIME);
        if (0 == cErrTime)
        {
            SDA_Output_Mode();
            I2CStop();
            return ERROR;
        }
    }
    SCL_Output(0);
    SDA_Output_Mode();
    delay1(DELAY_TIME);
    return SUCCESS;
}

//
void I2CSendAck(void)
{
    SDA_Output(0);
    delay1(DELAY_TIME);
    delay1(DELAY_TIME);
    SCL_Output(1);
    delay1(DELAY_TIME);
    SCL_Output(0);
    delay1(DELAY_TIME);

}

//
void I2CSendNotAck(void)
{
    SDA_Output(1);
    delay1(DELAY_TIME);
    delay1(DELAY_TIME);
    SCL_Output(1);
    delay1(DELAY_TIME);
    SCL_Output(0);
    delay1(DELAY_TIME);

}

//
void I2CSendByte(unsigned char cSendByte)
{
    unsigned char  i = 8;
    while (i--)
    {
        SCL_Output(0);
        delay1(DELAY_TIME);
        SDA_Output(cSendByte & 0x80);
        delay1(DELAY_TIME);
        cSendByte += cSendByte;
        delay1(DELAY_TIME);
        SCL_Output(1);
        delay1(DELAY_TIME);
    }
    SCL_Output(0);
    delay1(DELAY_TIME);
}

//
unsigned char I2CReceiveByte(void)
{
    unsigned char i = 8;
    unsigned char cR_Byte = 0;
    SDA_Input_Mode();
    while (i--)
    {
        cR_Byte += cR_Byte;
        SCL_Output(0);
        delay1(DELAY_TIME);
        delay1(DELAY_TIME);
        SCL_Output(1);
        delay1(DELAY_TIME);
        cR_Byte |=  SDA_Input();
    }
    SCL_Output(0);
    delay1(DELAY_TIME);
    SDA_Output_Mode();
    return cR_Byte;
}



//
void I2CInit(void)
{
    GPIO_InitTypeDef GPIO_InitStructure = {0};

    GPIO_InitStructure.Pin = SDA_PIN | SCL_PIN;
    GPIO_InitStructure.Mode = GPIO_MODE_OUTPUT_PP;
    GPIO_InitStructure.Pull = GPIO_PULLUP;
    GPIO_InitStructure.Speed = GPIO_SPEED_FREQ_HIGH;
    HAL_GPIO_Init(GPIOB, &GPIO_InitStructure);
}

void I2CSendLen(uint8_t addr, uint8_t reg,  uint16_t len, uint8_t *data)
{
    I2CStart();
    I2CSendByte(addr << 1 |0x00); //发送地址（写）
    I2CWaitAck();
    I2CSendByte(reg);//发送寄存器地址
    I2CWaitAck();
    for (int i = 0; i < len; i++)
    {
        I2CSendByte(data[i]);
        I2CWaitAck();
    }
    I2CStop();

}

void I2CReceiveLen(uint8_t addr , uint8_t reg, uint16_t len, uint8_t *data)
{
    I2CStart();
    I2CSendByte(addr<<1 | 0x00);//发送地址（写）
    I2CWaitAck();
    I2CSendByte(reg);//发送寄存器地址
    I2CWaitAck();

    I2CStart();
    I2CSendByte(addr<<1 | 0x01);//发送地址（读）
    I2CWaitAck();
    while (len--)
    {
        *data++ = I2CReceiveByte();
        if (len == 0) {
            I2CSendNotAck(); // 最后一个字节发送 NACK
        } else {
            I2CSendAck();    // 继续读下一个字节
        }
    }
    I2CStop();
}
