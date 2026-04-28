#pragma once

#include <F4SE/F4SE.h>
#include <F4SE/Impl/PCH.h>
#include <F4SE/IBufferStream.h>
#include <REX/W32.h>
#include <ME/Util.h>

namespace ME
{
	class Shop;
	class IOSystem;
	class GraphicsSystem;
	class HotkeySystem;

	class BaseShader;
	class ObjectShader;
	class ResourceView;
	class TextureShader;
	class BufferShader;
	class UnorderedAccessView;
	class SamplerState;
	class RenderTargetView;
	class DepthStencilView;
	class Shader;
	class PixelShader;
	class VertexShader;
	class ComputeShader;

	class GraphicsObject;
	class GraphicsManager;
	class GraphicsImage;

	class RendererCursor;

	namespace Controls
	{
		class IRendererControlBase;
		class IRendererControlChild;
		class IRendererControlButton;
		class RendererControlBase;
		class RendererControlFrame;
		class RendererControlLabel;
		class RendererControlAnimLogoSpinner;
		class RendererControlImageButton;
	}

	extern bool Renderer_CheckResult(bool a_condition, const std::string_view& a_shortMessage, const std::string_view& a_message) noexcept;
	extern void Renderer_ConfirmMessage(bool& a_result, const std::string_view& a_caption, const std::string_view& a_message) noexcept;
}