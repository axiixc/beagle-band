{
	"targets": [{
		"target_name": "motion_io",
		"cflags": [ "-Wno-unused-but-set-variable" ],
		"cxxflags": [ "-fpermissive" ],
		"sources": [
			"MotionIO.cpp",
			"Accelerometers/ADXL345.cpp",
			"Accelerometers/i2c/gpio-utils.cpp" ]
	}]
}
