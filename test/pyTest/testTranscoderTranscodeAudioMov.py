import os

from nose.tools import *

from pyAvTranscoder import avtranscoder as av

av.preloadCodecsAndFormats()
av.Logger.setLogLevel(av.AV_LOG_QUIET)


def testTranscodeMovVariableNbSamplesPerFrame():
	"""
	Transcode the audio stream of a MOV file which contains a video stream.
	The number of samples per frame can vary to fit the gop size.
	AudioTransform must manage these cases.
	"""
	inputFileName = os.environ['AVTRANSCODER_TEST_AUDIO_MOV_FILE']
	outputFileName = "testTranscodeMovVariableNbSamplesPerFrame.wav"

	ouputFile = av.OutputFile( outputFileName )
	transcoder = av.Transcoder( ouputFile )

        # create custom profile
	customProfile = av.ProfileMap()
	customProfile[av.avProfileIdentificator] = "customProfile"
	customProfile[av.avProfileIdentificatorHuman] = "custom profile"
	customProfile[av.avProfileType] = av.avProfileTypeAudio
	customProfile[av.avProfileCodec] = "pcm_s24le"

	inputFile = av.InputFile( inputFileName )
        audioStreamIndex = inputFile.getProperties().getAudioProperties()[0].getStreamIndex()
	transcoder.add( inputFileName, audioStreamIndex, customProfile )

	progress = av.ConsoleProgress()
	transcoder.process( progress )

	# get dst file of transcode
	dst_inputFile = av.InputFile( outputFileName )
	dst_inputFile.analyse( progress, av.eAnalyseLevelHeader )
	dst_properties = dst_inputFile.getProperties()
	dst_audioStream = dst_properties.getAudioProperties()[0]

	assert_equals( "pcm_s24le", dst_audioStream.getCodecName() )
	assert_equals( "PCM signed 24-bit little-endian", dst_audioStream.getCodecLongName() )

