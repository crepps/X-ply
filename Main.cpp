#include <conio.h>
#include <iostream>
#include "Controller.h"

int main(int argc, char** argv)
{
	Controller controller;

	XINPUT_STATE* state = controller.GetState();

	// Debug
	while (true)
	{
		std::cout << (int)state->Gamepad.wButtons;

		Sleep(100);

		system("cls");
	}

	_getch();
	
	return EXIT_SUCCESS;
}
