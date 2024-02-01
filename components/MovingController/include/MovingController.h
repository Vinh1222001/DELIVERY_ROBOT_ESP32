#pragma once

#ifndef MOVINGCONTROLLER_H
#define MOVINGCONTROLLER_H

#include "driver/gpio.h"
#include "esp_err.h"
#include "esp_log.h"

#ifdef __cplusplus
extern "C" {
#endif

#define HIGH_SIGNAL 1
#define LOW_SIGNAL 0

/**
* @brief Moving Statuses
*/

typedef enum{
    INIT;
    STOP,
    MOVING_FORWARD,
    MOVING_BACKWARD,
    ROTATING_LEFT,
    ROTATING_RIGHT,
    INIT_FAIL,
}moving_status_t;

typedef struct{

    gpio_num_t pin_A;
    gpio_num_t pin_B;

}motor_pins_t;

extern moving_status_t moving_status;

void moving_motor_pins_init(gpio_num_t left_A, gpio_num_t left_B, gpio_num_t right_A, gpio_num_t right_B);

esp_err_t moving_set_status(moving_status_t moving_status_val);

moving_status_t moving_get_status();

void moving_state_machine(void* arg);


#ifdef __cplusplus
}
#endif

#endif