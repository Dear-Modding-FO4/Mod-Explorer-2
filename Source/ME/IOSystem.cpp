#include <ME/IOSystem.h>
#include <ME/Shop.h>

#include <dinput.h>
#include <imgui/backends/imgui_impl_win32.h>

extern IMGUI_IMPL_API LRESULT WINAPI ImGui_ImplWin32_WndProcHandler(HWND, UINT, WPARAM, LPARAM);

bool ME::IOSystem::Initialize() noexcept
{
	*(uintptr_t*)&getRawInputData = Util::DetourIAT("user32.dll", "GetRawInputData", (uintptr_t)&User32GetRawInputData);
	*(uintptr_t*)&clipCursor = Util::DetourIAT("user32.dll", "ClipCursor", (uintptr_t)&User32ClipCursor);
	*(uintptr_t*)&gameWndProc = Util::DetourJump(REL::Relocation(REL::ID{ 719782, 2228990 }).address(), (uintptr_t)&User32GameWndProc);

	return getRawInputData && clipCursor && gameWndProc;
}

void ME::IOSystem::Release() noexcept
{
	if (getRawInputData)
	{
		Util::DetourIAT("user32.dll", "GetRawInputData", (uintptr_t)&getRawInputData);
		getRawInputData = nullptr;
	}

	if (clipCursor)
	{
		Util::DetourIAT("user32.dll", "ClipCursor", (uintptr_t)&clipCursor);
		clipCursor = nullptr;
	}

	if (gameWndProc)
	{
		Util::DetourRemove(*(uintptr_t*)&gameWndProc);
		gameWndProc = nullptr;
	}
}

bool ME::IOSystem::RegisterKeyboardListener(const std::function<KeyboardEvent>& a_handler) noexcept
{
	if (a_kbs.size() >= a_kbs.max_size())
		return false;
	a_kbs.emplace_back(a_handler);
	return true;
}

uint32_t ME::IOSystem::User32GetRawInputData(void* rawinput, uint32_t cmd, void* data, uint32_t* dataSize, uint32_t headerSize) noexcept
{
	auto shop = Shop::GetSingleton();
	auto result = IOSystem::GetSingleton()->getRawInputData(rawinput, cmd, data, dataSize, headerSize);

	if (!data || (cmd != RID_INPUT))
		return result;

	static auto input = (RAWINPUT*)data;
	static auto kbd = &input->data.keyboard;
	static auto mse = &input->data.mouse;

	switch (input->header.dwType)
	{
	case RIM_TYPEKEYBOARD:
		if (shop->HasOpened())
		{
			// Skip key actions
			std::fill_n((uint8_t*)kbd, sizeof(RAWKEYBOARD), 0);
			std::fill_n((uint8_t*)input, sizeof(RAWINPUT), 0);

			return -1;
		}

		break;
	case RIM_TYPEMOUSE:
		if (shop->HasOpened())
		{
			// Skip mouse actions
			std::fill_n((uint8_t*)mse, sizeof(RAWMOUSE), 0);
			std::fill_n((uint8_t*)input, sizeof(RAWINPUT), 0);

			return -1;
		}
		break;
	default:
		break;
	}

	return result;
}

bool ME::IOSystem::User32ClipCursor(void* pRect) noexcept
{
	return Shop::GetSingleton()->HasOpened() ? false : IOSystem::GetSingleton()->clipCursor(pRect);
}

int64_t ME::IOSystem::User32GameWndProc(void* a_wnd, uint32_t a_msg, int64_t a_wparam, int64_t a_lparam) noexcept
{
	if (ImGui_ImplWin32_WndProcHandler(static_cast<HWND>(a_wnd), a_msg, a_wparam, a_lparam))
		return S_FALSE;

	auto io = IOSystem::GetSingleton();
	auto shop = Shop::GetSingleton();

	switch (a_msg)
	{
	case WM_KEYUP:
	case WM_KEYDOWN:
	case WM_SYSKEYUP:
	case WM_SYSKEYDOWN:
	{
		if (a_wparam == VK_F4)
		{
			auto bShift = (GetAsyncKeyState(VK_SHIFT) & 0x8000) == 0x8000;
			auto bMenu = (GetAsyncKeyState(VK_MENU) & 0x8000) == 0x8000;
			auto bControl = (GetAsyncKeyState(VK_CONTROL) & 0x8000) == 0x8000;

			// ALT + F4 (send system)
			if (!bShift && !bControl && bMenu)
			{
				ClipCursor(nullptr);
				TerminateProcess(GetCurrentProcess(), 0);
				return S_OK;
			}
		}

		io->DoWindowKeyboardEvent((uint8_t)a_wparam, (a_msg == WM_KEYUP) || (a_msg == WM_SYSKEYUP));
		if (shop->HasOpened())
			return S_OK;

		break;
	}
	case WM_MOUSEMOVE:
	case WM_MOUSELEAVE:
	case WM_MOUSEHOVER:
	case WM_MOUSEWHEEL:
	case WM_MOUSEHWHEEL:
	case WM_LBUTTONUP:
	case WM_MBUTTONUP:
	case WM_RBUTTONUP:
	case WM_LBUTTONDOWN:
	case WM_MBUTTONDOWN:
	case WM_RBUTTONDOWN:
	case WM_LBUTTONDBLCLK:
	case WM_MBUTTONDBLCLK:
	case WM_RBUTTONDBLCLK:
	case WM_NCLBUTTONUP:
	case WM_NCMBUTTONUP:
	case WM_NCRBUTTONUP:
	case WM_NCLBUTTONDOWN:
	case WM_NCMBUTTONDOWN:
	case WM_NCRBUTTONDOWN:
	{
		if (shop->HasOpened())
			return S_OK;

		break;
	}
	case WM_ACTIVATE:
		if (a_wparam != WA_INACTIVE) shop->Close();
		break;
	default:
		break;
	}

	return io->gameWndProc(a_wnd, a_msg, a_wparam, a_lparam);
}

void ME::IOSystem::DoWindowKeyboardEvent(uint8_t a_vkCode, bool a_up) const noexcept
{
	for (auto& func : a_kbs)
		func(a_vkCode, a_up);
}
