#include <ME/Renderer/Controls/RendererControlFrame.h>
#include <imgui.h>
#include <format>

ME::Controls::RendererControlFrame::RendererControlFrame(RendererControlBase* a_parent, const char* a_class, const char* a_caption) :
	IRendererControlBase((IRendererControlBase*)a_parent, a_class, a_caption)
{
	colorbg = BasementColor;
	colorbg.w = 0.02f;
	colorBorder = BasementColor;
	colorBorder.w = 0.07f;
}

void ME::Controls::RendererControlFrame::Draw() const noexcept
{
	ImGui::PushStyleColor(ImGuiCol_WindowBg, colorbg);
	ImGui::PushStyleColor(ImGuiCol_Border, colorBorder);
	ImGui::PushStyleColor(ImGuiCol_Text, colorText);

	if (round)
		ImGui::PushStyleVar(ImGuiStyleVar_WindowRounding, roundSize);
	else
		ImGui::PushStyleVar(ImGuiStyleVar_WindowRounding, .0f);

	if (border)
		ImGui::PushStyleVar(ImGuiStyleVar_WindowBorderSize, borderSize);
	else
		ImGui::PushStyleVar(ImGuiStyleVar_WindowBorderSize, .0f);

	auto curFont = GetFont();
	if (curFont && curFont->Handle()) curFont->Push();

	ImGui::SetNextWindowPos(position);
	ImGui::SetNextWindowSize(size);

	ImGuiWindowFlags flags = ImGuiWindowFlags_NoDecoration | ImGuiWindowFlags_NoMove | ImGuiWindowFlags_NoSavedSettings;
	if (colorbg.w <= .01f) flags |= ImGuiWindowFlags_NoBackground;

	ImGui::Begin(std::format("{}###{}"sv, caption, className).c_str(), nullptr, flags);
	{
		std::lock_guard locker(const_cast<RendererControlFrame*>(this)->lock);
		for (auto child : childs)
			if (child)
				child->Draw();
	}
	ImGui::End();

	if (curFont && curFont->Handle()) curFont->Pop();

	ImGui::PopStyleVar(2);
	ImGui::PopStyleColor(3);
}