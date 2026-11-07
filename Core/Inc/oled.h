/*
 * oled.h
 *
 *  Created on: Nov 7, 2026
 *      Author: user
 */

#ifndef INC_OLED_H_
#define INC_OLED_H_
#include <stdint.h>

/* Everything the OLED shows. Fill it in main(), then call OLED_Update(). */
typedef struct
{
    int16_t temp_x10;     /* temperature in tenths of a degree: 524 = 52.4 C */
    uint8_t light;        /* 0 = DARK, 1 = DIM, 2 = BRIGHT                   */
    uint8_t home_light;   /* 0 = OFF,  1 = ON                                */
    uint8_t buzzer;       /* 0 = OFF,  1 = ON                                */
    uint8_t motor;        /* 0 = OFF,  1 = ON                                */
    uint8_t motor_speed;  /* 0 to 100 (percent)                              */
    uint8_t cooling;      /* 1 = COOLING, 0 = NORMAL                         */
} SystemStatus;

void I2C1_Init(void);                      /* PB8 = SCL, PB9 = SDA          */
void OLED_Init(void);                      /* SSD1306 128x32                */
void OLED_Clear(void);
/* screen 0 = Temperature, Light, Home Light, Buzzer
   screen 1 = Motor, Motor Speed, System
   Redraws only the lines that changed. */
void OLED_Update(const SystemStatus *s, uint8_t screen);

#endif /* INC_OLED_H_ */
