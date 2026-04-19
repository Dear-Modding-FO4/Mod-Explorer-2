#include <ME/Renderer/Controls/RendererControlLabel.h>
#include <imgui.h>

void ME::Controls::RendererControlLabel::DoDraw() const noexcept
{
	if (enabled)
		ImGui::TextColored(colorText, caption.c_str());
	else
	{
		ImGui::PushStyleColor(ImGuiCol_TextDisabled, colorDisabledText);
		ImGui::TextDisabled(caption.c_str());
		ImGui::PopStyleColor();
	}
}

ME::Controls::RendererControlLabel::RendererControlLabel(RendererControlBase* a_parent, const char* a_class, const char* a_caption) :
	RendererControlBase(a_parent, a_class, a_caption)
{
	colorText = BasementColor;
}