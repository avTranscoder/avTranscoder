#!/usr/bin/env python

import argparse # python2.7+

from pyAvTranscoder import avtranscoder as av


def parseConfigFile( inputConfigFile, transcoder ):
    file = open( inputConfigFile, 'r' )
    for line in file:
        line = line.strip( '\n' )

        filename, operation = line.split( '=' )

        streamIndexes, profileName = operation.split( ':' )
        if "." in streamIndexes:
            streamIndex, subStreamIndex = map(int, streamIndexes.split( '.' ))
        else:
            streamIndex = int(streamIndexes)
            subStreamIndex = -1

        transcoder.add( filename, streamIndex, subStreamIndex, profileName )


# Create command-line interface
parser = argparse.ArgumentParser(
    prog='pythumbnail',
    description='''Generate jpeg thumbnail from video/image.''',
)

# requirements
parser.add_argument('configFileName', help='The config file will be parsed by foloowing this pattern: <media_file>=<stream_index><.substream_index>:<profile_name> \nGenerated streams (video and audio) and offset are not supported.')
parser.add_argument('outputFileName', help='The output media file.')
# Parse command-line
args = parser.parse_args()

# setup avtranscoder
logger = av.Logger().setLogLevel(av.AV_LOG_QUIET)
av.preloadCodecsAndFormats()

# create Transcoder
ouputFile = av.OutputFile( args.outputFileName )
transcoder = av.Transcoder( ouputFile )

# parse configuration file
parseConfigFile( args.configFileName, transcoder )

# initialize Transcoder
transcoder.setProcessMethod( av.eProcessMethodLongest )

# Process transcode
progress = av.ConsoleProgress()
transcoder.process( progress )
