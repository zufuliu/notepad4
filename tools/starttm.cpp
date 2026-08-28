// cl /std:c++20 /EHsc /O2 /GR- /GS- /W4 /MD /DNDEBUG /D_CRT_SECURE_NO_WARNINGS /D_SCL_SECURE_NO_WARNINGS starttm.cpp
// clang-cl /std:c++20 /EHsc /O2 /GR- /GS- /W4 -Wextra -Wshadow -Wimplicit-fallthrough -Wformat=2 -Wundef -Wcomma /MD /DNDEBUG /D_CRT_SECURE_NO_WARNINGS /D_SCL_SECURE_NO_WARNINGS starttm.cpp
// g++ -std=gnu++20 -O2 -municode -fno-rtti -Wall -Wextra -Wshadow -Wimplicit-fallthrough -Wformat=2 -Wundef -DNDEBUG -D_CRT_SECURE_NO_WARNINGS -D_SCL_SECURE_NO_WARNINGS starttm.cpp -lshlwapi
#include <windows.h>
#include <shlwapi.h>
#include <cstdio>
#ifdef _MSC_VER
#pragma comment(lib, "user32.lib")
#pragma comment(lib, "shlwapi.lib")
#endif
/* set startup event in child process:
	HANDLE startEvent = OpenEventW(EVENT_MODIFY_STATE, TRUE, L"startup-time-event");
	SetEvent(startEvent);
	CloseHandle(startEvent);
*/
int __cdecl wmain(int argc, wchar_t *argv[]) {
	int runCount = 10;
	int interval = 1000;
	int delayTime = 1000;
	wchar_t command[MAX_PATH*2];
	memset(command, 0, sizeof(int));
	UINT commandLength = 0;
	bool option = true;
	enum class WaitType { InputIdle, ProcessFinish, StartEvent, } waitType = WaitType::InputIdle;
	static LPCWSTR const waitName[] = { L"idle", L"finish", L"event", };

	for (int i = 1; i < argc; i++) {
		LPWSTR arg = argv[i];
		if (option && i < 4 && arg[0] >= '0' && arg[0] <= '9') {
			const int value = StrToIntW(arg);
			if (i == 1) {
				runCount = value;
			} else if (i == 2) {
				interval = value;
			} else {
				delayTime = value;
			}
		} else if (option && (arg[0] == '-' || arg[0] == '/') && (arg[1] == '-' || arg[1] == '?' || arg[1] == 'w')) {
			option = false;
			if (arg[1] == 'w') {
				waitType = (arg[2] == 'e')? WaitType::StartEvent : WaitType::ProcessFinish;
			}
		} else if (arg[0]) {
			commandLength = GetFullPathNameW(arg, _countof(command), command, nullptr);
			if (commandLength == 0) {
				lstrcpynW(command, arg, _countof(command));
				commandLength = lstrlenW(command);
			}
			break;
		}
	}
	if (commandLength == 0) {
		wprintf(L"%s [runCount [interval [delayTime]]] [-w | -we] command\n", argv[0]);
		return 0;
	}
	wprintf(L"run: %d interval: %d delay: %d wait: %s command: %s\n", runCount, interval, delayTime, waitName[static_cast<UINT>(waitType)], command);

	LARGE_INTEGER freq;
	QueryPerformanceFrequency(&freq);
	SECURITY_ATTRIBUTES eventAttribute{sizeof(SECURITY_ATTRIBUTES), nullptr, TRUE};
	HANDLE startEvent = CreateEventW(&eventAttribute, TRUE, FALSE, L"startup-time-event");
	LONGLONG total = 0;
	int count = 0;
	commandLength = (commandLength + 1) * sizeof(wchar_t);
	const auto waitFunc = (waitType == WaitType::InputIdle)? WaitForInputIdle : WaitForSingleObject;
	for (int i = 0; i < runCount; i++) {
		PROCESS_INFORMATION procInfo{};
		STARTUPINFOW startInfo{};
		startInfo.cb = sizeof(STARTUPINFO);
		wchar_t commandLine[MAX_PATH*2];
		memcpy(commandLine, command, commandLength);
		ResetEvent(startEvent);
		LARGE_INTEGER begin;
		QueryPerformanceCounter(&begin);
		if (CreateProcessW(nullptr, commandLine, nullptr, nullptr, TRUE, 0, nullptr, nullptr, &startInfo, &procInfo)) {
			HANDLE waitObject = (waitType == WaitType::StartEvent)? startEvent : procInfo.hProcess;
			waitFunc(waitObject, INFINITE);
			LARGE_INTEGER end;
			QueryPerformanceCounter(&end);
			++count;
			total += end.QuadPart - begin.QuadPart;
			if (waitType != WaitType::ProcessFinish) {
				Sleep(delayTime);
				TerminateProcess(procInfo.hProcess, 0);
			}
			CloseHandle(procInfo.hProcess);
			CloseHandle(procInfo.hThread);
		} else {
			break;
		}
		Sleep(interval);
	}
	CloseHandle(startEvent);
	if (count != 0) {
		const double duration = (total * 1000) / static_cast<double>(freq.QuadPart) / count;
		wprintf(L"run: %d average: %.6f\n", count, duration);
	}
	return 0;
}
