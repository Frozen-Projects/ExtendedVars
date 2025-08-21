#include "Extended_Files.h"

FFindDirectories::FFindDirectories()
{

}

bool FFindDirectories::Visit(const TCHAR* CharPath, bool bIsDirectory)
{
	if (bIsDirectory == true)
	{
		FFolderContents EachContent;

		FString Path = FString(CharPath) + "/";
		FPaths::NormalizeDirectoryName(Path);

		EachContent.Path = Path;
		EachContent.Name = FPaths::GetBaseFilename(Path);
		EachContent.bIsFile = false;

		Array_Contents.Add(EachContent);
	}

	else if (bIsDirectory == false)
	{
		FFolderContents EachContent;

		EachContent.Path = CharPath;
		EachContent.Name = FPaths::GetCleanFilename(CharPath);
		EachContent.bIsFile = true;

		Array_Contents.Add(EachContent);
	}

	return true;
}