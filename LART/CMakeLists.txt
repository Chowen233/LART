cmake_minimum_required(VERSION 3.12)
project(LART)

set(CMAKE_CXX_STANDARD 20)
set(CMAKE_CXX_STANDARD_REQUIRED ON)

set(EXTERNAL
  src/external/stb_image_write.h
)

set(SOURCE_LART
  src/LART.cpp
  src/LART.h
  src/vec3.h
  src/color.h
  src/ray.h
  src/hittable.h
  src/sphere.h
  src/hittable_list.h
  src/interval.h
  src/camara.h
  src/material.h
  src/aabb.h
  src/quad.h
  src/obj_loader.h
 )

add_executable(LART ${EXTERNAL} ${SOURCE_LART})

# OpenMP
find_package(OpenMP REQUIRED)
target_link_libraries(LART PRIVATE OpenMP::OpenMP_CXX)

# OIDN 配置
set(OIDN_DIR "${CMAKE_CURRENT_SOURCE_DIR}/src/external/oidn")

if(EXISTS ${OIDN_DIR})
    message(STATUS "Found OIDN at: ${OIDN_DIR}")
    
    # 包含头文件
    target_include_directories(LART PRIVATE ${OIDN_DIR}/include)
    
    # Windows 平台：链接库文件
    if(WIN32)
        # 链接主库
        target_link_libraries(LART PRIVATE "${OIDN_DIR}/lib/OpenImageDenoise.lib")
        message(STATUS "Linked OIDN library: ${OIDN_DIR}/lib/OpenImageDenoise.lib")
        
        # 复制所有必需的 DLL 到输出目录
        add_custom_command(TARGET LART POST_BUILD
            COMMAND ${CMAKE_COMMAND} -E echo "Copying OIDN DLLs..."
            COMMAND ${CMAKE_COMMAND} -E copy_if_different
                "${OIDN_DIR}/bin/OpenImageDenoise.dll"
                "$<TARGET_FILE_DIR:LART>"
            COMMAND ${CMAKE_COMMAND} -E copy_if_different
                "${OIDN_DIR}/bin/OpenImageDenoise_core.dll"
                "$<TARGET_FILE_DIR:LART>"
            COMMAND ${CMAKE_COMMAND} -E copy_if_different
                "${OIDN_DIR}/bin/OpenImageDenoise_device_cpu.dll"
                "$<TARGET_FILE_DIR:LART>"
            COMMAND ${CMAKE_COMMAND} -E copy_if_different
                "${OIDN_DIR}/bin/tbb12.dll"
                "$<TARGET_FILE_DIR:LART>"
            COMMENT "Copying OIDN DLLs to output directory"
        )
    endif()
    
    target_compile_definitions(LART PRIVATE USE_OIDN)
    message(STATUS "OIDN support enabled")
    
else()
    message(WARNING "OIDN directory not found at ${OIDN_DIR}. Denoising will be disabled.")
    target_compile_definitions(LART PRIVATE OIDN_DISABLED)
endif()

# 设置输出目录
#set_target_properties(LART PROPERTIES
#    RUNTIME_OUTPUT_DIRECTORY ${CMAKE_BINARY_DIR}/bin
#)

# 复制模型文件（如果需要）
if(EXISTS "${CMAKE_CURRENT_SOURCE_DIR}/models")
    add_custom_command(TARGET LART POST_BUILD
        COMMAND ${CMAKE_COMMAND} -E make_directory ${CMAKE_BINARY_DIR}/bin/models
        COMMAND ${CMAKE_COMMAND} -E copy_directory
            "${CMAKE_CURRENT_SOURCE_DIR}/models"
            "${CMAKE_BINARY_DIR}/bin/models"
        COMMENT "Copying model files"
    )
endif()

# 设置编译器优化选项
if(MSVC)
    target_compile_options(LART PRIVATE /O2 /EHsc /fp:fast)
    target_compile_definitions(LART PRIVATE _USE_MATH_DEFINES)
else()
    target_compile_options(LART PRIVATE 
        -O3 
        -march=native 
        -ffast-math
        -Wall
        -Wextra
        -Wpedantic
    )
endif()