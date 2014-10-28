import sys

from mediaManager import mediaCore
from mediaManager import mediaIO
from mediaManager import mediaTransform

def parseConfigFile( inputConfigFile, transcoder ):
	"""
	Parse a config file with for each lines:
	<media_file>=<stream_index>.<substream_index>:<profile_name>
	* The substream index is not necessary.
	* Dummy streams (video and audio) are not supported.
	* Offset is not supported.
	"""
	file = open( inputConfigFile, 'r' )
	for line in file:
		line = line.strip( '\n' )

		name = line.split( '=' )
		filename = name[0]
		
		params = name[1].split( ':' )
		streamIndex = int( params[0] )
		subStreamIndex = -1 if '.' not in params[0] else params[0].split( '.' )[0]
		profileName = '' if not params[1] else params[1]

		transcoder.add( filename, streamIndex, subStreamIndex, profileName )

if len(sys.argv) < 3:
	print "pyProcessor requires an input config file and an output media filename"
	exit(1)

# create Transcoder
ouputFile = mediaIO.OutputFile( sys.argv[2] )
transcoder = mediaIO.Transcoder( ouputFile )

# parse configuration file
inputConfigFile = sys.argv[1]
parseConfigFile( inputConfigFile, transcoder )

# initialize Transcoder
transcoder.setProcessMethod( mediaIO.eProcessMethodLongest )
transcoder.init()

# Process transcode
progress = mediaCore.ProgressListener()
transcoder.process( progress )
