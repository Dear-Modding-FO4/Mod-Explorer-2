#include <F4SE/Plugin.h>
#include <ME/Shop.h>

#include <Windows.h>

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

F4SE_EXPORT bool F4SEAPI F4SEPlugin_Query(const F4SE::QueryInterface* a_f4se, F4SE::PluginInfo* a_info)
{
    if (!a_f4se)
        return false;
    if (!a_info)
        return false;

    if (a_f4se->RuntimeVersion() != REL::Version{ 1, 10, 163, 0 } /*RUNTIME_VERSION_1_10_163*/)
        return false;

    a_info->infoVersion = F4SE::PluginInfo::kVersion;
    a_info->version = MAKE_EXE_VERSION(VERSION_MAJOR, VERSION_MINOR, VERSION_BUILD);
    a_info->name = _PluginName;

    if (!std::filesystem::exists(ME::Util::GetRuntimeDirectory() + "Data\\F4SE\\Plugins\\version-1-10-163-0.bin"))
    {
        MessageBoxA(nullptr, "" _PluginName ": disabled, address library needs to be updated", "Warnings",
            MB_OK | MB_ICONWARNING | MB_TOPMOST);

        return false;
    }

    return true;
}

F4SE_PLUGIN_LOAD(const F4SE::LoadInterface* a_f4se)
{
    __try
    {
        return ME::Shop::GetSingleton()->Initialize(a_f4se);
    }
    __except (1)
    {
        return false;
    }
}