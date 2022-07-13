#include "../hardware/sensors.h"

extern uint8_t modebusbuffer2[8];
extern uint8_t *modebusbuffer_2;
extern uint8_t modebusbufferRead[8];
extern uint8_t *modebusbuffer_read;

SENSOR sensor;
int16_t Distance;

void init_sensor()
{
    sensor.average = 0x00;
    sensor.send_signal = send_signal;
    sensor.get_distance = get_distance;
    sensor.get_average = get_average;
    sensor.Change_address = Change_address;
    sensor.creat_sendArray=creat_sendArray;
}
void send_signal(uint8_t ad , uint16_t ad_re)
{
    modebus03_solve( ad , ad_re, modebusbuffer_2 );
    Serial3.write(modebusbuffer2 , 8);
}

int16_t get_distance()
{ 
    Serial3.readBytes(modebusbufferRead, 8);   
    Distance = modebusbuffer_readint16 (modebusbuffer_read);
    return Distance;
}

int16_t get_average(int16_t a , int16_t b)
{
    sensor.average = (float)a/2 +(float) b/2;
    return sensor.average;
}

void Change_address()
{
     modebus06_solve( 0x05 , 0x0200 , DYP_AD, modebusbuffer_2 );
     Serial3.write(modebusbuffer2 , 8);
}

uint8_t* creat_sendArray(int16_t data1,int16_t data2)
{
  uint8_t sendArray[4];
  sendArray[0]=data1>>8;
  sendArray[1]=data1&0xFF;
  sendArray[2]=data2>>8;
  sendArray[3]=data2&0xFF;
  return sendArray;
}
