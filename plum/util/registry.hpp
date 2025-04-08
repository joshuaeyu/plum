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
        
        RegistryHandle Push(T&& value);
        void Erase(RegistryHandle id);

    private:
        bool handleIsValid(RegistryHandle id);

        RegistryHandle currHandle = 0;
        std::vector<T> data;
};

template<class T>
Registry<T>::Registry() {}

template<class T>
T& Registry<T>::Get(RegistryHandle id) {
    if (!handleIsValid(id)) {
        throw std::runtime_error("Id provided is invalid.");
    }
    return data[id];
}

template<class T>
template<typename... Args>
RegistryHandle Registry<T>::Emplace(Args&& ...args) {
    data.emplace_back(std::make_shared<T>(args...));
    return currHandle++;
}

template<class T>
RegistryHandle Registry<T>::Push(T&& value) {
    data.push_back(value);
    return currHandle++;
}

template<class T>
void Registry<T>::Erase(RegistryHandle id) {
    if (!handleIsValid(id)) {
        throw std::runtime_error("Id provided is invalid.");
    }
    data[id].reset();
}

template<class T>
bool Registry<T>::handleIsValid(RegistryHandle id) {
    return id >= 0 && id < currHandle && data[id];
}