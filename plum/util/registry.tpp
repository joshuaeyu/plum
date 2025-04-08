#pragma once

#include <memory>
#include <vector>

typedef unsigned int RegistryHandle;

// Registry for any object
template<class T>
class Registry {
    public:
        Registry();

        T& Get(RegistryHandle id);
        
        template<typename... Args>
        RegistryHandle Emplace(Args&& ...args);
        
        RegistryHandle Push(std::shared_ptr<T> value);
        void Erase(RegistryHandle id);

    private:
        bool handleIsValid(RegistryHandle id);

        RegistryHandle currHandle = 0;
        std::vector<std::shared_ptr<T>> data;
};

#include <plum/util/registry.ipp>