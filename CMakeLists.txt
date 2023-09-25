__cc_find(zlib_static)
__cc_find(png_static)
__cc_find(trimesh2)
__cc_find(glm)

__assert_target(zlib_static)
__assert_target(png_static)
__assert_target(trimesh2)
__assert_target(glm)

set(LIBS glm)
set(ILIBS trimesh2 png_static zlib_static)
set(INCS )

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
									thumbnail/thumbnail.h
									thumbnail/thumbnail.cpp
									thumbnail/img2gcode.cpp
									thumbnail/img2gcode.h
								)

__add_real_target(thumbnail_static lib SOURCE ${SRCS}
									   LIB ${LIBS}
									   ILIB ${ILIBS}
								       INC ${INCS}
								       DEF ${DEFS}
								       INTERFACE ${CMAKE_CURRENT_SOURCE_DIR}
									   SOURCE_FOLDER
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
	__install_directory_specif(${CMAKE_CURRENT_SOURCE_DIR}/thumbnail thumbnail)
endif()
