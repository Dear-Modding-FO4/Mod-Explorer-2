#pragma once

#include <ME/Renderer/Controls/RendererControlBase.h>
#include <ME/Renderer/Controls/IRendererControlButton.h>
#include <ME/Graphics/GraphicsImage.h>
#include <array>
#include <memory>

namespace ME
{
	namespace Controls
	{
		class RendererControlImageButton final :
			public RendererControlBase,
			public IRendererControlButton
		{
			std::array<std::unique_ptr<ME::GraphicsImage>, 3> images{};

			void DrawButtonStage(uint8_t a_state) const noexcept;
		protected:
			void DoDraw() const noexcept override;
		public:
			RendererControlImageButton(RendererControlBase* a_parent, const char* a_class, const char* a_caption);
			~RendererControlImageButton() override = default;

			virtual bool SetImageDefaultByResId(REX::W32::ID3D11Device* a_device, REX::W32::ID3D11DeviceContext* a_deviceContext,
				const char* a_type, uint32_t a_resourceID, bool a_wic = false) noexcept;
			virtual bool SetImageHoverByResId(REX::W32::ID3D11Device* a_device, REX::W32::ID3D11DeviceContext* a_deviceContext,
				const char* a_type, uint32_t a_resourceID, bool a_wic = false) noexcept;
			virtual bool SetImagePressedByResId(REX::W32::ID3D11Device* a_device, REX::W32::ID3D11DeviceContext* a_deviceContext,
				const char* a_type, uint32_t a_resourceID, bool a_wic = false) noexcept;
		};
	}
}