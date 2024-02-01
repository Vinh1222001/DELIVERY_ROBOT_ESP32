#pragma once

#ifndef SHIFTREGISTER_H
#define SHIFTREGISTER_H

#include <stdio.h>
#include "freertos/FreeRTOS.h"
#include "freertos/task.h"
#include "driver/gpio.h"

#ifdef __cplusplus
extern "C" {
#endif


#define LSBFIRST 0
#define MSBFIRST 1
#define LOW 0
#define HIGH 1

void printHello(void* arg);

typedef struct{

    gpio_num_t clock_pin;
    gpio_num_t slatch_pin;
    gpio_num_t serial_data_pin;
    uint8_t bit_order;

}shift_register_t;

void shift_regiter_initialize(shift_register_t shift_regiter);
void shift_out(shift_register_t shift_regiter, uint8_t val);

#ifdef __cplusplus
}
#endif


#endif