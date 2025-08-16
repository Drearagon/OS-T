#include "serial_cli.hpp"
#include <Arduino.h>

std::string SerialCli::execute(const std::string& cmd) {
    Serial.println(cmd.c_str());
    Serial.flush();
    std::string resp;
    unsigned long start = millis();
    while (millis() - start < 1000) {
        while (Serial.available()) {
            char c = Serial.read();
            if (c == '\n') {
                return resp;
            }
            resp += c;
        }
        delay(10);
    }
    return resp;
}
