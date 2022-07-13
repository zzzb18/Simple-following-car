#ifndef MODEBUS_H
#define MODEBUS_H



#include "Arduino.h"
#include <string.h>



static uint16_t  CRC_check(uint8_t *pdata, uint32_t plen);

void modebus06_solve(uint8_t ad, uint16_t ad_re, uint16_t value,  uint8_t *modebusbuffer);

void modebus03_solve(uint8_t ad, uint16_t ad_re, uint8_t *modebusbuffer);

int16_t modebusbuffer_readint16( uint8_t *modebusbuffer);

uint16_t modebusbuffer_readuint16( uint8_t *modebusbuffer);

extern uint8_t modebusbuffer1[8];
extern uint8_t modebusbuffer2[8];
extern uint8_t modebusbufferRead[8];
extern uint8_t modebusreadbuffer[8];

#endif
