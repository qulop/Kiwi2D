#pragma once

#include <common/meta/TypeTraits.hpp>
#include <common/Definitions.hpp>
#include <common/Errors.hpp>
#include <common/filesystem/FileContent.hpp>
#include <common/filesystem/FileOpenMode.hpp>
#include <common/types/Result.hpp>

#include <fstream>
#include <filesystem>



namespace Kiwi {
    class File {
    public:
        KIWI_NODISCARD static Result<void> SaveInFile(std::filesystem::path filePath, StringView data, bool overwrite = false);
        KIWI_NODISCARD static Result<void> SaveInFile(std::filesystem::path filePath, const FileContent& data, bool isBinary, bool overwrite = false);

        KIWI_NODISCARD static Result<FileContent> LoadFromFile(std::filesystem::path filePath, EFileOpenMode::Type mode);

        KIWI_NODISCARD static Result<File> OpenFileStatic(std::filesystem::path filePath, EFileOpenMode::Type mode);

    public:
        File() = default;

        // std::fstream is move-only, so File mirrors that ownership model.
        File(File&&) noexcept = default;
        File& operator=(File&&) noexcept = default;

        File(const File&) = delete;
        File& operator=(const File&) = delete;

        KIWI_NODISCARD Result<void> Open(std::filesystem::path path, EFileOpenMode::Type mode);

        KIWI_NODISCARD std::fstream& GetStream();
        KIWI_NODISCARD const std::fstream& GetStream() const;

        KIWI_NODISCARD const std::filesystem::path& GetPath() const;

        KIWI_NODISCARD bool IsOpen() const;
        KIWI_NODISCARD bool IsEOF() const;
        KIWI_NODISCARD bool IsOnBegin() const;

        KIWI_NODISCARD size_t Tell() const;

        KIWI_NODISCARD size_t SeekCur(size_t offset) const;
        KIWI_NODISCARD size_t SeekBegin(size_t offset) const;
        KIWI_NODISCARD size_t SeekEnd(size_t offset) const;

        KIWI_NODISCARD Result<void> Write(const FileContent& data) const;
        KIWI_NODISCARD Result<void> Write(StringView data) const;

        KIWI_NODISCARD Result<FileContent> ReadAll(bool rewindOnEnd = false) const;
        KIWI_NODISCARD Result<std::shared_ptr<byte>> ReadAsBytes(bool rewindOnEnd = false) const;

        void Rewind() const;

        KIWI_NODISCARD size_t GetFileSize() const;
        KIWI_NODISCARD EFileOpenMode::Type GetOpenMode() const;

        ~File() = default;

    private:
        mutable std::fstream m_stream;
        EFileOpenMode::Type m_mode = EFileOpenMode::READ;
        size_t m_fileSize = 0;

        std::filesystem::path m_path;
    };
}
