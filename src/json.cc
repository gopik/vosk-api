#include "json.h"
namespace json {
JSON Array() {
    return JSON::Make( JSON::Class::Array );
}
JSON Object() {
    return JSON::Make( JSON::Class::Object );
}
std::ostream& operator<<( std::ostream &os, const JSON &json ) {
    os << json.dump();
    return os;
}
JSON JSON::Load( const string &str ) {
    size_t offset = 0;
    return parse_next( str, offset );
}
}
