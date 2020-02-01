#pragma once

#include "InputState.h"

class KeyBinding
{
protected:
    Keys key;
public:
    Keys getKey() const;

    bool isDown(InputState* inputState) const;
    bool isUp(InputState* inputState) const;
    bool isTapped(InputState* inputState) const;
    bool isReleased(InputState* inputState) const;

    KeyBinding(INIReader* reader, const std::string& section, const std::string& name, Keys fallback = NONE);
};
