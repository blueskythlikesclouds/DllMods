#include "AntiAliasing.h"
#include "BloomResolutionFix.h"
#include "BloomTypeImpl.h"
#include "Configuration.h"
#include "DeprecatedCode.h"
#include "DirectionalShadowImpl.h"
#include "LightShaftImpl.h"
#include "LuminanceRangeFix.h"
#include "ParameterManager.h"
#include "ParticlePostProcessing.h"
#include "ResolutionScaling.h"
#include "ShaderCodePatch.h"
#include "ShaderListPatch.h"
#include "ShadowCastImpl.h"
#include "SkyReflectionFix.h"

extern "C" void __declspec(dllexport) Init()
{
    if (!Configuration::load())
        MessageBox(nullptr, TEXT("Unable to locate BetterFxPipeline.ini"), TEXT("Better FxPipeline"), MB_ICONERROR);

    LuminanceRangeFix::init();
    SkyReflectionFix::init();
    ParameterManager::init();
    DirectionalShadowImpl::init();
    BloomTypeImpl::init();
    LightShaftImpl::init();
    ShaderListPatch::init();
    ResolutionScaling::init();
    AntiAliasing::init();
    ShadowCastImpl::init();
    ShaderCodePatch::init();
    BloomResolutionFix::init();
    ParticlePostProcessing::init();
}

extern "C" void __declspec(dllexport) PostInit()
{
    DeprecatedCode::init();
}

BOOL WINAPI DllMain(HINSTANCE instance, DWORD reason, LPVOID reserved) { return TRUE; }