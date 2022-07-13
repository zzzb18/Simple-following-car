/**************************************************************************
    @brief          : 小车串口通讯和控制相关函数，小车核心代码
    @author         : RXF
    @copyright      :
    @version        : Version1.1
    @note           : 进行了重构，简化了操作逻辑
    @history        :
***************************************************************************/
#include "protocol.h"

RECEIVE_DATA Receive_data;
SEND_DATA Send_data;
CMD_LIST Cmd_list;
CONTROLLER Controller;
IBusBM IBus;

extern int16_t distance1;
extern int16_t distance2;

/***************************************************************************
    @brief         : 在航模控制模式下，读取遥控器的值
    @param[in]     :
    @param[out]    :
    @return        : 无
    @others        :
***************************************************************************/
void remote_read(void)
{
  int16_t xval = IBus.readChannel(1);
  int16_t yval = IBus.readChannel(3);
  xval = abs(xval - 1500) >= MINUS_VALUE ? xval : 1500;
  yval = abs(yval - 1500) >= MINUS_VALUE ? yval : 1500;
  int16_t speed = map(xval, 1000, 2000, -MAXSPEED_FRE, MAXSPEED_FRE);
  int16_t speedminus = map(yval, 1000, 2000, -MAXSPEED_MINUS_FRE, MAXSPEED_MINUS_FRE);
  if (speedminus == 0)
  {
    if ((speed != 0) && (distance1>300)&&(distance2>300) )
    {
      Controller.stop_flag = MOVING;
      Controller.target_leftspeed = speed;
      Controller.target_rightspeed = speed;
    }
    else
    {
      Controller.stop_flag = EMERGENCY_STOP;
      Controller.target_leftspeed = 0;
      Controller.target_rightspeed = 0;
    }
  }
  else 
  {
    Controller.stop_flag = MOVING;
    Controller.target_leftspeed = speed + speedminus;
    Controller.target_rightspeed = speed - speedminus;
  }

}

/***************************************************************************
    @brief         : 切换控制模式
    @param[in]     : controlmode 对应的模式
    @param[out]    :
    @return        : 无
    @others        :
***************************************************************************/
void set_controlmode(uint8_t controlmode)
{
  switch (controlmode)
  {
    case REMOTEMODE:
      {
        IBus.begin(IBUS_SERIAL);
        INPUT_SERIAL.begin(9600,INPUT_SETTINGS);
        Controller.control_mode = REMOTEMODE;
        break;
      }
    case UARTMODE:
      {
        Serial2.begin(115200);//龙芯用串口2通讯,
        Serial3.begin(9600);
        Controller.control_mode = UARTMODE;
        break;
      }
    default:
      break;
  }

}

/***************************************************************************
    @brief         : 初始化速度控制器，会自动配置控制器
    @param[in]     : 无
    @param[out]    :
    @return        : 无
    @others        :
***************************************************************************/
void init_controller(void)
{
  Controller.stop_flag = EMERGENCY_STOP;
  Controller.target_leftspeed = 0;
  Controller.target_rightspeed = 0;
  init_cmds();
}


/**************************************************************************
    @brief         : 运行控制器
    @param[in]     : 无
    @param[out]    :
    @return        : 无
    @others        :
***************************************************************************/
void run_controller(void)
{
  
  switch (Controller.control_mode)
  {
    case REMOTEMODE:
      {
        remote_read();
        break;
      }
    case UARTMODE:
      {
        uartdata_read();
        break;
      }
    default:
      break;
  }
  if (Controller.stop_flag != MOVING)
  {
    
    stop_car(&Motor0, Controller.stop_flag);
    stop_car(&Motor1, Controller.stop_flag);
    stop_car(&Motor2, Controller.stop_flag);
    stop_car(&Motor3, Controller.stop_flag);

  }
  else
  {
    set_pidspeed(&Motor0, Controller.target_leftspeed);
    set_pidspeed(&Motor1, Controller.target_leftspeed);
    set_pidspeed(&Motor2, Controller.target_rightspeed);
    set_pidspeed(&Motor3, Controller.target_rightspeed);
  }
  
}


/**************************************************************************
    @brief         : 初始化命令列表，可以在此函数注册相关指令
    @param[in]     : 无
    @param[out]    :
    @return        : 无
    @others        :
***************************************************************************/
static void init_cmds(void)//在此函数中定义各项命令的结构体
{
  pinMode(LIGHT,OUTPUT);
  Cmd_list.cmd_num = 3;
  Cmd_list.cmdlist[0] = {Cmd_Setspeed, C_setspeed};
  Cmd_list.cmdlist[1] = {Cmd_Stopcar, C_stopcar};
  Cmd_list.cmdlist[2] = {Cmd_Lightset, C_lightset};


}
/**************************************************************************
    @brief         : 生成回送的数据帧
    @param[in]     : 无
    @param[out]    :
    @return        : 无
    @others        :
***************************************************************************/
void creat_sendframe(void)
{
  Send_data.Buffer[0] = Send_data.Return_data.Header;
  Send_data.Buffer[1] = Send_data.Return_data.Cmd_type;
  Send_data.Buffer[2] = Send_data.Return_data.Length;
  memcpy(&Send_data.Buffer[3], Send_data.Return_data.Databuffer, Send_data.Return_data.Length);
  Send_data.Return_data.Checksum = send_checksum(Send_data.Buffer, Send_data.Return_data.Length + 3);
  Send_data.Buffer[3 + Send_data.Return_data.Length] = Send_data.Return_data.Checksum;
  /*INPUT_SERIAL.write(Send_data.Buffer, 4 + Send_data.Return_data.Length);*/
}

/**************************************************************************
    @brief         : 指令0x01对应的操作函数，用于设定速度
    @param[in]     : 无
    @param[out]    :
    @return        : 无
    @others        :
***************************************************************************/

static void C_setspeed(void)
{
  int16_t leftspeed, rightspeed;
  leftspeed = (Receive_data.Cmd_data.Databuffer[0] << 8) + Receive_data.Cmd_data.Databuffer[1];
  rightspeed = (Receive_data.Cmd_data.Databuffer[2] << 8) + Receive_data.Cmd_data.Databuffer[3];

  leftspeed = abs(leftspeed) >= MAXSPEED_FRE ? MAXSPEED_FRE : leftspeed;
  rightspeed = abs(rightspeed) >= MAXSPEED_FRE ? MAXSPEED_FRE : rightspeed;

  DEBUG_SERIAL.print("leftspeed:"); DEBUG_SERIAL.print(leftspeed);
  DEBUG_SERIAL.print("rightspeed:"); DEBUG_SERIAL.println(rightspeed);
  if ((leftspeed == 0) && (rightspeed == 0))
  {
    Controller.stop_flag = FREE_STOP;
    Controller.target_leftspeed = leftspeed;
    Controller.target_rightspeed = rightspeed;
  }
  else if ((distance1<=300) || (distance2<=300))
     {
       Controller.stop_flag = EMERGENCY_STOP;
       Controller.target_leftspeed = 0;
       Controller.target_rightspeed = 0;
     }
      else if ((distance1>300)&&(distance2>300))
      {
        Controller.stop_flag = MOVING;
        Controller.target_leftspeed = leftspeed;
        Controller.target_rightspeed = rightspeed;
      }
     
  Send_data.Return_data.Header = FRAMEHEADER;
  Send_data.Return_data.Cmd_type = Cmd_Setspeed;
  Send_data.Return_data.Length = 4;
  Send_data.Return_data.Databuffer[0] = Receive_data.Cmd_data.Databuffer[0];
  Send_data.Return_data.Databuffer[1] = Receive_data.Cmd_data.Databuffer[1];
  Send_data.Return_data.Databuffer[2] = Receive_data.Cmd_data.Databuffer[2];
  Send_data.Return_data.Databuffer[3] = Receive_data.Cmd_data.Databuffer[3];


  creat_sendframe();



}




/**************************************************************************
    @brief         : 指令0x02对应的操作函数，用于停车
    @param[in]     : 无
    @param[out]    :
    @return        : 无
    @others        :
***************************************************************************/
static void C_stopcar(void)
{
  uint8_t inputmode;
  inputmode = Receive_data.Cmd_data.Databuffer[0];
  Controller.stop_flag = inputmode;
  Controller.target_leftspeed = 0;
  Controller.target_rightspeed = 0;

  Send_data.Return_data.Header = FRAMEHEADER;
  Send_data.Return_data.Cmd_type = Cmd_Stopcar;
  Send_data.Return_data.Length = 1;
  Send_data.Return_data.Databuffer[0] = Receive_data.Cmd_data.Databuffer[0];
  
  Send_data.Return_data.Checksum = send_checksum(Send_data.Return_data.Databuffer, Send_data.Return_data.Length + 3);

  creat_sendframe();

}

/**************************************************************************
    @brief         : 指令0x03对应的操作函数，用于开关灯
    @param[in]     : 无
    @param[out]    :
    @return        : 无
    @others        :
***************************************************************************/
static void C_lightset(void)
{
  uint8_t state=Receive_data.Cmd_data.Databuffer[0];
  if(state==LIGHTON||state==LIGHTOFF)
  {
    digitalWrite(LIGHT,state);
    Send_data.Return_data.Header = FRAMEHEADER;
    Send_data.Return_data.Cmd_type = Cmd_Stopcar;
    Send_data.Return_data.Length = 2;
    Send_data.Return_data.Databuffer[0] = Receive_data.Cmd_data.Databuffer[0];
    
    Send_data.Return_data.Checksum = send_checksum(Send_data.Return_data.Databuffer, Send_data.Return_data.Length + 3);

    creat_sendframe();
  }
  
  
}
/**************************************************************************
    @brief         : 指令表匹配函数，会自动遍历指令表执行对应的函数，内部调用
    @param[in]     : uint8_t inputcmd 指令编号
    @param[out]    :
    @return        : 无
    @others        :
***************************************************************************/
static void cmd_match(uint8_t inputcmd)
{
  for (int i = 0; i < Cmd_list.cmd_num; i++)
  {
    if (Cmd_list.cmdlist[i].Cmdnumber == inputcmd)
    {
      Cmd_list.cmdlist[i].cmd_callback();
    }
  }
}



/**************************************************************************
    @brief         : 串口接收函数，根据指令表自动解析
    @param[in]     : 无
    @param[out]    : 无
    @return        :
    @others        :
***************************************************************************/
void uartdata_read(void)
{
  uint8_t num = Serial2.available();
  uint8_t *ptr;
  if (0 <= num)
  {
    Serial2.readBytes(Receive_data.Buffer, num);
    for(int j=0 ; j<num ; j++)
       Serial.print(Receive_data.Buffer[j]);
    Serial.print ("\n");

    for (int i = 0; i < num; i++)
    {
      if (Receive_data.Buffer[i] == FRAMEHEADER)
      {
        DEBUG_SERIAL.println("ojbk1");
        ptr = &Receive_data.Buffer[i];
        Receive_data.Cmd_data.Header = *(ptr);
        Receive_data.Cmd_data.Cmd_type = *(ptr + 1);
        Receive_data.Cmd_data.Length = *(ptr + 2);
        if (receive_checksum(ptr, Receive_data.Cmd_data.Length + 4) == 0)
        {
          DEBUG_SERIAL.println("ojbk2");
          memcpy( Receive_data.Cmd_data.Databuffer, ptr + 3, Receive_data.Cmd_data.Length);
          cmd_match(Receive_data.Cmd_data.Cmd_type);
        }
      }
    }
  }
}

/**************************************************************************
    @brief         : 发送校验和计算函数，内部调用
    @param[in]     : uint8_t *str：供校验的字符串指针 int str_length：字符串长度
    @param[out]    :
    @return        : uint8_t CheckSum_Value 计算的校验值
    @others        :
***************************************************************************/
static uint8_t send_checksum(uint8_t *str, int str_length)
{
  uint8_t CheckSum_Value = 0;
  int i = 0;
  for (i = 0; i < str_length; i++)
  {
    CheckSum_Value += str[i];
  }
  CheckSum_Value = ~CheckSum_Value;
  return CheckSum_Value + 1;
}

/**************************************************************************
    @brief         : 接收校验和计算函数，内部调用
    @param[in]     : uint8_t *str：供校验的字符串指针 int str_length：字符串长度
    @param[out]    :
    @return        : uint8_t CheckSum_Value 返回计算的结果，如果是0代表正确传输
    @others        :
***************************************************************************/
static uint8_t receive_checksum(uint8_t *str, int str_length)
{
  uint8_t CheckSum_Value = 0;
  int i = 0;
  for (i = 0; i < str_length; i++)
  {
    CheckSum_Value += str[i];
  }
  return CheckSum_Value;
}
