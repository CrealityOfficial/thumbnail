__assert_target(png)
__assert_target(zlib)

if(NOT GLM_INCLUDE_DIRS)
	message(FATAL_ERROR "GLM_INCLUDE_DIRS Missing")
endif()

set(LIBS zlib png cxbin trimesh2)
set(INCS ${GLM_INCLUDE_DIRS})
set(DEFS GLM_FORCE_LEFT_HANDED THUMBNAIL_DLL)

__add_real_target(thumbnail dll SOURCE
									thumbnail/stl/helpers.h
									thumbnail/stl/parser.h
									thumbnail/stl/parser.cpp
									thumbnail/picture.cpp
									thumbnail/picture.h
									thumbnail/aabb.cpp
									thumbnail/aabb.h
									thumbnail/helper.cpp
									thumbnail/helper.h
									thumbnail/vec3.h
									thumbnail/vec4.h
									thumbnail/triangle.h
									thumbnail/raster_backend/raster.cpp
									thumbnail/raster_backend/raster.h
									thumbnail/raster_backend/rasterconfig.cpp
									thumbnail/raster_backend/rasterconfig.h
									thumbnail/raster_backend/zbuffer.cpp
									thumbnail/raster_backend/zbuffer.h
									thumbnail/topomesh.h
									thumbnail/topomesh.cpp
									thumbnail/stl_thumbnail.h
									thumbnail/stl_thumbnail.cpp
									thumbnail/thumbnail.h
									thumbnail/thumbnail.cpp
								LIB ${LIBS}
								INC ${INCS}
								DEF ${DEFS}
								INTERFACE ${CMAKE_CURRENT_SOURCE_DIR}
								)

if(INSTALL_THUMBNAIL)
	INSTALL(TARGETS thumbnail
					LIBRARY DESTINATION lib
					ARCHIVE DESTINATION .
					RUNTIME DESTINATION bin)
			
	INSTALL(FILES ${CMAKE_CURRENT_SOURCE_DIR}/thumbnail/stl_thumbnail.h
				${CMAKE_CURRENT_SOURCE_DIR}/thumbnail/exporter.h
				${CMAKE_CURRENT_SOURCE_DIR}/thumbnail/vec3.h 
				${CMAKE_CURRENT_SOURCE_DIR}/thumbnail/vec4.h 				
				DESTINATION ${CMAKE_INSTALL_PREFIX}/include/thumbnail/)
endif()

if(CC_INSTALL_THUMBNAIL)
	__install_directory_specif(${CMAKE_CURRENT_SOURCE_DIR} thumbnail)
endif()
