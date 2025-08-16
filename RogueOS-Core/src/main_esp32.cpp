#include "modules/command_router.hpp"
#include "modules/serial_cli.hpp"
#include "cli/help.hpp"
#include "esp_wifi.h"
#include <Arduino.h>
#include "esp_event.h"
#include "esp_log.h"
#include "nvs_flash.h"
#include "esp_netif.h"
#include "esp_http_server.h"

static const char *TAG = "RogueOS";

esp_err_t ws_handler(httpd_req_t *req) {
    httpd_ws_frame_t frame = {};
    frame.type = HTTPD_WS_TYPE_TEXT;
    frame.payload = (uint8_t*)"";
    frame.len = 0;
    return httpd_ws_recv_frame(req, &frame, 0);
}

esp_err_t ws_recv(httpd_req_t *req, httpd_ws_frame_t *frame, CommandRouter* router) {
    std::string cmd(reinterpret_cast<char*>(frame->payload), frame->len);
    std::string res = router->route(cmd);
    httpd_ws_frame_t send_pkt;
    send_pkt.type = HTTPD_WS_TYPE_TEXT;
    send_pkt.len = res.size();
    send_pkt.payload = (uint8_t*)res.c_str();
    return httpd_ws_send_frame(req, &send_pkt);
}

static esp_err_t ws_handler_wrapper(httpd_req_t *req) {
    if (req->method == HTTP_GET) {
        return ws_handler(req);
    }
    CommandRouter* router = (CommandRouter*)req->user_ctx;
    httpd_ws_frame_t frame;
    memset(&frame, 0, sizeof(httpd_ws_frame_t));
    frame.type = HTTPD_WS_TYPE_TEXT;
    httpd_ws_recv_frame(req, &frame, 0);
    std::vector<uint8_t> buf(frame.len + 1);
    frame.payload = buf.data();
    httpd_ws_recv_frame(req, &frame, frame.len);
    return ws_recv(req, &frame, router);
}

static httpd_handle_t start_server(CommandRouter* router) {
    httpd_config_t config = HTTPD_DEFAULT_CONFIG();
    httpd_handle_t server = NULL;
    if (httpd_start(&server, &config) == ESP_OK) {
        extern const unsigned char index_start[] asm("_binary_src_ui_webcli_index_html_start");
        extern const unsigned char index_end[] asm("_binary_src_ui_webcli_index_html_end");
        const size_t index_len = index_end - index_start;
        httpd_uri_t root = {
            .uri = "/",
            .method = HTTP_GET,
            .handler = [](httpd_req_t *req){
                extern const unsigned char index_start[] asm("_binary_src_ui_webcli_index_html_start");
                extern const unsigned char index_end[] asm("_binary_src_ui_webcli_index_html_end");
                const size_t index_len = index_end - index_start;
                httpd_resp_set_type(req, "text/html");
                httpd_resp_send(req, (const char*)index_start, index_len);
                return ESP_OK;
            },
            .user_ctx = NULL
        };
        httpd_register_uri_handler(server, &root);

        httpd_uri_t ws = {
            .uri = "/ws",
            .method = HTTP_GET,
            .handler = ws_handler_wrapper,
            .user_ctx = router,
            .is_websocket = true,
            .handle_ws_control = NULL,
        };
        httpd_register_uri_handler(server, &ws);
    }
    return server;
}

extern "C" void app_main(void) {
    nvs_flash_init();
    esp_netif_init();
    esp_event_loop_create_default();

    wifi_init_config_t cfg = WIFI_INIT_CONFIG_DEFAULT();
    esp_wifi_init(&cfg);
    esp_wifi_set_mode(WIFI_MODE_AP);
    wifi_config_t wifi_config = {
        .ap = {
            .ssid = "RogueOS",
            .ssid_len = 0,
            .channel = 1,
            .password = "password",
            .max_connection = 4,
            .authmode = WIFI_AUTH_WPA_WPA2_PSK
        }
    };
    esp_wifi_set_config(WIFI_IF_AP, &wifi_config);
    esp_wifi_start();

    Serial.begin(115200);
    SerialCli serial;
    CommandRouter router([&serial](const std::string& cmd){ return serial.execute(cmd); });
    start_server(&router);

    while (true) {
        vTaskDelay(pdMS_TO_TICKS(1000));
    }
}
