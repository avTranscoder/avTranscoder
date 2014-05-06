#include "Transcoder.hpp"

#include <AvTranscoder/InputFile.hpp>

namespace avtranscoder
{

Transcoder::Transcoder( const std::string& filename )
	: _outputFile( filename )
{
	_outputFile.setup();
}

Transcoder::Transcoder( OutputFile& outputFile )
	: _outputFile( outputFile )
{
	outputFile.setup();
}

void Transcoder::add( const std::string& filename, const size_t streamIndex )
{
	InputFile inputFile( filename );

	switch( inputFile.getStreamType( streamIndex ) )
	{
		case AVMEDIA_TYPE_VIDEO:
		{
			_inputStreams.push_back( avtranscoder::InputStream( filename, streamIndex ) );
			_outputFile.addVideoStream( _inputStreams.back().getVideoDesc() );
			break;
		}
		case AVMEDIA_TYPE_AUDIO:
		{
			_inputStreams.push_back( avtranscoder::InputStream( filename, streamIndex ) );
			_outputFile.addAudioStream( _inputStreams.back().getAudioDesc() );
			break;
		}
		case AVMEDIA_TYPE_DATA:
		case AVMEDIA_TYPE_SUBTITLE:
		case AVMEDIA_TYPE_ATTACHMENT:
		default:
		{
			return;
		}
	}
}

void Transcoder::add( const StreamsDefinition& streams )
{
	for( size_t streamIndex = 0; streamIndex < streams.size(); ++streamIndex )
	{
		add( streams.at( streamIndex ).first, streams.at( streamIndex ).second );
	}
}

void Transcoder::process()
{
	size_t frame = 0;

	std::vector< DataStream > dataStreams;

	dataStreams.reserve( _inputStreams.size() );

	for( size_t streamIndex = 0; streamIndex < _inputStreams.size(); ++streamIndex )
	{
		DataStream dataStream;
		dataStreams.push_back( dataStream );
	}

	bool continueProcess( true );

	while( 1 )
	{
		// read one frame for each streamIndex
		for( size_t streamIndex = 0; streamIndex < _inputStreams.size(); ++streamIndex )
		{
			bool ret = _inputStreams.at( streamIndex ).readNextPacket( dataStreams.at( streamIndex ) );

			if( ! ret || ( dataStreams.at( streamIndex ).getBuffer().size() == 0 ) )
				continueProcess = false;
		}

		if( ! continueProcess )
			break;

		std::cout << "\rprocess frame " << frame + 1 << std::flush;

		for( size_t streamIndex = 0; streamIndex < _inputStreams.size(); ++streamIndex )
		{
			_outputFile.wrap( dataStreams.at( streamIndex ), streamIndex );
		}

		++frame;
	}
}

}
