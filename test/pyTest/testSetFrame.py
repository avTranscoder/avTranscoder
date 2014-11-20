import os

from nose.tools import *

from pyAvTranscoder import avtranscoder as av


def testSetVideoFrame():
	"""
	Generate a video stream, and set its frame during process.
	"""

	# create output
	outputFileName = "testSetVideoFrame.mov"
	ouputFile = av.OutputFile( outputFileName )

	# create video frame and codec
	imageDesc = av.VideoFrameDesc()
	imageDesc.setWidth( 1920 )
	imageDesc.setHeight( 1080 )
	imageDesc.setDar( 1920, 1080 )

	inputPixel = av.Pixel()
	inputPixel.setColorComponents( av.eComponentRgb );
	inputPixel.setPlanar( False );

	imageDesc.setPixel( inputPixel );

	inputVideoCodec = av.VideoCodec( av.eCodecTypeEncoder, "mpeg2video" );
	inputVideoCodec.setImageParameters( imageDesc );

	# create transcoder and add a video stream
	transcoder = av.Transcoder( ouputFile )
	transcoder.add(  "", 0, "xdcamhd422", inputVideoCodec )
	videoEssence = transcoder.getStreamTranscoder( 0 ).getCurrentEssence()

	# start process
	transcoder.init()
	ouputFile.beginWrap()

	# process 255 frames
	for i in range(0,255):
		transcoder.processFrame()
		# set video frame
		frame = av.VideoFrame( imageDesc )
		frame.getBuffer().assign(frame.getBuffer().size(), i)
		videoEssence.setFrame( frame )

	# end process
	ouputFile.endWrap()

	# get dst file of transcode
	dst_inputFile = av.InputFile( outputFileName )
	progress = av.NoDisplayProgress()
	dst_inputFile.analyse( progress, av.InputFile.eAnalyseLevelFast )
	dst_properties = dst_inputFile.getProperties()
	dst_videoStream = dst_properties.videoStreams[0]

	assert_equals( "mpeg2video", dst_videoStream.codecName )
	assert_equals( "MPEG-2 video", dst_videoStream.codecLongName )
	assert_equals( 1920, dst_videoStream.width )
	assert_equals( 1080, dst_videoStream.height )
	assert_equals( 16, dst_videoStream.dar.num )
	assert_equals( 9, dst_videoStream.dar.den )


def testSetAudioFrame():
	"""
	Generate a audio stream, and set its frame during process.
	"""

	# create output
	outputFileName = "testSetAudioFrame.wav"
	ouputFile = av.OutputFile( outputFileName )

	# create video frame and codec
	audioDesc = av.AudioFrameDesc()
	audioDesc.setSampleRate( 48000 )
	audioDesc.setChannels( 1 )
	audioDesc.setSampleFormat( "s32" )

	inputAudioCodec = av.AudioCodec( av.eCodecTypeEncoder, "pcm_s24le" );
	inputAudioCodec.setAudioParameters( audioDesc );

	# create transcoder and add a video stream
	transcoder = av.Transcoder( ouputFile )
	transcoder.add(  "", 0, "wave24b48kmono", inputAudioCodec )
	audioEssence = transcoder.getStreamTranscoder( 0 ).getCurrentEssence()

	# start process
	transcoder.init()
	ouputFile.beginWrap()

	# process 255 frames
	for i in range(0,255):
		transcoder.processFrame()
		# set video frame
		frame = av.AudioFrame( audioDesc )
		frame.getBuffer().assign(frame.getBuffer().size(), i)
		audioEssence.setFrame( frame )

	# end process
	ouputFile.endWrap()

	# get dst file of transcode
	dst_inputFile = av.InputFile( outputFileName )
	progress = av.NoDisplayProgress()
	dst_inputFile.analyse( progress, av.InputFile.eAnalyseLevelFast )
	dst_properties = dst_inputFile.getProperties()
	dst_audioStream = dst_properties.audioStreams[0]

	assert_equals( "pcm_s24le", dst_audioStream.codecName )
	assert_equals( "PCM signed 24-bit little-endian", dst_audioStream.codecLongName )
	assert_equals( "s32", dst_audioStream.sampleFormatName )
	assert_equals( "signed 32 bits", dst_audioStream.sampleFormatLongName )
	assert_equals( 48000, dst_audioStream.sampleRate )
	assert_equals( 1, dst_audioStream.channels )

