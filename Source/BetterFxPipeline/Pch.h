#pragma once

#define WIN32_LEAN_AND_MEAN

#include <Windows.h>
#include <detours.h>

#include <boost/shared_ptr.hpp>

#include <array>
#include <cstdint>
#include <cstdio>

#include <INIReader.h>

#include <Helpers.h>

#include "DX_PATCH.h"
#include "Types.h"
#include "Functions.h"

// BlueBlur
#include <Hedgehog.h>
#include <Sonic.h>