### python/pyConcat

# Install app
install(
	FILES "pyconcat.py"
	PERMISSIONS OWNER_EXECUTE OWNER_WRITE OWNER_READ GROUP_EXECUTE GROUP_READ WORLD_READ WORLD_EXECUTE
	DESTINATION "bin/python"
)

if(UNIX)
	install( CODE "EXECUTE_PROCESS(COMMAND ln -sf python/pyconcat.py ${CMAKE_INSTALL_PREFIX}/bin/pyconcat)" )
endif(UNIX)
