project(win-agora)

add_definitions(${AGORA_DEFINITIONS})

if(WIN32 AND NOT MINGW)
	set(win-agora_PLATFORM_DEPS
		w32-pthreads)
endif()

set(win-agora_HEADERS
	)

set(win-agora_SOURCES
    agora-plugin.cpp
    agora-pcm-encoder.cpp
	agora-yuv-encoder.cpp
	agora-output.cpp
	agora-service.cpp)

add_library(win-agora MODULE
    ${win-agora_HEADERS}
    ${win-agora_SOURCES})

target_link_libraries(win-agora
	${win-agora_PLATFORM_DEPS}
    libobs)

install_obs_plugin_with_data(win-agora data)
