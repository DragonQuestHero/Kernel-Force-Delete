#include "IO_Control.h"
#include "Kernel_Force_Delete.h"

#define DEVICE_NAME L"\\Device\\IO_Control"
#define LINK_NAME L"\\??\\IO_Control"

#define TEST_1 CTL_CODE(FILE_DEVICE_UNKNOWN,0x7100,METHOD_BUFFERED ,FILE_ANY_ACCESS)
#define TEST_2 CTL_CODE(FILE_DEVICE_UNKNOWN,0x7101,METHOD_BUFFERED ,FILE_ANY_ACCESS)

NTSTATUS IO_Control::Create_IO_Control()
{
	NTSTATUS status = 0;
	//创建设备对象
	RtlInitUnicodeString(&Device_Name, DEVICE_NAME);
	status = IoCreateDevice(Driver_Object, 0, &Device_Name, FILE_DEVICE_UNKNOWN, 0, FALSE, &Device_Object);
	if (!NT_SUCCESS(status))
	{
		DbgPrint("Create Device error!\n");
		return status;
	}

	Device_Object->Flags |= DO_BUFFERED_IO;
	//创建符号连接
	RtlInitUnicodeString(&Link_Name, LINK_NAME);
	status = IoCreateSymbolicLink(&Link_Name, &Device_Name);
	if (!NT_SUCCESS(status))
	{
		IoDeleteDevice(Device_Object);
		DbgPrint("Create Link error!\n");
		return status;
	}

	DbgPrint("Create Device and Link SUCCESS!\n");

	Driver_Object->MajorFunction[IRP_MJ_CREATE] = IO_Control::IO_Default;
	Driver_Object->MajorFunction[IRP_MJ_DEVICE_CONTROL] = IO_Control::Code_Control_Center;

	return STATUS_SUCCESS;
}

NTSTATUS IO_Control::Delete_IO_Control()
{
	IoDeleteSymbolicLink(&Link_Name);
	IoDeleteDevice(Device_Object);
	DbgPrint("Link_Unload\n");
	return STATUS_SUCCESS;
}

NTSTATUS IO_Control::IO_Default(PDEVICE_OBJECT  DeviceObject, PIRP  pIrp)
{
	pIrp->IoStatus.Status = STATUS_SUCCESS;
	pIrp->IoStatus.Information = 0;
	IoCompleteRequest(pIrp, IO_NO_INCREMENT);
	return STATUS_SUCCESS;
}

NTSTATUS IO_Control::Code_Control_Center(PDEVICE_OBJECT  DeviceObject, PIRP  pIrp)
{
	PIO_STACK_LOCATION irp = IoGetCurrentIrpStackLocation(pIrp);
	ULONG Io_Control_Code = irp->Parameters.DeviceIoControl.IoControlCode;
	ULONG Input_Lenght = irp->Parameters.DeviceIoControl.InputBufferLength;
	ULONG Output_Lenght = irp->Parameters.DeviceIoControl.OutputBufferLength;

	if (Io_Control_Code == TEST_1)
	{
		WCHAR *temp_path = new WCHAR[Input_Lenght];
		RtlCopyMemory(temp_path, pIrp->AssociatedIrp.SystemBuffer, Input_Lenght);
		DbgPrint("%S", temp_path);
		Kernel_Force_Delete::Delete_File_Mode1(temp_path);
	}

	DECLARE_CONST_UNICODE_STRING(return_str, L"SUCCESS");
	RtlCopyMemory(pIrp->AssociatedIrp.SystemBuffer, return_str.Buffer, return_str.MaximumLength);
	pIrp->IoStatus.Status = STATUS_SUCCESS;
	pIrp->IoStatus.Information = return_str.MaximumLength;
	IoCompleteRequest(pIrp, IO_NO_INCREMENT);

	return STATUS_SUCCESS;
}