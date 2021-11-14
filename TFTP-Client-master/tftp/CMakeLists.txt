# add source code name in DIR_SRCS
aux_source_directory(. DIR_LIB_SRCS)
# set link library
add_library(tftp ${DIR_LIB_SRCS})