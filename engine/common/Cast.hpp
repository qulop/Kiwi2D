#pragma once


namespace Kiwi {
    template<typename...>
    struct CastTraits;


    template<typename T, typename... Args>
    using Cast = CastTraits<T, Args...>;
}