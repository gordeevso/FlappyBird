#pragma once

#include <android/sensor.h>
#include <android_native_app_glue.h>

enum class Orientation {
    ORIENTATION_UNKNOWN = -1,
    ORIENTATION_PORTRAIT = 0,
    ORIENTATION_LANDSCAPE = 1,
    ORIENTATION_REVERSE_PORTRAIT = 2,
    ORIENTATION_REVERSE_LANDSCAPE = 3,
};

/*
 * Helper to handle sensor inputs such as accelerometer.
 * The helper also check for screen rotation
 *
 */
class SensorManager {
    ASensorManager *sensorManager_;
    const ASensor *accelerometerSensor_;
    ASensorEventQueue *sensorEventQueue_;

public:


    SensorManager(android_app * app);
    ~SensorManager();
    void Init(android_app *state);
    void Process(const int32_t id);
    void Suspend();
    void Resume();
};
