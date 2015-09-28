#ifndef _AVPLAY_WINDOW_HPP_
#define _AVPLAY_WINDOW_HPP_

#include <string>

#include <AvTranscoder/reader/VideoReader.hpp>

class Window
{
public:
	Window( avtranscoder::VideoReader& reader );

	void launch();

private:

	static void display();
	static void keyboard( unsigned char k, int x, int y );
	static void specialKeyboard( int k, int x, int y );
	static void mouse( int button, int state, int x, int y );
	static void motion( int x, int y );
	static void reshape( int width, int height );

	static void displayHelp();
	static void displayInformations();
	static void move( float x, float y );
	static void zoom(float factor);
	static void mapToImage(int x, int y, int &iX, int &iY);

	static void setTransfert( float red, float green, float blue, float alpha=1.f );

	static void displayChannelTexture( bool& channel, const float red, const float green, const float blue );

	static void showRedChannelTexture();
	static void showGreenChannelTexture();
	static void showBlueChannelTexture();
	static void showAlphaChannelTexture();

	static void displayNextFrame();
	static void displayPrevFrame();
	static void displayFirstFrame();
	static void displayAtFrame( const size_t frame );

	static void loopPlaying( int value );

	static avtranscoder::VideoReader* _reader;

	static size_t _width;
	static size_t _height;

	static int _x1;
	static int _x2;
	static int _y1;
	static int _y2;

	static int _xMinViewport;
	static int _yMinViewport;

	static int _xMouseRef;
	static int _yMouseRef;

	static int _windowId;

	// viewing properties - zoom
	static float _currentZoom;
	static float _factorZoom;

	static float _scale;

	static bool _play;
	
	static bool _flip;
	static bool _flop;

	static bool _showRedChannel;
	static bool _showGreenChannel;
	static bool _showBlueChannel;
	static bool _showAlphaChannel;
};

#endif