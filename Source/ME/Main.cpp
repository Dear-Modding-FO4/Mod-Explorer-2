#include <ME/Main.h>
#include <ME/Util.h>

#include <F4SE/Plugin.h>
#include <F4SE/F4SE.h>

//#define ME_DEBUGBREAK 1
#if ME_DEBUGBREAK
#	include <Windows.h>
#endif

namespace ME
{
	static bool isPreloadInit{ false }, isInit{ false };
	static F4SE::Impl::F4SEInterface RestoreLoadInterface;

	// Added F4SE 0.7.1+
	[[nodiscard]] inline static const char* F4SEAPI F4SEGetSaveFolderName() noexcept
	{
		return ME::GetSaveFolderName();
	}

	static void F4SEMessageListener(F4SE::MessagingInterface::Message* a_msg) noexcept
	{
		// TODO
	}

	static bool PreloadInitUnsafe(const F4SE::PreLoadInterface* a_preloadf4se) noexcept
	{
		if (isPreloadInit)
			return true;

		static std::once_flag once;
		std::call_once(once, [&]() {
#if ME_DEBUGBREAK
			MessageBoxA(nullptr, "Debugbreak preload stage", "DEBUG", MB_OK | MB_TOPMOST);
#endif
			// Preload Init
			F4SE::Init(a_preloadf4se);

			auto game_ver = a_preloadf4se->RuntimeVersion();
			REX::INFO("" _PluginName " mod (ver: " VER_FILE_VERSION_STR ") Initializing..."sv);
			REX::INFO("Game version: {}.{}.{}.{}"sv, game_ver.major(), game_ver.minor(), game_ver.patch(), game_ver.build());

			// TODO

			isPreloadInit = true;
			});

		return isPreloadInit;
	}

	static bool InitUnsafe(const F4SE::LoadInterface* a_loadf4se) noexcept
	{
		if (isInit)
			return true;

		static std::once_flag once;
		std::call_once(once, [&]() {
#if ME_DEBUGBREAK
			MessageBoxA(nullptr, "Debugbreak load stage", "DEBUG", MB_OK | MB_TOPMOST);
#endif
			memcpy(&RestoreLoadInterface, a_loadf4se, 48 /* OG struct size */);
			(((F4SE::Impl::F4SEInterface*)(&RestoreLoadInterface))->GetSaveFolderName) = F4SEGetSaveFolderName;
			// Init
			F4SE::Init((const F4SE::LoadInterface*)(&RestoreLoadInterface));

			if (!isPreloadInit)
			{
				auto game_ver = a_loadf4se->RuntimeVersion();
				REX::INFO("" _PluginName " mod (ver: " VER_FILE_VERSION_STR ") Initializing..."sv);
				REX::INFO("Game version: {}.{}.{}.{}"sv, game_ver.major(), game_ver.minor(), game_ver.patch(), game_ver.build());
			}

			// Listen for Messages (to Install PostInit Patches)
			auto MessagingInterface = F4SE::GetMessagingInterface();
			if (MessagingInterface->RegisterListener(F4SEMessageListener))
				REX::INFO("Started Listening for F4SE Message Callbacks."sv);

			// TODO

			isInit = true;
			});

		return isInit;
	}

	bool PreloadInit(const F4SE::PreLoadInterface* a_preloadf4se)
	{
		__try
		{
			return PreloadInitUnsafe(a_preloadf4se);
		}
		__except (1)
		{
			return false;
		}
	}

	bool Init(const F4SE::LoadInterface* a_loadf4se)
	{
		__try
		{
			return InitUnsafe(a_loadf4se);
		}
		__except (1)
		{
			return false;
		}
	}
}