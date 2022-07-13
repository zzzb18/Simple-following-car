#ifndef SENSORS_H
#define SENSORS_H

#include "../modebus/modebus.h"

#define DYP_AD 0x01
#define Buffer_Size 8
#define Sensor_Buadrate 9600
 
/*与超声波模块相关的寄存器，超声波模块从机地址为0x05*/
#define DYP01 0x0106//01
#define DYP02 0x0107//02
#define DYP03 0x0108//03
#define DYP04 0x0109//04

void init_sensor();
void send_signal( uint8_t ad , uint16_t ad_re );
int16_t get_distance();
int16_t get_average( int16_t a , int16_t b );
void Change_address();
uint8_t* creat_sendArray(int16_t data1,int16_t data2);

struct SENSOR
{
   int16_t distance1;
   int16_t distance2;
   int16_t average;
   void (*send_signal)( uint8_t ad , uint16_t ad_re );
   int16_t (*get_distance)();
   int16_t (*get_average)( int16_t a , int16_t b );
   void (*Change_address)();
   uint8_t* (*creat_sendArray)(int16_t data1,int16_t data2);
};

#endif
