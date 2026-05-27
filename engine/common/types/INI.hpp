/*
    Created on: 26 Dec 2015
    Author: Fabian Meyer
    License: MIT
*/

#pragma once

#include <common/PCH.hpp>
#include <common/Definitions.hpp>


namespace Kiwi {
    /************************************************
     * Helper Functions
     ************************************************/

    /** Returns a string of whitespace characters. */
    constexpr const char* Whitespaces()
    {
        return " \t\n\r\f\v";
    }

    /** Returns a string of indentation characters. */
    constexpr const char* Indents()
    {
        return " \t";
    }

    /** Trims a string in place.
      * @param str string to be trimmed in place */
    inline void Trim(std::string &str)
    {
        // first erasing from end should be slighty more efficient
        // because erasing from start potentially moves all chars
        // multiple indices towards the front.

        auto lastpos = str.find_last_not_of(Whitespaces());
        if(lastpos == std::string::npos)
        {
            str.clear();
            return;
        }

        str.erase(lastpos + 1);
        str.erase(0, str.find_first_not_of(Whitespaces()));
    }

    /************************************************
     * Conversion Functors
     ************************************************/

    inline bool StringToLong(const std::string &value, long &result)
    {
        char *endptr;
        // check if decimal
        result = std::strtol(value.c_str(), &endptr, 10);
        if(*endptr == '\0')
            return true;
        // check if octal
        result = std::strtol(value.c_str(), &endptr, 8);
        if(*endptr == '\0')
            return true;
        // check if hex
        result = std::strtol(value.c_str(), &endptr, 16);
        if(*endptr == '\0')
            return true;

        return false;
    }

    inline bool StringToULong(const std::string &value, unsigned long &result)
    {
        char *endptr;
        // check if decimal
        result = std::strtoul(value.c_str(), &endptr, 10);
        if(*endptr == '\0')
            return true;
        // check if octal
        result = std::strtoul(value.c_str(), &endptr, 8);
        if(*endptr == '\0')
            return true;
        // check if hex
        result = std::strtoul(value.c_str(), &endptr, 16);
        if(*endptr == '\0')
            return true;

        return false;
    }

    template<typename T>
    struct Convert
    {};

    template<>
    struct Convert<bool>
    {
        void Decode(const std::string &value, bool &result)
        {
            std::string str(value);
            std::transform(str.begin(), str.end(), str.begin(), [](const char c){
                return static_cast<char>(::toupper(c));
            });

            if(str == "TRUE")
                result = true;
            else if(str == "FALSE")
                result = false;
            else
                throw std::invalid_argument("field is not a bool");
        }

        void Encode(const bool value, std::string &result)
        {
            result = value ? "true" : "false";
        }
    };

    template<>
    struct Convert<char>
    {
        void Decode(const std::string &value, char &result)
        {
            assert(value.size() > 0);
            result = value[0];
        }

        void Encode(const char value, std::string &result)
        {
            result = value;
        }
    };

    template<>
    struct Convert<unsigned char>
    {
        void Decode(const std::string &value, unsigned char &result)
        {
            assert(value.size() > 0);
            result = value[0];
        }

        void Encode(const unsigned char value, std::string &result)
        {
            result = value;
        }
    };

    template<>
    struct Convert<short>
    {
        void Decode(const std::string &value, short &result)
        {
            long tmp;
            if(!StringToLong(value, tmp))
                throw std::invalid_argument("field is not a short");
            result = static_cast<short>(tmp);
        }

        void Encode(const short value, std::string &result)
        {
            std::stringstream ss;
            ss << value;
            result = ss.str();
        }
    };

    template<>
    struct Convert<unsigned short>
    {
        void Decode(const std::string &value, unsigned short &result)
        {
            unsigned long tmp;
            if(!StringToULong(value, tmp))
                throw std::invalid_argument("field is not an unsigned short");
            result = static_cast<unsigned short>(tmp);
        }

        void Encode(const unsigned short value, std::string &result)
        {
            std::stringstream ss;
            ss << value;
            result = ss.str();
        }
    };

    template<>
    struct Convert<int>
    {
        void Decode(const std::string &value, int &result)
        {
            long tmp;
            if(!StringToLong(value, tmp))
                throw std::invalid_argument("field is not an int");
            result = static_cast<int>(tmp);
        }

        void Encode(const int value, std::string &result)
        {
            std::stringstream ss;
            ss << value;
            result = ss.str();
        }
    };

    template<>
    struct Convert<unsigned int>
    {
        void Decode(const std::string &value, unsigned int &result)
        {
            unsigned long tmp;
            if(!StringToULong(value, tmp))
                throw std::invalid_argument("field is not an unsigned int");
            result = static_cast<unsigned int>(tmp);
        }

        void Encode(const unsigned int value, std::string &result)
        {
            std::stringstream ss;
            ss << value;
            result = ss.str();
        }
    };

    template<>
    struct Convert<long>
    {
        void Decode(const std::string &value, long &result)
        {
            if(!StringToLong(value, result))
                throw std::invalid_argument("field is not a long");
        }

        void Encode(const long value, std::string &result)
        {
            std::stringstream ss;
            ss << value;
            result = ss.str();
        }
    };

    template<>
    struct Convert<unsigned long>
    {
        void Decode(const std::string &value, unsigned long &result)
        {
            if(!StringToULong(value, result))
                throw std::invalid_argument("field is not an unsigned long");
        }

        void Encode(const unsigned long value, std::string &result)
        {
            std::stringstream ss;
            ss << value;
            result = ss.str();
        }
    };

    template<>
    struct Convert<double>
    {
        void Decode(const std::string &value, double &result)
        {
            result = std::stod(value);
        }

        void Encode(const double value, std::string &result)
        {
            std::stringstream ss;
            ss << value;
            result = ss.str();
        }
    };

    template<>
    struct Convert<float>
    {
        void Decode(const std::string &value, float &result)
        {
            result = std::stof(value);
        }

        void Encode(const float value, std::string &result)
        {
            std::stringstream ss;
            ss << value;
            result = ss.str();
        }
    };

    template<>
    struct Convert<std::string>
    {
        void Decode(const std::string &value, std::string &result)
        {
            result = value;
        }

        void Encode(const std::string &value, std::string &result)
        {
            result = value;
        }
    };

    template<>
    struct Convert<String>
    {
        void Decode(const String &value, String &result)
        {
            result = value;
        }

        void Encode(const String &value, String &result)
        {
            result = value;
        }
    };

    template<>
    struct Convert<std::filesystem::path>
    {
        void Decode(const std::filesystem::path &value, std::filesystem::path &result)
        {
            result = value;
        }

        void Encode(const std::filesystem::path &value, std::filesystem::path &result)
        {
            result = value;
        }
    };

    template<>
    struct Convert<std::string_view>
    {
        void Decode(const std::string &value, std::string_view &result)
        {
            result = value;
        }

        void Encode(const std::string_view value, std::string &result)
        {
            result = value;
        }
    };

    template<>
    struct Convert<const char*>
    {
        void Encode(const char* const &value, std::string &result)
        {
            result = value;
        }

        void Decode(const std::string &value, const char* &result)
        {
            result = value.c_str();
        }
    };

    template<>
    struct Convert<char*>
    {
        void Encode(const char* const &value, std::string &result)
        {
            result = value;
        }
    };

    template<size_t n>
    struct Convert<char[n]>
    {
        void Encode(const char *value, std::string &result)
        {
            result = value;
        }
    };

    class IniField
    {
    private:
        std::string value_;

    public:
        IniField() : value_()
        {}

        IniField(const std::string &value) : value_(value)
        {}
        IniField(const IniField &field) : value_(field.value_)
        {}

        ~IniField()
        {}

        template<typename T>
        T As() const
        {
            Convert<T> conv;
            T result;
            conv.Decode(value_, result);
            return result;
        }

        template<typename T>
        IniField &operator=(const T &value)
        {
            Convert<T> conv;
            conv.Encode(value, value_);
            return *this;
        }

        IniField &operator=(const IniField &field)
        {
            value_ = field.value_;
            return *this;
        }
    };

    struct StringInsensitiveLess
    {
        using is_transparent = void;

        bool operator()(std::string lhs, std::string rhs) const
        {
                std::transform(lhs.begin(), lhs.end(), lhs.begin(), [](const char c){
                    return static_cast<char>(::tolower(c));
                });
                std::transform(rhs.begin(), rhs.end(), rhs.begin(), [](const char c){
                    return static_cast<char>(::tolower(c));
                });
                return  lhs < rhs;
        }

        bool operator()(std::string_view lhs, std::string_view rhs) const
        {
            // Fast case-insensitive comparison
            return std::lexicographical_compare(
                lhs.begin(), lhs.end(),
                rhs.begin(), rhs.end(),
                [](unsigned char a, unsigned char b) {
                    return ::tolower(a) < ::tolower(b);
                }
            );
        }
    };

    template <typename Comparator>
    class IniSectionBase : public std::map<std::string, IniField, Comparator>
    {
        using Super = std::map<std::string, IniField, Comparator>;

    public:
        IniSectionBase()
        {}
        ~IniSectionBase()
        {}

        using Super::operator[];

        KIWI_NODISCARD IniField& operator[](std::string_view sv)
        {
            auto it = this->find(sv);
            if (it != this->end())
            {
                return it->second;
            }

            return Super::operator[](std::string(sv));
        }

        KIWI_NODISCARD const IniField& operator[](std::string_view sv) const
        {
            return operator[](sv);
        }

    };

    using IniSectionCaseInsensitive = IniSectionBase<StringInsensitiveLess>;

    template <typename Comparator>
    class IniFileBase : private std::map<std::string, IniSectionBase<Comparator>, Comparator>
    {
        using Super = std::map<std::string, IniSectionBase<Comparator>, Comparator>;

    public:
        KIWI_NODISCARD static IniFileBase FromPath(const std::filesystem::path& path)
        {
            return { path };
        }

    private:
        char m_fieldSep = '=';
        char m_esc = '\\';
        std::vector<std::string> m_commentPrefixes = { "#" , ";" };
        bool m_multiLineValues = false;
        bool m_overwriteDuplicateFields = true;

        void EraseSingleComment(const std::string &commentPrefix,
            std::string &str,
            std::string::size_type startpos = 0)
        {
            size_t prefixpos = str.find(commentPrefix, startpos);
            if(std::string::npos == prefixpos)
                return;
            // Found a comment prefix, is it escaped?
            if(0 != prefixpos && str[prefixpos - 1] == m_esc)
            {
                // The comment prefix is escaped, so just delete the escape char
                // and keep erasing after the comment prefix
                str.erase(prefixpos - 1, 1);
                EraseSingleComment(
                    commentPrefix, str, prefixpos - 1 + commentPrefix.size());
            }
            else
            {
                str.erase(prefixpos);
            }
        }

        void EraseAllComments(std::string &str)
        {
            for(const std::string &commentPrefix : m_commentPrefixes) 
            {
                EraseSingleComment(commentPrefix, str);
            }
        }

        /** Tries to find a suitable comment prefix for the string data at the given
          * position. Returns m_commentPrefixes.end() if not match was found. */
        std::vector<std::string>::const_iterator FindCommentPrefix(const std::string &str,
            const std::size_t startpos) const
        {
            // if startpos is invalid simply return "not found"
            if(startpos >= str.size())
                return m_commentPrefixes.end();

            for(size_t i = 0; i < m_commentPrefixes.size(); ++i)
            {
                const std::string &prefix = m_commentPrefixes[i];
                // if this comment prefix is longer than the string view itself
                // then skip
                if(prefix.size() + startpos > str.size())
                    continue;

                bool match = true;
                for(size_t j = 0; j < prefix.size() && match; ++j)
                    match = str[startpos + j] == prefix[j];

                if(match)
                    return m_commentPrefixes.begin() + i;
            }

            return m_commentPrefixes.end();
        }

        void WriteEscaped(std::ostream &os, const std::string &str) const
        {
            for(size_t i = 0; i < str.length(); ++i)
            {
                auto prefixpos = FindCommentPrefix(str, i);
                // if no suitable prefix was found at this position
                // then simply write the current character
                if(prefixpos != m_commentPrefixes.end())
                {
                    const std::string &prefix = *prefixpos;
                    os.put(m_esc);
                    os.write(prefix.c_str(), prefix.size());
                    i += prefix.size() - 1;
                }
                else if (m_multiLineValues && str[i] == '\n')
                    os.write("\n\t", 2);
                else
                    os.put(str[i]);
            }
        }

    public:
        IniFileBase() = default;

        IniFileBase(const char fieldSep, const char comment)
            : m_fieldSep(fieldSep), m_commentPrefixes(1, std::string(1, comment))
        {}

        IniFileBase(const std::string &filename)
        {
            Load(filename);
        }
        
        IniFileBase(const std::filesystem::path& path) 
        {
            Load(path);
        }

        IniFileBase(std::istream &is)
        {
            Decode(is);
        }

        IniFileBase(const char fieldSep,
            const std::vector<std::string> &commentPrefixes)
            : m_fieldSep(fieldSep), m_commentPrefixes(commentPrefixes)
        {}

        IniFileBase(const std::string &filename,
            const char fieldSep,
            const std::vector<std::string> &commentPrefixes)
            : m_fieldSep(fieldSep), m_commentPrefixes(commentPrefixes)
        {
            Load(filename);
        }

        IniFileBase(std::istream &is,
            const char fieldSep,
            const std::vector<std::string> &commentPrefixes)
            : m_fieldSep(fieldSep), m_commentPrefixes(commentPrefixes)
        {
            Decode(is);
        }

        ~IniFileBase()
        {}

        /** Sets the separator charactor for fields in the INI file.
          * @param sep separator character to be used. */
        void setFieldSep(const char sep)
        {
            m_fieldSep = sep;
        }

        /** Sets the character that should be interpreted As the start of comments.
          * Default is '#'.
          * Note: If the inifile contains the comment character As data it must be prefixed with
          * the configured escape character.
          * @param comment comment character to be used. */
        void SetCommentChar(const char comment)
        {
            m_commentPrefixes = {std::string(1, comment)};
        }

        /** Sets the list of strings that should be interpreted As the start of comments.
          * Default is [ "#" ].
          * Note: If the inifile contains any comment string As data it must be prefixed with
          * the configured escape character.
          * @param commentPrefixes vector of comment prefix strings to be used. */
        void SetCommentPrefixes(const std::vector<std::string> &commentPrefixes)
        {
            m_commentPrefixes = commentPrefixes;
        }

        /** Sets the character that should be used to escape comment prefixes.
          * Default is '\'.
          * @param esc escape character to be used. */
        void SetEscapeChar(const char esc)
        {
            m_esc = esc;
        }

        /** Sets whether or not to parse multi-line field values.
          * Default is false.
          * @param enable enable or disable? */
        void SetMultiLineValues(bool enable)
        {
            m_multiLineValues = enable;
        }

        /** Sets whether or not overwriting duplicate fields is allowed.
          * If overwriting duplicate fields is not allowed,
          * an exception is thrown when a duplicate field is found inside a section.
          * Default is true.
          * @param allowed Is overwriting duplicate fields allowed or not? */
        void AllowOverwriteDuplicateFields(bool allowed)
        {
            m_overwriteDuplicateFields = allowed;
        }

        /** Tries to decode a ini file from the given input stream.
          * @param is input stream from which data should be read. */
        void Decode(std::istream &is)
        {
            this->clear();
            int lineNo = 0;
            IniSectionBase<Comparator> *currentSection = nullptr;
            std::string mutliLineValueFieldName = "";
            std::string line;
            // iterate file line by line
            while(!is.eof() && !is.fail())
            {
                std::getline(is, line, '\n');
                EraseAllComments(line);
                bool hasIndent = line.find_first_not_of(Indents()) != 0;
                Trim(line);
                ++lineNo;

                // skip if line is empty
                if(line.size() == 0)
                    continue;

                if(line[0] == '[')
                {
                    // line is a section
                    // check if the section is also closed on same line
                    std::size_t pos = line.find("]");
                    if(pos == std::string::npos)
                    {
                        std::stringstream ss;
                        ss << "l." << lineNo
                           << ": ini parsing failed, section not closed";
                        throw std::logic_error(ss.str());
                    }
                    // check if the section name is empty
                    if(pos == 1)
                    {
                        std::stringstream ss;
                        ss << "l." << lineNo
                           << ": ini parsing failed, section is empty";
                        throw std::logic_error(ss.str());
                    }

                    // retrieve section name
                    std::string secName = line.substr(1, pos - 1);
                    currentSection = &((*this)[secName]);

                    // clear multiline value field name
                    // a new section means there is no value to continue
                    mutliLineValueFieldName = "";
                }
                else
                {
                    // line is a field definition
                    // check if section was already opened
                    if(currentSection == nullptr)
                    {
                        std::stringstream ss;
                        ss << "l." << lineNo
                           << ": ini parsing failed, field has no section"
                                " or ini file in use by another application";
                        throw std::logic_error(ss.str());
                    }

                    // find key value separator
                    std::size_t pos = line.find(m_fieldSep);
                    if (m_multiLineValues && hasIndent && mutliLineValueFieldName != "")
                    {
                        // extend a multi-line value
                        IniField previous_value = (*currentSection)[mutliLineValueFieldName];
                        std::string value = previous_value.As<std::string>() + "\n" + line;
                        (*currentSection)[mutliLineValueFieldName] = value;
                    }
                    else if(pos == std::string::npos)
                    {
                        std::stringstream ss;
                        ss << "l." << lineNo
                           << ": ini parsing failed, no '"
                           << m_fieldSep
                           << "' found";
                        if (m_multiLineValues)
                            ss << ", and not a multi-line value continuation";
                        throw std::logic_error(ss.str());
                    }
                    else
                    {
                        // retrieve field name and value
                        std::string name = line.substr(0, pos);
                        Trim(name);
                        if (!m_overwriteDuplicateFields && currentSection->count(name) != 0)
                        {
                            std::stringstream ss;
                            ss << "l." << lineNo
                               << ": ini parsing failed, duplicate field found";
                            throw std::logic_error(ss.str());
                        }
                        std::string value = line.substr(pos + 1, std::string::npos);
                        Trim(value);
                        (*currentSection)[name] = value;
                        // store last field name for potential multi-line values
                        mutliLineValueFieldName = name;
                    }
                }
            }
        }

        /** Tries to decode a ini file from the given input string.
          * @param content string to be decoded. */
        void Decode(const std::string& content)
        {
            std::istringstream ss(content);
            Decode(ss);
        }
        
        
        void Load(const std::string &fileName)
        {
            std::ifstream is(fileName.c_str());
            Decode(is);
        }
        
        void Load(const std::filesystem::path& filePath) 
        {
            Load(filePath.string());     
        }

        /** Encodes this inifile object and writes the output to the given stream.
          * @param os target stream. */
        void Encode(std::ostream &os) const
        {
            // iterate through all sections in this file
            for(const auto &filePair : *this)
            {
                os.put('[');
                writeEscaped(os, filePair.first);
                os.put(']');
                os.put('\n');

                // iterate through all fields in the section
                for(const auto &secPair : filePair.second)
                {
                    writeEscaped(os, secPair.first);
                    os.put(m_fieldSep);
                    writeEscaped(os, secPair.second.template As<std::string>());
                    os.put('\n');
                }

                // Add a newline after each section
                os.put('\n');
            }
        }

        /** Encodes this inifile object As string and returns the result.
          * @return encoded infile string. */
        std::string Encode() const
        {
            std::ostringstream ss;
            Encode(ss);
            return ss.str();
        }

        /** Saves this inifile object to the file at the given path.
          * @param fileName path to the file where the data should be stored. */
        void SaveToFile(const std::string& fileName) const
        {
            std::ofstream os(fileName.c_str());
            Encode(os);
        }

        KIWI_NODISCARD bool IsEmpty() const
        {
            return Super::empty();
        }

        KIWI_NODISCARD const IniSectionBase<Comparator>& operator[](const String& name) const
        {
            return Super::operator[](name.ToStdString());
        }

        KIWI_NODISCARD IniSectionBase<Comparator>& operator[](const String& name)
        {
            return Super::operator[](name.ToStdString());
        }
    };

    using INI = IniFileBase<std::less<std::string>>;
    using IniSection = IniSectionBase<std::less<>>;
    using IniCaseInsensitive = IniFileBase<StringInsensitiveLess>;
    using IniSectionCaseInsensitive = IniSectionBase<StringInsensitiveLess>;
}
