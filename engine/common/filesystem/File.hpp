#pragma once

#include <common/meta/TypeTraits.hpp>
#include <common/Definitions.hpp>
#include <common/Errors.hpp>
#include <common/filesystem/FileContent.hpp>
#include <common/filesystem/FileOpenMode.hpp>
#include <common/types/Result.hpp>



namespace Kiwi {
    // TODO: Refactor this class
    class File {
    public:
        KIWI_NODISCARD static Status<Error<EErrorIO>> SaveInFile(std::filesystem::path filePath, StringView data, bool overwrite = false);
        KIWI_NODISCARD static Status<Error<EErrorIO>> SaveInFile(std::filesystem::path filePath, const FileContent& data, bool isBinary, bool overwrite = false);

        // Note: you should use `EFileOpenMode::READ | EFileOpenMode::BINARY` to read a file(even a plain text file!)
        KIWI_NODISCARD static Result<FileContent, EErrorIO> LoadFromFile(std::filesystem::path filePath, EFileOpenMode::Type mode = EFileOpenMode::READ);

        KIWI_NODISCARD static Result<File, EErrorIO> OpenFileStatic(std::filesystem::path filePath, EFileOpenMode::Type mode);

    public:
        File() = default;

        KIWI_NODISCARD Status<Error<EErrorIO>> Open(std::filesystem::path path, EFileOpenMode::Type mode);

        KIWI_NODISCARD FILE* GetFileHandle();
        KIWI_NODISCARD const FILE* GetFileHandle() const;

        KIWI_NODISCARD std::fstream ToStdFStream() const;

        KIWI_NODISCARD bool IsOpened() const;
        KIWI_NODISCARD bool IsEOF() const;
        KIWI_NODISCARD bool OnBegin() const;

        KIWI_NODISCARD size_t Tell() const;

        KIWI_NODISCARD i32 SeekCur(i32 offset) const;
        KIWI_NODISCARD i32 SeekBegin(i32 offset) const;
        KIWI_NODISCARD i32 SeekEnd(i32 offset) const;

        KIWI_NODISCARD Status<Error<EErrorIO>> Write(const FileContent& data) const;
        KIWI_NODISCARD Status<Error<EErrorIO>> Write(StringView data) const;

        KIWI_NODISCARD Result<FileContent, EErrorIO> ReadAll(bool rewindOnEnd = false) const;
        KIWI_NODISCARD Result<SharedPtr<byte>, EErrorIO> ReadAsBytes(bool rewindOnEnd = false) const;

        void Rewind() const;

        KIWI_NODISCARD u32 GetFileSize() const;
        KIWI_NODISCARD EFileOpenMode::Type GetOpenMode() const;

        ~File();

    private:
        FILE* m_file = nullptr;
        EFileOpenMode::Type m_mode = EFileOpenMode::READ;
        size_t m_fileSize = 0;
        
        std::filesystem::path m_path;
    };
}