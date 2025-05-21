cmake_minimum_required(VERSION 3.10.2)
project(TamaTasker)

set(CMAKE_CXX_STANDARD 23)
set(CMAKE_CXX_STANDARD_REQUIRED ON)
set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -Wall")
set(Boost_USE_MULTITHREADED ON)

# Поиск библиотек
find_package(Threads REQUIRED)
find_package(OpenSSL REQUIRED)
find_package(Boost COMPONENTS system REQUIRED)
find_package(CURL)
find_package(SQLite3 REQUIRED)

# Пути к заголовкам и библиотекам
include_directories(/usr/local/include ${OPENSSL_INCLUDE_DIR} ${Boost_INCLUDE_DIR} ${SQLite3_INCLUDE_DIRS}) 
link_directories(/usr/local/lib)

if (CURL_FOUND)
    include_directories(${CURL_INCLUDE_DIRS})
    add_definitions(-DHAVE_CURL)
endif()

# Логирование (для отладки)
message(STATUS "Boost include dir: ${Boost_INCLUDE_DIR}")
message(STATUS "OpenSSL libraries: ${OPENSSL_LIBRARIES}")
message(STATUS "SQLite3 libraries: ${SQLite3_LIBRARIES}")

# Указание исходников и сборка
file(GLOB SOURCES "src/*.cpp")
set(TGBOT_LIB /usr/local/lib/libTgBot.a)

add_executable(TamaTasker ${SOURCES})

target_link_libraries(TamaTasker
    ${TGBOT_LIB}
    ${Boost_LIBRARIES}
    ${OPENSSL_LIBRARIES}
    ${CMAKE_THREAD_LIBS_INIT}
    ${CURL_LIBRARIES}
    SQLite::SQLite3 
)
