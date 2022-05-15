#include <thread>

struct HwInputState
{
	uint32_t error;
	uint32_t keys;
	int16_t leftStickX;
	int16_t leftStickY;
	int16_t rightStickX;
	int16_t rightStickY;
	int16_t leftTrigger;
	int16_t rightTrigger;
};
ASSERT_SIZEOF(HwInputState, 20);

struct HwRumbleState
{
	uint16_t lowFrequency;
	uint16_t highFrequency;
};
ASSERT_SIZEOF(HwRumbleState, 4);

struct GameController
{
	int32_t id;
	SDL_GameController* handle;
	HwInputState state;
	CRITICAL_SECTION criticalSection;

	GameController() : id(-1), handle(nullptr), state(), criticalSection()
	{
		InitializeCriticalSection(&criticalSection);
	}
};

struct Lock
{
	GameController* controller;

	Lock() : controller(nullptr)
	{
	}

	Lock(const Lock& lock) = delete;
	Lock(Lock&& lock)
	{
		controller = lock.controller;
		lock.controller = nullptr;
	}

	Lock(GameController* controller) : controller(controller)
	{
		if (controller)
		    EnterCriticalSection(&controller->criticalSection);
	}

	~Lock()
	{
		if (controller)
		    LeaveCriticalSection(&controller->criticalSection);
	}
};

GameController controllers[4];

Lock getLockedControllerFromInstanceId(const int32_t id)
{
    for (auto& controller : controllers)
    {
		Lock lock(&controller);

		if (controller.id == id)
			return lock;
    }
	return Lock();
}

Lock getLockedController(const size_t index)
{
	if (index >= _countof(controllers))
		return Lock();

	Lock lock(&controllers[index]);
	if (lock.controller->id == -1)
		return Lock();

	return lock;
}

void handleEvent(const SDL_Event& event)
{
	switch (event.type)
	{

	case SDL_CONTROLLERAXISMOTION:
	{
		Lock lock = getLockedControllerFromInstanceId(event.caxis.which);

		if (!lock.controller)
			return;

		const int16_t value = event.caxis.value < -32767 ? -32767 : event.caxis.value;

		switch (event.caxis.axis)
		{
		case SDL_CONTROLLER_AXIS_LEFTX:
			lock.controller->state.leftStickX = value;
			break;

		case SDL_CONTROLLER_AXIS_LEFTY:
			lock.controller->state.leftStickY = value;
			break;

		case SDL_CONTROLLER_AXIS_RIGHTX:
			lock.controller->state.rightStickX = value;
			break;

		case SDL_CONTROLLER_AXIS_RIGHTY:
			lock.controller->state.rightStickY = value;
			break;

		case SDL_CONTROLLER_AXIS_TRIGGERLEFT:
			lock.controller->state.leftTrigger = value;

			if (lock.controller->state.leftTrigger > 0x1E * 0x7FFF / 0xFF)
				lock.controller->state.keys |= app::eKeyState_LeftTrigger;
			else
				lock.controller->state.keys &= ~app::eKeyState_LeftTrigger;

			break;

		case SDL_CONTROLLER_AXIS_TRIGGERRIGHT:
			lock.controller->state.rightTrigger = value;

			if (lock.controller->state.rightTrigger > 0x1E * 0x7FFF / 0xFF)
				lock.controller->state.keys |= app::eKeyState_RightTrigger;
			else
				lock.controller->state.keys &= ~app::eKeyState_RightTrigger;

			break;

		default:
			break;
		}

		break;
	}

	case SDL_CONTROLLERBUTTONDOWN:
	case SDL_CONTROLLERBUTTONUP:
	{
		const app::EKeyState controllerButtons[] =
		{
			app::eKeyState_A, // SDL_CONTROLLER_BUTTON_A
			app::eKeyState_B, // SDL_CONTROLLER_BUTTON_B
			app::eKeyState_X, // SDL_CONTROLLER_BUTTON_X
			app::eKeyState_Y, // SDL_CONTROLLER_BUTTON_Y
			app::eKeyState_Select, // SDL_CONTROLLER_BUTTON_BACK
			app::eKeyState_None, // SDL_CONTROLLER_BUTTON_GUIDE
			app::eKeyState_Start, // SDL_CONTROLLER_BUTTON_START
			app::eKeyState_LeftStick, // SDL_CONTROLLER_BUTTON_LEFTSTICK
			app::eKeyState_RightStick, // SDL_CONTROLLER_BUTTON_RIGHTSTICK
			app::eKeyState_LeftBumper, // SDL_CONTROLLER_BUTTON_LEFTSHOULDER
			app::eKeyState_RightBumper, // SDL_CONTROLLER_BUTTON_RIGHTSHOULDER
			app::eKeyState_DpadUp, // SDL_CONTROLLER_BUTTON_DPAD_UP
			app::eKeyState_DpadDown, // SDL_CONTROLLER_BUTTON_DPAD_DOWN
			app::eKeyState_DpadLeft, // SDL_CONTROLLER_BUTTON_DPAD_LEFT
			app::eKeyState_DpadRight // SDL_CONTROLLER_BUTTON_DPAD_RIGHT
		};

		if (event.cbutton.button >= _countof(controllerButtons))
			return;

		Lock lock = getLockedControllerFromInstanceId(event.cbutton.which);

		if (!lock.controller)
			return;

		if (event.cbutton.state == SDL_PRESSED)
			lock.controller->state.keys |= controllerButtons[event.cbutton.button];
		else
			lock.controller->state.keys &= ~controllerButtons[event.cbutton.button];

		break;
	}

	case SDL_CONTROLLERDEVICEADDED:
	{
		for (auto& controller : controllers)
		{
			Lock lock(&controller);

			if (controller.id != -1)
				continue;

			controller.handle = SDL_GameControllerOpen(event.cdevice.which);

			if (!controller.handle)
				continue;

			SDL_Joystick* joystick = SDL_GameControllerGetJoystick(controller.handle);

			if (!joystick)
				continue;

			controller.id = SDL_JoystickInstanceID(joystick);
			memset(&controller.state, 0, sizeof(HwInputState));
			break;
		}
		break;
	}

	case SDL_CONTROLLERDEVICEREMOVED:
	{
		Lock lock = getLockedControllerFromInstanceId(event.cdevice.which);

		if (!lock.controller)
			return;

		SDL_GameControllerClose(lock.controller->handle);

		lock.controller->id = -1;
		lock.controller->handle = nullptr;

		break;
	}

	default:
		break;
	}
}

HOOK(uint32_t, __fastcall, PollInput, 0x9C7CC0, void* This, void* Edx, void* A2, HwInputState* state)
{
	memset(state, 0, sizeof(HwInputState));
	state->error = 1u;

	Lock lock = getLockedController(*(size_t*)This);
	if (!lock.controller)
		return 0;

	*state = lock.controller->state;
	state->error = 0u;

	return 0;
}

HOOK(bool, __fastcall, ResetRumble, 0x9C7F80, void* This)
{
	Lock lock = getLockedController(*(size_t*)This);
	if (!lock.controller)
		return false;

	return SDL_GameControllerRumble(lock.controller->handle, 0, 0, 0) == 0;
}

HOOK(uint32_t, __fastcall, SetRumble, 0x9C7F50, void* This, void* Edx, HwRumbleState* state)
{
	Lock lock = getLockedController(*(size_t*)This);
	if (!lock.controller)
		return 1;

	return SDL_GameControllerRumble(lock.controller->handle, state->lowFrequency, state->highFrequency, 0) != 0;
}

int pollThreadImpl(void*)
{
	SDL_Event event;

	while (!*(uint32_t*)0x1E5E2E8 && SDL_WaitEvent(&event))
		handleEvent(event);

	SDL_Quit();
	return 0;
}

FUNCTION_PTR(LRESULT, WINAPI, wndProc, 0xE7B6C0, HWND hWnd, UINT Msg, WPARAM wParam, LPARAM lParam);

HOOK(void, __fastcall, ApplicationRun, 0xE7B300, void* This)
{
	SDL_SetWindowsMessageHook([](void*, void* hWnd, unsigned int message, Uint64 wParam, Sint64 lParam)
		{ wndProc((HWND)hWnd, message, (WPARAM)wParam, (LPARAM)lParam); }, nullptr);

	SDL_Thread* thread = SDL_CreateThread(pollThreadImpl, "GenerationsSDL2PollThread", nullptr);
	originalApplicationRun(This);
	SDL_WaitThread(thread, nullptr);
}

extern "C" __declspec(dllexport) void Init()
{
	SDL_InitSubSystem(SDL_INIT_GAMECONTROLLER);
	SDL_SetHint(SDL_HINT_JOYSTICK_HIDAPI_PS4_RUMBLE, "1");
	SDL_SetHint(SDL_HINT_JOYSTICK_HIDAPI_PS5_RUMBLE, "1");

	INSTALL_HOOK(PollInput);
	INSTALL_HOOK(ResetRumble);
	INSTALL_HOOK(SetRumble);
	INSTALL_HOOK(ApplicationRun);

	// Controller Hotplugging Support
	WRITE_MEMORY(0x9C85FA, uint8_t, 0x83);
	WRITE_MEMORY(0x9C85FB, uint8_t, 0xFA);
	WRITE_MEMORY(0x9C85FC, uint8_t, 0x4);
	WRITE_NOP(0x9C8603, 12);
	WRITE_MEMORY(0x9C86B2, uint8_t, 0x83);
	WRITE_MEMORY(0x9C86B3, uint8_t, 0xFA);
	WRITE_MEMORY(0x9C86B4, uint8_t, 0x4);
	WRITE_NOP(0x9C86BB, 12);
	WRITE_NOP(0x9C8163, 2);
	WRITE_NOP(0x9C8168, 8);
	WRITE_NOP(0x9C8173, 1);
	WRITE_MEMORY(0x9C8176, uint8_t, 0xE8);
	WRITE_MEMORY(0x9C8177, uint8_t, 0xC7);
	WRITE_MEMORY(0x9C8178, uint8_t, 0x40);
	WRITE_MEMORY(0x9C8179, uint8_t, 0xC);
	WRITE_MEMORY(0x9C817A, uint8_t, 0x4);
	WRITE_MEMORY(0x9C817B, uint8_t, 0x0);
	WRITE_MEMORY(0x9C817C, uint8_t, 0x0);
	WRITE_MEMORY(0x9C817D, uint8_t, 0x0);
	WRITE_NOP(0x9C8A70, 2);
	WRITE_NOP(0x9C8AC0, 2);
}