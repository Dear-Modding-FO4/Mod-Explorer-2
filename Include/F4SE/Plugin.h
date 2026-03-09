#pragma once

#include <F4SE/F4SE.h>

#include <cstddef>
#include <cstdint>

#include <resource_version2.h>
#include <ME/Util.h>
#include <ME/Main.h>

#define MAKE_EXE_VERSION_EX(major, minor, build, sub)	((((major) & 0xFF) << 24) | (((minor) & 0xFF) << 16) | (((build) & 0xFFF) << 4) | ((sub) & 0xF))
#define MAKE_EXE_VERSION(major, minor, build)			MAKE_EXE_VERSION_EX(major, minor, build, 0)

#define _PluginName         "Mod Explorer 2"
#define _PluginAuthor       "Dear Modding FO4 Team"

enum class F4SEAddressIndependence : std::uint32_t
{
    // set this if you exclusively use signature matching to find your addresses and have NO HARDCODED ADDRESSES
    // the F4SE code does not use signature matching, so calling functions in the F4SE headers is not safe with this flag
    kSignatures = 1 << 0,

    // set this if you are using a 1.10.980+ version of the Address Library
    kAddressLibrary_1_10_980 = 1 << 1,

    // set this if you are using a 1.11.137+ version of the Address Library
    kAddressLibrary_1_11_137 = 1 << 2,
};

enum class F4SEStructureIndependence : std::uint32_t
{
    // set this if your plugin doesn't use any game structures
    kNoStructs = 1 << 0,

    // works with the structure layout in 1.10.980+
    k1_10_980Layout = 1 << 1,

    // works with the structure layout in 1.11.137+
    k1_11_137Layout = 1 << 2,
};

// F4SE NP requirement
F4SE_EXPORT auto F4SEPlugin_Version = []() noexcept
{
    F4SE::PluginVersionData data
    {
        F4SE::PluginVersionData::kVersion,
        MAKE_EXE_VERSION(VERSION_MAJOR, VERSION_MINOR, VERSION_BUILD),
        _PluginName,
        _PluginAuthor,
        std::to_underlying(F4SEAddressIndependence::kAddressLibrary_1_10_980) |
        std::to_underlying(F4SEAddressIndependence::kAddressLibrary_1_11_137),
        std::to_underlying(F4SEStructureIndependence::k1_10_980Layout) |
        std::to_underlying(F4SEStructureIndependence::k1_11_137Layout),
        // compatibleVersions - no need
        {},
        // xseMinimum - no need
        0,
        // reserved
        0,
        0,
        {}
    };

    return data;
}();

// For F4SE OG
F4SE_EXPORT bool F4SEAPI F4SEPlugin_Query(const F4SE::QueryInterface* a_f4se, F4SE::PluginInfo* a_info);
// No supported OG
F4SE_PLUGIN_PRELOAD(const F4SE::PreLoadInterface* a_preloadf4se);
F4SE_PLUGIN_LOAD(const F4SE::LoadInterface* a_f4se);