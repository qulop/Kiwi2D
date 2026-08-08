#pragma once

#include <lua.h>


namespace Kiwi::Scripting {
    class LuaState {
    public:
        KIWI_NODISCARD static LuaState New();


    private:
        std::shared_ptr<lua_State> m_luaState;
    };
}