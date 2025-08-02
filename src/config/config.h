#ifndef CONFIG_H
#define CONFIG_H

// you need to create this file to use the net module
#include "config/net.h"

#define CONF_HB_L1 13
#define CONF_HB_L2 12
#define CONF_HB_R1 11
#define CONF_HB_R2 10
#define CONF_HB_SLEEP 15
#define CONF_HB_FAULT 14

#define CONF_ENC_L_A 6
#define CONF_ENC_L_B 7

#define CONF_ENC_R_A 8
#define CONF_ENC_R_B 9

#define CONF_MOT_PID_L_KP -1
#define CONF_MOT_PID_L_KI -1
#define CONF_MOT_PID_L_KD -1
#define CONF_MOT_PID_L_INT_MIN -1
#define CONF_MOT_PID_L_INT_MAX -1

#define CONF_MOT_PID_R_KP -1
#define CONF_MOT_PID_R_KI -1
#define CONF_MOT_PID_R_KD -1
#define CONF_MOT_PID_R_INT_MIN -1
#define CONF_MOT_PID_R_INT_MAX -1

#define CONF_SERVO_PIN 16
#define CONF_SERVO_MIN 0
#define CONF_SERVO_MID 0x7fff
#define CONF_SERVO_MAX 0xffff

#define CONF_ULTRA_TRIG 20
#define CONF_ULTRA_ECHO 19

#define CONF_RGB_SENS_SDA 26
#define CONF_RGB_SENS_SCL 27
#define CONF_RGB_SENS_I2C_IDX 0

#define CONF_RGB_SENS_LED_IN 17

#define CONF_RGB_LED_R -1
#define CONF_RGB_LED_G -1
#define CONF_RGB_LED_B -1

#define CONF_PROX_PIN 3
#define CONF_PROX_DEBOUNCE (500 * 1000)

#define CONF_BUZZ_PIN 2
#define CONF_BUZZ_FREQ 300

#define CONF_BUTTON_PIN 1
#define CONF_BUTTON_DEBOUNCE (500 * 1000)

#endif
