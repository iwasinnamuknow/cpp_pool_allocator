#include <vector>
#include "Object.hpp"

int main(int argc, char const *argv[]) {

    // Allocate 10 pointers to our `Object` instances:

    constexpr int num_objects = 10000;

    std::vector<Object*> m_objects{};
    m_objects.reserve(num_objects);

    // Two `uint64_t`, 16 bytes.
    //cout << "size(Object) = " << sizeof(Object) << endl << endl;

    // Allocate 10 objects. This causes allocating two larger,
    // blocks since we store only 8 chunks per block:

    //cout << "About to allocate " << num_objects << " objects" << endl;
    {
        for (int i = 0; i < num_objects; ++i) {
            ZoneScopedN("Object allocate");
            m_objects.push_back(new Object());
            //cout << "new [" << i << "] = " << m_objects[uuid] << " " << uuid << endl;
        }
    }

    //cout << endl;

    {
        // Deallocate all the objects
        while (!m_objects.empty()) {
            ZoneScopedN("Object deallocate");
            //cout << "delete " << i << " [" << it->second << "] = " << it->first << endl;
            delete m_objects.back();
            m_objects.pop_back();
        }
    }

    //cout << endl;

    {
        // New objects reuses previous blocks
        for (int i = 0; i < num_objects; ++i) {
            ZoneScopedN("Object allocate");
            m_objects.push_back(new Object());
            //cout << "new [" << i << "] = " << m_objects[uuid] << " " << uuid << endl;
        }
    }
}