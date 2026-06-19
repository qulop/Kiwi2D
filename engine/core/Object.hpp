#pragma once

#include <core/LogLevel.hpp>

#include <common/meta/TypeMetaInfo.hpp>
#include <common/Debug.hpp>

#include <common/sync/Thread.hpp>


#define KIWI_CREATE_OBJECT(ClassName, ParentName)                                               \
    public:                                                                                     \
        using Super = ParentName;                                                               \
        using SelfType = ClassName;                                                             \
                                                                                                \
    public:                                                                                     \
        KIWI_NODISCARD static const TypeMetaInfo& GetStaticTypeMetaInfo() {                     \
            static_assert(std::is_class_v<ClassName>);                                          \
            static_assert(std::is_class_v<ParentName>);                                         \
                                                                                                \
            static auto selfTypeMetaInfo = KIWI_CREATE_TYPEINFO_FUNC(ClassName);                \
            return selfTypeMetaInfo;                                                            \
        }                                                                                       \
                                                                                                \
        KIWI_NODISCARD static Kiwi::Hash64 GetStaticType() {                                    \
            return GetStaticTypeMetaInfo().GetType();                                           \
        }                                                                                       \
                                                                                                \
        KIWI_NODISCARD static const Kiwi::String& GetStaticTypeName() {                         \
            return GetStaticTypeMetaInfo().GetTypeName();                                       \
        }                                                                                       \
                                                                                                \
        KIWI_NODISCARD virtual const TypeMetaInfo& GetTypeMetaInfo() const override {           \
            return GetStaticTypeMetaInfo();                                                     \
        }                                                                                       \
                                                                                                \
        KIWI_NODISCARD virtual Kiwi::Hash64 GetType() const override {                          \
            return GetStaticType();                                                             \
        }                                                                                       \
                                                                                                \
        KIWI_NODISCARD virtual const Kiwi::String& GetTypeName() const override {               \
            return GetStaticTypeName();                                                         \
        }                                                                                       \
                                                                                                \
        protected:                                                                              \
            virtual void DirectInheritanceChecker(ClassName*) {}                                \
            void DirectInheritanceChecker(ParentName*) final override {}


/*  Will produce log with method signature which consists of a class name and a class method, separated with '::' sign.
    For example, `KIWI_CTX_LOG(WARNING, "Hello World") can produce such output:
        `ExampleClass::ExampleFunc() : Hello World` */
#define KIWI_CTX_LOG(verb, msg, ...)    \
    CtxLog(__func__, ELogLevel::verb##_LOG, msg __VA_OPT__(,) __VA_ARGS__)

#define KIWI_LOG(verb, msg, ...)        \
    Log(ELogLevel::verb##_LOG, msg __VA_OPT__(,) __VA_ARGS__)


namespace Kiwi {
    class Application;
    class ASubsystem;


    class AObject {
        using SubsystemHolder = std::vector<std::shared_ptr<ASubsystem>>;

    public:
        using SelfType = AObject;

    public:
        KIWI_NODISCARD virtual const TypeMetaInfo& GetTypeMetaInfo() const = 0;
        KIWI_NODISCARD virtual Hash64 GetType() const = 0;
        KIWI_NODISCARD virtual const String& GetTypeName() const = 0;

        virtual ~AObject() = default;

    protected:
        KIWI_NODISCARD const AObject* GetObjectBase() const;

        void RegisterSubsystem(ASubsystem* subsystem);
        void DestroySubsystem(Hash64 subsystemStaticTypeHash);
        static std::shared_ptr<ASubsystem> GetSubsystem(Hash64 subsystemStaticTypeHash);

        template<Concepts::DerivedFrom<ASubsystem> T>
        void RegisterSubsystem(T* subsystem) {
            RegisterSubsystem(BasicCast::HierarchyCast<ASubsystem*>(subsystem));
        }

        template<Concepts::DerivedFrom<ASubsystem> T, typename... Args>
        void RegisterSubsystem(Args&&... args) {
            ASubsystem* subsystem = KIWI_NOTHROW_NEW T(std::forward<Args>(args)...);
            RegisterSubsystem(subsystem);
        }

        template<Concepts::DerivedFrom<ASubsystem> T>
        void DestroySubsystem() {
            DestroySubsystem(T::GetStaticType());
        }

        template<Concepts::DerivedFrom<ASubsystem> T>
        KIWI_NODISCARD static std::shared_ptr<T> GetSubsystem() {
            return std::static_pointer_cast<T>(GetSubsystem(T::GetStaticType()));
        }

        template<typename... Args>
        static void Log(ELogLevel lvl, std::format_string<Args...> fmt, Args&&... args) {
            LogImpl(lvl, String::Format(fmt, std::forward<Args>(args)...));
        }

        template<typename... Args>
        void CtxLog(const char* funcName, ELogLevel lvl, std::format_string<Args...> fmt, Args&&... args) const {
            String fullSigName = String::Format("{}::{}()",
                GetTypeName(), funcName
            );

            LogImpl(lvl, String::Format("{} : {}",
                fullSigName, String::Format(fmt, std::forward<Args>(args)...))
            );
        }

        virtual void DirectInheritanceChecker(AObject*) = 0;

    private:
        void ShutdownAllSubsystems();
        static void LogImpl(ELogLevel lvl, const String& msg);

    private:
        friend class Application;

        TypeMetaInfo m_typeMetaInfo;
        static SubsystemHolder* s_subsystems;
    };


    class ASubsystem : public AObject {
        KIWI_CREATE_OBJECT(ASubsystem, AObject)

    public:
        virtual bool Init();
        virtual void DeInit();

        ~ASubsystem() override = default;
    };
}