template<typename T>
struct RelPtr
{
	uint32_t value;

	T* get(uint8_t* begin) const
	{
		return (T*)(begin + value);
	}

	bool null() const
	{
		return value == 0;
	}
};

struct StrIndex
{
	uint32_t value;

	const char* get(const RelPtr<RelPtr<const char>>& strings, uint8_t* objSet) const
	{
		if ((value & 0x8000) == 0)
			return nullptr;

		return strings.get(objSet)[value & 0x7FFF].get(objSet);
	}
};

struct OsageNode
{
	StrIndex name;
	float length;
	uint32_t matrixIndex;
};

struct OsageBlock
{
	RelPtr<const char> parentName;
	float position[3];
	float rotation[3];
	float scale[3];

	uint32_t nodeIndex;
	uint32_t nodeCount;

	StrIndex externalName;
	StrIndex name;

	RelPtr<void> skinParam;
	uint32_t reserved0[3];
};

struct Block
{
	RelPtr<const char> signature;
	RelPtr<void> data;
};

struct ExData
{
	uint32_t osageCount;
	uint32_t osageNodeCount;
	uint32_t reserved0;
	RelPtr<OsageNode> osageNodes;
	RelPtr<RelPtr<const char>> osageNames;
	RelPtr<Block> blocks;
	uint32_t stringCount;
	RelPtr<RelPtr<const char>> strings;
	RelPtr<void> osageSiblingInfos;
	uint32_t clothCount;
	uint32_t reserved1[7];
};

SIG_SCAN
(
	sigParseExData,
	0x1405983C0,
	"\x48\x89\x5C\x24\x20\x48\x89\x4C\x24\x08\x55\x56\x57\x41\x54\x41\x55\x41\x56\x41\x57\x48\x83\xEC\x40", "xxxxxxxxxxxxxxxxxxxxxxxxx"
);

HOOK(void*, __fastcall, ParseExData, sigParseExData(), void* a1, ExData* exData, uint8_t* objSet)
{
	for (size_t i = 0;; i++)
	{
		const Block& block = exData->blocks.get(objSet)[i];

		if (block.signature.null())
			break;

		if (strcmp(block.signature.get(objSet), "OSG") != 0)
			continue;

		OsageBlock* osageBlock = (OsageBlock*)block.data.get(objSet);

		const char* name = osageBlock->externalName.get(exData->strings, objSet);

		if (strstr(name, "tail") == nullptr && strstr(name, "level") == nullptr)
			continue;

		for (size_t j = 0; j < osageBlock->nodeCount; j++)
			exData->osageNodes.get(objSet)[osageBlock->nodeIndex + j].length *= 2;
	}

	return originalParseExData(a1, exData, objSet);
}

SIG_SCAN
(
	sigParseSkinParam,
	0x14059F020,
	"\x48\x89\x5C\x24\x20\x55\x56\x57\x41\x54\x41\x55\x41\x56\x41\x57\x48\x8D\xAC\x24\x60\xFE", "xxxxxxxxxxxxxxxxxxxxxx"
);

HOOK(void*, __fastcall, ParseSkinParam, sigParseSkinParam(), void* a1, const char* name, uint8_t* skinParam)
{
	void* result = originalParseSkinParam(a1, name, skinParam);

	uint32_t* coliType = (uint32_t*)(skinParam + 112);

	// Sphere -> Capsule
	*coliType = *coliType == 0 && (strstr(name, "tail") != nullptr || strstr(name, "level") != nullptr) ? 1 : *coliType;

	return result;
}

extern "C" __declspec(dllexport) void Init()
{
    if (sigValid)
    {
		INSTALL_HOOK(ParseExData);
		INSTALL_HOOK(ParseSkinParam);
    }
    else
    {
        MessageBox(nullptr, TEXT("Failed to install Rapunzel (game version is unsupported)"), TEXT("MegaMixRapunzel"), MB_ICONERROR);
    }
}

BOOL WINAPI DllMain(HINSTANCE instance, DWORD reason, LPVOID reserved)
{
    return TRUE;
}