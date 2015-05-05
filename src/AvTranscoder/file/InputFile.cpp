#include "InputFile.hpp"

#include <AvTranscoder/mediaProperty/util.hpp>
#include <AvTranscoder/mediaProperty/VideoProperties.hpp>
#include <AvTranscoder/mediaProperty/AudioProperties.hpp>
#include <AvTranscoder/mediaProperty/DataProperties.hpp>
#include <AvTranscoder/mediaProperty/SubtitleProperties.hpp>
#include <AvTranscoder/mediaProperty/AttachementProperties.hpp>
#include <AvTranscoder/mediaProperty/UnknownProperties.hpp>

extern "C" {
#include <libavcodec/avcodec.h>
#include <libavutil/avutil.h>
#include <libavutil/pixdesc.h>
}

#include <stdexcept>
#include <sstream>

namespace avtranscoder
{

InputFile::InputFile( const std::string& filename )
	: _formatContext( filename, AV_OPT_FLAG_DECODING_PARAM )
	, _properties( _formatContext )
	, _filename( filename )
	, _inputStreams()
{
	_formatContext.findStreamInfo();

	// Create streams
	for( size_t streamIndex = 0; streamIndex < _formatContext.getNbStreams(); ++streamIndex )
	{
		_inputStreams.push_back( new InputStream( *this, streamIndex ) );
	}
}

InputFile::~InputFile()
{
	for( std::vector< InputStream* >::iterator it = _inputStreams.begin(); it != _inputStreams.end(); ++it )
	{
		delete (*it);
	}
}

void InputFile::analyse( IProgress& progress, const EAnalyseLevel level )
{
	_properties.extractStreamProperties( progress, level );
}

FileProperties InputFile::analyseFile( const std::string& filename, IProgress& progress, const EAnalyseLevel level )
{
	InputFile file( filename );
	file.analyse( progress, level );
	return file.getProperties();
}

bool InputFile::readNextPacket( CodedData& data, const size_t streamIndex )
{
	bool nextPacketFound = false;
	while( ! nextPacketFound )
	{
		int ret = av_read_frame( &_formatContext.getAVFormatContext(), &data.getAVPacket() );
		if( ret < 0 ) // error or end of file
		{
			return false;
		}

		// if the packet stream is the expected one
		// return the packet data
		int packetStreamIndex = data.getAVPacket().stream_index;
		if( packetStreamIndex == (int)streamIndex )
		{
			nextPacketFound = true;
		}
		// else add the packet data to the stream cache
		else
		{
			_inputStreams.at( packetStreamIndex )->addPacket( data.getAVPacket() );
			data.clear();
		}
	}
	return true;
}

void InputFile::seekAtFrame( const size_t frame )
{
	uint64_t position = frame / getFps() * AV_TIME_BASE;
	_formatContext.seek( position, AVSEEK_FLAG_ANY );
}

void InputFile::seekAtTime( const double time )
{
	uint64_t position = time * AV_TIME_BASE;
	_formatContext.seek( position, AVSEEK_FLAG_ANY );
}

void InputFile::activateStream( const size_t streamIndex, bool activate )
{
	getStream( streamIndex ).activate( activate );
}

InputStream& InputFile::getStream( size_t index )
{
	try
	{
		return *_inputStreams.at( index );
	}
	catch( const std::out_of_range& e )
	{
		std::stringstream msg;
		msg << getFilename();
		msg << " has no stream at index ";
		msg << index;
		throw std::runtime_error( msg.str() );
	}
}

double InputFile::getFps()
{
	double fps = 1;
	if( _properties.getNbVideoStreams() )
		fps = _properties.getVideoProperties().at( 0 ).getFps();
	return fps;
}

void InputFile::setProfile( const ProfileLoader::Profile& profile )
{
	LOG_DEBUG( "Set profile of input file with:\n" << profile )

	for( ProfileLoader::Profile::const_iterator it = profile.begin(); it != profile.end(); ++it )
	{
		if( (*it).first == constants::avProfileIdentificator ||
			(*it).first == constants::avProfileIdentificatorHuman ||
			(*it).first == constants::avProfileType )
			continue;
		
		try
		{
			Option& formatOption = _formatContext.getOption( (*it).first );
			formatOption.setString( (*it).second );
		}
		catch( std::exception& e )
		{
			LOG_WARN( "InputFile - can't set option " << (*it).first <<  " to " << (*it).second << ": " << e.what() )
		}
	}
}

}