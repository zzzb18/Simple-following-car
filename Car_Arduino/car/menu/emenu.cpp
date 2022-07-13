/**************************************************************************
    @brief          : 菜单函数，包括了OLED屏函数
    @author         : RXF
    @copyright      :
    @version        : Version0.1
    @note           : 基于Adafruit_SSD1306开发
    @history        :
***************************************************************************/
#include "emenu.h"
Adafruit_SSD1306 display(OLED_WIDTH, OLED_HEIGHT, &Wire, OLED_RESET);
Keytable menu[5] =
{
  {0, 1, 1, 1, 1, 2, (*main_menu_1)},
  {1, 0, 0, 0, 0, 3, (*main_menu_2)},
  {2, 2, 2, 2, 2, 0, (*remote_mode_menu)},
  {3, 3, 3, 3, 3, 1, (*uart_mode_menu)},

};

uint8_t index = 0, menuflag = 1;

/**************************************************************************
    @brief         : 初始化OLED屏
    @param[in]     :
    @param[out]    :
    @return        :
    @others        :
***************************************************************************/
void oled_init()
{
  display.begin(SSD1306_SWITCHCAPVCC, 0x3C);
  display.clearDisplay();
  display.setTextSize(1);
  display.drawBitmap(0, 0, init_logo, 128, 64, WHITE);
  display.display();
}
/**************************************************************************
    @brief         : 显示菜单选项
    @param[in]     :strptr是指向选项字符串的指针，inverse_option控制是否反色显示
    @param[out]    :
    @return        :
    @others        :
***************************************************************************/
void oled_printmenuoption(char *strptr, uint8_t inverse_option)
{
  if (inverse_option == 1)
  {
    display.setTextColor(BLACK, WHITE);
  }
  else
  {
    display.setTextColor(WHITE);
  }
  display.println(strptr);
}

/**************************************************************************
    @brief         : 显示小车运行信息demo
    @param[in]     :
    @param[out]    :
    @return        :
    @others        :
***************************************************************************/
void oled_showinfo()
{
  display.clearDisplay();
  display.setCursor(0, 0);
  //display.print("ax:"); display.print(gyrompu.ax); display.print("    gx:"); display.println(gyrompu.gx);
  //display.print("ay:"); display.print(gyrompu.ay); display.print("    gy:"); display.println(gyrompu.gy);
  //display.print("az:"); display.print(gyrompu.az); display.print("    gz:"); display.println(gyrompu.gz);
  display.print("ls(Hz/s):"); display.println(Controller.target_leftspeed);
  display.print("rs(Hz/s):"); display.println(Controller.target_rightspeed);
  display.display();
}


/**************************************************************************
    @brief         : 下面都是菜单显示
    @param[in]     :
    @param[out]    :
    @return        :
    @others        :
***************************************************************************/
static void main_menu_1()
{
  display.clearDisplay();
  display.setCursor(0, 0);
  oled_printmenuoption("->Remote mode!", 1);
  oled_printmenuoption("  UART mode!", 0);
  oled_printmenuoption("  By:RXF", 0);
  display.display();
}
static void main_menu_2()
{
  display.clearDisplay();
  display.setCursor(0, 0);
  oled_printmenuoption("  Remote mode!", 0);
  oled_printmenuoption("->UART mode!", 1);
  oled_printmenuoption("  By:RXF", 0);
  display.display();
}

static void remote_mode_menu()
{
  display.clearDisplay();
  display.setCursor(8, 30);
  display.println("Wait for receiver!");
  //Serial.println("Wait for receiver");
  display.display();
  set_controlmode(REMOTEMODE);
  menuflag = 0;
  while (IBus.cnt_rec == 0) delay(100); //阻塞函数，防止遥控器未打开时小车失控
}

static void uart_mode_menu()
{
  display.clearDisplay();
  display.setCursor(8, 30);
  display.println("UART CONTROL MODE");
  //Serial.println("UART CONTROL MODE");
  display.display();
  set_controlmode(UARTMODE);
  menuflag = 0;
}


/**************************************************************************
    @brief         : 初始化五向按键
    @param[in]     :
    @param[out]    :
    @return        :
    @others        :
***************************************************************************/
void init_key(void)
{
  pinMode(UP_KEY, INPUT_PULLUP);
  pinMode(DOWN_KEY, INPUT_PULLUP);
  pinMode(LEFT_KEY, INPUT_PULLUP);
  pinMode(RIGHT_KEY, INPUT_PULLUP);
  pinMode(CENTER_KEY, INPUT_PULLUP);
  index = 0;
}
/**************************************************************************
    @brief         : 读取五向按键的值
    @param[in]     :
    @param[out]    :
    @return        : 按键值
    @others        :
***************************************************************************/
static uint8_t keyscan(void)
{
  uint8_t upval, downval, leftval, rightval, centerval;
  static uint8_t keyup = keyon;

  upval = digitalRead(UP_KEY);
  centerval = digitalRead(CENTER_KEY);
  downval = digitalRead(DOWN_KEY);
  leftval = digitalRead(LEFT_KEY);
  rightval = digitalRead(RIGHT_KEY);

  if (keyup && (upval == LOW || downval == LOW || leftval == LOW || rightval == LOW || centerval == LOW))
  {
    delay(10);
    keyup = keyoff;
    if (centerval == LOW)
    {
      return CenterKey;
    }
    else if (upval == LOW)
    {
      return UpKey;
    }
    else if (downval == LOW)
    {
      return DownKey;
    }
    else if (leftval == LOW)
    {
      return LeftKey;
    }
    else if (rightval == LOW)
    {
      return RightKey;
    }
  }
  else if ((upval == HIGH) && (downval == HIGH) && (leftval == HIGH) && (rightval == HIGH) && (centerval == HIGH))
  {
    keyup = keyon;
  }
  return 0;
}
/**************************************************************************
    @brief         : 按键值匹配函数，自动执行对应的操作函数
    @param[in]     :
    @param[out]    :
    @return        :
    @others        :
***************************************************************************/


void match_key(void)
{
  uint8_t value = keyscan();
  switch (value)
  {
    case 0: break;
    case RightKey: index = menu[index].right; break;
    case LeftKey: index = menu[index].left; break;
    case DownKey: index = menu[index].down; break;
    case UpKey: index = menu[index].up; break;
    case CenterKey: index = menu[index].center; break;
    default:
      break;
  }
  menu[index].current_operation();
}
