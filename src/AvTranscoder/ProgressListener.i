%module(directors="1") AvTranscoder


%{
 #include <AvTranscoder/ProgressListener.hpp>
%}

/* turn on director wrapping for IProgress */
%feature("director") IProgress;

%include <AvTranscoder/ProgressListener.hpp>
