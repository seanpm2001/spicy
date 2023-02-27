
#include <memory>

class Type {
public:
    int x;
};

class TypeType {};

using TypePtr = std::shared_ptr<Type>;

int inspectType1(const Type& foo) {
    return foo.x;
}

int inspectType2(Type foo) {
    return foo.x;
}

int main(int argc, char** argv) {
    Type f;
    TypeType tt;
    return inspectType2(f);
}
