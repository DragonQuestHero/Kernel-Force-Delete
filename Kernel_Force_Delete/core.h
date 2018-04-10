#pragma once

unsigned long MANHATAN_MOD = 0x31337;
wchar_t *driverName = L"\\Device\\mnhmod";
wchar_t *symLink = L"\\??\\mnhmod";

void OnUnload( IN PDRIVER_OBJECT pDriverObject );
NTSTATUS IceDeviceControl(PVOID pInputBuffer, ULONG InputBufferLength,
						  PVOID pOutputBuffer, ULONG OutputBufferLength,
						  ULONG ControlCode, PIO_STATUS_BLOCK pIoStatus);
NTSTATUS Dispatch_IRP_Routine(IN PDEVICE_OBJECT pDeviceObject, IN PIRP pIrp);
