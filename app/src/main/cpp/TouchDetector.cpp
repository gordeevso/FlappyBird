#include "TouchDetector.h"

#include "Log.h"

TouchDetector::TouchDetector() {
    mDPFactor = 1.f;
}

void TouchDetector::Init(AConfiguration *config) {
    mDPFactor = 160.f / AConfiguration_getDensity(config);
}


TapDetector::TapDetector(AConfiguration * config) : mDownX(0),
                                                    mDownY(0)
{
    Init(config);
}

TouchState TapDetector::Detect(AInputEvent const * motion_event) {
    if (AMotionEvent_getPointerCount(motion_event) > 1) {
        // Only support single touch
        return false;
    }

    int32_t action = AMotionEvent_getAction(motion_event);
    unsigned int flags = action & AMOTION_EVENT_ACTION_MASK;
    switch (flags) {
        case AMOTION_EVENT_ACTION_DOWN:
            mDownPointerId = AMotionEvent_getPointerId(motion_event, 0);
            mDownX = AMotionEvent_getX(motion_event, 0);
            mDownY = AMotionEvent_getY(motion_event, 0);
            break;
        case AMOTION_EVENT_ACTION_UP: {
            int64_t eventTime = AMotionEvent_getEventTime(motion_event);
            int64_t downTime = AMotionEvent_getDownTime(motion_event);
            if (eventTime - downTime <= TAP_TIMEOUT) {
                if (mDownPointerId == AMotionEvent_getPointerId(motion_event, 0)) {
                    float x = AMotionEvent_getX(motion_event, 0) - mDownX;
                    float y = AMotionEvent_getY(motion_event, 0) - mDownY;
                    if (x * x + y * y < TOUCH_SLOP * TOUCH_SLOP * mDPFactor) {
                        Log::info("TapDetector: Tap detected");
                        return GESTURE_STATE_ACTION;
                    }
                }
            }
            break;
        }
    }
    return GESTURE_STATE_NONE;
}