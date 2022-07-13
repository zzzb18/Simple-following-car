#ifndef EMENU_H
#define EMENU_H

#include "oled.h"
#include "../hardware/mpu6050.h"
#include "../controller/protocol.h"

#define UP_KEY 28
#define DOWN_KEY 29
#define LEFT_KEY 30
#define RIGHT_KEY 31
#define CENTER_KEY 32

enum keyvalue
{
  RightKey = 1,
  LeftKey,
  DownKey,
  UpKey,
  CenterKey
};
enum keystate
{
  keyoff,
  keyon
};


typedef void(*Key_callback)();

uint8_t keyscan(void);
void match_key(void);
void init_key(void);
static void main_menu_1();
static void main_menu_2();
static void remote_mode_menu();
static void uart_mode_menu();

typedef struct keytable
{
  uint8_t current;
  uint8_t up;
  uint8_t down;
  uint8_t left;
  uint8_t right;
  uint8_t center;

  Key_callback current_operation;
}Keytable;

extern uint8_t index, menuflag;


#endif
