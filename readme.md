builders
	type: "EXEC"
	exec_exe: "fx.exe"
	exec_argument_format: "/Tfx_2_0 /O3 /Gpp /Fo{0} {1}"
	src_extentions : ".fx"
	src [
		"shaders/*"
	]	
	dst : "shaders"
	
