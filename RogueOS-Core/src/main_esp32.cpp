#include "cli/router.h"
#include <Arduino.h>
#include <WiFi.h>
#include <ESPAsyncWebServer.h>
#include <AsyncTCP.h>
#include <lvgl.h>
#include <string>

AsyncWebServer server(80);
AsyncWebSocket ws("/ws");

std::string processInput(const std::string& input) {
    return router::handleCommand(input);
}

void onWsEvent(AsyncWebSocket * server, AsyncWebSocketClient * client,
               AwsEventType type, void * arg, uint8_t *data, size_t len) {
    if (type == WS_EVT_DATA) {
        std::string cmd((char*)data, len);
        std::string res = processInput(cmd);
        client->text(res.c_str());
    }
}

extern "C" void app_main(void) {
    lv_init();
    lv_obj_t * label = lv_label_create(lv_scr_act());
    lv_label_set_text(label, "RogueOS");

    lv_obj_t * btn = lv_btn_create(lv_scr_act());
    lv_obj_add_event_cb(btn, [](lv_event_t * e){
        processInput("help");
    }, LV_EVENT_CLICKED, NULL);
    lv_obj_align(btn, LV_ALIGN_BOTTOM_MID, 0, -10);

    WiFi.mode(WIFI_AP);
    WiFi.softAP("RogueOS", "password");

    ws.onEvent(onWsEvent);
    server.addHandler(&ws);
    server.begin();

    while (true) {
        lv_timer_handler();
        vTaskDelay(pdMS_TO_TICKS(5));
    }
}
