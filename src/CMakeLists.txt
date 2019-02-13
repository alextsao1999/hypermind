include_directories(.)

set(FILES main.cpp buffer.cpp lexer.cpp parser.cpp utils.cpp utils.h  vm.cpp obj/string.cpp buf.cpp buf.h)

add_executable(HyperMind ${FILES})
