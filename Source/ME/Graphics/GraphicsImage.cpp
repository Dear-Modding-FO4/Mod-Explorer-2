#include <ME/Graphics/GraphicsImage.h>
#include <ME/Graphics/GraphicsShaders.h>

using namespace ME;

ME::GraphicsImage::GraphicsImage(REX::W32::ID3D11Device* a_device, REX::W32::ID3D11DeviceContext* a_deviceContext)
{
	texture = std::make_shared<TextureShader>(a_device, a_deviceContext);
	resourceView = std::make_shared<ResourceView>(a_device, a_deviceContext);
}

bool ME::GraphicsImage::LoadFromFile(const char* a_filename)
{
	bool Result = texture->LoadFromFile(a_filename, resourceView.get());
	if (Result) REX::INFO("PSGRenrererImage::LoadFromFile() return successed [ File: \"{}\" ]"sv, static_cast<const char*>(a_filename));
	else REX::WARN("PSGRenrererImage::LoadFromFile() return failed [ File: \"{}\" ]"sv, static_cast<const char*>(a_filename));
	return Result;
}

bool ME::GraphicsImage::LoadFromMemory(const F4SE::IDataStream& a_stream, bool a_wic)
{
	if (!a_stream.GetLength())
		return false;

	auto& Stream = const_cast<F4SE::IDataStream&>(a_stream);
	auto Buffer = std::make_unique<std::uint8_t[]>(a_stream.GetLength());
	Stream.ReadBuf(Buffer.get(), (uint32_t)a_stream.GetLength());

	bool Result = texture->LoadFromStream(Buffer.get(), (uint32_t)a_stream.GetLength(), resourceView.get(), a_wic);
	if (Result) REX::INFO("PSGRenrererImage::LoadFromMemory() return successed"sv);
	else REX::WARN("PSGRenrererImage::LoadFromMemory() return failed"sv);
	return Result;
}

bool ME::GraphicsImage::LoadFromResource(const char* a_type, uint32_t a_resourceID, bool a_wic)
{
	bool Result = texture->LoadFromResource(a_type, a_resourceID, resourceView.get(), a_wic);
	if (Result) REX::INFO("PSGRenrererImage::LoadFromResource() return successed [ ResourceID: {} ]"sv, a_resourceID);
	else REX::WARN("PSGRenrererImage::LoadFromResource() return failed [ ResourceID: {} ]"sv, a_resourceID);
	return Result;
}

std::shared_ptr<TextureShader> ME::GraphicsImage::GetTexture() const noexcept(true)
{
	return texture;
}

std::shared_ptr<ResourceView> ME::GraphicsImage::GetResourceView() const noexcept(true)
{
	return resourceView;
}
