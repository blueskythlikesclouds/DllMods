#include "KeyBinding.h"
#include <unordered_map>

std::unordered_map<std::string, Sonic::EKeyState> keyNameMap =
{
    { "NONE", Sonic::eKeyState_None },
    { "A", Sonic::eKeyState_A },
    { "B", Sonic::eKeyState_B },
    { "X", Sonic::eKeyState_X },
    { "Y", Sonic::eKeyState_Y },
    { "DPAD_UP", Sonic::eKeyState_DpadUp },
    { "DPAD_DOWN", Sonic::eKeyState_DpadDown },
    { "DPAD_LEFT", Sonic::eKeyState_DpadLeft },
    { "DPAD_RIGHT", Sonic::eKeyState_DpadRight },
    { "START", Sonic::eKeyState_Start },
    { "SELECT", Sonic::eKeyState_Select },
    { "LEFT_BUMPER", Sonic::eKeyState_LeftBumper },
    { "RIGHT_BUMPER", Sonic::eKeyState_RightBumper },
    { "LEFT_TRIGGER", Sonic::eKeyState_LeftTrigger },
    { "RIGHT_TRIGGER", Sonic::eKeyState_RightTrigger },
    { "LEFT_STICK", Sonic::eKeyState_LeftStick },
    { "RIGHT_STICK", Sonic::eKeyState_RightStick }
};

Sonic::EKeyState KeyBinding::getKey() const
{
    return key;
}

bool KeyBinding::isDown(const Sonic::SPadState& padState) const
{
    return key != 0 ? (padState.DownState & key) == key : false;
}

bool KeyBinding::isUp(const Sonic::SPadState& padState) const
{
    return key != 0 ? (padState.UpState & key) == key : true;
}

bool KeyBinding::isTapped(const Sonic::SPadState& padState) const
{
    return key != 0 ? (padState.TappedState & key) == key : false;
}

bool KeyBinding::isReleased(const Sonic::SPadState& padState) const
{
    return key != 0 ? (padState.ReleasedState & key) == key : false;
}

KeyBinding::KeyBinding(const INIReader& reader, const std::string& section, const std::string& name, const Sonic::EKeyState fallback)
{
    std::string keyName = reader.Get(section, name, "");

    if (!keyName.empty())
    {
        std::transform(keyName.begin(), keyName.end(), keyName.begin(), std::toupper);

        key = keyNameMap[keyName];
        if (key == Sonic::eKeyState_None)
            key = fallback;
    }
    else
    {
        key = fallback;
    }
}
