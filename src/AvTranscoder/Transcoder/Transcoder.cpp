#include "Transcoder.hpp"

namespace avtranscoder
{

Transcoder::Transcoder( OutputFile& outputFile )
	: _outputFile( outputFile )
	, _profile( true )
	, _finalisedStreams( 0 )
	, _eProcessMethod ( eProcessMethodLongest )
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
	for( std::vector< StreamTranscoder* >::iterator it = _streamTranscoders.begin(); it != _streamTranscoders.end(); ++it )
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
		// if( _verbose )
		std::cerr << "can't add a stream with no filename indicated" << std::endl;
		return;
	}

	if( _verbose )
		std::cout << "add transcoding stream" << std::endl;
	addTranscodeStream( filename, streamIndex, profileDesc );
}

void Transcoder::add( const std::string& filename, const size_t streamIndex, Profile::ProfileDesc& profileDesc, CodedDesc& essenceDesc )
{
	_profile.update( profileDesc );
	if( ! filename.length() )
	{
		if( _verbose )
			std::cout << "add dummy stream" << std::endl;
		addDummyStream( profileDesc, essenceDesc );
		return;
	}
	
	if( _verbose )
		std::cout << "add transcoding stream" << std::endl;
	addTranscodeStream( filename, streamIndex, profileDesc );
}

void Transcoder::add( const std::string& filename, const size_t streamIndex, const int subStreamIndex, const std::string& profileName )
{
	if( subStreamIndex < 0 )
	{
		add( filename, streamIndex, profileName );
		return;
	}

	if( profileName.length() == 0 ) // no profile, only re-wrap stream
	{
		if( _verbose )
			std::cout << "add re-wrap stream for substream " << subStreamIndex << std::endl;

		addRewrapStream( filename, streamIndex );
		return;
	}

	Profile::ProfileDesc& transcodeProfile = _profile.getProfile( profileName );
	add( filename, streamIndex, subStreamIndex, transcodeProfile );
}

void Transcoder::add( const std::string& filename, const size_t streamIndex, const int subStreamIndex, Profile::ProfileDesc& profileDesc )
{
	_profile.update( profileDesc );
	
	if( subStreamIndex < 0 )
	{
		add( filename, streamIndex, profileDesc );
		return;
	}
	
	if( ! filename.length() )
	{
		if( _verbose )
			std::cerr << "can't add a stream with no filename indicated" << std::endl;
		return;
	}

	if( _verbose )
		std::cout << "add transcoding stream for substream " << subStreamIndex << std::endl;
	addTranscodeStream( filename, streamIndex, subStreamIndex, profileDesc );
}

void Transcoder::add( const std::string& filename, const size_t streamIndex, const int subStreamIndex, Profile::ProfileDesc& profileDesc, CodedDesc& essenceDesc )
{
	_profile.update( profileDesc );
	
	if( subStreamIndex < 0 )
	{
		add( filename, streamIndex, profileDesc );
		return;
	}
	
	if( ! filename.length() )
	{
		if( _verbose )
			std::cout << "add dummy stream" << std::endl;
		addDummyStream( profileDesc, essenceDesc );
		return;
	}

	if( _verbose )
		std::cout << "add transcoding stream for substream " << subStreamIndex << std::endl;
	addTranscodeStream( filename, streamIndex, subStreamIndex, profileDesc );
}

void Transcoder::add( StreamTranscoder& stream )
{
	_streamTranscoders.push_back( &stream );
}

bool Transcoder::processFrame()
{
	if( _streamTranscoders.size() == 0 )
	{
		return false;
	}

	if( _verbose )
		std::cout << "process frame" << std::endl;
	for( size_t streamIndex = 0; streamIndex < _streamTranscoders.size(); ++streamIndex )
	{
		if( _verbose )
			std::cout << "process stream " << streamIndex << "/" << _streamTranscoders.size() - 1 << std::endl;

		bool streamProcessStatus = _streamTranscoders.at( streamIndex )->processFrame();

		if( streamProcessStatus )
			continue;

		switch( _eProcessMethod )
		{
			case eProcessMethodShortest :
				_streamTranscoders.clear();
				break;
			case eProcessMethodLongest :
				++_finalisedStreams;
				_streamTranscoders.at( streamIndex )->switchToDummyEssence();
				if( _verbose )
					std::cout << "-> switch to dummy for stream " << streamIndex << std::endl;
				if( _finalisedStreams == _streamTranscoders.size() )
					_streamTranscoders.clear();
				break;
			case eProcessMethodInfinity :
				if( _verbose )
					std::cout << "-> infinity processing: switch to dummy for stream " << streamIndex << std::endl;
				_streamTranscoders.at( streamIndex )->switchToDummyEssence();
				break;
		}
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

	if( _verbose )
		av_log_set_level( AV_LOG_DEBUG );

	while( 1 )
	{
		if( _verbose )
			std::cout << "process frame " << frame << std::endl;
		if( ! processFrame() )
			break;
		
		if( progress.progress( _inputStreams.at( 0 )->getPacketDuration() * ( frame ), totalDuration ) == eJobStatusCancel )
		{
			break;
		}

		++frame;
	}

	if( _verbose )
		std::cout << "end of transcoding" << std::endl;

	_outputFile.endWrap();
}

void Transcoder::setProcessMethod( const EProcessMethod eProcessMethod )
{
	_eProcessMethod	= eProcessMethod;
	for( std::vector< StreamTranscoder* >::iterator it = _streamTranscoders.begin(); it != _streamTranscoders.end(); ++it )
	{
		(*it)->setInfinityProcess( eProcessMethod == eProcessMethodInfinity );
	}
}

void Transcoder::setVerbose( bool verbose )
{
	_verbose = verbose;
	for( std::vector< StreamTranscoder* >::iterator it = _streamTranscoders.begin(); it != _streamTranscoders.end(); ++it )
	{
		(*it)->setVerbose( _verbose );
	}
	_outputFile.setVerbose( _verbose );
}

void Transcoder::addRewrapStream( const std::string& filename, const size_t streamIndex )
{
	InputFile* referenceFile = addInputFile( filename, streamIndex );
	_streamTranscoders.push_back( new StreamTranscoder( referenceFile->getStream( streamIndex ), _outputFile ) );
	_inputStreams.push_back( &referenceFile->getStream( streamIndex ) );
}

void Transcoder::addTranscodeStream( const std::string& filename, const size_t streamIndex, Profile::ProfileDesc& profile )
{
	InputFile* referenceFile = addInputFile( filename, streamIndex );

	switch( referenceFile->getStreamType( streamIndex ) )
	{
		case AVMEDIA_TYPE_VIDEO:
		case AVMEDIA_TYPE_AUDIO:
		{
			_streamTranscoders.push_back( new StreamTranscoder( referenceFile->getStream( streamIndex ), _outputFile, profile ) );
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

void Transcoder::addTranscodeStream( const std::string& filename, const size_t streamIndex, const size_t subStreamIndex, Profile::ProfileDesc& profile )
{
	InputFile* referenceFile = addInputFile( filename, streamIndex );

	switch( referenceFile->getStreamType( streamIndex ) )
	{
		case AVMEDIA_TYPE_VIDEO:
		case AVMEDIA_TYPE_AUDIO:
		{
			_streamTranscoders.push_back( new StreamTranscoder( referenceFile->getStream( streamIndex ), _outputFile, profile, subStreamIndex ) );
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

void Transcoder::addDummyStream( const Profile::ProfileDesc& profile, const CodedDesc& essenceDesc )
{
	if( ! profile.count( Profile::avProfileType ) )
		throw std::runtime_error( "unable to found stream type (audio, video, etc.)" );

	if( profile.find( Profile::avProfileType )->second == Profile::avProfileTypeAudio )
	{
		if( _verbose )
			std::cout << "add dummy audio" << std::endl;
		_dummyAudio.push_back( new DummyAudio() );
		_dummyAudio.back()->setAudioDesc( static_cast<AudioDesc>( essenceDesc ) );
		
		_streamTranscoders.push_back( new StreamTranscoder( *_dummyAudio.back(), _outputFile, profile ) );
	}

	if( profile.find( Profile::avProfileType )->second == Profile::avProfileTypeVideo )
	{
		if( _verbose )
			std::cout << "add dummy video" << std::endl;
		_dummyVideo.push_back( new DummyVideo() );
		_dummyVideo.back()->setVideoDesc( static_cast<VideoDesc>( essenceDesc ) );
		
		_streamTranscoders.push_back( new StreamTranscoder( *_dummyVideo.back(), _outputFile, profile ) );
	}
}

InputFile* Transcoder::addInputFile( const std::string& filename, const size_t streamIndex )
{
	InputFile* referenceFile = NULL;

	for( std::vector< InputFile* >::iterator it = _inputFiles.begin(); it != _inputFiles.end(); ++it )
	{
		if( ( (*it)->getFilename() == filename ) &&
			( ! (*it)->getReadStream( streamIndex ) ) )
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
