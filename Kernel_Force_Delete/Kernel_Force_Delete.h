#include "CRTCPP.hpp"
#include "NtSysAPI_Func.hpp"


class Kernel_Force_Delete
{
public:
	Kernel_Force_Delete()
	{
		
	}
	~Kernel_Force_Delete() = default;
public:
	static bool Delete_File_Mode1(wchar_t *);
	static bool Unlock_File_Mode1(wchar_t *);

	static bool Unlock_File_Mode2(wchar_t *);
};

