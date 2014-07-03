
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
	if( _verbose )
		std::cout << "process frame" << std::endl;
	for( size_t streamIndex = 0; streamIndex < _streamTranscoders.size(); ++streamIndex )
	{
		if( ! _streamTranscoders.at( streamIndex ).processFrame() )
		{
			_streamTranscoders.clear();
		}
	}

	if( _streamTranscoders.size() == 0 )
	{
		return false;
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

	_streamTranscoders.push_back( StreamTranscoder( referenceFile->getStream( streamIndex ), _outputFile ) );
	_inputStreams.push_back( &referenceFile->getStream( streamIndex ) );
}

void Transcoder::addTranscodeStream( const std::string& filename, const size_t streamIndex, Profile::ProfileDesc& profile )
{
	InputFile* referenceFile = addInputFile( filename, streamIndex );

	switch( referenceFile->getStreamType( streamIndex ) )
	{
		case AVMEDIA_TYPE_VIDEO:
		{
			_streamTranscoders.push_back( StreamTranscoder( referenceFile->getStream( streamIndex ), _outputFile, profile ) );
			_inputStreams.push_back( &referenceFile->getStream( streamIndex ) );
			break;
		}
		case AVMEDIA_TYPE_AUDIO:
		{
			_streamTranscoders.push_back( StreamTranscoder( referenceFile->getStream( streamIndex ), _outputFile, profile ) );
			_inputStreams.push_back( &referenceFile->getStream( streamIndex ) );
			break;
		}
		case AVMEDIA_TYPE_DATA:
		case AVMEDIA_TYPE_SUBTITLE:
		case AVMEDIA_TYPE_ATTACHMENT:
		default:
		{
			throw std::runtime_error( "unsupported media type in transcode setup" );
		}
	}
}

void Transcoder::addDummyStream( Profile::ProfileDesc& profile )
{
	if( ! profile.count( Profile::avProfileType ) )
		throw std::runtime_error( "unable to found stream type (audio, video, etc.)" );

	if( profile.find( Profile::avProfileType )->second == Profile::avProfileTypeAudio )
	{
		_dummyAudio.push_back( DummyAudio() );
		_streamTranscoders.push_back( StreamTranscoder( _dummyAudio.back(), _outputFile, profile ) );
	}

	if( profile.find( Profile::avProfileType )->second == Profile::avProfileTypeVideo )
	{
		_dummyVideo.push_back( DummyVideo() );
		_streamTranscoders.push_back( StreamTranscoder( _dummyVideo.back(), _outputFile, profile ) );
	}
}

InputFile* Transcoder::addInputFile( const std::string& filename, const size_t streamIndex )
{
	InputFile* referenceFile = NULL;

	for( std::vector< InputFile >::iterator it = _inputFiles.begin(); it != _inputFiles.end(); ++it )
	{
		if( (*it).getFilename() == filename )
		{
			referenceFile = &(*it);
			break;
		}
	}

	if( ! referenceFile )
	{
		if( _verbose )
			std::cout << "new InputFile for " << filename << std::endl;
		_inputFiles.push_back( InputFile( filename ) );
		referenceFile = &_inputFiles.back();
	}

	referenceFile->readStream( streamIndex );

	return referenceFile;
}

}
