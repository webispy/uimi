CMAKE_MINIMUM_REQUIRED(VERSION 2.6)
PROJECT(uimi C)

#SET(PREFIX ${CMAKE_INSTALL_PREFIX})

INCLUDE(FindPkgConfig)
pkg_check_modules(pkgs REQUIRED glib-2.0 gobject-2.0 gio-2.0 gio-unix-2.0)

FOREACH(flag ${pkgs_CFLAGS})
	SET(EXTRA_CFLAGS "${EXTRA_CFLAGS} ${flag}")
ENDFOREACH(flag)

INCLUDE_DIRECTORIES(
	${CMAKE_SOURCE_DIR}/include
	${CMAKE_BINARY_DIR})

SET(CMAKE_C_FLAGS "${CMAKE_C_FLAGS} ${EXTRA_CFLAGS} -Werror -Wextra -Wno-unused-parameter -Wno-missing-field-initializers -Wdeclaration-after-statement -Wmissing-declarations -Wcast-align -g")

SET(SRCS
	src/util.c
	src/uimi_dbus.c
	src/uimi_driver.c
	src/uimi_config.c
	src/uimi_log.c
	src/main.c)

ADD_CUSTOM_COMMAND(
	OUTPUT ${CMAKE_BINARY_DIR}/generated-code.c
	COMMAND gdbus-codegen --interface-prefix org.uimi. --generate-c-code generated-code --c-namespace Uimi --c-generate-object-manager --generate-docbook generated-docs ${CMAKE_SOURCE_DIR}/introspection/uimi.xml
	COMMENT "Generating GDBus .c/.h")

ADD_EXECUTABLE(uimi ${SRCS} ${CMAKE_BINARY_DIR}/generated-code.c)
TARGET_LINK_LIBRARIES(uimi ${pkgs_LDFLAGS})

INSTALL(TARGETS uimi RUNTIME DESTINATION bin)
INSTALL(FILES ${CMAKE_SOURCE_DIR}/res/uimi.conf DESTINATION ${PREFIX}/etc)
INSTALL(FILES ${CMAKE_SOURCE_DIR}/res/test.csv DESTINATION /usr/share/uimi)
INSTALL(FILES ${CMAKE_SOURCE_DIR}/res/test2.csv DESTINATION /usr/share/uimi)

ADD_SUBDIRECTORY(drivers)
