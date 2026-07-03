#include "Extended_BPLib.h"

FString UExtendedVarsBPLibrary::Android_Path_Helper(FString In_FileName)
{
    if (UGameplayStatics::GetPlatformName() == "Android" || UGameplayStatics::GetPlatformName() == "IOS")
    {
        FString Path_Referance = FPlatformFileManager::Get().GetPlatformFile().ConvertToAbsolutePathForExternalAppForRead(*(FPaths::ProjectSavedDir()));

        TArray<FString> Path_Sections;
        Path_Referance.ParseIntoArray(Path_Sections, TEXT("/"), true);

        return "/" + Path_Sections[0] + "/" + Path_Sections[1] + "/" + Path_Sections[2] + "/" + In_FileName;
    }

    else
    {
        return "";
    }
}

bool UExtendedVarsBPLibrary::Get_Folder_Contents(TArray<FFolderContents>& OutContents, FString& ErrorCode, FString InPath)
{
    if (InPath.IsEmpty() == true)
    {
        ErrorCode = "Path is empty.";
        return false;
    }

    if (FPaths::DirectoryExists(InPath) == false)
    {
        ErrorCode = "Directory doesn't exist.";
        return false;
    }

    FFindDirectories GetFoldersVisitor;
    FPlatformFileManager::Get().GetPlatformFile().IterateDirectory(*InPath, GetFoldersVisitor);

    OutContents = GetFoldersVisitor.Array_Contents;

    return true;
}

void UExtendedVarsBPLibrary::Search_In_Folder(FDelegateFolderContents DelegateFolderContents, FString InPath, FString InSearch, bool bSearchExact)
{
    if (InPath.IsEmpty() == true)
    {
        return;
    }

    if (InSearch.IsEmpty() == true)
    {
        return;
    }

    if (FPaths::DirectoryExists(InPath) == false)
    {
        return;
    }

    AsyncTask(ENamedThreads::AnyNormalThreadNormalTask, [DelegateFolderContents, InPath, InSearch, bSearchExact]()
        {
            IFileManager& FileManager = FFileManagerGeneric::Get();

            TArray<FString> Array_Contents;
            TArray<FFolderContents> Array_Founds;

            FileManager.FindFilesRecursive(Array_Contents, *InPath, TEXT("*"), true, true, false);

            for (int32 ContentIndex = 0; ContentIndex < Array_Contents.Num(); ContentIndex++)
            {
                FFolderContents EachContent;

                if (bSearchExact == true)
                {
                    if (FPaths::GetBaseFilename(Array_Contents[ContentIndex]) == InSearch)
                    {
                        EachContent.Name = FPaths::GetCleanFilename(Array_Contents[ContentIndex]);
                        EachContent.Path = Array_Contents[ContentIndex];
                        EachContent.bIsFile = FPaths::FileExists(Array_Contents[ContentIndex]);

                        Array_Founds.Add(EachContent);
                    }
                }

                else
                {
                    if (FPaths::GetBaseFilename(Array_Contents[ContentIndex]).Contains(InSearch) == true)
                    {
                        EachContent.Name = FPaths::GetCleanFilename(Array_Contents[ContentIndex]);
                        EachContent.Path = Array_Contents[ContentIndex];
                        EachContent.bIsFile = FPaths::FileExists(Array_Contents[ContentIndex]);

                        Array_Founds.Add(EachContent);
                    }
                }
            }

            AsyncTask(ENamedThreads::GameThread, [DelegateFolderContents, Array_Founds]()
                {
                    DelegateFolderContents.ExecuteIfBound(true, "Success", Array_Founds);
                }
            );
        }
    );
}

bool UExtendedVarsBPLibrary::Read_File_From_Path_64(UBytesObject_64*& Out_Bytes_Object, FString In_Path, bool bUseLowLevel)
{
    if (In_Path.IsEmpty())
    {
        return false;
    }

    FPaths::NormalizeFilename(In_Path);
    FString Path = FPlatformFileManager::Get().GetPlatformFile().ConvertToAbsolutePathForExternalAppForRead(*In_Path);

    if (!FPaths::FileExists(Path))
    {
        return false;
    }

    TArray64<uint8> ByteArray;

    if (bUseLowLevel)
    {
        FPaths::MakePlatformFilename(Path);

        FILE* File = nullptr;

#ifdef _WIN64

        fopen_s(&File, TCHAR_TO_UTF8(*Path), "rb");

#else

        File = fopen(TCHAR_TO_UTF8(*Path), "rb");

#endif // _WIN64

        fseek(File, 0L, SEEK_END);
        const long FileSize = ftell(File);
        fseek(File, 0L, SEEK_SET);

        if (FileSize == 0)
        {
            fclose(File);
            return false;
        }

        uint8* Buffer = (uint8*)malloc(FileSize);
        fread(Buffer, sizeof(uint8), FileSize, File);

        ByteArray.SetNum(FileSize);
        FMemory::Memcpy(ByteArray.GetData(), Buffer, FileSize);

        fclose(File);
        free(Buffer);
        Buffer = nullptr;
    }

    else
    {
        FFileHelper::LoadFileToArray(ByteArray, *Path, FILEREAD_AllowWrite);

        if (ByteArray.IsEmpty())
        {
            return false;
        }
    }

    Out_Bytes_Object = NewObject<UBytesObject_64>();
    Out_Bytes_Object->ByteArray = ByteArray;

    return true;
}

bool UExtendedVarsBPLibrary::Read_File_From_Path_32(TArray<uint8>& Out_Bytes, FString In_Path, bool bUseLowLevel)
{
    if (In_Path.IsEmpty())
    {
        return false;
    }

    FPaths::NormalizeFilename(In_Path);
    FString Path = FPlatformFileManager::Get().GetPlatformFile().ConvertToAbsolutePathForExternalAppForRead(*In_Path);

    if (!FPaths::FileExists(Path))
    {
        return false;
    }

    TArray<uint8> ByteArray;

    if (bUseLowLevel)
    {
        FPaths::MakePlatformFilename(Path);

        FILE* File = nullptr;

#ifdef _WIN64

        fopen_s(&File, TCHAR_TO_UTF8(*Path), "rb");

#else

        File = fopen(TCHAR_TO_UTF8(*Path), "rb");

#endif // _WIN64

        fseek(File, 0L, SEEK_END);
        const long FileSize = ftell(File);
        fseek(File, 0L, SEEK_SET);

        if (FileSize == 0)
        {
            fclose(File);
            return false;
        }

        uint8* Buffer = (uint8*)FMemory::Malloc(FileSize);
        fread(Buffer, sizeof(uint8), FileSize, File);

        ByteArray.SetNum(FileSize);
        FMemory::Memcpy(ByteArray.GetData(), Buffer, FileSize);

        fclose(File);
        free(Buffer);
        Buffer = nullptr;

        Out_Bytes = ByteArray;
        return true;
    }

    else
    {
        FFileHelper::LoadFileToArray(Out_Bytes, *Path, FILEREAD_AllowWrite);
        return !Out_Bytes.IsEmpty();
    }
}

bool UExtendedVarsBPLibrary::Read_Text_From_Path(FString& Out_String, FString In_Path)
{
    if (In_Path.IsEmpty())
    {
        return false;
    }

    FPaths::MakeStandardFilename(In_Path);
    if (!FPaths::FileExists(In_Path))
    {

        return false;
    }

    return FFileHelper::LoadFileToString(Out_String, *In_Path);
}