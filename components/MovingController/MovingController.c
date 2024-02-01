#include "MovingController.h"

const char* MOVING_CONTROLLER_TAG = "MOVING CONTROLLER";

moving_status_t pre_moving_status = INIT;
moving_status_t moving_status = INIT;

motor_pins_t left_pins;
motor_pins_t right_pins;

void moving_motor_pins_init(gpio_num_t left_A, gpio_num_t left_B, gpio_num_t right_A, gpio_num_t right_B){
    left_pins.pin_A= left_A;
    left_pins.pin_B= left_B;
    right_pins.pin_A=right_A;
    right_pins.pin_B=right_B;
}

esp_err_t moving_set_status(moving_status_t moving_status_val){

    if(pre_moving_status = moving_status) return ESP_OK;

    if(moving_status = moving_status_val)return ESP_OK;

    return ESP_OK;
}

moving_status_t moving_get_status(){
    return moving_status;
}

void moving_state_machine(void* arg){
    if(pre_moving_status == moving_status) return;

    switch (moving_status)
    {
    case INIT:
        ESP_ERROR_CHECK( gpio_set_direction(left_pins.pin_A, GPIO_MODE_OUTPUT) );
        ESP_ERROR_CHECK( gpio_set_direction(left_pins.pin_B, GPIO_MODE_OUTPUT) );
        ESP_ERROR_CHECK( gpio_set_direction(right_pins.pin_A, GPIO_MODE_OUTPUT) );
        ESP_ERROR_CHECK( gpio_set_direction(right_pins.pin_B, GPIO_MODE_OUTPUT) );
        break;
    case STOP:
        ESP_ERROR_CHECK( gpio_set_level(left_pins.pin_A, LOW_SIGNAL) ); // 0
        ESP_ERROR_CHECK( gpio_set_level(left_pins.pin_B, LOW_SIGNAL) ); // 0
        ESP_ERROR_CHECK( gpio_set_level(right_pins.pin_A, LOW_SIGNAL) ); // 0
        ESP_ERROR_CHECK( gpio_set_level(right_pins.pin_B, LOW_SIGNAL) ); // 0
        break;
    case MOVING_FORWARD:
        ESP_ERROR_CHECK( gpio_set_level(left_pins.pin_A, LOW_SIGNAL) ); // 0
        ESP_ERROR_CHECK( gpio_set_level(left_pins.pin_B, HIGH_SIGNAL) ); // 1
        ESP_ERROR_CHECK( gpio_set_level(right_pins.pin_A, LOW_SIGNAL) ); // 0
        ESP_ERROR_CHECK( gpio_set_level(right_pins.pin_B, HIGH_SIGNAL) ); // 1
        break;

    case MOVING_BACKWARD:
        ESP_ERROR_CHECK( gpio_set_level(left_pins.pin_A, HIGH_SIGNAL) );// 1
        ESP_ERROR_CHECK( gpio_set_level(left_pins.pin_B, LOW_SIGNAL) );// 0
        ESP_ERROR_CHECK( gpio_set_level(right_pins.pin_A, HIGH_SIGNAL) );// 1
        ESP_ERROR_CHECK( gpio_set_level(right_pins.pin_B, LOW_SIGNAL) );// 0
        break;
    case ROTATING_LEFT:
        ESP_ERROR_CHECK( gpio_set_level(left_pins.pin_A, HIGH_SIGNAL) ); // 1
        ESP_ERROR_CHECK( gpio_set_level(left_pins.pin_B, LOW_SIGNAL) ); // 0
        ESP_ERROR_CHECK( gpio_set_level(right_pins.pin_A, LOW_SIGNAL) ); // 0
        ESP_ERROR_CHECK( gpio_set_level(right_pins.pin_B, HIGH_SIGNAL) ); // 1
        break;
    case ROTATING_RIGHT:
        ESP_ERROR_CHECK( gpio_set_level(left_pins.pin_A, LOW_SIGNAL) ); // 0
        ESP_ERROR_CHECK( gpio_set_level(left_pins.pin_B, HIGH_SIGNAL) ); // 1
        ESP_ERROR_CHECK( gpio_set_level(right_pins.pin_A, HIGH_SIGNAL) ); // 1
        ESP_ERROR_CHECK( gpio_set_level(right_pins.pin_B, LOW_SIGNAL) ); // 0
        break;
    case INIT_FAIL:
        ESP_LOGI(MOVING_CONTROLLER_TAG,"Initalizing gpio pins failed!");
        break;
    default:
        ESP_LOGI(MOVING_CONTROLLER_TAG,"moving_status was contain a unknown status");
        break;
    }
}