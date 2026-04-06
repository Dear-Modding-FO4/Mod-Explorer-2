#pragma once

#include <ME/Core.h>
#include <REX/REX/Singleton.h>
#include <functional>
#include <vector>

namespace ME
{
	class IOSystem :
		public REX::Singleton<IOSystem>
	{
	public:
		using KeyboardEvent = void(uint8_t a_vkCode, bool a_up);

		IOSystem() = default;
		virtual ~IOSystem() = default;

		virtual bool Initialize() noexcept;
		virtual void Release() noexcept;

		virtual bool RegisterKeyboardListener(const std::function<KeyboardEvent>& a_handler) noexcept;
	private:
		IOSystem(const IOSystem&) = delete;
		IOSystem(IOSystem&&) = delete;
		IOSystem& operator=(const IOSystem&) = delete;
		IOSystem& operator=(IOSystem&&) = delete;

		void DoWindowKeyboardEvent(uint8_t a_vkCode, bool a_up) const noexcept;

		static uint32_t User32GetRawInputData(void* rawinput, uint32_t cmd, void* data,
			uint32_t* dataSize, uint32_t headerSize) noexcept;
		static bool User32ClipCursor(void* pRect) noexcept;
		static int64_t User32GameWndProc(void* a_wnd, uint32_t a_msg, int64_t a_wparam, int64_t a_lparam) noexcept;
	private:
		decltype(&User32GetRawInputData) getRawInputData{ nullptr };
		decltype(&User32ClipCursor) clipCursor{ nullptr };
		decltype(&User32GameWndProc) gameWndProc{ nullptr };

		std::vector<std::function<KeyboardEvent>> a_kbs;
	};
}