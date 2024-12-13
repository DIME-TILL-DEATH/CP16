#ifndef __REVERB_H__
#define __REVERB_H__

#include "../../PROCESSING/Reverb/early.h"
#include "appdefs.h"

#include "eeprom.h"

#define rev_size 8192

void early1(void);
void early2(void);
void early3(void);
void hall1(void);
void hall2(void);
void hall3(void);
void hall4(void);
void room1(void);
void room2(void);
void room3(void);
void room4(void);
void plate1(void);
void plate2(void);
void plate3(void);
void plate4(void);
void plate5(void);
void plate6(void);
void plate7(void);
void spring1(void);
void spring2(void);
void spring3(void);
void gate1(void);
void gate2(void);
void gate3(void);
void gate4(void);
void revers1(void);
void revers2(void);
void revers3(void);

extern float ear_outL;
extern float ear_outR;
extern float reverb_accum;
extern float rev_tim;
//extern float __CCM_BSS__ early_buf[];

#endif /*__REVERB_H__*/
