#pragma once

#ifndef HTTPREQUESTS_H
#define HTTPREQUESTS_H

#ifdef __cplusplus
extern "C" {
#endif

#include <string.h>
#include <sys/param.h>
#include <stdlib.h>
#include <ctype.h>
#include "esp_log.h"
#include "esp_event.h"
#include "esp_crt_bundle.h"
#include "esp_system.h"
#include "esp_http_client.h"
#include "esp_tls.h"

#define SERVER_NAME                 CONFIG_SERVER_NAME
#define SERVER_URL                  CONFIG_SERVER_URL
#define SERVER_BASE_PATH            CONFIG_SERVER_BASE_PATH
#define SERVER_API_KEY              CONFIG_SERVER_API_KEY
#define SERVER_AUTHORIZATION        CONFIG_SERVER_AUTHORIZATION
#define MAX_TRANSMIT_BUFFER_SIZE    CONFIG_TRANSMIT_BUFFER_SIZE
#define MAX_RECIEVE_BUFFER_SIZE     CONFIG_RECIEVE_BUFFER_SIZE

#define MAX_API_URL_LENGTH          200

esp_err_t http_request_initialize();
char* http_request_get(char* table_name, char* query);
esp_err_t http_request_post(char* table_name, char* data);
esp_err_t http_request_update(char* table_name, char* query,char* data);
esp_err_t http_request_insert(char* table_name, char* data);
esp_err_t http_request_delete(char* table_name, char* query);

#ifdef __cplusplus
}
#endif


#endif