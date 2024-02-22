#include <stdio.h>
#include <string.h>
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
#include "WifiController.h"
#include "HTTPRequests.h"

#include "esp_wifi.h"
#include "esp_event.h"
#include "nvs_flash.h"



void app_main()
{
    // printf("Hello World!\n");
    // xTaskCreatePinnedToCore(mcpwm_example_servo_control, "mcpwm_example_servo_control", 4096, NULL, 5, NULL,0);
    // xTaskCreatePinnedToCore(run_servo,"Servo_runner", 4096, NULL, 5, NULL, 0);
    // xTaskCreatePinnedToCore(run_shift_register,"Shift_Register_Run", 2048, NULL,5, NULL, 0);
    esp_err_t ret = nvs_flash_init();
    if(ret == ESP_ERR_NVS_NO_FREE_PAGES || ret == ESP_ERR_NVS_NEW_VERSION_FOUND){
        ESP_ERROR_CHECK(nvs_flash_erase());
        ret= nvs_flash_init();
    }

    ESP_ERROR_CHECK(ret);
    wifi_station_initialize();
    // wifi_scan();
    wifi_station_connect();




    // vTaskDelay(2000 / portTICK_PERIOD_MS);
    // printf("WIFI was initiated ...........\n\n");

    http_request_initialize();
    char* response = http_request_get("users", "select=*");
    ESP_LOGI("HTTP GET Request: ", "%s",response);
    const char* update_data = "[{\"value\":\"100\"}]";
    http_request_delete("users","id=eq.14");
}

