#pragma once

#include <F4SE/IDataStream.h>
#include <algorithm>

namespace F4SE
{
	class IBufferStream : public IDataStream
	{
	public:
		IBufferStream() = default;
		IBufferStream([[maybe_unused]] const IBufferStream& a_rhs);
		IBufferStream(void* a_buf, uint64_t a_inLength);
		virtual ~IBufferStream();

		IBufferStream& operator=(const IBufferStream& a_rhs);

		virtual void				SetBuffer(void* a_buf, uint64_t a_inLength) noexcept;
		[[nodiscard]] virtual void*	GetBuffer() const noexcept { return streamBuf; }

		virtual inline void	OwnBuffer() noexcept { flags |= std::to_underlying(Flags::kOwnedBuf); }
		virtual inline void	DisownBuffer() noexcept { flags &= ~std::to_underlying(Flags::kOwnedBuf); }

		virtual void	ReadBuf(void* a_buf, uint32_t a_inLength) noexcept override;
		virtual void	WriteBuf(const void* a_buf, uint32_t a_inLength) noexcept override;

	protected:
		uint8_t*	streamBuf{ nullptr };
		uint32_t	flags{ 0 };

		enum class Flags
		{
			kOwnedBuf = 1 << 0,
		};
	};
}
