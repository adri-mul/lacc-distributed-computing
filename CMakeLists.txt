cmake_minimum_required(VERSION 3.16)

project(lasa_cc LANGUAGES CXX)

set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED ON)

# Course-friendly defaults
option(LACC_BUILD_TESTS "Build unit tests" ON)
option(LACC_ENABLE_OPENMP "Enable OpenMP support if available" ON)
option(LACC_ENABLE_MPI "Enable MPI support if available" OFF)
option(LACC_ENABLE_GW "Enable GW (geophysical) components and tests" OFF)

# Policy tooling (compile_commands.json)
set(CMAKE_EXPORT_COMPILE_COMMANDS ON)

# Optional runtimes
if(LACC_ENABLE_OPENMP)
  find_package(OpenMP QUIET)
endif()

if(LACC_ENABLE_MPI)
  find_package(MPI QUIET COMPONENTS CXX)
endif()

# -------------------------
# Libraries (engine)
# -------------------------

add_library(lacc_common
  src/adt/HashTableChained.cpp
  src/io/FileReader.cpp
)
target_include_directories(lacc_common
  PUBLIC
    ${CMAKE_CURRENT_SOURCE_DIR}/include
  PRIVATE
    ${CMAKE_CURRENT_SOURCE_DIR}/src
)

add_library(lacc_core
  src/core/Cli.cpp
  src/core/Runner.cpp
  src/core/Mpi.cpp
  src/core/WorkDistributorMPI.cpp
  src/core/ReferenceAdts.cpp
)
target_include_directories(lacc_core
  PUBLIC
    ${CMAKE_CURRENT_SOURCE_DIR}/include
  PRIVATE
    ${CMAKE_CURRENT_SOURCE_DIR}/src
)

if(OpenMP_CXX_FOUND)
  target_link_libraries(lacc_core PUBLIC OpenMP::OpenMP_CXX)
endif()

if(MPI_CXX_FOUND)
  target_link_libraries(lacc_core PUBLIC MPI::MPI_CXX)
  target_compile_definitions(lacc_core PUBLIC LACC_HAS_MPI=1)
endif()

if(LACC_ENABLE_GW)
  add_library(lacc_physics
    src/physics/GwCsv.cpp
    src/physics/MatchedFilter.cpp
  )
  target_link_libraries(lacc_physics PUBLIC lacc_common)
else()
  add_library(lacc_physics INTERFACE)
endif()

add_library(lacc_bio
  src/bio/Fasta.cpp
  src/bio/Kmer.cpp
)
target_link_libraries(lacc_bio PUBLIC lacc_common)

if(OpenMP_CXX_FOUND)
  target_link_libraries(lacc_bio PUBLIC OpenMP::OpenMP_CXX)
endif()

add_library(lacc_plot
  src/plot/SvgPlot.cpp
)
target_include_directories(lacc_plot
  PUBLIC
    ${CMAKE_CURRENT_SOURCE_DIR}/include
)

# -------------------------
# Apps (drivers)
# -------------------------

add_executable(lacc_analyze apps/lacc_analyze.cpp)
target_link_libraries(lacc_analyze PRIVATE lacc_bio lacc_plot)
if(LACC_ENABLE_GW)
  target_link_libraries(lacc_analyze PRIVATE lacc_physics)
endif()

add_executable(lacc_exec apps/lacc_exec.cpp)
target_link_libraries(lacc_exec PRIVATE lacc_core)

# -------------------------
# Tests
# -------------------------

if(LACC_BUILD_TESTS)
  include(CTest)
  enable_testing()

  # Catch2 setup (OFFLINE-FIRST)
  #
  # We do NOT download dependencies by default because some cluster environments
  # have restricted networking / broken TLS trust stores.
  #
  # Order:
  #  1) Use an installed Catch2 if found (system/vcpkg/homebrew)
  #  2) Else use vendored zip in third_party/
  #  3) Else fail with a clear message
  #
  set(LACC_CATCH2_VERSION "3.5.4" CACHE STRING "Pinned Catch2 version (offline-first)")

  find_package(Catch2 ${LACC_CATCH2_VERSION} CONFIG QUIET)

  if(NOT Catch2_FOUND)
    set(_catch2_zip "${CMAKE_CURRENT_SOURCE_DIR}/third_party/Catch2-${LACC_CATCH2_VERSION}.zip")
    if(EXISTS "${_catch2_zip}")
      include(FetchContent)

      # Use vendored archive (no network)
      FetchContent_Declare(
        Catch2
        URL "file://${_catch2_zip}"
        DOWNLOAD_EXTRACT_TIMESTAMP TRUE
      )
      FetchContent_MakeAvailable(Catch2)
    else()
      message(FATAL_ERROR
        "Catch2 ${LACC_CATCH2_VERSION} not found.\n"
        "\n"
        "This project is offline-first and does not download dependencies by default.\n"
        "\n"
        "Fix options:\n"
        "  (A) Vendor Catch2:\n"
        "      Put Catch2-${LACC_CATCH2_VERSION}.zip at:\n"
        "        third_party/Catch2-${LACC_CATCH2_VERSION}.zip\n"
        "\n"
        "  (B) Install Catch2 on your system and make it discoverable by CMake.\n"
        "\n"
        "Then delete your build directory and reconfigure.\n"
      )
    endif()
  endif()

  # Catch2 discovery helper (provided by Catch2)
  # Works both for installed Catch2 and FetchContent builds.
  include(Catch)

  add_executable(test_io tests/test_io.cpp)
  target_link_libraries(test_io PRIVATE lacc_common Catch2::Catch2WithMain)
  catch_discover_tests(test_io)

  add_executable(test_priority_queue tests/test_priority_queue.cpp)
  target_link_libraries(test_priority_queue PRIVATE lacc_common Catch2::Catch2WithMain)
  catch_discover_tests(test_priority_queue)

  add_executable(test_hash_table_chained tests/test_hash_table_chained.cpp)
  target_link_libraries(test_hash_table_chained PRIVATE lacc_common Catch2::Catch2WithMain)
  catch_discover_tests(test_hash_table_chained)

  if(LACC_ENABLE_GW)
    add_executable(test_small_gw tests/test_small_gw.cpp)
    target_link_libraries(test_small_gw PRIVATE lacc_physics lacc_common Catch2::Catch2WithMain)
    # IMPORTANT: run from repo root so "data/..." resolves
    catch_discover_tests(test_small_gw WORKING_DIRECTORY ${CMAKE_SOURCE_DIR})
  endif()

  add_executable(test_small_dna tests/test_small_dna.cpp)
  target_link_libraries(test_small_dna PRIVATE lacc_bio lacc_common Catch2::Catch2WithMain)
  # IMPORTANT: run from repo root so "data/..." resolves
  catch_discover_tests(test_small_dna WORKING_DIRECTORY ${CMAKE_SOURCE_DIR})

    # Contract-output integration is reserved for the completed implementation phase.
    # It is not enabled in the reduced student starter because the student-owned
    # parser and analysis paths are intentionally stubbed.
    #
    # add_executable(test_contract_outputs tests/test_contract_outputs.cpp)
    # target_link_libraries(test_contract_outputs PRIVATE lacc_physics lacc_bio lacc_common)
    # add_test(NAME test_contract_outputs COMMAND test_contract_outputs)
    # set_tests_properties(test_contract_outputs PROPERTIES WORKING_DIRECTORY ${CMAKE_SOURCE_DIR})

  # Policy test: ADT no-STL enforcement (script)
  find_package(Python3 COMPONENTS Interpreter REQUIRED)
  add_test(
    NAME test_no_stl_in_adt
    COMMAND ${Python3_EXECUTABLE} ${CMAKE_CURRENT_SOURCE_DIR}/scripts/check_no_stl_adt.py
  )

  # Optional MPI smoke test (labeled mpi so build_and_run.sh can exclude it)
  if(MPI_CXX_FOUND AND EXISTS "${CMAKE_CURRENT_SOURCE_DIR}/tests/test_mpi_smoke.cpp")
    add_executable(test_mpi_smoke tests/test_mpi_smoke.cpp)
    target_link_libraries(test_mpi_smoke PRIVATE MPI::MPI_CXX)
    target_compile_definitions(test_mpi_smoke PRIVATE LACC_HAS_MPI=1)
    add_test(NAME test_mpi_smoke
      COMMAND ${MPIEXEC_EXECUTABLE} ${MPIEXEC_NUMPROC_FLAG} 2 $<TARGET_FILE:test_mpi_smoke>
    )
    set_tests_properties(test_mpi_smoke PROPERTIES LABELS mpi)
  endif()
endif()

