#pragma once
#include <cstdio>
#include <string>
#include <vector>

static const int ModLoaderVer = 1;
static const int GameVer = 0;

using std::string;
using std::vector;

struct Mod
{
	const char* Name;
	const char* Path;
};

struct ModInfo
{
	vector<Mod*>* ModList;
	Mod* CurrentMod;
};

typedef void(__cdecl* ModInitEvent)(ModInfo* modInfo);
typedef void(__cdecl* ModCallEvent)();

struct VersionInfo
{
	int LoaderVersion;
	int GameVersion;
};