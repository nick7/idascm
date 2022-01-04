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

set (PROJECT test_json)
add_executable (
    ${PROJECT}
        test_json.cpp
)
target_link_libraries (
    ${PROJECT}
    PUBLIC
        core
)
