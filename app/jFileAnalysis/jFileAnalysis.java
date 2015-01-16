import org.avtranscoder.NoDisplayProgress;
import org.avtranscoder.InputFile;


public class jFileAnalysis {
	public static void main( String[] args ){
		System.loadLibrary("avutil");
		System.loadLibrary("swscale");
		System.loadLibrary("swresample");
		System.loadLibrary("avcodec");
		System.loadLibrary("avformat");
		System.loadLibrary("avtranscoder");
		System.loadLibrary("avtranscoder-java");

		System.out.println( "Start input file analyse");

		InputFile inputFile = new InputFile( args[0] );
		NoDisplayProgress progress = new NoDisplayProgress();
		inputFile.analyse( progress );

		System.out.println( "End input file analyse");
        }
}

// How to use

// Compile on UNIX: javac -cp "/path/to/jar/avtranscoder-<version>.jar:." jFileAnalysis.java
// Run on UNIX: java -Djava.library.path=/path/to/lib/ -cp "/path/to/jar/avtranscoder-<version>.jar:." jFileAnalysis <input_image>

// Compile on Windows: javac -cp "/path/to/jar/avtranscoder-<version>.jar;." jFileAnalysis.java
// Run on Windows: java -Djava.library.path=/path/to/lib/ -cp "/path/to/jar/avtranscoder-<version>.jar;." jFileAnalysis <input_image>

