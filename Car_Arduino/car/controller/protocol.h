#ifndef PROTOCOL_H
#define PROTOCOL_H

#include "Arduino.h"
#include "../hardware/motor.h"
#include <IBusBM.h>
#include "../hardware/sensors.h"

#define RECEIVE_SIZE 32
#define SEND_SIZE 16
#define CMDLIST_SIZE 16
#define DATA_BUFFER_SIZE 8
#define MODEBUS_SIZE 64
#define MODEBUS_DATA_SIZE 16

#define FRAMEHEADER 0xAA

#define Cmd_Setspeed 0x01
#define Cmd_Stopcar 0x02
#define Cmd_Lightset  0x03

#define MAXSPEED_FRE 2000
#define MAXSPEED_MINUS_FRE 1000
#define MINUS_VALUE 100

#define REMOTEMODE 0x01
#define UARTMODE 0x02

#define IBUS_SERIAL Serial2
#define INPUT_SERIAL Serial3
#define DEBUG_SERIAL Serial

#define INPUT_BAUDRATE 115200
#define INPUT_SETTINGS SERIAL_8N1

#define LIGHT 35
#define LIGHTON 1
#define LIGHTOFF 0


typedef struct _RECEIVE_DATA_
{
    uint8_t Buffer[RECEIVE_SIZE];
    struct _CMD_DATA_ 
    {
        uint8_t Header;
        uint8_t Cmd_type;
        uint8_t Length;
        uint8_t Databuffer[DATA_BUFFER_SIZE];
        uint8_t Checksum;

    }Cmd_data;
}RECEIVE_DATA;

typedef struct _SEND_DATA_
{
    uint8_t Buffer[SEND_SIZE];
    struct _RETURN_DATA_
    {
        uint8_t Header;
        uint8_t Cmd_type;
        uint8_t Length;
        uint8_t Databuffer[DATA_BUFFER_SIZE];
        uint8_t Checksum;
    }Return_data;
}SEND_DATA;

typedef void(*Cmd_Callback)();

typedef struct _CMD_
{
    uint8_t Cmdnumber;
    Cmd_Callback cmd_callback;
}CMD;

typedef struct _CMD_LIST_
{
    uint8_t cmd_num;
    CMD cmdlist[CMDLIST_SIZE];
}CMD_LIST;

typedef struct _CONTROLLER_
{
    uint8_t stop_flag;
    uint8_t control_mode;
    int16_t target_leftspeed;
    int16_t target_rightspeed;
}CONTROLLER;


void remote_read(void);
void uartdata_read(void);
void cmd_match(uint8_t inputcmd);
void set_controlmode(uint8_t controlmode);
void init_controller(void);
void run_controller(void);
void creat_sendframe(void);
static void init_cmds(void);
static void C_setspeed(void);
static void C_stopcar(void);
static void C_lightset(void);
static uint8_t send_checksum(uint8_t *str, int str_length);
static uint8_t receive_checksum(uint8_t *str, int str_length);

extern RECEIVE_DATA Receive_data;
extern SEND_DATA Send_data;
extern CMD_LIST Cmd_list;
extern CONTROLLER Controller;
extern IBusBM IBus;

#endif
