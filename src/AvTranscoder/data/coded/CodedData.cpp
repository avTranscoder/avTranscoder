#include "CodedData.hpp"

#include <cstring>

namespace avtranscoder
{

CodedData::CodedData()
    : _avStream(NULL)
{
    initAVPacket();
}

CodedData::CodedData(const size_t dataSize)
    : _avStream(NULL)
{
    const int err = av_new_packet(&_packet, dataSize);
    if(err != 0)
    {
        LOG_ERROR("Unable to allocate the payload of a packet and initialize its fields with default values: " << getDescriptionFromErrorCode(err))
        throw std::bad_alloc();
    }
}

CodedData::CodedData(const AVPacket& avPacket)
    : _avStream(NULL)
{
    copyAVPacket(avPacket);
}

CodedData::CodedData(const CodedData& other)
{
    copyAVPacket(other.getAVPacket());
    _avStream = other.getAVStream();
}

CodedData& CodedData::operator=(const CodedData& other)
{
    copyAVPacket(other.getAVPacket());
    _avStream = other.getAVStream();
    return *this;
}

CodedData::~CodedData()
{
    av_packet_unref(&_packet);
}

void CodedData::resize(const size_t newSize)
{
    if((int)newSize < _packet.size)
        av_shrink_packet(&_packet, newSize);
    else if((int)newSize > _packet.size)
        av_grow_packet(&_packet, newSize - _packet.size);
}

void CodedData::refData(unsigned char* buffer, const size_t size)
{
    _packet.data = buffer;
    _packet.size = size;
}

void CodedData::copyData(unsigned char* buffer, const size_t size)
{
    resize(size);
    if(size != 0)
        memcpy(_packet.data, buffer, _packet.size);
}

void CodedData::refData(CodedData& frame)
{
    _packet.data = frame.getData();
    _packet.size = frame.getSize();
}

void CodedData::clear()
{
    av_packet_unref(&_packet);
    initAVPacket();
}

void CodedData::assign(const size_t size, const int value)
{
    resize(size);
    memset(_packet.data, value, size);
}

void CodedData::initAVPacket()
{
    _packet = *av_packet_alloc();
    _packet.data = NULL;
    _packet.size = 0;
}

void CodedData::copyAVPacket(const AVPacket& avPacket)
{
#if AVTRANSCODER_FFMPEG_DEPENDENCY && LIBAVCODEC_VERSION_MAJOR > 57
    av_packet_ref(&_packet, &avPacket);
#elif AVTRANSCODER_FFMPEG_DEPENDENCY && LIBAVCODEC_VERSION_INT > AV_VERSION_INT(54, 56, 0)
    // Need const_cast<AVCodec*> for libav versions from 54.56. to 55.56.
    av_copy_packet(&_packet, const_cast<AVPacket*>(&avPacket));
#else
    // @todo: we just care about data, not side properties of AVPacket
    initAVPacket();
    copyData(avPacket.data, avPacket.size);
#endif
}
}
