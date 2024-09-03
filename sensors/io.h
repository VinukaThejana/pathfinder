/**
* Sensor configuration
*
*   SD Card reader
*       - CS   = 5
*       - MOSI = 23
*       - MISO = 19
*       - SCK  = 18
*
**/

#ifndef IO_H
#define IO_H

#include <Arduino.h>
#include <climits>
#include <cstring>
#include <SPI.h>
#include <SD.h>
#include "sensors.h"

#define SD_CS 5

class IO {
public:
    static ErrCode init() {
        if (!SD.begin(SD_CS)) {
            return ErrCode::FAIL;
        }
        return ErrCode::SUCCESS;
    }

    static ErrCode write(const char *path, const char *payload) {
        File file = SD.open(path, FILE_WRITE);
        if (!file) {
            return ErrCode::FILE_OPEN_FAILED;
        }
        file.println(payload);
        file.close();
        return ErrCode::SUCCESS;
    }

    static ErrCode read(const char *path, String &content) {
        File file = SD.open(path);
        if (!file) {
            return ErrCode::FILE_OPEN_FAILED;
        }

        if (!file.available()) {
            file.close();
            return ErrCode::READ_FAILED;
        }

        content = file.readStringUntil('\n');
        file.close();
        return ErrCode::SUCCESS;
    }
};

#endif // !IO_H
