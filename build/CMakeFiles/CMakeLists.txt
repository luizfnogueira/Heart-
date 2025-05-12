cmake_minimum_required(VERSION 3.10)
project(Heart C)

set(CMAKE_C_STANDARD 99)

# Adiciona diretório de saída para binários
set(CMAKE_RUNTIME_OUTPUT_DIRECTORY ${CMAKE_BINARY_DIR}/bin)

# Encontra o pacote Raylib (necessário ter instalado)
find_package(raylib QUIET)

if (NOT raylib_FOUND)
    include(FetchContent)
    FetchContent_Declare(
        raylib
        URL https://github.com/raysan5/raylib/archive/refs/tags/4.5.0.tar.gz
    )
    FetchContent_MakeAvailable(raylib)
endif()

# Adiciona os arquivos fonte
set(SOURCES
    main.c
    menu.c
    jogo.c
    pontuacao.c
)

# Cria o executável
add_executable(Heart ${SOURCES})

# Vincula com a biblioteca Raylib
target_link_libraries(Heart raylib)

# Configurações específicas para Windows
if(WIN32)
    target_link_libraries(Heart winmm)
    
    # Copia recursos para o diretório de saída
    add_custom_command(TARGET Heart POST_BUILD
        COMMAND ${CMAKE_COMMAND} -E make_directory "${CMAKE_RUNTIME_OUTPUT_DIRECTORY}/recursos"
        COMMAND ${CMAKE_COMMAND} -E copy_directory "${CMAKE_SOURCE_DIR}/recursos" "${CMAKE_RUNTIME_OUTPUT_DIRECTORY}/recursos"
    )
endif()
