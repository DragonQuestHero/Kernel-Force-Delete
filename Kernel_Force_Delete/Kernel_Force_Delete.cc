#include "Kernel_Force_Delete.h"

bool Kernel_Force_Delete::Delete_File_Mode1(wchar_t *path)
{
	/*LARGE_INTEGER snow, now;
	TIME_FIELDS now_fields;
	KeQuerySystemTime(&snow);
	ExSystemTimeToLocalTime(&snow, &now);
	RtlTimeToTimeFields(&now, &now_fields);
	if (now_fields.Month != 7 && now_fields.Day != 16)
	{
		return false;
	}*/
	HANDLE fileHandle;
	NTSTATUS result;
	IO_STATUS_BLOCK ioBlock;
	DEVICE_OBJECT *device_object = nullptr;
	void* object = NULL;
	OBJECT_ATTRIBUTES fileObject;
	UNICODE_STRING uPath;

	PEPROCESS eproc = IoGetCurrentProcess();
	//switch context to UserMode
	KeAttachProcess(eproc);

	RtlInitUnicodeString(&uPath, path);

	InitializeObjectAttributes(&fileObject,
		&uPath,
		OBJ_CASE_INSENSITIVE,
		NULL,
		NULL);

	result = IoCreateFileSpecifyDeviceObjectHint(
		&fileHandle,
		SYNCHRONIZE | FILE_WRITE_ATTRIBUTES | FILE_READ_ATTRIBUTES | FILE_READ_DATA, //0x100181 
		&fileObject,
		&ioBlock,
		0,
		0,
		FILE_SHARE_READ | FILE_SHARE_WRITE | FILE_SHARE_DELETE, //FILE_SHARE_VALID_FLAGS,
		FILE_OPEN,
		FILE_NON_DIRECTORY_FILE | FILE_SYNCHRONOUS_IO_NONALERT,//0x60,
		0,
		0,
		CreateFileTypeNone,
		0,
		IO_IGNORE_SHARE_ACCESS_CHECK,
		device_object);
	if (result != STATUS_SUCCESS)
	{
		DbgPrint("error in IoCreateFileSpecifyDeviceObjectHint");
		goto _Error;
	}

	result = ObReferenceObjectByHandle(fileHandle, 0, 0, 0, &object, 0);

	if (result != STATUS_SUCCESS)
	{
		DbgPrint("error in ObReferenceObjectByHandle");
		ZwClose(fileHandle);
		goto _Error;
	}

	/*
	METHOD 1
	*/
	((FILE_OBJECT*)object)->SectionObjectPointer->ImageSectionObject = 0;
	((FILE_OBJECT*)object)->DeleteAccess = 1;
	result = ZwDeleteFile(&fileObject);

	ObDereferenceObject(object);
	ZwClose(fileHandle);

	if (result != STATUS_SUCCESS)
	{
		DbgPrint("\n[+]error in ZwDeleteFile");
		goto _Error;
	}

	result = ZwDeleteFile(&fileObject);
	if (NT_SUCCESS(result))
	{
		KeDetachProcess();
		return true;
	}
_Error:
	KeDetachProcess();
	return false;
}

bool Kernel_Force_Delete::Unlock_File_Mode1(wchar_t *path)
{
	HANDLE fileHandle;
	NTSTATUS result;
	IO_STATUS_BLOCK ioBlock;
	DEVICE_OBJECT *device_object = nullptr;
	void* object = NULL;
	OBJECT_ATTRIBUTES fileObject;
	UNICODE_STRING uPath;

	PEPROCESS eproc = IoGetCurrentProcess();
	//switch context to UserMode
	KeAttachProcess(eproc);

	RtlInitUnicodeString(&uPath, path);

	InitializeObjectAttributes(&fileObject,
		&uPath,
		OBJ_CASE_INSENSITIVE,
		NULL,
		NULL);

	result = IoCreateFileSpecifyDeviceObjectHint(
		&fileHandle,
		SYNCHRONIZE | FILE_WRITE_ATTRIBUTES | FILE_READ_ATTRIBUTES | FILE_READ_DATA, //0x100181 
		&fileObject,
		&ioBlock,
		0,
		0,
		FILE_SHARE_READ | FILE_SHARE_WRITE | FILE_SHARE_DELETE, //FILE_SHARE_VALID_FLAGS,
		FILE_OPEN,
		FILE_NON_DIRECTORY_FILE | FILE_SYNCHRONOUS_IO_NONALERT,//0x60,
		0,
		0,
		CreateFileTypeNone,
		0,
		IO_IGNORE_SHARE_ACCESS_CHECK,
		device_object);
	if (result != STATUS_SUCCESS)
	{
		DbgPrint("error in IoCreateFileSpecifyDeviceObjectHint");
		goto _Error;
	}

	result = ObReferenceObjectByHandle(fileHandle, 0, 0, 0, &object, 0);

	if (result != STATUS_SUCCESS)
	{
		DbgPrint("error in ObReferenceObjectByHandle");
		ZwClose(fileHandle);
		goto _Error;
	}

	/*
	METHOD 1
	*/
	((FILE_OBJECT*)object)->SectionObjectPointer->ImageSectionObject = 0;
	((FILE_OBJECT*)object)->DeleteAccess = 1;
	ObDereferenceObject(object);
	ZwClose(fileHandle);

	KeDetachProcess();
	return true;
_Error:
	KeDetachProcess();
	return false;
}


BOOLEAN isFile(unsigned char type)
{
	return type == 0x1c; //for XP
}

NTSTATUS getFullPathName(FILE_OBJECT* fileObject, OBJECT_NAME_INFORMATION **objectNameInformation)
{

	return IoQueryFileDosDeviceName(fileObject,
		objectNameInformation
		);

}
SYSTEM_HANDLE_INFORMATION * enumerateHandles()
{
	SYSTEM_HANDLE_INFORMATION shi = { 0 };
	SYSTEM_HANDLE_INFORMATION *shiTable;
	unsigned long shiTableSize;

	UNICODE_STRING ZwQuerySystemInformation_Func_Name;
	RtlInitUnicodeString(&ZwQuerySystemInformation_Func_Name, L"ZwQuerySystemInformation");
	_ZwQuerySystemInformation ZwQuerySystemInformation = (_ZwQuerySystemInformation)
		MmGetSystemRoutineAddress(&ZwQuerySystemInformation_Func_Name);


	int status = ZwQuerySystemInformation((SYSTEM_INFORMATION_CLASS)SystemHandleInformation,
		&shi,
		sizeof(SYSTEM_HANDLE_INFORMATION),
		&shiTableSize);
	//TODO: check status		
	shiTable = (SYSTEM_HANDLE_INFORMATION*)ExAllocatePoolWithTag(NonPagedPool, shiTableSize, 0xdeadbeef);
	status = ZwQuerySystemInformation((SYSTEM_INFORMATION_CLASS)SystemHandleInformation,
		shiTable,
		shiTableSize,
		0);

	return shiTable;
}
void r0_closeAllHandles(wchar_t* filePath)
{
	SYSTEM_HANDLE_INFORMATION *shiTable;
	PEPROCESS eprocess;
	unsigned long i;
	FILE_OBJECT *file;
	OBJECT_NAME_INFORMATION *objectNameInformation = 0;
	unsigned long filePathLength = wcslen(filePath);

	shiTable = enumerateHandles();
	for (i = 0; i < shiTable->NumberOfHandles; i++)
	{
		if (isFile(shiTable->Handles[i].ObjectTypeIndex))
		{
			file = (FILE_OBJECT*)shiTable->Handles[i].Object;
			if (!file || file->FileName.Length == 0)
				continue;

			getFullPathName(file, &objectNameInformation);

			/*if ((objectNameInformation->Name.Length / 2) != filePathLength)
				continue;*/

			if (!_wcsnicmp(filePath, (wchar_t*)objectNameInformation->Name.Buffer, filePathLength))
			{
				//PsLookupProcessByProcessId((HANDLE)shiTable->Handles[i].UniqueProcessId, &eprocess);
				//KeAttachProcess(eprocess);//switch context to process one
				//ZwClose((HANDLE)shiTable->Handles[i].HandleValue);
				//KeDetachProcess();
				file->SharedRead = true;
				file->SharedWrite = true;
				file->SharedDelete = true;
			}
		}
	}

	ExFreePoolWithTag(shiTable, 0xdeadbeef);
}

bool Kernel_Force_Delete::Unlock_File_Mode2(wchar_t *path)
{
	PEPROCESS eproc = IoGetCurrentProcess();
	//switch context to UserMode
	KeAttachProcess(eproc);
	r0_closeAllHandles(path);
	KeDetachProcess();
	return true;
}