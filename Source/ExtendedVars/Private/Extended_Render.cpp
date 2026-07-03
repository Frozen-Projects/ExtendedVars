#include "Extended_BPLib.h"

EGammaSpace UExtendedVarsBPLibrary::ConvertGammaSpace(EGammaSpaceBp InGammaSpaceBp)
{
    switch (InGammaSpaceBp)
    {
        case EGammaSpaceBp::Pow22:
            return EGammaSpace::Pow22;

        case EGammaSpaceBp::Linear:
            return EGammaSpace::Linear;

        case EGammaSpaceBp::sRGB:
            return EGammaSpace::sRGB;

        default:
            return EGammaSpace::Invalid;
    }
}

bool UExtendedVarsBPLibrary::Encode_New_Internal(TArray<uint8>& Out_Buffer, FString& Out_Code, TArray<uint8> Texture_Data, FVector2D ImageRes, EImageExtensions CompressFormat, EGammaSpaceBp GammaSpaceBp)
{
    const FString FunctionName = TEXT("__FUNCTION__");
    const size_t Lenght = Texture_Data.Num();

    FImageView ImageView;
    ImageView.Format = ERawImageFormat::BGRA8;
    ImageView.SizeX = ImageRes.X;
    ImageView.SizeY = ImageRes.Y;
    ImageView.GammaSpace = UExtendedVarsBPLibrary::ConvertGammaSpace(GammaSpaceBp);
    ImageView.NumSlices = 1;
    ImageView.RawData = malloc(Lenght);
    FMemory::Memcpy(ImageView.RawData, Texture_Data.GetData(), Lenght);

    FString FormatString = "";
    EImageFormat ImageFormat = EImageFormat::Invalid;

    switch (CompressFormat)
    {
        case EImageExtensions::Ext_None:
        
            Out_Code = FString::Printf(TEXT("%s : Extension is not defined. Please define it."), *FunctionName);
            return false;

        case EImageExtensions::Ext_BMP:

            FormatString = "BMP";
            ImageFormat = EImageFormat::BMP;
            break;

        case EImageExtensions::EXT_JPEG:

            FormatString = "JPG";
            ImageFormat = EImageFormat::JPEG;
            break;

        case EImageExtensions::EXT_PNG:

            FormatString = "PNG";
            ImageFormat = EImageFormat::PNG;
            break;

        default:

            FormatString = "PNG";
            ImageFormat = EImageFormat::PNG;
            break;
    }

    IImageWrapperModule& ImageWrapperModule = FModuleManager::LoadModuleChecked<IImageWrapperModule>(FName("ImageWrapper"));

    TArray64<uint8> Compressed_64;
    if (!ImageWrapperModule.CompressImage(Compressed_64, ImageFormat, ImageView, (int32)EImageCompressionQuality::Max))
    {
		Out_Code = FString::Printf(TEXT("%s : ImageWrapperModule.CompressImage failed."), *FunctionName);
        return false;
    }

    if (Compressed_64.IsEmpty())
    {
		Out_Code = FString::Printf(TEXT("%s : There is no compressed bytes."), *FunctionName);
        return false;
    }

    const size_t Compressed_64_Size = Compressed_64.Num();

    if (Compressed_64_Size > TNumericLimits<int32>::Max())
    {
		Out_Code = FString::Printf(TEXT("%s : Compressed image size exceeds TArray<uint8> limits for this extension : %s"), *FunctionName, *FormatString);
        return false;
    }

    TArray<uint8> Compressed_86;
    Compressed_86.SetNumUninitialized(static_cast<int32>(Compressed_64_Size));
    FMemory::Memcpy(Compressed_86.GetData(), Compressed_64.GetData(), Compressed_64_Size);

    if (!Compressed_86.GetData())
    {
		Out_Code = FString::Printf(TEXT("%s : Compressed image data is empty for this extension : %s"), *FunctionName, *FormatString);
        return false;
    }

	Out_Buffer = Compressed_86;
	Out_Code = FString::Printf(TEXT("%s : %s compression is successfull for target texture with new api."), *FunctionName, *FormatString);
    
    return true;
}

bool UExtendedVarsBPLibrary::Encode_Old_Internal(TArray<uint8>& Out_Buffer, FString& Out_Code, TArray<uint8> Texture_Data, FVector2D ImageRes, EImageExtensions CompressFormat)
{
    const FString FunctionName = TEXT("__FUNCTION__");

    bool bUseRgba = false;
    FString FormatString = "";
    EImageFormat ImageFormat = EImageFormat::Invalid;

    switch (CompressFormat)
    {
        case EImageExtensions::Ext_None:
            Out_Code = FString::Printf(TEXT("%s : Extension is not defined. Please define it."), *FunctionName);
            return false;

        case EImageExtensions::Ext_BMP:

            bUseRgba = false;
            FormatString = TEXT("BMP");
            ImageFormat = EImageFormat::BMP;
            break;

        case EImageExtensions::EXT_JPEG:

            bUseRgba = true;
            FormatString = TEXT("JPG");
            ImageFormat = EImageFormat::JPEG;
            break;

        case EImageExtensions::EXT_PNG:

            bUseRgba = true;
            FormatString = TEXT("PNG");
            ImageFormat = EImageFormat::PNG;
            break;

        default:

            bUseRgba = true;
            FormatString = TEXT("PNG");
            ImageFormat = EImageFormat::PNG;
            break;
    }

    IImageWrapperModule& ImageWrapperModule = FModuleManager::LoadModuleChecked<IImageWrapperModule>(FName("ImageWrapper"));
    TSharedPtr<IImageWrapper> ImageWrapper = ImageWrapperModule.CreateImageWrapper(ImageFormat);

    if (!ImageWrapper.IsValid())
    {
        Out_Code = FString::Printf(TEXT("%s : Function couldn't initialize ImageWrapper."), *FunctionName);
        return false;
    }

    TArray<FColor> Array_Colors_RGBA;
    const size_t Lenght = Texture_Data.Num();

    if (bUseRgba)
    {
        const size_t Total = static_cast<size_t>(ImageRes.X * ImageRes.Y);

        TArray<FColor> Array_Colors_BGRA;
        Array_Colors_BGRA.SetNum(Total);
        FMemory::Memcpy(Array_Colors_BGRA.GetData(), Texture_Data.GetData(), Lenght);

        // Convert BGRA to RGBA
        Array_Colors_RGBA = Array_Colors_BGRA;
        for (int32 i = 0; i < Total; i++)
        {
            Array_Colors_RGBA[i].R = Array_Colors_BGRA[i].B;
            Array_Colors_RGBA[i].B = Array_Colors_BGRA[i].R;
        }
    }

    void* RawData = bUseRgba ? (void*)Array_Colors_RGBA.GetData() : (void*)Texture_Data.GetData();

    if (!ImageWrapper->SetRaw(RawData, Lenght, ImageRes.X, ImageRes.Y, bUseRgba ? ERGBFormat::RGBA : ERGBFormat::BGRA, 8))
    {
        Out_Code = FString::Printf(TEXT("%s : ImageWrapper->SetRaw failed."), *FunctionName);
        return false;
    }

    TArray64<uint8> Compressed_64 = ImageWrapper->GetCompressed(100);
    const size_t Compressed_64_Size = Compressed_64.Num();

    if (Compressed_64_Size <= 0)
    {
        Out_Code = FString::Printf(TEXT("%s : There is no compressed bytes."), *FunctionName);
        return false;
    }

    if (Compressed_64_Size > TNumericLimits<int32>::Max())
    {
        Out_Code = FString::Printf(TEXT("%s : Compressed image size exceeds TArray<uint8> limits for this extension : %s"), *FunctionName, *FormatString);
        return false;
    }

    TArray<uint8> Compressed_86;
    Compressed_86.SetNumUninitialized(static_cast<int32>(Compressed_64_Size));
    FMemory::Memcpy(Compressed_86.GetData(), Compressed_64.GetData(), Compressed_64_Size);

    if (!Compressed_86.GetData())
    {
        Out_Code = FString::Printf(TEXT("%s : Compressed image data is empty for this extension : %s"), *FunctionName, *FormatString);
        return false;
    }

    Out_Buffer = Compressed_86;
    Out_Code = FString::Printf(TEXT("%s : %s compression is successfull for target texture with new api."), *FunctionName, *FormatString);
    
    return true;
}

bool UExtendedVarsBPLibrary::GetTextureBuffer_Internal(TArray<uint8>& Out_Bytes, FString& Out_Code, UTexture* In_Texture, FRHICommandListImmediate* RHICmdList)
{
    if (!IsValid(In_Texture))
    {
        Out_Code = TEXT("Texture is not valid !");
        return false;
    }

    if (!RHICmdList)
    {
		Out_Code = TEXT("RHICmdList is not valid !");
        return false;
    }

    EPixelFormat PixelFormat = PF_Unknown;

    FTextureResource* TextureResource = In_Texture->GetResource();
    FTextureRHIRef TargetTextureRHI = nullptr;

    if (UTexture2D* T2D = Cast<UTexture2D>(In_Texture))
    {
        if (T2D->GetPixelFormat() != EPixelFormat::PF_B8G8R8A8)
        {
			Out_Code = TEXT("Texture2D pixel format is not PF_B8G8R8A8. Please use PF_B8G8R8A8 format.");
            return false;
        }

        if (T2D->CompressionSettings.GetIntValue() != 5 && T2D->CompressionSettings.GetIntValue() != 7)
        {
            Out_Code = TEXT("Texture compression settings are not correct !");
            return false;
        }

        PixelFormat = PF_B8G8R8A8;
		TargetTextureRHI = TextureResource->GetTexture2DRHI();
    }

    if (UTextureRenderTarget2D* TRT_2D = Cast<UTextureRenderTarget2D>(In_Texture))
    {
        if (TRT_2D->RenderTargetFormat.GetValue() != ETextureRenderTargetFormat::RTF_RGBA8 && TRT_2D->RenderTargetFormat.GetValue() != ETextureRenderTargetFormat::RTF_RGBA8_SRGB)
        {
            Out_Code = TEXT("Target TextureRenderTarget2D format is not RTF_RGBA8 or RTF_RGBA_SRGB.");
            return false;
        }

        PixelFormat = PF_B8G8R8A8;
        TargetTextureRHI = TextureResource->GetTextureRHI();
    }

    if (Cast<UMediaTexture>(In_Texture))
    {
        PixelFormat = PF_R8G8B8A8;
        TargetTextureRHI = TextureResource->GetTextureRHI();
    }

    const EGammaSpace TargetGammaSpace = In_Texture->GetGammaSpace();
    const int32 Size_X = TextureResource->GetSizeX();
    const int32 Size_Y = TextureResource->GetSizeY();

    // Create Copy Texture Descriptions
    FRHITextureCreateDesc Desc = FRHITextureCreateDesc::Create2D(TEXT("CopiedTextureDescription"))
        .SetExtent(Size_X, Size_Y)
        .SetFormat(PixelFormat)
        .SetInitialState(ERHIAccess::CopySrc);

    FTextureRHIRef CopiedTextureRHI = RHICmdList->CreateTexture(Desc);

    FRHICopyTextureInfo CopyTextureInfo;
    RHICmdList->CopyTexture(TargetTextureRHI, CopiedTextureRHI, CopyTextureInfo);

    TArray<FColor> SurfaceColor;
    FReadSurfaceDataFlags Flags(RCM_UNorm, CubeFace_MAX);
    RHICmdList->ReadSurfaceData(CopiedTextureRHI, FIntRect(0, 0, Size_X, Size_Y), SurfaceColor, Flags);

    if (!SurfaceColor.GetData())
    {
		Out_Code = TEXT("Unable to read surface data from texture.");
        return false;
    }

    TArray<uint8> BufferArray;
    const size_t BufferSize = SurfaceColor.Num() * sizeof(FColor);
    BufferArray.SetNumUninitialized(BufferSize);
    FMemory::Memcpy(BufferArray.GetData(), (uint8*)SurfaceColor.GetData(), BufferSize);

	Out_Bytes = BufferArray;
    return true;
}

UTextureRenderTarget2D* UExtendedVarsBPLibrary::Widget_To_TRT2D(FString& OutCode, UUserWidget* InWidget, FVector2D InDrawSize)
{
    if (IsValid(InWidget) == false)
    {
        OutCode = "Source widget is not valid";
        return nullptr;
    }

    UTextureRenderTarget2D* TextureTarget = FWidgetRenderer::CreateTargetFor(InDrawSize, TextureFilter::TF_Default, false);
    TextureTarget->RenderTargetFormat = RTF_RGBA8;

    FWidgetRenderer* WidgetRenderer = new FWidgetRenderer(true);
    WidgetRenderer->DrawWidget(TextureTarget, InWidget->TakeWidget(), InDrawSize, 0, false);

    if (IsValid(TextureTarget) == true)
    {
        return TextureTarget;
    }

    else
    {
        OutCode = "Unable to create Texture Render Target 2D";
        return nullptr;
    }
}

EGammaSpaceBp UExtendedVarsBPLibrary::GetGammaSpace(UTexture* In_Texture)
{
	if (!IsValid(In_Texture))
	{
		return EGammaSpaceBp::Invalid;
	}

	const EGammaSpace InGammaSpace = In_Texture->GetGammaSpace();

    switch (InGammaSpace)
    {
        case EGammaSpace::Pow22:
            return EGammaSpaceBp::Pow22;

        case EGammaSpace::Linear:
            return EGammaSpaceBp::Linear;

        case EGammaSpace::sRGB:
            return EGammaSpaceBp::sRGB;

        default:
            return EGammaSpaceBp::Invalid;
    }
}

FVector2D UExtendedVarsBPLibrary::GetTextureSize(UTexture* In_Texture)
{
    if (!IsValid(In_Texture))
    {
        return FVector2D::Zero();
    }

	FTextureResource* TextureResource = In_Texture->GetResource();
	
    if (!TextureResource)
	{
		return FVector2D::Zero();
	}

	const int32 Size_X = TextureResource->GetSizeX();
	const int32 Size_Y = TextureResource->GetSizeY();
	
    return FVector2D(Size_X, Size_Y);
}

void UExtendedVarsBPLibrary::TRT2D_To_T2D(FDelegateTexture2D Delegate_T2D, UTextureRenderTarget2D* In_TRT_2D)
{
    if (!IsValid(In_TRT_2D))
    {
        AsyncTask(ENamedThreads::GameThread, [Delegate_T2D]()
            {
                Delegate_T2D.ExecuteIfBound(false, "In_TRT_2D is not valid.", nullptr);
            }
        );

        return;
    }

    TEnumAsByte<ETextureRenderTargetFormat> RT_Format = In_TRT_2D->RenderTargetFormat;

    if (RT_Format != RTF_RGBA8 || RT_Format != RTF_RGBA8_SRGB)
    {
        if (!IsValid(In_TRT_2D))
        {
            AsyncTask(ENamedThreads::GameThread, [Delegate_T2D]()
                {
                    Delegate_T2D.ExecuteIfBound(false, "Render Target format is not correct. Please use RTF_RGBA8 or RTF_RGBA8_SRGB.", nullptr);
                }
            );

            return;
        }
    }

    ENQUEUE_RENDER_COMMAND(Convert_TRT2D_To_T2D)([Delegate_T2D, In_TRT_2D, RT_Format](FRHICommandListImmediate& RHICmdList)
        {
            // Get Texture Render Target 2D Properties.
            const int32 Size_X = In_TRT_2D->SizeX;
            const int32 Size_Y = In_TRT_2D->SizeY;
			const size_t Lenght = Size_X * Size_Y * sizeof(FColor);

            // Create Copy Descriptions.
            FRHITextureCreateDesc Desc =
                FRHITextureCreateDesc::Create2D(TEXT("CopiedTextureDescription"))
                .SetExtent(Size_X, Size_Y)
                .SetFormat(PF_B8G8R8A8)
                .SetInitialState(ERHIAccess::CopySrc);

            // Copy Texture Render Target 2D.
            FTextureRHIRef CopiedTextureResource = RHICmdList.CreateTexture(Desc);
            FRHICopyTextureInfo CopyTextureInfo;
            RHICmdList.CopyTexture(In_TRT_2D->GetResource()->GetTextureRHI(), CopiedTextureResource, CopyTextureInfo);

            FRHILockTextureArgs LockArgs = FRHILockTextureArgs::Lock2D(CopiedTextureResource, 0, EResourceLockMode::RLM_ReadOnly, false, true);
            FRHILockTextureResult LockResult = RHICmdList.LockTexture(LockArgs);
			void* TextureData = LockResult.Data;
            RHICmdList.UnlockTexture(LockArgs);

            AsyncTask(ENamedThreads::GameThread, [Delegate_T2D, Size_X, Size_Y, Lenght, TextureData, RT_Format]()
                {
                    // Create Texture
                    UTexture2D* Texture = UTexture2D::CreateTransient(Size_X, Size_Y, PF_B8G8R8A8);
                    Texture->NeverStream = true;
                    Texture->SRGB = RT_Format == RTF_RGBA8_SRGB ? true : false;

                    FTexture2DMipMap& Texture2D_Mip = Texture->GetPlatformData()->Mips[0];
                    void* Texture2D_Data = Texture2D_Mip.BulkData.Lock(LOCK_READ_WRITE);
                    FMemory::Memcpy(Texture2D_Data, TextureData, Lenght);
                    Texture2D_Mip.BulkData.Unlock();
                    Texture->UpdateResource();
                    
                    Delegate_T2D.ExecuteIfBound(true, "Sucessfull.", Texture);
                }
            );

            return;
        }
    );
}

bool UExtendedVarsBPLibrary::Export_Texture_File(FString& Out_Path, TArray<uint8> ImageBuffer, FVector2D ImageRes, FString In_Path, EGammaSpaceBp GammaSpaceBp, EImageExtensions Extension)
{
    if (ImageBuffer.IsEmpty())
    {
        return false;
    }

    if (ImageRes.X <= 0 || ImageRes.Y <= 0)
    {
        return false;
    }

    if (In_Path.IsEmpty())
    {
        return false;
    }

    FPaths::MakeStandardFilename(In_Path);

    switch (Extension)
    {
        case EImageExtensions::Ext_None:

            return false;

        case EImageExtensions::Ext_BMP:
        {
            Out_Path = In_Path + ".bmp";

            TArray<FColor> Array_Colors;
            Array_Colors.SetNum(ImageRes.X * ImageRes.Y);
            FMemory::Memcpy(Array_Colors.GetData(), reinterpret_cast<FColor*>(ImageBuffer.GetData()), ImageBuffer.Num());

            return FFileHelper::CreateBitmap(*Out_Path, ImageRes.X, ImageRes.Y, Array_Colors.GetData(), NULL, &IFileManager::Get(), NULL, true);
        }

        case EImageExtensions::EXT_JPEG:
        {
            FImageView ImageView;
            ImageView.Format = ERawImageFormat::BGRA8;
            ImageView.SizeX = ImageRes.X;
            ImageView.SizeY = ImageRes.Y;
            ImageView.GammaSpace = UExtendedVarsBPLibrary::ConvertGammaSpace(GammaSpaceBp);
            ImageView.NumSlices = 1;
            ImageView.RawData = malloc(ImageBuffer.Num());
            FMemory::Memcpy(ImageView.RawData, (uint8*)ImageBuffer.GetData(), ImageBuffer.Num());

            Out_Path = In_Path + ".jpg";
            return FImageUtils::SaveImageByExtension(*Out_Path, ImageView, 0);
        }

        case EImageExtensions::EXT_PNG:
        {
            FImageView ImageView;
            ImageView.Format = ERawImageFormat::BGRA8;
            ImageView.SizeX = ImageRes.X;
            ImageView.SizeY = ImageRes.Y;
            ImageView.GammaSpace = UExtendedVarsBPLibrary::ConvertGammaSpace(GammaSpaceBp);
            ImageView.NumSlices = 1;
            ImageView.RawData = malloc(ImageBuffer.Num());
            FMemory::Memcpy(ImageView.RawData, (uint8*)ImageBuffer.GetData(), ImageBuffer.Num());

            Out_Path = In_Path + ".png";
            return FImageUtils::SaveImageByExtension(*Out_Path, ImageView, 0);
        }
       
        default:
            return false;
    }
}

bool UExtendedVarsBPLibrary::Export_T2D_Colors(TArray<FColor>& Out_Array, FString& Out_Code, UTexture2D* Texture)
{
    if (!IsValid(Texture))
    {
        Out_Code = TEXT("Texture not valid.");
        return false;
    }

    if (Texture->GetPixelFormat() != EPixelFormat::PF_B8G8R8A8)
    {
		Out_Code = TEXT("Texture pixel format should be PF_B8G8R8A8.");
        return false;
    }

    if (Texture->LODGroup != TextureGroup::TEXTUREGROUP_UI)
    {
		Out_Code = TEXT("Texture LOD group should be UI.");
        return false;
    }

    TextureCompressionSettings OldCompressionSettings = Texture->CompressionSettings;
    Texture->CompressionSettings = TextureCompressionSettings::TC_VectorDisplacementmap;
    Texture->UpdateResource();

    int32 Texture_Width = Texture->GetSizeX();
    int32 Texture_Height = Texture->GetSizeY();
    size_t Lenght = static_cast<size_t>(Texture_Width * Texture_Height * sizeof(FColor));

    FTexture2DMipMap& Texture_Mip = Texture->GetPlatformData()->Mips[0];
    void* Texture_Data = Texture_Mip.BulkData.Lock(LOCK_READ_WRITE);

    if (!Texture_Data)
    {
        return false;
    }

    Out_Array.SetNum(Texture_Width * Texture_Height);
    FMemory::Memcpy(Out_Array.GetData(), static_cast<FColor*>(Texture_Data), Lenght);

    Texture_Mip.BulkData.Unlock();

    Texture->CompressionSettings = OldCompressionSettings;
    Texture->UpdateResource();

    return true;
}

bool UExtendedVarsBPLibrary::Export_T2D_Bytes(TArray<uint8>& Out_Array, FString& Out_Code, UTexture2D* Texture)
{
    if (!IsValid(Texture))
    {
        Out_Code = TEXT("Texture not valid.");
        return false;
    }

    if (Texture->GetPixelFormat() != EPixelFormat::PF_B8G8R8A8)
    {
        Out_Code = TEXT("Texture pixel format should be PF_B8G8R8A8.");
        return false;
    }

    if (Texture->LODGroup != TextureGroup::TEXTUREGROUP_UI)
    {
        Out_Code = TEXT("Texture LOD group should be UI.");
        return false;
    }

    TextureCompressionSettings OldCompressionSettings = Texture->CompressionSettings;
    Texture->CompressionSettings = TextureCompressionSettings::TC_VectorDisplacementmap;
    Texture->UpdateResource();

    int32 Texture_Width = Texture->GetSizeX();
    int32 Texture_Height = Texture->GetSizeY();
    size_t Lenght = static_cast<size_t>(Texture_Width * Texture_Height * sizeof(FColor));

    FTexture2DMipMap& Texture_Mip = Texture->GetPlatformData()->Mips[0];
    const void* Texture_Data = Texture_Mip.BulkData.LockReadOnly();

    if (!Texture_Data)
    {
        Texture_Mip.BulkData.Unlock();

        Out_Code = "Texture buffer is not valid.";
        return false;
    }

    Out_Array.SetNum(Lenght);
    FMemory::Memcpy(Out_Array.GetData(), (uint8*)Texture_Data, Lenght);

    Texture_Mip.BulkData.Unlock();

    Texture->CompressionSettings = OldCompressionSettings;
    Texture->UpdateResource();

    return true;
}

void UExtendedVarsBPLibrary::Export_Texture_Bytes_RT(FDelegateImageBuffer DelegateImageBuffer, UTexture* TargetTexture)
{    
    ENQUEUE_RENDER_COMMAND(Cmd_Export_Texture)([TargetTexture, DelegateImageBuffer](FRHICommandListImmediate& RHICmdList)
        {
			TArray<uint8> Out_Bytes;
            FString Out_Code;
			UExtendedVarsBPLibrary::GetTextureBuffer_Internal(Out_Bytes, Out_Code, TargetTexture, &RHICmdList);

            AsyncTask(ENamedThreads::GameThread, [DelegateImageBuffer, Out_Bytes]()
                {
                    DelegateImageBuffer.ExecuteIfBound(true, "Export texture as raw buffer is successful.", Out_Bytes);
                }
            );
        }
    );
}

void UExtendedVarsBPLibrary::EncodeTexture(FDelegateImageBuffer DelegateImageBuffer, TArray<uint8> Texture_Data, FVector2D ImageRes, EImageExtensions CompressFormat, EGammaSpaceBp GammaSpaceBp, bool bUseOldApi)
{
    ENQUEUE_RENDER_COMMAND(Cmd_Export_Texture)([DelegateImageBuffer, Texture_Data, ImageRes, CompressFormat, GammaSpaceBp, bUseOldApi](FRHICommandListImmediate& RHICmdList)
        {
            TArray<uint8> Out_Buffer;
            FString Out_Code;
			bool bIsSuccess = bUseOldApi ? 
                UExtendedVarsBPLibrary::Encode_Old_Internal(Out_Buffer, Out_Code, Texture_Data, ImageRes, CompressFormat) : 
                UExtendedVarsBPLibrary::Encode_New_Internal(Out_Buffer, Out_Code, Texture_Data, ImageRes, CompressFormat, GammaSpaceBp);

			AsyncTask(ENamedThreads::GameThread, [DelegateImageBuffer, bIsSuccess, Out_Code, Out_Buffer]()
				{
					DelegateImageBuffer.ExecuteIfBound(bIsSuccess, Out_Code, Out_Buffer);
				}
			);
        }
    );
}

void UExtendedVarsBPLibrary::DecodeTexture(FDelegateImageBuffer DelegateImageBuffer, TArray<uint8> In_Bytes)
{
    if (In_Bytes.IsEmpty())
    {
        DelegateImageBuffer.ExecuteIfBound(false, "There is no byte to decompres.", TArray<uint8>());
        return;
    }

    ENQUEUE_RENDER_COMMAND(Cmd_Decompress_Image)([DelegateImageBuffer, In_Bytes](FRHICommandList& RHICmdList)
        {
            IImageWrapperModule& ImageWrapperModule = FModuleManager::LoadModuleChecked<IImageWrapperModule>(FName("ImageWrapper"));
            
            FImage DecompressedImage;
            ImageWrapperModule.DecompressImage(In_Bytes.GetData(), In_Bytes.Num(), DecompressedImage);

            size_t Lenght = static_cast<size_t>(DecompressedImage.SizeX * DecompressedImage.SizeY * sizeof(FColor));

            TArray<uint8> DecompressedBuffer;
            DecompressedBuffer.SetNum(Lenght);
            FMemory::Memcpy(DecompressedBuffer.GetData(), DecompressedImage.AsBGRA8().GetData(), Lenght);

            FVector2D ImageSize = FVector2D(DecompressedImage.SizeX, DecompressedImage.SizeY);

            AsyncTask(ENamedThreads::GameThread, [DelegateImageBuffer, DecompressedBuffer]()
                {
                    DelegateImageBuffer.ExecuteIfBound(true, "Decompress is successful.", DecompressedBuffer);
                }
            );
        }
    );
}

bool UExtendedVarsBPLibrary::Import_T2D_Bytes(UTexture2D*& Out_Texture, TArray<uint8> In_Bytes, bool bUseSrgb)
{
    if (In_Bytes.Num() == 0)
    {
        return false;
    }

    UTexture2D* Texture = FImageUtils::ImportBufferAsTexture2D(In_Bytes);
    Texture->SRGB = bUseSrgb;

    if (IsValid(Texture) == true)
    {
        Out_Texture = Texture;
        return true;
    }

    else
    {
        return false;
    }
}

bool UExtendedVarsBPLibrary::Import_T2D_Bytes_LowLevel(UTexture2D*& Out_Texture, TArray<uint8> In_Bytes, FVector2D Size, bool bUseSrgb)
{
    if (In_Bytes.Num() == 0)
    {
        return false;
    }

    UTexture2D* Texture = UTexture2D::CreateTransient(Size.X, Size.Y, PF_B8G8R8A8);
    Texture->SRGB = bUseSrgb;

    FTexture2DMipMap& Texture_Mip = Texture->GetPlatformData()->Mips[0];
    void* Texture_Data = Texture_Mip.BulkData.Lock(LOCK_READ_WRITE);

    FMemory::Memcpy(Texture_Data, In_Bytes.GetData(), In_Bytes.Num());
    Texture_Mip.BulkData.Unlock();
    Texture->UpdateResource();

    Out_Texture = Texture;

    return true;
}

double UExtendedVarsBPLibrary::GetPaintRatio(UCanvasRenderTarget2D* InCRT, FLinearColor InWantedColor, uint8 Tolerance, bool bLogEachColor)
{
    if (!IsValid(InCRT))
    {
        return (double)-1;
    }

	UWorld* CurrentWorld = GEngine->GetCurrentPlayWorld();

    if (!IsValid(CurrentWorld))
    {
        return (double)-1;
    }

    TArray<FColor> Array_Colors;
    UKismetRenderingLibrary::ReadRenderTarget(CurrentWorld, InCRT, Array_Colors, false);
    const size_t TotalColors = Array_Colors.Num();

    if (TotalColors == 0)
    {
        return (double)-1;
    }

	// Each thread will process this many colors. (This value equals to 256x256 pixels)
    const size_t BlockSize = 65536;

	// Calculate the number of blocks we need to process all colors.
    const size_t NumBlocks = (TotalColors + BlockSize - 1) / BlockSize;
    std::atomic<int64> GlobalCount = { 0 };

    const FColor TargetColor = InWantedColor.ToFColor(false);

    ParallelFor(NumBlocks, [&](int32 BlockIdx)
        {
            const int32 Start = BlockIdx * BlockSize;
            const int32 End = FMath::Min(Start + BlockSize, TotalColors);

            int64 LocalCount = 0;
            // Tight loop over our slice; compare exact FColor equality.
            for (int32 Index = Start; Index < End; ++Index)
            {
				const FColor& Each_Color = Array_Colors[Index];

                if (bLogEachColor)
                {
                    UE_LOG(LogTemp, Warning, TEXT("Color : %s"), *Each_Color.ToString());
                }

                const uint8_t R = Each_Color.R;
				const uint8_t G = Each_Color.G;
				const uint8_t B = Each_Color.B;

				bool bDoesMatch = FMath::IsNearlyEqual(R, TargetColor.R, (double)Tolerance) && FMath::IsNearlyEqual(G, TargetColor.G, (double)Tolerance) && FMath::IsNearlyEqual(B, TargetColor.B, (double)Tolerance);

                if (bDoesMatch)
                {
                    ++LocalCount;
                }
            }

            // Single atomic add per block keeps contention tiny.
            if (LocalCount)
            {
                GlobalCount.fetch_add(LocalCount, std::memory_order_relaxed);
            }
        }
    );

    const double Ratio = (double)GlobalCount / (double)TotalColors;
	return Ratio;
}