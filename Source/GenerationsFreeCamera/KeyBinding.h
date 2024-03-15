#pragma once

class KeyBinding
{
protected:
    Sonic::EKeyState key;
public:
    Sonic::EKeyState getKey() const;

    bool isDown(const Sonic::SPadState& padState) const;
    bool isUp(const Sonic::SPadState& padState) const;
    bool isTapped(const Sonic::SPadState& padState) const;
    bool isReleased(const Sonic::SPadState& padState) const;

    KeyBinding(const INIReader& reader, const std::string& section, const std::string& name, Sonic::EKeyState fallback = Sonic::EKeyState::eKeyState_None);
};
