%{
#include <AvTranscoder/Library.hpp>
%}

namespace std {
// Allow vector of object with no default constructor
%ignore vector< avtranscoder::Library >::vector(size_type); 
%ignore vector< avtranscoder::Library >::resize;

// Create instantiations of a template classes
%template(Libraries) vector< avtranscoder::Library >;
}

%include <AvTranscoder/Library.hpp>
