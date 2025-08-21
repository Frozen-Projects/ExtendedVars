#include "Extended_Fonts.h"

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