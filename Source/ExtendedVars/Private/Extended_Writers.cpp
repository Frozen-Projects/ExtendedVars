#include "Extended_BPLib.h"

bool UExtendedVarsBPLibrary::Write_File_To_Path(FString& Out_Code, TArray<uint8> In_Bytes, FString In_Path)
{
    if (In_Path.IsEmpty())
    {
        Out_Code = "Path is empty.";
        return false;
    }

    if (In_Bytes.IsEmpty() || !In_Bytes.GetData())
    {
        Out_Code = "Bytes are empty.";
        return false;
    }

    FPaths::MakeStandardFilename(In_Path);
    FString HeaderExtension;

    // JPG
    if (UExtendedVarsBPLibrary::Bytes_x86_To_Hex(In_Bytes, 0, 1, false) == "ffd8" && UExtendedVarsBPLibrary::Bytes_x86_To_Hex(In_Bytes, (In_Bytes.Num() - 2), (In_Bytes.Num() - 1), false) == "ffd9")
    {
        HeaderExtension = "jpg";
    }

    // BMP
    else if (UExtendedVarsBPLibrary::Bytes_x86_To_Hex(In_Bytes, 0, 1, false) == "424d")
    {
        HeaderExtension = "bmp";
    }

    // PNG
    else if (UExtendedVarsBPLibrary::Bytes_x86_To_Hex(In_Bytes, 0, 7, false) == "89504e470d0a1a0a")
    {
        HeaderExtension = "png";
    }

    // PDF
    else if (UExtendedVarsBPLibrary::Bytes_x86_To_Hex(In_Bytes, 0, 3, false) == "25504446")
    {
        HeaderExtension = "pdf";
    }

    const FString PathExtension = FPaths::GetExtension(In_Path, false);

    // If the path has an extension, we need to check if it matches the header extension.
    if (!PathExtension.IsEmpty() && PathExtension != HeaderExtension)
    {
        Out_Code = "Path extension does not match header extension.";
        return false;
    }

    In_Path += "." + HeaderExtension;
    FPaths::MakeStandardFilename(In_Path);
    bool bIsSuccess = FFileHelper::SaveArrayToFile(In_Bytes, *In_Path);

    Out_Code = bIsSuccess ? "File saved successfully." : "Failed to save file.";
    return bIsSuccess;
}