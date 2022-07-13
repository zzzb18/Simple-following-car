/**************************************************************************
    @brief          : 小车电机控制函数
    @author         : RXF
    @copyright      :
    @version        : Version0.2
    @note           : 借助C语言对点击对象进行了封装，以后可以封装成C++类
    @history        :
***************************************************************************/
#include "motor.h"
extern uint8_t *modebusbuffer_1;

Motor motor[4] =
{
  {0x01, 0x0000},
  {0x02, 0x0000},
  {0x03, 0x0000},
  {0x04, 0x0000},
};

/**************************************************************************
    @brief         : 电机PWM调速函数，开环控制
    @param[in]     : Motor *p为Motor结构体指针，speed为开环速度
                    （-1000~1000，数值乘以 0.1%为目标占空比）
    @param[out]    :
    @return        : speed
    @others        :
***************************************************************************/

int16_t set_speed(Motor *p, int16_t speed)
{
  modebus06_solve(p->address, PWM_REG, speed, modebusbuffer_1);
  Serial1.write(modebusbuffer1, 8);
  p->speed = speed;
  delay(2);
  return p->speed;
}

/**************************************************************************
    @brief         : 电机调速函数，闭环控制
    @param[in]     : Motor *p为Motor结构体指针，speed为闭环速度
                     （-32768~32767,数值乘以 0.1 为目标换向频率，单位为 Hz）
    @param[out]    :
    @return        : speed
    @others        :
***************************************************************************/

int16_t set_pidspeed(Motor *p, int16_t speed)
{
  modebus06_solve(p->address, TS_REG, speed,modebusbuffer_1);
  Serial1.write(modebusbuffer1, 8);
  p->speed = speed;
  delay(2);
  return p->speed;
}

/**************************************************************************
    @brief         : 电机制动
    @param[in]     : Motor *p为电机地址，mode为方式（正常制动，紧急制动，自由制动）
    @param[out]    :
    @return        : speed
    @others        :
***************************************************************************/


int16_t stop_car(Motor *p, int16_t mode)
{
  modebus06_solve(p->address, STOP_REG, mode, modebusbuffer_1);
  Serial1.write(modebusbuffer1, 8);
  p->speed = 0;
  delay(2);
  return p->speed;
}

/**************************************************************************
    @brief         : 电机自锁
    @param[in]     : Motor *p为电机地址
    @param[out]    :
    @return        : 
    @others        :
***************************************************************************/

int16_t lock_car(Motor *p)
{
  modebus06_solve(p->address, POSITIONTARGET_HREG, 0x00,modebusbuffer_1);
  Serial1.write(modebusbuffer1, 8);
  modebus06_solve(p->address, POSITIONTARGET_LREG, 0x00,modebusbuffer_1);
  Serial1.write(modebusbuffer1, 8);
  p->speed = 0;
  //delay(2);
  return p->speed;
}