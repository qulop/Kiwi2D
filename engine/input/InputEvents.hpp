#pragma once


namespace Kiwi {
    enum class EInputAction {
        Press,
        Release,
        Repeat
    };

    struct KeyEvent {
        int keyCode;
        EInputAction action;
    };

    struct CharEvent {
        unsigned int codepoint;
    };

    struct MouseButtonEvent {
        int buttonCode;
        EInputAction action;
    };
}
