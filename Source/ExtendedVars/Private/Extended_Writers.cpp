#include "Extended_BPLib.h"

bool UExtendedVarsBPLibrary::Write_File_To_Path(FString& Out_Code, TArray<uint8> In_Bytes, FString In_Path)
{
    if (In_Path.IsEmpty())
    {
        Out_Code = TEXT("Path is empty.");
        return false;
    }

    if (In_Bytes.IsEmpty() || !In_Bytes.GetData())
    {
        Out_Code = TEXT("Bytes are empty.");
        return false;
    }

    FPaths::MakeStandardFilename(In_Path);
    const FString PathExtension = FPaths::GetExtension(In_Path, false);

    if (PathExtension.IsEmpty())
    {
        FString HeaderExtension;

        // JPG
        if (UExtendedVarsBPLibrary::Bytes_x86_To_Hex(In_Bytes, 0, 1, false) == TEXT("ffd8") && UExtendedVarsBPLibrary::Bytes_x86_To_Hex(In_Bytes, (In_Bytes.Num() - 2), (In_Bytes.Num() - 1), false) == TEXT("ffd9"))
        {
            HeaderExtension = TEXT("jpg");
        }

        // BMP
        else if (UExtendedVarsBPLibrary::Bytes_x86_To_Hex(In_Bytes, 0, 1, false) == TEXT("424d"))
        {
            HeaderExtension = TEXT("bmp");
        }

        // PNG
        else if (UExtendedVarsBPLibrary::Bytes_x86_To_Hex(In_Bytes, 0, 7, false) == TEXT("89504e470d0a1a0a"))
        {
            HeaderExtension = TEXT("png");
        }

        // PDF
        else if (UExtendedVarsBPLibrary::Bytes_x86_To_Hex(In_Bytes, 0, 3, false) == TEXT("25504446"))
        {
            HeaderExtension = TEXT("pdf");
        }

        // ZIP
        else if (UExtendedVarsBPLibrary::Bytes_x86_To_Hex(In_Bytes, 0, 3, false) == TEXT("504b0304"))
        {
            HeaderExtension = TEXT("zip");
        }

        else
        {
            Out_Code = TEXT("Unknown file type.");
            return false;
        }

        In_Path += "." + HeaderExtension;
    }

    const bool bIsSuccess = FFileHelper::SaveArrayToFile(In_Bytes, *In_Path);

    Out_Code = bIsSuccess ? TEXT("File saved successfully.") : TEXT("Failed to save file.");
    return bIsSuccess;
}