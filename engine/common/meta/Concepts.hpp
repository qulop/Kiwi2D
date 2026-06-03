#pragma once

#include "TypeTraits.hpp"


namespace Kiwi::Concepts {
    template<typename T, typename Ty>
    concept SameAs = std::same_as<T, Ty>;


    template<typename T>
    concept Range = requires(T& rng) {
        std::ranges::begin(rng);
        std::ranges::end(rng);
    };

    template<typename T>
    concept Integral = (std::is_integral_v<T>);

    template<typename T>
    concept FloatingPoint = (std::is_floating_point_v<T>);

    template<typename T>
    concept Number = (std::is_integral_v<T> || std::is_floating_point_v<T>);


    template<typename T>
    concept Enumeration = std::is_enum_v<T>;
    
    template<typename T>
    concept CString = \
        std::same_as<std::remove_cv_t<T>, char*> ||
        std::same_as<std::remove_cv_t<T>, wchar_t*>;

    template<typename T>
    concept TrivialCharacter = \
        SameAs<std::remove_cv_t<T>, char> ||
        SameAs<std::remove_cv_t<T>, wchar_t>;

    template<typename Fn, typename... Args>
    concept Callable = Traits::IsInvocable<Fn, Args...>::value;


    template<typename From, typename To>
    concept ConvertibleTo = std::convertible_to<From, To>;

    template<typename Der, typename Base>
    concept DerivedFrom = std::derived_from<Der, Base>;

    template<typename Der, typename Base>
    concept IsBaseOf = Traits::isBaseOf_v<Der, Base>;

    template<typename T>
    concept ImplementsEqualityOp = requires(T a, T b) {
        { a == b } -> ConvertibleTo<bool>;
    };

    template<typename T>
    concept ImplementsInequalityOp = requires(T a, T b) {
        { a != b } -> ConvertibleTo<bool>;
    };

    template<typename T>
    concept ContainsData = requires(T c) {
        T::value_type;

        { c.data() } -> ConvertibleTo<const typename T::value_type*>;
    };

    template<typename T>
    concept ContainerSTL = ContainsData<T> && requires(T c) {
        { c.size() } -> ConvertibleTo<size_t>;
        { c.empty() } -> ConvertibleTo<bool>;
    };
}