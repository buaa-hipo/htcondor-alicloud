 ###############################################################
 # 
 # Copyright 2011 Red Hat, Inc. 
 # 
 # Licensed under the Apache License, Version 2.0 (the "License"); you 
 # may not use this file except in compliance with the License.  You may 
 # obtain a copy of the License at 
 # 
 #    http://www.apache.org/licenses/LICENSE-2.0 
 # 
 # Unless required by applicable law or agreed to in writing, software 
 # distributed under the License is distributed on an "AS IS" BASIS, 
 # WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
 # See the License for the specific language governing permissions and 
 # limitations under the License. 
 # 
 ############################################################### 
 
MACRO (CONDOR_POST_EXTERNAL _TARGET _INC_DIR _LIB_DIR)

	string( TOUPPER "${_TARGET}" _UP_TARGET )

	if (WINDOWS)
		set_property( TARGET ${_TARGET} PROPERTY FOLDER "externals" )
	endif()	
		
	# if a local built copy exists disable from all build 
	if ( ${_UP_TARGET}_PREBUILT )  
		set_target_properties( ${_TARGET} PROPERTIES EXCLUDE_FROM_ALL TRUE)
		message (STATUS "--> (${_TARGET}): excluded from all target, cached version exists") 
	else()
		append_var(CONDOR_EXTERNALS ${_TARGET})
		append_var(${_UP_TARGET}_REF ${_TARGET})
	endif()
	
	set( ${_UP_TARGET}_FOUND ${${_UP_TARGET}_FOUND} PARENT_SCOPE )
	set( HAVE_EXT_${_UP_TARGET} ON PARENT_SCOPE )

	# used by --with packages
	set( ${_UP_TARGET}_INSTALL_LOC ${${_UP_TARGET}_INSTALL_LOC} PARENT_SCOPE)

	set(${_TARGET}INCLUDE ${_INC_DIR})
	if(${_TARGET}INCLUDE)
		append_var(CONDOR_EXTERNAL_INCLUDE_DIRS ${${_UP_TARGET}_INSTALL_LOC}/${_INC_DIR})
		append_var(${_UP_TARGET}_INCLUDE ${${_UP_TARGET}_INSTALL_LOC}/${_INC_DIR})
	endif()
	
	set(${_TARGET}LINK ${_LIB_DIR})
	if(${_TARGET}LINK)
		append_var(CONDOR_EXTERNAL_LINK_DIRS ${${_UP_TARGET}_INSTALL_LOC}/${_LIB_DIR})
		append_var(${_UP_TARGET}_LD ${${_UP_TARGET}_INSTALL_LOC}/${_LIB_DIR})
	endif()

ENDMACRO(CONDOR_POST_EXTERNAL)
