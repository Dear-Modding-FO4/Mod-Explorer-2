#include <ME/Renderer/Controls/IRendererControlButton.h>

void ME::Controls::IRendererControlButton::DoHovered() const noexcept
{
	if (OnHovered)
		OnHovered((IRendererControlBase*)this);
}

void ME::Controls::IRendererControlButton::DoClicked() const noexcept
{
	if (OnClicked)
		OnClicked((IRendererControlBase*)this);
}
