#include <F4SE/IBufferStream.h>
#include <cassert>

F4SE::IBufferStream::IBufferStream(const IBufferStream& a_rhs)
{
	// explicitly not supporting copy constructor for self-owned buffers
	assert((flags & std::to_underlying(Flags::kOwnedBuf)) == 0);

	SetBuffer(a_rhs.streamBuf, a_rhs.streamLength);
}

F4SE::IBufferStream::IBufferStream(void* a_buf, uint64_t a_inLength)
{
	SetBuffer(a_buf, a_inLength);
}

F4SE::IBufferStream::~IBufferStream()
{
	if(flags & std::to_underlying(Flags::kOwnedBuf))
		delete[] streamBuf;
}

F4SE::IBufferStream& F4SE::IBufferStream::operator=(const IBufferStream& rhs)
{
	// explicitly not supporting copying for self-owned buffers
	assert((flags & kFlag_OwnedBuf) == 0);

	streamBuf = rhs.streamBuf;
	flags = rhs.flags;

	return *this;
}

void F4SE::IBufferStream::SetBuffer(void* a_buf, uint64_t a_inLength) noexcept
{
	streamBuf = (uint8_t*)a_buf;
	streamLength = a_inLength;

	Rewind();
}

void F4SE::IBufferStream::ReadBuf(void* a_buf, uint32_t a_inLength) noexcept
{
	memcpy(a_buf, &streamBuf[streamOffset], a_inLength);
	streamOffset += a_inLength;
}

void F4SE::IBufferStream::WriteBuf(const void* a_buf, uint32_t a_inLength) noexcept
{
	memcpy(&streamBuf[streamOffset], a_buf, a_inLength);
	streamOffset += a_inLength;
}
