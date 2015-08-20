What is this?
-------------
Peon is a tool to manage the data pipeline of a game. Some features:

- Only build files that have changed or are missing.
- Simple JSON configuration of builders.
- Can watch directory for changes.

This tool is a variant of the build pipeline used by Ironclad Games and was used in two shipped games.

How does peon work?
-----------
- peon uses the file extension to figure out how to build the file.

How build exe?
-----------
- git clone https://github.com/jseward/peon.git
- git clone https://github.com/jseward/solar.git (peon uses the solar engine for doing io and other features)
- both projects should be side-by-side in same parent directory or else you will have to resolve the paths manually yourself so peon can find the solar code.
- build peon_win32 project in peon/msvc/peon_win32

How to use exe?
----------
- peon_win32.exe requires two .json files to run
	- settings.json
	- builders.json

ex. settings.json

	{
		"src_root" : "../../data",
		"dst_root" : "../../bin/mygame"
	}
	
ex. builders.json

	[
	  {
	    "type": "EXEC",
	    "src_extension": ".fx",
	    "dst_folder": "shaders",
	    "dst_extension": ".fxo",
	    "exe_path": "./.exe/fxc/fxc.exe",
	    "exe_arguments_format": "/nologo /Tfx_2_0 /O3 /Gpp /Fo{1} {0}"
	  },
	  {
	    "type": "DEPENDENCY_FILE",
	    "src_extension": ".fxh",
	    "dependent_extensions": [
	      ".fx",
	      ".fxh"
	    ]
	  },
	  {
	    "type": "COPY",
	    "src_extension": ".font",
	    "dst_folder": "fonts"
	  }
	]
	
builders
===
EXEC
----
execute a command line app to build the file

DEPENDENCY_FILE
----
not actually built but will cause other files to be built if changed (ex. .fxh - header files for d3d9 shaders)

COPY
----
simply copy the file

todo
----
- build folder support (ex. zip up a group of files)


