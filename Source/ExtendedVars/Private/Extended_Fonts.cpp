#include "Extended_Fonts.h"
#include "Extended_BPLib.h"

void URuntimeFont::BeginDestroy()
{
    if (IsValid(this->Font_Face))
    {
        this->Font_Face->RemoveFromRoot();
        bool bIsFontFaceDestroyed = this->Font_Face->ConditionalBeginDestroy();

        if (bIsFontFaceDestroyed)
        {
            UE_LOG(LogTemp, Display, TEXT("FontFace destroyed : %s"), *this->FontName);
        }

        else
        {
            UE_LOG(LogTemp, Warning, TEXT("FontFace couldn't destroyed"));
        }
    }

    if (IsValid(this->Font))
    {
        this->Font->RemoveFromRoot();
        bool bIsFontDestroyed = this->Font->ConditionalBeginDestroy();

        if (bIsFontDestroyed)
        {
            UE_LOG(LogTemp, Display, TEXT("Font destroyed : %s"), *this->FontName);
        }

        else
        {
            UE_LOG(LogTemp, Warning, TEXT("Font couldn't destroyed"));
        }
    }

    Super::BeginDestroy();
}

URuntimeFont* UExtendedVarsBPLibrary::Runtime_Font_Load(TArray<uint8> In_Bytes, FString FontName)
{
    UFontFace* FontFace = NewObject<UFontFace>();
    FontFace->LoadingPolicy = EFontLoadingPolicy::Inline;
    FontFace->FontFaceData = FFontFaceData::MakeFontFaceData(CopyTemp(In_Bytes));
    FontFace->AddToRoot();

    UFont* Font = NewObject<UFont>();
    Font->FontCacheType = EFontCacheType::Runtime;

    FCompositeFont& CompositeFont = Font->GetMutableInternalCompositeFont();
    TArray<FTypefaceEntry>& Array_Fonts = CompositeFont.DefaultTypeface.Fonts;
    FTypefaceEntry& TypefaceEntry = Array_Fonts[Array_Fonts.AddDefaulted()];

    TypefaceEntry.Name = FName(*FontName);
    TypefaceEntry.Font = FFontData(FontFace);

    Font->AddToRoot();

    URuntimeFont* RuntimeFont = NewObject<URuntimeFont>();
    RuntimeFont->Font_Face = FontFace;
    RuntimeFont->Font = Font;
    RuntimeFont->FontName = FontName;

    return RuntimeFont;
}
