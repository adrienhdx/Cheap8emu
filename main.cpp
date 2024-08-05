#ifndef APPLI_MAIN_CPP
#define APPLI_MAIN_CPP

#include <chrono>
#include <iostream>
using namespace std;
#include <cstdlib>
#include <string>
#include "chip8.h"
#include "platform.h"

int main(int argc, char** argv)
{
	if (argc != 4)
	{
		std::cerr << "Usage: " << argv[0] << " <Scale> <Delay> <ROM>\n";
		std::exit(EXIT_FAILURE);
	}

	int videoScale = std::stoi(argv[1]);
	int cycleDelay = std::stoi(argv[2]);
	char const* romFilename = argv[3];

	Platform platform("CHIP-8 Emulator", 64 * videoScale, 32 * videoScale, 64, 32);

	Chip8 chip8;
	chip8.LoadROM(romFilename);
	chip8.ShowState();

	int videoPitch = sizeof(chip8.video[0]) * 64;

	auto lastCycleTime = std::chrono::high_resolution_clock::now();
	auto lastFrameTime = std::chrono::high_resolution_clock::now();
	bool quit = false;

	const int FPS = 60; // Desired frames per second
	const float frameDelay = 1000.0f / FPS; // Frame delay in milliseconds
	
	cout << "Frame rate : " << (float)(1000/frameDelay) << " f/s" << endl;
	cout << "Cycle rate : " << (float)(1000/cycleDelay) << " cy/s" << endl;


	while (!quit)
	{
		quit = platform.ProcessInput(chip8.key);

		auto currentTime = std::chrono::high_resolution_clock::now();
		float dt = std::chrono::duration<float, std::chrono::milliseconds::period>(currentTime - lastCycleTime).count();

		if (dt > cycleDelay)
		{
			lastCycleTime = currentTime;

			chip8.Cycle();
		}

		float frameTime = std::chrono::duration<float, std::chrono::milliseconds::period>(currentTime - lastFrameTime).count();
		if (frameTime > frameDelay)
		{
			lastFrameTime = currentTime;

			platform.Update(chip8.video, videoPitch);
		}
	}

	return 0;
}

#endif // MAIN_CPP