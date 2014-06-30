
#include "Transcoder.hpp"
#include "StreamTranscoder.hpp"

#include <AvTranscoder/CodedStream/AvInputStream.hpp>

namespace avtranscoder
{

Transcoder::Transcoder( OutputFile& outputFile )
	: _outputFile( outputFile )
{
	_outputFile.setup();
}

Transcoder::~Transcoder()
{
	for( std::vector< InputFile* >::iterator it = _inputFiles.begin(); it != _inputFiles.end(); ++it )
	{
		delete (*it);
	}
	for( std::vector< DummyAudio* >::iterator it = _dummyAudio.begin(); it != _dummyAudio.end(); ++it )
	{
		delete (*it);
	}
	for( std::vector< DummyVideo* >::iterator it = _dummyVideo.begin(); it != _dummyVideo.end(); ++it )
	{
		delete (*it);
	}
	for( std::vector< StreamTranscoder* >::iterator it = _streamTranscoders.begin(); it != _streamTranscoders.end(); ++it )
	{
		delete (*it);
	}
}

void Transcoder::add( const std::string& filename, const size_t streamIndex, const std::string& profile )
{
	// if( ! filename.length() )
	// {
	// 	addDummyStream( profile );
	// 	return;
	// }

	// if( ! profile.length() )
	// {
	// 	addRewrapStream( filename, streamIndex );
	// 	return;
	// }

	// addTranscodeStream( filename, streamIndex, profile );

	// InputFile* referenceFile = NULL;

	// for( std::vector< InputFile* >::iterator it = _inputFiles.begin(); it != _inputFiles.end(); ++it )
	// {
	// 	if( (*it)->getFilename() == filename )
	// 	{
	// 		referenceFile = (*it);
	// 		break;
	// 	}
	// }

	// if( ! referenceFile )
	// {
	// 	_inputFiles.push_back( new InputFile( filename ) );
	// 	referenceFile = _inputFiles.back();
	// }

	// referenceFile->readStream( streamIndex );

	// switch( referenceFile->getStreamType( streamIndex ) )
	// {
	// 	case AVMEDIA_TYPE_VIDEO:
	// 	{
	// 		StreamTranscoder* streamTranscoder = new StreamTranscoder( referenceFile->getStream( streamIndex ), _outputFile.getStream( _streamTranscoders.size() ) );
	// 		streamTranscoder->init( profile );
	// 		_streamTranscoders.push_back( streamTranscoder );
	// 		_inputStreams.push_back( & referenceFile->getStream( streamIndex ) );
	// 		break;
	// 	}
	// 	case AVMEDIA_TYPE_AUDIO:
	// 	{
	// 		StreamTranscoder* streamTranscoder = new StreamTranscoder( referenceFile->getStream( streamIndex ), _outputFile.getStream( _streamTranscoders.size() ) );
	// 		streamTranscoder->init( profile );
	// 		_streamTranscoders.push_back( streamTranscoder );
	// 		_inputStreams.push_back( & referenceFile->getStream( streamIndex ) );
	// 		break;
	// 	}
	// 	case AVMEDIA_TYPE_DATA:
	// 	case AVMEDIA_TYPE_SUBTITLE:
	// 	case AVMEDIA_TYPE_ATTACHMENT:
	// 	default:
	// 	{
	// 		return;
	// 	}
	// }
	return;
}

void Transcoder::add( const InputStreamsDesc& streamDefs )
{
	for( size_t streamDest = 0; streamDest < streamDefs.size(); ++streamDest )
	{
		add( streamDefs.at( streamDest ).filename,
		     streamDefs.at( streamDest ).streamId,
		     streamDefs.at( streamDest ).transcodeProfile );
	}
	if( _inputStreams.size() != _streamTranscoders.size() )
		throw std::runtime_error( "error during settings streams and transcoders" );
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


// void addRewrapStream()
// {

// }

// void addTranscodeStream()
// {

// }

// void addDummyStream( const Profile::ProfileDesc& profile )
// {
// 	try
// 	{
// 		// be sure the first inputStream is an AvInputStream created from an audio file
// 		dynamic_cast<AvInputStream*>( _inputStreams.at( 0 ) );
// 	}
// 	catch( std::exception& e)
// 	{
// 		throw std::runtime_error( "dummy stream can't be the first audio channel" );
// 	}

// 	_dummyAudio.push_back( new DummyAudio() );
	
// 	//_inputStreams.push_back( _dummyAudio.back() );
	
// 	// _dummyAudio.back()->setAudioDesc( _inputStreams.at( 0 )->getAudioDesc() );
// 	// _outputFile.addAudioStream( _inputStreams.back()->getAudioDesc() );
	
// 	StreamTranscoder* streamTranscoder = new StreamTranscoder( *_dummyAudio.back(), _outputFile.getStream( _streamTranscoders.size() ), profile );
// 	_streamTranscoders.push_back( streamTranscoder );
// }


}
