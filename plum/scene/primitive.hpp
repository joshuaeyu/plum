#pragma once

#include <plum/scene/mesh.hpp>

namespace Component {

    class Primitive : public Mesh {
        public:
            static void DrawQuad();
        protected:
            Primitive();
    };

    class Cube : public Primitive {
        public:
            Cube(const int nrows = 1, const int ncols = 1);
        private:
            int nRows, nCols;
    };

    class Sphere : public Primitive  {
        public:
            Sphere(const int nstacks = 30, const int nsectors = 30);
        private:
            int nStacks, nSectors;
    };

    class Rectangle : public Primitive {
        public:
            Rectangle(const int nrows = 1, const int ncols = 1);
        private:
            int nRows, nCols;
    };

}