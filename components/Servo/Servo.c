#include "servo.h"

// static const char *servo->name = "example";

void servo_init_config(servo_t *servo,char* name, int gpio_num, int group_id, int initial_angle,int step){
    servo->timer = NULL;
    servo->oper = NULL;
    servo->comparator = NULL;
    servo->generator = NULL;
    servo->name = name;
    servo->gpio_num = gpio_num;
    servo->group_id = group_id;
    servo->initial_angle = initial_angle;
    servo->step = step; 
}

static inline uint32_t example_angle_to_compare(int angle)
{
    return (angle - SERVO_MIN_DEGREE) * (SERVO_MAX_PULSEWIDTH_US - SERVO_MIN_PULSEWIDTH_US) / (SERVO_MAX_DEGREE - SERVO_MIN_DEGREE) + SERVO_MIN_PULSEWIDTH_US;
}

void servo_init(servo_t *servo)
{
    ESP_LOGI(servo->name, "Create timer and operator");

    mcpwm_timer_config_t timer_config = {
        .group_id = servo->group_id,
        .clk_src = MCPWM_TIMER_CLK_SRC_DEFAULT,
        .resolution_hz = SERVO_TIMEBASE_RESOLUTION_HZ,
        .period_ticks = SERVO_TIMEBASE_PERIOD,
        .count_mode = MCPWM_TIMER_COUNT_MODE_UP,
    };
    ESP_ERROR_CHECK(mcpwm_new_timer(&timer_config, &servo->timer));

    mcpwm_operator_config_t operator_config = {
        .group_id = servo->group_id, // operator must be in the same group to the timer
    };
    ESP_ERROR_CHECK(mcpwm_new_operator(&operator_config, &servo->oper));

    ESP_LOGI(servo->name, "Connect timer and operator");
    ESP_ERROR_CHECK(mcpwm_operator_connect_timer(servo->oper, servo->timer));

    ESP_LOGI(servo->name, "Create comparator and generator from the operator");
    mcpwm_comparator_config_t comparator_config = {
        .flags.update_cmp_on_tez = true,
    };
    ESP_ERROR_CHECK(mcpwm_new_comparator(servo->oper, &comparator_config, &servo->comparator));

    mcpwm_generator_config_t generator_config = {
        .gen_gpio_num = servo->gpio_num,
    };
    ESP_ERROR_CHECK(mcpwm_new_generator(servo->oper, &generator_config, &servo->generator));

    // set the initial compare value, so that the servo will spin to the center position
    ESP_ERROR_CHECK(mcpwm_comparator_set_compare_value(servo->comparator, example_angle_to_compare(0)));

    ESP_LOGI(servo->name, "Set generator action on timer and compare event");
    // go high on counter empty
    ESP_ERROR_CHECK(mcpwm_generator_set_action_on_timer_event(servo->generator,
                                                              MCPWM_GEN_TIMER_EVENT_ACTION(MCPWM_TIMER_DIRECTION_UP, MCPWM_TIMER_EVENT_EMPTY, MCPWM_GEN_ACTION_HIGH)));
    // go low on compare threshold
    ESP_ERROR_CHECK(mcpwm_generator_set_action_on_compare_event(servo->generator,
                                                                MCPWM_GEN_COMPARE_EVENT_ACTION(MCPWM_TIMER_DIRECTION_UP, servo->comparator, MCPWM_GEN_ACTION_LOW)));

    ESP_LOGI(servo->name, "Enable and start timer");
    ESP_ERROR_CHECK(mcpwm_timer_enable(servo->timer));
    ESP_ERROR_CHECK(mcpwm_timer_start_stop(servo->timer, MCPWM_TIMER_START_NO_STOP));

}

void servo_run(servo_t *servo){
    if(servo->comparator == NULL){
        ESP_LOGI(servo->name, "Comparator is NULL");
    }else{

        while (1) {
            ESP_LOGI(servo->name, "Angle of rotation: %d", servo->initial_angle);
            ESP_LOGI(servo->name, "Angle of rotation: %lu", example_angle_to_compare(servo->initial_angle));
            ESP_LOGI(servo->name, "Start comparing!");
            ESP_ERROR_CHECK(mcpwm_comparator_set_compare_value(servo->comparator, example_angle_to_compare(servo->initial_angle)));
            //Add delay, since it takes time for servo to rotate, usually 200ms/60degree rotation under 5V power supply
            vTaskDelay(pdMS_TO_TICKS(500));
            ESP_LOGI(servo->name, "servo->initial_angle + servo->step: %d", servo->initial_angle + servo->step);
            if ((servo->initial_angle + servo->step) > 60 || (servo->initial_angle + servo->step) < -60) {
                servo->step *= -1;
            }
            servo->initial_angle += servo->step;
        }
    }
}