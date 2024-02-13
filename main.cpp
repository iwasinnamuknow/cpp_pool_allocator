#include <iostream>
#include "Object.hpp"

using std::cout;
using std::endl;

int main(int argc, char const *argv[]) {

    // Allocate 10 pointers to our `Object` instances:

    constexpr int arraySize = 100;

    Object *objects[arraySize];

    // Two `uint64_t`, 16 bytes.
    cout << "size(Object) = " << sizeof(Object) << endl << endl;

    // Allocate 10 objects. This causes allocating two larger,
    // blocks since we store only 8 chunks per block:

    cout << "About to allocate " << arraySize << " objects" << endl;

    for (int i = 0; i < arraySize; ++i) {
        objects[i] = new Object();
        cout << "new [" << i << "] = " << objects[i] << endl;
    }

    cout << endl;

    // Deallocated all the objects:

    for (int i = arraySize - 1; i >= 0; --i) {
        cout << "delete [" << i << "] = " << objects[i] << endl;
        delete objects[i];
    }

    cout << endl;

    // New object reuses previous block:

    objects[0] = new Object();
    cout << "new [0] = " << objects[0] << endl << endl;
}