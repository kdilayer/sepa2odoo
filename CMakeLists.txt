cmake_minimum_required(VERSION 3.5)


set(CMAKE_CXX_STANDARD 20)
set(CMAKE_CXX_STANDARD_REQUIRED ON)
set(CMAKE_MODULE_PATH ${CMAKE_MODULE_PATH} "${CMAKE_CURRENT_SOURCE_DIR}/cmake")

message(STATUS "compiler: " ${CMAKE_CXX_COMPILER_ID})
message(STATUS "sepa2odoo build type: " ${CMAKE_BUILD_TYPE})

#set(CMAKE_CXX_STANDARD 20)
project(sepa2odoo CXX C)

configure_file(${PROJECT_SOURCE_DIR}/config_app.h.in
${PROJECT_SOURCE_DIR}/config_app.h @ONLY)

if (CMAKE_BUILD_TYPE)
    string(TOLOWER ${CMAKE_BUILD_TYPE} _type) 
    if (_type STREQUAL release)
        message(STATUS "build type RELEASE") 
        set(CMAKE_BUILD_DIR build_release)
        add_compile_options(-D_RELEASE)
        #add flto
        add_compile_options(${GCC_LTO})
        #add_compile_options(-O2)
        add_definitions(${GCC_LTO})
    else ()
        set(CMAKE_BUILD_DIR build)
        message(STATUS "build type DEBUG")
        add_compile_options(-D_DEBUG)
        add_compile_options(-g)
    endif ()
else (CMAKE_BUILD_TYPE)
    message(STATUS "build type DEBUG")
    set(CMAKE_BUILD_DIR build)
    add_compile_options(-D_DEBUG)
    add_compile_options(-g)
endif (CMAKE_BUILD_TYPE)

add_compile_options("-Wno-sign-compare")
add_compile_options("-Wno-unused-function")


set(base_sources
    main.cpp
    odoo_api.cpp
    util.cpp
    logger.cpp
    config_profile.cpp
    camt05300102.cpp
    pain00100109.cpp
    pain00200110.cpp
    raaccounttransactions.cpp
)
set(prj_sources
    ${base_sources}
)

set(private_headers

)
find_package(XMLRPC REQUIRED COMPONENTS client c++)
find_package(CURL REQUIRED)
include_directories(${CURL_INCLUDE_DIRS})

message(STATUS "XMLRPC libs: " ${XMLRPC_LIBRARIES})
include_directories(${XMLRPC_INCLUDE_DIRS}) 


#RapidXML
include_directories("third_party/RapidXML/RapidXML")
include_directories("third_party/rapidXPath-cpp/src")

#rapidjson - conflicts with plutobook document.h
include_directories("third_party/rapidjson/include")
#xml2json
include_directories("third_party/xml2json/include")


# when using from source include_directories("${CMAKE_SOURCE_DIR}/third_party/poppler/build/cpp")

add_executable(${PROJECT_NAME} ${prj_sources})

#target_link_libraries(${PROJECT_NAME} minizip)

target_link_libraries(${PROJECT_NAME} ${XMLRPC_LIBRARIES})
target_link_libraries(${PROJECT_NAME} xmlrpc_client++)
target_link_libraries(${PROJECT_NAME} xmlrpc++)
target_link_libraries(${PROJECT_NAME} xmlrpc_client)
target_link_libraries(${PROJECT_NAME} xmlrpc)
target_link_libraries(${PROJECT_NAME} ${CURL_LIBRARIES})
#target_link_libraries(${PROJECT_NAME} jsoncpp)



#target_link_libraries(${PROJECT_NAME} uuid)
#target_link_libraries(${PROJECT_NAME} ssl)
#target_link_libraries(${PROJECT_NAME} crypto) 
#target_link_libraries(${PROJECT_NAME} mariadb)
#target_link_libraries(${PROJECT_NAME} z)


