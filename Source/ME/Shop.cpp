#include <F4SE/Plugin.h>
#include <ME/Shop.h>
#include <ME/IOSystem.h>
#include <ME/Graphics/GraphicsSystem.h>
#include <Windows.h>

bool ME::Shop::Initialize(const F4SE::LoadInterface* a_f4se)
{
	static REX::TOML::Str<> sGeneralHotkey{ "General"sv, "sHotkey"sv, "F10" };
	static F4SE::Impl::F4SEInterface RestoreLoadInterface{};
	static std::once_flag once;
	std::call_once(once, [&]() {
		memcpy(&RestoreLoadInterface, a_f4se, 48 /* OG struct size */);
		(((F4SE::Impl::F4SEInterface*)(&RestoreLoadInterface))->GetSaveFolderName) = Util::GetSaveFolderName;
		// Init
		F4SE::Init((const F4SE::LoadInterface*)(&RestoreLoadInterface));

		auto game_ver = a_f4se->RuntimeVersion();
		REX::INFO("" _PluginName " mod (ver: " VER_FILE_VERSION_STR ") Initializing..."sv);
		REX::INFO("Game version: {}.{}.{}.{}"sv, game_ver.major(), game_ver.minor(), game_ver.patch(), game_ver.build());

		// Load the Config
		const auto config = REX::TOML::SettingStore::GetSingleton();
		config->Init("Data/F4SE/Plugins/" _PluginName ".toml", "Data/F4SE/Plugins/" _PluginName "Custom.toml");
		config->Load();

		// Listen for Messages (to Install PostInit Patches)
		auto MessagingInterface = F4SE::GetMessagingInterface();
		if (MessagingInterface->RegisterListener(F4SEMessageListener))
			REX::INFO("Started Listening for F4SE Message Callbacks."sv);

		hotkey = HotkeySystem::GetSingleton()->StringToHotkey(sGeneralHotkey.GetValue());
		REX::INFO("Started Listening hotkey \"{}\" for show shop menu."sv, 
			HotkeySystem::GetSingleton()->HotkeyToString(hotkey));

		hasInitialize = true;
	});

	return hasInitialize;
}

void ME::Shop::Open() noexcept
{
	if (!hasOpened)
	{
		std::lock_guard guard(lock);

		hasOpened = true;
		// I will definitely restore the cursor restrictions after
		auto rendererData = GraphicsSystem::GetSingleton()->GetRendererData();
		if (rendererData)
		{
			RECT rc{ 
				rendererData->renderWindow[0].windowX, rendererData->renderWindow[0].windowY,
				rendererData->renderWindow[0].windowWidth, rendererData->renderWindow[0].windowHeight
			};

			ClipCursor(&rc);
		}
	}
}

void ME::Shop::Close() noexcept
{
	if (hasOpened)
	{
		std::lock_guard guard(lock);

		hasOpened = false;
	}
}

void ME::Shop::InitializeGameHooks() noexcept
{
	static std::once_flag once;
	std::call_once(once, [&]() {
		if (!IOSystem::GetSingleton()->Initialize())
		{
			REX::CRITICAL("IOSystem::Initialize() return failed.");
			return;
		}

		if (!GraphicsSystem::GetSingleton()->Initialize())
		{
			IOSystem::GetSingleton()->Release();

			REX::CRITICAL("GraphicsSystem::Initialize() return failed.");
			return;
		}

		hasInitializeGameHooks = true;
	});
}

void ME::Shop::InitializeGraphicsAndData() noexcept
{
	if (!hasInitializeGameHooks)
		return;

	static std::once_flag once;
	std::call_once(once, [&]() {
		// TODO

		if (!GraphicsSystem::GetSingleton()->InitializeContinue())
		{
			IOSystem::GetSingleton()->Release();
			GraphicsSystem::GetSingleton()->Release();

			hasInitializeGameHooks = false;

			REX::CRITICAL("GraphicsSystem::InitializeContinue() return failed.");
			return;
		}

		IOSystem::GetSingleton()->RegisterKeyboardListener(GameKeyboardListener);

		hasInitializeGraphicsAndData = true;
	});
}

void ME::Shop::F4SEMessageListener(F4SE::MessagingInterface::Message* a_msg) noexcept
{
	if (!a_msg) return;

	switch (a_msg->type)
	{
	case F4SE::MessagingInterface::kPostLoad:
		Shop::GetSingleton()->InitializeGameHooks();
		break;
	case F4SE::MessagingInterface::kGameLoaded:
		Shop::GetSingleton()->InitializeGraphicsAndData();
		break;
	default:
		break;
	}
}

void ME::Shop::GameKeyboardListener(uint8_t a_vkCode, bool a_up) noexcept
{
	auto shop = Shop::GetSingleton();
	if (a_up)
	{
		if (shop->HasOpened() && (a_vkCode == VK_ESCAPE))
			shop->Close();
		else if (a_vkCode == shop->hotkey.vkCode)
		{
			if (shop->hotkey.modificator)
			{
				if (shop->hotkey.modificator & std::to_underlying(HotkeySystem::Modificator::kCtrl))
				{
					if (!(GetKeyState(VK_CONTROL) & 0x8000))
						return;
				}

				if (shop->hotkey.modificator & std::to_underlying(HotkeySystem::Modificator::kAlt))
				{
					if (!(GetKeyState(VK_MENU) & 0x8000))
						return;
				}

				if (shop->hotkey.modificator & std::to_underlying(HotkeySystem::Modificator::kShift))
				{
					if (!(GetKeyState(VK_SHIFT) & 0x8000))
						return;
				}
			}

			shop->Open();
		}
	}
}
