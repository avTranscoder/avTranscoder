### python/pyThumbnail
set(CMAKE_MODULE_PATH ${PROJECT_SOURCE_DIR}/cmake)
include(AvTranscoderMacros)

# Install app
install(
	FILES "pythumbnail.py"
	PERMISSIONS OWNER_EXECUTE OWNER_WRITE OWNER_READ GROUP_EXECUTE GROUP_READ WORLD_READ WORLD_EXECUTE
	DESTINATION "bin/python"
)

if(UNIX)
	install( CODE "EXECUTE_PROCESS(COMMAND ln -sf python/pythumbnail.py ${CMAKE_INSTALL_PREFIX}/bin/pythumbnail-${AVTRANSCODER_VERSION})" )
	install( CODE "EXECUTE_PROCESS(COMMAND ln -sf pythumbnail-${AVTRANSCODER_VERSION} ${CMAKE_INSTALL_PREFIX}/bin/pythumbnail)" )
endif(UNIX)
