#pragma once

#include <ME/Core.h>
#include <REX/REX/Singleton.h>

namespace ME
{
	class HotkeySystem :
		public REX::Singleton<HotkeySystem>
	{
	public:
		enum class Modificator : uint8_t
		{
			kCtrl	= 1 << 0,
			kAlt	= 1 << 1,
			kShift	= 1 << 2,
		};

		struct Hotkey
		{
			uint8_t modificator{ 0 };
			uint8_t vkCode{ 0 };
		};

		HotkeySystem() = default;
		virtual ~HotkeySystem() = default;

		virtual Hotkey StringToHotkey(const std::string& str) const noexcept;
		virtual std::string HotkeyToString(const Hotkey& hotkey) const noexcept;
	private:
		HotkeySystem(const HotkeySystem&) = delete;
		HotkeySystem(HotkeySystem&&) = delete;
		HotkeySystem& operator=(const HotkeySystem&) = delete;
		HotkeySystem& operator=(HotkeySystem&&) = delete;
	};

}