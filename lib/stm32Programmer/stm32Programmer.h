#ifndef STM32_PROGRAMMER_H
#define STM32_PROGRAMMER_H
#include <Arduino.h>
#include <stm32ota.h>

#define STM32_NRST_PIN 5
#define STM32_BOOT0_PIN 4
#define ESP_LED_PIN 2

class STM32Programmer
{
private:
    uint8_t getBoardId();
public:
    STM32Programmer(/* args */);
    ~STM32Programmer();
    void startFlashMode();
    void startNormalMode();
    bool getBoardName(String * boardName);
};

extern STM32Programmer STM32Prog;
#endif