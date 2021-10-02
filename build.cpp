#include <string>
#include <vector>

#include <iostream>
#include <sstream>
#include <fstream>

using namespace std;

/* CONFIG BEGIN */
string default_compiler 	= "clang"; // can be changed to gcc
string cppversion 			= "-std=c++14";

vector< string > default_flags = {
	"-lstdc++",
    "-fno-signed-zeros",
    "-mrecip=all",
    "-ffp-contract=fast",
    "-fassociative-math",
    "-ffinite-math-only",
    "-funsafe-math-optimizations",
    "-fno-trapping-math",
#if defined( __clang__ )
    "-fdenormal-fp-math=positive-zero",
    "-ffp-exception-behavior=ignore",
#endif
};

vector< string > clang_flags = {
    "-m64",
    "-fno-unwind-tables",
    "-fno-exceptions",
    "-fno-rtti",
    "-ftls-model=local-exec",
    ""
};

vector< string > debug_flags = {
    "-O2",
    "-g3",
    "-Wall",
    "-Wextra",
    "-Werror",
    "-Wpedantic",
#if !defined(_WIN32)
    "-fsanitize=thread",
#endif
};

vector< string > opt_flags = {
    "-Os",
    "-ffast-math",
    "-finline-functions",
    "-funroll-loops",
    "-ftree-vectorize",
    "-ftree-slp-vectorize",
#if defined( __x86_64__ ) // x86 vector instruction flags
     "-mavx",
     "-msse4.2",
#endif
#if !defined( _WIN32 ) // flto didn't work on windows last time I tried.
    "-flto=full",
#endif
};

/* CONFIG END */

struct compile_settings {
	string 	source_file = "";
	string 	name 		= "";
	bool 	use_web		= false;
	bool 	optimized	= false;
};

string run_cmd( const string& cmd ) {
	const string tmp_file_name = "tmp.txt";

	if ( system( ( cmd + " > " + tmp_file_name ).c_str() ) == 0 ) {
		stringstream ss;
		ss << ifstream( tmp_file_name ).rdbuf();
		system( ( string( "rm " ) + tmp_file_name ).c_str() );
		return ss.str();
	} else {
		return "cmd (" + cmd + ") failed.";
	}
}

string include_sdl2() {
#if defined( _WIN32 )
	return "-L.\\lib\\sdl2win64 -I.\\include\\ -lSDL2main -lSDL2";
#else
	return run_cmd( "sdl2-config --cflags --static-libs" );
#endif
}

void append_flags( string* s, const vector< string >& flags ) {
	for ( const string& flag : flags ) { *s += flag + ' '; }
}

compile_settings parse_args( int argc, const char* argv[] ) {
	compile_settings output{};
	
	const vector< string > extensions = { ".cpp", ".cxx", ".cc" };

	for ( int i = 1; i < argc; ++i ) {
		
		string s = argv[ i ];
		for ( char& c : s ) c = tolower( c );
		
		bool found_name = false;
		
		for ( const string& ext : extensions ) {
			size_t e = s.find( ext );
			if ( e != string::npos ) {
				output.source_file 	= s;
				output.name 		= s.substr( 0, e );
				found_name 			= true;
			}
		}

		if ( !found_name ) {
			if ( s.find( "web" ) != string::npos ) { output.use_web 	= true; }
			if ( s.find( "opt" ) != string::npos ) { output.optimized = true; }
		}
	}
	return output;
}

int main( int argc, const char* argv[] ) {
	compile_settings settings = parse_args( argc, argv );

	string cmd = ( settings.use_web ? string( "emcc" ) : default_compiler ) + ' ' + settings.source_file + ' ';

	cmd += ' ' + include_sdl2() + ' ';

	append_flags( &cmd, default_flags );

	if ( !settings.use_web ) { 
		append_flags( &cmd, clang_flags );
	}

	if ( !settings.optimized ) {
		append_flags( &cmd, debug_flags );
	} else if ( !settings.use_web ) {
		append_flags( &cmd, opt_flags );
	}

	string exe_name = settings.name;
	if ( settings.use_web ) { exe_name += ".html"; }
#if defined( _WIN32 )
	else { exe_name += ".exe"; }
#endif

	cmd += "-o " + exe_name + " && ";
#if !defined( _WIN32 )
	cmd += "./";
#endif
	cmd += exe_name;

	cout << cmd << endl; 
	return system( cmd.c_str() );
}
