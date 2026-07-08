#pragma once

#include "CoreMinimal.h"
#include "Extended_Includes.h"
#include "Extended_Delegates.h"

struct FFF_TextureReadbackContext
{
    TUniquePtr<FRHIGPUTextureReadback>  Readback;
    FDelegateImageBuffer                OnComplete;
    FIntPoint                           Size = FIntPoint::ZeroValue;
    EPixelFormat                        Format = PF_Unknown;
    int32                               FramesWaited = 0;

    // Safety net so a lost/never-signalled fence can't leak the ticker forever.
    static constexpr int32 MaxFramesToWait = 300; // 5s at 60 fps
};

using FContextRef = TSharedRef<FFF_TextureReadbackContext, ESPMode::ThreadSafe>;