#include "ToneMap.hlsl.h"

static FUNCTION_PTR(void, __cdecl, drawQuad2D, 0x44BC90,
	void* a1,
	void* a2,
	void* a3,
	void* a4,
	void* a5,
	void* a6,
	void* a7,
	void* a8,
	void* a9,
	void* a10,
	void* a11,
	void* a12);

struct DrawContext
{
	INSERT_PADDING(0x1C);
	IDirect3DPixelShader9* d3dPixelShader;
};

static ComPtr<IDirect3DPixelShader9> s_toneMapPixelShader;

static void __cdecl implOfDrawQuad2D(
	void* a1,
	void* a2,
	void* a3,
	void* a4,
	void* a5,
	void* a6,
	void* a7,
	void* a8,
	void* a9,
	void* a10,
	void* a11,
	void* a12)
{
	const auto drawContext = reinterpret_cast<DrawContext*>(0x50AD270);

	if (!s_toneMapPixelShader)
	{
		(*reinterpret_cast<IDirect3DDevice9Ex**>(0x5BB9534))->CreatePixelShader(
			reinterpret_cast<const DWORD*>(ToneMapBytecode), s_toneMapPixelShader.GetAddressOf());
	}

	const auto pixelShader = drawContext->d3dPixelShader;

	if ((GetAsyncKeyState(VK_F6) & 0x8000) == 0)
		drawContext->d3dPixelShader = s_toneMapPixelShader.Get();

	drawQuad2D(a1, a2, a3, a4, a5, a6, a7, a8, a9, a10, a11, a12);

	drawContext->d3dPixelShader = pixelShader;
}

static FUNCTION_PTR(void, __fastcall, draw, 0xC706E0, void* a1);

static void __fastcall implOfIdDraw(void* a1)
{
	if ((GetAsyncKeyState(VK_F7) & 0x8000) == 0)
		draw(a1);
}

BOOL WINAPI DllMain(HINSTANCE instance, DWORD reason, LPVOID reserved)
{
	if (reason == DLL_PROCESS_ATTACH)
	{
#ifdef _DEBUG
		if (!GetConsoleWindow())
			AllocConsole();

		freopen("CONOUT$", "w", stdout);
#endif

		WRITE_CALL(0x4E529E, implOfDrawQuad2D);
		WRITE_MEMORY(0xC66FB3, uint8_t, D3DFMT_A16B16G16R16F);
		WRITE_MEMORY(0xC66FDA, uint8_t, D3DFMT_A16B16G16R16F);
		WRITE_CALL(0x4E5358, implOfIdDraw);
		WRITE_CALL(0x4E53E0, implOfIdDraw);
	}

	return TRUE;
}
