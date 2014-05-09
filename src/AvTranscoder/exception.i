%include <std_except.i>

/*
All swig exception types:
SWIG_MemoryError
SWIG_IndexError
SWIG_RuntimeError
SWIG_IOError
SWIG_TypeError
SWIG_OverflowError
SWIG_ValueError

SWIG_DivisionByZero
SWIG_SyntaxError
SWIG_SystemError
*/

%exception {
try
{
	$action
}
catch( std::out_of_range& e )
{
	SWIG_exception( SWIG_IndexError, e.what() );
}
catch( std::bad_alloc& e )
{
	SWIG_exception( SWIG_MemoryError, e.what() );
}
catch( std::ios_base::failure& e )
{
	SWIG_exception( SWIG_IOError, e.what() );
}/*
catch( std::bad_cast& e )
{
	SWIG_exception( SWIG_TypeError, e.what() );
}*/
catch( std::overflow_error& e )
{
	SWIG_exception( SWIG_OverflowError, e.what() );
}
catch( std::underflow_error& e )
{
	SWIG_exception( SWIG_OverflowError, e.what() );
}
catch( std::invalid_argument& e )
{
	SWIG_exception( SWIG_ValueError, e.what() );
}
catch( std::exception& e )
{
	SWIG_exception( SWIG_RuntimeError, e.what() );
}
catch(...)
{
	SWIG_exception( SWIG_RuntimeError, "Unknown C++ exception" );
}

}