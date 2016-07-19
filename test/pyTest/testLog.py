import os
from nose.tools import *

from pyAvTranscoder import avtranscoder as av


def _logMessages(logger):
    """
    Local function to log several messages.
    """
    logger.log(av.AV_LOG_DEBUG, 'This is a debug message!')
    logger.log(av.AV_LOG_INFO, 'This is an info message!')
    logger.log(av.AV_LOG_WARNING, 'This is a warning message!')
    logger.log(av.AV_LOG_ERROR, 'This is an error message!')


def testLogInFile():
    """
    Check the behavior of the logger file.
    """
    logger = av.Logger()
    # log several messages in the standard output
    _logMessages(logger)

    # redirect the output of the logging messages
    logger.logInFile()
    # the logger message should exist and be empty
    assert_equals(os.stat(av.LOG_FILE).st_size, 0)

    # log several messages in the logger file
    _logMessages(logger)
    # the logger message should exist and be filled
    assert_greater(os.stat(av.LOG_FILE).st_size, 0)
