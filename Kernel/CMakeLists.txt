cmake_minimum_required(VERSION 2.8)

if(NOT DEFINED CMAKE_TOOLCHAIN_FILE)
  if(DEFINED ENV{VITASDK})
    set(CMAKE_TOOLCHAIN_FILE "$ENV{VITASDK}/share/vita.toolchain.cmake" CACHE PATH "toolchain file")
  else()
    message(FATAL_ERROR "Please define VITASDK to point to your SDK path!")
  endif()
endif()

project(psp2renga)
include("${VITASDK}/share/vita.cmake" REQUIRED)

set(CMAKE_C_FLAGS "-Wl,-q -Wall -O3 -std=gnu99")
set(CMAKE_CXX_FLAGS "${CMAKE_C_FLAGS} -std=c++11 -fno-rtti -fno-exceptions")

include_directories(
)

link_directories(
	${CMAKE_CURRENT_BINARY_DIR}
	${CMAKE_BINARY_DIR}/365_stubs
)

add_executable(psp2renga.elf
	main.c
)

target_link_libraries(psp2renga.elf
	SceSblSmCommForKernel_stub
	SceIofilemgrForDriver_stub
	SceSysclibForDriver_stub
	SceSysmemForDriver_stub
	SceSysrootForKernel_stub
	taihenForKernel_stub
	taihenModuleUtils_stub
)

set_target_properties(psp2renga.elf
	PROPERTIES LINK_FLAGS "-nostdlib"
	COMPILE_FLAGS "-D__VITA_KERNEL__"
)

add_custom_target(psp2renga.skprx ALL
	COMMAND vita-elf-create -e ${CMAKE_SOURCE_DIR}/psp2renga.yml psp2renga.elf psp2renga.velf
	COMMAND vita-make-fself -c psp2renga.velf psp2renga.skprx
)

add_dependencies(psp2renga.skprx psp2renga.elf)

vita_create_stubs(stubs psp2renga.elf ${CMAKE_SOURCE_DIR}/psp2renga.yml KERNEL)