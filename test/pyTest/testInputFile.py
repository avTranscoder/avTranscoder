import os

# Check if environment is setup to run the tests
if os.environ.get('AVTRANSCODER_TEST_VIDEO_MOV_FILE') is None:
    from nose.plugins.skip import SkipTest
    raise SkipTest("Need to define environment variables "
        "AVTRANSCODER_TEST_VIDEO_MOV_FILE")

from nose.tools import *

from pyAvTranscoder import avtranscoder as av


@raises(IOError)
def testCreateInputFileFromUnexistingFilename():
    """
    Create an InputFile from an unexisting filename.
    """
    inputFileName = "testCreateInputFileFromUnexistingFilename"
    av.InputFile( inputFileName )


def testInputFileAnalyseFirstGop():
    """
    Analyse the first gop of an InputFile, and check if the correct attributes are filled.
    """
    inputFileName = os.environ['AVTRANSCODER_TEST_VIDEO_MOV_FILE']
    inputFile = av.InputFile( inputFileName )

    # The analyse of the first GOP is not done yet
    videoProperties = inputFile.getProperties().getVideoProperties()[0]
    assert_equals(videoProperties.isInterlaced(), False)
    assert_equals(videoProperties.isTopFieldFirst(), False)
    assert_equals(videoProperties.getGopStructure(), ())

    # Analyse first GOP
    progress = av.NoDisplayProgress()
    inputFile.analyse( progress, av.eAnalyseLevelFirstGop )

    # Check properties after GOP analysis
    videoProperties = inputFile.getProperties().getVideoProperties()[0]
    assert_not_equals(videoProperties.getGopStructure(), ())
