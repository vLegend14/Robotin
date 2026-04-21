file(GLOB srcs "*.c")
list(REMOVE_ITEM srcs "${CMAKE_CURRENT_SOURCE_DIR}/u8x8_d_stdio.c")

idf_component_register(SRCS ${srcs}
                       INCLUDE_DIRS ".")