#include "test_dll.h"
#include <iostream>

extern "C" __declspec(dllimport) Polyhedron parse_obj(char *file);

int main()
{
    Polyhedron cube = parse_obj("../SEP Project Models/1. Cube.obj");
    std::cout << cube.list[0].list[0].X << std::endl;
}