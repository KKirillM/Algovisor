cmake_minimum_required(VERSION 2.4) 

# Разрешаем линковаться с биб-ками без указания их точного пути (используется для системных биб-к)
cmake_policy(SET CMP0003 NEW)

# Ограничиваем возможные конфигурации
set (CMAKE_CONFIGURATION_TYPES Debug Release CACHE TYPE INTERNAL FORCE) 

project (edbExport CXX) 

macro (SourceDirectories sources)
	foreach (dir ${ARGN})
		file (GLOB s_ RELATIVE ${CMAKE_SOURCE_DIR} ${dir}/*.cpp ${dir}/*.h ${dir}/*.hpp)
		list (APPEND ${sources} ${s_})
	endforeach(dir) 
endmacro(SourceDirectories)

find_package(PLAZA-II REQUIRED PATHS .. share/plaza2 NO_DEFAULT_PATH) 
include(${PLAZA-II_ROOT}/cmake_utils.cmake) 

P2Build_ConfigureVerRC(Source/ver.rc ${CMAKE_BINARY_DIR}/ver.rc) 
SourceDirectories (sources Source)
list (APPEND sources ${CMAKE_BINARY_DIR}/ver.rc)


#if (NOT MSVC)
#	set (CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -std=c++0x")
#endif (NOT MSVC)

P2Build_SetCompilerFlags()
P2Build_DisableInSourceBuilds()

# Задаем путь относительно CMakeLists.txt куда будут выкладываться бинарники.
# проверка того, происходит ли сборка на Jenkins'e
if (DEFINED ENV{JENKINS_URL}) 
	P2Build_SetOutputDirectories(${CMAKE_CURRENT_BINARY_DIR}) 
else (DEFINED ENV{JENKINS_URL})
	P2Build_SetOutputDirectories(${CMAKE_CURRENT_SOURCE_DIR}/../..) 
endif (DEFINED ENV{JENKINS_URL})

# Сборка исполняемого модуля
include_directories(${PLAZA-II_INCLUDE})
add_executable (edbexport ${sources})
target_link_libraries(edbexport ${P2Sys} ${P2eReplStorage})

if(MSVC)
    set_property (TARGET edbexport APPEND_STRING PROPERTY COMPILE_FLAGS "/W4")
endif(MSVC)

P2Build_GenerateDebugSymbolsInReleaseBuild(edbexport)
P2Build_SetTargetPostfix(edbexport)

# Инсталляция 
install (TARGETS edbexport DESTINATION bin) 
install (DIRECTORY Run DESTINATION . FILES_MATCHING REGEX .*\\.ini$)
install (DIRECTORY ${CMAKE_RUNTIME_OUTPUT_DIRECTORY_RELEASE} DESTINATION debug CONFIGURATIONS Release FILES_MATCHING REGEX .*[^D]\\.pdb$)  
install (DIRECTORY ${CMAKE_RUNTIME_OUTPUT_DIRECTORY_DEBUG}   DESTINATION debug CONFIGURATIONS Debug   FILES_MATCHING REGEX .*D\\.pdb$) 
