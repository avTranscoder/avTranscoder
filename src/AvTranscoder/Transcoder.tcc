#include <AvTranscoder/InputFile.hpp>

#include <AvTranscoder/AvInputStream.hpp>

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

	for( std::vector< DummyInputStream* >::iterator it = _dummyInputStreams.begin(); it != _dummyInputStreams.end(); ++it )
	{
		delete (*it);
	}
}

void Transcoder::add( const std::string& filename, const size_t streamIndex )
{
	if( ! filename.length() )
	{
		_dummyInputStreams.push_back( new DummyInputStream() );
		
		_inputStreams.push_back( _dummyInputStreams.back() );
		
		if( _inputStreams.at( 1 ) )
		{
			_dummyInputStreams.back()->setAudioDesc( _inputStreams.at( 1 )->getAudioDesc() );
			_outputFile.addAudioStream( _inputStreams.back()->getAudioDesc() );
		}
		else
			std::cout << "dummy can't be the first audio channel" << std::endl;

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
			_inputStreams.push_back( &referenceFile->getStream( streamIndex ) );
			_outputFile.addVideoStream( _inputStreams.back()->getVideoDesc() );
			break;
		}
		case AVMEDIA_TYPE_AUDIO:
		{
			_inputStreams.push_back( &referenceFile->getStream( streamIndex ) );
			_outputFile.addAudioStream( _inputStreams.back()->getAudioDesc() );
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

void Transcoder::add( const StreamsDefinition& streams )
{
	for( size_t streamIndex = 0; streamIndex < streams.size(); ++streamIndex )
	{
		add( streams.at( streamIndex ).first, streams.at( streamIndex ).second );
	}
	return;
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

	bool continueProcess( true );

	while( 1 )
	{
		// read one frame for each streamIndex
		for( size_t streamIndex = 0; streamIndex < _inputStreams.size(); ++streamIndex )
		{
			bool ret = _inputStreams.at( streamIndex )->readNextPacket( dataStreams.at( streamIndex ) );

			if( ! ret || ( dataStreams.at( streamIndex ).getBuffer().size() == 0 ) )
			{
				continueProcess = false;
			}
		}

		if( ! continueProcess )
			break;

		switch( progress.progress( _inputStreams.at( 0 )->getPacketDuration() * ( frame + 1 ), _inputStreams.at( 0 )->getDuration() ) )
		{
			case eJobStatusContinue:
			{
				break;
			}
			case eJobStatusCancel:
			{
				continueProcess = false;
				break;
			}
		}

		if( ! continueProcess )
			break;

		for( size_t streamIndex = 0; streamIndex < _inputStreams.size(); ++streamIndex )
		{
			_outputFile.wrap( dataStreams.at( streamIndex ), streamIndex );
		}

		++frame;
	}

	_outputFile.endWrap();

}

}
