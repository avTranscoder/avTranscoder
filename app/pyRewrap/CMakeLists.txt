### python/pyRewrap
set(CMAKE_MODULE_PATH ${PROJECT_SOURCE_DIR}/cmake)
include(AvTranscoderMacros)

# Install app
install(
	FILES "pyrewrap.py"
	PERMISSIONS OWNER_EXECUTE OWNER_WRITE OWNER_READ GROUP_EXECUTE GROUP_READ WORLD_READ WORLD_EXECUTE
	DESTINATION "bin/python"
)

if(UNIX)
	install( CODE "EXECUTE_PROCESS(COMMAND ln -sf python/pyrewrap.py ${CMAKE_INSTALL_PREFIX}/bin/pyrewrap-${AVTRANSCODER_VERSION})" )
	install( CODE "EXECUTE_PROCESS(COMMAND ln -sf pyrewrap-${AVTRANSCODER_VERSION} ${CMAKE_INSTALL_PREFIX}/bin/pyrewrap)" )
endif(UNIX)
