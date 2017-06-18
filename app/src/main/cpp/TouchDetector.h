#pragma once

#include <android_native_app_glue.h>
#include <cstdint>
#include <android/configuration.h>
#include <android/input.h>

int32_t const TAP_TIMEOUT = 180 * 1000000;
int32_t const TOUCH_SLOP = 8;

enum {
    GESTURE_STATE_NONE = 0,
    GESTURE_STATE_START = 1,
    GESTURE_STATE_MOVE = 2,
    GESTURE_STATE_END = 4,
    GESTURE_STATE_ACTION = (GESTURE_STATE_START | GESTURE_STATE_END),
};
typedef int32_t TouchState;

class TouchDetector {
protected:
    float mDPFactor;

public:
    TouchDetector();
    virtual ~TouchDetector() = default;
    virtual void Init(AConfiguration *config);

    virtual TouchState Detect(const AInputEvent* motionEvent) = 0;
};


class TapDetector : public TouchDetector {
public:
    TapDetector(AConfiguration * config);

    virtual ~TapDetector() {}
    virtual TouchState Detect(const AInputEvent* motionEvent);

private:
    int32_t mDownPointerId;
    float mDownX;
    float mDownY;
};

