#include <cmath>

#include "SensorManager.h"
#include "LogWrapper.h"

SensorManager::SensorManager(android_app * app)
        : sensorManager_(nullptr),
          accelerometerSensor_(nullptr),
          sensorEventQueue_(nullptr)
{
    sensorManager_ = ASensorManager_getInstance();
    accelerometerSensor_ = ASensorManager_getDefaultSensor(sensorManager_,
                                                           ASENSOR_TYPE_ACCELEROMETER);
    sensorEventQueue_ = ASensorManager_createEventQueue(
            sensorManager_,
            app->looper,
            LOOPER_ID_USER,
            NULL,
            NULL);
}

SensorManager::~SensorManager() {}


void SensorManager::Process(const int32_t id) {
    // If a sensor has data, process it now.
    if (id == LOOPER_ID_USER) {
        if (accelerometerSensor_ != NULL) {
            ASensorEvent event;
            while (ASensorEventQueue_getEvents(sensorEventQueue_, &event, 1) > 0) {
                float maginitude = event.acceleration.x * event.acceleration.x +
                                   event.acceleration.y * event.acceleration.y;
                if (maginitude * 4 >= event.acceleration.z * event.acceleration.z) {
                    Orientation orientation = Orientation::ORIENTATION_REVERSE_LANDSCAPE;
                    float angle = atan2f(-event.acceleration.y, event.acceleration.x);
                    if (angle <= -M_PI_2 - M_PI_4) {
                        orientation = Orientation::ORIENTATION_REVERSE_LANDSCAPE;
                    } else if (angle <= -M_PI_4) {
                        orientation = Orientation::ORIENTATION_PORTRAIT;
                    } else if (angle <= M_PI_4) {
                        orientation = Orientation::ORIENTATION_LANDSCAPE;
                    } else if (angle <= M_PI_2 + M_PI_4) {
                        orientation = Orientation::ORIENTATION_REVERSE_PORTRAIT;
                    }

//                    LogWrapper::info( "orientation %f %d", angle, orientation);
                }
            }
        }
    }
}

void SensorManager::Resume() {
    // When the app gains focus, start monitoring the accelerometer.
    if (accelerometerSensor_ != NULL) {
        ASensorEventQueue_enableSensor(sensorEventQueue_, accelerometerSensor_);
        // We'd like to get 60 events per second (in us).
        ASensorEventQueue_setEventRate(sensorEventQueue_, accelerometerSensor_,
                                       (1000L / 60) * 1000);
    }
}

void SensorManager::Suspend() {
    // When the app loses focus, stop monitoring the accelerometer.
    // This is to avoid consuming battery while not being used.
    if (accelerometerSensor_ != NULL) {
        ASensorEventQueue_disableSensor(sensorEventQueue_, accelerometerSensor_);
    }
}