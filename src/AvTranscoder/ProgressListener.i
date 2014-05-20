%module(directors="1") AvTranscoder


%{
 #include <AvTranscoder/ProgressListener.hpp>
%}

/* turn on director wrapping ProgressListener */
%feature("director") ProgressListener;

%include <AvTranscoder/ProgressListener.hpp>