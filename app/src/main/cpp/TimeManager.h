#pragma once

#include <ctime>
#include "Log.h"

class TimeManager
{
public:

    static TimeManager & GetInstance() {
        static TimeManager instance;
        return instance;
    }

public:
    explicit TimeManager();
    ~TimeManager() = default;
    TimeManager(TimeManager const &) = delete;
    TimeManager & operator=(TimeManager const &) = delete;

    void UpdateMainLoop();

    double GetTimeNow() const noexcept;
    void Reset() noexcept;
    double FrameTime() noexcept;
    double FramesPerSecond() noexcept;
    double GetElapsed() const noexcept;
    void ResetElapsed() noexcept;
    double GetSleepTime() const noexcept;

private:
    double mElapsed;
    double mLastGetTime;
    double mTimer;
    double mFrameTime;
    double mNowTime;
    double mSleepTime;
    double mFPS;
    timespec mSleepTS;
};

