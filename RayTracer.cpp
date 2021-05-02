#include <limits>
#include "RayTracer.h"
#include "Material.h"
#include "Geometry.h"


//Базовый алгоритм трассировки луча
float3 SimpleRT::TraceRay(const Ray & ray, const std::vector<std::shared_ptr<GeoObject>> &geo, const int &depth)
{
  float tnear = std::numeric_limits<float>::max();
  int   geoIndex = -1;

  SurfHit surf;

  //среди геометрии сцены ищем объекты, с которыми пересекается текущий луч и находим ближайшее пересечение
  for (int i = 0; i < geo.size(); ++i)
  {
    SurfHit temp;

    if (geo.at(i)->Intersect(ray, 0.001, tnear, temp))
    {
      if (temp.t < tnear)
      {
        tnear = temp.t;
        geoIndex = i;
        surf = temp;
      }
    }
  }


  //если луч не пересек ни один объект, то значит он улетел в фон
  //вычисляем цвет как градиент цвета фона
  if (geoIndex == -1)
  {
    float3 unit_direction = normalize(ray.d);
    float t = 0.5f * (unit_direction.y + 1.0f);

    return (1.0f - t) * float3(1.0f, 1.0f, 1.0f) + t * bg_color;
  }

  float3 surfColor(0.0f, 0.0f, 0.0f);
  if (dot(ray.d, surf.normal) > 0)
  {
    surf.normal = -surf.normal;
  }


  Ray scattered;//Луч, который отдалился от нас 
  if(depth < max_ray_depth && surf.m_ptr->Scatter(ray, surf, surfColor, scattered))
  {
    return surfColor * TraceRay(scattered, geo, depth + 1);
  }
  else
  {
      return surfColor;
  }
}


