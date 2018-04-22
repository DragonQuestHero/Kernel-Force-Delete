#include "CRTCPP.hpp"
#include "IO_Control.h"
#include "Kernel_Force_Delete.h"


void DriverUnload(PDRIVER_OBJECT drive_object)
{
	DbgPrint("Unload Over!\n");
}

extern "C" NTSTATUS DriverEntry(PDRIVER_OBJECT drive_object, PUNICODE_STRING path)
{
	drive_object->DriverUnload = DriverUnload;

	IO_Control *i = new IO_Control(drive_object);
	i->Create_IO_Control();


	Kernel_Force_Delete *f = new Kernel_Force_Delete();

	return STATUS_SUCCESS;
}