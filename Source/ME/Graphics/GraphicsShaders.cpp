#include <ME/Graphics/GraphicsShaders.h>

#include <wincodec.h>
#include <shlwapi.h>
#include <limits.h>
#include <comdef.h>

#include <microsoft/DDSTextureLoader11.h>
#include <microsoft/WICTextureLoader11.h>
#include <microsoft/ScreenGrab11.h>

#undef ERROR
#undef MAX_PATH
#undef MEM_RELEASE

extern HMODULE globalDllHandle;

namespace ME
{
	/////////////////////////////////////////
	// BaseShader

	BaseShader::BaseShader(REX::W32::ID3D11Device* a_device, REX::W32::ID3D11DeviceContext* a_deviceContext) :
		device(a_device),
		deviceContext(a_deviceContext)
	{}

	/////////////////////////////////////////
	// ObjectShader

	ObjectShader::ObjectShader(REX::W32::ID3D11Device* a_device, REX::W32::ID3D11DeviceContext* a_deviceContext) :
		BaseShader(a_device, a_deviceContext)
	{}

	/////////////////////////////////////////
	// ResourceView

	ResourceView::ResourceView(REX::W32::ID3D11Device* a_device, REX::W32::ID3D11DeviceContext* a_deviceContext) :
		ObjectShader(a_device, a_deviceContext)
	{}

	void ResourceView::DebugInfo() const noexcept
	{
		REX::INFO("[DBG] Resource info: Format({:X}) Buffer([{}:{}] Off:{} Width:{}) ViewDimension({:X})"sv,
			std::to_underlying(desc.format), desc.buffer.firstElement, desc.buffer.numElements, desc.buffer.elementOffset,
			desc.buffer.elementWidth, std::to_underlying(desc.viewDimension));
	}

	bool ResourceView::Create(const REX::W32::ID3D11Resource* a_resource, const REX::W32::D3D11_SHADER_RESOURCE_VIEW_DESC* a_desc) noexcept
	{
		if (!a_resource || !a_desc)
			return false;

		memcpy(&desc, a_desc, sizeof(REX::W32::D3D11_SHADER_RESOURCE_VIEW_DESC));
		auto hr = device->CreateShaderResourceView(const_cast<REX::W32::ID3D11Resource*>(a_resource), &desc,
			ReleaseAndGetAddressOf());
		if (FAILED(hr))
		{
			REX::ERROR("ResourceView: CreateShaderResourceView returned failed \"{}\""sv, 
				static_cast<const char*>(_com_error(hr).ErrorMessage()));
			return false;
		}

		return true;
	}

	bool ResourceView::Create(const REX::W32::ID3D11ShaderResourceView* a_view) noexcept
	{
		if (!a_view)
			return false;

		REX::W32::D3D11_SHADER_RESOURCE_VIEW_DESC srvDesc;
		auto rv = const_cast<REX::W32::ID3D11ShaderResourceView*>(a_view);
		rv->GetDesc(&srvDesc);

		Microsoft::WRL::ComPtr<REX::W32::ID3D11Resource> Res;
		rv->GetResource(Res.GetAddressOf());

		return Create(Res.Get(), &srvDesc);
	}

	void ResourceView::InitPipeline(uint32_t a_type, uint32_t a_bindID) noexcept
	{
		if ((a_type & std::to_underlying(ShaderType::PIXEL_SHADER)))
		{
			deviceContext->PSSetShaderResources(a_bindID, 1, GetAddressOf());
			bindId[0] = a_bindID;
		}

		if ((a_type & std::to_underlying(ShaderType::VERTEX_SHADER)))
		{
			deviceContext->VSSetShaderResources(a_bindID, 1, GetAddressOf());
			bindId[1] = a_bindID;
		}

		if ((a_type & std::to_underlying(ShaderType::COMPUTE_SHADER)))
		{
			deviceContext->CSSetShaderResources(a_bindID, 1, GetAddressOf());
			bindId[2] = a_bindID;
		}

		ipType = a_type;
	}

	void ResourceView::ShutdownPipeline() noexcept
	{
		if ((ipType & std::to_underlying(ShaderType::PIXEL_SHADER)))
			deviceContext->PSSetShaderResources(bindId[0], 0, nullptr);

		if ((ipType & std::to_underlying(ShaderType::VERTEX_SHADER)))
			deviceContext->VSSetShaderResources(bindId[1], 0, nullptr);

		if ((ipType & std::to_underlying(ShaderType::COMPUTE_SHADER)))
			deviceContext->CSSetShaderResources(bindId[2], 0, nullptr);

		ipType = 0;
	}

	bool ResourceView::Empty() const noexcept
	{
		return resourceView == nullptr;
	}

	REX::W32::ID3D11ShaderResourceView* ResourceView::Get() const noexcept
	{
		return resourceView.Get();
	}

	REX::W32::ID3D11ShaderResourceView** ResourceView::GetAddressOf() noexcept
	{
		return resourceView.GetAddressOf();
	}

	REX::W32::ID3D11ShaderResourceView** ResourceView::ReleaseAndGetAddressOf() noexcept
	{
		return resourceView.ReleaseAndGetAddressOf();
	}

	const REX::W32::D3D11_SHADER_RESOURCE_VIEW_DESC* ResourceView::GetDesc() const noexcept
	{
		return &desc;
	}

	/////////////////////////////////////////
	// TextureShader

	TextureShader::TextureShader(REX::W32::ID3D11Device* a_device, REX::W32::ID3D11DeviceContext* a_deviceContext) :
		BaseShader(a_device, a_deviceContext)
	{}

	void TextureShader::DebugInfo() const noexcept
	{
		REX::INFO("[DBG] Texture info: Size({}x{}) Format({:X}) Usage({:X}) BindFlags({:X})"sv,
			desc.width, desc.height, std::to_underlying(desc.format), std::to_underlying(desc.usage), desc.bindFlags);
	}

	bool TextureShader::LoadFromFile(const char* a_filename, ResourceView* a_resource) noexcept
	{
		if (!a_filename || !a_filename[0] || !a_resource)
			return false;

		auto hRes = S_OK;
		auto WIC = _stricmp(PathFindExtensionA(a_filename), ".dds");
		auto len = strlen(a_filename);
		auto buffer = std::make_unique<wchar_t[]>(len + 1);
		
		if (!buffer)
			return false;

		ZeroMemory(buffer.get(), (size_t)len + 1);
		if (MultiByteToWideChar(CP_ACP, 0, a_filename, (int32_t)len, buffer.get(), (int32_t)len) <= 0)
			return false;

		if (WIC)
			hRes = DirectX::CreateWICTextureFromFile((ID3D11Device*)device, buffer.get(),
				(ID3D11Resource**)ReleaseAndGetAddressOf(),
				(ID3D11ShaderResourceView**)a_resource->ReleaseAndGetAddressOf());
		else
			hRes = DirectX::CreateDDSTextureFromFile((ID3D11Device*)device, buffer.get(),
				(ID3D11Resource**)ReleaseAndGetAddressOf(), 
				(ID3D11ShaderResourceView**)a_resource->ReleaseAndGetAddressOf());

		bool Succeed = SUCCEEDED(hRes);
		if (Succeed)
		{
			Get()->GetDesc(&desc);
			a_resource->Get()->GetDesc(const_cast<REX::W32::D3D11_SHADER_RESOURCE_VIEW_DESC*>(a_resource->GetDesc()));
		}

		return Succeed;
	}

	bool TextureShader::LoadFromStream(const void* a_buffer, uint32_t a_bufferSize,
		ResourceView* a_resource, bool a_wic) noexcept
	{
		if (!a_buffer || !a_bufferSize || !a_resource)
			return false;

		auto hRes = S_OK;

		if (a_wic)
			hRes = DirectX::CreateWICTextureFromMemory((ID3D11Device*)device, (const std::uint8_t*)a_buffer, a_bufferSize,
				(ID3D11Resource**)ReleaseAndGetAddressOf(), (ID3D11ShaderResourceView**)a_resource->ReleaseAndGetAddressOf());
		else
			hRes = DirectX::CreateDDSTextureFromMemory((ID3D11Device*)device, (const std::uint8_t*)a_buffer, a_bufferSize,
			(ID3D11Resource**)ReleaseAndGetAddressOf(), (ID3D11ShaderResourceView**)a_resource->ReleaseAndGetAddressOf());

		bool Succeed = SUCCEEDED(hRes);
		if (Succeed)
		{
			Get()->GetDesc(&desc);
			a_resource->Get()->GetDesc(const_cast<REX::W32::D3D11_SHADER_RESOURCE_VIEW_DESC*>(a_resource->GetDesc()));
		}

		return Succeed;
	}

	bool TextureShader::LoadFromResource(const char* a_type, uint32_t a_resourceID,
		ResourceView* a_resource, bool a_wic) noexcept
	{
		if (!a_type || !a_resource)
			return false;

		auto hModule = globalDllHandle;
		if (!hModule)
			return false;

		auto hResource = FindResourceA(hModule, MAKEINTRESOURCEA(a_resourceID), a_type);
		if (!hResource)
		{
			REX::WARN("Resource no found"sv);
			return false;
		}

		auto Size = (int32_t)SizeofResource(hModule, hResource);
		if (Size >= std::numeric_limits<int32_t>::max())
		{
			REX::WARN("Size of resource exceeds 2 gigs"sv);
			return false;
		}

		auto hResourceMemory = LoadResource(hModule, hResource);
		if (!hResourceMemory)
		{
			REX::ERROR("hResourceMemory == nullptr"sv);
			return false;
		}

		bool Succeed = LoadFromStream(LockResource(hResourceMemory), Size, a_resource, a_wic);
		FreeResource(hResourceMemory);

		return Succeed;
	}

	bool TextureShader::SaveToFile(const char* a_filename) noexcept
	{
		auto hRes = S_OK;

		auto Ext = PathFindExtensionA(a_filename);
		auto WIC = _stricmp(Ext, ".dds");
		auto len = strlen(a_filename);
		auto buffer = std::make_unique<wchar_t[]>(len + 1);

		if (!buffer)
			return false;

		ZeroMemory(buffer.get(), (size_t)len + 1);
		if (MultiByteToWideChar(CP_ACP, 0, a_filename, (int32_t)len, buffer.get(), (int32_t)len) <= 0)
			return false;

		if (WIC)
		{
			GUID FormatTex{};

			if (!_stricmp(Ext, ".png"))
				FormatTex = GUID_ContainerFormatPng;
			else if (!_stricmp(Ext, ".jpeg") || !_stricmp(Ext, ".jpg"))
				FormatTex = GUID_ContainerFormatJpeg;
			else if (!_stricmp(Ext, ".tiff") || !_stricmp(Ext, ".tif"))
				FormatTex = GUID_ContainerFormatTiff;
			else if (!_stricmp(Ext, ".bmp"))
				FormatTex = GUID_ContainerFormatBmp;
			else
				return false;

			hRes = DirectX::SaveWICTextureToFile((ID3D11DeviceContext*)deviceContext, (ID3D11Texture2D*)Get(), FormatTex, buffer.get());
		}
		else
			hRes = DirectX::SaveDDSTextureToFile((ID3D11DeviceContext*)deviceContext, (ID3D11Texture2D*)Get(), buffer.get());

		return SUCCEEDED(hRes);
	}

	bool TextureShader::Create(const REX::W32::D3D11_TEXTURE2D_DESC* a_desc) noexcept
	{
		if (!a_desc)
		{
			REX::ERROR("TextureShader: description nullptr"sv);
			return false;
		}

		memcpy(&desc, a_desc, sizeof(REX::W32::D3D11_TEXTURE2D_DESC));
		auto hr = device->CreateTexture2D(&desc, nullptr, ReleaseAndGetAddressOf());
		if (FAILED(hr))
		{
			REX::ERROR("TextureShader: CreateTexture2D returned failed \"{}\""sv, _com_error(hr).ErrorMessage());
			return false;
		}

		return true;
	}

	bool TextureShader::Create(const REX::W32::ID3D11Texture2D* a_texture, uint32_t a_bindFlags) noexcept
	{
		if (!a_texture)
		{
			REX::ERROR("TextureShader: texture nullptr"sv);
			return false;
		}

		auto Tex = const_cast<REX::W32::ID3D11Texture2D*>(a_texture);
		Tex->GetDesc(&desc);
		desc.bindFlags = a_bindFlags;

		if (Create(&desc))
		{
			deviceContext->CopyResource(Get(), Tex);
			return true;
		}

		return false;
	}

	bool TextureShader::Create(const REX::W32::ID3D11Resource* a_resource, uint32_t a_bindFlags) noexcept
	{
		if (!a_resource)
		{
			REX::ERROR("TextureShader: resource nullptr"sv);
			return false;
		}

		auto Res = const_cast<REX::W32::ID3D11Resource*>(a_resource);
		Microsoft::WRL::ComPtr<REX::W32::ID3D11Texture2D> Tex;

		auto hr = Res->QueryInterface(REX::W32::IID_ID3D11Texture2D, reinterpret_cast<void**>(Tex.GetAddressOf()));
		if (FAILED(hr))
		{
			REX::ERROR("TextureShader: QueryInterface returned failed \"{}\""sv, _com_error(hr).ErrorMessage());
			return false;
		}

		return Create(Tex.Get(), a_bindFlags);
	}

	bool TextureShader::CopyFrom(const TextureShader* a_texture) noexcept
	{
		if (!a_texture) return false;
		return CopyFrom(a_texture->Get());
	}

	bool TextureShader::CopyFrom(const REX::W32::ID3D11Texture2D* a_texture) noexcept
	{
		if (!a_texture) return false;
		deviceContext->CopyResource(Get(), const_cast<REX::W32::ID3D11Texture2D*>(a_texture));
		return true;
	}

	bool TextureShader::CopyFrom(const REX::W32::ID3D11Resource* a_resource) noexcept
	{
		if (!a_resource)
			return false;

		deviceContext->CopyResource(Get(), const_cast<REX::W32::ID3D11Resource*>(a_resource));

		return true;
	}

	bool TextureShader::Empty() const noexcept
	{
		return texture == nullptr;
	}

	REX::W32::ID3D11Texture2D* TextureShader::Get() const noexcept
	{ 
		return texture.Get();
	}

	REX::W32::ID3D11Texture2D** TextureShader::GetAddressOf() noexcept
	{
		return texture.GetAddressOf();
	}

	REX::W32::ID3D11Texture2D** TextureShader::ReleaseAndGetAddressOf() noexcept
	{
		return texture.ReleaseAndGetAddressOf();
	}

	const REX::W32::D3D11_TEXTURE2D_DESC* TextureShader::GetDesc() const noexcept
	{
		return &desc;
	}

	/////////////////////////////////////////
	// BufferShader

	BufferShader::BufferShader(REX::W32::ID3D11Device* a_device, REX::W32::ID3D11DeviceContext* a_deviceContext) :
		ObjectShader(a_device, a_deviceContext)
	{}

	bool BufferShader::Create(const void* a_buffer, uint32_t a_size, uint32_t a_bindFlags, REX::W32::D3D11_USAGE a_usage) noexcept
	{
		if (!a_buffer || !a_size)
			return false;

		// Fill in a buffer description
		CD3D11_BUFFER_DESC cbDesc(a_size, a_bindFlags, (D3D11_USAGE)a_usage);
		memcpy(&desc, &cbDesc, sizeof(REX::W32::D3D11_BUFFER_DESC));

		// Fill in the subresource data
		REX::W32::D3D11_SUBRESOURCE_DATA InitData;
		ZeroMemory(&InitData, sizeof(REX::W32::D3D11_SUBRESOURCE_DATA));
		InitData.sysMem = a_buffer;

		// Create the buffer
		auto hr = device->CreateBuffer(&desc, &InitData, buffer.ReleaseAndGetAddressOf());
		if (FAILED(hr))
		{
			REX::ERROR("BufferShader: CreateBuffer returned failed \"{}\""sv, _com_error(hr).ErrorMessage());
			return false;
		}

		return true;
	}

	void BufferShader::InitPipeline(uint32_t a_type, uint32_t a_bindId) noexcept
	{
		if ((a_type & std::to_underlying(ShaderType::PIXEL_SHADER)))
		{
			deviceContext->PSSetConstantBuffers(a_bindId, 1, buffer.GetAddressOf());
			bindId[0] = a_bindId;
		}

		if ((a_type & std::to_underlying(ShaderType::VERTEX_SHADER)))
		{
			deviceContext->VSSetConstantBuffers(a_bindId, 1, buffer.GetAddressOf());
			bindId[1] = a_bindId;
		}

		if ((a_type & std::to_underlying(ShaderType::COMPUTE_SHADER)))
		{
			deviceContext->CSSetConstantBuffers(a_bindId, 1, buffer.GetAddressOf());
			bindId[2] = a_bindId;
		}

		ipType = a_type;
	}

	void BufferShader::ShutdownPipeline() noexcept
	{
		if ((ipType & std::to_underlying(ShaderType::PIXEL_SHADER)))
			deviceContext->PSSetConstantBuffers(bindId[0], 0, nullptr);

		if ((ipType & std::to_underlying(ShaderType::VERTEX_SHADER)))
			deviceContext->VSSetConstantBuffers(bindId[1], 0, nullptr);

		if ((ipType & std::to_underlying(ShaderType::COMPUTE_SHADER)))
			deviceContext->CSSetConstantBuffers(bindId[2], 0, nullptr);

		ipType = 0;
	}

	REX::W32::ID3D11Buffer* BufferShader::Get() const noexcept
	{
		return buffer.Get();
	}

	REX::W32::ID3D11Buffer** BufferShader::GetAddressOf() noexcept
	{
		return buffer.GetAddressOf();
	}

	REX::W32::ID3D11Buffer** BufferShader::ReleaseAndGetAddressOf() noexcept
	{
		return buffer.ReleaseAndGetAddressOf();
	}

	const REX::W32::D3D11_BUFFER_DESC* BufferShader::GetDesc() const noexcept
	{
		return &desc;
	}

	

	/////////////////////////////////////////
	// UnorderedAccessView

	UnorderedAccessView::UnorderedAccessView(REX::W32::ID3D11Device* a_device, REX::W32::ID3D11DeviceContext* a_deviceContext) :
		ObjectShader(a_device, a_deviceContext)
	{

	}

	void UnorderedAccessView::DebugInfo() const noexcept
	{
		REX::INFO("[DBG] Unordered access info: Format({:X}) Buffer([{}:{}] Flags:{:X}) ViewDimension({:X})"sv,
			std::to_underlying(desc.format), desc.buffer.firstElement, desc.buffer.numElements,
			desc.buffer.flags, std::to_underlying(desc.viewDimension));
	}

	bool UnorderedAccessView::Create(const REX::W32::ID3D11Resource* a_resource, const REX::W32::D3D11_UNORDERED_ACCESS_VIEW_DESC* a_desc) noexcept
	{
		if (!a_resource || !a_desc)
			return false;

		memcpy(&desc, a_desc, sizeof(REX::W32::D3D11_UNORDERED_ACCESS_VIEW_DESC));
		auto hr = device->CreateUnorderedAccessView(const_cast<REX::W32::ID3D11Resource*>(a_resource), &desc,
			ReleaseAndGetAddressOf());
		if (FAILED(hr))
		{
			REX::ERROR("UnorderedAccessView: CreateUnorderedAccessView returned failed \"{}\""sv, _com_error(hr).ErrorMessage());
			return false;
		}

		return true;
	}

	bool UnorderedAccessView::Create(const REX::W32::ID3D11UnorderedAccessView* a_view) noexcept
	{
		if (!a_view)
			return false;

		REX::W32::D3D11_UNORDERED_ACCESS_VIEW_DESC srvDesc;
		auto rtv = const_cast<REX::W32::ID3D11UnorderedAccessView*>(a_view);
		rtv->GetDesc(&srvDesc);

		Microsoft::WRL::ComPtr<REX::W32::ID3D11Resource> Res;
		rtv->GetResource(Res.GetAddressOf());

		return Create(Res.Get(), &srvDesc);
	}

	void UnorderedAccessView::InitPipeline(uint32_t a_type, uint32_t a_bindID) noexcept
	{
		if ((a_type & std::to_underlying(ShaderType::COMPUTE_SHADER)))
		{
			deviceContext->CSSetUnorderedAccessViews(a_bindID, 1, GetAddressOf(), &counts);
			bindId = a_bindID;
		}

		ipType = a_type;
	}

	void UnorderedAccessView::ShutdownPipeline() noexcept
	{
		if ((ipType & std::to_underlying(ShaderType::COMPUTE_SHADER)))
			deviceContext->CSSetUnorderedAccessViews(bindId, 0, nullptr, nullptr);

		ipType = 0;
	}

	bool UnorderedAccessView::Empty() const noexcept
	{
		return view == nullptr;
	}

	REX::W32::ID3D11UnorderedAccessView* UnorderedAccessView::Get() const noexcept
	{
		return view.Get();
	}

	REX::W32::ID3D11UnorderedAccessView** UnorderedAccessView::GetAddressOf() noexcept
	{
		return view.GetAddressOf();
	}

	REX::W32::ID3D11UnorderedAccessView** UnorderedAccessView::ReleaseAndGetAddressOf() noexcept
	{
		return view.ReleaseAndGetAddressOf();
	}

	const REX::W32::D3D11_UNORDERED_ACCESS_VIEW_DESC* UnorderedAccessView::GetDesc() const noexcept
	{
		return &desc;
	}

	/////////////////////////////////////////
	// SamplerState

	SamplerState::SamplerState(REX::W32::ID3D11Device* a_device, REX::W32::ID3D11DeviceContext* a_deviceContext) :
		ObjectShader(a_device, a_deviceContext)
	{}

	void SamplerState::DebugInfo() const noexcept
	{
		REX::INFO("[DBG] Sampler info: Address({:X}x{:X}x{:X}) BorderColor({:.3f},{:.3f},{:.3f},{:.3f}) ComparisonFunc({:X})"
			" Filter({:X}) MaxAnisotropy({}) LOD({:.3f}:{:.3f}) MipLODBias({:.3f})"sv,
			std::to_underlying(desc.addressU), std::to_underlying(desc.addressV), std::to_underlying(desc.addressW),
			desc.borderColor[0], desc.borderColor[1], desc.borderColor[2], desc.borderColor[3],
			std::to_underlying(desc.comparisonFunc), std::to_underlying(desc.filter), desc.maxAnisotropy,
			desc.minLOD, desc.maxLOD, desc.mipLODBias);
	}

	bool SamplerState::Create(const REX::W32::D3D11_SAMPLER_DESC* a_desc) noexcept
	{
		if (!a_desc)
			return false;

		memcpy(&desc, a_desc, sizeof(REX::W32::D3D11_SAMPLER_DESC));
		auto hr = device->CreateSamplerState(&desc, ReleaseAndGetAddressOf());
		if (FAILED(hr))
		{
			REX::ERROR("SamplerState: CreateSamplerState returned failed \"{}\""sv, _com_error(hr).ErrorMessage());
			return false;
		}

		return true;
	}

	void SamplerState::InitPipeline(uint32_t _type, uint32_t a_bindID) noexcept
	{
		if ((_type & std::to_underlying(ShaderType::PIXEL_SHADER)))
		{
			deviceContext->PSSetSamplers(a_bindID, 1, GetAddressOf());
			bindId[0] = a_bindID;
		}

		if ((_type & std::to_underlying(ShaderType::VERTEX_SHADER)))
		{
			deviceContext->VSSetSamplers(a_bindID, 1, GetAddressOf());
			bindId[1] = a_bindID;
		}

		if ((_type & std::to_underlying(ShaderType::COMPUTE_SHADER)))
		{
			deviceContext->CSSetSamplers(a_bindID, 1, GetAddressOf());
			bindId[2] = a_bindID;
		}

		ipType = _type;
	}

	void SamplerState::ShutdownPipeline() noexcept
	{
		if ((ipType & std::to_underlying(ShaderType::PIXEL_SHADER)))
			deviceContext->PSSetSamplers(bindId[0], 0, nullptr);

		if ((ipType & std::to_underlying(ShaderType::VERTEX_SHADER)))
			deviceContext->VSSetSamplers(bindId[1], 0, nullptr);

		if ((ipType & std::to_underlying(ShaderType::COMPUTE_SHADER)))
			deviceContext->CSSetSamplers(bindId[2], 0, nullptr);

		ipType = 0;
	}

	bool SamplerState::Empty() const noexcept
	{
		return samplerState == nullptr;
	}

	REX::W32::ID3D11SamplerState* SamplerState::Get() const noexcept
	{
		return samplerState.Get();
	}

	REX::W32::ID3D11SamplerState** SamplerState::GetAddressOf() noexcept
	{
		return samplerState.GetAddressOf();
	}

	REX::W32::ID3D11SamplerState** SamplerState::ReleaseAndGetAddressOf() noexcept
	{
		return samplerState.ReleaseAndGetAddressOf();
	}

	const REX::W32::D3D11_SAMPLER_DESC* SamplerState::GetDesc() const noexcept
	{
		return &desc;
	}

	/////////////////////////////////////////
	// RenderTargetView

	RenderTargetView::RenderTargetView(REX::W32::ID3D11Device* a_device, REX::W32::ID3D11DeviceContext* a_deviceContext) :
		BaseShader(a_device, a_deviceContext)
	{}

	void RenderTargetView::DebugInfo() const noexcept
	{
		REX::INFO("[DBG] Render target info: Format({:X}) Buffer([{}:{}] Off:{} Width:{}) ViewDimension({:X})"sv,
			std::to_underlying(desc.format), desc.buffer.firstElement, desc.buffer.numElements, desc.buffer.elementOffset,
			desc.buffer.elementWidth, std::to_underlying(desc.viewDimension));
	}

	bool RenderTargetView::Create(const REX::W32::ID3D11Resource* a_resource, const REX::W32::D3D11_RENDER_TARGET_VIEW_DESC* a_desc) noexcept
	{
		if (!a_resource || !a_desc)
			return false;

		memcpy(&desc, a_desc, sizeof(REX::W32::D3D11_RENDER_TARGET_VIEW_DESC));
		auto hr = device->CreateRenderTargetView(const_cast<REX::W32::ID3D11Resource*>(a_resource), &desc,
			ReleaseAndGetAddressOf());
		if (FAILED(hr))
		{
			REX::ERROR("RenderTargetView: CreateRenderTargetView returned failed \"{}\""sv, _com_error(hr).ErrorMessage());
			return false;
		}

		return true;
	}

	bool RenderTargetView::Create(const REX::W32::ID3D11RenderTargetView* a_view) noexcept
	{
		if (!a_view)
			return false;

		REX::W32::D3D11_RENDER_TARGET_VIEW_DESC rtvDesc;
		auto rtv = const_cast<REX::W32::ID3D11RenderTargetView*>(a_view);
		rtv->GetDesc(&rtvDesc);

		Microsoft::WRL::ComPtr<REX::W32::ID3D11Resource> Res;
		rtv->GetResource(Res.GetAddressOf());

		return Create(Res.Get(), &rtvDesc);
	}

	bool RenderTargetView::Empty() const noexcept
	{
		return renderTarget == nullptr;
	}

	REX::W32::ID3D11RenderTargetView* RenderTargetView::Get() const noexcept
	{
		return renderTarget.Get();
	}

	REX::W32::ID3D11RenderTargetView** RenderTargetView::GetAddressOf() noexcept
	{
		return renderTarget.GetAddressOf();
	}

	REX::W32::ID3D11RenderTargetView** RenderTargetView::ReleaseAndGetAddressOf() noexcept
	{
		return renderTarget.ReleaseAndGetAddressOf();
	}

	const REX::W32::D3D11_RENDER_TARGET_VIEW_DESC* RenderTargetView::GetDesc() const noexcept
	{
		return &desc;
	}

	/////////////////////////////////////////
	// DepthStencilView

	DepthStencilView::DepthStencilView(REX::W32::ID3D11Device* a_device, REX::W32::ID3D11DeviceContext* a_deviceContext) :
		BaseShader(a_device, a_deviceContext)
	{}

	void DepthStencilView::DebugInfo() const noexcept
	{
		REX::INFO("[DBG] Depth and stencil info: Format({:X}) Flags({:X}) ViewDimension({:X})"sv,
			std::to_underlying(desc.format), desc.flags, std::to_underlying(desc.viewDimension));
	}

	bool DepthStencilView::Create(const REX::W32::ID3D11Resource* a_resource, const REX::W32::D3D11_DEPTH_STENCIL_VIEW_DESC* a_desc) noexcept
	{
		if (!a_resource || !a_desc)
			return false;

		memcpy(&desc, a_desc, sizeof(REX::W32::D3D11_DEPTH_STENCIL_VIEW_DESC));
		auto hr = device->CreateDepthStencilView(const_cast<REX::W32::ID3D11Resource*>(a_resource), &desc,
			ReleaseAndGetAddressOf());
		if (FAILED(hr))
		{
			REX::ERROR("DepthStencilView: CreateDepthStencilView returned failed \"{}\""sv, _com_error(hr).ErrorMessage());
			return false;
		}

		return true;
	}

	bool DepthStencilView::Create(const REX::W32::ID3D11DepthStencilView* a_view) noexcept
	{
		if (!a_view)
			return false;

		REX::W32::D3D11_DEPTH_STENCIL_VIEW_DESC dsvDesc;
		auto rtv = const_cast<REX::W32::ID3D11DepthStencilView*>(a_view);
		rtv->GetDesc(&dsvDesc);

		Microsoft::WRL::ComPtr<REX::W32::ID3D11Resource> Res;
		rtv->GetResource(Res.GetAddressOf());

		return Create(Res.Get(), &dsvDesc);
	}

	bool DepthStencilView::Empty() const noexcept
	{
		return depthTarget == nullptr;
	}

	REX::W32::ID3D11DepthStencilView* DepthStencilView::Get() const noexcept
	{
		return depthTarget.Get();
	}

	REX::W32::ID3D11DepthStencilView** DepthStencilView::GetAddressOf() noexcept
	{
		return depthTarget.GetAddressOf();
	}

	REX::W32::ID3D11DepthStencilView** DepthStencilView::ReleaseAndGetAddressOf() noexcept
	{
		return depthTarget.ReleaseAndGetAddressOf();
	}

	const REX::W32::D3D11_DEPTH_STENCIL_VIEW_DESC* DepthStencilView::GetDesc() const noexcept
	{
		return &desc;
	}

	/////////////////////////////////////////
	// Shader

	Shader::Shader(REX::W32::ID3D11Device* a_device, REX::W32::ID3D11DeviceContext* a_deviceContext) :
		BaseShader(a_device, a_deviceContext),
		IBufferStream()	
	{}

	Shader::~Shader()
	{
		Release();
	}

	void Shader::Release() noexcept 
	{
		Shutdown();
	}

	bool Shader::LoadFromResource(std::uint32_t ResourceID) noexcept
	{
		return LoadFromResource(MAKEINTRESOURCEA(ResourceID));
	}

	bool Shader::LoadFromResource(const char* ResourceName) noexcept
	{
		if (!ResourceName)
			return false;

		auto hModule = globalDllHandle;
		if (!hModule)
			return false;

		auto hResource = FindResourceA(hModule, ResourceName, "SHADER");
		if (!hResource)
		{
			REX::WARN("Resource no found"sv);
			return false;
		}

		auto Size = (int32_t)SizeofResource(hModule, hResource);
		if (Size >= std::numeric_limits<int32_t>::max())
		{
			REX::WARN("Size of resource exceeds 2 gigs"sv);
			return false;
		}

		auto hResourceMemory = LoadResource(hModule, hResource);
		if (!hResourceMemory)
		{
			REX::ERROR("hResourceMemory == nullptr"sv);
			return false;
		}

		auto pBufferForStream = new uint8_t[Size];
		if (!pBufferForStream)
		{
			REX::ERROR("pBufferForStream == nullptr"sv);
			return false;
		}

		OwnBuffer();
		SetBuffer(pBufferForStream, Size);
		WriteBuf(LockResource(hResourceMemory), Size);
		SetOffset(0);

		bool Successed = Size == (uint32_t)GetLength();
		if (!Successed)
		{
			DisownBuffer();
			SetBuffer(nullptr, 0);
			delete[] pBufferForStream;

			REX::ERROR("An error occurred while reading the resource");
		}
		else
			REX::INFO("Resource was loaded successfully"sv);

		FreeResource(hResourceMemory);
		return Successed;
	}

	bool Shader::Empty() const noexcept
	{
		return GetLength() == 0;
	}

	/////////////////////////////////////////
	// PixelShader

	PixelShader::PixelShader(REX::W32::ID3D11Device* a_device, REX::W32::ID3D11DeviceContext* a_deviceContext) :
		Shader(a_device, a_deviceContext)
	{}

	void PixelShader::Bind() noexcept
	{
		deviceContext->PSSetShader(pixelShader.Get(), nullptr, 0);
	}

	void PixelShader::Unbind() noexcept
	{
		deviceContext->PSSetShader(nullptr, nullptr, 0);
	}

	bool PixelShader::Install() noexcept
	{
		if (Empty())
			return false;

		// create the shader object
		auto hr = device->CreatePixelShader(streamBuf, streamLength, nullptr, pixelShader.GetAddressOf());
		if (FAILED(hr))
		{
			REX::ERROR("CreatePixelShader: error has occurred: \"{}\""sv, _com_error(hr).ErrorMessage());
			return false;
		}

		REX::INFO("PixelShader initialization was successful"sv);

		return true;
	}

	void PixelShader::Shutdown() noexcept
	{
		pixelShader.Reset();
	}

	/////////////////////////////////////////
	// VertexShader

	VertexShader::VertexShader(REX::W32::ID3D11Device* a_device, REX::W32::ID3D11DeviceContext* a_deviceContext) :
		Shader(a_device, a_deviceContext)
	{
		inputBuffer = std::make_unique<BufferShader>(a_device, a_deviceContext);
		indexesBuffer = std::make_unique<BufferShader>(a_device, a_deviceContext);
	}

	void VertexShader::Bind() noexcept
	{
		deviceContext->IASetPrimitiveTopology(topology);
		deviceContext->IASetInputLayout(inputLayout.Get());
		deviceContext->IASetVertexBuffers(0, 1, inputBuffer->GetAddressOf(), &strideBuffer, &offsetBuffer);
		deviceContext->IASetIndexBuffer(indexesBuffer->Get(), REX::W32::DXGI_FORMAT_R32_UINT, 0);
		deviceContext->VSSetShader(vertexShader.Get(), nullptr, 0);
	}

	void VertexShader::Unbind() noexcept
	{
		deviceContext->IASetPrimitiveTopology(topology);
		deviceContext->IASetInputLayout(nullptr);
		deviceContext->IASetVertexBuffers(0, 0, nullptr, nullptr, nullptr);
		deviceContext->IASetIndexBuffer(nullptr, REX::W32::DXGI_FORMAT_R32_UINT, 0);
		deviceContext->VSSetShader(nullptr, nullptr, 0);
	}

	bool VertexShader::Install() noexcept
	{
		if (Empty())
			return false;

		// create the shader object
		auto hr = device->CreateVertexShader(GetBuffer(), GetLength(), nullptr, vertexShader.GetAddressOf());
		if (FAILED(hr))
		{
			REX::ERROR("CreateVertexShader: error has occurred: \"{}\""sv, _com_error(hr).ErrorMessage());
			return false;
		}

		REX::W32::D3D11_INPUT_ELEMENT_DESC inputElementDesc[] =
		{
			{ "POS", 0, REX::W32::DXGI_FORMAT_R32G32_FLOAT, 0, 0, REX::W32::D3D11_INPUT_PER_VERTEX_DATA, 0 },
			{ "TEX", 0, REX::W32::DXGI_FORMAT_R32G32_FLOAT, 0, D3D11_APPEND_ALIGNED_ELEMENT, REX::W32::D3D11_INPUT_PER_VERTEX_DATA, 0 },
		};

		hr = device->CreateInputLayout(inputElementDesc, std::size(inputElementDesc), 
			GetBuffer(), GetLength(), inputLayout.GetAddressOf());
		if (FAILED(hr))
		{
			REX::ERROR("CreateInputLayout: error has occurred: \"{}\""sv, _com_error(hr).ErrorMessage());
			return false;
		}

		REX::INFO("VertexShader initialization was successful"sv);

		return true;
	}

	void VertexShader::Shutdown() noexcept
	{
		vertexShader.Reset();
	}

	bool VertexShader::InitData(REX::W32::D3D_PRIMITIVE_TOPOLOGY a_topology, const std::initializer_list<Graphics::Vec2>& a_inputData) noexcept
	{
		if (!a_inputData.size())
			return false;

		topology = a_topology;
		sizeBuffer = (uint32_t)(a_inputData.size() >> 1);
		strideBuffer = sizeof(Graphics::Vec2) + sizeof(Graphics::Vec2);
		offsetBuffer = 0;

		if (!inputBuffer->Create(reinterpret_cast<const void*>(a_inputData.begin()),
			strideBuffer * sizeBuffer, D3D11_BIND_VERTEX_BUFFER))
			return false;

		REX::INFO("VertexShader initialization data was successful"sv);

		return true;
	}

	bool VertexShader::InitDataWithIndexes(REX::W32::D3D_PRIMITIVE_TOPOLOGY a_topology, const std::initializer_list<Graphics::Vec2>& a_inputData,
		const std::initializer_list<uint32_t>& a_indexes) noexcept
	{
		if (!a_indexes.size() || !InitData(a_topology, a_inputData))
			return false;

		if (!indexesBuffer->Create(reinterpret_cast<const void*>(a_indexes.begin()),
			(uint32_t)(sizeof(std::uint32_t) * a_indexes.size()), D3D11_BIND_INDEX_BUFFER))
			return false;

		REX::INFO("VertexShader initialization indexes was successful"sv);

		return true;
	}

	/////////////////////////////////////////
	// ComputeShader

	ComputeShader::ComputeShader(REX::W32::ID3D11Device* a_device, REX::W32::ID3D11DeviceContext* a_deviceContext) :
		Shader(a_device, a_deviceContext)
	{}

	void ComputeShader::Dispatch(uint32_t a_threadGroupCountX, uint32_t a_threadGroupCountY, uint32_t a_threadGroupCountZ) noexcept
	{
		deviceContext->Dispatch(a_threadGroupCountX, a_threadGroupCountY, a_threadGroupCountZ);
	}

	void ComputeShader::Bind() noexcept
	{
		deviceContext->CSSetShader(computeShader.Get(), nullptr, 0);
	}

	void ComputeShader::Unbind() noexcept
	{
		deviceContext->CSSetShader(nullptr, nullptr, 0);
	}

	bool ComputeShader::Install() noexcept
	{
		if (Empty())
			return false;

		// create the shader object
		auto hr = device->CreateComputeShader(GetBuffer(), GetLength(), nullptr, computeShader.GetAddressOf());
		if (FAILED(hr))
		{
			REX::ERROR("CreateComputeShader: error has occurred: \"{}\""sv, _com_error(hr).ErrorMessage());
			return false;
		}

		REX::INFO("ComputeShader initialization was successful"sv);

		return true;
	}

	void ComputeShader::Shutdown() noexcept
	{
		computeShader.Reset();
	}
}