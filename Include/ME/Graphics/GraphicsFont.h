#pragma once

#include <ME/Core.h>
#include <ME/Graphics/GraphicsMath.h>
#include <string>
#include <memory>

namespace ME
{
	namespace Graphics
	{
		class GraphicsFont : 
			public std::enable_shared_from_this<GraphicsFont>
		{
			ImFont* font{ nullptr };

			GraphicsFont(const GraphicsFont&) = delete;
			GraphicsFont(GraphicsFont&&) = delete;
			GraphicsFont& operator=(const GraphicsFont&) = delete;
			GraphicsFont& operator=(GraphicsFont&&) = delete;
		public:
			constexpr GraphicsFont() = default;
			virtual ~GraphicsFont();

			[[nodiscard]] virtual bool OpenFromFile(const std::string& a_filename, float a_size) noexcept;
			[[nodiscard]] virtual bool OpenFromStream(const F4SE::IBufferStream& a_stream, float a_size) noexcept;
			[[nodiscard]] virtual bool OpenFromResource(const char* a_type, uint32_t a_resourceID, float a_size) noexcept;

			virtual void Push() noexcept;
			virtual void Pop() noexcept;
			virtual void Release() noexcept;

			[[nodiscard]] virtual inline ImFont* Handle() const noexcept { return font; }
		};
	}
}

