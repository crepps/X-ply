#pragma once

#include <Windows.h>
#include <Xinput.h>
#include <vector>
#include <thread>
#include <mutex>
#include <exception>
#include <cmath>

#define NUM_BUTTONS 16

#define	UP		XINPUT_GAMEPAD_DPAD_UP
#define	DOWN		XINPUT_GAMEPAD_DPAD_DOWN
#define	LEFT		XINPUT_GAMEPAD_DPAD_LEFT
#define	RIGHT		XINPUT_GAMEPAD_DPAD_RIGHT
#define	START		XINPUT_GAMEPAD_START
#define	BACK		XINPUT_GAMEPAD_BACK
#define	L3		XINPUT_GAMEPAD_LEFT_THUMB
#define	R3		XINPUT_GAMEPAD_RIGHT_THUMB
#define	LB		XINPUT_GAMEPAD_LEFT_SHOULDER
#define	RB		XINPUT_GAMEPAD_RIGHT_SHOULDER
#define	A		XINPUT_GAMEPAD_A
#define	B		XINPUT_GAMEPAD_B
#define	X		XINPUT_GAMEPAD_X
#define	Y		XINPUT_GAMEPAD_Y

class Controller
{
private:
	enum MODE
	{
		IDLE,
		VLC_MODE,
		WMP_MODE,
		WIN_MODE
	} _mode;

	static int _buttonValues[NUM_BUTTONS];
	
	XINPUT_STATE _state;

	XINPUT_VIBRATION _vibration;

	std::mutex _mutex;

	DWORD _winError;

	std::vector<int> _buttonDown;

	static bool _processed[NUM_BUTTONS];

public:
	Controller();

	void Update();

	void Poll();

	bool ButtonPress(int);

	void Vibrate(int, int);

	int Map(int);

	void KeyEvent(DWORD, int);

	XINPUT_STATE* GetState() { return &_state; }
};
