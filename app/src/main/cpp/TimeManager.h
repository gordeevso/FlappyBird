#pragma once

#include <ctime>

class TimeManager
{
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

