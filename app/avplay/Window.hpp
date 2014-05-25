#ifndef _AVPLAY_WINDOW_HPP_
#define _AVPLAY_WINDOW_HPP_

#include <string>

#include "Reader.hpp"

class Window
{
public:
	Window( Reader& reader);

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

	static Reader* m_reader;

	static size_t m_width;
	static size_t m_height;

	static int m_x1;
	static int m_x2;
	static int m_y1;
	static int m_y2;

	static int m_xMinViewport;
	static int m_yMinViewport;

	static int m_xMouseRef;
	static int m_yMouseRef;

	static int m_windowId;

	// viewing properties - zoom
	static float m_currentZoom;
	static float m_factorZoom;

	static float m_scale;

	static bool m_play;
	
	static bool m_flip;
	static bool m_flop;

	static bool m_showRedChannel;
	static bool m_showGreenChannel;
	static bool m_showBlueChannel;
	static bool m_showAlphaChannel;
};

#endif