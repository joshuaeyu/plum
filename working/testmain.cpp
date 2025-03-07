#include "filemanager.hpp"

#include <filesystem>
#include <iostream>

int main() {
    // const std::filesystem::path mypath(".hubba/path/fh");
    // for (std::filesystem::path::iterator i = mypath.begin(); i != mypath.end(); i++) {
    //     std::cout << *i << std::endl;
    // }
    // for (auto const& dir : mypath) {
    //     std::cout << dir << std::endl;
    // }
    // int a[] = {1,2,3,4};
    // for (int& i : a) {
    //     std::cout << i << std::endl;
    // }
    const std::filesystem::path currdir = std::filesystem::current_path();
    for (auto i = std::filesystem::directory_iterator(currdir); i != std::filesystem::end(i); i++) {
        std::cout << i->path() << std::endl;
    }
    for (auto const& entry : std::filesystem::directory_iterator(currdir))
        std::cout << entry.path().filename() << std::endl;

}