#include "OutputFile.hpp"

#include <AvTranscoder/util.hpp>

#include <stdexcept>

namespace avtranscoder
{

OutputFile::OutputFile( const std::string& filename )
	: _formatContext( AV_OPT_FLAG_ENCODING_PARAM )
	, _outputStreams()
	, _frameCount()
	, _filename( filename )
	, _previousProcessedStreamDuration( 0.0 )
{
	_formatContext.setFilename( _filename );
	_formatContext.setOutputFormat( _filename );
}

OutputFile::~OutputFile()
{
	for( std::vector< OutputStream* >::iterator it = _outputStreams.begin(); it != _outputStreams.end(); ++it )
	{
		delete (*it);
	}
}

IOutputStream& OutputFile::addVideoStream( const VideoCodec& videoDesc )
{
	AVStream& stream = _formatContext.addAVStream( videoDesc.getAVCodec() );

	stream.codec->width  = videoDesc.getAVCodecContext().width;
	stream.codec->height = videoDesc.getAVCodecContext().height;
	stream.codec->bit_rate = videoDesc.getAVCodecContext().bit_rate;
	stream.codec->pix_fmt = videoDesc.getAVCodecContext().pix_fmt;
	stream.codec->profile = videoDesc.getAVCodecContext().profile;
	stream.codec->level = videoDesc.getAVCodecContext().level;

	// need to set the time_base on the AVCodecContext and the AVStream
	// compensating the frame rate with the ticks_per_frame and keeping
	// a coherent reading speed.
	av_reduce(
		&stream.codec->time_base.num,
		&stream.codec->time_base.den,
		videoDesc.getAVCodecContext().time_base.num * videoDesc.getAVCodecContext().ticks_per_frame,
		videoDesc.getAVCodecContext().time_base.den,
		INT_MAX );

	stream.time_base = stream.codec->time_base;

	OutputStream* avOutputStream = new OutputStream( *this, _formatContext.getNbStreams() - 1 );
	_outputStreams.push_back( avOutputStream );

	return *_outputStreams.back();
}

IOutputStream& OutputFile::addAudioStream( const AudioCodec& audioDesc )
{
	AVStream& stream = _formatContext.addAVStream( audioDesc.getAVCodec() );

	stream.codec->sample_rate = audioDesc.getAVCodecContext().sample_rate;
	stream.codec->channels = audioDesc.getAVCodecContext().channels;
	stream.codec->sample_fmt = audioDesc.getAVCodecContext().sample_fmt;

	OutputStream* avOutputStream = new OutputStream( *this, _formatContext.getNbStreams() - 1 );
	_outputStreams.push_back( avOutputStream );

	return *_outputStreams.back();
}

IOutputStream& OutputFile::addDataStream( const DataCodec& dataDesc )
{
	_formatContext.addAVStream( dataDesc.getAVCodec() );

	OutputStream* avOutputStream = new OutputStream( *this, _formatContext.getNbStreams() - 1 );
	_outputStreams.push_back( avOutputStream );

	return *_outputStreams.back();
}

IOutputStream& OutputFile::getStream( const size_t streamId )
{
	if( streamId >= _outputStreams.size() )
		throw std::runtime_error( "unable to get output stream (out of range)" );
	return *_outputStreams.at( streamId );
}

std::string OutputFile::getFormatName() const
{
	if( _formatContext.getAVOutputFormat().name == NULL )
	{
		LOG_WARN("Unknown muxer format name of '" << _filename << "'.")
		return "";
	}
	return std::string(_formatContext.getAVOutputFormat().name);
}

std::string OutputFile::getFormatLongName() const
{
	if( _formatContext.getAVOutputFormat().long_name == NULL )
	{
		LOG_WARN("Unknown muxer format long name of '" << _filename << "'.")
		return "";
	}
	return std::string(_formatContext.getAVOutputFormat().long_name);
}

std::string OutputFile::getFormatMimeType() const
{
	if( _formatContext.getAVOutputFormat().mime_type == NULL )
	{
		LOG_WARN("Unknown muxer format mime type of '" << _filename << "'.")
		return "";
	}
	return std::string(_formatContext.getAVOutputFormat().mime_type);
}

bool OutputFile::beginWrap( )
{
	LOG_DEBUG( "Begin wrap of OutputFile" )

	_formatContext.openRessource( _filename, AVIO_FLAG_WRITE );
	_formatContext.writeHeader();

	_frameCount.clear();
	_frameCount.resize( _outputStreams.size(), 0 );

	return true;
}

IOutputStream::EWrappingStatus OutputFile::wrap( const CodedData& data, const size_t streamId )
{
	if( ! data.getSize() )
		return IOutputStream::eWrappingSuccess;

	LOG_DEBUG( "Wrap on stream " << streamId << " (" << data.getSize() << " bytes for frame " << _frameCount.at( streamId ) << ")" )

	AVPacket packet;
	av_init_packet( &packet );
	packet.stream_index = streamId;
	packet.data = (uint8_t*)data.getData();
	packet.size = data.getSize();

	_formatContext.writeFrame( packet );

	// free packet.side_data, set packet.data to NULL and packet.size to 0
	av_free_packet( &packet );

	double currentStreamDuration = _outputStreams.at( streamId )->getStreamDuration();
	if( currentStreamDuration < _previousProcessedStreamDuration )
	{
		// if the current stream is strictly shorter than the previous, wait for more data
		return IOutputStream::eWrappingWaitingForData;
	}

	_previousProcessedStreamDuration = currentStreamDuration;
	
	_frameCount.at( streamId )++;
	return IOutputStream::eWrappingSuccess;
}

bool OutputFile::endWrap( )
{
	LOG_DEBUG( "End wrap of OutputFile" )

	_formatContext.writeTrailer();
	_formatContext.closeRessource();
	return true;
}

void OutputFile::addMetadata( const PropertyVector& data )
{
	for( PropertyVector::const_iterator it = data.begin(); it != data.end(); ++it )
	{
		addMetadata( it->first, it->second );
	}
}

void OutputFile::addMetadata( const std::string& key, const std::string& value )
{
	_formatContext.addMetaData( key, value );
}

void OutputFile::setupWrapping( const ProfileLoader::Profile& profile )
{
	LOG_DEBUG( "Set profile of output file with:\n" << profile )

	// check if output format indicated is valid with the filename extension
	if( ! matchFormat( profile.find( constants::avProfileFormat )->second, _filename ) )
	{
		throw std::runtime_error( "Invalid format according to the file extension." );
	}

	// set output format
	_formatContext.setOutputFormat( _filename, profile.find( constants::avProfileFormat )->second );

	// set format options
	for( ProfileLoader::Profile::const_iterator it = profile.begin(); it != profile.end(); ++it )
	{
		if( (*it).first == constants::avProfileIdentificator ||
			(*it).first == constants::avProfileIdentificatorHuman ||
			(*it).first == constants::avProfileType ||
			(*it).first == constants::avProfileFormat )
			continue;

		try
		{
			Option& formatOption = _formatContext.getOption( (*it).first );
			formatOption.setString( (*it).second );
		}
		catch( std::exception& e )
		{
			LOG_WARN( "OutputFile - can't set option " << (*it).first <<  " to " << (*it).second << ": " << e.what() )
		}
	}
}

}
