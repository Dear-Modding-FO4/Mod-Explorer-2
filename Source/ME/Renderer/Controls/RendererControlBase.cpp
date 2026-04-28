#include <ME/Renderer/Controls/RendererControlBase.h>

ME::Controls::RendererControlBase::RendererControlBase(RendererControlBase* a_parent, const char* a_class, const char* a_caption) :
	IRendererControlBase(a_parent, a_class, a_caption)
{}

void ME::Controls::RendererControlBase::Draw() const noexcept
{
	if (parent && (size.x > .1f) && (size.y > .1f))
	{
		auto curFont = GetFont();
		if (curFont && curFont->Handle()) curFont->Push();

		ImGui::SetCursorPos(position);
		ImGui::PushClipRect(position, position + size, false);

		if (!transparentBg && (colorbg.w > .01f))
		{
			auto drawList = ImGui::GetWindowDrawList();
			drawList->AddRectFilled(position, position + size, ImColor{ colorbg });
		}

		DoDraw();

		ImGui::PopClipRect();

		if (enabled && hint.length() && ImGui::IsItemHovered(ImGuiHoveredFlags_ForTooltip))
		{
			auto& io = ImGui::GetIO();
			auto pos = io.MousePos;
			pos.y += 22.f;
			ImGui::SetNextWindowPos(pos, ImGuiCond_Always, ImVec2{ .5f, -1.f });
			// Full transparent
			ImGui::PushStyleColor(ImGuiCol_Text, ImVec4{ .0f, .0f, .0f, 0.f });
			ImGui::PushStyleColor(ImGuiCol_PopupBg, ImVec4{ ImColor{ IM_COL32(170, 255, 252, 0) } });
			// uses like as padding
			ImGui::PushStyleVar(ImGuiStyleVar_WindowRounding, 16.f);
			// show tooltips
			if(ImGui::BeginItemTooltip())
			{
				// Input text
				ImGui::PushTextWrapPos(ImGui::GetFontSize() * 64.f);
				ImGui::TextUnformatted(hint.c_str());

				auto pos = Graphics::Vec2{ ImGui::GetWindowPos() };
				auto size = Graphics::Vec2{ ImGui::GetWindowSize() };

				auto drawList = ImGui::GetWindowDrawList();
				drawList->AddRectFilledMultiColorEx(pos/* + ImVec2{ .0f, 2.f }*/, pos + size/* - ImVec2{ .0f, 2.f }*/,
					IM_COL32(62, 175, 109, 64), IM_COL32(62, 175, 109, 64), 
					IM_COL32(62, 175, 109, 32), IM_COL32(62, 175, 109, 32), 
					2.f, ImDrawFlags_RoundCornersAll);
				
				drawList->AddRect(pos, pos + size, IM_COL32(62, 175, 109, 96), 
					2.f, ImDrawFlags_RoundCornersAll);
				
				auto sizeText = ImGui::CalcTextSize(hint.c_str());
				drawList->AddText(pos + ((size - sizeText) * .5f) - ImVec2{ .0f, 2.f }, IM_COL32(62, 175, 109, 255), hint.c_str());
				
				ImGui::PopTextWrapPos();
				ImGui::EndTooltip();
			}

			ImGui::PopStyleVar();
			ImGui::PopStyleColor(2);
		}

		if (curFont && curFont->Handle()) curFont->Pop();
	}
}
