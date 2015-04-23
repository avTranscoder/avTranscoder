import argparse # python2.7+

from pyAvTranscoder import avtranscoder as av

# Create command-line interface
parser = argparse.ArgumentParser(
    prog='pythumbnail',
    description='''Generate jpeg thumbnail from video/image.''',
)

# requirements
parser.add_argument('inputFileName', help='It could be any media file with at least one video stream (video, image...). Support file without extension.')
# options
parser.add_argument("-o", "--outputFile", dest="outputFileName", default="thumbnail.jpg", help="Set the output filename (thumbnail.jpg by default). Must be with jpg extension!")
parser.add_argument("-t", "--time", dest="time", type=float, default=0, help="Set time (in seconds) of where to seek in the video stream to generate the thumbnail (0 by default).")
parser.add_argument("-f", "--frame", dest="frame", type=int, default=0, help="Set time (in frames) of where to seek in the video stream to generate the thumbnail (0 by default). Warning: priority to frame if user indicates both frame and time!")
parser.add_argument("-w", "--width", dest="width", type=int, default=0, help="Override the width of the thumbnail (same as input by default).")
parser.add_argument("-he", "--height", dest="height", type=int, default=0, help="Override the height of the thumbnail (same as input by default).")
# Parse command-line
args = parser.parse_args()

# setup avtranscoder
logger = av.Logger().setLogLevel(av.AV_LOG_QUIET)
av.preloadCodecsAndFormats()

# create input file
inputFile = av.InputFile(args.inputFileName)
if len(inputFile.getProperties().getVideoProperties()) == 0:
    print "No video stream found in file ", args.inputFileName
    exit(1)

# seek in file
if args.frame:
    inputFile.seekAtFrame(args.frame)
elif args.time:
    inputFile.seekAtTime(args.time)

# create output file (need to set format profile of encoding to force output format to mjpeg)
formatProfile = av.ProfileMap()
formatProfile[ av.avProfileIdentificator ] = "thumbnailFormatPreset"
formatProfile[ av.avProfileIdentificatorHuman ] = "Thumbnail format preset"
formatProfile[ av.avProfileType ] = av.avProfileTypeFormat
formatProfile[ av.avProfileFormat ] = "mjpeg"
outputFile = av.OutputFile( args.outputFileName )
outputFile.setProfile( formatProfile )

# create input stream
videoStreamIndex = inputFile.getProperties().getVideoProperties()[0].getStreamIndex()
inputStream = av.InputStream(inputFile, videoStreamIndex)
inputStream.activate()

# create output stream
videoProfile = av.ProfileMap()
videoProfile[ av.avProfileIdentificator ] = "thumbnailVideoPreset"
videoProfile[ av.avProfileIdentificatorHuman ] = "Thumbnail video preset"
videoProfile[ av.avProfileType ] = av.avProfileTypeVideo
videoProfile[ av.avProfileCodec ] = "mjpeg"
videoProfile[ av.avProfilePixelFormat ] = "yuvj420p"
if args.width:
    videoProfile[ av.avProfileWidth ] = str(args.width)
if args.height:
    videoProfile[ av.avProfileHeight ] = str(args.height)
outputStream = av.StreamTranscoder( inputStream, outputFile, videoProfile )

# create transcoder
transcoder = av.Transcoder( outputFile )
transcoder.add( outputStream );

# launch process
outputFile.beginWrap();
transcoder.preProcessCodecLatency();
transcoder.processFrame();
outputFile.endWrap();
