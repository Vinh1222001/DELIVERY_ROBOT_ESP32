#include "ShiftRegister.h"

void shift_regiter_initialize(shift_register_t shift_regiter){

    gpio_set_direction(shift_regiter.slatch_pin, GPIO_MODE_OUTPUT);
    gpio_set_direction(shift_regiter.clock_pin, GPIO_MODE_OUTPUT);
    gpio_set_direction(shift_regiter.serial_data_pin, GPIO_MODE_OUTPUT);

}
void shift_out(shift_register_t shift_regiter, uint8_t val){
    uint8_t i;

    for(i = 0; i < 8; i++) {
        if(shift_regiter.bit_order == LSBFIRST)
            gpio_set_level(shift_regiter.serial_data_pin, !!(val & (1 << i)));
        else
            gpio_set_level(shift_regiter.serial_data_pin, !!(val & (1 << (7 - i))));

        gpio_set_level(shift_regiter.clock_pin, HIGH);
        gpio_set_level(shift_regiter.clock_pin, LOW);
    }
}

void printHello(void* arg){
    while(1){

        printf("In ShiftRegister: %s\n",(char*)arg);
        vTaskDelay(1000/portTICK_PERIOD_MS);
    }
}