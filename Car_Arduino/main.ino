/**************************************************************************
   @brief          : 小车主程序，嵌入式大赛特供版
   @author         : RXF
   @copyright      :
   @version        : Version2.0
   @note           :
   @history        :1.针对协议进行了优化
                    2.采用了新的IMU程序和控制逻辑
                    3.新的工程结构
                    4.更新了电机程序
   @Last update    :2022.3.28
***************************************************************************/
#include "./src/controller/system.h"
#include <Arduino_FreeRTOS.h>
//#include <semphr.h>
//#include <queue.h>

int16_t distance1;
int16_t distance2;
uint8_t send_to_loongson[4];

TaskHandle_t StartTask_Handler;

void StartTask(void *pvParameters);
void Hardware_init();
void IMUTask(void *pvParameters);
void SENSORTask(void *pvParameters);
void MenuTask(void *pvParameters);
void SpeedTask(void *pvParameters);
void SerialTask(void *pvParameters);


void setup() {
  
  Change_address();//超声波更改从机地址
  
  Hardware_init();//硬件初始化
  
  
  while (menuflag)
  {
    match_key();
  }
  xTaskCreate(StartTask, "StartTask", START_STK_SIZE,
              NULL, START_TASK_PRIO, &StartTask_Handler);
  //创建开始任务
  
}



void loop() {
 
}

void Hardware_init()
{
  Serial3.begin(9600);
  Serial.begin(RS485_Buadrate);
  /*初始化与电机相连的串口Serial1*/
  Serial1.begin(RS485_Buadrate, SERIAL_8N1);
  Serial.println("Mega2560 is working now, config buadrate 115200bps");
  /*初始化屏幕*/
  oled_init();
  init_key();
  /*初始化陀螺仪*/
  init_mpu();
  mpu_setzero();
  init_controller();
  init_sensor();
  
}
void StartTask(void *pvParameters)
{
  xTaskCreate(IMUTask, "IMUTask", IMU_STK_SIZE, NULL, IMU_TASK_PRIO, NULL);
  xTaskCreate(SENSORTask, "SENSORTask", SENSOR_STK_SIZE, NULL, SENSOR_TASK_PRIO, NULL);
  xTaskCreate(MenuTask, "MenuTask", MENU_STK_SIZE, NULL, MENU_TASK_PRIO, NULL);
  xTaskCreate(SpeedTask, "SpeedTask", SPEED_STK_SIZE, NULL, SPEED_TASK_PRIO, NULL);
  xTaskCreate(SerialTask, "SerialTask", SERIAL_STK_SIZE, NULL, SERIAL_TASK_PRIO, NULL);

  vTaskDelete(StartTask_Handler);
}
void IMUTask(void *pvParameters)
{
  for (;;)
  {
    vTaskDelay(10 / portTICK_PERIOD_MS);//每隔10ms更新一次
    mpu_update();
  }
}
void SENSORTask(void *pvParameters)
{
  for (;;)
  {
    vTaskDelay(20 / portTICK_PERIOD_MS);//每隔30ms更新一次
    send_signal( DYP_AD , DYP01 );
    vTaskDelay(100 / portTICK_PERIOD_MS);
    distance1 = get_distance();
    send_signal( DYP_AD , DYP02 );
    vTaskDelay(100 / portTICK_PERIOD_MS);
    distance2 = get_distance();
    Serial.println(distance1);
    Serial.println(distance2);
    //send_to_loongson=creat_sendArray(distance1,distance2);
    //Serial2.write(send_to_loongson,4);
  }
}
void MenuTask(void *pvParameters)
{
  
  Serial.begin(RS485_Buadrate);
  Serial1.begin(RS485_Buadrate, SERIAL_8N1);
  Serial.println ("Mega2560 is working now, config buadrate 115200bps");
  for (;;)
  {
    //long starttime = millis();
    
    vTaskDelay(200 / portTICK_PERIOD_MS);//每隔200ms更新一次
    oled_showinfo();
    //Serial.println(millis() - starttime);
  }
}

void SpeedTask(void *pvParameters)
{
  for (;;)
  { 
    
    vTaskDelay(50 / portTICK_PERIOD_MS);//每隔50ms更新一次
    run_controller();
    if (((distance1<=300) && (distance1>0))||((distance2<=300) && (distance2>0)))
    {
      stop_car(&Motor0, EMERGENCY_STOP);
      stop_car(&Motor1, EMERGENCY_STOP);
      stop_car(&Motor2, EMERGENCY_STOP);
      stop_car(&Motor3, EMERGENCY_STOP);
    }    
  }
}

void SerialTask(void *pvParameters)
{
  for (;;)
  {
    vTaskDelay(50 / portTICK_PERIOD_MS);//每隔50ms更新一次
    
  }
}
