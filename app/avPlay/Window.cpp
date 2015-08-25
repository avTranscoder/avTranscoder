
#include "Window.hpp"

#ifdef __APPLE__
 #include <OpenGL/gl.h>
 #include <GLUT/glut.h>
#else
 #include <GL/gl.h>
 #include <GL/glut.h>
 #include <GL/freeglut.h>
#endif

#include <iostream>
#include <iomanip>
#include <cstring>

avtranscoder::VideoReader* Window::_reader = NULL;

size_t Window::_width = 0;
size_t Window::_height = 0;

int Window::_x1 = -1.0;
int Window::_x2 =  1.0;
int Window::_y1 =  1.0;
int Window::_y2 = -1.0;

int Window::_xMinViewport = 0;
int Window::_yMinViewport = 0;
int Window::_xMouseRef = 0;
int Window::_yMouseRef = 0;

int Window::_windowId = 0;

float Window::_currentZoom = 1.0;
float Window::_factorZoom = 1.25;

float Window::_scale = 1.0;

bool Window::_play = false;

bool Window::_flip = false;
bool Window::_flop = false;

// channel properties
bool Window::_showRedChannel   = false;
bool Window::_showGreenChannel = false;
bool Window::_showBlueChannel  = false;
bool Window::_showAlphaChannel = false;

// image properties
struct ImgProperties
{
	const char* data;
	GLint  internalFormat;
	size_t width;
	size_t height;
	GLenum format;
	GLenum type;
	size_t component;
};

ImgProperties _imageProperties;


void loadNewTexture( const ImgProperties& properties )
{
	// loading texture
	glTexImage2D( GL_TEXTURE_2D, 0, properties.internalFormat, properties.width, properties.height, 0, properties.format, properties.type, properties.data );
	glTexParameterf(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
	glTexParameterf(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);

	glTexParameterf(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_REPEAT);
	glTexParameterf(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_REPEAT);

	glEnable(GL_TEXTURE_2D);
}

void loadNewTexture( const char* data, GLint internalFormat, size_t width, size_t height, GLenum format, GLenum type )
{
	_imageProperties.data   = data;
	_imageProperties.internalFormat = internalFormat;
	_imageProperties.width  = width;
	_imageProperties.height = height;
	_imageProperties.format = format;
	_imageProperties.type   = type;

	switch( _imageProperties.format )
	{
		case GL_LUMINANCE : _imageProperties.component = 1; break;
		case GL_RGB       : _imageProperties.component = 3; break;
		case GL_RGBA      : _imageProperties.component = 4; break;
	}

	loadNewTexture( _imageProperties );
}

Window::Window( avtranscoder::VideoReader& reader )
{
	_reader = &reader;
	_width  = _reader->getWidth();
	_height = _reader->getHeight();

	char *argv[2] = { (char*)"", NULL };
	int   argc    = 1;
	glutInit( &argc, argv );
	glutInitDisplayMode( GLUT_DOUBLE | GLUT_RGB | GLUT_RGBA | GLUT_MULTISAMPLE );
	glutInitWindowSize( _width, _height );
	glutInitWindowPosition( 0, 0 );
#ifdef GLUT_ACTION_ON_WINDOW_CLOSE
	glutSetOption( GLUT_ACTION_ON_WINDOW_CLOSE, GLUT_ACTION_CONTINUE_EXECUTION );
#endif

	_windowId = glutCreateWindow("AV Player Viewer");
	glClearColor(0.0f, 0.0f, 0.0f, 0.0f );
	glClear( GL_DEPTH_BUFFER_BIT | GL_COLOR_BUFFER_BIT );

	glutDisplayFunc ( display );
	glutKeyboardFunc( keyboard );
	glutSpecialFunc ( specialKeyboard );
	glutMouseFunc   ( mouse );
	glutMotionFunc  ( motion );
	glutReshapeFunc ( reshape );
}

void Window::launch()
{
	displayFirstFrame();
	glutMainLoop();
}

void Window::display()
{
	if( glutGetWindow() == 0 ) return;
	glClearColor(0.0f, 0.0f, 0.0f, 0.0f );
	glClear( GL_DEPTH_BUFFER_BIT | GL_COLOR_BUFFER_BIT );

	glBegin (GL_QUADS);

	float x1 = _x1;
	float x2 = _x2;

	float y1 = _y1;
	float y2 = _y2;

	if( _flip )
	{
		y1 = -y1;
		y2 = -y2;
	}
	if( _flop )
	{
		x1 = -x1;
		x2 = -x2;
	}

	glTexCoord2f( 0 , 0 );
	glVertex2f  ( x1, y1 );

	glTexCoord2f( 0, 1 );
	glVertex2f  ( x1, y2 );

	glTexCoord2f( 1, 1 );
	glVertex2f  ( x2, y2 );

	glTexCoord2f( 1, 0 );
	glVertex2f  ( x2, y1 );

	glEnd();

	glutSwapBuffers();
}

void Window::keyboard( unsigned char k, int x, int y )
{
	bool shift= false;
	if( glutGetModifiers() == GLUT_ACTIVE_SHIFT )
	{
		shift = true;
	}

	switch( k )
	{
		case '\r':
			glutDestroyWindow( _windowId );
			_windowId = 0;
			break;
		case 27: // ESCAPE key
			glutDestroyWindow( _windowId );
			_windowId = 0;
			break;
		case 'i':
			displayInformations();
			break;
		case 'z':
			glutReshapeWindow( _width, _height );
			_currentZoom = 1.0;
			_x1 = -1.0;
			_x2 =  1.0;
			_y1 =  1.0;
			_y2 = -1.0;
			glutPostRedisplay();
			break;
		case 'h':
			displayHelp();
			break;
		case 32: // spacebar
			_play = ! _play;
			loopPlaying(0);
			break;

		case 'r':
			showRedChannelTexture( ); 
			break;
		case 'g':
			showGreenChannelTexture( ); 
			break;
		case 'b':
			showBlueChannelTexture( ); 
			break;
		case 'a':
			showAlphaChannelTexture();
			break;
		case 'H':
			if( shift )
			{
				_flop = ! _flop;
				glutPostRedisplay();
			}
			break;
		case 'V':
			if( shift )
			{
				_flip = ! _flip;
				glutPostRedisplay();
			}
			break;
	}
}

void Window::specialKeyboard( int k, int x, int y )
{
	//std::cout << "k=" << k << " x=" << x << " y=" << y << std::endl; 
	switch (k)
	{
		case GLUT_KEY_UP:
			// cursor move
			break;
		case GLUT_KEY_DOWN:
			// cursor move
			break;
		case GLUT_KEY_LEFT:
			// cursor move
			displayPrevFrame();
			break;
		case GLUT_KEY_RIGHT:
			// cursor move
			displayNextFrame();
			break;
		case GLUT_KEY_HOME:
			displayFirstFrame();
		case GLUT_KEY_END:
			break;
		case GLUT_KEY_F1:
			displayHelp();
			break;
	}
}

void Window::mouse( int button, int state, int x, int y )
{
	if( state == 0 && button == 0 )
	{
		int iX, iY;

		mapToImage(x, y, iX, iY);

		if( iX < 0 || iY < 0 || iX >= (int)_imageProperties.width || iY >= (int)_imageProperties.height )
			return;

		std::cout << "at " << std::setw(4) << iX << "," << std::setw(4) << (int)_imageProperties.height - iY << ": ";

		for( size_t i = 0; i < _imageProperties.component; i++ )
		{
			size_t idx = ( iX + iY * _imageProperties.width ) * _imageProperties.component + i;
			switch( _imageProperties.type )
			{
				case GL_UNSIGNED_BYTE:
				{
					const unsigned char* d = (const unsigned char*) _imageProperties.data;
					std::cout << std::setw(5) << (unsigned int) d[idx] ;
					break;
				}
				case GL_UNSIGNED_SHORT:
				{
					const unsigned short* d = (const unsigned short*) _imageProperties.data;
					std::cout << std::setw(7) << d[idx] ;
					break;
				}
				case GL_FLOAT:
				{
					const float* d = (const float*) _imageProperties.data;
					std::cout << std::setw(10) << d[idx] ;
					break;
				}
			}
		}
		std::cout << std::endl;
	}
	if( state == 0 && ( button == 3 || button == 4 ) )
	{
		int iX, iY, iX2, iY2;

		mapToImage(x, y, iX, iY);

		if(button == 3)
		{
			_currentZoom *= _factorZoom;
			zoom( _factorZoom );
		}
		else
		{
			_currentZoom /= _factorZoom;
			zoom( 1.0 / _factorZoom );
		}

		mapToImage(x, y, iX2, iY2);

		move( ( _currentZoom / _imageProperties.width * 2) * (iX2 - iX),
		      ( _currentZoom / _imageProperties.height * 2) * (iY2 - iY));

		glutPostRedisplay ();
	}

	_xMouseRef = x;
	_yMouseRef = y;
}

void Window::motion( int x, int y )
{
	float x_diff, y_diff;

	x_diff = ( x - _xMouseRef ) / _currentZoom;
	y_diff = ( _yMouseRef - y ) / _currentZoom;

	if( _flip )
	{
		y_diff *= -1.0;
	}

	if( _flop )
	{
		x_diff *= -1.0;
	}

	move( _currentZoom / _imageProperties.width  * 2 * x_diff,
	      _currentZoom / _imageProperties.height * 2 * y_diff );

	_xMouseRef = x;
	_yMouseRef = y;

	glutPostRedisplay();
}

void Window::reshape( int width, int height )
{
	float w, h, xPos, yPos;

	if( (float) _width / _height > (float) width / height )
	{
		w = width;
		h = 1.0f * _height / _width * (float)width;
		xPos = 0.0;
		yPos = 0.5f * (height - h);
	}
	else
	{
		w = 1.0f * _width / _height * (float)height;
		h = height;
		xPos = 0.5f * (width - w);
		yPos = 0.0;
	}

	_xMinViewport = xPos;
	_yMinViewport = yPos;

	_scale = w / _width;

	glViewport( (GLsizei) xPos, (GLsizei) yPos, (GLsizei) w, (GLsizei) h );
	glutReshapeWindow( width, height );
}

void Window::displayHelp()
{
	static const std::string kViewerHelp =
	"Av Player Viewer Help\n" \
	"i                  : information about image (dimensions, bit depth, channels) + video stream\n"\
	"z                  : zoom view to 1:1\n"\
	"h, F1              : print help\n" \
	"SHIFT + V          : flip\n" \
	"SHIFT + H          : flop\n" \
	"clic on image      : print RGBA values\n" \
	"ESC, Return, Space : quit and continue process";
	std::cout << kViewerHelp << std::endl;
}

void Window::displayInformations()
{
	std::string textureType;
	switch( _imageProperties.format )
	{
		case GL_LUMINANCE : textureType = "Gray "; break;
		case GL_RGB       : textureType = "RGB  "; break;
		case GL_RGBA      : textureType = "RGBA "; break;
	}
	switch( _imageProperties.type )
	{
		case GL_UNSIGNED_BYTE  : textureType += "8 bits"; break;
		case GL_UNSIGNED_SHORT : textureType += "16 bits"; break;
		case GL_FLOAT          : textureType += "32 float"; break;
	}
	std::cout << textureType << " " << _width << "x" << _height << std::endl;

	// stream info
	_reader->printInfo();
}

void Window::move( float x, float y )
{
	_x1 += x;
	_x2 += x;
	_y1 += y;
	_y2 += y;
}

void Window::zoom(float factor)
{
	_x1 *= factor;
	_x2 *= factor;
	_y1 *= factor;
	_y2 *= factor;
}

void Window::mapToImage(int x, int y, int &iX, int &iY)
{
	int mapX, mapY;
	float mx, my;

	mapX = ( x - _xMinViewport ) / _scale;
	mapY = ( y - _yMinViewport ) / _scale;

	if( ! _flip )
	{
		mapY = _imageProperties.height - mapY;
	}

	if( _flop )
	{
		mapX = _imageProperties.width - mapX;
	}

	mx = (float)mapX / (float)_imageProperties.width * 2.0 - 1.0;
	iX = ((_x1 - mx) / (_currentZoom * 2.0) * (float)_imageProperties.width * -1.0) + 0.5;

	my = (float)mapY / (float)_imageProperties.height * 2.0 - 1.0;
	iY = ((_y1 - my) / (_currentZoom * 2.0) * (float)_imageProperties.height * -1.0) + 0.5;	
}

void Window::setTransfert( float red, float green, float blue, float alpha )
{
	switch( _imageProperties.format )
	{
		case GL_LUMINANCE :
			return;
		case GL_RGB :
			glPixelTransferf( GL_RED_SCALE,   red );
			glPixelTransferf( GL_GREEN_SCALE, green );
			glPixelTransferf( GL_BLUE_SCALE,  blue );
			break;
		case GL_RGBA :
			glPixelTransferf( GL_RED_SCALE,   red );
			glPixelTransferf( GL_GREEN_SCALE, green );
			glPixelTransferf( GL_BLUE_SCALE,  blue );
			glPixelTransferf( GL_ALPHA_SCALE, alpha );
			break;
	}
}

void Window::displayChannelTexture( bool& channel, const float red, const float green, const float blue )
{
	ImgProperties p = _imageProperties;
	if( ! channel )
	{
		setTransfert( red, green, blue );
		_showRedChannel   = false;
		_showGreenChannel = false;
		_showBlueChannel  = false;
		_showAlphaChannel = false;
		channel = true;
	}
	else
	{
		setTransfert( 1.f, 1.f, 1.f );
		channel = false;
	}
	loadNewTexture( p );

	glutPostRedisplay();
}

void Window::showRedChannelTexture( )
{
	displayChannelTexture( _showRedChannel, 1.f, 0.f, 0.f );
}

void Window::showGreenChannelTexture( )
{
	displayChannelTexture( _showGreenChannel, 0.f, 1.f, 0.f );
}

void Window::showBlueChannelTexture( )
{
	displayChannelTexture( _showBlueChannel, 0.f, 0.f, 1.f );
}

void Window::showAlphaChannelTexture( )
{
	glutPostRedisplay ();
}

void Window::displayNextFrame()
{
	const char* buffer = _reader->readNextFrame();
	loadNewTexture( buffer, _reader->getComponents(), _reader->getWidth(), _reader->getHeight(), GL_RGB, GL_UNSIGNED_BYTE );
	display();
}

void Window::displayPrevFrame()
{
	const char* buffer = _reader->readPrevFrame();
	loadNewTexture( buffer, _reader->getComponents(), _reader->getWidth(), _reader->getHeight(), GL_RGB, GL_UNSIGNED_BYTE );
	display();
}

void Window::displayFirstFrame()
{
	displayAtFrame( 0 );
}

void Window::displayAtFrame( const size_t frame )
{
	const char* buffer = _reader->readFrameAt( frame );
	loadNewTexture( buffer, _reader->getComponents(), _reader->getWidth(), _reader->getHeight(), GL_RGB, GL_UNSIGNED_BYTE );
	display();
}

void Window::loopPlaying( int value )
{
	if( _play )
		glutTimerFunc( 40, &loopPlaying, 0 );
	displayNextFrame();
}
