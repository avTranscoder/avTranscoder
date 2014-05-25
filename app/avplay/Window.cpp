
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

Reader* Window::m_reader = NULL;

size_t Window::m_width = 0;
size_t Window::m_height = 0;

int Window::m_x1 = -1.0;
int Window::m_x2 =  1.0;
int Window::m_y1 =  1.0;
int Window::m_y2 = -1.0;

int Window::m_xMinViewport = 0;
int Window::m_yMinViewport = 0;
int Window::m_xMouseRef = 0;
int Window::m_yMouseRef = 0;

int Window::m_windowId = 0;

float Window::m_currentZoom = 1.0;
float Window::m_factorZoom = 1.25;

float Window::m_scale = 1.0;

bool Window::m_play = false;

bool Window::m_flip = false;
bool Window::m_flop = false;

// channel properties
bool Window::m_showRedChannel   = false;
bool Window::m_showGreenChannel = false;
bool Window::m_showBlueChannel  = false;
bool Window::m_showAlphaChannel = false;

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

ImgProperties m_imageProperties;


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
	m_imageProperties.data   = data;
	m_imageProperties.internalFormat = internalFormat;
	m_imageProperties.width  = width;
	m_imageProperties.height = height;
	m_imageProperties.format = format;
	m_imageProperties.type   = type;

	switch( m_imageProperties.format )
	{
		case GL_LUMINANCE : m_imageProperties.component = 1; break;
		case GL_RGB       : m_imageProperties.component = 3; break;
		case GL_RGBA      : m_imageProperties.component = 4; break;
	}

	loadNewTexture( m_imageProperties );
}

Window::Window( Reader& reader )
{
	m_reader = &reader;
	m_width  = m_reader->getWidth();
	m_height = m_reader->getHeight();

	char *argv[2] = { (char*)"", NULL };
	int   argc    = 1;
	glutInit( &argc, argv );
	glutInitDisplayMode( GLUT_DOUBLE | GLUT_RGB | GLUT_RGBA | GLUT_MULTISAMPLE );
	glutInitWindowSize( m_width, m_height );
	glutInitWindowPosition( 0, 0 );
#ifdef GLUT_ACTION_ON_WINDOW_CLOSE
	glutSetOption( GLUT_ACTION_ON_WINDOW_CLOSE, GLUT_ACTION_CONTINUE_EXECUTION );
#endif

	m_windowId = glutCreateWindow("AV Player Viewer");
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
	displayNextFrame();
	glutMainLoop();
}

void Window::display()
{
	if( glutGetWindow() == 0 ) return;
	glClearColor(0.0f, 0.0f, 0.0f, 0.0f );
	glClear( GL_DEPTH_BUFFER_BIT | GL_COLOR_BUFFER_BIT );

	glBegin (GL_QUADS);

	float x1 = m_x1;
	float x2 = m_x2;

	float y1 = m_y1;
	float y2 = m_y2;

	if( m_flip )
	{
		y1 = -y1;
		y2 = -y2;
	}
	if( m_flop )
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
			glutDestroyWindow( m_windowId );
			m_windowId = 0;
			break;
		case 27: // ESCAPE key
			glutDestroyWindow( m_windowId );
			m_windowId = 0;
			break;
		case 'i':
			displayInformations();
			break;
		case 'z':
			glutReshapeWindow( m_width, m_height );
			m_currentZoom = 1.0;
			m_x1 = -1.0;
			m_x2 =  1.0;
			m_y1 =  1.0;
			m_y2 = -1.0;
			glutPostRedisplay();
			break;
		case 'h':
			displayHelp();
			break;
		case 32: // spacebar
			m_play = !m_play;
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

		case 'm':
			m_reader->printMetadatas();
			break;

		case 'H':
			if( shift )
			{
				m_flop = !m_flop;
				glutPostRedisplay();
			}
			break;
		case 'V':
			if( shift )
			{
				m_flip = !m_flip;
				glutPostRedisplay();
			}
			break;
	}
}

void Window::specialKeyboard( int k, int x, int y )
{
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
			break;
		case GLUT_KEY_RIGHT:
			// cursor move
			displayNextFrame();
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

		if( iX < 0 || iY < 0 || iX >= (int)m_imageProperties.width || iY >= (int)m_imageProperties.height )
			return;

		std::cout << "at " << std::setw(4) << iX << "," << std::setw(4) << (int)m_imageProperties.height - iY << ": ";

		for( size_t i = 0; i < m_imageProperties.component; i++ )
		{
			size_t idx = ( iX + iY * m_imageProperties.width ) * m_imageProperties.component + i;
			switch( m_imageProperties.type )
			{
				case GL_UNSIGNED_BYTE:
				{
					const unsigned char* d = (const unsigned char*) m_imageProperties.data;
					std::cout << std::setw(5) << (unsigned int) d[idx] ;
					break;
				}
				case GL_UNSIGNED_SHORT:
				{
					const unsigned short* d = (const unsigned short*) m_imageProperties.data;
					std::cout << std::setw(7) << d[idx] ;
					break;
				}
				case GL_FLOAT:
				{
					const float* d = (const float*) m_imageProperties.data;
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
			m_currentZoom *= m_factorZoom;
			zoom( m_factorZoom );
		}
		else
		{
			m_currentZoom /= m_factorZoom;
			zoom( 1.0 / m_factorZoom );
		}

		mapToImage(x, y, iX2, iY2);

		move( ( m_currentZoom / m_imageProperties.width * 2) * (iX2 - iX),
		      ( m_currentZoom / m_imageProperties.height * 2) * (iY2 - iY));

		glutPostRedisplay ();
	}

	m_xMouseRef = x;
	m_yMouseRef = y;
}

void Window::motion( int x, int y )
{
	float x_diff, y_diff;

	x_diff = ( x - m_xMouseRef ) / m_currentZoom;
	y_diff = ( m_yMouseRef - y ) / m_currentZoom;

	if( m_flip )
	{
		y_diff *= -1.0;
	}

	if( m_flop )
	{
		x_diff *= -1.0;
	}

	move( m_currentZoom / m_imageProperties.width  * 2 * x_diff,
	      m_currentZoom / m_imageProperties.height * 2 * y_diff );

	m_xMouseRef = x;
	m_yMouseRef = y;

	glutPostRedisplay();
}

void Window::reshape( int width, int height )
{
	float w, h, xPos, yPos;

	if( (float) m_width / m_height > (float) width / height )
	{
		w = width;
		h = 1.0f * m_height / m_width * (float)width;
		xPos = 0.0;
		yPos = 0.5f * (height - h);
	}
	else
	{
		w = 1.0f * m_width / m_height * (float)height;
		h = height;
		xPos = 0.5f * (width - w);
		yPos = 0.0;
	}

	m_xMinViewport = xPos;
	m_yMinViewport = yPos;

	m_scale = w / m_width;

	glViewport( (GLsizei) xPos, (GLsizei) yPos, (GLsizei) w, (GLsizei) h );
	glutReshapeWindow( width, height );
}

void Window::displayHelp()
{
	static const std::string kViewerHelp =
	"Av Player Viewer Help\n" \
	"i                  : information about image (dimensions, bit depth, channels)\n"\
	"m                  : full metadatas of media\n"\
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
	switch( m_imageProperties.format )
	{
		case GL_LUMINANCE : textureType = "Gray "; break;
		case GL_RGB       : textureType = "RGB  "; break;
		case GL_RGBA      : textureType = "RGBA "; break;
	}
	switch( m_imageProperties.type )
	{
		case GL_UNSIGNED_BYTE  : textureType += "8 bits"; break;
		case GL_UNSIGNED_SHORT : textureType += "16 bits"; break;
		case GL_FLOAT          : textureType += "32 float"; break;
	}
	std::cout << textureType << " " << m_width << "x" << m_height << std::endl;
}

void Window::move( float x, float y )
{
	m_x1 += x;
	m_x2 += x;
	m_y1 += y;
	m_y2 += y;
}

void Window::zoom(float factor)
{
	m_x1 *= factor;
	m_x2 *= factor;
	m_y1 *= factor;
	m_y2 *= factor;
}

void Window::mapToImage(int x, int y, int &iX, int &iY)
{
	int mapX, mapY;
	float mx, my;

	mapX = ( x - m_xMinViewport ) / m_scale;
	mapY = ( y - m_yMinViewport ) / m_scale;

	if( !m_flip )
	{
		mapY = m_imageProperties.height - mapY;
	}

	if( m_flop )
	{
		mapX = m_imageProperties.width - mapX;
	}

	mx = (float)mapX / (float)m_imageProperties.width * 2.0 - 1.0;
	iX = ((m_x1 - mx) / (m_currentZoom * 2.0) * (float)m_imageProperties.width * -1.0) + 0.5;

	my = (float)mapY / (float)m_imageProperties.height * 2.0 - 1.0;
	iY = ((m_y1 - my) / (m_currentZoom * 2.0) * (float)m_imageProperties.height * -1.0) + 0.5;	
}

void Window::setTransfert( float red, float green, float blue, float alpha )
{
	switch( m_imageProperties.format )
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
	ImgProperties p = m_imageProperties;
	if( ! channel )
	{
		setTransfert( red, green, blue );
		m_showRedChannel   = false;
		m_showGreenChannel = false;
		m_showBlueChannel  = false;
		m_showAlphaChannel = false;
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
	displayChannelTexture( m_showRedChannel, 1.f, 0.f, 0.f );
}

void Window::showGreenChannelTexture( )
{
	displayChannelTexture( m_showGreenChannel, 0.f, 1.f, 0.f );
}

void Window::showBlueChannelTexture( )
{
	displayChannelTexture( m_showBlueChannel, 0.f, 0.f, 1.f );
}

void Window::showAlphaChannelTexture( )
{
	glutPostRedisplay ();
}

void Window::displayNextFrame()
{
	const char* buffer = m_reader->readNextFrame();
	loadNewTexture( buffer, m_reader->getComponents(), m_reader->getWidth(), m_reader->getHeight(), GL_RGB, GL_UNSIGNED_BYTE );
}

void Window::loopPlaying( int value )
{
	if( m_play )
		glutTimerFunc( 40, &loopPlaying, 0 );
	displayNextFrame();
}
