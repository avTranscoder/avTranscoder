
#include "Transcoder.hpp"

#include <AvTranscoder/CodedStream/AvInputStream.hpp>

namespace avtranscoder
{

Transcoder::Transcoder( OutputFile& outputFile )
	: _outputFile( outputFile )
	, _profile( true )
	, _verbose( false )
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

void Transcoder::add( const std::string& filename, const size_t streamIndex, const std::string& profileName )
{
	if( profileName.length() == 0 ) // no profile, only re-wrap stream
	{
		if( _verbose )
			std::cout << "add re-wrap stream" << std::endl;
		addRewrapStream( filename, streamIndex );
		return;
	}

	Profile::ProfileDesc& transcodeProfile = _profile.getProfile( profileName );
	add( filename, streamIndex, transcodeProfile );
}

void Transcoder::add( const std::string& filename, const size_t streamIndex, Profile::ProfileDesc& profileDesc )
{
	_profile.update( profileDesc );
	if( ! filename.length() )
	{
		if( _verbose )
			std::cout << "add encoding stream for dummy input" << std::endl;
		addDummyStream( profileDesc );
		return;
	}

	if( _verbose )
		std::cout << "add transcoding stream" << std::endl;
	addTranscodeStream( filename, streamIndex, profileDesc );
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
	// for( size_t i = 0; i < _streamTranscoders.size(); ++i )
	// {
	// 	_streamTranscoders.at( i )->processFrame();
	// }
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

	if( ! _inputStreams.size() )
	{
		throw std::runtime_error( "missing input streams in transcoder" );
	}
	
	if( _verbose )
		std::cout << "begin transcoding" << std::endl;
	
	_outputFile.beginWrap();

	double totalDuration = _inputStreams.at( 0 )->getDuration();

	while( 1 )
	{
		if( progress.progress( _inputStreams.at( 0 )->getPacketDuration() * ( frame + 1 ), totalDuration ) == eJobStatusCancel )
		{
			break;
		}

		if( ! processFrame() )
			break;

		++frame;
	}

	if( _verbose )
		std::cout << "end of transcoding" << std::endl;

	_outputFile.endWrap();
}

void Transcoder::addRewrapStream( const std::string& filename, const size_t streamIndex )
{
	InputFile* referenceFile = addInputFile( filename, streamIndex );

	StreamTranscoder* streamTranscoder = new StreamTranscoder( referenceFile->getStream( streamIndex ), _outputFile );
	_streamTranscoders.push_back( streamTranscoder );
	_inputStreams.push_back( &referenceFile->getStream( streamIndex ) );
}

void Transcoder::addTranscodeStream( const std::string& filename, const size_t streamIndex, Profile::ProfileDesc& profile )
{
	InputFile* referenceFile = addInputFile( filename, streamIndex );

	switch( referenceFile->getStreamType( streamIndex ) )
	{
		case AVMEDIA_TYPE_VIDEO:
		{
			StreamTranscoder* streamTranscoder = new StreamTranscoder( referenceFile->getStream( streamIndex ), _outputFile, profile );
			_streamTranscoders.push_back( streamTranscoder );
			_inputStreams.push_back( &referenceFile->getStream( streamIndex ) );
			break;
		}
		case AVMEDIA_TYPE_AUDIO:
		{
			StreamTranscoder* streamTranscoder = new StreamTranscoder( referenceFile->getStream( streamIndex ), _outputFile, profile );
			_streamTranscoders.push_back( streamTranscoder );
			_inputStreams.push_back( &referenceFile->getStream( streamIndex ) );
			break;
		}
		case AVMEDIA_TYPE_DATA:
		case AVMEDIA_TYPE_SUBTITLE:
		case AVMEDIA_TYPE_ATTACHMENT:
		default:
		{
			throw std::runtime_error( "unsupported media type in transcode setup" );
			return;
		}
	}
}

void Transcoder::addDummyStream( Profile::ProfileDesc& profile )
{
	if( profile.find( Profile::avProfileType )->second == Profile::avProfileTypeAudio )
	{
		_dummyAudio.push_back( new DummyAudio() );
		StreamTranscoder* streamTranscoder = new StreamTranscoder( *_dummyAudio.back(), _outputFile, profile );
		_streamTranscoders.push_back( streamTranscoder );
	}

	if( profile.find( Profile::avProfileType )->second == Profile::avProfileTypeVideo )
	{
		_dummyVideo.push_back( new DummyVideo() );
		StreamTranscoder* streamTranscoder = new StreamTranscoder( *_dummyVideo.back(), _outputFile, profile );
		_streamTranscoders.push_back( streamTranscoder );
	}
}

InputFile* Transcoder::addInputFile( const std::string& filename, const size_t streamIndex )
{
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
		if( _verbose )
			std::cout << "new InputFile for " << filename << std::endl;
		_inputFiles.push_back( new InputFile( filename ) );
		referenceFile = _inputFiles.back();
	}

	referenceFile->readStream( streamIndex );

	return referenceFile;
}

}
