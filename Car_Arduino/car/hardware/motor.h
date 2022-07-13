#ifndef MOTOR_H
#define MOTOR_H

#include "Arduino.h"
#include "../modebus/modebus.h"

#define Motor0 motor[0]
#define Motor1 motor[1]
#define Motor2 motor[2]
#define Motor3 motor[3]

#define RS485_Buadrate 115200
#define VOLTAGE_REG 0x0038//电源输入电压
/*速度控制寄存器地址*/
#define STOP_REG 0x0040//停止
#define PWM_REG 0x0042//设定占空比,PWM调速
#define TS_REG 0x0043//通过换向频率来设定速度闭环控制目标速度
#define PWMUPT_REG 0x0050//占空比调速加速缓冲时间
#define PWMDOWNT_REG 0x0051//占空比调速减速缓冲时间
#define SPEEDUPA_REG 0x0052//速度闭环控制、位置闭环控制加速加速度
#define SPEEDDOWNA_REG 0x0053//速度闭环控制、位置闭环控制减速加速度
#define POSITIONFRE_REG 0x0044//位置闭环换向频率
#define POSITIONSTATE_REG 0x0045//位置闭环控制类型
#define POSITIONTARGET_HREG 0x0046//位置闭环目标位置高半字
#define POSITIONTARGET_LREG 0x0047//位置闭环目标位置低半字

/*与轮毂电机相关的参数配置寄存器地址*/
#define MSPEEDUPA_REG 0x0062//速度闭环控制、位置闭环控制最大加速加速度
#define MSPEEDDOWNA_REG 0x0064//速度闭环控制、位置闭环控制最大减速加速度

#define READNAME_REG 0x0000//设备标识
#define READPWMFRE_REG 0x000B//PWM频率
#define READPWM_REG 0x0020//实时PWM
#define READCURRENT_REG 0x0021//实时电流
#define READTSF_REG 0x0022//实时换向频率（转速）
#define READSPEED_REG 0x0034//电机转速
#define READTEMP_REG 0x0037//内部（驱动电路）温度
#define READVOL_REG 0x0038//电源电压


#define MOVING 0x03
#define NORMAL_STOP 0x00
#define EMERGENCY_STOP 0x01
#define FREE_STOP 0x02

typedef struct _motor_
{
  uint8_t address;
  int16_t speed;
}Motor;;
int16_t set_speed(Motor *p, int16_t speed);
int16_t set_pidspeed(Motor *p, int16_t speed);
int16_t stop_car(Motor *p, int16_t mode);
int16_t lock_car(Motor *p);
int16_t set_speed(Motor *p, int16_t speed);
int16_t set_pidspeed(Motor *p, int16_t speed);
int16_t stop_car(Motor *p, int16_t mode);


extern Motor motor[4];

#endif
