#pragma once

#include <string>

class Demo {
    public:
        Demo(const std::string& title) : title(title) {}
        virtual ~Demo() = default;

        const std::string title;

        bool ShouldEnd() const { return shouldEnd; }
        
        virtual void Initialize() = 0;
        virtual void Display() = 0;
        virtual void CleanUp() = 0;
    
    protected:
        bool shouldEnd = false;
};