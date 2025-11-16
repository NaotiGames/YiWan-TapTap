#pragma once

class TUDBDevice
{
public:
	static FString GetDeviceId5();
	static FString BytesToHexString(const uint8* Bytes, uint32 Length);
};
