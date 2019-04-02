# Created By 曹顺 

include_directories(.)

set(DIRS . obj)

foreach (DIR ${DIRS})
    aux_source_directory(${DIR} FILES)
endforeach ()

add_executable(HyperMind ${FILES})
