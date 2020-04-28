project(src)

set(SOURCE_FILES main.cpp DCWSDeque.h CircularArray.h MemoryPool.h)

add_executable(src ${SOURCE_FILES} MemoryPool.h)
