from nose.tools import *

from pyAvTranscoder import avtranscoder as av


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
	metadata_to_check = ("date", "value")
	ouputFile.addMetadata( metadata_to_check[0], metadata_to_check[1] )

	progress = av.NoDisplayProgress()
	transcoder.process( progress )

	inputFile = av.InputFile( outputFileName )
	inputFile.analyse( progress, av.InputFile.eAnalyseLevelFast )
	properties = inputFile.getProperties()

	assert_in( metadata_to_check, properties.metadatas )

def testAddImpossibleMetadata():
	"""
	Can't add an impossible metadata to the outputFile.
	"""
	outputFileName = "testAddMetadataPlop.wav"

	ouputFile = av.OutputFile( outputFileName )
	transcoder = av.Transcoder( ouputFile )

	# rewrap a stream
	transcoder.add( "../data/audio/audio.wav", 0, "")

	# add one metadata
	metadata_to_check = ("undefinedMetadataKey", "undefinedMetadataValue")
	ouputFile.addMetadata( metadata_to_check[0], metadata_to_check[1] )

	progress = av.NoDisplayProgress()
	transcoder.process( progress )

	inputFile = av.InputFile( outputFileName )
	inputFile.analyse( progress, av.InputFile.eAnalyseLevelFast )
	properties = inputFile.getProperties()

	assert_not_in( metadata_to_check, properties.metadatas )
