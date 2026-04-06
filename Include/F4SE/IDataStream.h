#pragma once

#include <cstdint>

namespace F4SE
{
	class IDataStream
	{
	public:
		IDataStream() = default;
		virtual ~IDataStream() = default;

		[[nodiscard]] virtual uint8_t	Read8() noexcept;
		[[nodiscard]] virtual uint16_t	Read16() noexcept;
		[[nodiscard]] virtual uint32_t	Read32() noexcept;
		[[nodiscard]] virtual uint64_t	Read64() noexcept;
		[[nodiscard]] virtual float		ReadFloat() noexcept;
		virtual uint32_t				ReadString(char* a_buf, uint32_t a_bufLength, char a_altTerminator = 0, char a_altTerminator2 = 0) noexcept;
		virtual void					ReadBuf(void* a_buf, uint32_t a_inLength) noexcept = 0;

		virtual uint8_t			Peek8() noexcept;
		virtual uint16_t		Peek16() noexcept;
		virtual uint32_t		Peek32() noexcept;
		virtual uint64_t		Peek64() noexcept;
		virtual float			PeekFloat() noexcept;
		virtual void			PeekBuf(void* a_buf, uint32_t a_inLength) noexcept;

		virtual inline void		Skip(int64_t a_inBytes) noexcept { SetOffset(GetOffset() + a_inBytes); }

		virtual inline void		Write8(uint8_t a_inData) noexcept { WriteBuf(&a_inData, sizeof(uint8_t)); }
		virtual inline void		Write16(uint16_t a_inData) noexcept { WriteBuf(&a_inData, sizeof(uint16_t)); }
		virtual inline void		Write32(uint32_t a_inData) noexcept { WriteBuf(&a_inData, sizeof(uint32_t)); }
		virtual inline void		Write64(uint64_t a_inData) noexcept { WriteBuf(&a_inData, sizeof(uint64_t)); }
		virtual inline void		WriteFloat(float a_inData) noexcept { WriteBuf(&a_inData, sizeof(float)); }
		virtual void			WriteString(const char* a_buf) noexcept;
		virtual void			WriteBuf(const void* a_buf, uint32_t a_inLength) noexcept = 0;

		[[nodiscard]] virtual inline int64_t	GetLength() const noexcept { return streamLength; }
		[[nodiscard]] virtual inline int64_t	GetRemain() const noexcept { return streamLength - streamOffset; }
		[[nodiscard]] virtual inline int64_t	GetOffset() const noexcept { return streamOffset; }
		[[nodiscard]] virtual inline bool		HitEOF() const noexcept { return streamOffset >= streamLength; }

		virtual inline void		SetOffset(int64_t a_inOffset) noexcept { streamOffset = a_inOffset; }
		virtual inline void		Rewind() noexcept { SetOffset(0); }

		[[nodiscard]] virtual int64_t		GetParentOffset() const noexcept { return GetOffset(); }
		[[nodiscard]] virtual IDataStream*	GetParent() const noexcept { return nullptr; }

		const IDataStream* GetRootParent() const noexcept;

		static void	CopyStreams(IDataStream* a_out, IDataStream* a_in, uint64_t a_bufferSize = 1024 * 1024,
			uint8_t* a_buf = nullptr) noexcept;
		static void	CopySubStreams(IDataStream* a_out, IDataStream* a_in, uint64_t a_remain, uint64_t a_bufferSize = 1024 * 1024,
			uint8_t* a_buf = nullptr) noexcept;

	protected:
		int64_t	streamLength{ 0 };
		int64_t	streamOffset{ 0 };
	};

	class IDataStream_PositionSaver
	{
		IDataStream*	stream{ nullptr };
		int64_t			offset{ 0 };
	public:
		IDataStream_PositionSaver(IDataStream* a_tgt);
		~IDataStream_PositionSaver();
	};

	class IDataSubStream : public IDataStream
	{
		IDataStream*	stream{ nullptr };
		int64_t			subBase{ 0 };
	public:
		IDataSubStream() = default;
		IDataSubStream(IDataStream* a_inStream, int64_t a_inOffset, int64_t a_inLength);
		virtual ~IDataSubStream() = default;

		virtual void	Attach(IDataStream* a_inStream, int64_t a_inOffset, int64_t a_inLength) noexcept;

		virtual void	ReadBuf(void* a_buf, uint32_t a_inLength) noexcept override;
		virtual void	WriteBuf(const void* a_buf, uint32_t a_inLength) noexcept override;
		virtual void	SetOffset(int64_t a_inOffset) noexcept override;

		[[nodiscard]] virtual inline int64_t		GetParentOffset() const noexcept { return stream->GetOffset(); }
		[[nodiscard]] virtual inline IDataStream*	GetParent() const noexcept { return stream; }
		[[nodiscard]] virtual inline int64_t		GetSubBase() const noexcept { return subBase; }
	};
}