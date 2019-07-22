project( "Ethereal-Lang" ) {
	version( 0.1 )
	language( cpp, 11 )
	license( bsd3 )
	author( Electrux, "ElectruxRedsworth@gmail.com" )
}

USE_SELF_PREFIX = "false"
if( "${PREFIX}" == "" ) {
	PREFIX = "%{pwd}"
	USE_SELF_PREFIX = "true"
}

builds.add_flags( "-fPIC", "-Wall", "-Wextra", "-Wno-unused-parameter",
		  "-DBUILD_PREFIX_DIR=${PREFIX}", "-Wl,-rpath,${PREFIX}/lib/ethereal/" )
if( "${CC}" != "g++" ) {
	builds.add_flags( "-Wno-c99-extensions", "-Wno-unused-command-line-argument" )
}
builds.add_lib_dirs( "-L./buildfiles/" )

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
	sources( "src/(.*)\.cpp", "-src/VM/Main.cpp", "-src/VM/Modules/(.*)\.cpp" )
	if( "${OS}" == OS_OSX ) {
		builds.add_flags( "-Wl,-install_name -Wl,@rpath/libet.so" )
	}
	build( et, "src/VM/Main.cpp" )
}

builds( bin ) {
	sources( "src/(.*)\.cpp", "-src/VM/Modules/(.*)\.cpp", "-src/VM/Main.cpp" )
	build( et, "src/FE/Main.cpp" )
}

builds( lib, dynamic ) {
	builds.add_lib_flags( "-let" )
	if( "${OS}" == OS_OSX ) {
		builds.add_flags( "-Wl,-install_name -Wl,@rpath/libcore.so" )
	}
	build( core, "src/VM/Modules/core.cpp" )
	if( "${OS}" == OS_OSX ) {
		builds.add_flags( "-Wl,-install_name -Wl,@rpath/libstr.so" )
	}
	build( str, "src/VM/Modules/str.cpp" )
}

if( "${ARGC}" > 0 && "${ARG_0}" == "install" || "${USE_SELF_PREFIX}" == "true" ) {
	if( "${IS_ROOT}" == "true" || "${OS}" == OS_OSX ) {
		install( "buildfiles/et", "${PREFIX}/bin/" )
		install( "buildfiles/lib*.so", "${PREFIX}/lib/ethereal/" )
	} else {
		print( "{r}Run as root to install the built files{0}\n" )
	}
}