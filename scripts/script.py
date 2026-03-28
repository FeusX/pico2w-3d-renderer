import sys
import os

def convert_obj_to_header(obj_file):
    name = os.path.splitext(os.path.basename(obj_file))[0]
    vertices = []
    edges = set()
    faces = []

    with open(obj_file, 'r') as f:
        for line in f:
            parts = line.split()
            if not parts: continue
            
            if parts[0] == 'v':
                vertices.append([float(parts[1]), float(parts[2]), float(parts[3])])
            
            elif parts[0] == 'f':
                v_indices = [int(p.split('/')[0]) - 1 for p in parts[1:]]
                
                for i in range(len(v_indices)):
                    v1, v2 = v_indices[i], v_indices[(i + 1) % len(v_indices)]
                    edges.add(tuple(sorted((v1, v2))))
                
                if len(v_indices) >= 3:
                    for i in range(1, len(v_indices) - 1):
                        faces.append((v_indices[0], v_indices[i], v_indices[i+1]))

    with open(f"{name}.h", 'w') as out:
        header_guard = f"{name.upper()}_MODEL_H"
        out.write(f"#ifndef {header_guard}\n#define {header_guard}\n\n")
        out.write('#include "../utils/type.h"\n\n')
        
        out.write(f"static const vec3_t {name}_verts[] = {{\n")
        for v in vertices:
            out.write(f"    {{ {v[0]}f, {v[1]}f, {v[2]}f }},\n")
        out.write("};\n\n")
        
        out.write(f"static const uint16_t {name}_edges[] = {{\n")
        for e in sorted(list(edges)):
            out.write(f"    {e[0]}, {e[1]},\n")
        out.write("};\n\n")

        out.write(f"static const uint16_t {name}_faces[] = {{\n")
        for f in faces:
            out.write(f"    {f[0]}, {f[1]}, {f[2]},\n")
        out.write("};\n\n")
        
        out.write(f"static const mesh_t {name}_mesh = {{\n")
        out.write(f"    .vertex_count = {len(vertices)},\n")
        out.write(f"    .vertices = {name}_verts,\n")
        out.write(f"    .edge_count = {len(edges)},\n")
        out.write(f"    .edges = {name}_edges,\n")
        out.write(f"    .face_count = {len(faces)},\n")
        out.write(f"    .faces = {name}_faces\n")
        out.write("};\n\n#endif")

if __name__ == "__main__":
    if len(sys.argv) > 1:
        convert_obj_to_header(sys.argv[1])
    else:
        print("Usage: python convert.py model.obj")
