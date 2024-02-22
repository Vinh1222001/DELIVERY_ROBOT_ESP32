#include "HTTPRequests.h"

const char* HTTP_REQUEST_TAG = "HTTP_REQUESTS";
// static char* request_path = SERVER_BASE_PATH;

// static char* local_response_buffer = (char*)calloc(MAX_RECIEVE_BUFFER_SIZE, sizeof(char));
static char* local_response_buffer[MAX_RECIEVE_BUFFER_SIZE];
static esp_http_client_handle_t client;

char* createAPI(char* table_name, char* query){
    // Tạo buffer để lưu trữ API URL
    char* api_url = malloc(MAX_API_URL_LENGTH);
    if (api_url == NULL) {
        // Lỗi cấp phát bộ nhớ
        return NULL;
    }

    // Ghi http://SERVER_URL SERVER_BASE_PATH vào buffer
    strcpy(api_url, "http://");
    strcat(api_url, SERVER_URL);
    strcat(api_url, SERVER_BASE_PATH);

    // Thêm table_name vào buffer
    strcat(api_url, table_name);

    // Nếu query != NULL, thêm "?" và query vào buffer
    if (query != NULL) {
        strcat(api_url, "?");
        strcat(api_url, query);
    }

    // Trả về API URL
    return api_url;
}

esp_err_t add_header(void){
    esp_err_t err = esp_http_client_set_header(client, "apikey", SERVER_API_KEY);
    if(err != ESP_OK){
        ESP_LOGE(HTTP_REQUEST_TAG, "HTTP GET request failed: %s", esp_err_to_name(err));
        return err;
    }
    err = esp_http_client_set_header(client, "Authorization", SERVER_AUTHORIZATION);
    if(err != ESP_OK){
        ESP_LOGE(HTTP_REQUEST_TAG, "HTTP GET request failed: %s", esp_err_to_name(err));
        return err;
    }
    err = esp_http_client_set_header(client, "Content-Type", "application/json");
    if (err != ESP_OK) {
        ESP_LOGE(HTTP_REQUEST_TAG, "Failed to set content type: %s", esp_err_to_name(err));
        return err;
    }
    return ESP_OK;
}

esp_err_t _http_event_handler(esp_http_client_event_t *evt)
{
    static char *output_buffer;  // Buffer to store response of http request from event handler
    static int output_len;       // Stores number of bytes read
    switch(evt->event_id) {
        case HTTP_EVENT_ERROR:
            ESP_LOGD(HTTP_REQUEST_TAG, "HTTP_EVENT_ERROR");
            break;
        case HTTP_EVENT_ON_CONNECTED:
            ESP_LOGD(HTTP_REQUEST_TAG, "HTTP_EVENT_ON_CONNECTED");
            break;
        case HTTP_EVENT_HEADER_SENT:
            ESP_LOGD(HTTP_REQUEST_TAG, "HTTP_EVENT_HEADER_SENT");
            break;
        case HTTP_EVENT_ON_HEADER:
            ESP_LOGD(HTTP_REQUEST_TAG, "HTTP_EVENT_ON_HEADER, key=%s, value=%s", evt->header_key, evt->header_value);
            break;
        case HTTP_EVENT_ON_DATA:
            ESP_LOGD(HTTP_REQUEST_TAG, "HTTP_EVENT_ON_DATA, len=%d", evt->data_len);
            // Clean the buffer in case of a new request
            if (output_len == 0 && evt->user_data) {
                // we are just starting to copy the output data into the use
                memset(evt->user_data, 0, MAX_TRANSMIT_BUFFER_SIZE);
            }
            /*
             *  Check for chunked encoding is added as the URL for chunked encoding used in this example returns binary data.
             *  However, event handler can also be used in case chunked encoding is used.
             */
            if (esp_http_client_is_chunked_response(evt->client)) {
                // If user_data buffer is configured, copy the response into the buffer
                int copy_len = 0;
                if (evt->user_data) {
                    // The last byte in evt->user_data is kept for the NULL character in case of out-of-bound access.
                    copy_len = MIN(evt->data_len, (MAX_TRANSMIT_BUFFER_SIZE - output_len));
                    if (copy_len) {
                        memcpy(evt->user_data + output_len, evt->data, copy_len);
                    }
                } else {
                    int content_len = esp_http_client_get_content_length(evt->client);
                    if (output_buffer == NULL) {
                        // We initialize output_buffer with 0 because it is used by strlen() and similar functions therefore should be null terminated.
                        output_buffer = (char *) calloc(content_len + 1, sizeof(char));
                        output_len = 0;
                        if (output_buffer == NULL) {
                            ESP_LOGE(HTTP_REQUEST_TAG, "Failed to allocate memory for output buffer");
                            return ESP_FAIL;
                        }
                    }
                    copy_len = MIN(evt->data_len, (content_len - output_len));
                    if (copy_len) {
                        memcpy(output_buffer + output_len, evt->data, copy_len);
                    }
                }
                output_len += copy_len;
            }
            break;
        case HTTP_EVENT_ON_FINISH:
            ESP_LOGD(HTTP_REQUEST_TAG, "HTTP_EVENT_ON_FINISH");
            if (output_buffer != NULL) {
                // Response is accumulated in output_buffer. Uncomment the below line to print the accumulated response
                // ESP_LOG_BUFFER_HEX(HTTP_REQUEST_TAG, output_buffer, output_len);
                free(output_buffer);
                output_buffer = NULL;
            }
            output_len = 0;
            break;
        case HTTP_EVENT_DISCONNECTED:
            ESP_LOGI(HTTP_REQUEST_TAG, "HTTP_EVENT_DISCONNECTED");
            int mbedtls_err = 0;
            esp_err_t err = esp_tls_get_and_clear_last_error((esp_tls_error_handle_t)evt->data, &mbedtls_err, NULL);
            if (err != 0) {
                ESP_LOGI(HTTP_REQUEST_TAG, "Last esp error code: 0x%x", err);
                ESP_LOGI(HTTP_REQUEST_TAG, "Last mbedtls failure: 0x%x", mbedtls_err);
            }
            if (output_buffer != NULL) {
                free(output_buffer);
                output_buffer = NULL;
            }
            output_len = 0;
            break;
        case HTTP_EVENT_REDIRECT:
            ESP_LOGD(HTTP_REQUEST_TAG, "HTTP_EVENT_REDIRECT");
            esp_http_client_set_header(evt->client, "From", "user@example.com");
            esp_http_client_set_header(evt->client, "Accept", "text/html");
            esp_http_client_set_redirection(evt->client);
            break;
    }
    return ESP_OK;
}

esp_err_t http_request_initialize(){
    esp_http_client_config_t config = {
        .url = "http://"CONFIG_SERVER_URL,
        // .host = SERVER_URL,
        // .path = path,
        // .query ="select=*",
        .method = HTTP_METHOD_GET,
        .event_handler = _http_event_handler,
        .user_data = local_response_buffer,
        .buffer_size = MAX_RECIEVE_BUFFER_SIZE,
        .buffer_size_tx = MAX_TRANSMIT_BUFFER_SIZE,
        .auth_type = HTTP_AUTH_TYPE_NONE,
        .transport_type = HTTP_TRANSPORT_OVER_SSL,
        .crt_bundle_attach = esp_crt_bundle_attach,
    };
    client = esp_http_client_init(&config);
    return add_header();
}

char* http_request_get(char* table_name, char* query)
{
    char* api_url = createAPI(table_name, query);
    esp_http_client_set_url(client, api_url);
    esp_http_client_set_method(client, HTTP_METHOD_GET);
    esp_err_t err = esp_http_client_perform(client);
    free(api_url);
    if (err == ESP_OK) {
        ESP_LOGI(HTTP_REQUEST_TAG, "HTTP GET Status = %d, content_length = %"PRId64,
                esp_http_client_get_status_code(client),
                esp_http_client_get_content_length(client));
    } else {
        ESP_LOGE(HTTP_REQUEST_TAG, "HTTP GET request failed: %s", esp_err_to_name(err));
    }
    // esp_http_client_cleanup(client);
    return local_response_buffer;
}

esp_err_t http_request_post(char* table_name, char* data){

    // ESP_LOGI(HTTP_REQUEST_TAG, "Post data");
    char* api_url = createAPI(table_name, NULL);
    esp_http_client_set_url(client, api_url);
    esp_http_client_set_method(client, HTTP_METHOD_POST);
    esp_http_client_set_post_field(client, data, strlen(data));
    esp_err_t err = esp_http_client_perform(client);
    free(api_url);
    if(err != ESP_OK){
        ESP_LOGE(HTTP_REQUEST_TAG, "HTTP POST request failed: %s", esp_err_to_name(err));
        return err;
    }
    ESP_LOGI(HTTP_REQUEST_TAG, "HTTP POST Status = %d, content_length = %"PRId64,
        esp_http_client_get_status_code(client),
        esp_http_client_get_content_length(client));
    return ESP_OK;
}
esp_err_t http_request_update(char* table_name, char* query,char* data){
    ESP_LOGI(HTTP_REQUEST_TAG, "Update data");
    char* api_url = createAPI(table_name, query);
    esp_http_client_set_url(client, api_url);
    esp_http_client_set_method(client, HTTP_METHOD_PATCH);
    esp_http_client_set_post_field(client, data, strlen(data));
    esp_err_t err = esp_http_client_perform(client);
    free(api_url);
    if(err != ESP_OK){
        ESP_LOGE(HTTP_REQUEST_TAG, "HTTP UPDATE request failed: %s", esp_err_to_name(err));
        return err;
    }
    ESP_LOGI(HTTP_REQUEST_TAG, "HTTP UPDATE Status = %d, content_length = %"PRId64,
        esp_http_client_get_status_code(client),
        esp_http_client_get_content_length(client));
    return ESP_OK;

}
esp_err_t http_request_insert(char* table_name, char* data){
    ESP_LOGI(HTTP_REQUEST_TAG, "Insert data");
    return ESP_OK;
}

esp_err_t http_request_delete(char* table_name, char* query){
    ESP_LOGI(HTTP_REQUEST_TAG, "Delete data from the server");
    char* api_url = createAPI(table_name, query);
    esp_http_client_set_url(client, api_url);
    esp_http_client_set_method(client, HTTP_METHOD_DELETE);
    esp_err_t err = esp_http_client_perform(client);
    free(api_url);
    if(err != ESP_OK){
        ESP_LOGE(HTTP_REQUEST_TAG, "HTTP DELETE request failed: %s", esp_err_to_name(err));
        return err;
    }
    ESP_LOGI(HTTP_REQUEST_TAG, "HTTP DELETE Status = %d, content_length = %"PRId64,
        esp_http_client_get_status_code(client),
        esp_http_client_get_content_length(client));
    return ESP_OK;
}