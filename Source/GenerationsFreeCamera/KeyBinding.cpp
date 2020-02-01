#include "KeyBinding.h"
#include <unordered_map>

std::unordered_map<std::string, Keys> keyNameMap =
{
    { "NONE", NONE },
    { "A", A },
    { "B", B },
    { "X", X },
    { "Y", Y },
    { "DPAD_UP", DPAD_UP },
    { "DPAD_DOWN", DPAD_DOWN },
    { "DPAD_LEFT", DPAD_LEFT },
    { "DPAD_RIGHT", DPAD_RIGHT },
    { "START", START },
    { "SELECT", SELECT },
    { "LEFT_BUMPER", LEFT_BUMPER },
    { "RIGHT_BUMPER", RIGHT_BUMPER },
    { "LEFT_TRIGGER", LEFT_TRIGGER },
    { "RIGHT_TRIGGER", RIGHT_TRIGGER },
    { "LEFT_STICK", LEFT_STICK },
    { "RIGHT_STICK", RIGHT_STICK }
};

Keys KeyBinding::getKey() const
{
    return key;
}

bool KeyBinding::isDown(InputState* inputState) const
{
    return key != 0 ? (inputState->downState & key) == key : false;
}

bool KeyBinding::isUp(InputState* inputState) const
{
    return key != 0 ? (inputState->upState & key) == key : true;
}

bool KeyBinding::isTapped(InputState* inputState) const
{
    return key != 0 ? (inputState->tappedState & key) == key : false;
}

bool KeyBinding::isReleased(InputState* inputState) const
{
    return key != 0 ? (inputState->releasedState & key) == key : false;
}

KeyBinding::KeyBinding(INIReader* reader, const std::string& section, const std::string& name, const Keys fallback)
{
    std::string keyName = reader->Get(section, name, "");

    if (!keyName.empty())
    {
        std::transform(keyName.begin(), keyName.end(), keyName.begin(), std::toupper);

        key = keyNameMap[keyName];
        if (key == NONE)
            key = fallback;
    }
    else
    {
        key = fallback;
    }
}
