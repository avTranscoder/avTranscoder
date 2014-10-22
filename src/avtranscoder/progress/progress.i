%{
 #include <avtranscoder/common.hpp>
 #include <avtranscoder/progress/IProgress.hpp>
 #include <avtranscoder/progress/ConsoleProgress.hpp>
 #include <avtranscoder/progress/NoDisplayProgress.hpp>
%}

/* turn on director wrapping for IProgress */
%feature("director") IProgress;

%include <avtranscoder/common.hpp>
%include <avtranscoder/progress/IProgress.hpp>
%include <avtranscoder/progress/ConsoleProgress.hpp>
%include <avtranscoder/progress/NoDisplayProgress.hpp>
