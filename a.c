float iluminacao( Vertex v0, Vertex v1, Vertex v2,
                  Vertex L, Vertex V,
                  float ka, float kd, float ks, int n )
{
    Vertex v1_v0 = (Vertex){v1.x - v0.x, v1.y - v0.y, v1.z - v0.z};
    Vertex v2_v0 = (Vertex){v2.x - v0.x, v2.y - v0.y, v2.z - v0.z};

    Vertex normal = (Vertex){
        v1_v0.y * v2_v0.z - v1_v0.z * v2_v0.y,
        v1_v0.z * v2_v0.x - v1_v0.x * v2_v0.z,
        v1_v0.x * v2_v0.y - v1_v0.y * v2_v0.x
    };

    float len = sqrtf(normal.x * normal.x + normal.y * normal.y + normal.z * normal.z);
    if (len != 0.0f) {
        normal.x /= len;
        normal.y /= len;
        normal.z /= len;
    }

    float lenL = sqrtf(L.x*L.x + L.y*L.y + L.z*L.z);
    if (lenL != 0.0f) {
        L.x /= lenL;
        L.y /= lenL;
        L.z /= lenL;
    }

    float lenV = sqrtf(V.x*V.x + V.y*V.y + V.z*V.z);
    if (lenV != 0.0f) {
        V.x /= lenV;
        V.y /= lenV;
        V.z /= lenV;
    }

    float diff = fmaxf(0.0f, normal.x * L.x + normal.y * L.y + normal.z * L.z);

    float dotNL = normal.x * L.x + normal.y * L.y + normal.z * L.z;
    Vertex R = (Vertex){
        2 * dotNL * normal.x - L.x,
        2 * dotNL * normal.y - L.y,
        2 * dotNL * normal.z - L.z
    };

    float dotRV = R.x * V.x + R.y * V.y + R.z * V.z;
    float spec = powf(fmaxf(0.0f, dotRV), n);

    float intensidade = ka + kd * diff + ks * spec;
    if (intensidade > 1.0f) intensidade = 1.0f;
    return intensidade;
}
