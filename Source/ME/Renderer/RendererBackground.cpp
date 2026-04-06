#include <ME/Renderer/RendererBackground.h>
#include <ME/Graphics/GraphicsSystem.h>
#include <ME/Graphics/GraphicsMath.h>
#include <imgui.h>

ME::RendererBackground::RendererBackground() :
	GraphicsObject()
{


}

void ME::RendererBackground::Execute() const noexcept
{
	auto drawList = ImGui::GetBackgroundDrawList();
	auto renderer = GraphicsSystem::GetSingleton();
	auto rendererData = renderer->GetRendererData();

	if (!drawList || !rendererData) return;

	Graphics::Vec2 wndSize
	{
		(float)(rendererData->renderWindow[0].windowWidth - rendererData->renderWindow[0].windowX),
		(float)(rendererData->renderWindow[0].windowHeight - rendererData->renderWindow[0].windowY)
	};

	drawList->AddRectFilledMultiColor({ .0f, 0.f }, wndSize,
		IM_COL32(255, 0, 0, 255), IM_COL32(255, 0, 128, 255),
		IM_COL32(255, 0, 255, 255), IM_COL32(255, 128, 0, 255));
}
