#include "Shader.h"
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

static FUNCTION_PTR(HRESULT, WINAPI, D3DXGetShaderConstantTable, 0xC85640, const DWORD* pFunction, size_t ppConstantTable);

BOOL __fastcall implOfCreateShader(size_t This, void* _, const DWORD* vertexShader, const DWORD* pixelShader)
{
	IDirect3DDevice9* device = *reinterpret_cast<IDirect3DDevice9**>(0x5BB9534);

	if (device == nullptr || (vertexShader == nullptr && pixelShader == nullptr))
		return FALSE;

	if (FAILED(device->CreateVertexShader(vertexShader, reinterpret_cast<IDirect3DVertexShader9**>(This + 36))))
		return FALSE;

	D3DXGetShaderConstantTable(vertexShader, This + 40);

	if (pixelShader != nullptr)
	{
		size_t dwordCount = 0;
		while (pixelShader[dwordCount] != 0xFFFF)
			++dwordCount;

		const XXH32_hash_t hash = XXH32(pixelShader, (dwordCount + 1) * sizeof(DWORD), 0);
		auto it = std::lower_bound(s_shaders, s_shaders + s_shaderCount, hash, [](const auto& lhs, const auto& rhs) { return lhs.first < rhs; });

		const DWORD* pixelShaderToCreate = pixelShader;

		if (it != (s_shaders + s_shaderCount) && it->first == hash)
			pixelShaderToCreate = reinterpret_cast<const DWORD*>(it->second);

		if (FAILED(device->CreatePixelShader(pixelShaderToCreate, reinterpret_cast<IDirect3DPixelShader9**>(This + 28))))
			return FALSE;

		D3DXGetShaderConstantTable(pixelShader, This + 32);
	}

	return TRUE;
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
		WRITE_JUMP(0xC5FD70, implOfCreateShader);
		WRITE_MEMORY(0xC66FB3, uint8_t, D3DFMT_A16B16G16R16F);
		WRITE_MEMORY(0xC66FDA, uint8_t, D3DFMT_A16B16G16R16F);
		
	}

	return TRUE;
}
