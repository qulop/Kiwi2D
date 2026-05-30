#pragma once

#include <common/Definitions.hpp>
#include <common/cast/Cast.hpp>

#include <memory/Memory.hpp>

#include <cstring>
#include <cwchar>


namespace Kiwi {
    template<typename T>
    struct BasicCString;


	template<>
	struct BasicCString<char> final {
	public:
		using CharType = char;
		
	public:
		BasicCString() = delete;

	public:
        static CharType* StrCpy(CharType* dst, size_t dstsz, const CharType* src) {
            return (strcpy_s(dst, dstsz, src) == 0) ? dst : nullptr;
		}

        static CharType* StrCpyN(CharType* dst, size_t dstsz, const CharType* src, size_t count) {
            return (strncpy_s(dst, dstsz, src, count) == 0) ? dst : nullptr;
        }

        static CharType** StrArrCpy(const CharType** src, size_t len, size_t offset = 0) {
            if (!src || offset > len) {
                return nullptr;
            }

            size_t numToCopy = len - offset;
            auto** dstArr = KIWI_NOTHROW_NEW CharType*[numToCopy + 1];
            if (!dstArr) {
                return nullptr;
            }

            for (size_t i = 0; i < numToCopy; i++) {
                size_t srcIndex = i + offset;
                size_t srcLen = StrLen(src[srcIndex]);

                dstArr[i] = KIWI_NOTHROW_NEW CharType[srcLen + 1];
                if (!dstArr[i]) {
                    for (size_t j = 0; j < i; j++) {
                        delete[] dstArr[j];
                    }

                    delete[] dstArr;
                    return nullptr;
                }

                StrCpy(dstArr[i], srcLen + 1, src[srcIndex]);
            }

            dstArr[numToCopy] = nullptr;
            return dstArr;
        }

        static CharType* StrCat(CharType* dst, size_t dstsz, const CharType* src) {
            return (strcat_s(dst, dstsz, src) == 0) ? dst : nullptr;
        }

        static CharType* StrCatN(CharType* dst, size_t dstsz, const CharType* src, size_t count) {
            return (strncat_s(dst, dstsz, src, count) == 0) ? dst : nullptr;
        }

        template<typename TReturnType = size_t>
        static TReturnType StrLen(const CharType* str) {
            return static_cast<TReturnType>(std::strlen(str));
        }

        static i32 StrCmp(const CharType* lhs, const CharType* rhs) {
            return std::strcmp(lhs, rhs);
        }

        static bool StrCmpBool(const CharType* lhs, const CharType* rhs) {
            return (StrCmp(lhs, rhs) == 0);
        }

        KIWI_NODISCARD static CharType* New(size_t sz) {
            return static_cast<CharType*>(Memory::Malloc(sz * sizeof(CharType)));
        }

        static void Delete(CharType* ptr) {
            Memory::Free(ptr);
        }
	};

    template<>
    struct BasicCString<wchar_t> final {
    public:
        using CharType = wchar_t;

    public:
        BasicCString() = delete;

    public:
        static CharType* StrCpy(CharType* dst, size_t dstsz, const CharType* src) {
            return (wcscpy_s(dst, dstsz, src) == 0) ? dst : nullptr;
        }

        static CharType* StrCpyN(CharType* dst, size_t dstsz, const CharType* src, size_t count) {
            return (wcsncpy_s(dst, dstsz, src, count) == 0) ? dst : nullptr;
        }

        static CharType** StrArrCpy(const CharType** src, size_t len, size_t offset = 0) {
            if (!src || offset > len) {
                return nullptr;
            }

            size_t numToCopy = len - offset;
            auto** dstArr = KIWI_NOTHROW_NEW CharType*[numToCopy + 1];
            if (!dstArr) {
                return nullptr;
            }

            for (size_t i = 0; i < numToCopy; i++) {
                size_t srcIndex = i + offset;
                size_t srcLen = StrLen(src[srcIndex]);

                dstArr[i] = KIWI_NOTHROW_NEW CharType[srcLen + 1];
                if (!dstArr[i]) {
                    for (size_t j = 0; j < i; j++) {
                        delete[] dstArr[j];
                    }

                    delete[] dstArr;
                    return nullptr;
                }

                StrCpy(dstArr[i], srcLen + 1, src[srcIndex]);
            }

            dstArr[numToCopy] = nullptr;
            return dstArr;
        }

        static CharType* StrCat(CharType* dst, size_t dstsz, const CharType* src) {
            return (wcscat_s(dst, dstsz, src) == 0) ? dst : nullptr;
        }

        static CharType* StrCatN(CharType* dst, size_t dstsz, const CharType* src, size_t count) {
            return (wcsncat_s(dst, dstsz, src, count) == 0) ? dst : nullptr;
        }

        template<typename TReturnType = size_t>
        static TReturnType StrLen(const CharType* str) {
            return static_cast<TReturnType>(std::wcslen(str));
        }

        static i32 StrCmp(const CharType* lhs, const CharType* rhs) {
            return std::wcscmp(lhs, rhs);
        }

        static bool StrCmpBool(const CharType* lhs, const CharType* rhs) {
            return (StrCmp(lhs, rhs) == 0);
        }

        KIWI_NODISCARD static CharType* New(size_t sz) {
            return static_cast<CharType*>(Memory::Malloc(sz * sizeof(CharType)));
        }

        static void Delete(CharType* ptr) {
            Memory::Free(ptr);
        }
    };


	class CMemoryTools {
	public:
		using RawMemoryPtr = void*;

	public:
        static RawMemoryPtr MemSet(RawMemoryPtr dst, i32 ch, size_t count) {
			return std::memset(dst, ch, count);
		}

        static RawMemoryPtr MemCpy(RawMemoryPtr dst, const RawMemoryPtr src, size_t count) {
			return std::memcpy(dst, src, count);
		}

        static i32 MemCmp(const RawMemoryPtr lhs, const RawMemoryPtr rhs, size_t count) {
			return std::memcmp(lhs, rhs, count);
		}

        static bool MemCmpBool(const RawMemoryPtr lhs, const RawMemoryPtr rhs, size_t count) {
            return (MemCmp(lhs, rhs, count) == 0);
        }

        static void MemSetSafe(RawMemoryPtr mem, size_t count) {
            // TODO: Not as safe, as it could be
            volatile byte* ptr = static_cast<byte*>(mem);
            for (size_t i = 0; i < count; i++) {
                ptr[i] = 0;
            }
        }

	    template<typename T>
	    static T* Malloc(size_t sz) {
            return CastTo<T*>(Memory::Malloc(sz));
        }

	    static void Free(void* ptr) {
            Memory::Free(ptr);
        }
    };


    using CString = BasicCString<char>;
    using CWideString = BasicCString<wchar_t>;
}