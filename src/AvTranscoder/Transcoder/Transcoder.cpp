
#include "Transcoder.hpp"
#include "StreamTranscoder.hpp"

#include <AvTranscoder/CodedStream/AvInputStream.hpp>

namespace avtranscoder
{

Transcoder::Transcoder( OutputFile& outputFile )
	: _outputFile( outputFile )
	, _profile( true )
{
	_outputFile.setup();
}

Transcoder::~Transcoder()
{
	for( std::vector< InputFile* >::iterator it = _inputFiles.begin(); it != _inputFiles.end(); ++it )
	{
		delete (*it);
	}

	for( std::vector< DummyInputStream* >::iterator it = _dummyInputStreams.begin(); it != _dummyInputStreams.end(); ++it )
	{
		delete (*it);
	}

	for( std::vector< StreamTranscoder* >::iterator it = _streamTranscoders.begin(); it != _streamTranscoders.end(); ++it )
	{
		delete (*it);
	}
}

void Transcoder::add( const std::string& filename, const size_t streamIndex, const std::string& profileName )
{
	InputStreamDesc streamDesc( streamIndex, filename, profileName );
	add( streamDesc );
}

void Transcoder::add( const std::string& filename, const size_t streamIndex, const Profile::ProfileDesc& profileDesc )
{
	_profile.update( profileDesc );
	
	InputStreamDesc streamDesc( streamIndex, filename, profileDesc );
	add( streamDesc );
}

void Transcoder::add( const InputStreamsDesc& streamDefs )
{
	for( size_t streamDest = 0; streamDest < streamDefs.size(); ++streamDest )
	{
		add( streamDefs.at( streamDest ) );
	}
	if( _inputStreams.size() != _streamTranscoders.size() )
		throw std::runtime_error( "_inputStreams and _streamTranscoders must have the same number of streams" );
}

void Transcoder::add( const InputStreamDesc& streamDefinition )
{
	const std::string filename( streamDefinition.filename );
	const size_t streamIndex = streamDefinition.streamId;
	const Profile::ProfileDesc profileDesc = streamDefinition.transcodeProfile;
	
	if( ! filename.length() )
	{
		try
		{
			// be sure the first inputStream is an AvInputStream created from an audio file
			dynamic_cast<AvInputStream*>( _inputStreams.at( 0 ) );
		}
		catch( std::exception& e)
		{
			throw std::runtime_error( "dummy stream can't be the first audio channel" );
		}

		_dummyInputStreams.push_back( new DummyInputStream() );
		
		_inputStreams.push_back( _dummyInputStreams.back() );
		
		_dummyInputStreams.back()->setAudioDesc( _inputStreams.at( 0 )->getAudioDesc() );
		_outputFile.addAudioStream( _inputStreams.back()->getAudioDesc() );
		
		StreamTranscoder* streamTranscoder = new StreamTranscoder( *_dummyInputStreams.back(), _outputFile, _streamTranscoders.size() );
		_streamTranscoders.push_back( streamTranscoder );

		return;
	}

	InputFile* referenceFile = NULL;

	for( std::vector< InputFile* >::iterator it = _inputFiles.begin(); it != _inputFiles.end(); ++it )
	{
		if( (*it)->getFilename() == filename )
		{
			referenceFile = (*it);
			break;
		}
	}

	if( ! referenceFile )
	{
		_inputFiles.push_back( new InputFile( filename ) );
		referenceFile = _inputFiles.back();
	}

	referenceFile->readStream( streamIndex );

	switch( referenceFile->getStreamType( streamIndex ) )
	{
		case AVMEDIA_TYPE_VIDEO:
		{
			StreamTranscoder* streamTranscoder = new StreamTranscoder( referenceFile->getStream( streamIndex ), _outputFile, _streamTranscoders.size() );
			streamTranscoder->init( profileDesc );
			_streamTranscoders.push_back( streamTranscoder );
			_inputStreams.push_back( & referenceFile->getStream( streamIndex ) );
			break;
		}
		case AVMEDIA_TYPE_AUDIO:
		{
			StreamTranscoder* streamTranscoder = new StreamTranscoder( referenceFile->getStream( streamIndex ), _outputFile, _streamTranscoders.size() );
			streamTranscoder->init( profileDesc );
			_streamTranscoders.push_back( streamTranscoder );
			_inputStreams.push_back( & referenceFile->getStream( streamIndex ) );
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
	return;
}


bool Transcoder::processFrame()
{
	for( size_t streamIndex = 0; streamIndex < _inputStreams.size(); ++streamIndex )
	{
		if( ( _streamTranscoders.size() > streamIndex ) &&
			! _streamTranscoders.at( streamIndex )->processFrame() )
		{
			//_inputStreams.erase( _inputStreams.begin() + streamIndex );
			_inputStreams.clear();
		}
	}

	if( _inputStreams.size() == 0 )
	{
		return false;
	}
	for( size_t i = 0; i < _streamTranscoders.size(); ++i )
	{
		_streamTranscoders.at( i )->processFrame();
	}
	return true;
}


void Transcoder::process( ProgressListener& progress )
{
	size_t frame = 0;

	std::vector< DataStream > dataStreams;

	dataStreams.reserve( _inputStreams.size() );

	for( size_t streamIndex = 0; streamIndex < _inputStreams.size(); ++streamIndex )
	{
		DataStream dataStream;
		dataStreams.push_back( dataStream );
	}

	_outputFile.beginWrap();


	while( 1 )
	{
		if( progress.progress( _inputStreams.at( 0 )->getPacketDuration() * ( frame + 1 ), _inputStreams.at( 0 )->getDuration() ) == eJobStatusCancel )
		{
			break;
		}

		if( ! processFrame() )
			break;

		++frame;
	}

	_outputFile.endWrap();
}

bool Transcoder::getStreamsNextPacket( std::vector< DataStream >& dataStreams )
{
	for( size_t i = 0; i < _inputStreams.size(); ++i )
	{
		bool ret = _inputStreams.at( i )->readNextPacket( dataStreams.at( i ) );
		if( ! ret || ( dataStreams.at( i ).getBuffer().size() == 0 ) )
		{
			return false;
		}
	}
	return true;
}

}
