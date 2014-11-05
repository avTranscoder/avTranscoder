import os

from nose.tools import *

from pyAvTranscoder import avtranscoder as av


def testSetVideoFrame():
	"""
	Generate a video stream, and set its frame during process.
	"""
	profile = av.Profile(True)

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

	video = av.GeneratorVideo()
	video.setVideoFrameDesc( imageDesc )

	# create transcoder and add a video stream
	transcoder = av.Transcoder( ouputFile )
	streamTranscoder = av.StreamTranscoder( video, ouputFile, profile.getProfile( "xdcamhd422" ) )
	transcoder.add( streamTranscoder )

	# start process
	transcoder.init()
	ouputFile.beginWrap()

	# process 255 frames
	for i in range(0,255):
		transcoder.processFrame()
		# set video frame
		frame = av.VideoFrame( imageDesc )
		frame.getBuffer().assign(frame.getBuffer().size(), i)
		video.setFrame( frame )

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
	profile = av.Profile(True)

	# create output
	outputFileName = "testSetAudioFrame.wav"
	ouputFile = av.OutputFile( outputFileName )

	# create video frame and codec
	audioDesc = av.AudioFrameDesc()
	audioDesc.setSampleRate( 48000 )
	audioDesc.setChannels( 1 )
	audioDesc.setSampleFormat( "s32" )

	audio = av.GeneratorAudio()
	audio.setAudioFrameDesc( audioDesc )

	# create transcoder and add a video stream
	transcoder = av.Transcoder( ouputFile )
	streamTranscoder = av.StreamTranscoder( audio, ouputFile, profile.getProfile( "wave24b48kmono" ) )
	transcoder.add(  streamTranscoder )

	# start process
	transcoder.init()
	ouputFile.beginWrap()

	# process 255 frames
	for i in range(0,255):
		transcoder.processFrame()
		# set video frame
		frame = av.AudioFrame( audioDesc )
		frame.getBuffer().assign(frame.getBuffer().size(), i)
		audio.setFrame( frame )

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
	assert_equals( "signed 32 bits", dst_audioStream.sampleFormat )
	assert_equals( 48000, dst_audioStream.sampleRate )
	assert_equals( 1, dst_audioStream.channels )

