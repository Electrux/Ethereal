project( "Ethereal-Lang" ) {
	version( 0.0.1 )
	language( cpp, 11 )
	license( bsd3 )
	author( Electrux, "ElectruxRedsworth@gmail.com" )
}

USE_SELF_PREFIX = "false"
if( "${PREFIX}" == "" ) {
	PREFIX = "%{pwd}"
	USE_SELF_PREFIX = "true"
}
# TODO: fix the rpaths with respect to libet and the standard library
builds.add_flags( "-fPIC", "-Wall", "-Wextra", "-Wno-unused-parameter",
		  "-DVERSION_MAIN=0", "-DVERSION_SUB=0", "-DVERSION_PATCH=2",
		  "-DBUILD_PREFIX_DIR=${PREFIX}", "-Wl,-rpath,${PREFIX}/lib/ethereal/" )

if( "${CC}" != "g++" ) {
	builds.add_flags( "-Wno-c99-extensions", "-Wno-unused-command-line-argument" )
} elif( "${OS}" == "OS_ANDROID" ) {
	builds.add_flags( "-Wno-unused-command-line-argument" )
}

builds.add_lib_dirs( "-L./buildfiles/" )

if( "${OS}" == "OS_BSD" ) {
	builds.add_inc_dirs( "-I/usr/local/include" )
	builds.add_lib_dirs( "-I/usr/local/lib" )
}

use_lib( dl )
use_lib( gmpxx )

# handle remove buildfiles if DEBUG_MODE is changed
if( "${ARGC}" > 0 && "${ARG_0}" == debug || "${ARG_0}" == memlog ) {
	if( "#{stat buildfiles 2>/dev/null}" == 0 && "#{stat buildfiles/.debug_mode 2>/dev/null}" != 0 ) {
		eval( "#{rm -rf buildfiles; mkdir buildfiles; touch buildfiles/.debug_mode}" )
	}
	if( "${ARG_0}" == debug ) { builds.add_flags( "-DDEBUG_MODE", "-DMEM_LOGS" ) }
	if( "${ARG_0}" == memlog ) { builds.add_flags( "-DMEM_LOGS" ) }
} else {
	if( "#{stat buildfiles 2>/dev/null}" == 0 && "#{stat buildfiles/.debug_mode 2>/dev/null}" == 0 ) {
		eval( "#{rm -rf buildfiles}" )
	}
	builds.add_flags( "-march=native", "-O2", "-flto" )
}

builds( lib, dynamic ) {
	builds.add_flags( "-DAS_LIB" )
	sources( "src/(.*)\.cpp", "-src/VM/Main.cpp", "-src/FE/Main.cpp" )
	build( et, "src/VM/Main.cpp" )
}

builds( bin ) {
	sources( "src/(.*)\.cpp", "-src/VM/Main.cpp" )
	build( et, "src/FE/Main.cpp" )
}

builds( lib, dynamic ) {
	builds.add_lib_flags( "-let" )
	build( core,  "modules/std/core.cpp" )
	build( fs,    "modules/std/fs.cpp" )
	build( map,   "modules/std/map.cpp" )
	build( math,  "modules/std/math.cpp" )
	build( opt,   "modules/std/opt.cpp" )
	build( os,    "modules/std/os.cpp" )
	build( set,   "modules/std/set.cpp" )
	build( str,   "modules/std/str.cpp" )
	build( term,  "modules/std/term.cpp" )
	build( time,  "modules/std/time.cpp" )
	build( tuple, "modules/std/tuple.cpp" )
	build( vec,   "modules/std/vec.cpp" )
}

if( "${ARGC}" > 0 && "${ARG_0}" == "install" || "${USE_SELF_PREFIX}" == "true" ) {
	if( "${IS_ROOT}" == "true" || "${OS}" == OS_OSX ) {
		install( "buildfiles/et", "${PREFIX}/bin/" )
		install( "buildfiles/libet.so", "${PREFIX}/lib/ethereal/" )
		install( "buildfiles/lib*.so", "${PREFIX}/lib/ethereal/std/" )
		if( "%{pwd}" != "${PREFIX}" ) {
			install( "include/*", "${PREFIX}/include/" )
		}
		eval( "#{rm -f ${PREFIX}/lib/ethereal/std/libet.so}" )
	} else {
		print( "{r}Run as root to install the built files{0}\n" )
	}
}