#include <F4SE/IDataStream.h>
#include <cstring>
#include <cassert>

/**** IDataStream *************************************************************/

uint8_t F4SE::IDataStream::Read8() noexcept
{
	uint8_t	out;
	ReadBuf(&out, sizeof(uint8_t));
	return out;
}

uint16_t F4SE::IDataStream::Read16() noexcept
{
	uint16_t out{};
	ReadBuf(&out, sizeof(uint16_t));
	return out;
}

uint32_t F4SE::IDataStream::Read32() noexcept
{
	uint32_t out{};
	ReadBuf(&out, sizeof(uint32_t));
	return out;
}

uint64_t F4SE::IDataStream::Read64() noexcept
{
	uint64_t out;
	ReadBuf(&out, sizeof(uint64_t));
	return out;
}

float F4SE::IDataStream::ReadFloat() noexcept
{
	uint32_t out = Read32();
	return *((float*)&out);
}

uint32_t F4SE::IDataStream::ReadString(char* a_buf, uint32_t a_bufLength, char a_altTerminator, char a_altTerminator2) noexcept
{
	char* traverse = a_buf;
	bool breakOnReturns = false;

	if((a_altTerminator == '\n') || (a_altTerminator2 == '\n'))
		breakOnReturns = true;

	assert(a_bufLength > 0);
	if(a_bufLength == 1)
	{
		a_buf[0] = 0;
		return 0;
	}

	a_bufLength--;

	for(uint32_t i = 0; i < a_bufLength; i++)
	{
		if(HitEOF()) break;

		uint8_t	data = Read8();

		if(breakOnReturns)
		{
			if(data == 0x0D)
			{
				if(Peek8() == 0x0A)
					Skip(1);

				break;
			}
		}

		if(!data || (data == a_altTerminator) || (data == a_altTerminator2))
			break;

		*traverse++ = data;
	}

	*traverse++ = 0;

	return (uint32_t)(traverse - a_buf - 1);
}

uint8_t F4SE::IDataStream::Peek8() noexcept
{
	IDataStream_PositionSaver saver(this);
	return Read8();
}

uint16_t F4SE::IDataStream::Peek16() noexcept
{
	IDataStream_PositionSaver saver(this);
	return Read16();
}

uint32_t F4SE::IDataStream::Peek32() noexcept
{
	IDataStream_PositionSaver saver(this);

	return Read32();
}

uint64_t F4SE::IDataStream::Peek64() noexcept
{
	IDataStream_PositionSaver saver(this);
	return Read64();
}

float F4SE::IDataStream::PeekFloat() noexcept
{
	IDataStream_PositionSaver saver(this);
	return ReadFloat();
}

void F4SE::IDataStream::PeekBuf(void* a_buf, uint32_t a_inLength) noexcept
{
	IDataStream_PositionSaver saver(this);
	ReadBuf(a_buf, a_inLength);
}

void F4SE::IDataStream::WriteString(const char * a_buf) noexcept
{
	WriteBuf(a_buf, (uint32_t)std::strlen(a_buf));
}

const F4SE::IDataStream* F4SE::IDataStream::GetRootParent() const noexcept
{
	IDataStream* parent = GetParent();
	return (parent) ? parent->GetRootParent() : this;
}

void F4SE::IDataStream::CopyStreams(IDataStream* a_out, IDataStream* a_in, uint64_t a_bufferSize, uint8_t* a_buf) noexcept
{
	a_in->Rewind();
	bool ourBuffer = false;

	if(!a_buf)
	{
		a_buf = new uint8_t[a_bufferSize];
		ourBuffer = true;
	}

	uint64_t remain = a_in->GetLength();
	while(remain > 0)
	{
		uint64_t transferSize = remain;

		if(transferSize > a_bufferSize)
			transferSize = a_bufferSize;

		a_in->ReadBuf(a_buf, (uint32_t)transferSize);
		a_out->WriteBuf(a_buf, (uint32_t)transferSize);

		remain -= transferSize;
	}

	if(ourBuffer)
		delete[] a_buf;
}

void F4SE::IDataStream::CopySubStreams(IDataStream* a_out, IDataStream* a_in, uint64_t a_remain, uint64_t a_bufferSize,
	uint8_t* a_buf) noexcept
{
	bool ourBuffer = false;

	if(!a_buf)
	{
		a_buf = new uint8_t[a_bufferSize];
		ourBuffer = true;
	}

	while(a_remain > 0)
	{
		uint64_t transferSize = a_remain;

		if(transferSize > a_bufferSize)
			transferSize = a_bufferSize;

		a_in->ReadBuf(a_buf, (uint32_t)transferSize);
		a_out->WriteBuf(a_buf, (uint32_t)transferSize);

		a_remain -= transferSize;
	}

	if(ourBuffer)
		delete[] a_buf;
}

/**** IDataStream_PositionSaver ***********************************************/

F4SE::IDataStream_PositionSaver::IDataStream_PositionSaver(IDataStream* a_tgt)
{
	stream = a_tgt;
	offset = a_tgt->GetOffset();
}

F4SE::IDataStream_PositionSaver::~IDataStream_PositionSaver()
{
	stream->SetOffset(offset);
}

/**** IDataSubStream **********************************************************/

F4SE::IDataSubStream::IDataSubStream(IDataStream* a_inStream, int64_t a_inOffset, int64_t a_inLength)
{
	Attach(a_inStream, a_inOffset, a_inLength);
}

void F4SE::IDataSubStream::Attach(IDataStream* a_inStream, int64_t a_inOffset, int64_t a_inLength) noexcept
{
	stream = a_inStream;
	subBase = a_inOffset;
	streamLength = a_inLength;
	stream->SetOffset(a_inOffset);
}

void F4SE::IDataSubStream::ReadBuf(void* a_buf, uint32_t a_inLength) noexcept
{
	assert(a_inLength <= GetRemain());

	if(stream->GetOffset() != subBase + streamOffset)
		stream->SetOffset(subBase + streamOffset);

	stream->ReadBuf(a_buf, a_inLength);
	streamOffset += a_inLength;
}

void F4SE::IDataSubStream::WriteBuf(const void* a_buf, uint32_t a_inLength) noexcept
{
	if(stream->GetOffset() != subBase + streamOffset)
		stream->SetOffset(subBase + streamOffset);

	stream->WriteBuf(a_buf, a_inLength);
	streamOffset += a_inLength;

	if(streamLength < streamOffset)
		streamLength = streamOffset;
}

void F4SE::IDataSubStream::SetOffset(int64_t a_inOffset) noexcept
{
	stream->SetOffset(subBase + a_inOffset);
	streamOffset = a_inOffset;
}
