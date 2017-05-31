#pragma once

#include <android_native_app_glue.h>

const int32_t DOUBLE_TAP_TIMEOUT = 300 * 1000000;
const int32_t TAP_TIMEOUT = 180 * 1000000;
const int32_t DOUBLE_TAP_SLOP = 100;
const int32_t TOUCH_SLOP = 8;

enum {
    GESTURE_STATE_NONE = 0,
    GESTURE_STATE_START = 1,
    GESTURE_STATE_MOVE = 2,
    GESTURE_STATE_END = 4,
    GESTURE_STATE_ACTION = (GESTURE_STATE_START | GESTURE_STATE_END),
};
typedef int32_t GESTURE_STATE;

/******************************************************************
 * Base class of Gesture Detectors
 * GestureDetectors handles input events and detect gestures
 * Note that different detectors may detect gestures with an event at
 * same time. The caller needs to manage gesture priority accordingly
 *
 */
class GestureDetector {
protected:
    float dp_factor_;

public:
    GestureDetector();
    virtual ~GestureDetector() {}
    virtual void SetConfiguration(AConfiguration* config);

    virtual GESTURE_STATE Detect(const AInputEvent* motion_event) = 0;
};

/******************************************************************
 * Tap gesture detector
 * Returns GESTURE_STATE_ACTION when a tap gesture is detected
 *
 */
class TapDetector : public GestureDetector {
private:
    int32_t down_pointer_id_;
    float down_x_;
    float down_y_;

    TapDetector();
    TapDetector(TapDetector const &);
    TapDetector & operator=(TapDetector const &);

public:
    static TapDetector * GetInstance() {
        static TapDetector instance;
        return &instance;
    }

    virtual ~TapDetector() {}
    virtual GESTURE_STATE Detect(const AInputEvent* motion_event);
};

