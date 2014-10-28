%{
 #include <mediaCore/common.hpp>
 #include <mediaCore/progress/IProgress.hpp>
 #include <mediaCore/progress/ConsoleProgress.hpp>
 #include <mediaCore/progress/NoDisplayProgress.hpp>
%}

/* turn on director wrapping for IProgress */
%feature("director") IProgress;

%include <mediaCore/common.hpp>
%include <mediaCore/progress/IProgress.hpp>
%include <mediaCore/progress/ConsoleProgress.hpp>
%include <mediaCore/progress/NoDisplayProgress.hpp>
