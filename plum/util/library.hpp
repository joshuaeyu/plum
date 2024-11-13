#ifndef LIBRARY_HPP
#define LIBRARY_HPP

#include <map>

namespace Util {

    typedef unsigned int Id;
    template <typename T>
    class Library {
        public:
            void Add(Id* id, const T rawdata) {
                Add(id, std::make_shared<T>(rawdata));
            }
            void Add(Id* id, std::shared_ptr<T> data) {
                catalog[*id] = data;
            }
            std::shared_ptr<T> Get(Id id) const {
                return catalog[id];
            }
            void Remove(Id id) {
                catalog.erase(id);
            }
        private:
            std::map<Id, std::shared_ptr<T>> catalog;
    };

}

#endif