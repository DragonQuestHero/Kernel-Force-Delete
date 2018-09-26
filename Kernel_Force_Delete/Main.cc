#include "CRTCPP.hpp"
#include "IO_Control.h"
#include "Kernel_Force_Delete.h"

IO_Control *_IO_Control = nullptr;
Kernel_Force_Delete *_Kernel_Force_Delete = nullptr;


void DriverUnload(PDRIVER_OBJECT drive_object)
{
	delete _IO_Control;
	delete _Kernel_Force_Delete;
	DbgPrint("Unload Over!\n");
}

extern "C" NTSTATUS DriverEntry(PDRIVER_OBJECT drive_object, PUNICODE_STRING path)
{
	drive_object->DriverUnload = DriverUnload;

	_IO_Control = new IO_Control(drive_object);
	_IO_Control->Create_IO_Control();


	_Kernel_Force_Delete = new Kernel_Force_Delete();
	DbgBreakPoint();
	_Kernel_Force_Delete->Unlock_File_Mode2(L"C:\\1.txt");

	return STATUS_SUCCESS;
}