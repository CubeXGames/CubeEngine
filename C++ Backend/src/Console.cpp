#include "Console.h"

std::ofstream Console::stream;

bool Console::InitializeConsole() {

	tm newtime;
	long long longTime;
	char timebuf[26];
	memset(timebuf, '\0', sizeof(timebuf));
	_time64(&longTime);

	if (_localtime64_s(&newtime, &longTime)) return false;
	if (asctime_s(timebuf, sizeof(timebuf), &newtime)) return false;

	std::string fileName = ".\\logs\\";
	fileName += timebuf;
	fileName += ".txt";
	stream.open(fileName, std::ofstream::out | std::ofstream::app);
	return stream.good();
}

void Console::WriteLine(const char* text) {

	stream << text << '\n';
}

void Console::FlushConsole() {

	stream.flush();
}

void Console::FlushAndCloseConsole() {

	if(stream.good()) stream.flush();
	stream.close();
}