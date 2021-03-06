#include "Public.h"

#include <windows.h>
#include <setupapi.h>
#include <stdio.h>
#include <stdlib.h>
#include <conio.h>
#include <winioctl.h>


PCHAR
GetDevicePath(
IN  LPGUID InterfaceGuid
);

HANDLE hDevice = INVALID_HANDLE_VALUE;


int __cdecl main(void)
{
	PCHAR  DevicePath;
	ULONG nOutput;
	printf("Application Spw_PCIe starting...\n");
	//在这里修改GUID
	DevicePath = GetDevicePath((LPGUID)&GUID_DEVINTERFACE_Spw_PCIe);//这里是不是有问题，把GUID值改一下看是否还能成功？？

	hDevice = CreateFile(DevicePath,
		GENERIC_READ | GENERIC_WRITE,
		FILE_SHARE_READ | FILE_SHARE_WRITE,
		NULL,
		OPEN_EXISTING,
		FILE_ATTRIBUTE_NORMAL,
		NULL);//Creates or opens a file or I/O device. 

	if (hDevice == INVALID_HANDLE_VALUE) {
		printf("ERROR opening device: (%0x) returned from CreateFile\n", GetLastError());
		return 0;
	}
	printf("continue? Y/N?\n");
	while (getchar() != 'Y');
	printf("OK.\n");
	ULONG outBuffer;
	ULONG inBuffer;
	ULONG AddressOffset;
	//------------------read physical address-----------------------
	printf("continue to operate reading physical address? Y/N?\n");
	while (getchar() != 'Y');
	if (!DeviceIoControl(hDevice,
		Spw_PCIe_IOCTL_READ_PADDRESS,
		NULL,
		0,
		&outBuffer,
		sizeof(ULONG),
		&nOutput,
		NULL)
		)
	{
		printf("ERROR: DeviceIoControl returns %0x.", GetLastError());
		printf("error happens! continue? Y/N?\n");
		while (getchar() != 'Y');
		//		goto exit;
		return 0;
	}
	printf("PAddress data:%x\n", outBuffer);
	printf("datasize:%d\n", nOutput);
	//------------------read-----------------------
	nOutput = 0;
	printf("continue to operate reading? Y/N?\n");
	while (getchar() != 'Y');
	printf("please input address offset\n");
	printf(">");
	scanf("%d", &AddressOffset);
	if (!DeviceIoControl(hDevice,
		Spw_PCIe_IOCTL_WRITE_OFFSETADDRESS,
		&AddressOffset,
		sizeof(ULONG),
		NULL,
		0,
		&nOutput,
		NULL)
		)
	{
		printf("ERROR: DeviceIoControl returns %0x.", GetLastError());
		printf("error happens! continue? Y/N?\n");
		while (getchar() != 'Y');
		//		goto exit;
		return 0;
	}
	printf("OffsetAddress has writen to PCIe\n");
	if (!DeviceIoControl(hDevice,
		Spw_PCIe_IOCTL_OUT_BUFFERED,
		NULL,
		0,
		&outBuffer,
		sizeof(ULONG),
		&nOutput,
		NULL)
		)
	{
		printf("ERROR: DeviceIoControl returns %0x.", GetLastError());
		printf("error happens! continue? Y/N?\n");
		while (getchar() != 'Y');
		//		goto exit;
		return 0;
	}
	printf("read data:%d\n", outBuffer);
	printf("datasize:%d\n", nOutput);
	//------------------write----------------------
	printf("continue to operate writing ? Y/N?\n");
	while (getchar() != 'Y');

	printf("please input address offset\n");
	printf(">");
	scanf("%d", &AddressOffset);
	if (!DeviceIoControl(hDevice,
		Spw_PCIe_IOCTL_WRITE_OFFSETADDRESS,
		&AddressOffset,
		sizeof(ULONG),
		NULL,
		0,
		&nOutput,
		NULL)
		)
	{
		printf("ERROR: DeviceIoControl returns %0x.", GetLastError());
		printf("error happens! continue? Y/N?\n");
		while (getchar() != 'Y');
		//		goto exit;
		return 0;
	}
	printf("OffsetAddress has writen to PCIe\n");
	printf("input write data:\n");
	printf(">");
	scanf("%d", &inBuffer);
	if (!DeviceIoControl(hDevice,
		Spw_PCIe_IOCTL_IN_BUFFERED,
		&inBuffer,
		sizeof(ULONG),
		NULL,
		0,
		&nOutput,
		NULL)
		)
	{
		printf("ERROR: DeviceIoControl returns %0x.", GetLastError());
		printf("error happens! continue? Y/N?\n");
		while (getchar() != 'Y');
		//		goto exit;
		return 0;
	}
	printf("write data :%d finished!\n",inBuffer);
	printf("datasize:%d\n", nOutput);
	//------------------read-----------------------
	nOutput = 0;
	printf("continue to operate reading? Y/N?\n");
	while (getchar() != 'Y');
	printf("please input address offset\n");
	printf(">");
	scanf("%d", &AddressOffset);
	if (!DeviceIoControl(hDevice,
		Spw_PCIe_IOCTL_WRITE_OFFSETADDRESS,
		&AddressOffset,
		sizeof(ULONG),
		NULL,
		0,
		&nOutput,
		NULL)
		)
	{
		printf("ERROR: DeviceIoControl returns %0x.", GetLastError());
		printf("error happens! continue? Y/N?\n");
		while (getchar() != 'Y');
		//		goto exit;
		return 0;
	}
	printf("OffsetAddress has writen to PCIe\n");
	if (!DeviceIoControl(hDevice,
		Spw_PCIe_IOCTL_OUT_BUFFERED,
		NULL,
		0,
		&outBuffer,
		sizeof(ULONG),
		&nOutput,
		NULL)
		)
	{
		printf("ERROR: DeviceIoControl returns %0x.", GetLastError());
		printf("error happens! continue? Y/N?\n");
		while (getchar() != 'Y');
		//		goto exit;
		return 0;
	}
	printf("read data:%d\n", outBuffer);
	printf("datasize:%d\n", nOutput);

	printf("continue to close? Y/N?\n");
	while (getchar() != 'Y');
	//exit:
	if (hDevice != INVALID_HANDLE_VALUE) {
		CloseHandle(hDevice);
	}
	return 0;
}

PCHAR
GetDevicePath(
IN  LPGUID InterfaceGuid
)
{
	HDEVINFO HardwareDeviceInfo;
	SP_DEVICE_INTERFACE_DATA DeviceInterfaceData;
	PSP_DEVICE_INTERFACE_DETAIL_DATA DeviceInterfaceDetailData = NULL;
	ULONG Length, RequiredLength = 0;
	BOOL bResult;

	HardwareDeviceInfo = SetupDiGetClassDevs(
		InterfaceGuid,
		NULL,
		NULL,
		(DIGCF_PRESENT | DIGCF_DEVICEINTERFACE));
	printf("SetupDiGetClasDevs Success!.\n");

	if (HardwareDeviceInfo == INVALID_HANDLE_VALUE) {
		printf("SetupDiGetClassDevs failed!\n");
		exit(1);
	}

	DeviceInterfaceData.cbSize = sizeof(SP_DEVICE_INTERFACE_DATA);

	bResult = SetupDiEnumDeviceInterfaces(HardwareDeviceInfo,
		0,
		InterfaceGuid,
		0,
		&DeviceInterfaceData);
	printf("SetupDiEnumDeviceInterfaces Success!.\n");

	if (bResult == FALSE) {
		printf("SetupDiEnumDeviceInterfaces failed.\n");

		SetupDiDestroyDeviceInfoList(HardwareDeviceInfo);
		exit(1);
	}

	SetupDiGetDeviceInterfaceDetail(
		HardwareDeviceInfo,
		&DeviceInterfaceData,
		NULL,
		0,
		&RequiredLength,
		NULL
		);

	DeviceInterfaceDetailData = (PSP_DEVICE_INTERFACE_DETAIL_DATA)LocalAlloc(LMEM_FIXED, RequiredLength);

	if (DeviceInterfaceDetailData == NULL) {
		SetupDiDestroyDeviceInfoList(HardwareDeviceInfo);
		printf("Failed to allocate memory.\n");
		exit(1);
	}

	DeviceInterfaceDetailData->cbSize = sizeof(SP_DEVICE_INTERFACE_DETAIL_DATA);

	Length = RequiredLength;

	bResult = SetupDiGetDeviceInterfaceDetail(
		HardwareDeviceInfo,
		&DeviceInterfaceData,
		DeviceInterfaceDetailData,
		Length,
		&RequiredLength,
		NULL);
	printf("SetupDiGetDeviceInterfaceDetail success!\n");

	if (bResult == FALSE) {
		printf("Error in SetupDiGetDeviceInterfaceDetail\n");

		SetupDiDestroyDeviceInfoList(HardwareDeviceInfo);
		LocalFree(DeviceInterfaceDetailData);
		exit(1);
	}
	printf("GetDevicePath Finished!\n");

	return DeviceInterfaceDetailData->DevicePath;

}
