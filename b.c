void barycentric_coordinate(Vertex a, Vertex b, Vertex c, float red, float green, float blue) {
  // Calcula o bounding box do triângulo
  int x_min = floorf(fminf(fminf(a.x, b.x), c.x));
  int x_max = ceilf(fmaxf(fmaxf(a.x, b.x), c.x));
  int y_min = floorf(fminf(fminf(a.y, b.y), c.y));
  int y_max = ceilf(fmaxf(fmaxf(a.y, b.y), c.y));

  float area_abc = 0.5f * fabsf(
    a.x*(b.y - c.y) +
    b.x*(c.y - a.y) +
    c.x*(a.y - b.y)
  );

  for (int y = y_min; y <= y_max; y++) {
    for (int x = x_min; x <= x_max; x++) {
      Vertex p = {x, y, 0};

      float area_pbc = 0.5f * fabsf(p.x*(b.y - c.y) + b.x*(c.y - p.y) + c.x*(p.y - b.y));
      float area_apc = 0.5f * fabsf(a.x*(p.y - c.y) + p.x*(c.y - a.y) + c.x*(a.y - p.y));
      float area_abp = 0.5f * fabsf(a.x*(b.y - p.y) + b.x*(p.y - a.y) + p.x*(a.y - b.y));

      float alfa = area_pbc / area_abc;
      float beta = area_apc / area_abc;
      float gamma = area_abp / area_abc;

      if (alfa >= 0.0f && beta >= 0.0f && gamma >= 0.0f && (alfa + beta + gamma <= 1.0001f)) {
        set_pixel(x, y, red, green, blue);
      }
    }
  }
}
