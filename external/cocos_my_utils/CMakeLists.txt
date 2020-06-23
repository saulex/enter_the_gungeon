add_library(cocos_my_utils)

target_include_directories(cocos_my_utils
	PUBLIC include/)

target_link_libraries(cocos_my_utils
	PRIVATE cocos2d)

target_sources(cocos_my_utils
	PRIVATE src/my_utils.cpp)

source_group("Header Files" FILES include/my_utils.hpp)