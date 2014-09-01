from nose.tools import *

from AvTranscoder import AvTranscoder as av


def testAddMetadataDate():
	"""
	Add metadata 'date' to the outputFile.
	"""
	outputFileName = "testAddMetadataDate.wav"

	ouputFile = av.OutputFile( outputFileName )
	transcoder = av.Transcoder( ouputFile )

	# rewrap a stream
	transcoder.add( "../data/audio/audio.wav", 0, "")

	# add one metadata
	ouputFile.addMetadata( "date", "metadata_to_check" )

	progress = av.ProgressListener()
	transcoder.process( progress )

	inputFile = av.InputFile( outputFileName )
	inputFile.analyse( progress, av.InputFile.eAnalyseLevelFast )
	properties = inputFile.getProperties()

	assert_in( ("date", "metadata_to_check"), properties.metadatas )

def testAddMetadataPlop():
	"""
	Can't add metadata 'plop' to the outputFile.
	"""
	outputFileName = "testAddMetadataPlop.wav"

	ouputFile = av.OutputFile( outputFileName )
	transcoder = av.Transcoder( ouputFile )

	# rewrap a stream
	transcoder.add( "../data/audio/audio.wav", 0, "")

	# add one metadata
	ouputFile.addMetadata( "plop", "metadata_to_check" )

	progress = av.ProgressListener()
	transcoder.process( progress )

	inputFile = av.InputFile( outputFileName )
	inputFile.analyse( progress, av.InputFile.eAnalyseLevelFast )
	properties = inputFile.getProperties()

	assert_not_in( ("plop", "metadata_to_check"), properties.metadatas )
