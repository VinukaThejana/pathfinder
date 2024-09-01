#ifndef GPIO_H
#define GPIO_H

#include <Arduino.h>
#include <climits>
#include <cstring>

#define LAT "lat:"
#define LON "lon:"
#define SEPERATOR "|"

struct Coordinates {
    double lat;
    double lon;
    bool isValid;

    Coordinates() : lat(0.0), lon(0.0), isValid(false) {}
    Coordinates(double latitude, double longitude) : lat(latitude), lon(longitude), isValid(false) {}


    void set(double latitude, double longitude, bool isValid) {
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
        Coordinates coords;

        int latIndex = data.indexOf(LAT);
        if (latIndex == -1) {
            return coords;
        }
        int latEndIndex = data.indexOf(SEPERATOR, latIndex);
        if (latEndIndex == -1) latEndIndex = data.length();
        String latStr = data.substring(latIndex + strlen(LAT), latEndIndex);
        float lat = latStr.toDouble();
        if (!isValidLat(lat)) {
            return coords;
        }
        coords.lat = lat;

        int lonIndex = data.indexOf(LON);
        if (lonIndex == -1) {
            return coords;
        }
        int lonEndIndex = data.indexOf(SEPERATOR, lonIndex);
        if (lonEndIndex == -1) lonEndIndex = data.length();
        String lonStr = data.substring(lonIndex + strlen(LON), lonEndIndex);
        float lon = lonStr.toDouble();
        if (!isValidLon(lon)) {
            return coords;
        }
        coords.lon = lon;
        coords.isValid = true;

        return coords;
    }
};

#endif // !GPIO_H
