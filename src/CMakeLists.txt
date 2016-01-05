set(SOURCES
    ${SOURCES}

    ${CMAKE_CURRENT_SOURCE_DIR}/jsoncpp.cpp

    PARENT_SCOPE
)
set(HEADERS
   	${HEADERS}

    ${CMAKE_CURRENT_SOURCE_DIR}/json/json-forwards.h
    ${CMAKE_CURRENT_SOURCE_DIR}/json/json.h
	
    PARENT_SCOPE
)
