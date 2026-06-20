#include <platform/Platform.hpp>    // Logger doesn't initialize at this moment, so we need to use Console::Write() or Platform::writeInConsole()

#include <common/types/CString.hpp>
#include <common/types/String.hpp>
#include <common/cast/Cast.hpp>
#include <common/ProgramOptions.hpp>
#include <common/Debug.hpp>

#include <platform/io/SystemConsole.hpp>

#include <core/EngineConfig.hpp>


namespace Kiwi {
    namespace EOptionArgType {
        String ToString(Type t) {
             switch (t) {
                 case INT:
                     return "int";
                 case BOOL:
                     return "bool";
                 case STRING:
                     return "string";
                 case PATH:
                     return "path";
                 default:
                     return "none";
             }
        }
    }


    EOptionArgType::Type CmdLineOption::DeduceArgumentType(StringView arg) noexcept {
        KIWI_ASSERT_BASIC(!arg.empty() && !arg.starts_with('-'));

        if (arg == "true" || arg == "false") {
            return EOptionArgType::Type::BOOL;
        }
        if (String::ParseIntI64(arg)) {
            return EOptionArgType::Type::INT;
        }
        if (std::filesystem::exists(arg)) {
            return EOptionArgType::Type::PATH;
        }
        return EOptionArgType::Type::STRING;
    }

    bool CmdLineOption::CheckArgumentType(StringView arg, EOptionArgType::Type expected) noexcept {
        if (arg.empty() || arg.starts_with('-')) {
            return false;
        }

        return DeduceArgumentType(arg) == expected;
    }


    ProgramOptions::ProgramOptions(const ProgramOptions& other) {
        *this = other;
    }

    ProgramOptions::ProgramOptions(ProgramOptions&& other) noexcept {
        *this = std::move(other);
    }

    Opt<ProgramOptions> ProgramOptions::Parse(const std::vector<String>& args) {
        ProgramOptions result;
        std::vector<CmdLineOption> existingOptions = EngineConfig::GetCommandLineOptions();

        for (size_t tokenIndex = 0; tokenIndex < args.size(); tokenIndex++) {
            const String& token = args.at(tokenIndex);

            // Stars with '-' sign and contains the letter in the second cell
            if (!((token.Size() > 1 && token.StartsWith('-')) && std::isalpha(token.CharAt(1)))) {
                Console::WriteLine("An argument doesn't apply to any flag: \"{}\"", token);
                continue;
            }

            Opt<String> optionName = GetOptionName(token.ToStringView());
            if (!optionName) {
                Console::WriteLine("Token \"{}\" not recognized as an option", token);
                return ZERO_OPT;
            }

            auto foundOption = std::ranges::find(existingOptions, *optionName, &CmdLineOption::name);
            if (foundOption == existingOptions.end()) {
                Console::WriteLine("Unknown option \"{}\"", *optionName);
                return ZERO_OPT;
            }

            // Just a flag case - just adding the std::monostate{}(i.e. "nothing")
            if (foundOption->type == EOptionArgType::Type::NONE) {
                result.m_options[foundOption->name] = std::monostate{};
                continue;
            }

            tokenIndex += 1;
            const String& argToken = (tokenIndex < args.size()) ? args.at(tokenIndex) : String::EmptyString();

            Opt<ArgumentType> argument = ParseArgument(argToken.ToStringView(), *foundOption);
            if (!argument) {
                Console::WriteLine("Failed to parse an argument for option \"-{}\". Invalid token: \"{}\". <{}> type expected instead",
                    *optionName, argToken, EOptionArgType::ToString(foundOption->type)
                );

                return ZERO_OPT;
            }


            result.m_options[*optionName] = *argument;
        }

        return result;
    }

    bool ProgramOptions::IsEmpty() const {
        return m_options.empty();
    }

    bool ProgramOptions::HasOption(const String& opt) const {
        return m_options.contains(opt);
    }

    ProgramOptions& ProgramOptions::operator=(const ProgramOptions& other) {
        if (this == &other) {
            return *this;
        }

        m_options = other.m_options;

        return *this;
    }

    ProgramOptions& ProgramOptions::operator=(ProgramOptions&& other) noexcept {
        if (this == &other) {
            return *this;
        }

        m_options = std::move(other.m_options);

        return *this;
    }

    Opt<String> ProgramOptions::GetOptionName(StringView opt) {
        size_t beginOfFlagName = opt.find_first_not_of('-');
        if (beginOfFlagName == StringView::npos) {
            return ZERO_OPT;
        }

        return String{ opt.substr(beginOfFlagName) };
    }

    Opt<ProgramOptions::ArgumentType> ProgramOptions::ParseArgument(StringView arg, const CmdLineOption& opt) {
        KIWI_ASSERT(opt.type != EOptionArgType::Type::NONE, "Hmm... Looks like someone pass a wrong option here (๏ᆺ๏υ)");

        if (!CmdLineOption::CheckArgumentType(arg, opt.type)) {
            return ZERO_OPT;
        }

        if (opt.type == EOptionArgType::Type::INT) {
            return String::ParseIntI32(arg).GetValue();
        }
        if (opt.type == EOptionArgType::Type::BOOL) {
            return String::ParseBool(arg).GetValue();
        }
        if (opt.type == EOptionArgType::Type::PATH) {
            return std::filesystem::path { arg };
        }

        return String{ arg };
    }
}