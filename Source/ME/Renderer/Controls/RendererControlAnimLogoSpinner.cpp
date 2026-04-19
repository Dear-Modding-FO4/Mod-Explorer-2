#include <ME/Renderer/Controls/RendererControlAnimLogoSpinner.h>
#include <imgui.h>
#include <imspinner.h>

void ME::Controls::RendererControlAnimLogoSpinner::DoDraw() const noexcept
{
	auto radius2 = std::min(size.x, size.y);
	radius2 -= fmodf(radius2, 12.f);

	auto height = radius2 * .25f;
	ImGui::SetCursorPos(position + ImVec2{ height, height });
	ImSpinner::SpinnerTwinBlocks(std::format("{}###{}"sv, caption, className).c_str(), height, radius2 / 12.f,
		{ colorText.x, colorText.y, colorText.z, .16f }, { colorText.x, colorText.y, colorText.z, 1.f }, 4.5);

	height = ImGui::CalcTextSize("A\nA").y;
	ImGui::SetCursorPos(position + ImVec2{ radius2, (size.y - height - 4.f) * .5f });
	ImGui::TextColored({ colorText.x, colorText.y, colorText.z, .8f }, "Perchik71");
	ImGui::SetCursorPosX(position.x + radius2);
	ImGui::TextColored({ colorText.x, colorText.y, colorText.z, 1.f }, "Mod Explorer");
}

ME::Controls::RendererControlAnimLogoSpinner::RendererControlAnimLogoSpinner(RendererControlBase* a_parent,
	const char* a_class, const char* a_caption) :
	RendererControlBase(a_parent, a_class, a_caption)
{
	colorText = BasementColor;
}