#ifndef SENSORS_H
#define SENSORS_H

enum class ErrCode {
    SUCCESS,
    FAIL,
    TIMEOUT,
    INVALIE_READING,
    FILE_OPEN_FAILED,
    WRITE_FAILED,
    READ_FAILED,
    WIRING_FAILED,
};

#endif // !SENSORS_H
