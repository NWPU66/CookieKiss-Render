#include <string>

#include <open3d/Open3D.h>
#include <open3d/visualization/utility/Draw.h>

int main(int argc, char* argv[])
{
    auto sphere = open3d::geometry::TriangleMesh::CreateSphere(1.0);
    sphere->ComputeVertexNormals();
    sphere->PaintUniformColor({0.0, 1.0, 0.0});
    open3d::visualization::Draw({sphere});
    return 0;
}