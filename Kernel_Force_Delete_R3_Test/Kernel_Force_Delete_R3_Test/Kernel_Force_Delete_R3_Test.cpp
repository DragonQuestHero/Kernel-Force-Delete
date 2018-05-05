// Drive_Control_R3.cpp : 定义控制台应用程序的入口点。
//

#include "stdafx.h"

#include <windows.h>
#include <string>

using namespace std;

#define PROTECT_PROCESS CTL_CODE(FILE_DEVICE_UNKNOWN,0x7100,METHOD_BUFFERED ,FILE_ANY_ACCESS)


static std::wstring C_TO_W(std::string str)
{
	std::wstring result;
	DWORD strsize = MultiByteToWideChar(CP_ACP, 0, str.data(), -1, NULL, 0);
	wchar_t *pwstr = new wchar_t[strsize];
	MultiByteToWideChar(CP_ACP, 0, str.data(), -1, pwstr, strsize);
	result = pwstr;
	delete[] pwstr;
	return result;
}

int Delete(const wchar_t *path ,int lenght)
{
	HANDLE m_hDevice = CreateFileA("\\\\.\\IO_Control", GENERIC_READ | GENERIC_WRITE, 0,
		NULL, OPEN_EXISTING, FILE_ATTRIBUTE_NORMAL, NULL);
	if (INVALID_HANDLE_VALUE == m_hDevice)
	{
		return 0;
	}
	DWORD dwRet = 0;
	wchar_t p[1024] = { 0 };
	memcpy(p, path, lenght*2);
	DeviceIoControl(m_hDevice, PROTECT_PROCESS, p, sizeof(p), p, sizeof(p), &dwRet, NULL); //hook it.
	CloseHandle(m_hDevice);
	return 1;
}


int main(int argc, char* argv[])
{
	string str = "\\??\\" + string(argv[1]);
	wstring temp_str = C_TO_W(str);
	Delete(temp_str.data(), temp_str.length());

	return 0;
}

