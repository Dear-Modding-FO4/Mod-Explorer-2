#include <ME/Renderer/Controls/IRendererControlBase.h>

ME::Controls::IRendererControlBase::IRendererControlBase(IRendererControlBase* a_parent, const char* a_class, const char* a_caption) :
	parent(a_parent), className(a_class), caption(a_caption)
{}