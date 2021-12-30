set (PROJECT test_engine)
add_executable (
    ${PROJECT}
        test_engine.cpp
)
target_link_libraries (
    ${PROJECT}
    PUBLIC
        engine
)
