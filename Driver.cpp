#include "Driver.h"
#include <ntddk.h>
#include <wdf.h>

void Print(PCHAR pWriteDataBuffer, ULONG size)
{
    if (pWriteDataBuffer + size == '\0')
    {
        DbgPrint(pWriteDataBuffer);
    }
    else
    {
        DbgPrint("Received data without \\0!\r\n");
    }
}

NTSTATUS UnsupportedFunction(PDEVICE_OBJECT DeviceObject, PIRP Irp)
{
    DbgPrint("Unsupported function was called!\r\n");
    return STATUS_SUCCESS;
}

NTSTATUS Write(PDEVICE_OBJECT DeviceObject, PIRP Irp)
{
    NTSTATUS NtStatus = STATUS_SUCCESS;
    DbgPrint("Write function was called\r\n");
    
    PIO_STACK_LOCATION pIoStackIrp = IoGetCurrentIrpStackLocation(Irp);
    if(pIoStackIrp)
    {
        PCHAR pWriteDataBuffer = static_cast<PCHAR>(MmGetSystemAddressForMdlSafe(Irp->MdlAddress, NormalPagePriority));
    
        if(pWriteDataBuffer)
        {                             
            Print(pWriteDataBuffer, pIoStackIrp->Parameters.Write.Length);
        }
    }

    return NtStatus;
}

NTSTATUS Read(PDEVICE_OBJECT DeviceObject, PIRP Irp)
{
    NTSTATUS NtStatus = STATUS_BUFFER_TOO_SMALL;
    PCHAR pReturnData = "Hello from the Kernel!";
    unsigned long dwDataSize = sizeof(pReturnData);
    unsigned long dwDataRead = 0;

    DbgPrint("Read function was called\r\n");
    PIO_STACK_LOCATION pIoStackIrp = IoGetCurrentIrpStackLocation(Irp);

    if (pIoStackIrp && Irp->MdlAddress)
    {
        PCHAR pReadDataBuffer = static_cast<PCHAR>(MmGetSystemAddressForMdlSafe(Irp->MdlAddress, NormalPagePriority));

        if (pReadDataBuffer && pIoStackIrp->Parameters.Read.Length >= dwDataSize)
        {
            RtlCopyMemory(pReadDataBuffer, pReturnData, dwDataSize);
            dwDataRead = dwDataSize;
            NtStatus = STATUS_SUCCESS;
        }
    }

    Irp->IoStatus.Status = NtStatus;
    Irp->IoStatus.Information = dwDataRead;
    IoCompleteRequest(Irp, IO_NO_INCREMENT);

    return NtStatus;
}

NTSTATUS Ioctl_DirectInIo(PIRP Irp, PIO_STACK_LOCATION pIoStackIrp)
{
    NTSTATUS NtStatus = STATUS_UNSUCCESSFUL;
    DbgPrint("Ioctl_DirectInIo function was called\r\n");

    PCHAR pInputBuffer = static_cast<PCHAR>(Irp->AssociatedIrp.SystemBuffer);
    Print(pInputBuffer, pIoStackIrp->Parameters.DeviceIoControl.InputBufferLength);

    return NtStatus;
}

NTSTATUS Ioctl_DirectOutIo(PIRP Irp, PIO_STACK_LOCATION pIoStackIrp, unsigned long* pdwDataWritten)
{
    NTSTATUS NtStatus = STATUS_UNSUCCESSFUL;
    PCHAR pReturnData = "IOCTL - Direct Out I/O From Kernel!";
    unsigned long dwDataSize = sizeof(pReturnData);
    DbgPrint("Ioctl_DirectOutIo function was called\r\n");

    PCHAR pOutputBuffer = NULL;
    if (Irp->MdlAddress)
    {
        pOutputBuffer = static_cast<PCHAR>(MmGetSystemAddressForMdlSafe(Irp->MdlAddress, NormalPagePriority));
    }

    if (pOutputBuffer)
    {
        if (pIoStackIrp->Parameters.DeviceIoControl.OutputBufferLength >= dwDataSize)
        {
            RtlCopyMemory(pOutputBuffer, pReturnData, dwDataSize);
            *pdwDataWritten = dwDataSize;
            NtStatus = STATUS_SUCCESS;
        }
        else
        {
            *pdwDataWritten = dwDataSize;
            NtStatus = STATUS_BUFFER_TOO_SMALL;
        }
    }

    return NtStatus;
}

NTSTATUS NARUTO_LOL(PIRP Irp, PIO_STACK_LOCATION pIoStackIrp, unsigned long* pdwDataWritten)
{
    NTSTATUS NtStatus = STATUS_UNSUCCESSFUL;
    PCHAR pReturnData = "NARUTO_LOL: Ninja has no goal - there is only way";
    unsigned long dwDataSize = sizeof(pReturnData);
    DbgPrint("NARUTO_LOL function was called\r\n");

    PCHAR pOutputBuffer = NULL;
    if (Irp->MdlAddress)
    {
        pOutputBuffer = static_cast<PCHAR>(MmGetSystemAddressForMdlSafe(Irp->MdlAddress, NormalPagePriority));
    }

    if (pOutputBuffer)
    {
        if (pIoStackIrp->Parameters.DeviceIoControl.OutputBufferLength >= dwDataSize)
        {
            RtlCopyMemory(pOutputBuffer, pReturnData, dwDataSize);
            *pdwDataWritten = dwDataSize;
            NtStatus = STATUS_SUCCESS;
        }
        else
        {
            *pdwDataWritten = dwDataSize;
            NtStatus = STATUS_BUFFER_TOO_SMALL;
        }
    }

    return NtStatus;
}

NTSTATUS Example_IoControl(PDEVICE_OBJECT DeviceObject, PIRP Irp)
{
    NTSTATUS NtStatus = STATUS_NOT_SUPPORTED;
    unsigned long dwDataWritten = 0;

    DbgPrint("IoControl function was called\r\n");
    PIO_STACK_LOCATION pIoStackIrp = IoGetCurrentIrpStackLocation(Irp);
    if (pIoStackIrp)
    {
        switch (pIoStackIrp->Parameters.DeviceIoControl.IoControlCode)
        {
        case IOCTL_EXAMPLE_SAMPLE_DIRECT_IN_IO:
        {
            NtStatus = Ioctl_DirectInIo(Irp, pIoStackIrp);
            break;
        }
        case IOCTL_EXAMPLE_SAMPLE_DIRECT_OUT_IO:
        {
            NtStatus = Ioctl_DirectOutIo(Irp, pIoStackIrp, &dwDataWritten);
            break;
        }
        case IOCTL_EXAMPLE_SAMPLE_NARUTO_LOL:
        {
            NtStatus = NARUTO_LOL(Irp, pIoStackIrp, &dwDataWritten);
            break;
        }
        default:
        {
            DbgPrint("Unsupported ioctl!\r\n");
        }
        }
    }

    Irp->IoStatus.Status = NtStatus;
    Irp->IoStatus.Information = dwDataWritten;
    IoCompleteRequest(Irp, IO_NO_INCREMENT);

    return NtStatus;
}

VOID Example_Unload(PDRIVER_OBJECT  DriverObject)
{

    UNICODE_STRING usDosDeviceName;

    DbgPrint("Example_Unload Called \r\n");

    RtlInitUnicodeString(&usDosDeviceName, L"\\DosDevices\\Example");
    IoDeleteSymbolicLink(&usDosDeviceName);

    IoDeleteDevice(DriverObject->DeviceObject);
}

extern "C"
NTSTATUS DriverEntry(PDRIVER_OBJECT  pDriverObject, PUNICODE_STRING  pRegistryPath)
{
    NTSTATUS NtStatus = STATUS_SUCCESS;
    unsigned long uiIndex = 0;
    PDEVICE_OBJECT pDeviceObject = NULL;
    UNICODE_STRING usDriverName, usDosDeviceName;

    DbgPrint("DriverEntry Called \r\n");

    RtlInitUnicodeString(&usDriverName, L"\\Device\\Example");
    RtlInitUnicodeString(&usDosDeviceName, L"\\DosDevices\\Example");

    NtStatus = IoCreateDevice(pDriverObject, 0,
        &usDriverName,
        FILE_DEVICE_UNKNOWN,
        FILE_DEVICE_SECURE_OPEN,
        FALSE, &pDeviceObject);

    for (uiIndex = 0; uiIndex < IRP_MJ_MAXIMUM_FUNCTION; uiIndex++)
        pDriverObject->MajorFunction[uiIndex] = UnsupportedFunction;

    pDriverObject->MajorFunction[IRP_MJ_DEVICE_CONTROL] = Example_IoControl;
    pDriverObject->MajorFunction[IRP_MJ_READ] = Read;
    pDriverObject->MajorFunction[IRP_MJ_WRITE] = Write;

    pDriverObject->DriverUnload = Example_Unload;

    pDeviceObject->Flags |= DO_DIRECT_IO;
    pDeviceObject->Flags &= (~DO_DEVICE_INITIALIZING);

    IoCreateSymbolicLink(&usDosDeviceName, &usDriverName);

    return NtStatus;
}