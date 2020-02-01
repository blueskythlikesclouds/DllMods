#pragma once

enum Keys : uint32_t
{
	NONE = 0x0,

	A = 0x1,
	B = 0x2,
	X = 0x8,
	Y = 0x10,

	DPAD_UP = 0x40,
	DPAD_DOWN = 0x80,
	DPAD_LEFT = 0x100,
	DPAD_RIGHT = 0x200,

	START = 0x400,
	SELECT = 0x800,

	LEFT_BUMPER = 0x1000,
	RIGHT_BUMPER = 0x2000,

	LEFT_TRIGGER = 0x4000,
	RIGHT_TRIGGER = 0x8000,

	LEFT_STICK = 0x10000,
	RIGHT_STICK = 0x20000,
};

struct InputState
{
	Keys downState;
	Keys upState;
	Keys tappedState;
	Keys releasedState;

	uint32_t field10;

	float leftStickHorizontal;
	float leftStickVertical;
	uint16_t field1C;
	uint16_t field1E;

	float rightStickHorizontal;
	float rightStickVertical;
	uint16_t field28;
	uint16_t field2A;

	float field2C;
	float field30;
	uint16_t field34;
	uint16_t field36;

	uint32_t field38;
	uint32_t field3C;
	uint32_t field40;
	uint32_t field44;
	uint32_t field48;
	uint32_t field4C;
	uint32_t field50;

	bool isDown(const Keys keys) const
	{
		return (downState & keys) == keys;
	}

	bool isUp(const Keys keys) const
	{
		return (upState & keys) == keys;
	}

	bool isTapped(const Keys keys) const
	{
		return (tappedState & keys) == keys;
	}

	bool isReleased(const Keys keys) const
	{
		return (releasedState & keys) == keys;
	}
};

InputState* getInputState();
