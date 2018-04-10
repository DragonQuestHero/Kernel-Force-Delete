#pragma once
#include "utilities.h"


wchar_t *g_tmpFile = L"\\??\\C:\\icewall.txt";
wchar_t *g_tmpFileName = L"\\icewall.txt";
wchar_t *g_fileToDelete = L"\\windows\\system32\\drivers\\hldrrr.exe";	
wchar_t *g_fileToDeletePath = L"\\??\\C:\\windows\\system32\\drivers\\hldrrr.exe";	
#define IO_IGNORE_SHARE_ACCESS_CHECK    0x0800

//PDRIVER_DISPATCH saved_mj[3];
PDRIVER_DISPATCH create;
PDRIVER_DISPATCH cleanup;
PDRIVER_DISPATCH close;
//-------------------------------------------------------------------------------------------
NTSTATUS Call_NtfsFsdCreate(PDEVICE_OBJECT device_object,PIRP Irp)
{
	//NTSTATUS rez;
	//__asm
	//{
	//	push Irp
	//	push device_object
	//	mov eax,create
	//	call eax //NtfsFsCreate
	//	mov [rez],eax
	//}
	//return rez;
	return create(device_object, Irp);
}
//-------------------------------------------------------------------------------------------
NTSTATUS Call_NtfsFsdCleanUp(PDEVICE_OBJECT device_object,PIRP Irp)
{
	//NTSTATUS rez;
	//__asm
	//{
	//	push Irp
	//	push device_object
	//	mov eax,cleanup
	//	call eax //NtfsFsCleanUp
	//	mov [rez],eax
	//}
	//return rez;
	return cleanup(device_object, Irp);
}
//-------------------------------------------------------------------------------------------
NTSTATUS Call_NtfsFsdClose(PDEVICE_OBJECT device_object,PIRP Irp)
{
	//NTSTATUS rez;
	//__asm
	//{
	//	push Irp
	//	push device_object
	//	mov eax,close		
	//	call eax //NtfsFsClose
	//	mov [rez],eax
	//}
	//return rez;
	return close(device_object, Irp);
}
//-------------------------------------------------------------------------------------------
NTSTATUS NTAPI
HookedNtfsFsdCreate(PDEVICE_OBJECT DeviceObject,
	   PIRP Irp)
{	
	unsigned long proper_file = 0; //whether we get Irp with interesting for us file_object ?
	UNICODE_STRING uTestFile;
	UNICODE_STRING bfile;
	NTSTATUS rez;
	PFILE_OBJECT pFileObject;
	
	
	RtlInitUnicodeString(&uTestFile,g_fileToDelete);	
	pFileObject = Irp->Tail.Overlay.CurrentStackLocation->FileObject;
	if(!pFileObject)
		goto _end;

	if((pFileObject->FileName.Length / 2) != wcslen(g_tmpFileName))
		goto _end;
			
	if(!_wcsnicmp(g_tmpFileName,
				  (wchar_t*)pFileObject->FileName.Buffer,
				  (pFileObject->FileName.Length / 2)))
	{
			DbgPrint("\n[+]HookedNtfsFsdCreate");
		    bfile = pFileObject->FileName;//backup current string
			pFileObject->FileName = uTestFile; //hook string value 
			proper_file = 1;			
	}

_end:

	rez = Call_NtfsFsdCreate(DeviceObject,Irp);
	if(pFileObject)
	{
		if(proper_file)
		{
			pFileObject->FileName = bfile;
		}
	}
	return 0;
}
//-------------------------------------------------------------------------------------------
NTSTATUS NTAPI
HookedNtfsFsdCleanUp(PDEVICE_OBJECT DeviceObject,
	   PIRP Irp)
{	
	unsigned long proper_file = 0; //whether we get Irp with interesting for us file_object ?
	UNICODE_STRING uTestFile;
	UNICODE_STRING bfile;
	NTSTATUS rez;
	PFILE_OBJECT pFileObject;
	
	RtlInitUnicodeString(&uTestFile,g_fileToDelete);	
	pFileObject = Irp->Tail.Overlay.CurrentStackLocation->FileObject;
	if(!pFileObject)
		goto _end;

	if((pFileObject->FileName.Length / 2) != wcslen(g_tmpFileName))
		goto _end;
			
	if(!_wcsnicmp(g_tmpFileName,
				  (wchar_t*)pFileObject->FileName.Buffer,
				  (pFileObject->FileName.Length / 2)))
	{
			DbgPrint("\n[+]HookedNtfsFsdCleanUp");
		    bfile = pFileObject->FileName;//backup current string
			pFileObject->FileName = uTestFile; //hook string value 
			proper_file = 1;			
	}

_end:

	rez = Call_NtfsFsdCleanUp(DeviceObject,Irp);
	if(pFileObject)
	{
		if(proper_file)
		{
			pFileObject->FileName = bfile;
		}
	}
	return 0;
}
//-------------------------------------------------------------------------------------------
NTSTATUS NTAPI
HookedNtfsFsdClose(PDEVICE_OBJECT DeviceObject,
				   PIRP Irp)
{	
	unsigned long proper_file = 0; //whether we get Irp with interesting for us file_object ?
	UNICODE_STRING uTestFile;
	UNICODE_STRING bfile;
	NTSTATUS rez;
	PFILE_OBJECT pFileObject;
	
	RtlInitUnicodeString(&uTestFile,g_fileToDelete);	
	pFileObject = Irp->Tail.Overlay.CurrentStackLocation->FileObject;
	if(!pFileObject)
		goto _end;

	if((pFileObject->FileName.Length / 2) != wcslen(g_tmpFileName))
		goto _end;
			
	if(!_wcsnicmp(g_tmpFileName,
				  (wchar_t*)pFileObject->FileName.Buffer,
				  (pFileObject->FileName.Length / 2)))
	{
			DbgPrint("\n[+]HookedNtfsFsdClose");
		    bfile = pFileObject->FileName;//backup current string
			pFileObject->FileName = uTestFile; //hook string value 
			proper_file = 1;			
	}

_end:

	rez = Call_NtfsFsdClose(DeviceObject,Irp);
	if(pFileObject)
	{
		if(proper_file)
		{
			pFileObject->FileName = bfile;
		}
	}
	return 0;
}
void hook_it(DEVICE_OBJECT *device_object)
{	
	NTSTATUS result;
	OBJECT_ATTRIBUTES fileObj;
	UNICODE_STRING uTmpFile;
	HANDLE fileHandle;
	IO_STATUS_BLOCK ioStatus;	
	FILE_BASIC_INFORMATION fileBasicInfo;

	//initialize variables related with fake file
	RtlInitUnicodeString(&uTmpFile,g_tmpFile);

	InitializeObjectAttributes(&fileObj,
								&uTmpFile,
								OBJ_CASE_INSENSITIVE,
								NULL,
								NULL);
	
	//save original MJ functions	
	create  = device_object->DriverObject->MajorFunction[0];
	cleanup = device_object->DriverObject->MajorFunction[0x12];
	close   = device_object->DriverObject->MajorFunction[0x2];

	result = ZwCreateFile(&fileHandle,
						  4,
						  &fileObj,
						  &ioStatus,
						  0,
						  0x80,
						  2,
						  3,
						  0x20,
						  0,
						  0);	
	if(result != STATUS_SUCCESS)
		return;
	
	ZwClose(fileHandle);
	
	//install hooks
	device_object->DriverObject->MajorFunction[0]    = HookedNtfsFsdCreate;  
	device_object->DriverObject->MajorFunction[0x12] = HookedNtfsFsdCleanUp;  
	device_object->DriverObject->MajorFunction[0x2]  = HookedNtfsFsdClose;  

	ZwDeleteFile(&fileObj);//launche our hooks
	
	//restore original MJ functions
	device_object->DriverObject->MajorFunction[0]    = create;
	device_object->DriverObject->MajorFunction[0x12] = cleanup;
	device_object->DriverObject->MajorFunction[0x2]  = close;
	
}

NTSTATUS 
ForceDelete(
			wchar_t *path
			)
{
	r0_fileToDelete(path);


	HANDLE fileHandle;
	NTSTATUS result;
	IO_STATUS_BLOCK ioBlock;
	DEVICE_OBJECT *device_object=nullptr;
	void* object = NULL;
	OBJECT_ATTRIBUTES fileObject;
	wchar_t deviceName[100];
	UNICODE_STRING uDeviceName;
	UNICODE_STRING uPath;

	
	PEPROCESS eproc = IoGetCurrentProcess();
	//switch context to UserMode
	KeAttachProcess(eproc);
	
	//initialize file to delete variable
	g_fileToDelete = path;
	g_fileToDelete += 6; //take from \??\C:\example only \example
	

	//e.g "\??\C:\"
	memset(deviceName,0,sizeof(deviceName));
	wcsncpy(deviceName,path,wcslen(path));
	uDeviceName.Buffer = deviceName;
	uDeviceName.Length = wcslen(path)*2;
	//file path to unicode string
	RtlInitUnicodeString(&uPath,path);
	
	InitializeObjectAttributes(&fileObject,
								&uDeviceName,
								OBJ_CASE_INSENSITIVE,
								NULL,
								NULL);

	result = ZwOpenFile(&fileHandle,
						SYNCHRONIZE,
						&fileObject,
						&ioBlock,
						FILE_SHARE_READ,
						FILE_SYNCHRONOUS_IO_NONALERT);

	if(result != STATUS_SUCCESS)
	{
		DbgPrint("Some problems with open file ;[");
		goto _end;
	}

    if ( !ObReferenceObjectByHandle(fileHandle, 0, 0, 0, &object, 0) )
    {

		device_object = IoGetBaseFileSystemDeviceObject((PFILE_OBJECT)object);
      ObfDereferenceObject(object);
    }
	
    ZwClose(fileHandle);	
	
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
			   FILE_SHARE_READ | FILE_SHARE_WRITE |FILE_SHARE_DELETE, //FILE_SHARE_VALID_FLAGS,
			   FILE_OPEN,
			   FILE_NON_DIRECTORY_FILE | FILE_SYNCHRONOUS_IO_NONALERT,//0x60,
			   0,
			   0,
			   CreateFileTypeNone,
			   0,
			   IO_IGNORE_SHARE_ACCESS_CHECK,
			   device_object);
	if(result != STATUS_SUCCESS)
	{
		DbgPrint("error in IoCreateFileSpecifyDeviceObjectHint");
		goto _end;
	}

	result = ObReferenceObjectByHandle(fileHandle, 0, 0, 0, &object, 0);

	if(result != STATUS_SUCCESS)
	{
		DbgPrint("error in ObReferenceObjectByHandle");
		ZwClose(fileHandle);
		goto _end;
	}
	/*
		METHOD 1
	*/
	((FILE_OBJECT*)object)->SectionObjectPointer->ImageSectionObject = 0;
	((FILE_OBJECT*)object)->DeleteAccess = 1;	
	 result = ZwDeleteFile(&fileObject);
	
	if(result != STATUS_SUCCESS)
	{
		DbgPrint("\n[+]error in ZwDeleteFile");
	}
	ObDereferenceObject(object);
	ZwClose(fileHandle);
	 
	result = ZwDeleteFile(&fileObject);
	if(result != STATUS_SUCCESS)
	{
		DbgPrint("\n[+]error in ZwDeleteFile");
		/*
			METHOD 2
		*/		
		r0_fileToDelete(path);
		
		/*
			METHOD 3
			If simple solutions did not help, try this one.
		*/
		hook_it(device_object);
	}
		
_end:
	//goes back to r0
	KeDetachProcess();
	return result;
}
#define MAX_PATH 255
#define U_MAX_PATH 512
typedef struct _MY_SYSTEM_HANDLE_TABLE_ENTRY_INFO {
    USHORT UniqueProcessId;
    USHORT CreatorBackTraceIndex;
    UCHAR ObjectTypeIndex;
    UCHAR HandleAttributes;
    USHORT HandleValue;
    PVOID Object;
    ULONG GrantedAccess;
} MY_SYSTEM_HANDLE_TABLE_ENTRY_INFO, *PMY_SYSTEM_HANDLE_TABLE_ENTRY_INFO;

typedef struct _MY_SYSTEM_HANDLE_INFORMATION {
    ULONG NumberOfHandles;
    MY_SYSTEM_HANDLE_TABLE_ENTRY_INFO Handles[ 1 ];
} MY_SYSTEM_HANDLE_INFORMATION, *MY_PSYSTEM_HANDLE_INFORMATION;
BOOLEAN isFile(unsigned char type)
{
	return type == 0x1c; //for XP
}
NTSTATUS getFullPathName(FILE_OBJECT* fileObject,OBJECT_NAME_INFORMATION **objectNameInformation)
{
	 
	 return IoQueryFileDosDeviceName(fileObject,
							  objectNameInformation
							  );
						      
}
MY_SYSTEM_HANDLE_INFORMATION * enumerateHandles()
{			
	MY_SYSTEM_HANDLE_INFORMATION shi = {0};
	MY_SYSTEM_HANDLE_INFORMATION *shiTable;	
	unsigned long shiTableSize;

	UNICODE_STRING ZwQueryInformationProcess_Func_Name;
	RtlInitUnicodeString(&ZwQueryInformationProcess_Func_Name, L"ZwQuerySystemInformation");
	_ZwQuerySystemInformation ZwQuerySystemInformation = (_ZwQuerySystemInformation)
		MmGetSystemRoutineAddress(&ZwQueryInformationProcess_Func_Name);

	int status  = ZwQuerySystemInformation(SystemHandleInformation,
						     &shi,
							 sizeof(MY_SYSTEM_HANDLE_INFORMATION),
							 &shiTableSize);
	//TODO: check status		
	shiTable = (MY_SYSTEM_HANDLE_INFORMATION*)ExAllocatePoolWithTag(NonPagedPool,shiTableSize,0xdeadbeef);
	status = ZwQuerySystemInformation((SYSTEM_INFORMATION_CLASS)SystemHandleInformation,
									  shiTable,
									  shiTableSize,
									  0);
	
	return shiTable;
}
void r0_closeAllHandles(wchar_t* filePath)
{
	MY_SYSTEM_HANDLE_INFORMATION *shiTable;
	PEPROCESS eprocess;
	unsigned long i;
	FILE_OBJECT *file;
	OBJECT_NAME_INFORMATION *objectNameInformation = 0;
	unsigned long filePathLength = wcslen(filePath);

	shiTable = enumerateHandles();
	for(i = 0; i < shiTable->NumberOfHandles;i++)
	{		
		if( isFile( shiTable->Handles[i].ObjectTypeIndex) )
		{					
			file = (FILE_OBJECT*)shiTable->Handles[i].Object;			
			if(!file || file->FileName.Length == 0)
				continue;
			
			getFullPathName(file,&objectNameInformation);

			if((objectNameInformation->Name.Length/2 ) != filePathLength)
				continue;
					
			if( !_wcsnicmp(filePath,(wchar_t*)objectNameInformation->Name.Buffer,filePathLength) )
			{
				PsLookupProcessByProcessId((HANDLE)shiTable->Handles[i].UniqueProcessId,&eprocess);
				KeAttachProcess(eprocess);//switch context to process one
				ZwClose((HANDLE)shiTable->Handles[i].HandleValue);
				KeDetachProcess();				
			}
		}
	}
	
	ExFreePoolWithTag(shiTable,0xdeadbeef);
}
/* CALL ME IN PROPER PLACE */
void r0_fileToDelete(wchar_t *filePath)
{	
	r0_closeAllHandles(filePath);
	/*
		after closing all handles to the file ,try to delete it
		ofc,,,before it convert DosPath to NtPath (just add \??\ before filePath)
	*/
	return;//ZwDeleteFile();
}