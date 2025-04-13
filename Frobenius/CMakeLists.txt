cmake_minimum_required(VERSION 3.10)
project(Frobenius VERSION 1.0.0 LANGUAGES C CXX)

# Impostazioni globali
set(CMAKE_CXX_STANDARD 14)
set(CMAKE_C_STANDARD 11)
set(CMAKE_EXPORT_COMPILE_COMMANDS ON)

# Directory di output
set(CMAKE_RUNTIME_OUTPUT_DIRECTORY ${CMAKE_BINARY_DIR}/bin)

# Verifica sistema operativo
if(WIN32)
    add_definitions(-D_CRT_SECURE_NO_WARNINGS)
endif()

# Opzioni di configurazione
option(ENABLE_C_VERSION "Abilita la compilazione della versione C" ON)
option(ENABLE_TESTS "Abilita la compilazione dei test" ON)
option(ENABLE_BENCHMARKS "Abilita la compilazione dei benchmark" ON)

# Definisci una funzione per aggiungere avvisi di compilazione
function(set_compiler_warnings target)
    if(MSVC)
        target_compile_options(${target} PRIVATE 
            /W4 
            /wd4996  # Disabilita avvisi per funzioni deprecate di C
        )
    else()
        target_compile_options(${target} PRIVATE
            -Wall 
            -Wextra 
            -Wshadow 
            -Wconversion 
            -Wpedantic
        )
    endif()
endfunction()

# Versione C standard
if(ENABLE_C_VERSION)
    add_executable(frobenius Frobenius.c)
    set_compiler_warnings(frobenius)
    
    # Copia l'eseguibile nella directory bin principale
    add_custom_command(TARGET frobenius POST_BUILD
        COMMAND ${CMAKE_COMMAND} -E copy $<TARGET_FILE:frobenius> ${CMAKE_SOURCE_DIR}/bin/
    )
endif()

# Test
if(ENABLE_TESTS)
    enable_testing()
    
    # Test della versione C
    if(ENABLE_C_VERSION)
        add_executable(test_frobenius test/test_frobenius.c)
        set_compiler_warnings(test_frobenius)
        add_test(NAME C_Implementation COMMAND test_frobenius)
    endif()
endif()

# Benchmark
if(ENABLE_BENCHMARKS)
    if(ENABLE_C_VERSION)
        add_executable(bench_frobenius benchmark/bench_frobenius.c)
        set_compiler_warnings(bench_frobenius)
    endif()
endif()

# Installazione
install(TARGETS frobenius DESTINATION bin)

# Verifica presenza strumenti aggiuntivi
find_program(CARGO_EXECUTABLE cargo)
if(CARGO_EXECUTABLE)
    message(STATUS "Rust Cargo trovato: ${CARGO_EXECUTABLE}")
    
    # Aggiungi un target personalizzato per compilare il progetto Rust
    add_custom_target(rust_build
        COMMAND ${CARGO_EXECUTABLE} build --release
        WORKING_DIRECTORY ${CMAKE_SOURCE_DIR}
        COMMENT "Compilazione del progetto Rust"
    )
    
    # Aggiungi una dipendenza per copiare l'eseguibile Rust nella directory bin
    add_custom_command(TARGET rust_build POST_BUILD
        COMMAND ${CMAKE_COMMAND} -E copy 
            ${CMAKE_SOURCE_DIR}/target/release/frobenius_num.exe 
            ${CMAKE_SOURCE_DIR}/bin/
        COMMENT "Copia dell'eseguibile Rust nella directory bin"
    )
else()
    message(STATUS "Rust Cargo non trovato. La versione Rust non sarà compilata automaticamente.")
endif()

find_program(GHC_EXECUTABLE ghc)
if(GHC_EXECUTABLE)
    message(STATUS "Haskell GHC trovato: ${GHC_EXECUTABLE}")
    
    # Aggiungi un target personalizzato per compilare il codice Haskell
    add_custom_target(haskell_build
        COMMAND ${GHC_EXECUTABLE} -O2 -o ${CMAKE_SOURCE_DIR}/bin/frobenius_hs.exe ${CMAKE_SOURCE_DIR}/Frobenius.hs
        WORKING_DIRECTORY ${CMAKE_SOURCE_DIR}
        COMMENT "Compilazione del codice Haskell"
    )
else()
    message(STATUS "Haskell GHC non trovato. La versione Haskell non sarà compilata automaticamente.")
endif()

find_program(PYTHON_EXECUTABLE python)
if(PYTHON_EXECUTABLE)
    message(STATUS "Python trovato: ${PYTHON_EXECUTABLE}")
    
    # Verifica le dipendenze Python
    execute_process(
        COMMAND ${PYTHON_EXECUTABLE} -c "import numpy, sympy"
        RESULT_VARIABLE PYTHON_DEPS_MISSING
        OUTPUT_QUIET
        ERROR_QUIET
    )
    
    if(PYTHON_DEPS_MISSING)
        message(STATUS "Dipendenze Python mancanti. Eseguire: pip install numpy sympy")
    else()
        message(STATUS "Dipendenze Python verificate.")
    endif()
else()
    message(STATUS "Python non trovato. Lo script Python dovrà essere eseguito manualmente.")
endif()

# Info di compilazione
message(STATUS "Generazione configurata per ${CMAKE_SYSTEM_NAME}")
message(STATUS "Compilatore C: ${CMAKE_C_COMPILER_ID} ${CMAKE_C_COMPILER_VERSION}")
message(STATUS "Compilatore C++: ${CMAKE_CXX_COMPILER_ID} ${CMAKE_CXX_COMPILER_VERSION}")