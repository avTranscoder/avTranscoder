#ifndef _AV_TRANSCODER_CONTEXT_HPP_
#define _AV_TRANSCODER_CONTEXT_HPP_

#include <string>

namespace avtranscoder
{

/**
 * @brief Wrapper of AVContext.
 * Can access Options through the corresponding context.
 */
class Context
{
public:
	Context( void* objContext )
		: _objContext( objContext )
	{}

	void set( const std::string& key, const std::string& flag, const bool enable );
	void set( const std::string& key, const bool value );
	void set( const std::string& key, const int value );
	void set( const std::string& key, const int num, const int den );
	void set( const std::string& key, const double value );
	void set( const std::string& key, const std::string& value );
	
private:
	void* _objContext;
};

}

#endif
