# Configuração mínima requerida do CMake
cmake_minimum_required(VERSION 3.15)

# Nome do projeto e linguagem
project(concurrency_control_apis CXX)

# Configuração padrão do C++ (C++17 para recursos de concorrência)
set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED ON)

# Opções de compilação para debugging e warnings
set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -Wall -Wextra -pthread")

# Opções específicas para Debug e Release
set(CMAKE_CXX_FLAGS_DEBUG "${CMAKE_CXX_FLAGS_DEBUG} -g -O0 -DDEBUG")
set(CMAKE_CXX_FLAGS_RELEASE "${CMAKE_CXX_FLAGS_RELEASE} -O3 -DNDEBUG")

# Diretórios de include
include_directories(include)

# Biblioteca principal do projeto
add_library(concurrency_control
    src/thread_pool/thread_pool.cpp
    src/thread_pool/task_queue.cpp
    src/thread_pool/worker_thread.cpp
    src/resource_manager/resource_manager.cpp
    src/resource_manager/shared_resource.cpp
    src/resource_manager/lock_types.cpp
)

# Configurações específicas da biblioteca
target_include_directories(concurrency_control PUBLIC include)

# Exemplos de uso
add_executable(thread_pool_example examples/thread_pool_example.cpp)
target_link_libraries(thread_pool_example concurrency_control)

add_executable(resource_manager_example examples/resource_manager_example.cpp)
target_link_libraries(resource_manager_example concurrency_control)

add_executable(benchmark examples/benchmark.cpp)
target_link_libraries(benchmark concurrency_control)

add_executable(advanced_usage examples/advanced_usage.cpp)
target_link_libraries(advanced_usage concurrency_control)

# Testes (requer Google Test)
find_package(GTest)
if(GTEST_FOUND)
    enable_testing()

    add_executable(test_thread_pool tests/test_thread_pool.cpp)
    target_link_libraries(test_thread_pool concurrency_control GTest::GTest GTest::Main)

    add_executable(test_resource_manager tests/test_resource_manager.cpp)
    target_link_libraries(test_resource_manager concurrency_control GTest::GTest GTest::Main)

    # Adiciona testes ao CTest
    add_test(NAME ThreadPoolTest COMMAND test_thread_pool)
    add_test(NAME ResourceManagerTest COMMAND test_resource_manager)
else()
    message(WARNING "Google Test não encontrado. Testes não serão compilados.")
endif()

# Configurações de instalação (opcional)
install(TARGETS concurrency_control
    ARCHIVE DESTINATION lib
    LIBRARY DESTINATION lib
)

install(DIRECTORY include/ DESTINATION include)

# Configuração para análise de cobertura de código
if(CMAKE_BUILD_TYPE STREQUAL "Debug")
    option(ENABLE_COVERAGE "Enable code coverage" OFF)
    if(ENABLE_COVERAGE)
        target_compile_options(concurrency_control PRIVATE --coverage -fprofile-arcs -ftest-coverage)
        target_link_libraries(concurrency_control PRIVATE --coverage)
    endif()
endif()

# Configuração para sanitizers
option(ENABLE_SANITIZERS "Enable sanitizers (Address, Undefined)" OFF)
if(ENABLE_SANITIZERS)
    target_compile_options(concurrency_control PRIVATE -fsanitize=address,undefined)
    target_link_libraries(concurrency_control PRIVATE -fsanitize=address,undefined)
endif()
