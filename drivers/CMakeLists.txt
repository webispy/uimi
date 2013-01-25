SET(DRIVERS csv dictdz)

FOREACH(drv ${DRIVERS})
	ADD_LIBRARY(${drv}-driver SHARED ${drv}.c)
	TARGET_LINK_LIBRARIES(${drv}-driver ${pkgs_LDFLAGS})
	SET_TARGET_PROPERTIES(${drv}-driver PROPERTIES PREFIX "" OUTPUT_NAME ${drv}-driver)

	INSTALL(TARGETS ${drv}-driver LIBRARY DESTINATION lib/uimi/drivers)
ENDFOREACH(drv)


