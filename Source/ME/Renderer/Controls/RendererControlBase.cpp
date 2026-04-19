#include <ME/Renderer/Controls/RendererControlBase.h>

ME::Controls::RendererControlBase::RendererControlBase(RendererControlBase* a_parent, const char* a_class, const char* a_caption) :
	IRendererControlBase(a_parent, a_class, a_caption)
{}

void ME::Controls::RendererControlBase::Draw() const noexcept
{
	if (parent && (size.x > .1f) && (size.y > .1f))
	{
		ImGui::SetCursorPos(position);
		ImGui::PushClipRect(position, position + size, false);

		if (!transparentBg && (colorbg.w > .01f))
		{
			auto drawList = ImGui::GetWindowDrawList();
			drawList->AddRectFilled(position, position + size, ImColor{ colorbg });
		}

		auto curFont = GetFont();
		if (curFont && curFont->Handle()) curFont->Push();

		DoDraw();

		if (curFont && curFont->Handle()) curFont->Pop();
		ImGui::PopClipRect();
	}
}
