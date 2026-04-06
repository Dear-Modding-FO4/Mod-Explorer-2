#include <ME/Renderer/RendererCursor.h>
#include <ME/Graphics/GraphicsSystem.h>
#include <ME/Graphics/GraphicsShaders.h>
#include <ME/Graphics/GraphicsImage.h>
#include <ME/Graphics/GraphicsMath.h>
#include <Resource.h>
#include <imgui.h>

ME::RendererCursor::RendererCursor(REX::W32::ID3D11Device* a_device, REX::W32::ID3D11DeviceContext* a_deviceContext) :
	GraphicsObject()
{
	image = std::make_shared<GraphicsImage>(a_device, a_deviceContext);
	if (!image || !image->LoadFromResource("\"PNG\"", IDB_PNG1, true))
		REX::ERROR("Can't load cursor texture"sv);
}

void ME::RendererCursor::Execute() const noexcept
{
	auto TexDesc = image->GetTexture()->GetDesc();
	Graphics::Vec2 vRightButtom = { (float)TexDesc->width, (float)TexDesc->height };
	ImGuiIO& io = ImGui::GetIO();
	Graphics::Vec2 vLeftTop = io.MousePos;
	ImDrawList* pDrawList = ImGui::GetForegroundDrawList();

	switch (GraphicsSystem::GetSingleton()->GetRatioEnum())
	{
	case GraphicsSystem::Ratio::kRatio_4x3:
		vRightButtom *= .5f;
		//pDrawList->AddText({ 0,0 }, IM_COL32(255, 255, 255, 255), "kRatio_4x3");
		break;
	case GraphicsSystem::Ratio::kRatio_16x9:
		vRightButtom *= .75f;
		//pDrawList->AddText({ 0,0 }, IM_COL32(255, 255, 255, 255), "kRatio_16x9");
		break;
	default:
		break;
	}

	vRightButtom += vLeftTop;
	pDrawList->AddImage((ImTextureID)(intptr_t)image->GetResourceView()->Get(), vLeftTop, vRightButtom);
}