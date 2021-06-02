#pragma once 

#include <Hedgehog/Base/hhObject.h>
#include <Hedgehog/Base/hhRefCountObject.h>
#include <Hedgehog/Base/Container/hhList.h>
#include <Hedgehog/Base/Container/hhMap.h>
#include <Hedgehog/Base/Container/hhTree.h>
#include <Hedgehog/Base/Container/hhVector.h>
#include <Hedgehog/Base/Platform/D3D9/hhCriticalSectionD3D9.h>
#include <Hedgehog/Base/System/hhAllocator.h>
#include <Hedgehog/Base/System/hhSymbol.h>
#include <Hedgehog/Base/Thread/hhHolderBase.h>
#include <Hedgehog/Base/Thread/hhSynchronizedObject.h>
#include <Hedgehog/Base/Type/hhSharedString.h>
#include <Hedgehog/Database/System/hhArchiveDatabaseLoader.h>
#include <Hedgehog/Database/System/hhDatabase.h>
#include <Hedgehog/Database/System/hhDatabaseData.h>
#include <Hedgehog/Database/System/hhDatabaseLoader.h>
#include <Hedgehog/Database/System/hhRawData.h>
#include <Hedgehog/MirageCore/Camera/hhCamera.h>
#include <Hedgehog/MirageCore/Misc/hhDirector.h>
#include <Hedgehog/MirageCore/Misc/hhLightManager.h>
#include <Hedgehog/MirageCore/Misc/hhRenderingDevice.h>
#include <Hedgehog/MirageCore/Misc/hhRenderingInfrastructure.h>
#include <Hedgehog/MirageCore/Misc/hhStaticLightContext.h>
#include <Hedgehog/MirageCore/RenderData/hhLightData.h>
#include <Hedgehog/MirageCore/RenderData/hhLightListData.h>
#include <Hedgehog/MirageCore/RenderData/hhMirageDatabaseWrapper.h>
#include <Hedgehog/MirageCore/RenderData/hhPictureData.h>
#include <Hedgehog/MirageCore/RenderData/hhPixelShaderData.h>
#include <Hedgehog/MirageCore/RenderData/hhVertexShaderData.h>
#include <Hedgehog/MotionCore/MotionData/hhLightMotionData.h>
#include <Hedgehog/MotionCore/MotionData/hhMotionDatabaseWrapper.h>
#include <Hedgehog/Universe/Engine/hhMessage.h>
#include <Hedgehog/Universe/Engine/hhMessageActor.h>
#include <Hedgehog/Universe/Engine/hhMessageProcess.h>
#include <Hedgehog/Universe/Engine/hhParallelJob.h>
#include <Hedgehog/Universe/Engine/hhStateMachineMessageReceiver.h>
#include <Hedgehog/Universe/Engine/hhUpdateUnit.h>
#include <Hedgehog/Yggdrasill/hhYggAbstractBuffer.h>
#include <Hedgehog/Yggdrasill/hhYggAllocator.h>
#include <Hedgehog/Yggdrasill/hhYggDevice.h>
#include <Hedgehog/Yggdrasill/hhYggJob.h>
#include <Hedgehog/Yggdrasill/hhYggMisc.h>
#include <Hedgehog/Yggdrasill/hhYggPicture.h>
#include <Hedgehog/Yggdrasill/hhYggSceneRenderer.h>
#include <Hedgehog/Yggdrasill/hhYggScheduler.h>
#include <Hedgehog/Yggdrasill/hhYggSurface.h>
#include <Hedgehog/Yggdrasill/hhYggTexture.h>

// Lost World/Forces style namespace aliases
namespace hh = Hedgehog;
namespace Hedgehog
{
    namespace base = Base;
    namespace db = Database;
    namespace mr = Mirage;
    namespace mot = Motion;
    namespace fnd = Universe;
    namespace ygg = Yggdrasill;
}