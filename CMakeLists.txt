
__assert_target(png)
if(NOT GLM_INCLUDE_DIRS)
	message(FATAL_ERROR "GLM_INCLUDE_DIRS Missing")
endif()

set(LIBS png cxbin trimesh2 cxalarm)
set(INCS ${GLM_INCLUDE_DIRS})
set(DEFS GLM_FORCE_LEFT_HANDED THUMBNAIL_DLL)

__add_real_target(thumbnail dll SOURCE
									stl/helpers.h
									stl/parser.h
									stl/parser.cpp
									picture.cpp
									picture.h
									aabb.cpp
									aabb.h
									helper.cpp
									helper.h
									vec3.h
									vec4.h
									triangle.h
									raster_backend/raster.cpp
									raster_backend/raster.h
									raster_backend/rasterconfig.cpp
									raster_backend/rasterconfig.h
									raster_backend/zbuffer.cpp
									raster_backend/zbuffer.h
									topomesh.h
									topomesh.cpp
									stl_thumbnail.h
									stl_thumbnail.cpp
								LIB ${LIBS}
								INC ${INCS}
								DEF ${DEFS}
								INTERFACE ${CMAKE_CURRENT_SOURCE_DIR}
								)


