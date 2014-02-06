import os
import ConfigParser

config = ConfigParser.RawConfigParser()

config.read( [
    'scons.cfg',
] )

javaInclude  = config.get( 'JAVA', 'inc' ).split(':')
pyInclude    = config.get( 'PYTHON', 'inc' ).split(':')
libavInclude = config.get( 'LIBAV', 'inc' ).split(':')
libavLibDir  = config.get( 'LIBAV', 'libdir' ).split(':')

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
    ],
    LIBPATH = [
        libavLibDir,
        "#src",
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
        '-std=gnu++0x',
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

Export( "env" )
Export( "envJava" )
Export( "envPy" )

VariantDir( 'build/src', 'src', duplicate = 0 )
VariantDir( 'build/app', 'app', duplicate = 0 )

SConscript( [
    'build/src/SConscript',
    'build/app/SConscript',
] )
