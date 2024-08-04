#include "Fmerge.h"

struct FmergeNameTableHeader
{
	uint32_t stride;

	char* getName(size_t index)
	{
		return reinterpret_cast<char*>(this + 1) + index * stride;
	}
};

struct FmergeHeader
{
	char identifier[4];
	uint32_t fileNum;
	uint32_t offsetTableOffset;
	uint32_t extensionTableOffset;
	uint32_t fileNameTableOffset;
	uint32_t sizeTableOffset;
	uint32_t padding0;
	uint32_t padding1;

	uint32_t* getOffset(size_t index)
	{
		return reinterpret_cast<uint32_t*>(identifier + offsetTableOffset) + index;
	}

	char* getExtension(size_t index)
	{
		return reinterpret_cast<char*>(identifier + extensionTableOffset) + sizeof(char[4]) * index;
	}

	FmergeNameTableHeader* getFileNameTable()
	{
		return reinterpret_cast<FmergeNameTableHeader*>(identifier + fileNameTableOffset);
	}

	uint32_t* getSize(size_t index)
	{
		return reinterpret_cast<uint32_t*>(identifier + sizeTableOffset) + index;
	}
};

struct Heap;

struct FileReadManagerWork
{
	INSERT_PADDING(0x4);
	uint32_t fileType;
	char filePath[0x20];
	INSERT_PADDING(0x4);
	FmergeHeader* fileData;
	uint32_t needSize;
	Heap* heap;
	uint32_t flag;
};

static FUNCTION_PTR(void*, __thiscall, heapAlloc, 0xC58070, Heap* This, uint32_t size, uint32_t align, uint32_t mode);
static FUNCTION_PTR(void, __cdecl, heapFree, 0xC58200, void* ptr);

static size_t alignUp(size_t value, size_t alignment)
{
	return (value + alignment - 1) & ~(alignment - 1);
}

HOOK(void, __fastcall, RegisterResource, 0x439900, FileReadManagerWork* This)
{
	if (This->fileType >= 1 && This->fileType <= 3 &&
		This->needSize >= sizeof(FmergeHeader) &&
		strcmp(This->fileData->identifier, "DAT") == 0)
	{		
		char dirPath[MAX_PATH];
		strcpy_s(dirPath, This->filePath);

		char* ext = strstr(dirPath, ".dat");
		if (ext != nullptr)
			*ext = '\0';

		char filter[MAX_PATH];
		sprintf_s(filter, "%s\\*", dirPath);

		WIN32_FIND_DATAA findFileData{};
		HANDLE hFindFile = FindFirstFileA(filter, &findFileData);
		if (hFindFile != INVALID_HANDLE_VALUE)
		{
			const uint32_t fileNameStride = This->fileData->getFileNameTable()->stride;
			bool foundAny = false;

			do
			{
				if ((findFileData.dwFileAttributes & FILE_ATTRIBUTE_DIRECTORY) == 0)
				{
					for (size_t i = 0; i < This->fileData->fileNum; i++)
					{
						if (strcmp(This->fileData->getFileNameTable()->getName(i), findFileData.cFileName) == 0)
						{
							*This->fileData->getOffset(i) = 0;
							*This->fileData->getSize(i) = findFileData.nFileSizeLow;

							foundAny = true;
							break;
						}
					}
				}
			} while (FindNextFileA(hFindFile, &findFileData));
			FindClose(hFindFile);

			if (foundAny)
			{
				FmergeHeader header;
				header.fileNum = This->fileData->fileNum;
				header.offsetTableOffset = sizeof(FmergeHeader);
				header.extensionTableOffset = header.offsetTableOffset + This->fileData->fileNum * sizeof(uint32_t);
				header.fileNameTableOffset = header.extensionTableOffset + This->fileData->fileNum * sizeof(char[4]);
				header.sizeTableOffset = alignUp(header.fileNameTableOffset + sizeof(uint32_t) + This->fileData->fileNum * fileNameStride, 0x4);
				header.padding0 = 0;
				header.padding1 = 0;

				size_t dataOffset = alignUp(header.sizeTableOffset + This->fileData->fileNum * sizeof(uint32_t), 0x10);
				size_t needSize = dataOffset;

				for (size_t i = 0; i < This->fileData->fileNum; i++)
					needSize += alignUp(*This->fileData->getSize(i), 0x10);

				auto fileData = reinterpret_cast<FmergeHeader*>(heapAlloc(This->heap, needSize, 0x1000, ((This->flag & 0x10) != 0) + 1));

				*fileData = header;
				memcpy(fileData->getExtension(0), This->fileData->getExtension(0), This->fileData->fileNum * sizeof(char[4]));
				memcpy(fileData->getFileNameTable(), This->fileData->getFileNameTable(), sizeof(uint32_t) + This->fileData->fileNum * fileNameStride);
				memcpy(fileData->getSize(0), This->fileData->getSize(0), This->fileData->fileNum * sizeof(uint32_t));

				for (size_t i = 0; i < This->fileData->fileNum; i++)
				{
					if (*This->fileData->getOffset(i) == 0)
					{
						char filePath[MAX_PATH];
						sprintf_s(filePath, "%s\\%s", dirPath, This->fileData->getFileNameTable()->getName(i));

						HANDLE hFile = CreateFileA(filePath, GENERIC_READ, FILE_SHARE_READ, nullptr, OPEN_EXISTING, FILE_ATTRIBUTE_NORMAL, nullptr);
						if (hFile != INVALID_HANDLE_VALUE)
						{
							ReadFile(hFile, fileData->identifier + dataOffset, *This->fileData->getSize(i), nullptr, nullptr);
							CloseHandle(hFile);
						}
					}
					else
					{
						memcpy(fileData->identifier + dataOffset, This->fileData->identifier + *This->fileData->getOffset(i), *This->fileData->getSize(i));
					}

					*fileData->getOffset(i) = dataOffset;
					dataOffset += alignUp(*This->fileData->getSize(i), 0x10);
				}

				heapFree(This->fileData);
				This->fileData = fileData;
				This->needSize = needSize;
			}
		}
	}

	originalRegisterResource(This);
}

void Fmerge::init()
{
	INSTALL_HOOK(RegisterResource);
}