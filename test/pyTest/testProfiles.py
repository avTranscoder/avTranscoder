import os

# Check if environment is setup to run the tests
if os.environ.get('AVTRANSCODER_TEST_AUDIO_WAVE_FILE') is None:
    from nose.plugins.skip import SkipTest
    raise SkipTest("Need to define environment variables "
        "AVTRANSCODER_TEST_AUDIO_WAVE_FILE")

from nose.tools import *

from pyAvTranscoder import avtranscoder as av

def testLoadingAllDefaultProfiles():
    """
    Load all default profiles and check them.
    """
    profileLoader = av.ProfileLoader()

    formatProfiles = profileLoader.getFormatProfiles()
    assert_equals(4, len(formatProfiles))
    for formatProfile in formatProfiles:
        assert_equals("avProfileTypeFormat", formatProfile["avProfileType"])

    videoProfiles = profileLoader.getVideoProfiles()
    assert_equals(14, len(videoProfiles))
    for videoProfile in videoProfiles:
        assert_equals("avProfileTypeVideo", videoProfile["avProfileType"])

    audioProfiles = profileLoader.getAudioProfiles()
    assert_equals(6, len(audioProfiles))
    for audioProfile in audioProfiles:
        assert_equals("avProfileTypeAudio", audioProfile["avProfileType"])

def testAddingProfile():
    """
    Add a profile and get it back.
    """
    profileLoader = av.ProfileLoader(False)

    aviProfile = av.ProfileMap();
    aviProfile["avProfileName"] = "avi"
    aviProfile["avProfileLongName"] = "AVI (Audio Video Interleaved)"
    aviProfile["avProfileType"] = "avProfileTypeFormat"
    aviProfile["format"] = "avi"

    profileLoader.addProfile(aviProfile)
    extractedProfile = profileLoader.getProfile("avi")
    assert_equals(aviProfile["avProfileName"], extractedProfile["avProfileName"])
    assert_equals(aviProfile["avProfileLongName"], extractedProfile["avProfileLongName"])
    assert_equals(aviProfile["avProfileType"], extractedProfile["avProfileType"])
    assert_equals(aviProfile["format"], extractedProfile["format"])

@raises(RuntimeError)
def testGettingNotLoadedProfile():
    """
    Try to get a profile that is not loaded.
    """
    profileLoader = av.ProfileLoader(False)
    profileLoader.getProfile("avi")
