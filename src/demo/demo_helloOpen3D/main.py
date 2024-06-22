import numpy as np
import open3d as o3d
import open3d.visualization as vis

if __name__ == "__main__":
    a_cube = o3d.geometry.TriangleMesh.create_box(
        2, 4, 4, create_uv_map=True, map_texture_to_each_face=True
    )
    a_cube.compute_triangle_normals()
    a_cube.translate((-5, 0, -2))
    a_cube = o3d.t.geometry.TriangleMesh.from_legacy(a_cube)

    geoms = [dict(name="cube", geometry=a_cube)]

    vis.draw(
        geoms, bg_color=(0.8, 0.9, 0.9, 1.0), show_ui=True, width=1920, height=1080
    )
