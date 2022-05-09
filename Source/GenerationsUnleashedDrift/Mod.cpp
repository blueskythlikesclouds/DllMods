// Mod.cpp

enum class SkeletonType
{
    Generations,
    Unleashed
};

SkeletonType skeletonType;

const char* volatile const DriftL = "DriftL";
const char* volatile const DriftStartL = "DriftStartL";
const char* volatile const DriftEndL = "DriftEndL";
const char* volatile const DriftR = "DriftR";
const char* volatile const DriftStartR = "DriftStartR";
const char* volatile const DriftEndR = "DriftEndR";

// Generations: 16
// Unleashed: 20
float driftStartFinalFrame;

// Generations: 26
// Unleashed: 26
const float driftEndFinalFrame = 26.0f;

//~~~~~~~~~~~~~~~~~~~~~~~~~//
// Insert drift animations // 
//~~~~~~~~~~~~~~~~~~~~~~~~~//

HOOK(void*, __cdecl, InitializeSonicAnimationList, 0x1272490)
{
    void* result = originalInitializeSonicAnimationList();
    {
        CAnimationStateSet* set = (CAnimationStateSet*)0x15E8D40;
        CAnimationStateInfo* entries = new CAnimationStateInfo[set->count + 6];

        std::copy(set->entries, set->entries + set->count, entries);

        entries[set->count].name = DriftL;
        entries[set->count].fileName = skeletonType == SkeletonType::Generations ? "sn_drift_bb_l_loop" : "sn_drift_swa_l_loop";
        entries[set->count].speed = 1.0f;
        entries[set->count].playbackType = 0;
        entries[set->count].field10 = 0;
        entries[set->count].field14 = -1.0f;
        entries[set->count].field18 = -1.0f;
        entries[set->count].field1C = 0;
        entries[set->count].field20 = -1;
        entries[set->count].field24 = -1;
        entries[set->count].field28 = -1;
        entries[set->count].field2C = -1;

        entries[set->count + 1].name = DriftStartL;
        entries[set->count + 1].fileName = skeletonType == SkeletonType::Generations ? "sn_drift_bb_l_s" : "sn_drift_swa_l_s";
        entries[set->count + 1].speed = 1.0f;
        entries[set->count + 1].playbackType = 1;
        entries[set->count + 1].field10 = 0;
        entries[set->count + 1].field14 = -1.0f;
        entries[set->count + 1].field18 = -1.0f;
        entries[set->count + 1].field1C = 0;
        entries[set->count + 1].field20 = -1;
        entries[set->count + 1].field24 = -1;
        entries[set->count + 1].field28 = -1;
        entries[set->count + 1].field2C = -1;

        entries[set->count + 2].name = DriftEndL;
        entries[set->count + 2].fileName = skeletonType == SkeletonType::Generations ? "sn_drift_bb_l_e" : "sn_drift_swa_l_e";
        entries[set->count + 2].speed = 1.0f;
        entries[set->count + 2].playbackType = 1;
        entries[set->count + 2].field10 = 0;
        entries[set->count + 2].field14 = -1.0f;
        entries[set->count + 2].field18 = -1.0f;
        entries[set->count + 2].field1C = 0;
        entries[set->count + 2].field20 = -1;
        entries[set->count + 2].field24 = -1;
        entries[set->count + 2].field28 = -1;
        entries[set->count + 2].field2C = -1;

        entries[set->count + 3].name = DriftR;
        entries[set->count + 3].fileName = skeletonType == SkeletonType::Generations ? "sn_drift_bb_r_loop" : "sn_drift_swa_r_loop";
        entries[set->count + 3].speed = 1.0f;
        entries[set->count + 3].playbackType = 0;
        entries[set->count + 3].field10 = 0;
        entries[set->count + 3].field14 = -1.0f;
        entries[set->count + 3].field18 = -1.0f;
        entries[set->count + 3].field1C = 0;
        entries[set->count + 3].field20 = -1;
        entries[set->count + 3].field24 = -1;
        entries[set->count + 3].field28 = -1;
        entries[set->count + 3].field2C = -1;

        entries[set->count + 4].name = DriftStartR;
        entries[set->count + 4].fileName = skeletonType == SkeletonType::Generations ? "sn_drift_bb_r_s" : "sn_drift_swa_r_s";
        entries[set->count + 4].speed = 1.0f;
        entries[set->count + 4].playbackType = 1;
        entries[set->count + 4].field10 = 0;
        entries[set->count + 4].field14 = -1.0f;
        entries[set->count + 4].field18 = -1.0f;
        entries[set->count + 4].field1C = 0;
        entries[set->count + 4].field20 = -1;
        entries[set->count + 4].field24 = -1;
        entries[set->count + 4].field28 = -1;
        entries[set->count + 4].field2C = -1;

        entries[set->count + 5].name = DriftEndR;
        entries[set->count + 5].fileName = skeletonType == SkeletonType::Generations ? "sn_drift_bb_r_e" : "sn_drift_swa_r_e";
        entries[set->count + 5].speed = 1.0f;
        entries[set->count + 5].playbackType = 1;
        entries[set->count + 5].field10 = 0;
        entries[set->count + 5].field14 = -1.0f;
        entries[set->count + 5].field18 = -1.0f;
        entries[set->count + 5].field1C = 0;
        entries[set->count + 5].field20 = -1;
        entries[set->count + 5].field24 = -1;
        entries[set->count + 5].field28 = -1;
        entries[set->count + 5].field2C = -1;

        WRITE_MEMORY(&set->entries, void*, entries);
        WRITE_MEMORY(&set->count, size_t, set->count + 6);
    }

    return result;
}

HOOK(void, __fastcall, CSonicCreateAnimationStates, 0xE1B6C0, void* This, void* Edx, void* A2, void* A3)
{
    originalCSonicCreateAnimationStates(This, Edx, A2, A3);

    FUNCTION_PTR(void*, __stdcall, createAnimationState, 0xCDFA20,
        void* This, SharedPtrTypeless&spAnimationState, const hh::base::CSharedString & name, const hh::base::CSharedString & alsoName);

    SharedPtrTypeless animationState;

    createAnimationState(A2, animationState, DriftL, DriftL);
    createAnimationState(A2, animationState, DriftStartL, DriftStartL);
    createAnimationState(A2, animationState, DriftEndL, DriftEndL);
    createAnimationState(A2, animationState, DriftR, DriftR);
    createAnimationState(A2, animationState, DriftStartR, DriftStartR);
    createAnimationState(A2, animationState, DriftEndR, DriftEndR);
}

HOOK(bool, __stdcall, ParseArchiveTree, 0xD4C8E0, void* A1, char* data, const size_t size, void* database)
{
    std::string str;
    {
        std::stringstream stream;

        stream << "  <DefAppend>\n";
        stream << "    <Name>Sonic</Name>\n";
        stream << "    <Archive>SonicDrift</Archive>\n";
        stream << "  </DefAppend>\n";

        str = stream.str();
    }

    const size_t newSize = size + str.size();
    const std::unique_ptr<char[]> buffer = std::make_unique<char[]>(newSize);
    memcpy(buffer.get(), data, size);

    char* insertionPos = strstr(buffer.get(), "<Include>");

    memmove(insertionPos + str.size(), insertionPos, size - (size_t)(insertionPos - buffer.get()));
    memcpy(insertionPos, str.c_str(), str.size());

    bool result;
    {
        result = originalParseArchiveTree(A1, buffer.get(), newSize, database);
    }

    return result;
}

//~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~//
// Use correct drift sound depending on what surface Sonic is on // 
//~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~//

uint32_t currentDriftCueId;

uint32_t __cdecl getDriftCueId()
{
    CSonicStateFlags* flags = GetSonicStateFlags();
    return flags->OnWater ? 2002059 : 2002039;
}

uint32_t CSonicStateDriftDriftingSoundMidAsmHookReturnAddress = 0xDF36A3;

void __declspec(naked) CSonicStateDriftDriftingSoundMidAsmHook()
{
    __asm
    {
        push ebx
        push ecx
        push edx

        call getDriftCueId
        mov currentDriftCueId, eax

        pop edx
        pop ecx
        pop ebx

        push eax

        jmp[CSonicStateDriftDriftingSoundMidAsmHookReturnAddress]
    }
}

//~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~//
// Implement Intro -> Loop -> Outro animation sequence //
// Restore footsteps when drifting                     // 
//~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~//

//
// Choose left/right intro animation appropriately
//

bool __fastcall ShouldDriftStartChangeAnimation()
{
    void* sonicContext = *(void**)0x1E5E2F0;
    MsgGetAnimationInfo msgGetAnimationInfo{};
    CSonicSpeedProcMsgGetAnimationInfo(*(void**)((uint8_t*)sonicContext + 0x110), &msgGetAnimationInfo);

    // Return false if current animation is already drift, but true if it's ending
    return (strstr(msgGetAnimationInfo.name, "Drift") == nullptr || strstr(msgGetAnimationInfo.name, "DriftEnd") != nullptr);
}

uint32_t CSonicStateDriftDriftingLoopAnimationMidAsmHookReturnAddress = 0xDF35F3;

void __declspec(naked) CSonicStateDriftDriftingLoopAnimationMidAsmHook()
{
    static size_t skipAnimDriftR = 0xDF3611;
    static size_t skipAnimDriftL = 0xDF3644;
    __asm
    {
        // check if change anim
        push ebx
        call [ShouldDriftStartChangeAnimation]
        pop ebx

        test al,al
        jnz changeAnim

        fldz
        fxch st(1)
        lea ecx, [esp + 0x40 - 0x2C]
        fcomip st, st(1)
        fstp st
        jbe skipDriftL
        jmp [skipAnimDriftR]
    skipDriftL:
        jmp [skipAnimDriftL]

    changeAnim:
        fldz
        fxch st(1)
        lea ecx, [esp + 0x40 - 0x2C]
        fcomip st, st(1)
        fstp st
        jbe pushDriftL

        lea edx, DriftStartR
        jmp return

    pushDriftL:
        lea edx, DriftStartL

    return:
        push[edx]
        jmp[CSonicStateDriftDriftingLoopAnimationMidAsmHookReturnAddress]
    }
}

//
// Handle Loop -> Outro transition
// Implement footstep sounds
// Handle Water -> Ground drift sound transition
//

bool wasDriftRight = false;
float driftDirectionTime = 0.0f;
HOOK(void, __fastcall, CSonicStateDriftDriftingUpdate, 0xDF32D0, void* This)
{
    void* sonicContext = *(void**)((uint32_t)*(void**)((uint32_t)This + 8) + 8);
    float elapsedTime = *(float*)(*(uint32_t*)((uint32_t)This + 12) + 36);

    const uint32_t cueId = getDriftCueId();
    if (currentDriftCueId != cueId)
    {
        CSonicSpeedContextPlaySound* playSound = *(CSonicSpeedContextPlaySound**)(*(uint32_t*)sonicContext + 116);

        SharedPtrTypeless srcSoundHandle;
        playSound(sonicContext, nullptr, srcSoundHandle, cueId, 1);

        SharedPtrTypeless* dstSoundHandle = (SharedPtrTypeless*)((char*)This + 96);
        dstSoundHandle->reset(); // For some reason I have to reset it first, otherwise the sound handle stays.
        *dstSoundHandle = srcSoundHandle;
        currentDriftCueId = cueId;
    }

    originalCSonicStateDriftDriftingUpdate(This);

    MsgGetAnimationInfo msgGetAnimationInfo {};
    CSonicSpeedProcMsgGetAnimationInfo(*(void**)((uint8_t*)sonicContext + 0x110), &msgGetAnimationInfo);

    if (strstr(msgGetAnimationInfo.name, "DriftStartL") != nullptr && msgGetAnimationInfo.frame >= driftStartFinalFrame)
    {
        CSonicContextChangeAnimation(sonicContext, DriftL);
    }
    else if (strstr(msgGetAnimationInfo.name, "DriftStartR") != nullptr && msgGetAnimationInfo.frame >= driftStartFinalFrame)
    {
        CSonicContextChangeAnimation(sonicContext, DriftR);
    }
     
    bool driftRight = GetSonicStateFlags()->DriftRight;
    if (wasDriftRight != driftRight)
    {
        // Reset timer if switching direction
        driftDirectionTime = 0.0f;
        wasDriftRight = driftRight;
    }
    else
    {
        driftDirectionTime += elapsedTime;
        if (driftDirectionTime > 0.4f)
        {
            if (strstr(msgGetAnimationInfo.name, "DriftL") != nullptr && driftRight)
            {
                driftDirectionTime = 0.0f;
                CSonicContextChangeAnimation(sonicContext, DriftStartR);
            }
            else if (strstr(msgGetAnimationInfo.name, "DriftR") != nullptr && !driftRight)
            {
                driftDirectionTime = 0.0f;
                CSonicContextChangeAnimation(sonicContext, DriftStartL);
            }
        }
    }

    static float intervalTime = 0.0f;
    if (intervalTime > 0.07f)
    {
        CSonicSpeedContextPlaySound* playSound = *(CSonicSpeedContextPlaySound**)(*(uint32_t*)sonicContext + 116);

        FUNCTION_PTR(uint32_t, __thiscall, getSurfaceId, 0xDFD420, void* This, uint32_t type);

        SharedPtrTypeless soundHandle;
        playSound(sonicContext, nullptr, soundHandle, getSurfaceId(sonicContext, 1), 1);

        intervalTime = 0.0f;
    }
    else
    {
        intervalTime += 1.0f / 60.0f;
    }
}

//
// Swap to outro animation when finishing drift
//

HOOK(void, __fastcall, CSonicStateDriftOnLeave, 0xDF2D20, void* This)
{
    driftDirectionTime = 0.0f;
    void* sonicContext = *(void**)0x1E5E2F0;

    MsgGetAnimationInfo msgGetAnimationInfo {};
    CSonicSpeedProcMsgGetAnimationInfo(*(void**)((uint8_t*)sonicContext + 0x110), &msgGetAnimationInfo);

    if (strcmp(msgGetAnimationInfo.name, DriftL) == 0 || strcmp(msgGetAnimationInfo.name, DriftR) == 0)
        CSonicContextChangeAnimation(sonicContext, GetSonicStateFlags()->DriftRight ? DriftEndR : DriftEndL);

    originalCSonicStateDriftOnLeave(This);
}

//
// Make outro animation persist till it's done playing
//

HOOK(void, __stdcall, ChangeAnimation, 0xCDFC80, void* A1, SharedPtrTypeless& A2, const hh::base::CSharedString& name)
{
    if (name == "Walk")
    {
        void* sonicContext = *(void**)0x1E5E2F0;

        MsgGetAnimationInfo msgGetAnimationInfo {};
        CSonicSpeedProcMsgGetAnimationInfo(*(void**)((uint8_t*)sonicContext + 0x110), &msgGetAnimationInfo);

        if (strstr(msgGetAnimationInfo.name, "DriftEnd") != nullptr && msgGetAnimationInfo.frame < driftEndFinalFrame)
        {
            memset(&A2, 0, sizeof(SharedPtrTypeless));
            return;
        }
    }

    originalChangeAnimation(A1, A2, name);
}

//
// Swap to walk animation when outro is done
//

// TODO: Does not work properly with brake. Sonic gets stuck in walk animation when idling.

HOOK(void, __fastcall, CPlayerSpeedUpdate, 0xE6BF20, void* This, void* Edx, void* A2)
{
    void* sonicContext = *(void**)0x1E5E2F0;

    MsgGetAnimationInfo msgGetAnimationInfo {};
    CSonicSpeedProcMsgGetAnimationInfo(*(void**)((uint8_t*)sonicContext + 0x110), &msgGetAnimationInfo);

    if (strstr(msgGetAnimationInfo.name, "DriftEnd") != nullptr && msgGetAnimationInfo.frame >= driftEndFinalFrame)
        CSonicContextChangeAnimation(sonicContext, "Walk");

    originalCPlayerSpeedUpdate(This, Edx, A2);
}

//~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~//
// Restore sea-spray sound when boosting on water // 
//~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~//
static SharedPtrTypeless seaSpraySoundHandle;
HOOK(void, __fastcall, CSonicStatePluginOnWaterUpdate, 0x119BED0, void* This)
{
	CSonicStateFlags* flags = GetSonicStateFlags();
	if (flags->OnWater && flags->Boost)
	{
		if (seaSpraySoundHandle == nullptr)
		{
			void* sonicContext = *(void**)((uint32_t)This + 8);
			CSonicSpeedContextPlaySound* playSound = *(CSonicSpeedContextPlaySound**)(*(uint32_t*)sonicContext + 116);

			playSound(sonicContext, nullptr, seaSpraySoundHandle, 2002059, 1);
		}
	}
	else
	{
		seaSpraySoundHandle.reset();
	}

	originalCSonicStatePluginOnWaterUpdate(This);
}

// Fix dying while boosting on water doesn't destroy the sfx
HOOK(int, __fastcall, MsgRestartStage, 0xE76810, uint32_t* This, void* Edx, void* message)
{
    seaSpraySoundHandle.reset();
    return originalMsgRestartStage(This, Edx, message);
}

//~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~//
// Don't abruptly stop player when drift threshold isn't met //
//~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~//
uint32_t CSonicStateDriftStartUpdateMidAsmHookReturnAddress = 0xDF2D18;

void __declspec(naked) CSonicStateDriftStartUpdateMidAsmHook()
{
    __asm
    {
        mov edi, dword ptr[edi + 8]
        lea edi, [edi + 107]
        mov byte ptr[edi], 1
        jmp[CSonicStateDriftStartUpdateMidAsmHookReturnAddress]
    }
}

//~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~//
// Tests to make controls act like Unleashed.                                          //
// These do not work properly. Leaving them here because I might return to them later. //
// ~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~//

int __stdcall compareIsLessThanOrEqualToZero(float value)
{
	return value <= 0;
}

const volatile float CSonicPostureDriftAngleThresholdLeft = -cosf(1.308996915817261f);
const volatile float CSonicPostureDriftAngleThresholdRight = cosf(1.308996915817261f);

uint32_t CSonicPostureDriftMidAsmHookReturnAddressOnFalse = 0x1199042;
uint32_t CSonicPostureDriftMidAsmHookReturnAddressOnTrue = 0x119902F;
uint32_t CSonicPostureDriftFunctionAddress = 0x9BF650;

void __declspec(naked) CSonicPostureDriftMidAsmHookLeft()
{
	__asm
	{
		call[CSonicPostureDriftFunctionAddress]
		fld[CSonicPostureDriftAngleThresholdLeft]

		push[esp + 0x28]
		call compareIsLessThanOrEqualToZero

		cmp eax, 0
		jz OnTrue

		jmp[CSonicPostureDriftMidAsmHookReturnAddressOnFalse]

    OnTrue:
		jmp[CSonicPostureDriftMidAsmHookReturnAddressOnTrue]
	}
}

void __declspec(naked) CSonicPostureDriftMidAsmHookRight()
{
    __asm
	{
		call[CSonicPostureDriftFunctionAddress]
		fld[CSonicPostureDriftAngleThresholdRight]
		fxch st(1)

		push[esp + 0x28]
		call compareIsLessThanOrEqualToZero

		cmp eax, 0
		jnz OnTrue

		jmp[CSonicPostureDriftMidAsmHookReturnAddressOnFalse]

	OnTrue:
		jmp[CSonicPostureDriftMidAsmHookReturnAddressOnTrue]
	}
}

uint32_t CSonicPostureDriftMidAsmHookCmpReturnAddress = 0x1199014;

void __declspec(naked) CSonicPostureDriftMidAsmHookCmp()
{
    __asm
	{
		fld [esp+0x28]
		fldz
		fcomip st(0), st(1)
		jmp[CSonicPostureDriftMidAsmHookCmpReturnAddress]
	}
}

//~~~~~~~~~~~~~//
// Entry Point //
//~~~~~~~~~~~~~//

extern "C" __declspec(dllexport) void __cdecl Init(ModInfo* info)
{
    std::string dir = info->CurrentMod->Path;

    size_t pos = dir.find_last_of("\\/");
    if (pos != std::string::npos)
        dir.erase(pos + 1);

    const INIReader reader(dir + "GenerationsUnleashedDrift.ini");

    if (reader.ParseError() != 0)
        MessageBox(NULL, L"Failed to parse GenerationsUnleashedDrift.ini", NULL, MB_ICONERROR);
    else
        skeletonType = (SkeletonType)reader.GetInteger("Main", "SkeletonType", 0);

    driftStartFinalFrame = skeletonType == SkeletonType::Generations ? 16.0f : 20.0f;

    // Add drift animations to the animation list
    INSTALL_HOOK(InitializeSonicAnimationList);

    // Patch archive tree to load animation files
    INSTALL_HOOK(ParseArchiveTree);

    // Don't change to ball model during drift
    WRITE_NOP(0xDF30AB, 0xD);

    // Make hop end immediately
    static double hopHeight = 0;
    WRITE_MEMORY(0xDF2BD5 + 2, double*, &hopHeight);

    // Don't change animation in CStart state
    WRITE_NOP(0xDF2B33, 0xE);

    // Don't abruptly stop player when abort is called
    WRITE_JUMP(0xDF2CE8, CSonicStateDriftStartUpdateMidAsmHook);
    WRITE_MEMORY(0xDF2F4B, uint8_t, 0x90, 0xE9);

    // Use correct animations
    WRITE_JUMP(0xDF35E2, CSonicStateDriftDriftingLoopAnimationMidAsmHook);

    // Prevent Sonic from actually hopping
    WRITE_MEMORY(0xDF2C58, uint8_t, 0xD9, 0x54, 0x24, 0x90);

    // Don't play hop sound
    WRITE_MEMORY(0xDF2B59 + 1, int32_t, -1);

	// Play water slider sound when drifting on water
	WRITE_JUMP(0xDF369E, CSonicStateDriftDriftingSoundMidAsmHook);

	// Player surfing sound when boosting on water
	INSTALL_HOOK(CSonicStatePluginOnWaterUpdate);
	INSTALL_HOOK(MsgRestartStage);

    // Don't slow down on ground
    WRITE_JUMP(0xDF33A7, (void*)0xDF3482);

	// Keep drifting even if we hit an obstacle
	WRITE_MEMORY(0xDF3373, uint8_t, 0xEB);

	// Play footsteps when drifting
	INSTALL_HOOK(CSonicStateDriftDriftingUpdate);

	// Don't set animation when outro animation is playing (yes I know it's hacky)
	INSTALL_HOOK(ChangeAnimation);

	INSTALL_HOOK(CSonicStateDriftOnLeave);

	INSTALL_HOOK(CPlayerSpeedUpdate);

	// Attempts to simulate Sonic Unleashed's turning
	// WRITE_NOP(0x1198FDB, 2);
	// WRITE_JUMP(0x119901D, CSonicPostureDriftMidAsmHookLeft);
	// WRITE_JUMP(0x1199026, CSonicPostureDriftMidAsmHookRight);
	// WRITE_MEMORY(0x119901B, uint8_t, 0x75);
	// WRITE_JUMP(0x119900D, CSonicPostureDriftMidAsmHookCmp);
	// WRITE_MEMORY(0x119901B, uint8_t, 0x7D);

	INSTALL_HOOK(CSonicCreateAnimationStates);

	// Disable drift particle
	WRITE_MEMORY(0x15E92B4, uint8_t, 0x00);
	WRITE_MEMORY(0x15E92CC, uint8_t, 0x00);
}