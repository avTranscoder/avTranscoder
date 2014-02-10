#ifndef _AV_TRANSCODER_INPUT_STREAM_VIDEO_HPP_
#define _AV_TRANSCODER_INPUT_STREAM_VIDEO_HPP_

#include "InputFile.hpp"
#include "DatasStructures/Image.hpp"
#include "DatasStructures/DataStreamDesc.hpp"

#include <vector>

class AVFormatContext;
class AVCodec;
class AVCodecContext;
class AVFrame;

namespace avtranscoder
{

class InputStreamVideo : public InputFile
{
public:
	InputStreamVideo();
	~InputStreamVideo();

	bool setup( const std::string& filename, const size_t streamIndex );

	bool readNextFrame( Image& frameBuffer );

private:
	AVFormatContext* m_formatContext;
	AVCodec*         m_codec;
	AVCodecContext*  m_codecContext;
	AVFrame*         m_frame;

	int              m_selectedStream;
};

}

#endif