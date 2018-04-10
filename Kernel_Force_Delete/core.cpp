//#include <ntddk.h>
#include <ntdef.h>
#include <ntifs.h>
#include <string.h>
#include "core.h"
#include "utilities.h"
#include "io_codes.h"

NTSTATUS DriverEntry( IN PDRIVER_OBJECT pDriverObject, IN PUNICODE_STRING theRegistryPath )
{
	NTSTATUS ntStatus;
	PDEVICE_OBJECT mnhmod;
	UNICODE_STRING uDriverName;
	UNICODE_STRING uSymLink;

	DbgBreakPoint();
	ForceDelete(L"\\??\\C:\\1.exe");

	RtlInitUnicodeString(&uDriverName,driverName);
	RtlInitUnicodeString(&uSymLink,symLink);
	ntStatus = IoCreateDevice(pDriverObject,
					0,
					&uDriverName,
					MANHATAN_MOD,
					0,
					TRUE,
					&mnhmod);

	if(ntStatus != STATUS_SUCCESS)
	{
		DbgPrint("ManhatanMod error");
		return 0;
	}


	DbgPrint("ManhatanMod loaded successful");
	//create symlink
	IoCreateSymbolicLink(&uSymLink,&uDriverName);
	pDriverObject->DriverUnload = OnUnload;
	
	pDriverObject->MajorFunction[IRP_MJ_SHUTDOWN]        =
    pDriverObject->MajorFunction[IRP_MJ_CREATE]          =
    pDriverObject->MajorFunction[IRP_MJ_CLOSE]           =
	pDriverObject->MajorFunction[IRP_MJ_DEVICE_CONTROL]  = Dispatch_IRP_Routine;

	return ntStatus;	
}
//-------------------------------------------------------------------------------------------------------------------
void OnUnload( IN PDRIVER_OBJECT pDriverObject )
{
	UNICODE_STRING uSymLink;
	RtlInitUnicodeString(&uSymLink,symLink);
	IoDeleteSymbolicLink(&uSymLink);
	if(pDriverObject->DeviceObject != 0 )
	{
		IoDeleteDevice(pDriverObject->DeviceObject);
	}	
	DbgPrint("ManhatanMod unloaded successful");
}
//-------------------------------------------------------------------------------------------------------------------
NTSTATUS 
MnhDeviceControl(
		PVOID pInputBuffer, ULONG InputBufferLength,
	    PVOID pOutputBuffer, ULONG OutputBufferLength,
	    ULONG ControlCode, PIO_STATUS_BLOCK pIoStatus
			  )
{
	NTSTATUS stat;
	OBJECT_ATTRIBUTES obj;
	UNICODE_STRING target;
	ANSI_STRING ansi;
		
	pIoStatus->Status = STATUS_SUCCESS;
    pIoStatus->Information = 0;
	
	
	switch ( ControlCode ) 
	{
	case IOCTL_DeleteFile:
		stat = ForceDelete((wchar_t*)pInputBuffer);		
		*(NTSTATUS*)pOutputBuffer = stat;
		pIoStatus->Information = 4;
		break;
		default:		
			pIoStatus->Status = STATUS_NOT_SUPPORTED;
			return pIoStatus->Status;
	}

 return pIoStatus->Status;
}
//-------------------------------------------------------------------------------------------------------------------
NTSTATUS 
Dispatch_IRP_Routine(
					 IN PDEVICE_OBJECT pDeviceObject,
					 IN PIRP pIrp
					 )
{
	PVOID pInputBuffer;
	PVOID pOutputBuffer;
	ULONG inputBufferLength;
	ULONG outputBufferLength;
	ULONG ControlCode;
	NTSTATUS				ntStatus;
	PIO_STACK_LOCATION      pIrpStack;
	
	pIrp->IoStatus.Status      = STATUS_SUCCESS;
    pIrp->IoStatus.Information = 0;
	
	pInputBuffer  = pIrp->AssociatedIrp.SystemBuffer;
	pOutputBuffer = pIrp->AssociatedIrp.SystemBuffer;
		
	//locate the stack for current IRP
	pIrpStack = IoGetCurrentIrpStackLocation(pIrp);
	
	//Lenght of buffers
	inputBufferLength  = pIrpStack->Parameters.DeviceIoControl.InputBufferLength;
	outputBufferLength = pIrpStack->Parameters.DeviceIoControl.OutputBufferLength;

	ControlCode = pIrpStack->Parameters.DeviceIoControl.IoControlCode;

    switch (pIrpStack->MajorFunction)
	{
    case IRP_MJ_DEVICE_CONTROL:
		ntStatus = MnhDeviceControl(pInputBuffer, inputBufferLength,
									pOutputBuffer, outputBufferLength,
									ControlCode, &pIrp->IoStatus
									);
		break;
	}
	
	IoCompleteRequest( pIrp, IO_NO_INCREMENT );
		
	return ntStatus;
}
