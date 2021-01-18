#include "stm32Programmer.h"
#include <FS.h>

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

bool STM32Programmer::programByte(unsigned char * firmwareBuff, uint8_t address)
{
    bool timeoutOcurred = false;
    uint32_t processTimeout = millis();
    uint8_t rxCommand = 255;

    stm32SendCommand(STM32WR); //SEND Write Address command

    // update process timer
    processTimeout = millis();

    while(!Serial.available() && !timeoutOcurred)
    {
        timeoutOcurred =  ((millis() - processTimeout) > 10000);
    }

    if (timeoutOcurred) return false; //exit when a timeout  ocurred

    /* Get STM32 Response */
    rxCommand = Serial.read();

    if (rxCommand == STM32ACK)
    {
        rxCommand = stm32Address(STM32STADDR + (256 * address)) == STM32ACK; // add timeout

        if (!rxCommand)
        {
            return false;
        }

        rxCommand = stm32SendData(firmwareBuff, 255) == STM32ACK; //program data // add timeout

        if (!rxCommand)
        {
            return false;
        }
    }
    else return false;

    return true;
}

bool  STM32Programmer::getBoardName(String * boardName)
{
    uint8_t chipId = this->getBoardId();
    if (chipId == STM32NACK) return false;
    *boardName = STM32_CHIPNAME[chipId];
    return true;  
}

bool STM32Programmer::program(char * firmwareName)
{

    if (!SPIFFS.exists(firmwareName))
    {
        return false; //TODO: add program status
    }

    File firmwareFs;

    firmwareFs = SPIFFS.open(firmwareName, "r");

    if (!firmwareFs) 
    {
        return false; //TODO: add program status
    }

    uint8_t bini = 0;
    uint8_t lastBuf = 0;

    uint8_t firmwareBuffer[256];

    bool programOK = false; //program process success flag

    bini = firmwareFs.size() / 256; //byte to bits conversion
    lastBuf = firmwareFs.size() % 256;

    for (uint8_t i = 0; i < bini; i++)
    {
        firmwareFs.read(firmwareBuffer, 256);
        programOK = this->programByte(firmwareBuffer, i);
        if (!programOK) break;
    }

    if (programOK)
    {
        /* Send remainder data to STM32 */
        firmwareFs.read(firmwareBuffer, lastBuf);
        programOK = this->programByte(firmwareBuffer, lastBuf);
    }
    firmwareFs.close();
    return programOK;
}

STM32Programmer STM32Prog;