import os

# Check if environment is setup to run the tests
if os.environ.get('AVTRANSCODER_TEST_AUDIO_WAVE_FILE') is None or \
    os.environ.get('AVTRANSCODER_TEST_AUDIO_MOV_FILE') is None:
    from nose.plugins.skip import SkipTest
    raise SkipTest("Need to define environment variables "
        "AVTRANSCODER_TEST_AUDIO_WAVE_FILE and "
        "AVTRANSCODER_TEST_AUDIO_MOV_FILE")

from nose.tools import *

from pyAvTranscoder import avtranscoder as av


@raises(IndexError)
def testAccessOfUnknownAudioCodecOptions():
    """
    Check access to an unknown codec option.
    """
    optionName = 'a'
    inputFileName = os.environ['AVTRANSCODER_TEST_AUDIO_WAVE_FILE']
    inputFile = av.InputFile(inputFileName)
    audioProperties = inputFile.getProperties().getAudioProperties()[0]

    audioProperties.getCodecOption(optionName)
    

def testAccessOfKnownCodecOptionInAudioStream():
    """
    Check access to a known codec option from an audio stream.
    """
    optionName = 'b'
    inputFileName = os.environ['AVTRANSCODER_TEST_AUDIO_WAVE_FILE']
    inputFile = av.InputFile(inputFileName)
    audioProperties = inputFile.getProperties().getAudioProperties()[0]

    option = audioProperties.getCodecOption(optionName)

    # common attributes
    assert_equals(option.getName(), optionName)
    assert_equals(option.getHelp(), 'set bitrate (in bits/s)')
    assert_equals(option.getType(), av.eOptionBaseTypeInt)
    assert_greater(option.getOffset(), 0)
    assert_equals(option.getMin(), 0.0)
    assert_greater(option.getMax(), 0.0)
    assert_greater(option.getMax(), 0.0)

    # flags
    assert_equals(option.isEncodingOpt(), True)
    assert_equals(option.isDecodingOpt(), False)
    assert_equals(option.isAudioOpt(), True)
    assert_equals(option.isVideoOpt(), True)
    assert_equals(option.isSubtitleOpt(), False)

    # value
    assert_equals(option.getInt(), audioProperties.getBitRate())
    assert_equals(option.getDefaultInt(), 200000)  # see AV_CODEC_DEFAULT_BITRATE

    # childs
    assert_equals(option.hasChild(), False)
    assert_equals(len(option.getChilds()), 0)


def testAccessOfKnownCodecOptionInVideoStream():
    """
    Check access to a known codec option from a video stream.
    """
    optionName = 'flags'
    inputFileName = os.environ['AVTRANSCODER_TEST_AUDIO_MOV_FILE']
    inputFile = av.InputFile(inputFileName)
    videoProperties = inputFile.getProperties().getVideoProperties()[0]

    option = videoProperties.getCodecOption(optionName)

    # common attributes
    assert_equals(option.getName(), optionName)
    assert_equals(option.getUnit(), optionName)
    assert_equals(option.getType(), av.eOptionBaseTypeGroup)

    # flags
    assert_equals(option.isEncodingOpt(), True)
    assert_equals(option.isDecodingOpt(), True)
    assert_equals(option.isAudioOpt(), True)
    assert_equals(option.isVideoOpt(), True)
    assert_equals(option.isSubtitleOpt(), True)

    # value
    assert_equals(option.getInt(), 0)
    assert_equals(option.getDefaultInt(), 0)

    # childs
    assert_equals(option.hasChild(), True)
    assert_greater(len(option.getChilds()), 0)
    # check if all the childs have a name
    for child in option.getChilds():
        assert_greater(len(child.getName()), 0)
