add_executable(my_exe CrowExample.cpp)
target_link_libraries(my_exe PRIVATE Crow::Crow asio::asio ZLIB::ZLIB libuv::uv)
target_include_directories(my_exe PRIVATE ${UWEBSOCKETS_INCLUDE_DIRS})