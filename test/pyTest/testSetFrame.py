import os

from nose.tools import *

from pyAvTranscoder import avtranscoder as av

av.preloadCodecsAndFormats()
av.Logger.setLogLevel(av.AV_LOG_QUIET)


def testSetVideoFrame():
	"""
	Generate a video stream, and set its frame during process.
	"""

	# create output
	outputFileName = "testSetVideoFrame.mov"
	ouputFile = av.OutputFile( outputFileName )

	# create video frame and codec
	inputVideoCodec = av.VideoCodec( av.eCodecTypeEncoder, "mpeg2video" );
	imageDesc = av.VideoFrameDesc( 1920, 1080, "rgb24" )
	inputVideoCodec.setImageParameters( imageDesc )

	# create transcoder and add a video stream
	transcoder = av.Transcoder( ouputFile )
	transcoder.add(  "", 0, "mpeg2", inputVideoCodec )
	videoDecoder = transcoder.getStreamTranscoder( 0 ).getCurrentDecoder()

	# start process
	ouputFile.beginWrap()
	transcoder.preProcessCodecLatency()
	p = av.ConsoleProgress()

	# process 10 frames
        nbFrames = 10
	for i in range(0, nbFrames):
		transcoder.processFrame()
                p.progress( i, nbFrames )

		# set video frame
		frame = av.VideoFrame( imageDesc )
		frame.assign(frame.getSize(), i)
		videoDecoder.setNextFrame( frame )

	# end process
	ouputFile.endWrap()

	# get dst file of transcode
	dst_inputFile = av.InputFile( outputFileName )
	progress = av.NoDisplayProgress()
	dst_inputFile.analyse( progress, av.eAnalyseLevelHeader )
	dst_properties = dst_inputFile.getProperties()
	dst_videoStream = dst_properties.getVideoProperties()[0]

	assert_equals( "mpeg2video", dst_videoStream.getCodecName() )
	assert_equals( "MPEG-2 video", dst_videoStream.getCodecLongName() )
	assert_equals( 1920, dst_videoStream.getWidth() )
	assert_equals( 1080, dst_videoStream.getHeight() )
	assert_equals( 16, dst_videoStream.getDar().num )
	assert_equals( 9, dst_videoStream.getDar().den )


def testSetAudioFrame():
	"""
	Generate a audio stream, and set its frame during process.
	"""

	# create output
	outputFileName = "testSetAudioFrame.wav"
	ouputFile = av.OutputFile( outputFileName )

	# create video frame and codec
	inputAudioCodec = av.AudioCodec( av.eCodecTypeEncoder, "pcm_s24le" );
	audioDesc = av.AudioFrameDesc( 48000, 1, "s32" )
	inputAudioCodec.setAudioParameters( audioDesc );

	# create transcoder and add a video stream
	transcoder = av.Transcoder( ouputFile )
	transcoder.add(  "", 0, "wave24b48kmono", inputAudioCodec )
	audioDecoder = transcoder.getStreamTranscoder( 0 ).getCurrentDecoder()

	# start process
	ouputFile.beginWrap()
	transcoder.preProcessCodecLatency()
	p = av.ConsoleProgress()

	# process 10 frames
        nbFrames = 10
	for i in range(0, nbFrames):
		transcoder.processFrame()
                p.progress( i, nbFrames )

		# set video frame
		frame = av.AudioFrame( audioDesc )
		frame.assign(frame.getSize(), i)
		audioDecoder.setNextFrame( frame )

	# end process
	ouputFile.endWrap()

	# get dst file of transcode
	dst_inputFile = av.InputFile( outputFileName )
	progress = av.NoDisplayProgress()
	dst_inputFile.analyse( progress, av.eAnalyseLevelHeader )
	dst_properties = dst_inputFile.getProperties()
	dst_audioStream = dst_properties.getAudioProperties()[0]

	assert_equals( "pcm_s24le", dst_audioStream.getCodecName() )
	assert_equals( "PCM signed 24-bit little-endian", dst_audioStream.getCodecLongName() )
	assert_equals( "s32", dst_audioStream.getSampleFormatName() )
	assert_equals( "signed 32 bits", dst_audioStream.getSampleFormatLongName() )
	assert_equals( 48000, dst_audioStream.getSampleRate() )
	assert_equals( 1, dst_audioStream.getChannels() )

