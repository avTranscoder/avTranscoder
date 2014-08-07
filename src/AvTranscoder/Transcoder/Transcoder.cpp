#include "Transcoder.hpp"
#include <limits>

namespace avtranscoder
{

Transcoder::Transcoder( OutputFile& outputFile )
	: _outputFile( outputFile )
	, _inputFiles()
	, _inputStreams()
	, _streamTranscoders()
	, _dummyAudio()
	, _dummyVideo()
	, _profile( true )
	, _outputFps( 25 )
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

void Transcoder::add( const std::string& filename, const size_t streamIndex, const std::string& profileName, const size_t offset )
{
	if( profileName.length() == 0 ) // no profile, only re-wrap stream
	{
		if( _verbose )
			std::cout << "add re-wrap stream" << std::endl;
		addRewrapStream( filename, streamIndex );
		return;
	}

	Profile::ProfileDesc& transcodeProfile = _profile.getProfile( profileName );
	add( filename, streamIndex, transcodeProfile, offset );
}

void Transcoder::add( const std::string& filename, const size_t streamIndex, const std::string& profileName, CodedDesc& essenceDesc, const size_t offset )
{
	if( profileName.length() == 0 ) // no profile, only re-wrap stream
	{
		if( _verbose )
			std::cout << "add re-wrap stream" << std::endl;
		
		if( filename.length() == 0 )
		{
			std::cerr << "can't add a dummy stream with no profileName indicated" << std::endl;
			return;
		}
		
		addRewrapStream( filename, streamIndex );
		return;
	}
	
	Profile::ProfileDesc& transcodeProfile = _profile.getProfile( profileName );
	add( filename, streamIndex, transcodeProfile, essenceDesc, offset );
}

void Transcoder::add( const std::string& filename, const size_t streamIndex, Profile::ProfileDesc& profileDesc, const size_t offset )
{
	_profile.update( profileDesc );
	if( ! filename.length() )
	{
		std::cerr << "can't add a stream with no filename indicated" << std::endl;
		return;
	}

	if( _verbose )
		std::cout << "add transcoding stream" << std::endl;
	addTranscodeStream( filename, streamIndex, profileDesc, offset );
}

void Transcoder::add( const std::string& filename, const size_t streamIndex, Profile::ProfileDesc& profileDesc, CodedDesc& essenceDesc, const size_t offset )
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
	addTranscodeStream( filename, streamIndex, profileDesc, offset );
}

void Transcoder::add( const std::string& filename, const size_t streamIndex, const int subStreamIndex, const std::string& profileName, const size_t offset )
{
	if( subStreamIndex < 0 )
	{
		add( filename, streamIndex, profileName, offset );
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
	add( filename, streamIndex, subStreamIndex, transcodeProfile, offset );
}

void Transcoder::add( const std::string& filename, const size_t streamIndex, const int subStreamIndex, const std::string& profileName, CodedDesc& essenceDesc, const size_t offset )
{
	if( subStreamIndex < 0 )
	{
		add( filename, streamIndex, profileName, essenceDesc );
		return;
	}
	
	if( profileName.length() == 0 ) // no profile, only re-wrap stream
	{
		if( _verbose )
			std::cout << "add re-wrap stream for substream " << subStreamIndex << std::endl;

		if( filename.length() == 0 )
		{
			std::cerr << "can't add a dummy stream with no profileName indicated" << std::endl;
			return;
		}
		
		addRewrapStream( filename, streamIndex );
		return;
	}

	Profile::ProfileDesc& transcodeProfile = _profile.getProfile( profileName );
	add( filename, streamIndex, subStreamIndex, transcodeProfile, essenceDesc, offset );
}

void Transcoder::add( const std::string& filename, const size_t streamIndex, const int subStreamIndex, Profile::ProfileDesc& profileDesc, const size_t offset )
{
	_profile.update( profileDesc );
	
	if( subStreamIndex < 0 )
	{
		add( filename, streamIndex, profileDesc, offset );
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
	addTranscodeStream( filename, streamIndex, subStreamIndex, profileDesc, offset );
}

void Transcoder::add( const std::string& filename, const size_t streamIndex, const int subStreamIndex, Profile::ProfileDesc& profileDesc, CodedDesc& essenceDesc, const size_t offset )
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
	addTranscodeStream( filename, streamIndex, subStreamIndex, profileDesc, offset );
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

		_streamTranscoders.clear();
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

	if( ! _inputStreams.size() &&
		! _dummyVideo.size() && 
		! _dummyAudio.size() )
	{
		throw std::runtime_error( "missing input streams in transcoder" );
	}
	
	if( _verbose )
		std::cout << "begin transcoding" << std::endl;
	
	_outputFile.beginWrap();

	double totalDuration = std::numeric_limits<double>::max();
	switch( _eProcessMethod )
	{
		case eProcessMethodShortest :
			totalDuration = getMinTotalDuration();
			break;
		case eProcessMethodLongest :
			totalDuration = getMaxTotalDuration();
			break;
		case eProcessMethodInfinity :
			totalDuration = std::numeric_limits<double>::max();
			break;
	}

	if( _verbose )
		av_log_set_level( AV_LOG_DEBUG );

	while( 1 )
	{
		if( _verbose )
			std::cout << "process frame " << frame << std::endl;
		if( ! processFrame() )
			break;
		
		if( progress.progress( 1 / _outputFps * ( frame ), totalDuration ) == eJobStatusCancel )
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

	for( size_t i = 0; i < _streamTranscoders.size(); ++i )
	{
		switch( _eProcessMethod )
		{
			case eProcessMethodShortest :
				if( _streamTranscoders.at( i )->getDuration() == getMinTotalDuration() )
					_streamTranscoders.at( i )->setInfinityStream( false );
				else
					_streamTranscoders.at( i )->setInfinityStream( true );
				break;
			case eProcessMethodLongest :
				if( _streamTranscoders.at( i )->getDuration() == getMaxTotalDuration() )
					_streamTranscoders.at( i )->setInfinityStream( false );
				else
					_streamTranscoders.at( i )->setInfinityStream( true );
				break;
			case eProcessMethodInfinity :
				_streamTranscoders.at( i )->setInfinityStream( true );
				break;
		}
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

void Transcoder::addTranscodeStream( const std::string& filename, const size_t streamIndex, Profile::ProfileDesc& profile, const size_t offset )
{
	InputFile* referenceFile = addInputFile( filename, streamIndex );

	switch( referenceFile->getStreamType( streamIndex ) )
	{
		case AVMEDIA_TYPE_VIDEO:
		case AVMEDIA_TYPE_AUDIO:
		{
			_streamTranscoders.push_back( new StreamTranscoder( referenceFile->getStream( streamIndex ), _outputFile, profile, -1 , offset ) );
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

void Transcoder::addTranscodeStream( const std::string& filename, const size_t streamIndex, const size_t subStreamIndex, Profile::ProfileDesc& profile, const size_t offset )
{
	InputFile* referenceFile = addInputFile( filename, streamIndex );

	switch( referenceFile->getStreamType( streamIndex ) )
	{
		case AVMEDIA_TYPE_VIDEO:
		case AVMEDIA_TYPE_AUDIO:
		{
			_streamTranscoders.push_back( new StreamTranscoder( referenceFile->getStream( streamIndex ), _outputFile, profile, subStreamIndex, offset ) );
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

double Transcoder::getMinTotalDuration() const
{
	double minTotalDuration = std::numeric_limits<double>::max();
	
	for( size_t i = 0; i < _streamTranscoders.size(); ++i )
	{
		minTotalDuration = std::min( _streamTranscoders.at( i )->getDuration(), minTotalDuration );
	}
	return minTotalDuration;
}

double Transcoder::getMaxTotalDuration() const
{
	double maxTotalDuration = 0;
	
	for( size_t i = 0; i < _streamTranscoders.size(); ++i )
	{
		maxTotalDuration = std::max( _streamTranscoders.at( i )->getDuration(), maxTotalDuration );
	}
	return maxTotalDuration;
}

}
