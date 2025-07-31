/**
 * \file model.c
 *
 * \brief Implementação das funções de manipulação do modelo 3D.
 *
 * \author
 * Petrucio Ricardo Tavares de Medeiros \n
 * Universidade Federal Rural do Semi-Árido \n
 * Departamento de Engenharias e Tecnologia \n
 * petrucio at ufersa (dot) edu (dot) br
 *
 * \version 1.0
 * \date May 2025
 */

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <math.h>

#include "image.h"
#include "model.h"

int load_obj(const char *filename, Vertex *vertices, int *vcount, Face *faces,
             int *fcount)
{
  FILE *file = fopen(filename, "r");
  if (!file)
  {
    perror("Erro ao abrir o arquivo");
    return 0;
  }

  char line[512];
  *vcount = 0;
  *fcount = 0;

  while (fgets(line, sizeof(line), file))
  {
    if (strncmp(line, "v ", 2) == 0)
    {
      if (sscanf(line + 2, "%f %f %f", &vertices[*vcount].x,
                 &vertices[*vcount].y, &vertices[*vcount].z) == 3)
      {
        (*vcount)++;
      }
    }
    else if (strncmp(line, "f ", 2) == 0)
    {
      Face face = {.n = 0};
      char *token = strtok(line + 2, " \n");
      while (token && face.n < MAX_FACE_VERTS)
      {
        int index;
        if (sscanf(token, "%d", &index) == 1)
        {
          face.verts[face.n++] = index;
        }
        token = strtok(NULL, " \n");
      }
      faces[(*fcount)++] = face;
    }
  }

  fclose(file);
  return 1;
}

void resizing(Vertex v0, Vertex v1)
{
  int x0 = (int)((v0.x + 1.0f) * WIDTH / 2.0f);
  int y0 = (int)((1.0f - v0.y) * HEIGHT / 2.0f);
  int x1 = (int)((v1.x + 1.0f) * WIDTH / 2.0f);
  int y1 = (int)((1.0f - v1.y) * HEIGHT / 2.0f);

  draw_line(x0, y0, x1, y1);
}

void render_faces(Vertex *vertices, Face *faces, int fcount)
{
  for (int i = 0; i < fcount; i++)
  {
    Face face = faces[i];

    Vertex v0 = vertices[face.verts[0] - 1];
    Vertex v1 = vertices[face.verts[1] - 1];
    Vertex v2 = vertices[face.verts[2] - 1];

    // Rotacione os vertices (180 graus)
    rotate_z(&v0, M_PI);
    rotate_z(&v1, M_PI);
    rotate_z(&v2, M_PI);

    // Projecao 3D -> 2D
    project_3dto2d(&v0);
    project_3dto2d(&v1);
    project_3dto2d(&v2);

    barycentric_coordinate(v0, v1, v2, rand() % 255, rand() % 255, rand() % 255);
  }
  Vertex light = {0, 0, -1};   //{0.25, 0.0, -0.75};
  Vertex view_dir = {0, 0, 1}; // Camera olhando para -z

  for (int i = 0; i < fcount; i++)
  {
    Face face = faces[i];

    Vertex v0 = vertices[face.verts[0] - 1];
    Vertex v1 = vertices[face.verts[1] - 1];
    Vertex v2 = vertices[face.verts[2] - 1];

    // Coeficientes de phong
    float ka = 0.2; // ambiente
    float kd = 0.6; // difusa
    float ks = 0.4; // especular
    int brilho = 32;

    float intensity = iluminacao(v0, v1, v2, light, view_dir, ka, kd, ks, brilho);

    // Rotacione os vertices (180 graus)
    rotate_z(&v0, M_PI);
    rotate_z(&v1, M_PI);
    rotate_z(&v2, M_PI);

    // Projecao 3D -> 2D
    project_3dto2d(&v0);
    project_3dto2d(&v1);
    project_3dto2d(&v2);

    barycentric_coordinate(v0, v1, v2, intensity * 255, intensity * 255, intensity * 255);
  }

  for (int i = 0; i < fcount; i++)
  {
    Face face = faces[i];
    for (int j = 0; j < face.n; j++)
    {
      Vertex v0 = vertices[face.verts[j] - 1];
      Vertex v1 = vertices[face.verts[(j + 1) % face.n] - 1];
      resizing(v0, v1);
    }
  }
}

void rotate_z(Vertex *v, float angle_rad)
{
  float x = v->x;
  float y = v->y;
  v->x = x * cosf(angle_rad) - y * sinf(angle_rad);
  v->y = x * sinf(angle_rad) + y * cosf(angle_rad);
}

void project_3dto2d(Vertex *v)
{
  v->x = (v->x + 1.0f) * (WIDTH / 2.0f);
  v->y = (1.0f + v->y) * (HEIGHT / 2.0f);
}

void barycentric_coordinate(Vertex a, Vertex b, Vertex c, float red, float green, float blue)
{
  int x_min = floorf(fminf(fminf(a.x, b.x), c.x));
  int x_max = ceilf(fmaxf(fmaxf(a.x, b.x), c.x));
  int y_min = floorf(fminf(fminf(a.y, b.y), c.y));
  int y_max = ceilf(fmaxf(fmaxf(a.y, b.y), c.y));

  float area_abc = 0.5f * fabsf(
                              a.x * (b.y - c.y) +
                              b.x * (c.y - a.y) +
                              c.x * (a.y - b.y));

  for (int y = y_min; y <= y_max; y++)
  {
    for (int x = x_min; x <= x_max; x++)
    {
      Vertex p = {x, y, 0};

      float area_pbc = 0.5f * fabsf(p.x * (b.y - c.y) + b.x * (c.y - p.y) + c.x * (p.y - b.y));
      float area_apc = 0.5f * fabsf(a.x * (p.y - c.y) + p.x * (c.y - a.y) + c.x * (a.y - p.y));
      float area_abp = 0.5f * fabsf(a.x * (b.y - p.y) + b.x * (p.y - a.y) + p.x * (a.y - b.y));

      float alfa = area_pbc / area_abc;
      float beta = area_apc / area_abc;
      float gamma = area_abp / area_abc;

      if (alfa >= 0.0f && beta >= 0.0f && gamma >= 0.0f && (alfa + beta + gamma <= 1.0001f))
      {
        set_pixel(x, y, red, green, blue);
      }
    }
  }
}

float iluminacao(Vertex v0, Vertex v1, Vertex v2,
                 Vertex L, Vertex V,
                 float ka, float kd, float ks, int n)
{
  Vertex v1_v0 = (Vertex){v1.x - v0.x, v1.y - v0.y, v1.z - v0.z};
  Vertex v2_v0 = (Vertex){v2.x - v0.x, v2.y - v0.y, v2.z - v0.z};

  Vertex normal = (Vertex){
      v1_v0.y * v2_v0.z - v1_v0.z * v2_v0.y,
      v1_v0.z * v2_v0.x - v1_v0.x * v2_v0.z,
      v1_v0.x * v2_v0.y - v1_v0.y * v2_v0.x};

  float len = sqrtf(normal.x * normal.x + normal.y * normal.y + normal.z * normal.z);
  if (len != 0.0f)
  {
    normal.x /= len;
    normal.y /= len;
    normal.z /= len;
  }

  float lenL = sqrtf(L.x * L.x + L.y * L.y + L.z * L.z);
  if (lenL != 0.0f)
  {
    L.x /= lenL;
    L.y /= lenL;
    L.z /= lenL;
  }

  float lenV = sqrtf(V.x * V.x + V.y * V.y + V.z * V.z);
  if (lenV != 0.0f)
  {
    V.x /= lenV;
    V.y /= lenV;
    V.z /= lenV;
  }

  float diff = fmaxf(0.0f, normal.x * L.x + normal.y * L.y + normal.z * L.z);

  float dotNL = normal.x * L.x + normal.y * L.y + normal.z * L.z;
  Vertex R = (Vertex){
      2 * dotNL * normal.x - L.x,
      2 * dotNL * normal.y - L.y,
      2 * dotNL * normal.z - L.z};

  float dotRV = R.x * V.x + R.y * V.y + R.z * V.z;
  float spec = powf(fmaxf(0.0f, dotRV), n);

  float intensidade = ka + kd * diff + ks * spec;
  if (intensidade > 1.0f)
    intensidade = 1.0f;
  return intensidade;
}
