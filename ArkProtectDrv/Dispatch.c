#include "Dispatch.h"



NTSTATUS
APIoControlPassThrough(IN PDEVICE_OBJECT DeviceObject, IN PIRP Irp)
{
	NTSTATUS			Status = STATUS_SUCCESS;
	PVOID				InputBuffer = NULL;
	PVOID               OutputBuffer = NULL;
	UINT32				InputLength = 0;
	UINT32				OutputLength = 0;
	PIO_STACK_LOCATION	IrpStack;
	UINT32				IoControlCode;

	IrpStack = IoGetCurrentIrpStackLocation(Irp);		// ��õ�ǰIrp��ջ
	InputBuffer = IrpStack->Parameters.DeviceIoControl.Type3InputBuffer;
	OutputBuffer = Irp->UserBuffer;
	InputLength = IrpStack->Parameters.DeviceIoControl.InputBufferLength;
	OutputLength = IrpStack->Parameters.DeviceIoControl.OutputBufferLength;

	Irp->IoStatus.Status = STATUS_SUCCESS;
	Irp->IoStatus.Information = 0;

	switch (IrpStack->MajorFunction)
	{
	case IRP_MJ_DEVICE_CONTROL:
	{
		IoControlCode = IrpStack->Parameters.DeviceIoControl.IoControlCode;
		switch (IoControlCode)
		{
			//////////////////////////////////////////////////////////////////////////
			// ProcessCore

		case IOCTL_ARKPROTECT_PROCESSNUM:
		{
			DbgPrint("Get Process Count\r\n");

			__try
			{
				ProbeForWrite(OutputBuffer, OutputLength, sizeof(UINT32));

				Status = APGetProcessNum(OutputBuffer);

				Irp->IoStatus.Status = Status;
			}
			__except (EXCEPTION_EXECUTE_HANDLER)
			{
				DbgPrint("Catch Exception\r\n");
				Status = STATUS_UNSUCCESSFUL;
			}

			break; 
		}
		case IOCTL_ARKPROTECT_ENUMPROCESS:
		{
			DbgPrint("Enum Process\r\n");

			__try
			{
				ProbeForWrite(OutputBuffer, OutputLength, sizeof(UINT8));

				Status = APEnumProcessInfo(OutputBuffer, OutputLength);

				Irp->IoStatus.Status = Status;
			}
			__except (EXCEPTION_EXECUTE_HANDLER)
			{
				DbgPrint("Catch Exception\r\n");
				Status = STATUS_UNSUCCESSFUL;
			}

			break;
		}



		default:
			Irp->IoStatus.Status = STATUS_INVALID_PARAMETER;
			break;
		}
	}
	default:
		break;
	}

	Status = Irp->IoStatus.Status;
	IoCompleteRequest(Irp, IO_NO_INCREMENT);

	return Status;
}


