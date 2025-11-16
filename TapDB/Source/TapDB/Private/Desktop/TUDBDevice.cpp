#include "TUDBDevice.h"
#include "Misc/Guid.h"
#include "HAL/PlatformProcess.h"
#include "Misc/DateTime.h"

#if PLATFORM_WINDOWS
#include "Windows/AllowWindowsPlatformTypes.h"
#include <windows.h>
#include <processthreadsapi.h>
#include "Windows/HideWindowsPlatformTypes.h"
#endif

FString TUDBDevice::GetDeviceId5()
{
#if PLATFORM_WINDOWS
	uint64 StartTime = 0;
	DWORD ProcessId = GetCurrentProcessId();

	FILETIME ftCreation, ftExit, ftKernel, ftUser;
	if (GetProcessTimes(GetCurrentProcess(), &ftCreation, &ftExit, &ftKernel, &ftUser))
	{
		StartTime = (static_cast<uint64>(ftCreation.dwHighDateTime) << 32) | ftCreation.dwLowDateTime;
	}

	// 拼接字符串：启动时间 + PID
	FString TmpStr = FString::Printf(TEXT("%llu-%lu"), StartTime, ProcessId);

	// 计算 MD5（使用 UE 内置）
	FMD5 Md5Gen;
	FTCHARToUTF8 Utf8(*TmpStr);
	Md5Gen.Update(reinterpret_cast<const uint8*>(Utf8.Get()), Utf8.Length());
	uint8 Digest[16];
	Md5Gen.Final(Digest);

	FString Md5Hex = BytesToHexString(Digest, 16);

	return Md5Hex;
#else
	return "";
#endif
}

// 转 16 进制字符串（等价于 bin_to_strhex）
FString TUDBDevice::BytesToHexString(const uint8* Bytes, uint32 Length)
{
	static const TCHAR* Hex = TEXT("0123456789abcdef");
	FString Out;
	Out.Reserve(Length * 2);

	for (uint32 i = 0; i < Length; i++)
	{
		uint8 Byte = Bytes[i];
		Out.AppendChar(Hex[(Byte >> 4) & 0xF]);
		Out.AppendChar(Hex[Byte & 0xF]);
	}
	return Out;
}