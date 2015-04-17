%{
#include <AvTranscoder/Option.hpp>
%}

namespace std {
// Allow vector of object with no default constructor
%ignore vector< avtranscoder::Option >::vector(size_type); 
%ignore vector< avtranscoder::Option >::resize;

// Create instantiations of a template classes
%template(OptionArray) vector< avtranscoder::Option >;
%template(IntPair) pair< size_t, size_t >;
}

%include <AvTranscoder/Option.hpp>
