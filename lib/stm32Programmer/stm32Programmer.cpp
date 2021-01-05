#include "stm32Programmer.h"

const String STM32_CHIPNAME[8] = {
  "Unknown Chip",
  "STM32F03xx4/6",
  "STM32F030x8/05x",
  "STM32F030xC",
  "STM32F103x4/6",
  "STM32F103x8/B",
  "STM32F103xC/D/E",
  "STM32F105/107"
};

STM32Programmer::STM32Programmer(/* args */)
{
}

STM32Programmer::~STM32Programmer()
{
}

void STM32Programmer::startFlashMode()
{
    /*
        To start stm32 into run/normal mode:
        1. Set BOOT0 PIN as HIGH
        2. Reset STM32 Board via NRST PIN
        3. Set ESP LED as HIGH to indicate that we're in FlashMode
    */
    digitalWrite(STM32_BOOT0_PIN, HIGH);
    delay(100);
    digitalWrite(STM32_NRST_PIN, LOW);
    digitalWrite(ESP_LED_PIN, LOW);
    delay(50);
    digitalWrite(STM32_NRST_PIN, HIGH);
    delay(200);
    digitalWrite(ESP_LED_PIN, HIGH);
}

void STM32Programmer::startNormalMode()
{
    
    /*
        To start stm32 into run/normal mode:
        1. Set BOOT0 PIN as LOW
        2. Reset STM32 Board via NRST PIN
        3. Set ESP LED as LOW to indicate that we're in run/normalMode
    */
    digitalWrite(STM32_BOOT0_PIN, LOW);
    delay(100);
    digitalWrite(STM32_NRST_PIN, LOW);
    digitalWrite(ESP_LED_PIN, LOW);
    delay(50);
    digitalWrite(STM32_NRST_PIN, HIGH);
    delay(200);
}

uint8_t STM32Programmer::getBoardId()
{
    Serial.flush();
    int command = STM32ERR;
    Serial.write(STM32INIT);
    delay(10); //Wait for a response
    if (Serial.available() > 0) command = Serial.read();
    if (command == STM32ACK) //Get Chip ID
    {
        return stm32GetId();
    }
    return STM32NACK;
}

bool  STM32Programmer::getBoardName(String * boardName)
{
    uint8_t chipId = this->getBoardId();
    if (chipId == STM32NACK) return false;
    *boardName = STM32_CHIPNAME[chipId];
    return true;  
}

STM32Programmer STM32Prog;