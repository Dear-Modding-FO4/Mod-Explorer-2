#include <ME/Renderer/RendererBackground.h>
#include <ME/Graphics/GraphicsSystem.h>
#include <ME/Graphics/GraphicsMath.h>
#include <imgui.h>

ME::RendererBackground::RendererBackground() :
	GraphicsObject()
{}

void ME::RendererBackground::Execute() const noexcept
{
	constexpr static auto basementColor = IM_COL32(11, 15, 17, 255);
	constexpr static auto basementHitColor = IM_COL32(13, 19, 21, 255);

	auto drawList = ImGui::GetBackgroundDrawList();
	auto renderer = GraphicsSystem::GetSingleton();
	auto rendererData = renderer->GetRendererData();

	if (!drawList || !rendererData) return;

	Graphics::Vec2 wndSize
	{
		(float)(rendererData->renderWindow[0].windowWidth - rendererData->renderWindow[0].windowX),
		(float)(rendererData->renderWindow[0].windowHeight - rendererData->renderWindow[0].windowY)
	};

	drawList->AddRectFilled({ .0f, 0.f }, wndSize, basementColor);
	
	for (float j = .0f; j < wndSize.x; j += 5.f)
		for (float k = .0f; k < wndSize.y; k += 5.f)
			drawList->AddRectFilled({ j, k }, { j + 1, k + 1.f }, basementHitColor);
}
