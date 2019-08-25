#include "Controller.h"

int Controller::_buttonValues[NUM_BUTTONS] =
{
	UP, DOWN, LEFT, RIGHT,
	START, BACK,
	L3, R3, LB, RB,
	NULL, NULL,
	A, B, X, Y
};

bool Controller::_processed[NUM_BUTTONS] = { false };

Controller::Controller()
{
	_mode = IDLE;

	_winError = NULL;

	std::thread updateThread(&Controller::Update, this);

	updateThread.detach();
}

void Controller::Update()
{
	ZeroMemory(&_state, sizeof(XINPUT_STATE));

	std::thread pollThread(&Controller::Poll, this);

	while (true)
	{
		// Controller connected
		if (_winError = XInputGetState(0, &_state) == NO_ERROR)
		{
			Sleep(10);

			// Program state (mode) selection
			if (ButtonPress(BACK) && ButtonPress(Y) && !_processed[Map(Y)])
			{
				_processed[Map(Y)] = true;

				_mode = (_mode != VLC_MODE ? VLC_MODE : IDLE);

				Vibrate((_mode != VLC_MODE ? 12000 : 35000), (_mode != VLC_MODE ? 0 : 35000));

				Sleep(250);	// Sleep will halt the thread; implement timing logic

				Vibrate(0, 0);

				// More mode selections here after their cases have been defined
			}

			// Finite-state machine
			switch (_mode)
			{
			case IDLE:
				Sleep(90);

				break;

			case VLC_MODE:
				// Volume up
				if (ButtonPress(UP) && !_processed[Map(UP)])
				{
					KeyEvent(VK_LCONTROL, 1);

					KeyEvent(VK_UP, 2);

					KeyEvent(VK_LCONTROL, 0);

					_processed[Map(UP)] = true;
				}

				// Volume down
				else if (ButtonPress(DOWN) && !_processed[Map(DOWN)])
				{
					KeyEvent(VK_LCONTROL, 1); 
					
					KeyEvent(VK_DOWN, 2);

					KeyEvent(VK_LCONTROL, 0);

					_processed[Map(DOWN)] = true;
				}

				// Rewind (1 minute)
				else if (ButtonPress(LEFT) && !_processed[Map(LEFT)])
				{
					if (_state.Gamepad.bLeftTrigger >= 30)
					{
						KeyEvent(VK_LCONTROL, 1);

						KeyEvent(VK_LEFT, 2);

						KeyEvent(VK_LCONTROL, 0);
					}

					_processed[Map(LEFT)] = true;
				}

				// Fast-forward (1 minute)
				else if (ButtonPress(RIGHT) && !_processed[Map(RIGHT)])
				{
					if (_state.Gamepad.bRightTrigger >= 30)
					{
						KeyEvent(VK_LCONTROL, 1);

						KeyEvent(VK_RIGHT, 2);

						KeyEvent(VK_LCONTROL, 0);
					}

					_processed[Map(RIGHT)] = true;
				}

				// Previous
				else if (ButtonPress(LB) && !_processed[Map(LB)])
				{
					KeyEvent('P', 2);

					_processed[Map(LB)] = true;
				}

				// Next
				else if (ButtonPress(RB) && !_processed[Map(RB)])
				{
					KeyEvent('N', 2);

					_processed[Map(RB)] = true;
				}

				// Play / pause
				else if (ButtonPress(A) && !_processed[Map(A)])
				{
					KeyEvent(VK_SPACE, 2);

					_processed[Map(A)] = true;
				}

				// Toggle / cycle subtitles
				else if (ButtonPress(B) && !_processed[Map(B)])
				{
					KeyEvent('V', 2);

					_processed[Map(B)] = true;
				}

				// Toggle playlist view
				else if (ButtonPress(X) && !_processed[Map(X)])
				{
					KeyEvent(VK_ESCAPE, 2);

					Sleep(10);

					KeyEvent(VK_LCONTROL, 1);

					KeyEvent('L', 2);

					KeyEvent(VK_LCONTROL, 0);

					_processed[Map(X)] = true;
				}

				// Toggle full screen
				else if (ButtonPress(Y) && !_processed[Map(Y)])
				{
					KeyEvent('F', 2);

					_processed[Map(Y)] = true;
				}

				//Rewind (10 seconds)
				if (_state.Gamepad.bLeftTrigger >= 30 && !_processed[10] && !ButtonPress(LEFT))
				{
					KeyEvent(VK_MENU, 1);

					KeyEvent(VK_LEFT, 2);

					KeyEvent(VK_MENU, 0);

					_processed[10] = true;
				}

				//Fast forward (10 seconds)
				if (_state.Gamepad.bRightTrigger >= 30 && !_processed[11] && !ButtonPress(RIGHT))
				{
					KeyEvent(VK_MENU, 1);

					KeyEvent(VK_RIGHT, 2);

					KeyEvent(VK_MENU, 0);

					_processed[11] = true;
				}

				break;

			case WMP_MODE:
				break;

			case WIN_MODE:
				break;
			}

			// Memory lock 'button down' vector, remove buttons from it, and set 'processed' boolean back to false if button is not currently down
			_mutex.lock();

			for (unsigned int i = 0; i < _buttonDown.size(); ++i)
			{
				if (!(_state.Gamepad.wButtons & _buttonDown[i]))
				{
					_processed[Map(_buttonDown[i])] = false;
					
					_buttonDown.erase(_buttonDown.begin() + i);
				}
			}

			_mutex.unlock();

			// Set triggers' 'processed' booleans to false if not currently down (enough)
			if (_state.Gamepad.bLeftTrigger < 30 && _processed[10])
				_processed[10] = false;

			if (_state.Gamepad.bRightTrigger < 30 && _processed[11])
				_processed[11] = false;
		}

		// Controller disconnected
		else
			Sleep(500);

	}

	pollThread.join();
}

void Controller::Poll()
{
	while (true)
	{
		Sleep(10);

		// Memory lock 'button down' vector while polling controller state
		_mutex.lock();

		for (int i = 0; i < NUM_BUTTONS; ++i)
		{
			// If a button is down
			if (_state.Gamepad.wButtons & _buttonValues[i])
			{
				// If the button's value isn't already in the 'button down' vector, push it into the vector
				if (std::find(_buttonDown.begin(), _buttonDown.end(), _buttonValues[i]) == _buttonDown.end())
					_buttonDown.push_back(_buttonValues[i]);
			}
		}

		_mutex.unlock();
	}
}

bool Controller::ButtonPress(int value)
{
	// Memory lock the 'button down' vector, search it for button value, unlock and return search result
	_mutex.lock();

	if (std::find(_buttonDown.begin(), _buttonDown.end(), value) != _buttonDown.end())
	{
		_mutex.unlock();

		return true;
	}

	_mutex.unlock();

	return false;
}

void Controller::Vibrate(int l_value, int r_value)
{
	_vibration.wLeftMotorSpeed = l_value;

	_vibration.wRightMotorSpeed = r_value;

	XInputSetState(0, &_vibration);
}

int Controller::Map(int value)
{
	/*
		0		UP				0x0001		1
		1		DOWN			0x0002		2
		2		LEFT			0x0004		4
		3		RIGHT			0x0008		8
		4		START			0x0010		16
		5		BACK			0x0020		32
		6		L3				0x0040		64
		7		R3				0x0080		128
		8		LB				0x0100		256
		9		RB				0x0200		512
		10		UNDEFINED		0x0400		1024
		11		UNDEFINED		0x0800		2048
		12		A				0x1000		4096
		13		B				0x2000		8192
		14		X				0x4000		16384
		15		Y				0x8000		32768

		This function takes a button's XInput-defined value (columns 3 and 4) and returns its enumerated value (column 1).
	*/

	return (int)(log(value) / log(2));
}

void Controller::KeyEvent(DWORD key, int type)
{
	// Full keystroke
	if (type == 2)
	{
		keybd_event(static_cast<BYTE>(key), MapVirtualKey(key, MAPVK_VK_TO_VSC), 0, 0);

		keybd_event(static_cast<BYTE>(key), MapVirtualKey(key, MAPVK_VK_TO_VSC), KEYEVENTF_KEYUP, 0);
	}

	// Set key down or up
	else
		keybd_event(static_cast<BYTE>(key), MapVirtualKey(key, MAPVK_VK_TO_VSC), (type ? 0 : KEYEVENTF_KEYUP), 0);
}