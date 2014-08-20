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

icommonInclude = []
commonLibDir   = []
installPrefix  = "/usr/local"

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
libavInclude = config.get( 'LIBAV', 'inc' ).split( splitChar )
libavLibDir  = config.get( 'LIBAV', 'libdir' ).split( splitChar )

env     = Environment().Clone()
envJava = Environment().Clone()
envPy   = Environment().Clone()

# C++ environment
env.Append(
    CPPPATH = [
        libavInclude,
        "#src",
    ],
    CXXFLAGS = [
        '-Wall',
        '-fPIC',
    ],
    LIBPATH = [
        libavLibDir,
        "#src",
        "#build/src"
    ],
)

# Java environment
envJava.Replace(
    CPPPATH = [
        javaInclude,
        libavInclude,
        ".",
    ],
    SWIGCXXFILESUFFIX= '_wrapJava$CXXFILESUFFIX',
    CXXFLAGS = [
        '-Wall',
    ],
    SWIGFLAGS = [
        '-java',
        '-c++',
        '-fcompact',
    ],
    LINKFLAGS = [
    ],
    LIBPATH = [
        libavLibDir,
        "#src",
    ],
    JARCHDIR = env.Dir('#build/'+mymode+'/src/AvTranscoder').get_abspath(),
)

envJava.Append(
    SWIGPATH = envJava['CPPPATH'],
    SWIGFLAGS = [ '-package', 'org.AvTranscoder' ],
    )

# Python environment
envPy.Replace(
    CPPPATH = [
        pyInclude,
        libavInclude,
        ".",
    ],
    SWIGCXXFILESUFFIX= '_wrapPython$CXXFILESUFFIX',
    SHLIBPREFIX= '_',
    CXXFLAGS = [
        '-Wall',
    ],
    SWIGFLAGS = [
        '-python',
        '-c++',
        '-fcompact',
    ],
    LINKFLAGS = [
    ],
    LIBPATH = [
        libavLibDir,
        "#src",
    ],
)

envPy.Append( SWIGPATH = envPy['CPPPATH'] )

conf = Configure( env )

resampleLibraryFlag = '-DAV_RESAMPLE_LIBRARY'
resampleLibraryName = 'avresample'

if not conf.CheckLibWithHeader('avutil', 'libavutil/avutil.h', 'c'):
    sys.exit( 0 )

if not conf.CheckLibWithHeader('avresample', 'libavresample/avresample.h', 'c'):
    if conf.CheckLibWithHeader('swresample', 'libswresample/swresample.h', 'c'):
        resampleLibraryFlag = '-DFF_RESAMPLE_LIBRARY'
        resampleLibraryName = 'swresample'

if not conf.CheckLibWithHeader('avcodec', 'libavcodec/avcodec.h', 'c'):
    sys.exit( 0 )

if not conf.CheckLibWithHeader('avformat', 'libavformat/avformat.h', 'c'):
    sys.exit( 0 )

if not conf.CheckLibWithHeader('swscale', 'libswscale/swscale.h', 'c'):
    sys.exit( 0 )


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
Export( "mymode" )
Export( "installPrefix" )
Export( "resampleLibraryName" )

VariantDir( 'build/'+mymode+'/src', 'src', duplicate = 0 )
VariantDir( 'build/'+mymode+'/app', 'app', duplicate = 0 )

sconscripts = [
    'build/src/SConscript',
    'build/app/SConscript',
]
 
SConscript('src/SConscript', variant_dir='build/'+mymode+'/src')
SConscript('app/SConscript', variant_dir='build/'+mymode+'/app')
