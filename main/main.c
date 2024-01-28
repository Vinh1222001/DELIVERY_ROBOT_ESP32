/*
 * SPDX-FileCopyrightText: 2010-2022 Espressif Systems (Shanghai) CO LTD
 *
 * SPDX-License-Identifier: CC0-1.0
 */

#include <stdio.h>
#include <inttypes.h>
#include "sdkconfig.h"
#include "freertos/FreeRTOS.h"
#include "freertos/task.h"
#include "esp_chip_info.h"
#include "esp_flash.h"

#include "ShiftRegister.h"
#include "Servo.h"

servo_t servo1, servo2;

void app_main(void)
{
    // printHello();

    servo_init_config(&servo1,"SERVO 1", 25, 0, 0, 2);
    servo_init_config(&servo2,"SERVO 2", 26, 1, 60, 2);

    servo_init(&servo1);  // GPIO cho servo 1
    servo_init(&servo2);  // GPIO cho servo 2
    
    xTaskCreate(servo_run, "Servo_run1", 2048, &servo1, 10, NULL);
    xTaskCreate(servo_run, "Servo_run2", 2048, &servo2, 10, NULL);
}
