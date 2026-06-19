#include "Object.hpp"

#include <common/sync/Thread.hpp>

#include <core/LogSubsystem.hpp>



namespace Kiwi {
    AObject::SubsystemHolder* AObject::s_subsystems = nullptr;


    const AObject* AObject::GetObjectBase() const {
        return this;
    }

    void AObject::RegisterSubsystem(ASubsystem* subsystem) {
        KIWI_ASSERT_BASIC(s_subsystems);
        KIWI_ASSERT(ThisThread::IsMainThread(), "You must call this function only from the main thread");

        // We don't want to register the same subsystem twice
        KIWI_ENSURE(GetSubsystem(subsystem->GetType()) == nullptr);

        s_subsystems->emplace_back(subsystem);
    }

    void AObject::DestroySubsystem(Hash64 subsystemStaticTypeHash) {
        KIWI_ASSERT_BASIC(s_subsystems);
        KIWI_ASSERT(ThisThread::IsMainThread(), "You must call this function only from the main thread");

        auto it = std::ranges::find_if(*s_subsystems,
    [subsystemStaticTypeHash](const std::shared_ptr<ASubsystem>& subsystem)
              {
                  return subsystem->GetType() == subsystemStaticTypeHash;
              }
        );

        if (it != std::ranges::end(*s_subsystems)) {
            it->get()->DeInit();
            it->reset();
        }
    }

    std::shared_ptr<ASubsystem> AObject::GetSubsystem(Hash64 subsystemStaticTypeHash) {
        KIWI_ASSERT_BASIC(s_subsystems);

        auto it = std::ranges::find_if(*s_subsystems,
        [subsystemStaticTypeHash](const std::shared_ptr<ASubsystem>& subsystem)
              {
                  return subsystem->GetType() == subsystemStaticTypeHash;
              }
        );

        return it != std::ranges::end(*s_subsystems) ? *it : nullptr;
    }


    void AObject::ShutdownAllSubsystems() {
        KIWI_ASSERT_BASIC(s_subsystems);
        KIWI_ASSERT(ThisThread::IsMainThread(), "You must call this function only from the main thread");

        for (std::shared_ptr<ASubsystem>& subsystem : *s_subsystems | std::views::reverse) {
            DestroySubsystem(subsystem->GetType());
        }
    }

    void AObject::LogImpl(ELogLevel lvl, const String& msg) {
        std::shared_ptr<LogSubsystem> logSubsystem = GetSubsystem<LogSubsystem>();
        KIWI_ENSURE(logSubsystem);

        logSubsystem->Log(lvl, msg);
    }



    bool ASubsystem::Init() {
        KIWI_ASSERT(ThisThread::IsMainThread(), "A subsystem must be initialized only from a main thread");

        return true;
    }

    void ASubsystem::DeInit() {
        KIWI_ASSERT(ThisThread::IsMainThread(), "A subsystem must be deinitialized only from a main thread");
    }
}
