#pragma once

#include <ME/Core.h>
#include <memory>

namespace ME
{
	class GraphicsImage
	{
		std::shared_ptr<TextureShader> texture;
		std::shared_ptr<ResourceView> resourceView;

		GraphicsImage(const GraphicsImage&) = delete;
		GraphicsImage& operator=(const GraphicsImage&) = delete;
		GraphicsImage(GraphicsImage&&) = delete;
		GraphicsImage& operator=(GraphicsImage&&) = delete;
	public:
		GraphicsImage(REX::W32::ID3D11Device* a_device, REX::W32::ID3D11DeviceContext* a_deviceContext);
		virtual ~GraphicsImage() = default;

		[[nodiscard]] virtual bool LoadFromFile(const char* a_filename);
		[[nodiscard]] virtual bool LoadFromMemory(const F4SE::IDataStream& a_stream, bool a_wic = false);
		[[nodiscard]] virtual bool LoadFromResource(const char* a_type, uint32_t a_resourceID, bool a_wic = false);

		[[nodiscard]] virtual std::shared_ptr<TextureShader> GetTexture() const noexcept(true);
		[[nodiscard]] virtual std::shared_ptr<ResourceView> GetResourceView() const noexcept(true);
	};
}