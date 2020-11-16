#pragma once

#include <BlueBlur.h>

namespace Sonic
{
	enum EKeyState : uint32_t
	{
		KEY_STATE_NONE			= 0x0,

		KEY_STATE_A				= 0x1,
		KEY_STATE_B				= 0x2,
		KEY_STATE_X				= 0x8,
		KEY_STATE_Y				= 0x10,

		KEY_STATE_DPAD_UP		= 0x40,
		KEY_STATE_DPAD_DOWN		= 0x80,
		KEY_STATE_DPAD_LEFT		= 0x100,
		KEY_STATE_DPAD_RIGHT	= 0x200,

		KEY_STATE_START			= 0x400,
		KEY_STATE_SELECT		= 0x800,

		KEY_STATE_LEFT_BUMPER	= 0x1000,
		KEY_STATE_RIGHT_BUMPER	= 0x2000,

		KEY_STATE_LEFT_TRIGGER	= 0x4000,
		KEY_STATE_RIGHT_TRIGGER = 0x8000,

		KEY_STATE_LEFT_STICK	= 0x10000,
		KEY_STATE_RIGHT_STICK	= 0x20000
	};

	struct SPadState
	{
		EKeyState DownState;
		EKeyState UpState;
		EKeyState TappedState;
		EKeyState ReleasedState;

		INSERT_PADDING(0x4);

		float LeftStickHorizontal;
		float LeftStickVertical;

		INSERT_PADDING(0x4);

		float RightStickHorizontal;
		float RightStickVertical;

		INSERT_PADDING(0x2C);

		bool IsDown(const EKeyState keys) const
		{
			return (DownState & keys) == keys;
		}

		bool IsUp(const EKeyState keys) const
		{
			return (UpState & keys) == keys;
		}

		bool IsTapped(const EKeyState keys) const
		{
			return (TappedState & keys) == keys;
		}

		bool IsReleased(const EKeyState keys) const
		{
			return (ReleasedState & keys) == keys;
		}
	};

	ASSERT_SIZEOF(SPadState, 0x54);
}