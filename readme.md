What is this?
-------------
Peon is a tool to manage the data pipeline of a game. Some features:

- Only build files that have changed or are missing.
- Simple JSON configuration of builders.
- Can watch directory for changes.

This tool is based on the build pipeline used by Ironclad Games.

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
	    "type": "EXEC",
	    "src_extension": ".tga",
		"src_has_peon_customization": true,
	    "dst_folder": "textures",
	    "dst_extension": ".dds",
	    "exe_path": "./.exe/nvdxt/nvdxt.exe",
	    "exe_arguments_format": "-file {0} -output {1} {2}"
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

If a side-by-side file with the same name, but with the extension .peon is found then it will be inserted into exe_arguments_format. This allows for per-file customization of how the file is built. (ex. .dds compression settings).

DEPENDENCY_FILE
----
not actually built but will cause other files to be built if changed (ex. .fxh - header files for d3d9 shaders)

COPY
----
simply copy the file

todo
----
- build folder support (ex. zip up a group of files)


