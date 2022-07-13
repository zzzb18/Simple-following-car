#ifndef SYSTEM_H
#define SYSTEM_H


#include "../hardware/motor.h"
#include "../hardware/mpu6050.h"
#include "../hardware/sensors.h"
#include "../menu/emenu.h"
#include "../modebus/modebus.h"
#include "protocol.h"

#define SERIAL_RX_BUFFER_SIZE 512 //修改串口发送缓冲区大小为512
#define SERIAL_TX_BUFFER_SIZE 512 //修改串口接收缓冲区大小为512

#define IMU_STK_SIZE 256
#define MENU_STK_SIZE 512
#define SPEED_STK_SIZE 512
#define SERIAL_STK_SIZE 512
#define SENSOR_STK_SIZE 256
#define START_STK_SIZE 256

#define IMU_TASK_PRIO 3
#define MENU_TASK_PRIO 3
#define SENSOR_TASK_PRIO 4
#define SPEED_TASK_PRIO 4
#define SERIAL_TASK_PRIO 4
#define START_TASK_PRIO 4

#endif
