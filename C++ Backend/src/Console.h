#pragma once
#include <iostream>
#include <fstream>
#include <iomanip>
#include <ctime>
#include <cstdarg>

class Console {

public:

	static bool InitializeConsole();
	static void WriteLine(const char* text);
	static void FlushConsole();
	static void FlushAndCloseConsole();
	static std::ofstream stream;
};