#pragma once

#include <plum/component/mesh.hpp>

namespace Component {

    class Primitive : public Mesh {
        public:
            static void DrawQuad();
        protected:
            Primitive();
    };

    class Cube : public Primitive {
        public:
            Cube(int nrows = 1, int ncols = 1);
        private:
            int nRows, nCols;
    };

    class Sphere : public Primitive  {
        public:
            Sphere(int nstacks = 30, int nsectors = 30);
        private:
            int nStacks, nSectors;
    };

    class Plane : public Primitive {
        public:
            Plane(int nrows = 1, int ncols = 1);
        private:
            int nRows, nCols;
    };

}