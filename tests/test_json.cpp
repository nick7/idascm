# include <core/json.hpp>

namespace
{
    char const gs_test_json[] = R"(
    {
        "foo": [ 1, "bar" ],
    }
    )";
}

int main(int argc, char * argv[])
{
    using namespace idascm;

    json_object root;

    auto value  = json_value::from_string(gs_test_json);
    root = value.to_object();
    
   
    auto foo    = root["foo"].to_array();
    auto first  = foo.at(0).to_primitive();


    return 0;
}
