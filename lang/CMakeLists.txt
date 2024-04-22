add_executable(${EXECUTABLE_NAME_1} main.cpp)

target_link_libraries(${EXECUTABLE_NAME_1} 
	PUBLIC ${LIBRARY_NAME}
)

add_executable(${EXECUTABLE_NAME_2} ast_print.cpp)

target_link_libraries(${EXECUTABLE_NAME_2} 
	PUBLIC ${LIBRARY_NAME}
)