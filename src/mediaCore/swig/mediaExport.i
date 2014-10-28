// Need to modify the visibility of the constructor and getCPtr method
// if you are invoking SWIG more than once and generating the wrapped classes
// into different packages in each invocation.
%typemap(javabody) SWIGTYPE %{
	protected long swigCPtr;
	protected boolean swigCMemOwn;

	public $javaclassname(long cPtr, boolean cMemoryOwn) {
		swigCMemOwn = cMemoryOwn;
		swigCPtr = cPtr;
	}

	public static long getCPtr($javaclassname obj) {
		return (obj == null) ? 0 : obj.swigCPtr;
	}
%}

%typemap(javabody_derived) SWIGTYPE %{
	public $javaclassname(long cPtr, boolean cMemoryOwn) {
		super( cPtr, cMemoryOwn );
	}

	public static long getCPtr($javaclassname obj) {
		return (obj == null) ? 0 : obj.swigCPtr;
	}
%}
