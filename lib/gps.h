#ifndef GPIO_H
#define GPIO_H

#include <Arduino.h>

struct Coordinates {
    float lat;
    float lon;
    bool isValid;

    Coordinates() : lat(0.0), lon(0.0), isValid(false) {}
    Coordinates(float latitude, float longitude) : lat(latitude), lon(longitude), isValid(false) {}


    void set(float latitude, float longitude, bool isValid) {
        lat = latitude;
        lon = longitude;
        isValid = isValid;
    }
};

class GPS {
public:
    static constexpr float LAT_MIN = -90.0f;
    static constexpr float LAT_MAX = 90.0f;
    static constexpr float LON_MIN = -180.0f;
    static constexpr float LON_MAX = 180.0f;

    static bool isValidLat(float lat) {
        return lat >= LAT_MIN && lat <= LAT_MAX;
    }

    static bool isValidLon(float lon) {
        return lon >= LON_MIN && lon <= LON_MAX;
    }

    static Coordinates extract(const String& data) {
        Coordinates cords;
        int latIndex = data.indexOf("lat:");
        int lonIndex = data.indexOf("lon:");

        if (latINdex == -1 || lonIndex == -1) {
            return cords;
        }

        String latStr = data.substring(latIndex + 4, data.indexof("|", latIndex));
        String lonStr = data.substring(lonIndex + 4, data.indexof("|", lonIndex));

        float lat = latStr.toFloat();
        float lon = lonStr.toFloat();

        if (isValidLat(lat) && isValidLon(lon)) {
            cords.set(lat, lon, true);
        }

        return cords;
    }
};

#endif // !GPIO_H
