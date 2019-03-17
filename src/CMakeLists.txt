# Created By 曹顺 

include_directories(.)

foreach (DIR ${DIRS})
    aux_source_directory(${DIR} FILES)
endforeach ()

add_executable(HyperMind ${FILES})
