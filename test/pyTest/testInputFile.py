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


@raises(RuntimeError)
def testInputFileAnalyseHeaderTryToAccessisInterlaced():
    """
    Analyse only header of an InputFile, and try to access a properties computed when access the first GOP.
    """
    inputFileName = os.environ['AVTRANSCODER_TEST_VIDEO_MOV_FILE']
    inputFile = av.InputFile(inputFileName)

    # The analyse of the first GOP is not done yet
    videoProperties = inputFile.getProperties().getVideoProperties()[0]
    videoProperties.isInterlaced()


@raises(RuntimeError)
def testInputFileAnalyseHeaderTryToAccessisTopFieldFirst():
    """
    Analyse only header of an InputFile, and try to access a properties computed when access the first GOP.
    """
    inputFileName = os.environ['AVTRANSCODER_TEST_VIDEO_MOV_FILE']
    inputFile = av.InputFile(inputFileName)

    # The analyse of the first GOP is not done yet
    videoProperties = inputFile.getProperties().getVideoProperties()[0]
    videoProperties.isTopFieldFirst()


@raises(RuntimeError)
def testInputFileAnalyseHeaderTryToAccessisGopSize():
    """
    Analyse only header of an InputFile, and try to access a properties computed when access the first GOP.
    """
    inputFileName = os.environ['AVTRANSCODER_TEST_VIDEO_MOV_FILE']
    inputFile = av.InputFile(inputFileName)

    # The analyse of the first GOP is not done yet
    videoProperties = inputFile.getProperties().getVideoProperties()[0]
    videoProperties.getGopSize()


@raises(RuntimeError)
def testInputFileAnalyseHeaderTryToAccessisGopStructure():
    """
    Analyse only header of an InputFile, and try to access a properties computed when access the first GOP.
    """
    inputFileName = os.environ['AVTRANSCODER_TEST_VIDEO_MOV_FILE']
    inputFile = av.InputFile(inputFileName)

    # The analyse of the first GOP is not done yet
    videoProperties = inputFile.getProperties().getVideoProperties()[0]
    videoProperties.getGopStructure()

@raises(RuntimeError)
def testInputFileSetupInvalidUnwrappingProfile():
    """
    Analyse only header of an InputFile, and try to access a properties computed when access the first GOP.
    """
    inputFileName = os.environ['AVTRANSCODER_TEST_VIDEO_MOV_FILE']

    emptyUnwrappingProfile = av.ProfileMap()
    inputFile = av.InputFile(inputFileName)
    inputFile.setupUnwrapping(emptyUnwrappingProfile);


def testInputFileAnalyseFirstGop():
    """
    Analyse the first gop of an InputFile, and check if the correct attributes are filled.
    """
    inputFileName = os.environ['AVTRANSCODER_TEST_VIDEO_MOV_FILE']
    inputFile = av.InputFile( inputFileName )

    # Analyse first GOP
    progress = av.NoDisplayProgress()
    inputFile.analyse( progress, av.eAnalyseLevelFirstGop )

    # Check properties after GOP analysis
    videoProperties = inputFile.getProperties().getVideoProperties()[0]
    assert_greater(videoProperties.getGopSize(), 0)
    assert_not_equals(videoProperties.getGopStructure(), ())
    for image in videoProperties.getGopStructure():
        pictureType = image[0]
        encodedPictureSize = image[1]
        assert_in(pictureType, ['I', 'P', 'B'])
        assert_greater(encodedPictureSize, 0)
    assert_not_equals(videoProperties.getDuration(), 0)
    assert_not_equals(videoProperties.getBitRate(), 0)
    assert_not_equals(videoProperties.getNbFrames(), 0)


def testInputFileAnalyseFull():
    """
    Analyse the full video stream of an InputFile, and check if the correct attributes are filled.
    """
    inputFileName = os.environ['AVTRANSCODER_TEST_VIDEO_RAW_FILE']
    inputFile = av.InputFile( inputFileName )

    # Analyse full stream
    progress = av.ConsoleProgress()
    inputFile.analyse(progress, av.eAnalyseLevelFull)

    # Check properties after full analysis
    videoProperties = inputFile.getProperties().getVideoProperties()[0]
    assert_greater(videoProperties.getGopSize(), 0)
    assert_not_equals(videoProperties.getGopStructure(), ())
    for image in videoProperties.getGopStructure():
        pictureType = image[0]
        encodedPictureSize = image[1]
        assert_in(pictureType, ['I', 'P', 'B'])
        assert_greater(encodedPictureSize, 0)
    assert_not_equals(videoProperties.getDuration(), 0)
    assert_not_equals(videoProperties.getBitRate(), 0)
    assert_not_equals(videoProperties.getNbFrames(), 0)
