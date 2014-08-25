EnsureSConsVersion(2, 3, 0)

import os
import sys
import ConfigParser

mymode = ARGUMENTS.get('mode', 'release')

if not (mymode in ['debug', 'release']):
    print "Error: expected 'debug' or 'release', found: " + mymode
    Exit(1)

config = ConfigParser.RawConfigParser()

config.read( [
    'scons.cfg',
] )

commonInclude = []
commonLibDir   = []

splitChar = ";"

if config.has_section( 'COMMON' ):
    if( config.has_option( 'COMMON', 'inc' ) ):
        commonInclude.append( config.get( 'COMMON', 'inc' ).split( splitChar ) )
    if( config.has_option( 'COMMON', 'libdir' ) ):
        commonLibDir.append( config.get( 'COMMON', 'libdir' ).split( splitChar ) )
    if( config.has_option( 'COMMON', 'prefix' ) ):
        installPrefix = config.get( 'COMMON', 'prefix' )

if not config.has_section( 'LIBAV' ):
    print "missing LIBAV section in scons.cfg file configuration"
    sys.exit( -1 )

if not config.has_section( 'JAVA' ):
    print "missing JAVA section in scons.cfg file configuration"
    sys.exit( -1 )

if not config.has_section( 'PYTHON' ):
    print "missing PYTHON section in scons.cfg file configuration"
    sys.exit( -1 )


javaInclude  = config.get( 'JAVA', 'inc' ).split( splitChar )

pyInclude    = config.get( 'PYTHON', 'inc' ).split( splitChar )
pyLibrary    = []
if config.has_option( 'PYTHON', 'libdir' ):
    pyLibrary = config.get( 'PYTHON', 'libdir' ).split( splitChar )

libavInclude = config.get( 'LIBAV', 'inc' ).split( splitChar )
libavLibDir  = config.get( 'LIBAV', 'libdir' ).split( splitChar )

env = Environment( ENV = { 'PATH' : os.environ[ 'PATH' ] } )

# C++ environment
env.Append(
    CPPPATH = [
        libavInclude,
        "#src",
    ] + commonInclude,
    CXXFLAGS = [
        '-Wall',
        '-fPIC',
    ],
    LIBPATH = [
        libavLibDir,
        "#src",
        "#build/src"
    ] + commonLibDir,
)

if os.name == "nt" and sys.platform.startswith("win"): # detect windows plateform
    env.AppendUnique( CPPDEFINES = 'WIN' )
    env.AppendUnique( CPPDEFINES = 'WIN32' )
    env.AppendUnique( CPPDEFINES = 'WINDOWS' )
    env.AppendUnique( CPPDEFINES = '_WINDOWS' )
    env.AppendUnique( CPPDEFINES = '__WINDOWS__' )
    env.AppendUnique( CPPDEFINES = '__STDC_CONSTANT_MACROS' )
    bits = 64
    if 'PROGRAMFILES(X86)' not in os.environ:
        bits = 32
    env.AppendUnique( CPPDEFINES = 'WIN'+str(bits) )
    env.AppendUnique( TMP = os.environ['TMP'].split( splitChar ) )

    if 'LIB' not in os.environ or 'LIBPATH' not in os.environ :
        print "Compiler environment not set."
        sys.exit( -1 )
    env.AppendUnique( LIB = os.environ['LIB'].split( splitChar ) )
    env.AppendUnique( LIBPATH = os.environ['LIBPATH'].split( splitChar ) )
    env.AppendUnique( TMP = os.environ['TMP'].split( splitChar ) )
else:
    env.AppendUnique( CPPDEFINES = 'UNIX' )
    env.AppendUnique( CPPDEFINES = '__UNIX__' )
    if sys.platform.startswith( "darwin" ): # for disabling macros such as check, verify, require ... ( AssertMacros.h )
        env.AppendUnique( CPPDEFINES = '__ASSERT_MACROS_DEFINE_VERSIONS_WITHOUT_UNDERSCORES=0' )

envJava = env.Clone()
envPy   = env.Clone()

# Java environment
envJava.Replace(
    SWIGCXXFILESUFFIX= '_wrapJava$CXXFILESUFFIX',
    SWIGFLAGS = [
        '-java',
        '-c++',
        '-fcompact',
    ],
)
envJava.AppendUnique(
    CPPPATH = javaInclude,
    SWIGPATH = envJava['CPPPATH'],
    SWIGFLAGS = [ '-package', 'org.AvTranscoder' ],
    JARCHDIR = env.Dir('#build/'+mymode+'/src/AvTranscoder').get_abspath(),
)

# Python environment
envPy.Replace(
    SWIGCXXFILESUFFIX= '_wrapPython$CXXFILESUFFIX',
    SHLIBPREFIX= '_',
    SWIGFLAGS = [
        '-python',
        '-c++',
        '-fcompact',
    ],
)
envPy.AppendUnique(
    CPPPATH = pyInclude,
    LIBPATH = pyLibrary,
    SWIGPATH = envPy['CPPPATH']
)

conf = Configure( env )

resampleLibraryFlag = '-DAV_RESAMPLE_LIBRARY'
resampleLibraryName = 'avresample'

if os.name == "nt" and sys.platform.startswith("win"): # detect windows plateform
    if not conf.CheckLibWithHeader('avutil', 'libavutil/avutil.h', 'c++'):
        sys.exit( -1 )

    if not conf.CheckLibWithHeader('avcodec', 'libavcodec/avcodec.h', 'c++'):
        sys.exit( -1 )

    if not conf.CheckLibWithHeader('avformat', 'libavformat/avformat.h', 'c++'):
        sys.exit( -1 )

    if not conf.CheckLibWithHeader('swscale', 'libswscale/swscale.h', 'c++'):
        sys.exit( -1 )

    if not conf.CheckLibWithHeader('avresample', 'libavresample/avresample.h', 'c++'):
        if conf.CheckLibWithHeader('swresample', 'libswresample/swresample.h', 'c++'):
            resampleLibraryFlag = '-DFF_RESAMPLE_LIBRARY'
            resampleLibraryName = 'swresample'
else:
    if not conf.CheckCHeader('libavutil/avutil.h'):
        sys.exit( -1 )

    if not conf.CheckCHeader('libavcodec/avcodec.h'):
        sys.exit( -1 )

    if not conf.CheckCHeader('libavformat/avformat.h'):
        sys.exit( -1 )

    if not conf.CheckCHeader('libswscale/swscale.h'):
        sys.exit( -1 )

    if not conf.CheckCHeader('libavresample/avresample.h'):
        if conf.CheckCHeader('libswresample/swresample.h'):
            resampleLibraryFlag = '-DFF_RESAMPLE_LIBRARY'
            resampleLibraryName = 'swresample'

env.Append(
    CXXFLAGS = resampleLibraryFlag
)
envJava.Append(
    CXXFLAGS = resampleLibraryFlag
)
envPy.Append(
    CXXFLAGS = resampleLibraryFlag
)

if mymode == "release":
        env.Append(CCFLAGS = ['-O3'])
if mymode == "debug":
        env.Append(CCFLAGS = ['-pg', '-g'])

Export( "env" )
Export( "envJava" )
Export( "envPy" )
Export( "resampleLibraryName" )
Export( "mymode" )

VariantDir( 'build/'+mymode+'/src', 'src', duplicate = 0 )
VariantDir( 'build/'+mymode+'/app', 'app', duplicate = 0 )
 
SConscript('src/SConscript', variant_dir='build/'+mymode+'/src')
SConscript('app/cpp/SConscript', variant_dir='build/'+mymode+'/app/cpp')
#SConscript('app/java/SConscript', variant_dir='build/'+mymode+'/app/java')
