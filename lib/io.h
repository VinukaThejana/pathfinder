#ifndef IO_H
#define IO_H

#include <Arduino.h>
#include <climits>
#include <cstring>
#include <SPI.h>
#include <SD.h>

class IO {
public:
    enum ErrCode {
        SUCCESS = 0,
        FILE_OPEN_FAILED,
        WRITE_FAILED,
        READ_FAILED
    };

    static ErrCode write(const char *path, const char *payload) {
        File file = SD.open(path, FILE_WRITE);
        if (!file) {
            return FILE_OPEN_FAILED;
        }
        file.println(payload);
        file.close();
        return SUCCESS;
    }

    static ErrCode read(const char *path, String &content) {
        File file = SD.open(path);
        if (!file) {
            return FILE_OPEN_FAILED;
        }

        if (!file.available()) {
            file.close();
            return READ_FAILED;
        }

        content = file.readStringUntil('\n');
        file.close();
        return SUCCESS;
    }
};

#endif // !IO_H
