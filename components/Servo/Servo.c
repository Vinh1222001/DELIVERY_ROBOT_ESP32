#include "Servo.h"

/**
 * @brief Use this function to calcute pulse width for per degree rotation
 *
 * @param  degree_of_rotation the angle in degree to which servo has to rotate
 *
 * @return
 *     - calculated pulse width
 */
const char* USER_SERVO_TAG = "SERVO";

static uint32_t angle_to_pulse_width_converter(uint32_t degree_of_rotation)
{
    uint32_t cal_pulsewidth = 0;
    cal_pulsewidth = (SERVO_MIN_PULSEWIDTH + (((SERVO_MAX_PULSEWIDTH - SERVO_MIN_PULSEWIDTH) * (degree_of_rotation)) / (SERVO_MAX_DEGREE)));
    return cal_pulsewidth;
}

void servo_init(servo_t servo){
    ESP_LOGI(USER_SERVO_TAG,"Initialize new servo at GPIO: %d", servo.gpio);
    ESP_ERROR_CHECK(mcpwm_gpio_init(servo.unit, servo.signal, servo.gpio));
    mcpwm_config_t pwm_config;
    pwm_config.frequency = 50;    //frequency = 50Hz, i.e. for every servo motor time period should be 20ms
    pwm_config.cmpr_a = 0;    //duty cycle of PWMxA = 0
    pwm_config.cmpr_b = 0;    //duty cycle of PWMxb = 0
    pwm_config.counter_mode = MCPWM_UP_COUNTER;
    pwm_config.duty_mode = MCPWM_DUTY_MODE_0;
    ESP_ERROR_CHECK(mcpwm_init(servo.unit, servo.timer, &pwm_config));
}

void servo_set_angle(servo_t* servo, int angle){
    servo->angle =angle;
    ESP_ERROR_CHECK( mcpwm_set_duty_in_us(servo->unit, servo->timer, servo->operator,angle_to_pulse_width_converter(servo->angle)));
}

int servo_get_angle(servo_t* servo){
    return servo->angle;
}
