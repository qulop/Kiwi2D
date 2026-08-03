#include "Time.hpp"

#include <common/sync/Thread.hpp>
#include <common/Debug.hpp>


namespace Kiwi {
    Time::TimeStorage Time::s_storage = {};



    f32 Time::DeltaTime() {
        return std::chrono::duration<f32, std::ratio<1, 1>>(DeltaTimeNative()).count();
    }

    Time::DurationType Time::DeltaTimeNative() {
        return DurationType(
            s_storage.deltaTime.load(std::memory_order_acquire)
        );
    }

    Time::TimePointType Time::Now() {
        return ClockType::now();
    }

    Time::DurationType Time::NowAsDuration() {
        return Now().time_since_epoch();
    }

    Time::TimePointRepType Time::NowAsCount() {
        return NowAsDuration().count();
    }

    Time::DurationType Time::GetTime() {
        return NowAsDuration() - s_storage.engineInitializationPoint.time_since_epoch();
    }

    void Time::UpdateTime() {
        KIWI_ASSERT(ThisThread::IsMainThread(), "This function must be called from the main thread");
        KIWI_ASSERT_BASIC(s_storage.wasInitialized);

        const auto nowCount = NowAsCount();

        s_storage.deltaTime.store(
            nowCount - s_storage.lastFrameTime.load(std::memory_order_relaxed),
            std::memory_order_release
        );

        s_storage.lastFrameTime.store(nowCount);
    }

    void Time::SetInitializationPoint() {
        KIWI_ASSERT(!s_storage.wasInitialized, "Time::SetInitializationPoint() should be invoked only once");

        const TimePointType now = Now();
        s_storage.engineInitializationPoint = now;

        s_storage.lastFrameTime.store(now.time_since_epoch().count());
        s_storage.deltaTime.store(0);

        KIWI_SWITCH_BOOL(s_storage.wasInitialized);
    }


    StopWatch::TimePointType StopWatch::Now() {
        return ClockType::now();
    }

    StopWatch::StopWatch() :
      m_begin(StopWatch::Now())
    {}

    std::chrono::duration<f64, std::milli> StopWatch::GetElapsed() const {
        DurationType end = StopWatch::Now() - m_begin;

        return std::chrono::duration_cast<std::chrono::duration<f64, std::milli>>(end);
    }

    void StopWatch::Restart() {
        m_begin = StopWatch::Now();
    }
}
