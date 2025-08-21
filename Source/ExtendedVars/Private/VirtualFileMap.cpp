#include "VirtualFileMap.h"

void UVirtualFileSubsystem::Initialize(FSubsystemCollectionBase& Collection)
{
	Super::Initialize(Collection);
}

void UVirtualFileSubsystem::Deinitialize()
{
	this->CleanUpFileHandles();
	Super::Deinitialize();
}

#ifdef _WIN64
std::string UVirtualFileSubsystem::GetErrorString(DWORD ErrorCode)
{
	char* msgBuffer = nullptr;
	size_t size = FormatMessageA(FORMAT_MESSAGE_ALLOCATE_BUFFER | FORMAT_MESSAGE_FROM_SYSTEM, nullptr, ErrorCode, 0, (LPSTR)&msgBuffer, 0, nullptr);

	std::string message(msgBuffer, size);
	LocalFree(msgBuffer);
	return message;
}
#endif // _WIN64

bool UVirtualFileSubsystem::FileAddCallback(FString& Out_Code, FString FileName, TMap<FString, FString> Headers, const TArray<uint8>& FileData)
{
#ifdef _WIN64

	FScopeLock Lock(&this->VFM_Guard);

	const wchar_t* FileNameChar = *FileName;
	const size_t BufferSize = FileData.Num();

	HANDLE TempHandle = CreateFileMappingW(INVALID_HANDLE_VALUE, nullptr, PAGE_READWRITE, 0, static_cast<DWORD>(BufferSize), FileNameChar);
	const DWORD LastError = GetLastError();

	if (!TempHandle)
	{
		FString ErrorTitle = "Failed to create file mapping at: " + FileName + " : ";

		std::stringstream ErrorStream;
		ErrorStream << TCHAR_TO_UTF8(*ErrorTitle) << this->GetErrorString(LastError);
		Out_Code = FString(ErrorStream.str().c_str());

		return false;
	}

	if (LastError == ERROR_ALREADY_EXISTS)
	{
		FString ErrorTitle = "There is already a virtual file with that name from another process: " + FileName + " : ";

		std::stringstream ErrorStream;
		ErrorStream << TCHAR_TO_UTF8(*ErrorTitle) << this->GetErrorString(LastError);
		Out_Code = FString(ErrorStream.str().c_str());

		CloseHandle(TempHandle);

		return false;
	}

	void* TempBuffer = MapViewOfFile(TempHandle, FILE_MAP_ALL_ACCESS, 0, 0, BufferSize);

	if (!TempBuffer)
	{
		FString ErrorTitle = "Failed to map view of file at: " + FileName + " : ";

		std::stringstream ErrorStream;
		ErrorStream << TCHAR_TO_UTF8(*ErrorTitle) << this->GetErrorString(GetLastError());
		Out_Code = FString(ErrorStream.str().c_str());

		CloseHandle(TempHandle);
		UnmapViewOfFile(TempBuffer);

		return false;
	}

	FMemory::Memcpy(TempBuffer, FileData.GetData(), BufferSize);

	FFileMapHandles FileStruct;
	FileStruct.MappedMemory = TempBuffer;
	FileStruct.MappedSize = BufferSize;
	FileStruct.MappingHandle = TempHandle;

	for (const TPair<FString, FString>& Header : Headers)
	{
		FileStruct.Header.JsonObject->SetStringField(Header.Key, Header.Value);
	}

	this->VirtualFileMaps.Add(FileName, FileStruct);

	Out_Code = TEXT("File added successfully.");
	return true;

#else

	return false;

#endif // _WIN64
}

void UVirtualFileSubsystem::FileRemoveCallback(FString FileName)
{
#ifdef _WIN64
	FScopeLock Lock(&this->VFM_Guard);

	if (FFileMapHandles* FileHandle = this->VirtualFileMaps.Find(FileName))
	{
		if (FileHandle->MappedMemory)
		{
			UnmapViewOfFile(FileHandle->MappedMemory);
			FileHandle->MappedMemory = nullptr;
		}

		if (FileHandle->MappingHandle)
		{
			CloseHandle(FileHandle->MappingHandle);
			FileHandle->MappingHandle = nullptr;
		}

		FileHandle->Header.JsonObject.Reset();
		FileHandle->MappedSize = 0;

		this->VirtualFileMaps.Remove(FileName);
	}
#else
	return;
#endif // _WIN64
}

void UVirtualFileSubsystem::CleanUpFileHandles()
{
#ifdef _WIN64
	FScopeLock Lock(&this->VFM_Guard);

	for (TPair<FString, FFileMapHandles>& Pair : this->VirtualFileMaps)
	{
		FFileMapHandles& FileHandle = Pair.Value;

		if (FileHandle.MappedMemory)
		{
			UnmapViewOfFile(FileHandle.MappedMemory);
			FileHandle.MappedMemory = nullptr;
		}

		if (FileHandle.MappingHandle)
		{
			CloseHandle(FileHandle.MappingHandle);
			FileHandle.MappingHandle = nullptr;
		}

		FileHandle.Header.JsonObject.Reset();
		FileHandle.MappedSize = 0;
	}

	this->VirtualFileMaps.Empty();
#else
	return;
#endif // _WIN64
}

bool UVirtualFileSubsystem::AddFile(FString&Out_Code, FString FileName, TMap<FString, FString> Headers, const TArray<uint8>& FileData, bool bAllowUpdate)
{
#ifdef _WIN64
	if (FileName.IsEmpty())
	{
		Out_Code = TEXT("FileName is empty");
		return false;
	}

	if (FileData.IsEmpty() || !FileData.GetData())
	{
		Out_Code = TEXT("FileData is empty or invalid");
		return false;
	}

	if (!this->VirtualFileMaps.Contains(FileName))
	{
		return this->FileAddCallback(Out_Code, FileName, Headers, FileData);
	}

	else if (bAllowUpdate)
	{
		this->FileRemoveCallback(FileName);
		return this->FileAddCallback(Out_Code, FileName, Headers, FileData);
	}

	Out_Code = "File already exists and update is not allowed.";
	return false;
#else
	return false;
#endif // _WIN64
}

bool UVirtualFileSubsystem::RemoveFile(FString& Out_Code, FString FileName)
{
#ifdef _WIN64
	if (FileName.IsEmpty())
	{
		Out_Code = TEXT("FileName is empty");
		return false;
	}

	if (this->VirtualFileMaps.IsEmpty())
	{
		Out_Code = TEXT("No files in the virtual file map.");
		return false;
	}

	if (!this->VirtualFileMaps.Contains(FileName))
	{
		Out_Code = TEXT("File does not exist in the virtual file map.");
		return false;
	}

	this->FileRemoveCallback(FileName);

	Out_Code = TEXT("File removed successfully.");
	return true;
#else
	return false;
#endif // _WIN64
}

bool UVirtualFileSubsystem::GetFile(TArray<uint8>& Out_Buffer, FString& Out_Code, FString FileName)
{
#ifdef _WIN64
	if (FileName.IsEmpty())
	{
		Out_Code = TEXT("FileName is empty");
		return false;
	}

	if (this->VirtualFileMaps.IsEmpty())
	{
		Out_Code = TEXT("No files in the virtual file map.");
		return false;
	}

	if (!this->VirtualFileMaps.Contains(FileName))
	{
		Out_Code = TEXT("File does not exist in the virtual file map.");
		return false;
	}

	FFileMapHandles& FileHandle = this->VirtualFileMaps[FileName];

	if (FileHandle.MappedMemory && FileHandle.MappedSize > 0)
	{
		TArray<uint8> TempBuffer;
		TempBuffer.SetNumUninitialized(FileHandle.MappedSize);
		FMemory::Memcpy(TempBuffer.GetData(), FileHandle.MappedMemory, FileHandle.MappedSize);
		
		Out_Buffer.Empty();
		Out_Buffer = TempBuffer;

		Out_Code = TEXT("File retrieved successfully.");
		return true;
	}

	Out_Code = TEXT("File is not mapped or has no data.");
	return false;
#else
	return false;
#endif // _WIN64
}

bool UVirtualFileSubsystem::GetFileHeader(TMap<FString, FString>& Out_Headers, FString& Out_Code, FString FileName)
{
#ifdef _WIN64
	if (FileName.IsEmpty())
	{
		Out_Code = TEXT("FileName is empty");
		return false;
	}

	if (this->VirtualFileMaps.IsEmpty())
	{
		Out_Code = TEXT("No files in the virtual file map.");
		return false;
	}

	if (!this->VirtualFileMaps.Contains(FileName))
	{
		Out_Code = TEXT("File does not exist in the virtual file map.");
		return false;
	}

	FFileMapHandles& FileHandle = this->VirtualFileMaps[FileName];

	if (FileHandle.Header.JsonObject.IsValid())
	{
		TMap<FString, FString> TempHeaders;

		for (const TPair<FString, TSharedPtr<FJsonValue>>& Header : FileHandle.Header.JsonObject->Values)
		{
			const FString Key = Header.Key;
			const FString Value = Header.Value->AsString();
			TempHeaders.Add(Key, Value);
		}

		Out_Headers = TempHeaders;
		Out_Code = TEXT("File header retrieved successfully.");
		return true;
	}

	return false;
#else
	return false;
#endif // _WIN64
}

bool UVirtualFileSubsystem::GetFileNames(TArray<FString>& Out_Names, FString& Out_Code)
{
#ifdef _WIN64
	if (this->VirtualFileMaps.IsEmpty())
	{
		return false;
	}

	TArray<FString> TempNames;
	this->VirtualFileMaps.GetKeys(TempNames);

	Out_Names.Empty();
	Out_Names = TempNames;

	return true;
#else
	return false;
#endif // _WIN64
}