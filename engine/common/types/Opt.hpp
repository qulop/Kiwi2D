#pragma once

#include <common/meta/Concepts.hpp>


namespace Kiwi::Details::Types {
    struct ZeroOpt {};
}


namespace Kiwi {
    inline constexpr auto ZERO_OPT = Details::Types::ZeroOpt();


    template<typename T>
    class Opt {
    public:
        using ValueType = T;

    public:
        constexpr Opt() = default;

        constexpr Opt(Details::Types::ZeroOpt) :
            m_optional(std::nullopt)
        {}

        constexpr Opt(const Opt& other) :
            m_optional(other.m_optional)
        {}

        constexpr Opt(Opt&& other) :
            m_optional(std::move(other.m_optional))
        {}

        template<typename U>
        constexpr Opt(const Opt<U>& other) :
            m_optional(other.m_optional)
        {}

        template<typename U>
        constexpr Opt(Opt<U>&& other) :
            m_optional(std::move(other.m_optional))
        {}

        template<typename... Args>
        constexpr explicit Opt(std::in_place_t, Args&&... args) :
            m_optional(std::in_place, std::forward<Args>(args)...)
        {}

        template<typename U, typename... Args>
        constexpr explicit Opt(std::in_place_t, std::initializer_list<U> i, Args&&... args) :
            m_optional(std::in_place, std::move(i), std::forward<Args>(args)...)
        {}

        template<typename U = std::remove_cv_t<T>>
            requires (!std::is_same_v<std::remove_cvref_t<U>, Details::Types::ZeroOpt>
                   && !std::is_same_v<std::remove_cvref_t<U>, std::in_place_t>
                   && !std::is_same_v<std::remove_cvref_t<U>, Opt>
                   && std::is_constructible_v<T, U>)
        constexpr Opt(U&& value) :
            m_optional(std::forward<U>(value))
        {}

    public:
        KIWI_NODISCARD constexpr ValueType& GetValue() {
            return *m_optional;
        }

        KIWI_NODISCARD constexpr ValueType StealValue() {
            return std::move(m_optional);
        }

        KIWI_NODISCARD constexpr const ValueType& GetValue() const {
            return *m_optional;
        }

        KIWI_NODISCARD constexpr bool HasValue() const {
            return m_optional.has_value();
        }

        template<Concepts::ConvertibleTo<ValueType> TDefault = std::remove_cv_t<ValueType>>
        KIWI_NODISCARD constexpr ValueType ValueOr(TDefault&& defaultValue) const {
            return m_optional.value_or(std::forward<TDefault>(defaultValue));
        }

        KIWI_NODISCARD constexpr ValueType GetOrDefault() const
            requires std::default_initializable<ValueType>
        {
            return ValueOr(ValueType());
        }

        KIWI_NODISCARD constexpr decltype(auto) operator*() const {
            return m_optional.operator*();
        }

        KIWI_NODISCARD constexpr decltype(auto) operator*() {
            return m_optional.operator*();
        }

        constexpr decltype(auto) operator->() const {
            return m_optional.operator->();
        }

        constexpr decltype(auto) operator->() {
            return m_optional.operator->();
        }

        constexpr explicit operator bool() const noexcept {
            return HasValue();
        }

        constexpr ~Opt() = default;

    private:
        template<typename U>
        friend class Opt;

        std::optional<T> m_optional;
    };
}