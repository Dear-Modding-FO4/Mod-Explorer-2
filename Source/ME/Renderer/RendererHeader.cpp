#include <ME/Renderer/Controls/RendererControlFrame.h>
#include <ME/Renderer/Controls/RendererControlLabel.h>
#include <ME/Renderer/Controls/RendererControlAnimLogoSpinner.h>
#include <ME/Renderer/RendererHeader.h>
#include <ME/Graphics/GraphicsSystem.h>
#include <ME/Graphics/GraphicsImage.h>
#include <ME/Graphics/GraphicsMath.h>
#include <Resource.h>
#include <imgui.h>
#include <imspinner.h>

namespace ME
{
	extern std::shared_ptr<Graphics::GraphicsFont> DefaultFont;
	extern std::shared_ptr<Graphics::GraphicsFont> DefaultFontBold;
	extern std::shared_ptr<Graphics::GraphicsFont> LogoFontBold;
}

ME::RendererHeader::RendererHeader(REX::W32::ID3D11Device* a_device, REX::W32::ID3D11DeviceContext* a_deviceContext)
{
	frame = std::make_shared<Controls::RendererControlFrame>(nullptr, "HeaderFrame", "Header");
	if (frame)
	{
		logo = std::make_unique<Controls::RendererControlAnimLogoSpinner>(nullptr, "HeaderAnimLogo", "AnimLogo");
		if (logo)
		{
			logo->SetTextColor(IM_COL32(43, 237, 230, 255));
			logo->SetTransparentBackground(true);
			logo->SetFont(LogoFontBold);
			frame->Attach(logo.get());
		}

		quitBtn = std::make_unique<Controls::RendererControlImageButton>(nullptr, "HeaderQuitBtn", "");
		if (quitBtn)
		{
			quitBtn->SetTransparentBackground(true);
			if (!quitBtn->SetImageDefaultByResId(a_device, a_deviceContext, "\"PNG\"",  IDB_PNG2, true))
				REX::ERROR("Quit button load default image failed");
			if (!quitBtn->SetImageHoverByResId(a_device, a_deviceContext, "\"PNG\"", IDB_PNG3, true))
				REX::ERROR("Quit button load hover image failed");
			quitBtn->SetHint("Quit");
			quitBtn->SetFont(DefaultFontBold);
			frame->Attach(quitBtn.get());
		}
	}
}

static bool showwww = false;

void ME::RendererHeader::Execute() const noexcept
{
	auto renderer = GraphicsSystem::GetSingleton();
	auto rendererData = renderer->GetRendererData();
	if (!rendererData) return;
	
	auto clientX = rendererData->renderWindow[0].windowWidth - rendererData->renderWindow[0].windowX;
	auto clientY = rendererData->renderWindow[0].windowHeight - rendererData->renderWindow[0].windowY;

	Graphics::Vec2 vSize = { (float)clientX, std::max(60.f, std::min(120.f, clientY * 0.10f)) };

	if (logo)
	{
		logo->SetPosition({ 40.0f, 0.f });
		logo->SetSize({ vSize.y + 140.f, vSize.y });
	}

	if (quitBtn)
	{
		quitBtn->SetPosition({ (float)clientX - 100.0f, (vSize.y - 34.f) * .5f });
		quitBtn->SetSize({ 34.f, 34.f });
	}

	frame->SetPosition({ .0f, 0.f });
	frame->SetSize(vSize);
	frame->Draw();

	if (!showwww)
		Renderer_ConfirmMessage(showwww, "Man", "Look at the boobs?");
	else
		Renderer_CheckResult(false, "bitch message"sv, 
		 "bitch bitch bitch bitch bitch bitch bitch bitch bitchbitch bitch bitch bitch bitch bitch bitch bitch"
		 " bitchbitch bitch bitch bitch bitch bitch bitch bitch bitchbitch bitch bitch bitch bitch bitch bitch"
		 " bitch bitchbitch bitch bitch bitch bitch bitch bitch bitch bitchbitch bitch bitch bitch bitch bitch bitch bitch bitch"sv);
}