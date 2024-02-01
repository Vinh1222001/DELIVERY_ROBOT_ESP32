#include <stdio.h>
#include <inttypes.h>
#include "sdkconfig.h"
#include "freertos/FreeRTOS.h"
#include "freertos/task.h"
#include "esp_chip_info.h"
#include "esp_flash.h"

#include "esp_log.h"

#include "esp_attr.h"

// #include "driver/mcpwm.h"
// #include "soc/mcpwm_reg.h"
// #include "soc/mcpwm_struct.h"

#include "ShiftRegister.h"
#include "Servo.h"
#include "MovingController.h"

#define CLOCK_PIN GPIO_NUM_27
#define SLATCH_PIN GPIO_NUM_26
#define SERIAL_DATA_PIN GPIO_NUM_25

servo_t servo18={
    .unit = MCPWM_UNIT_0,
    .signal = MCPWM0A,
    .timer = MCPWM_TIMER_0,
    .operator = MCPWM_OPR_A,
    .gpio =18 ,
    .angle = 0,
};

shift_register_t my_register = {
    .clock_pin = CLOCK_PIN,
    .slatch_pin = SLATCH_PIN,
    .serial_data_pin = SERIAL_DATA_PIN,
    .bit_order = MSBFIRST,
};

void run_servo(void* arg){
    servo_init(servo18);
    uint32_t count=0;
    int step=2;
    while (1)
    {
        if(count< 0 || count >SERVO_MAX_DEGREE){
            step*=-1;
        }
        count+=step;
        servo_set_angle(&servo18, count);
        ESP_LOGI("SERVO", "Current Angle: %d", servo_get_angle(&servo18));
        vTaskDelay(10); 
    }
    
}

void run_shift_register(void* arg){
    shift_regiter_initialize(my_register);
    while (1)
    {
        for (int val = 0; val < 255; val++)
        {
            gpio_set_level(SLATCH_PIN,0);

            if(val%2){

                shift_out(my_register, 0);
            }else{
                shift_out(my_register,2);
            }

            gpio_set_level(SLATCH_PIN,1);
            ESP_LOGI("Value","%d",val);
            vTaskDelay(5000/portTICK_PERIOD_MS);
        }
        
    }
}

void app_main()
{
    // printf("Hello World!\n");
    // xTaskCreatePinnedToCore(mcpwm_example_servo_control, "mcpwm_example_servo_control", 4096, NULL, 5, NULL,0);
    // xTaskCreatePinnedToCore(run_servo,"Servo_runner", 4096, NULL, 5, NULL, 0);
    gpio_set_direction(GPIO_NUM_18, GPIO_MODE_OUTPUT);
    gpio_set_direction(GPIO_NUM_19, GPIO_MODE_OUTPUT);

    int state=1;
    while(1){
        ESP_LOGI("MOTOR'S STATE", "%d", state);
        switch (state)
        {
            case 1:
                gpio_set_level(GPIO_NUM_18, 0);
                gpio_set_level(GPIO_NUM_19, 0);
                break;
            case 2:
                gpio_set_level(GPIO_NUM_18, 1);
                gpio_set_level(GPIO_NUM_19, 0);
                break;
            case 3:
                gpio_set_level(GPIO_NUM_18, 0);
                gpio_set_level(GPIO_NUM_19, 1);
                break;
            case 4:
                gpio_set_level(GPIO_NUM_18, 1);
                gpio_set_level(GPIO_NUM_19, 1);
                break;

        }
        if(++state>4){
            state = 1;
        }
        vTaskDelay(5000/portTICK_PERIOD_MS);
    }

    // xTaskCreatePinnedToCore(run_shift_register,"Shift_Register_Run", 2048, NULL,5, NULL, 0);

}

// void app_main(void)
// {
//     printHello();
//     servo_init();
// }
