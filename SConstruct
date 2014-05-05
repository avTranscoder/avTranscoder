import os
import ConfigParser

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
	JARCHDIR = env.Dir('#build/src/AvTranscoder').get_abspath(),
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

Export( "env" )
Export( "envJava" )
Export( "envPy" )
Export( "installPrefix" )

VariantDir( 'build/src', 'src', duplicate = 0 )
VariantDir( 'build/app', 'app', duplicate = 0 )

SConscript( [
	'build/src/SConscript',
	'build/app/SConscript',
] )
