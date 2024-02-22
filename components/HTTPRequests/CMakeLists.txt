idf_component_register(SRCS "HTTPRequests.c"
                       INCLUDE_DIRS "include"
                       REQUIRES esp_event mbedtls esp_http_client esp-tls
                       )
