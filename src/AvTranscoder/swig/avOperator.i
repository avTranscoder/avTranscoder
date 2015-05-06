#if SWIGPYTHON
    %rename(__assign__) *::operator=;
#endif

#if SWIGJAVA
    %rename(clone) *::operator=;
#endif
