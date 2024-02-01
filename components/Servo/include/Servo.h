#ifndef SERVO_H
#define SERVO_H

#include "driver/mcpwm.h"
#include "soc/mcpwm_reg.h"
#include "soc/mcpwm_struct.h"
#include "esp_log.h"

#define SERVO_MIN_PULSEWIDTH 500 //Minimum pulse width in microsecond
#define SERVO_MAX_PULSEWIDTH 2500 //Maximum pulse width in microsecond
#define SERVO_MAX_DEGREE 180 //Maximum angle in degree upto which servo can rotate

typedef struct{
    mcpwm_unit_t unit;
    mcpwm_io_signals_t signal;
    mcpwm_timer_t timer;
    mcpwm_operator_t operator;
    int gpio;
    int angle;
} servo_t;

void servo_init(servo_t servo);
void servo_set_angle(servo_t* servo, int angle);
int servo_get_angle(servo_t* servo);
#endif