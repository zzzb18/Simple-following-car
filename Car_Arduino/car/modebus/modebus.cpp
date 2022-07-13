/**************************************************************************
    @brief          : 小车所用Modebus协议相关函数
    @author         : RXF
    @copyright      :
    @version        : Version0.1
    @note           : 仅提供03,06功能码生成数据帧功能
    @history        : 基于Modebus.ino改编
***************************************************************************/
#include "../modebus/modebus.h"
uint8_t modebusbuffer1[8] = {0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00};
uint8_t modebusbuffer2[8] = {0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00};
uint8_t modebusbufferRead[8];
uint8_t *modebusbuffer_1=modebusbuffer1;
uint8_t *modebusbuffer_2=modebusbuffer2;
uint8_t *modebusbuffer_read=modebusbufferRead;


/**************************************************************************
    @brief         : CRC校验函数
    @param[in]     : *pdata:需要进行校验的数组 plen： 需要进行校验的数组数据长度
    @param[out]    :
    @return        : uint_16 crc，为数组计算得到的16位crc校验值
    @others        : 内部调用，计算数据帧2字节校验位
***************************************************************************/
static uint16_t  CRC_check(uint8_t *pdata, uint32_t plen)
{
  uint16_t temp = 0xFFFF, crc = 0;
  for (int i = 0; i < plen; i++)
  {
    temp ^= (uint16_t)(*(pdata+i));
    for (int j = 0; j < 8; j++)
    {
      if (temp & 0x0001)//若最低位为1，右移一位后与0xA001相异或
      {
        temp = (temp >> 1) ^ 0xA001;
      }
      else
      {
        temp = temp >> 1;
      }
    }
  }
  crc = temp;
  return crc;
}

/**************************************************************************
    @brief         : ModebusRTU写单个寄存器，功能码为06
    @param[in]     : ad：Modebus通讯时从机地址 *参数ad_re：要写入的寄存器地址，两字节
                     参数value：要写入的值，两字节
    @param[out]    :
    @return        : 无
    @others        : 用于06功能码生成发送数据帧,注意使用时务必进行寄存器地址检验
***************************************************************************/
void modebus06_solve(uint8_t ad, uint16_t ad_re, uint16_t value, uint8_t *modebusbuffer)
{

  uint16_t crc = 0;
  uint8_t crcl = 0, crch = 0;
  *(modebusbuffer) = ad;
  *(modebusbuffer+1) = 0x06;
  *(modebusbuffer+2) = (uint8_t)(ad_re >> 8);
  *(modebusbuffer+3) = (uint8_t)(ad_re & 0xFF);
  *(modebusbuffer+4) = (uint8_t)(value >> 8);
  *(modebusbuffer+5) = (uint8_t)(value & 0xFF);
  crc = CRC_check(modebusbuffer, 6);
  crcl = crc & 0xFF;
  crch = crc >> 8;
  *(modebusbuffer+6) = crcl;
  *(modebusbuffer+7) = crch;
}
/**************************************************************************
    @brief         : ModebusRTU读取单个寄存器指令，功能码为03
    @param[in]     : ad：Modebus通讯时从机地址 ad_re：要读的寄存器开始地址，两字节
    @param[out]    :
    @return        : 无
    @others        : 此函数只是生成了一条03指令数据帧，读取需要根据程序逻辑进行移植
***************************************************************************/
void modebus03_solve(uint8_t ad, uint16_t ad_re, uint8_t *modebusbuffer)
{
  uint16_t crc = 0;
  uint8_t crcl = 0, crch = 0;
  *(modebusbuffer) = ad;
  *(modebusbuffer+1) = 0x03;
  *(modebusbuffer+2) = (uint8_t)(ad_re >> 8);
  *(modebusbuffer+3) = (uint8_t)(ad_re & 0xFF);
  *(modebusbuffer+4) = 0x00;
  *(modebusbuffer+5) = 0x01;
  crc = CRC_check(modebusbuffer, 6);
  crcl = crc & 0xFF;
  crch = crc >> 8;
  *(modebusbuffer+6) = crcl;
  *(modebusbuffer+7) = crch;

}
/**************************************************************************
    @brief         : ModebusRTU解析函数，自动解析缓存，取出的值带符号16位
    @param[in]     : 无
    @param[out]    :
    @return        : 返回两个字节的值合并到一个int_16中
    @others        :
***************************************************************************/
int16_t modebusbuffer_readint16( uint8_t *modebusbuffer)
{
  int16_t value = 0;
  if (*(modebusbuffer+1) == 0x03)
  {
    value =(int16_t)(*(modebusbuffer+3) << 8) + (int16_t)*(modebusbuffer+4);
    return value;
  }
  else if (*(modebusbuffer+1) == 0x06)
  {
    value = (int16_t)(*(modebusbuffer+4) << 8) + (int16_t)*(modebusbuffer+5);
    return value;
  }
  else if (*(modebusbuffer+1) == 0x86)
  {
    return 0xFFFF;
  }
}

/**************************************************************************
    @brief         : ModebusRTU解析函数，自动解析缓存，取出的值为无符号16位
    @param[in]     : 无
    @param[out]    :
    @return        : 返回两个字节的值合并到一个uint_16中
    @others        :
***************************************************************************/
uint16_t modebusbuffer_readuint16( uint8_t *modebusbuffer)
{
  uint16_t value = 0;
  if (*(modebusbuffer+1) == 0x03)
  {
    value = (*(modebusbuffer+3) << 8) + *(modebusbuffer+4);
    return value;
  }
  else if (*(modebusbuffer+1) == 0x06)
  {
    value = (*(modebusbuffer+4) << 8) + *(modebusbuffer+5);
    return value;
  }
  else if (*(modebusbuffer+1) == 0x86)
  {
    return 0xFFFF;
  }
}


