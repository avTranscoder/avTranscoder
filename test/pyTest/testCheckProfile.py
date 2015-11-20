from nose.tools import *

from pyAvTranscoder import avtranscoder as av

av.preloadCodecsAndFormats()
av.Logger.setLogLevel(av.AV_LOG_QUIET)


def testCheckFormatProfile():
    """
    Check a format profile.
    """
    # create custom profile
    customProfile = av.ProfileMap()
    assert_equals( av.ProfileLoader.checkFormatProfile( customProfile ), False )
    customProfile[av.avProfileIdentificator] = "customProfile"
    assert_equals( av.ProfileLoader.checkFormatProfile( customProfile ), False )
    customProfile[av.avProfileIdentificatorHuman] = "custom profile"
    assert_equals( av.ProfileLoader.checkFormatProfile( customProfile ), False )
    customProfile[av.avProfileType] = av.avProfileTypeFormat
    assert_equals( av.ProfileLoader.checkFormatProfile( customProfile ), False )
    customProfile[av.avProfileFormat] = "avi"

    # check the valid profile
    assert_equals( av.ProfileLoader.checkFormatProfile( customProfile ), True )


def testCheckVideoProfile():
    """
    Check a video profile.
    """
    # create custom profile
    customProfile = av.ProfileMap()
    assert_equals( av.ProfileLoader.checkVideoProfile( customProfile ), False )
    customProfile[av.avProfileIdentificator] = "customProfile"
    assert_equals( av.ProfileLoader.checkVideoProfile( customProfile ), False )
    customProfile[av.avProfileIdentificatorHuman] = "custom profile"
    assert_equals( av.ProfileLoader.checkVideoProfile( customProfile ), False )
    customProfile[av.avProfileType] = av.avProfileTypeVideo
    assert_equals( av.ProfileLoader.checkVideoProfile( customProfile ), False )
    customProfile[av.avProfileCodec] = "mpeg2video"

    # check the valid profile
    assert_equals( av.ProfileLoader.checkVideoProfile( customProfile ), True )


def testCheckAudioProfile():
    """
    Check a audio profile.
    """
    # create custom profile
    customProfile = av.ProfileMap()
    assert_equals( av.ProfileLoader.checkAudioProfile( customProfile ), False )
    customProfile[av.avProfileIdentificator] = "customProfile"
    assert_equals( av.ProfileLoader.checkAudioProfile( customProfile ), False )
    customProfile[av.avProfileIdentificatorHuman] = "custom profile"
    assert_equals( av.ProfileLoader.checkAudioProfile( customProfile ), False )
    customProfile[av.avProfileType] = av.avProfileTypeAudio
    assert_equals( av.ProfileLoader.checkAudioProfile( customProfile ), False )
    customProfile[av.avProfileCodec] = "pcm_s24le"

    # check the valid profile
    assert_equals( av.ProfileLoader.checkAudioProfile( customProfile ), True )

