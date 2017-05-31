#include "TouchDetector.h"

#include "LogWrapper.h"

GestureDetector::GestureDetector() { dp_factor_ = 1.f; }

void GestureDetector::SetConfiguration(AConfiguration* config) {
    dp_factor_ = 160.f / AConfiguration_getDensity(config);
}

//--------------------------------------------------------------------------------
// TapDetector
//--------------------------------------------------------------------------------
TapDetector::TapDetector() : down_x_(0), down_y_(0) {}

GESTURE_STATE TapDetector::Detect(const AInputEvent* motion_event) {
    if (AMotionEvent_getPointerCount(motion_event) > 1) {
        // Only support single touch
        return false;
    }

    int32_t action = AMotionEvent_getAction(motion_event);
    unsigned int flags = action & AMOTION_EVENT_ACTION_MASK;
    switch (flags) {
        case AMOTION_EVENT_ACTION_DOWN:
            down_pointer_id_ = AMotionEvent_getPointerId(motion_event, 0);
            down_x_ = AMotionEvent_getX(motion_event, 0);
            down_y_ = AMotionEvent_getY(motion_event, 0);
            break;
        case AMOTION_EVENT_ACTION_UP: {
            int64_t eventTime = AMotionEvent_getEventTime(motion_event);
            int64_t downTime = AMotionEvent_getDownTime(motion_event);
            if (eventTime - downTime <= TAP_TIMEOUT) {
                if (down_pointer_id_ == AMotionEvent_getPointerId(motion_event, 0)) {
                    float x = AMotionEvent_getX(motion_event, 0) - down_x_;
                    float y = AMotionEvent_getY(motion_event, 0) - down_y_;
                    if (x * x + y * y < TOUCH_SLOP * TOUCH_SLOP * dp_factor_) {
                        LogWrapper::info("TapDetector: Tap detected");
                        return GESTURE_STATE_ACTION;
                    }
                }
            }
            break;
        }
    }
    return GESTURE_STATE_NONE;
}