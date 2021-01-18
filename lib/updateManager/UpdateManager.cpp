#include "UpdateManager.h"
#include <FS.h>
#include "stm32Programmer.h"

#define OTA_UPDATE_TIMER_INTERVAL 86400000
UpdateManager::UpdateManager(/* args */)
{
    requireUpdate = true;
    checkForNewUpdate = true;
    updateTimer = millis();
}

UpdateManager::~UpdateManager()
{
}

/* Private Methods */


bool UpdateManager::checkSTM32Update()
{
    uint8_t currentVersion = 1; //TODO: get version from STM32 and check if an serial bridge error exists
    String newVersionStr = "1";

    if (!SPIFFS.exists("/stm32.ver")) return false;
    File versionFS = SPIFFS.open("/stm32.ver", "r");
    if (!versionFS) return false;

    newVersionStr = versionFS.readStringUntil("\n");
    int newVersion = atoi(newVersionStr);

    /* Check if a new firmware update is required */
    if (currentVersion < newVersion) return true;

    return false;
}


/* Public Methods */

void UpdateManager::begin()
{

}

void UpdateManager::loop()
{

    if (millis() - updateTimer > OTA_UPDATE_TIMER_INTERVAL)
    {
        updateTimer = millis();
        checkForNewUpdate = true;
    }

    if (!checkForNewUpdate) return;
    checkForNewUpdate = false;

    t_httpUpdate_return status = this->checkUpdateSpiffs();

    if (t_httpUpdate_return == HTTP_UPDATE_OK && this->checkSTM32Update())
    {
        this->update();
    }
}

t_httpUpdate_return UpdateManager::checkUpdateSpiffs()
{
    String spiffsVer = "STM32-0";
    File f = SPIFFS.open("/spiffs.ver", "r");
    if (!f) return HTTP_UPDATE_FAILED;
    
    spiffsVer = f.readStringUntil('\n');

    int otaPort = 199;
    char otaServerHost[60];
    strcpy(otaServerHost, "192.168.1.103");
    
    char url[120];
    sprintf(url, "http://%s:%d/amperama/updatespiffs", otaServerHost, otaPort);
    
    t_httpUpdate_return ret = ESPhttpUpdate.updateSpiffs(url, spiffsVer);

    #ifdef DEBUG
    switch(ret)
    {
        case HTTP_UPDATE_FAILED:
        DEBUG_PORT.println("Update failed");
        break;
        case HTTP_UPDATE_NO_UPDATES:
        DEBUG_PORT.println("no update required");
        break;
        case HTTP_UPDATE_OK:
        DEBUG_PORT.println("Update ok");
        break;
    }
    #endif

    ledManager.setR(false);
    ledManager.setG(false);

    return ret;

}


bool UpdateManager::update()
{
    if (!requireUpdate) return false;
    requireUpdate = false;
    bool success = STM32Prog.program((char*)"/stm32Fw.bin");
    return success;
}