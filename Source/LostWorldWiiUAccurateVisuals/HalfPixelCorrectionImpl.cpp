#include "HalfPixelCorrectionImpl.h"

#include "Configuration.h"

HOOK(void, __cdecl, HUDSetupShader, ASLR(0x0052FD60), bool a1, int vs_type, int ps_type, bool set_screen, void* color)
{
	auto infrastructure = app::gfx::RenderManager::GetInstance()->GetRenderingDevice();
	auto& device = infrastructure->m_RenderingDevice;

	originalHUDSetupShader(a1, vs_type, ps_type, set_screen, color);

	if (device.m_pCurrentVertexShaderData != nullptr)
	{
		hh::rsdx::_RSDXFORMAT format{};
		uint32_t width{};
		uint32_t height{};

		infrastructure->GetScreenInfo(width, height, format);

		const float screen[4] = { static_cast<float>(width), static_cast<float>(height), 0, 0 };
		device.SetVertexShaderParameterF("g_Screen_Size", screen, 0, 1, device.m_pCurrentVertexShaderData);
	}
}

void HalfPixelCorrectionImpl::init()
{
	if (!Configuration::halfPixelCorrection)
		return;

	// SurfRide Half pixel correction
	INSTALL_HOOK(HUDSetupShader);
}
