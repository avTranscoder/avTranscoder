import org.avtranscoder.NoDisplayProgress;
import org.avtranscoder.InputFile;
import org.avtranscoder.avtranscoder;


public class jFileAnalysis {
	public static void main( String[] args ){
		System.loadLibrary("avtranscoder-java");

		System.out.println( "Start input file analyse");

		avtranscoder.preloadCodecsAndFormats();

		InputFile inputFile = new InputFile( args[0] );
		NoDisplayProgress progress = new NoDisplayProgress();
		inputFile.analyse( progress );

		// Access the properties you want with
		// inputFile.getProperties()

		System.out.println( "End input file analyse");
	}
}

// How to use

// Compile on UNIX: javac -cp "/path/to/jar/avtranscoder-<version>.jar:." jFileAnalysis.java
// Run on UNIX: java -Djava.library.path=/path/to/lib/ -cp "/path/to/jar/avtranscoder-<version>.jar:." jFileAnalysis <input_image>

// Compile on Windows: javac -cp "/path/to/jar/avtranscoder-<version>.jar;." jFileAnalysis.java
// Run on Windows: java -Djava.library.path=/path/to/lib/ -cp "/path/to/jar/avtranscoder-<version>.jar;." jFileAnalysis <input_image>

