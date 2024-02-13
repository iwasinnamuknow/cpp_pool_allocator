//
// MIT License
//
// Copyright (c) 2024 iwasinnamuknow
//
// Permission is hereby granted, free of charge, to any person obtaining a copy
// of this software and associated documentation files (the "Software"), to deal
// in the Software without restriction, including without limitation the rights
// to use, copy, modify, merge, publish, distribute, sublicense, and/or sell
// copies of the Software, and to permit persons to whom the Software is
// furnished to do so, subject to the following conditions:
//
// The above copyright notice and this permission notice shall be included in all
// copies or substantial portions of the Software.
//
// THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS OR
// IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY,
// FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT. IN NO EVENT SHALL THE
// AUTHORS OR COPYRIGHT HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER
// LIABILITY, WHETHER IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING FROM,
// OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER DEALINGS IN THE
// SOFTWARE.
//

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