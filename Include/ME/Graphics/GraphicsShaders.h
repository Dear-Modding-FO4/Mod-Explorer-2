#pragma once

#include <ME/Core.h>
#include <ME/Graphics/GraphicsMath.h>
#include <wrl/client.h>

#undef ERROR
#undef MAX_PATH
#undef MEM_RELEASE

#include <memory>
#include <initializer_list>

namespace ME
{
	enum class ShaderType : uint8_t
	{
		PIXEL_SHADER = 1 << 0,
		VERTEX_SHADER = 1 << 1,
		COMPUTE_SHADER = 1 << 2,
	};

	class BaseShader
	{
		BaseShader(const BaseShader&) = delete;
		BaseShader& operator=(const BaseShader&) = delete;
		BaseShader(BaseShader&&) = delete;
		BaseShader& operator=(BaseShader&&) = delete;
	protected:
		REX::W32::ID3D11Device* device{ nullptr };
		REX::W32::ID3D11DeviceContext* deviceContext{ nullptr };
	public:
		explicit BaseShader(REX::W32::ID3D11Device* a_device, REX::W32::ID3D11DeviceContext* a_deviceContext);
		virtual ~BaseShader() = default;
	};

	class ObjectShader :
		public BaseShader
	{
		ObjectShader(const ObjectShader&) = delete;
		ObjectShader& operator=(const ObjectShader&) = delete;
		ObjectShader(ObjectShader&&) = delete;
		ObjectShader& operator=(ObjectShader&&) = delete;
	public:
		explicit ObjectShader(REX::W32::ID3D11Device* a_device, REX::W32::ID3D11DeviceContext* a_deviceContext);
		virtual ~ObjectShader() = default;

		virtual void InitPipeline(uint32_t, uint32_t) noexcept = 0;
		virtual void ShutdownPipeline() noexcept = 0;
	};

	class ResourceView :
		public ObjectShader
	{
		uint32_t ipType{ 0 }, bindId[3]{};
		REX::W32::D3D11_SHADER_RESOURCE_VIEW_DESC desc{};
		Microsoft::WRL::ComPtr<REX::W32::ID3D11ShaderResourceView> resourceView{};

		ResourceView(const ResourceView&) = delete;
		ResourceView& operator=(const ResourceView&) = delete;
		ResourceView(ResourceView&&) = delete;
		ResourceView& operator=(ResourceView&&) = delete;
	public:
		explicit ResourceView(REX::W32::ID3D11Device* a_device, REX::W32::ID3D11DeviceContext* a_deviceContext);
		virtual ~ResourceView() = default;

		virtual void DebugInfo() const noexcept;
		virtual bool Create(const REX::W32::ID3D11Resource* a_resource, const REX::W32::D3D11_SHADER_RESOURCE_VIEW_DESC* a_desc) noexcept;
		virtual bool Create(const REX::W32::ID3D11ShaderResourceView* a_view) noexcept;
		virtual void InitPipeline(uint32_t a_type, uint32_t a_bindID) noexcept;
		virtual void ShutdownPipeline() noexcept;

		[[nodiscard]] virtual bool Empty() const noexcept;
		[[nodiscard]] virtual REX::W32::ID3D11ShaderResourceView* Get() const noexcept;
		[[nodiscard]] virtual REX::W32::ID3D11ShaderResourceView** GetAddressOf() noexcept;
		[[nodiscard]] virtual REX::W32::ID3D11ShaderResourceView** ReleaseAndGetAddressOf() noexcept;
		[[nodiscard]] virtual const REX::W32::D3D11_SHADER_RESOURCE_VIEW_DESC* GetDesc() const noexcept;
	};

	class TextureShader :
		public BaseShader
	{
		REX::W32::D3D11_TEXTURE2D_DESC desc{};
		Microsoft::WRL::ComPtr<REX::W32::ID3D11Texture2D> texture{};

		TextureShader(const TextureShader&) = delete;
		TextureShader& operator=(const TextureShader&) = delete;
		TextureShader(TextureShader&&) = delete;
		TextureShader& operator=(TextureShader&&) = delete;
	public:
		explicit TextureShader(REX::W32::ID3D11Device* a_device, REX::W32::ID3D11DeviceContext* a_deviceContext);
		virtual ~TextureShader() = default;

		[[nodiscard]] virtual bool LoadFromFile(const char* a_filename, ResourceView* a_resource) noexcept;
		[[nodiscard]] virtual bool LoadFromStream(const void* a_buffer, uint32_t a_bufferSize,
			ResourceView* a_resource, bool a_wic = false) noexcept;
		[[nodiscard]] virtual bool LoadFromResource(const char* a_type, uint32_t a_resourceID,
			ResourceView* a_resource, bool a_wic = false) noexcept;
		[[nodiscard]] virtual bool SaveToFile(const char* a_filename) noexcept;

		virtual void DebugInfo() const noexcept;
		virtual bool Create(const REX::W32::D3D11_TEXTURE2D_DESC* a_desc) noexcept;
		virtual bool Create(const REX::W32::ID3D11Texture2D* a_texture, uint32_t a_bindFlags = REX::W32::D3D11_BIND_SHADER_RESOURCE) noexcept;
		virtual bool Create(const REX::W32::ID3D11Resource* a_resource, uint32_t a_bindFlags = REX::W32::D3D11_BIND_SHADER_RESOURCE) noexcept;

		virtual bool CopyFrom(const TextureShader* a_texture) noexcept;
		virtual bool CopyFrom(const REX::W32::ID3D11Texture2D* a_texture) noexcept;
		virtual bool CopyFrom(const REX::W32::ID3D11Resource* a_resource) noexcept;

		[[nodiscard]] virtual bool Empty() const noexcept;
		[[nodiscard]] virtual REX::W32::ID3D11Texture2D* Get() const noexcept;
		[[nodiscard]] virtual REX::W32::ID3D11Texture2D** GetAddressOf() noexcept;
		[[nodiscard]] virtual REX::W32::ID3D11Texture2D** ReleaseAndGetAddressOf() noexcept;
		[[nodiscard]] virtual const REX::W32::D3D11_TEXTURE2D_DESC* GetDesc() const noexcept;
	};

	class BufferShader :
		public ObjectShader
	{
		uint32_t ipType{ 0 }, bindId[3]{};
		REX::W32::D3D11_BUFFER_DESC desc{};
		Microsoft::WRL::ComPtr<REX::W32::ID3D11Buffer> buffer{};

		BufferShader(const BufferShader&) = delete;
		BufferShader& operator=(const BufferShader&) = delete;
		BufferShader(BufferShader&&) = delete;
		BufferShader& operator=(BufferShader&&) = delete;
	public:
		explicit BufferShader(REX::W32::ID3D11Device* a_device, REX::W32::ID3D11DeviceContext* a_deviceContext);
		virtual ~BufferShader() = default;

		virtual bool Create(const void* a_buffer, uint32_t a_size, uint32_t a_bindFlags,
			REX::W32::D3D11_USAGE a_usage = REX::W32::D3D11_USAGE_DEFAULT) noexcept;
		virtual void InitPipeline(uint32_t a_type, uint32_t a_bindId) noexcept override;
		virtual void ShutdownPipeline() noexcept override;

		[[nodiscard]] virtual REX::W32::ID3D11Buffer* Get() const noexcept;
		[[nodiscard]] virtual REX::W32::ID3D11Buffer** GetAddressOf() noexcept;
		[[nodiscard]] virtual REX::W32::ID3D11Buffer** ReleaseAndGetAddressOf() noexcept;
		[[nodiscard]] virtual const REX::W32::D3D11_BUFFER_DESC* GetDesc() const noexcept;
	};

	class UnorderedAccessView :
		public ObjectShader
	{
		uint32_t counts{ 0 }, ipType{ 0 }, bindId{ 0 };
		REX::W32::D3D11_UNORDERED_ACCESS_VIEW_DESC desc{};
		Microsoft::WRL::ComPtr<REX::W32::ID3D11UnorderedAccessView> view{};

		UnorderedAccessView(const UnorderedAccessView&) = delete;
		UnorderedAccessView& operator=(const UnorderedAccessView&) = delete;
		UnorderedAccessView(UnorderedAccessView&&) = delete;
		UnorderedAccessView& operator=(UnorderedAccessView&&) = delete;
	public:
		explicit UnorderedAccessView(REX::W32::ID3D11Device* a_device, REX::W32::ID3D11DeviceContext* a_deviceContext);
		virtual ~UnorderedAccessView() = default;

		virtual void DebugInfo() const noexcept;
		virtual bool Create(const REX::W32::ID3D11Resource* a_resource, const REX::W32::D3D11_UNORDERED_ACCESS_VIEW_DESC* a_desc) noexcept;
		virtual bool Create(const REX::W32::ID3D11UnorderedAccessView* a_view) noexcept;
		virtual void InitPipeline(uint32_t a_type, uint32_t a_bindID) noexcept;
		virtual void ShutdownPipeline() noexcept;

		[[nodiscard]] virtual bool Empty() const noexcept;
		[[nodiscard]] virtual REX::W32::ID3D11UnorderedAccessView* Get() const noexcept;
		[[nodiscard]] virtual REX::W32::ID3D11UnorderedAccessView** GetAddressOf() noexcept;
		[[nodiscard]] virtual REX::W32::ID3D11UnorderedAccessView** ReleaseAndGetAddressOf() noexcept;
		[[nodiscard]] virtual const REX::W32::D3D11_UNORDERED_ACCESS_VIEW_DESC* GetDesc() const noexcept;
	};

	class SamplerState :
		public ObjectShader
	{
		std::uint32_t ipType{ 0 }, bindId[3]{};
		REX::W32::D3D11_SAMPLER_DESC desc{};
		Microsoft::WRL::ComPtr<REX::W32::ID3D11SamplerState> samplerState{};

		SamplerState(const SamplerState&) = delete;
		SamplerState& operator=(const SamplerState&) = delete;
		SamplerState(SamplerState&&) = delete;
		SamplerState& operator=(SamplerState&&) = delete;
	public:
		explicit SamplerState(REX::W32::ID3D11Device* a_device, REX::W32::ID3D11DeviceContext* a_deviceContext);
		virtual ~SamplerState() = default;

		virtual void DebugInfo() const noexcept;
		virtual bool Create(const REX::W32::D3D11_SAMPLER_DESC* a_desc) noexcept;
		virtual void InitPipeline(uint32_t _type, uint32_t a_bindID) noexcept;
		virtual void ShutdownPipeline() noexcept;

		[[nodiscard]] virtual bool Empty() const noexcept;
		[[nodiscard]] virtual REX::W32::ID3D11SamplerState* Get() const noexcept;
		[[nodiscard]] virtual REX::W32::ID3D11SamplerState** GetAddressOf() noexcept;
		[[nodiscard]] virtual REX::W32::ID3D11SamplerState** ReleaseAndGetAddressOf() noexcept;
		[[nodiscard]] virtual const REX::W32::D3D11_SAMPLER_DESC* GetDesc() const noexcept;
	};

	class RenderTargetView :
		public BaseShader
	{
		REX::W32::D3D11_RENDER_TARGET_VIEW_DESC desc{};
		Microsoft::WRL::ComPtr<REX::W32::ID3D11RenderTargetView> renderTarget{};

		RenderTargetView(const RenderTargetView&) = delete;
		RenderTargetView& operator=(const RenderTargetView&) = delete;
		RenderTargetView(RenderTargetView&&) = delete;
		RenderTargetView& operator=(RenderTargetView&&) = delete;
	public:
		explicit RenderTargetView(REX::W32::ID3D11Device* a_device, REX::W32::ID3D11DeviceContext* a_deviceContext);
		virtual ~RenderTargetView() = default;

		virtual void DebugInfo() const noexcept;
		virtual bool Create(const REX::W32::ID3D11Resource* a_resource, const REX::W32::D3D11_RENDER_TARGET_VIEW_DESC* a_desc) noexcept;
		virtual bool Create(const REX::W32::ID3D11RenderTargetView* a_view) noexcept;

		[[nodiscard]] virtual bool Empty() const noexcept;
		[[nodiscard]] virtual REX::W32::ID3D11RenderTargetView* Get() const noexcept;
		[[nodiscard]] virtual REX::W32::ID3D11RenderTargetView** GetAddressOf() noexcept;
		[[nodiscard]] virtual REX::W32::ID3D11RenderTargetView** ReleaseAndGetAddressOf() noexcept;
		[[nodiscard]] virtual const REX::W32::D3D11_RENDER_TARGET_VIEW_DESC* GetDesc() const noexcept;
	};

	class DepthStencilView :
		public BaseShader
	{
		REX::W32::D3D11_DEPTH_STENCIL_VIEW_DESC desc{};
		Microsoft::WRL::ComPtr<REX::W32::ID3D11DepthStencilView> depthTarget{};

		DepthStencilView(const DepthStencilView&) = delete;
		DepthStencilView& operator=(const DepthStencilView&) = delete;
		DepthStencilView(DepthStencilView&&) = delete;
		DepthStencilView& operator=(DepthStencilView&&) = delete;
	public:
		explicit DepthStencilView(REX::W32::ID3D11Device* a_device, REX::W32::ID3D11DeviceContext* a_deviceContext);
		virtual ~DepthStencilView() = default;

		virtual void DebugInfo() const noexcept;
		virtual bool Create(const REX::W32::ID3D11Resource* a_resource, const REX::W32::D3D11_DEPTH_STENCIL_VIEW_DESC* a_desc) noexcept;
		virtual bool Create(const REX::W32::ID3D11DepthStencilView* a_view) noexcept;

		[[nodiscard]] virtual bool Empty() const noexcept;
		[[nodiscard]] virtual REX::W32::ID3D11DepthStencilView* Get() const noexcept;
		[[nodiscard]] virtual REX::W32::ID3D11DepthStencilView** GetAddressOf() noexcept;
		[[nodiscard]] virtual REX::W32::ID3D11DepthStencilView** ReleaseAndGetAddressOf() noexcept;
		[[nodiscard]] virtual const REX::W32::D3D11_DEPTH_STENCIL_VIEW_DESC* GetDesc() const noexcept;
	};

	class Shader :
		public BaseShader,
		public F4SE::IBufferStream
	{
		Shader(const Shader&) = delete;
		Shader& operator=(const Shader&) = delete;
		Shader(Shader&&) = delete;
		Shader& operator=(Shader&&) = delete;
	public:
		explicit Shader(REX::W32::ID3D11Device* a_device, REX::W32::ID3D11DeviceContext* a_deviceContext);
		virtual ~Shader();

		virtual void Bind() noexcept = 0;
		virtual void Unbind() noexcept = 0;
		virtual bool Install() noexcept = 0;
		virtual void Shutdown() noexcept = 0;
		virtual void Release() noexcept;

		[[nodiscard]] virtual bool LoadFromResource(uint32_t a_resourceID) noexcept;
		[[nodiscard]] virtual bool LoadFromResource(const char* a_resourceName) noexcept;
		[[nodiscard]] virtual bool Empty() const noexcept;
	};

	class PixelShader :
		public Shader
	{
		Microsoft::WRL::ComPtr<REX::W32::ID3D11PixelShader> pixelShader{};

		PixelShader(const PixelShader&) = delete;
		PixelShader& operator=(const PixelShader&) = delete;
		PixelShader(PixelShader&&) = delete;
		PixelShader& operator=(PixelShader&&) = delete;
	public:
		explicit PixelShader(REX::W32::ID3D11Device* a_device, REX::W32::ID3D11DeviceContext* a_deviceContext);
		virtual ~PixelShader() = default;

		virtual void Bind() noexcept;
		virtual void Unbind() noexcept;
		virtual bool Install() noexcept;
		virtual void Shutdown() noexcept;
	};

	class VertexShader :
		public Shader
	{
		Microsoft::WRL::ComPtr<REX::W32::ID3D11VertexShader> vertexShader{};
		Microsoft::WRL::ComPtr<REX::W32::ID3D11InputLayout> inputLayout{};
		std::unique_ptr<BufferShader> inputBuffer{};
		std::unique_ptr<BufferShader> indexesBuffer{};
		uint32_t sizeBuffer{ 0 };
		uint32_t strideBuffer{ 0 };
		uint32_t offsetBuffer{ 0 };
		REX::W32::D3D_PRIMITIVE_TOPOLOGY topology{ REX::W32::D3D_PRIMITIVE_TOPOLOGY_UNDEFINED };

		VertexShader(const VertexShader&) = delete;
		VertexShader& operator=(const VertexShader&) = delete;
		VertexShader(VertexShader&&) = delete;
		VertexShader& operator=(VertexShader&&) = delete;
	public:
		explicit VertexShader(REX::W32::ID3D11Device* a_device, REX::W32::ID3D11DeviceContext* a_deviceContext);
		virtual ~VertexShader() = default;

		virtual void Bind() noexcept;
		virtual void Unbind() noexcept;
		virtual bool Install() noexcept;
		virtual void Shutdown() noexcept;

		virtual bool InitData(REX::W32::D3D_PRIMITIVE_TOPOLOGY a_topology, const std::initializer_list<Graphics::Vec2>& a_inputData) noexcept;
		virtual bool InitDataWithIndexes(REX::W32::D3D_PRIMITIVE_TOPOLOGY a_topology, const std::initializer_list<Graphics::Vec2>& a_inputData,
			const std::initializer_list<std::uint32_t>& a_indexes) noexcept;
	};

	class ComputeShader :
		public Shader
	{
		Microsoft::WRL::ComPtr<REX::W32::ID3D11ComputeShader> computeShader;

		ComputeShader(const ComputeShader&) = delete;
		ComputeShader& operator=(const ComputeShader&) = delete;
		ComputeShader(ComputeShader&&) = delete;
		ComputeShader& operator=(ComputeShader&&) = delete;
	public:
		ComputeShader(REX::W32::ID3D11Device* a_device, REX::W32::ID3D11DeviceContext* a_deviceContext);
		virtual ~ComputeShader() = default;

		virtual void Dispatch(uint32_t a_threadGroupCountX, uint32_t a_threadGroupCountY, uint32_t a_threadGroupCountZ) noexcept;
		virtual void Bind() noexcept;
		virtual void Unbind() noexcept;
		virtual bool Install() noexcept;
		virtual void Shutdown() noexcept;
	};
}