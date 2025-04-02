#pragma once

#include <string>

class Demo {
    public:
        Demo(const std::string& title) : title(title) {}
        
        virtual ~Demo() = default;

        bool ShouldEnd() const { return shouldEnd; }
        
        virtual void Initialize() = 0;
        virtual void Display() = 0;
        virtual void CleanUp() = 0;

        const std::string title;
    
    protected:
        bool shouldEnd = false;
};