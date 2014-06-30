#ifndef _AV_TRANSCODER_TRANSCODER_HPP_
#define _AV_TRANSCODER_TRANSCODER_HPP_

#include <AvTranscoder/File/InputFile.hpp>
#include <AvTranscoder/File/OutputFile.hpp>
#include <AvTranscoder/CodedStream/InputStream.hpp>
#include <AvTranscoder/EssenceStream/DummyAudio.hpp>
#include <AvTranscoder/EssenceStream/DummyVideo.hpp>
#include <AvTranscoder/ProgressListener.hpp>

#include <string>
#include <vector>

namespace avtranscoder
{

class StreamTranscoder;

class Transcoder
{

public:
	struct InputStreamDesc {
		size_t streamId;
		std::string filename;
		std::string transcodeProfile;

		InputStreamDesc( const size_t& sId, const std::string& filename, const std::string& profile )
			: streamId( sId )
			, filename( filename )
			, transcodeProfile( profile )
		{
		}
	};

	typedef std::vector< InputStreamDesc > InputStreamsDesc;

	Transcoder( OutputFile& outputFile );
	~Transcoder();

	void add( const std::string& filename, const size_t streamIndex, const std::string& profile );
	void add( const InputStreamsDesc& streamDefs );

	bool processFrame();

	void process( ProgressListener& progress );

private:
	// void addRewrapStream();
	// void addTranscodeStream();
	// void addDummyStream();

private:
	OutputFile&                      _outputFile;
	std::vector< InputFile* >        _inputFiles;

	std::vector< InputStream* >      _inputStreams;
	std::vector< StreamTranscoder* > _streamTranscoders;
	
	std::vector< DummyAudio* > _dummyAudio;
	std::vector< DummyVideo* > _dummyVideo;
};

}

#endif
