__cc_find(Png)
__cc_find(CXBin)
__cc_find(Trimesh2)
__cc_find(Zlib)
__cc_find(Glm)
__cc_find(MMesh)


__assert_target(png)
__assert_target(zlib)

if(NOT GLM_INCLUDE_DIRS)
	message(FATAL_ERROR "GLM_INCLUDE_DIRS Missing")
endif()

set(LIBS png cxbin trimesh2 mmesh zlib)
set(INCS ${GLM_INCLUDE_DIRS})
set(DEFS GLM_FORCE_LEFT_HANDED)

set(SRCS   thumbnail/stl/helpers.h
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
									thumbnail/img2gcode.cpp
									thumbnail/img2gcode.h
								LIB ${LIBS}
								INC ${INCS}
								DEF ${DEFS}
								INTERFACE ${CMAKE_CURRENT_SOURCE_DIR}
								)

__add_common_library(thumbnail)

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
	__install_directory_specif(${CMAKE_CURRENT_SOURCE_DIR}/thumbnail thumbnail)
endif()
