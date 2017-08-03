
#include "TimeManager.h"
#include "Log.h"

double const TARGET_FRAME_RATE = 60.0;
double const TARGET_FRAME_TIME = 1.0 / TARGET_FRAME_RATE;

double const GIGA = 1.0e9;
double const NANO = 1.0e-9;

TimeManager::TimeManager() :  mElapsed {},
                              mLastGetTime {},
                              mTimer{},
                              mFrameTime{TARGET_FRAME_TIME},
                              mNowTime{},
                              mSleepTime{},
                              mFPS{} {
    mLastGetTime = GetTimeNow();
    mTimer = GetTimeNow();
}

void TimeManager::UpdateMainLoop() {
    mTimer += TARGET_FRAME_TIME;

    mNowTime = GetTimeNow();
    mFrameTime = mNowTime - mLastGetTime;
    mLastGetTime = mNowTime;

    mFPS = 1.f / mFrameTime;
    mSleepTime = mTimer - mNowTime;

    mElapsed += mFrameTime;

    if (mSleepTime > 0.f) {
        mSleepTS.tv_sec = static_cast<int32_t>(mSleepTime);

        if(mSleepTS.tv_sec != 0) {
            mSleepTS.tv_nsec = static_cast<int32_t>(
                    mSleepTime - static_cast<int32_t>(mSleepTime * GIGA) );
        }
        else {
            mSleepTS.tv_nsec = static_cast<int32_t>(mSleepTime * GIGA);
        }

        nanosleep(&mSleepTS, nullptr);
    }
}

double TimeManager::GetTimeNow() const noexcept {
    timespec timeVal;
    clock_gettime(CLOCK_MONOTONIC, &timeVal);

    return timeVal.tv_sec + timeVal.tv_nsec * NANO;
}

void TimeManager::Reset() noexcept {
    mFrameTime = 0.f;
    mLastGetTime = GetTimeNow();
    mTimer = GetTimeNow();
}

double TimeManager::FrameTime() noexcept {
    return mFrameTime;
}

double TimeManager::FramesPerSecond() noexcept {
    return mFPS;
}

double TimeManager::GetElapsed() const noexcept {
    return mElapsed;
}

void TimeManager::ResetElapsed() noexcept {
    mElapsed = 0.f;
}