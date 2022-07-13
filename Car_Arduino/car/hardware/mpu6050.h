#ifndef MPU6050_H
#define MPU6050_H

#include <Arduino.h>
#include <Wire.h>

#define MPUAD 0x68
#define MPUPM1 0x6B//Power Management
#define GYRO_CONFIG 0x1B
#define ACCEL_CONFIG 0x1C
#define ACCEL_OUTRE_START 0x3B
#define GYRO_OUTRE_START 0x43
#define LED_PIN 13
typedef struct _DATA_FLOAT_
{
    float x;
    float y;
    float z;
}d_float;
typedef struct _DATA_INT16_
{
    int16_t x;
    int16_t y;
    int16_t z;
}d_int16;


typedef struct _TRANS_
{
    d_float origin;
    d_int16 average;
    d_float history;
    d_float offset;
    d_int16 quiet;
}Trans;

typedef struct _MPU_
{
  Trans accel;
  Trans gyro;  
  float pitch;
  float gyro_z;
}MPU;

extern MPU mpu_sensor;

void init_LED(void);
void init_mpu(void);
void mpu_getdata(void);
void mpu_setzero(void);
void mpu_calculation(void);
void mpu_print(void);
void mpu_update(void);
void first_order_Filter(float Accel,float Gyro);
void Kalman_Filter(float Accel,float Gyro);


#endif
