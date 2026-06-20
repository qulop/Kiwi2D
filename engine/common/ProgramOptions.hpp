#pragma once

#include <common/PCH.hpp>

#include <common/cast/Cast.hpp>
#include <common/Definitions.hpp>
#include <common/types/String.hpp>
#include <common/types/Opt.hpp>


namespace Kiwi {
    namespace EOptionArgType {
        enum Type : u8 {
            NONE,
            INT, BOOL,
            STRING,
            PATH
        };
        
        String ToString(Type t);
    };


    struct CmdLineOption {
    public:
        String name;
        EOptionArgType::Type type = EOptionArgType::Type::STRING;

    public:
        CmdLineOption() = default;
        CmdLineOption(String name, EOptionArgType::Type type) :
            name(std::move(name)),
            type(type)
        {}

    public:
        KIWI_NODISCARD static EOptionArgType::Type DeduceArgumentType(StringView arg) noexcept;
        KIWI_NODISCARD static bool CheckArgumentType(StringView arg, EOptionArgType::Type expected) noexcept;
    };


    class KIWI_API ProgramOptions {
        using ArgumentType = std::variant<std::monostate, i32, bool, String, std::filesystem::path>;
        using ArgumentsMapType = std::unordered_map<String, ArgumentType>;

    public:
        KIWI_NODISCARD static Opt<ProgramOptions> Parse(const std::vector<String>& args);

    public:
        ProgramOptions() = default;

        ProgramOptions(const ProgramOptions& other);
        ProgramOptions(ProgramOptions&& other) noexcept;

        KIWI_NODISCARD bool IsEmpty() const;
        KIWI_NODISCARD bool HasOption(const String& opt) const;

        template<typename T>
        KIWI_NODISCARD Opt<T> Get(const String& opt) const {
            if (!HasOption(opt) || std::holds_alternative<std::monostate>(m_options.at(opt))) {
                return ZERO_OPT;
            }

            if (const T* res = std::get_if<T>(&m_options.at(opt))) {
                return *res;
            }

            return ZERO_OPT;
        }

        ProgramOptions& operator=(const ProgramOptions& other);
        ProgramOptions& operator=(ProgramOptions&& other) noexcept;


        ~ProgramOptions() = default;
        
    private:
        KIWI_NODISCARD static Opt<String> GetOptionName(StringView opt);
        KIWI_NODISCARD static Opt<ArgumentType> ParseArgument(StringView arg, const CmdLineOption& opt);

    private:
        ArgumentsMapType m_options;
    };
}