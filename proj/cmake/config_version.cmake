function(parseBranchVersion branchName MAJOR MINOR PATCH)
	set(tempName ${branchName})
	string(REPLACE "/" ";" templist "${tempName}")
	list(LENGTH templist len)
	if(${len} LESS 2)
		message(STATUS branchName:${branchName} " branch name does not meet this requirements")
		return()
	endif()
	list(GET templist 1 branchVersion)
	string(FIND ${branchVersion} "_" splitIndex)
	if(${splitIndex} GREATER_EQUAL 0)
		string(SUBSTRING ${branchVersion} 0 ${splitIndex} branchVersion)
	endif()
	string(REPLACE "." ";" VERSION_LIST "${branchVersion}")
	list(LENGTH VERSION_LIST len)
	if(${len} LESS 3)
		message(STATUS branchName:${branchName} "branch name does not meet this requirements")
		return()
	endif()
	list(GET VERSION_LIST 0 TEMP_MAJOR)
	list(GET VERSION_LIST 1 TEMP_MINOR)
	list(GET VERSION_LIST 2 TEMP_PATCH)
	set(${MAJOR} ${TEMP_MAJOR} PARENT_SCOPE)
	set(${MINOR} ${TEMP_MINOR} PARENT_SCOPE)
	set(${PATCH} ${TEMP_PATCH} PARENT_SCOPE)
endfunction()

if(${CMAKE_SYSTEM_NAME} STREQUAL "Windows")
	#version
	set(MAJOR_VERSION 1)
	set(MINOR_VERSION 0)
	set(PATCH_VERSION 0)
	set(CUSTOM_BUILD_VERSION 0)
	set(CUSTOM_BUILD_NUMBER 0)
	if(DEFINED BUILD_NUMBER)
		set(CUSTOM_BUILD_NUMBER ${BUILD_NUMBER})
	endif()
	find_package(Git)
	if(GIT_FOUND)
		execute_process(
			COMMAND ${GIT_EXECUTABLE} rev-parse --short HEAD
			WORKING_DIRECTORY "${CMAKE_CURRENT_SOURCE_DIR}"
			OUTPUT_VARIABLE CUSTOM_BUILD_VERSION
			ERROR_QUIET
			OUTPUT_STRIP_TRAILING_WHITESPACE
		)
		execute_process(
			COMMAND ${GIT_EXECUTABLE} symbolic-ref -q --short HEAD
			WORKING_DIRECTORY "${CMAKE_CURRENT_SOURCE_DIR}"
			OUTPUT_VARIABLE PKG_BRANCH
			ERROR_QUIET
			OUTPUT_STRIP_TRAILING_WHITESPACE
		)
		message(STATUS "PKG_BRANCH:${PKG_BRANCH} CUSTOM_BUILD_VERSION:${CUSTOM_BUILD_VERSION}")
		set(PKG_BRANCH "release/12.1.0")
		if(NOT ${PKG_BRANCH} STREQUAL "")
			parseBranchVersion(${PKG_BRANCH} MAJOR_VERSION MINOR_VERSION PATCH_VERSION)
		endif()
		message(STATUS "Parse version from branch:" ${MAJOR_VERSION} ${MINOR_VERSION} ${PATCH_VERSION})
	endif()
	
	# other info
	string(TIMESTAMP CURRENT_YEAR "%Y")
	set(COPYRIGHT_INFO "Copyright (C) ${CURRENT_YEAR} LJP")
	
	set(WITHICO_RC_TEMPLETE_PATH ${CMAKE_CURRENT_SOURCE_DIR}/windows/Version/cmake_version_withico.rc.in)
	set(NOICO_RC_TEMPLETE_PATH ${CMAKE_CURRENT_SOURCE_DIR}/windows/Version/cmake_version.rc.in)
endif()