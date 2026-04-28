#include <ME/Graphics/GraphicsSystem.h>
#include <ME/Graphics/GraphicsFont.h>
#include <ME/Graphics/GraphicsManager.h>
#include <ME/Renderer/RendererCursor.h>
#include <ME/Renderer/RendererBackground.h>
#include <ME/Renderer/RendererHeader.h>
#include <ME/Renderer/RendererWarningMessage.h>
#include <ME/Renderer/RendererConfirmMessage.h>
#include <ME/Shop.h>
#include <Windows.h>
#include <Resource.h>

#include <imgui.h>
#include <imgui_internal.h>
#include <imgui/backends/imgui_impl_dx11.h>
#include <imgui/backends/imgui_impl_win32.h>

#undef ERROR
#undef MEM_RELEASE
#undef MAX_PATH

namespace ME
{
	std::shared_ptr<Graphics::GraphicsFont> DefaultFont;
	std::shared_ptr<Graphics::GraphicsFont> DefaultFontBold;
	std::shared_ptr<Graphics::GraphicsFont> LogoFontBold;
	std::shared_ptr<Graphics::GraphicsFont> UserFont;
	std::shared_ptr<Graphics::GraphicsFont> UserFontBold;
	std::shared_ptr<Graphics::GraphicsFont> CaptionMessageFont;
	std::shared_ptr<Graphics::GraphicsFont> DefaultMessageFont;
	std::shared_ptr<Graphics::GraphicsFont> DefaultTextMessageFont;
	std::shared_ptr<Graphics::GraphicsFont> UserCaptionMessageFont;
	std::shared_ptr<Graphics::GraphicsFont> UserDefaultTextMessageFont;

	static REX::TOML::Str<> sGeneralFontDefault{ "General"sv, "sFontDefault"sv, "tahoma.ttf" };
	static REX::TOML::Str<> sGeneralFontBold{ "General"sv, "sFontBold"sv, "tahomabd.ttf" };
}

bool ME::GraphicsSystem::Initialize() noexcept
{
	// Hook draw mode
	*(uintptr_t*)&subInRenderWndSwap = Util::DetourCall(REL::Relocation(REL::ID{ 1075087, 2228913 },
		REL::Offset{ 0xE3 }).address(), (uintptr_t)&GameSubInRenderWndSwap);

	// Hook draw frame
	*(uintptr_t*)&rendererUI = REL::Relocation(REL::ID{ 386550, 2222551 }).address();
	*(uintptr_t*)&rendererUIRestore = Util::DetourCall(REL::Relocation(REL::ID{ 1075087, 2228913 }, 
		REL::Offset{ 0x2AA, 0x28D }).address(), (uintptr_t)&GameRendererUI);

	return subInRenderWndSwap && rendererUI && rendererUIRestore;
}

bool ME::GraphicsSystem::InitializeContinue() noexcept
{
	rendererData = (RE::BSGraphics::RendererData*)REL::Relocation(REL::ID{ 235166, 2704527 }).address();
	if (!rendererData) return false;

	// Make process DPI aware and obtain main monitor scale
	ImGui_ImplWin32_EnableDpiAwareness();

	// Setup Dear ImGui context
	IMGUI_CHECKVERSION();
	ImGui::CreateContext();
	auto& io = ImGui::GetIO();
	io.ConfigFlags |= ImGuiConfigFlags_NavEnableKeyboard;     // Enable Keyboard Controls
	io.ConfigFlags |= ImGuiConfigFlags_NavEnableGamepad;      // Enable Gamepad Controls

	// Setup Dear ImGui style
	ImGui::StyleColorsDark();

	// For a normal monitor, the width is greater than the height
	horzDisplay = rendererData->renderWindow[0].windowHeight > rendererData->renderWindow[0].windowWidth;

	if (horzDisplay)
		ratio = (float)rendererData->renderWindow[0].windowHeight / (float)rendererData->renderWindow[0].windowWidth;
	else
		ratio = (float)rendererData->renderWindow[0].windowWidth / (float)rendererData->renderWindow[0].windowHeight;

	if (ratio >= 2.2)
		ratioEnum = Ratio::kRatio_21x9;
	else if (ratio >= 1.7)
		ratioEnum = Ratio::kRatio_16x9;
	else if (ratio >= 1.2)
		ratioEnum = Ratio::kRatio_4x3;

	DefaultFont					= std::make_shared<Graphics::GraphicsFont>();
	DefaultFontBold				= std::make_shared<Graphics::GraphicsFont>();
	LogoFontBold				= std::make_shared<Graphics::GraphicsFont>();
	UserFont					= std::make_shared<Graphics::GraphicsFont>();
	UserFontBold				= std::make_shared<Graphics::GraphicsFont>();
	CaptionMessageFont			= std::make_shared<Graphics::GraphicsFont>();
	DefaultMessageFont			= std::make_shared<Graphics::GraphicsFont>();
	DefaultTextMessageFont		= std::make_shared<Graphics::GraphicsFont>();
	UserCaptionMessageFont		= std::make_shared<Graphics::GraphicsFont>();
	UserDefaultTextMessageFont	= std::make_shared<Graphics::GraphicsFont>();

	float fontSize = 14.f;
	switch (ratioEnum)
	{
	case ME::GraphicsSystem::Ratio::kRatio_16x9:
		fontSize = 16.f;
		break;
	case ME::GraphicsSystem::Ratio::kRatio_21x9:
		fontSize = 18.f;
		break;
	default:
		break;
	}

	if (!DefaultFont->OpenFromFile("tahoma.ttf", fontSize))
	{
		REX::ERROR("Font \"tahoma.ttf\" no found");
		DefaultFont.reset();
	}

	if (!DefaultFontBold->OpenFromFile("tahomabd.ttf", fontSize))
	{
		REX::ERROR("Font \"tahomabd.ttf\" no found");
		DefaultFontBold.reset();
	}

	if (!LogoFontBold->OpenFromFile("tahomabd.ttf", fontSize * 1.5f))
	{
		REX::ERROR("Font \"tahomabd.ttf\" no found");
		LogoFontBold.reset();
	}

	if (!CaptionMessageFont->OpenFromResource("\"FONT\"", IDB_FONTBASEB, 64))
	{
		REX::ERROR("Font \"{}\" no found", IDB_FONTBASEB);
		CaptionMessageFont.reset();
	}

	if (!DefaultMessageFont->OpenFromResource("\"FONT\"", IDB_FONTBASEN, 24))
	{
		REX::ERROR("Font \"{}\" no found", IDB_FONTBASEN);
		DefaultMessageFont.reset();
	}

	if (!DefaultTextMessageFont->OpenFromResource("\"FONT\"", IDB_FONTBASEN, 18))
	{
		REX::ERROR("Font \"{}\" no found", IDB_FONTBASEB);
		DefaultTextMessageFont.reset();
	}

	if (!UserCaptionMessageFont->OpenFromFile(sGeneralFontDefault.GetValue(), 24))
	{
		REX::ERROR("Font \"{}\" no found", IDB_FONTBASEN);
		UserCaptionMessageFont.reset();
	}

	if (!UserDefaultTextMessageFont->OpenFromFile(sGeneralFontDefault.GetValue(), 18))
	{
		REX::ERROR("Font \"{}\" no found", IDB_FONTBASEB);
		UserDefaultTextMessageFont.reset();
	}

	auto& style = ImGui::GetStyle();

	style.WindowBorderSize = 0.f;
	style.ChildBorderSize = 0.f;
	style.PopupBorderSize = 0.f;
	style.FrameBorderSize = 0.f;
	style.WindowRounding = 12.f;
	style.PopupRounding = 8.f;
	style.AntiAliasedLines = true;

	style.Colors[ImGuiCol_PopupBg] = ImVec4(.07f, .07f, .09f, 0.6f);
	style.Colors[ImGuiCol_ModalWindowDimBg] = { .0f, .0f, .0f, .8f };

	// Create renderer obj mandatory
	GraphicsManager::GetSingleton()->Add(std::make_shared<RendererBackground>());
	GraphicsManager::GetSingleton()->Add(std::make_shared<RendererHeader>(rendererData->device, rendererData->context));

	// TODO

	GraphicsManager::GetSingleton()->Add(std::make_shared<RendererCursor>(rendererData->device, rendererData->context));
	GraphicsManager::GetSingleton()->Add(std::make_shared<RendererConfirmMessage>());
	GraphicsManager::GetSingleton()->Add(std::make_shared<RendererWarningMessage>());

	// Setup Platform/Renderer backends
	return 
		ImGui_ImplWin32_Init(rendererData->renderWindow[0].hwnd) &&
		ImGui_ImplDX11_Init((ID3D11Device*)rendererData->device, (ID3D11DeviceContext*)rendererData->context);
}

void ME::GraphicsSystem::Release() noexcept
{
	auto shop = Shop::GetSingleton();
	shop->Lock();

	if (subInRenderWndSwap)
	{
		Util::DetourRemove(*(uintptr_t*)&subInRenderWndSwap);
		subInRenderWndSwap = nullptr;
	}

	if (rendererUIRestore)
	{
		Util::DetourRemove(*(uintptr_t*)&rendererUIRestore);
		rendererUIRestore = nullptr;
	}

	// Cleanup
	ImGui_ImplDX11_Shutdown();
	ImGui_ImplWin32_Shutdown();
	ImGui::DestroyContext();

	shop->Unlock();
}

void* ME::GraphicsSystem::GameSubInRenderWndSwap(void* _this) noexcept
{
	if (Shop::GetSingleton()->HasOpened())
		// Draw menu mode ??? idk black screen...
		return nullptr;
	// Return default
	return Util::ThisVirtualCall<void*>(0x460, _this);
}

void ME::GraphicsSystem::GameRendererUI(uint32_t a_unk) noexcept
{
	auto shop = Shop::GetSingleton();
	auto graphics = GraphicsSystem::GetSingleton();

	shop->Lock();
	if (!shop->HasOpened())
	{
		shop->Unlock();
		// Renderer normal
		graphics->rendererUI(a_unk);
		return;
	}
	if (!shop->HasInitializeGraphicsAndData())
	{
		shop->Unlock();
		// fatal
		return;
	}

	// -------------------
	// RENDER
	// -------------------

	if (!graphics->rendererData)
	{
		shop->Unlock();
		// fatal
		return;
	}

	REX::W32::ID3D11RenderTargetView* pRenderTargetViews{ nullptr };
	REX::W32::ID3D11DepthStencilView* pDepthStencilView{ nullptr };
	graphics->rendererData->context->OMGetRenderTargets(0, &pRenderTargetViews, &pDepthStencilView);

	constexpr static float bg[4]{ 0.0f, 0.0f, 0.0f, 0.0f };
	graphics->rendererData->context->ClearRenderTargetView(pRenderTargetViews, bg);

	/*POINT ps{};
	if (GetCursorPos(&ps))
	{
		auto& io = ImGui::GetIO();
		io.MousePos = ImVec2{ (float)ps.x, (float)ps.y };
	}*/

	ImGui_ImplDX11_NewFrame();
	ImGui_ImplWin32_NewFrame();
	ImGui::NewFrame();

	GraphicsManager::GetSingleton()->Execute();

	ImGui::Render();
	ImGui_ImplDX11_RenderDrawData(ImGui::GetDrawData());

	shop->Unlock();
}
