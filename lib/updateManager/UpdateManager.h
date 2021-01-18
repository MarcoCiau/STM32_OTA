#ifndef UPDATE_MANAGER_H
#define UPDATE_MANAGER_H
#include <Arduino.h>
#include <ArduinoOTA.h>
#include <ESP8266HTTPClient.h>
#include <ESP8266httpUpdate.h>

class UpdateManager
{
private:
    bool checkForNewUpdate;
    bool requireUpdate;

    uint32_t updateTimer;
    /* Check if STM32 device require new firmware update */
    bool checkSTM32Update();
public:
    UpdateManager(/* args */);
    ~UpdateManager();
    void begin();
    void loop();

    /* Check if a new firmware is available from OTA Server and donwload it to the SPIFFS */
    t_httpUpdate_return checkUpdateSpiffs();

    /* Program STM32 Device */
    bool update();
};





#endif //UPDATE_MANAGER_H