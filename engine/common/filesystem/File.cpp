#include "File.hpp"

#include <common/meta/TypeTraits.hpp>
#include <common/cast/Cast.hpp>
#include <common/Debug.hpp>

#include <cerrno>


namespace Kiwi {
    Result<void> File::SaveInFile(std::filesystem::path filePath, StringView data, bool overwrite) {
        return SaveInFile(
            std::move(filePath),
            FileContent{ EFileContentDataFormat::PLAIN_TEXT, data },
            false,
            overwrite
        );
    }

    Result<void> File::SaveInFile(std::filesystem::path filePath, const FileContent& data, bool isBinary, bool overwrite) {
        EFileOpenMode::Type mode = EFileOpenMode::WRITE;
        if (isBinary) {
            mode |= EFileOpenMode::BINARY;
        }
        if (overwrite) {
            mode |= EFileOpenMode::TRUNCATE;
        }
        else {
            mode |= EFileOpenMode::APPEND;
        }

        if (const std::filesystem::path parentPath = filePath.parent_path(); !parentPath.empty()) {
            std::error_code ec;

            std::filesystem::create_directories(parentPath, ec);
            if (ec) {
                return Error::Create(EErrorIO::UNKNOWN, ec.message());
            }
        }

        if (Result<File> res = OpenFileStatic(std::move(filePath), mode); !res) {
            return res.GetError();
        }
        else {
            return res.GetValue().Write(data);
        }
    }

    Result<FileContent> File::LoadFromFile(std::filesystem::path filePath, EFileOpenMode::Type mode) {
        if (Result<File> res = OpenFileStatic(std::move(filePath), mode); !res) {
            return res.GetError();
        }
        else {
            return res.GetValue().ReadAll();
        }
    }

    Result<File> File::OpenFileStatic(std::filesystem::path filePath, EFileOpenMode::Type mode) {
        File file;
        if (Result<void> res = file.Open(std::move(filePath), mode); !res) {
            return res.GetError();
        }

        return Success(std::move(file));
    }

    Result<void> File::Open(std::filesystem::path path, EFileOpenMode::Type mode) {
        const std::ios_base::openmode stdMode = EFileOpenMode::ToStdOpenMode(mode);

        errno = 0;
        m_stream.open(path, stdMode);
        if (!m_stream.is_open()) {
            if (const errno_t err = errno; err != 0) {
                return Error::Create(EErrorIO::FromPosixCode(err), std::strerror(err));
            }

            std::error_code ec;
            if (!std::filesystem::exists(path, ec)) {
                return Error::Create(EErrorIO::DOES_NOT_EXIST, ec.message());
            }

            return Error::Create(EErrorIO::UNKNOWN);
        }

        m_mode = mode;

        std::error_code ec;
        const auto onDiskSize = std::filesystem::file_size(path, ec);
        m_fileSize = ec ? 0 : static_cast<size_t>(onDiskSize);

        m_path = std::move(path);

        if (!(mode & EFileOpenMode::APPEND)) {
            Rewind();
        }

        return Success<void>();
    }

    std::fstream& File::GetStream() {
        return m_stream;
    }

    const std::fstream& File::GetStream() const {
        return m_stream;
    }

    const std::filesystem::path& File::GetPath() const {
        return m_path;
    }

    bool File::IsOpen() const {
        return m_stream.is_open();
    }

    bool File::IsEOF() const {
        return m_stream.eof();
    }

    bool File::IsOnBegin() const {
        return Tell() == 0;
    }

    size_t File::Tell() const {
        std::streampos pos(-1);
        if (m_mode & EFileOpenMode::READ) {
            pos = m_stream.tellg();
        }
        else if (m_mode & EFileOpenMode::WRITE) {
            pos = m_stream.tellp();
        }

        const std::streamoff off = static_cast<std::streamoff>(pos);
        return off < 0 ? 0 : static_cast<size_t>(off);
    }

    size_t File::SeekCur(size_t offset) const {
        return SeekBegin(Tell() + offset);
    }

    size_t File::SeekBegin(size_t offset) const {
        m_stream.clear();

        const auto target = static_cast<std::streamoff>(offset);
        if (m_mode & EFileOpenMode::READ) {
            m_stream.seekg(target, std::ios::beg);
        }
        if (m_mode & EFileOpenMode::WRITE) {
            m_stream.seekp(target, std::ios::beg);
        }

        return m_stream.good() ? 0 : -1;
    }

    size_t File::SeekEnd(size_t offset) const {
        m_stream.clear();

        const auto target = static_cast<std::streamoff>(offset);
        if (m_mode & EFileOpenMode::READ) {
            m_stream.seekg(target, std::ios::end);
        }
        if (m_mode & EFileOpenMode::WRITE) {
            m_stream.seekp(target, std::ios::end);
        }

        return m_stream.good() ? 0 : -1;
    }

    Result<void> File::Write(const FileContent& data) const {
        KIWI_ASSERT_BASIC(m_stream.is_open());

        if (data.GetContentFormat() == EFileContentDataFormat::PLAIN_TEXT) {
            String str = data.GetAsString();

            m_stream.write(str.ToCString(), str.Size());
            m_stream.flush();
        }
        else {
            const std::vector<u8> bytesStream = data.GetAsBytesStream();
            const size_t bytesToWrite = bytesStream.size();
            if (bytesToWrite == 0) {
                return {};
            }

            m_stream.write(
                reinterpret_cast<const char*>(bytesStream.data()),
                static_cast<std::streamsize>(bytesToWrite)
            );
            m_stream.flush();
        }


        if (!m_stream.good()) {
            return Error::Create(EErrorIO::UNKNOWN);
        }

        return {};
    }

    Result<void> File::Write(StringView data) const {
        return Write(FileContent(EFileContentDataFormat::PLAIN_TEXT, data));
    }

    Result<FileContent> File::ReadAll(bool rewindOnEnd) const {
        KIWI_ASSERT_BASIC(m_stream.is_open());

        const auto contentType = (m_mode & EFileOpenMode::BINARY) ?
            EFileContentDataFormat::BINARY : EFileContentDataFormat::PLAIN_TEXT;

        KIWI_IGNORE_RETURN(SeekBegin(0));
        Result fileBytes = ReadAsBytes(rewindOnEnd);
        if (fileBytes) {
            FileContent content(contentType, fileBytes.GetValue().get(), m_fileSize);
            return Success(content);
        }

        return fileBytes.GetError();
    }

    Result<std::shared_ptr<byte>> File::ReadAsBytes(bool rewindOnEnd) const {
        KIWI_ASSERT_BASIC(m_stream.is_open());

        byte* buffer = KIWI_NOTHROW_NEW byte[m_fileSize + 1];
        if (!buffer) {
            return Error::Create(EGeneralError::ALLOC_FAILED, "Failed to allocate buffer");
        }

        m_stream.read(
            reinterpret_cast<char*>(buffer),
            static_cast<std::streamsize>(m_fileSize)
        );

        if (const auto wasRead = static_cast<size_t>(m_stream.gcount()); wasRead != m_fileSize) {
            delete[] buffer;

            if (m_stream.bad()) {
                return Error::Create(EErrorIO::IO_ERROR, "Bad bit set in stream");
            }
            if (m_stream.eof()) {
                return Error::Create(EErrorIO::UNEXPECTED_EOF, "Unexpected end of file");
            }

            return Error::Create(EErrorIO::UNKNOWN, "Unknown error");
        }

        buffer[m_fileSize] = static_cast<byte>('\0');
        if (rewindOnEnd) {
            Rewind();
        }

        return Success(std::shared_ptr<byte>(buffer, [](byte* ptr) {
            delete[] ptr;
        }));
    }

    void File::Rewind() const {
        m_stream.clear();

        m_stream.seekg(0, std::ios::beg);
        m_stream.seekp(0, std::ios::beg);
    }

    size_t File::GetFileSize() const {
        return m_fileSize;
    }

    EFileOpenMode::Type File::GetOpenMode() const {
        return m_mode;
    }
}
