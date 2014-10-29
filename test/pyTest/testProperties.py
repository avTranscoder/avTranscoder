import os

from nose.tools import *

from mediaEngine import mediaCore
from mediaEngine import mediaIO

def testAddMetadataDate():
	"""
	Add metadata 'date' to the outputFile.
	"""
	outputFileName = "testAddMetadataDate.wav"

	ouputFile = mediaIO.OutputFile( outputFileName )
	transcoder = mediaIO.Transcoder( ouputFile )

	# rewrap a stream
	transcoder.add( os.environ['MEDIA_ENGINE_TEST_AUDIO_FILE'], 0, "")

	# add one metadata
	metadata_to_check = ("date", "value")
	ouputFile.addMetadata( metadata_to_check[0], metadata_to_check[1] )

	progress = mediaCore.NoDisplayProgress()
	transcoder.process( progress )

	inputFile = mediaIO.InputFile( outputFileName )
	inputFile.analyse( progress, mediaIO.InputFile.eAnalyseLevelFast )
	properties = inputFile.getProperties()

	assert_in( metadata_to_check, properties.metadatas )

def testAddImpossibleMetadata():
	"""
	Can't add an impossible metadata to the outputFile.
	"""
	outputFileName = "testAddMetadataPlop.wav"

	ouputFile = mediaIO.OutputFile( outputFileName )
	transcoder = mediaIO.Transcoder( ouputFile )

	# rewrap a stream
	transcoder.add( os.environ['MEDIA_ENGINE_TEST_AUDIO_FILE'], 0, "")

	# add one metadata
	metadata_to_check = ("undefinedMetadataKey", "undefinedMetadataValue")
	ouputFile.addMetadata( metadata_to_check[0], metadata_to_check[1] )

	progress = mediaCore.NoDisplayProgress()
	transcoder.process( progress )

	inputFile = mediaIO.InputFile( outputFileName )
	inputFile.analyse( progress, mediaIO.InputFile.eAnalyseLevelFast )
	properties = inputFile.getProperties()

	assert_not_in( metadata_to_check, properties.metadatas )
