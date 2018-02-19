// ======================================================================== //
// Copyright 2009-2018 Intel Corporation                                    //
//                                                                          //
// Licensed under the Apache License, Version 2.0 (the "License");          //
// you may not use this file except in compliance with the License.         //
// You may obtain a copy of the License at                                  //
//                                                                          //
//     http://www.apache.org/licenses/LICENSE-2.0                           //
//                                                                          //
// Unless required by applicable law or agreed to in writing, software      //
// distributed under the License is distributed on an "AS IS" BASIS,        //
// WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied. //
// See the License for the specific language governing permissions and      //
// limitations under the License.                                           //
// ======================================================================== //

#pragma once

#include "subdivpatch1cached.h"
#include "grid_soa.h"
#include "grid_soa_intersector1.h"
#include "grid_soa_intersector_packet.h"
#include "../common/ray.h"

namespace embree
{
  namespace isa
  {
    template<typename T>
      class SubdivPatch1EagerPrecalculations : public T
    { 
    public:
      __forceinline SubdivPatch1EagerPrecalculations (const Ray& ray, const void* ptr)
        : T(ray,ptr) {}
    };

    template<int K, typename T>
      class SubdivPatch1EagerPrecalculationsK : public T
    { 
    public:
      __forceinline SubdivPatch1EagerPrecalculationsK (const vbool<K>& valid, RayK<K>& ray)
        : T(valid,ray) {}
    };

    class SubdivPatch1EagerIntersector1
    {
    public:
      typedef GridSOA Primitive;
      typedef SubdivPatch1EagerPrecalculations<GridSOAIntersector1::Precalculations> Precalculations;

      static __forceinline bool processLazyNode(Precalculations& pre, IntersectContext* context, const Primitive* prim, size_t& lazy_node)
      {
        lazy_node = prim->root(0);
        pre.grid = (Primitive*)prim;
        return false;
      }

      /*! Intersect a ray with the primitive. */
      template<int N, int Nx, bool robust>
        static __forceinline void intersect(const Accel::Intersectors* This, Precalculations& pre, RayHit& ray, IntersectContext* context, const Primitive* prim, size_t ty, const TravRay<N,Nx,robust> &tray, size_t& lazy_node) 
      {
        if (likely(ty == 0)) GridSOAIntersector1::intersect(pre,ray,context,prim,lazy_node);
        else                 processLazyNode(pre,context,prim,lazy_node);
      }

      template<int N, int Nx, bool robust>
      static __forceinline void intersect(const Accel::Intersectors* This, Precalculations& pre, RayHit& ray, IntersectContext* context, size_t ty0, const Primitive* prim, size_t ty, const TravRay<N,Nx,robust> &tray, size_t& lazy_node) {
        intersect(This,pre,ray,context,prim,ty,tray,lazy_node);
      }
      
      /*! Test if the ray is occluded by the primitive */
      template<int N, int Nx, bool robust>
      static __forceinline bool occluded(const Accel::Intersectors* This, Precalculations& pre, Ray& ray, IntersectContext* context, const Primitive* prim, size_t ty, const TravRay<N,Nx,robust> &tray, size_t& lazy_node)
      {
        if (likely(ty == 0)) return GridSOAIntersector1::occluded(pre,ray,context,prim,lazy_node);
        else                 return processLazyNode(pre,context,prim,lazy_node);
      }

      template<int N, int Nx, bool robust>
      static __forceinline bool occluded(const Accel::Intersectors* This, Precalculations& pre, Ray& ray, IntersectContext* context, size_t ty0, const Primitive* prim, size_t ty, const TravRay<N,Nx,robust> &tray, size_t& lazy_node) {
        return occluded(This,pre,ray,context,prim,ty,tray,lazy_node);
      }
    };

    class SubdivPatch1EagerMBIntersector1
    {
    public:
      typedef SubdivPatch1Cached Primitive;
      typedef GridSOAMBIntersector1::Precalculations Precalculations;
      
      static __forceinline bool processLazyNode(Precalculations& pre, Ray& ray, IntersectContext* context, const Primitive* prim_i, size_t& lazy_node)
      {
        Primitive* prim = (Primitive*) prim_i;
        GridSOA* grid = nullptr;
        grid = (GridSOA*) prim->root_ref.get();
        pre.itime = getTimeSegment(ray.time(), float(grid->time_steps-1), pre.ftime);
        lazy_node = grid->root(pre.itime);
        pre.grid = grid;
        return false;
      }

      /*! Intersect a ray with the primitive. */
      template<int N, int Nx, bool robust>
      static __forceinline void intersect(const Accel::Intersectors* This, Precalculations& pre, RayHit& ray, IntersectContext* context, const Primitive* prim, size_t ty, const TravRay<N,Nx,robust> &tray, size_t& lazy_node) 
      {
        if (likely(ty == 0)) GridSOAMBIntersector1::intersect(pre,ray,context,prim,lazy_node);
        else                 processLazyNode(pre,ray,context,prim,lazy_node);
      }

      template<int N, int Nx, bool robust>
      static __forceinline void intersect(const Accel::Intersectors* This, Precalculations& pre, RayHit& ray, IntersectContext* context, size_t ty0, const Primitive* prim, size_t ty, const TravRay<N,Nx,robust> &tray, size_t& lazy_node) {
        intersect(This,pre,ray,context,prim,ty,tray,lazy_node);
      }
      
      /*! Test if the ray is occluded by the primitive */
      template<int N, int Nx, bool robust>
      static __forceinline bool occluded(const Accel::Intersectors* This, Precalculations& pre, Ray& ray, IntersectContext* context, const Primitive* prim, size_t ty, const TravRay<N,Nx,robust> &tray, size_t& lazy_node)
      {
        if (likely(ty == 0)) return GridSOAMBIntersector1::occluded(pre,ray,context,prim,lazy_node);
        else                 return processLazyNode(pre,ray,context,prim,lazy_node);
      }

      template<int N, int Nx, bool robust>
      static __forceinline bool occluded(const Accel::Intersectors* This, Precalculations& pre, Ray& ray, IntersectContext* context, size_t ty0, const Primitive* prim, size_t ty, const TravRay<N,Nx,robust> &tray, size_t& lazy_node) {
        return occluded(This,pre,ray,context,prim,ty,tray,lazy_node);
      }
    };

    template <int K>
      struct SubdivPatch1EagerIntersectorK
    {
      typedef GridSOA Primitive;
      typedef SubdivPatch1EagerPrecalculationsK<K,typename GridSOAIntersectorK<K>::Precalculations> Precalculations;
      
      static __forceinline bool processLazyNode(Precalculations& pre, IntersectContext* context, const Primitive* prim, size_t& lazy_node)
      {
        lazy_node = prim->root(0);
        pre.grid = (Primitive*)prim;
        return false;
      }
      
      static __forceinline void intersect(const vbool<K>& valid, const Accel::Intersectors* This, Precalculations& pre, RayHitK<K>& ray, IntersectContext* context, const Primitive* prim, size_t ty, size_t& lazy_node)
      {
        if (likely(ty == 0)) GridSOAIntersectorK<K>::intersect(valid,pre,ray,context,prim,lazy_node);
        else                 processLazyNode(pre,context,prim,lazy_node);
      }
      
      static __forceinline vbool<K> occluded(const vbool<K>& valid, const Accel::Intersectors* This, Precalculations& pre, RayK<K>& ray, IntersectContext* context, const Primitive* prim, size_t ty, size_t& lazy_node)
      {
        if (likely(ty == 0)) return GridSOAIntersectorK<K>::occluded(valid,pre,ray,context,prim,lazy_node);
        else                 return processLazyNode(pre,context,prim,lazy_node);
      }
      
      template<int N, int Nx, bool robust>              
        static __forceinline void intersect(const Accel::Intersectors* This, Precalculations& pre, RayHitK<K>& ray, size_t k, IntersectContext* context, const Primitive* prim, size_t ty, const TravRay<N,Nx,robust> &tray, size_t& lazy_node)
      {
        if (likely(ty == 0)) GridSOAIntersectorK<K>::intersect(pre,ray,k,context,prim,lazy_node);
        else                 processLazyNode(pre,context,prim,lazy_node);
      }
      
      template<int N, int Nx, bool robust>              
      static __forceinline bool occluded(const Accel::Intersectors* This, Precalculations& pre, RayK<K>& ray, size_t k, IntersectContext* context, const Primitive* prim, size_t ty, const TravRay<N,Nx,robust> &tray, size_t& lazy_node)
      {
        if (likely(ty == 0)) return GridSOAIntersectorK<K>::occluded(pre,ray,k,context,prim,lazy_node);
        else                 return processLazyNode(pre,context,prim,lazy_node);
      }
    };

    typedef SubdivPatch1EagerIntersectorK<4>  SubdivPatch1EagerIntersector4;
    typedef SubdivPatch1EagerIntersectorK<8>  SubdivPatch1EagerIntersector8;
    typedef SubdivPatch1EagerIntersectorK<16> SubdivPatch1EagerIntersector16;

    template <int K>
      struct SubdivPatch1EagerMBIntersectorK
    {
      typedef SubdivPatch1Cached Primitive;
      //typedef GridSOAMBIntersectorK<K>::Precalculations Precalculations;
      typedef SubdivPatch1EagerPrecalculationsK<K,typename GridSOAMBIntersectorK<K>::Precalculations> Precalculations;
      
      static __forceinline bool processLazyNode(Precalculations& pre, IntersectContext* context, const Primitive* prim_i, size_t& lazy_node)
      {
        Primitive* prim = (Primitive*) prim_i;
        GridSOA* grid = (GridSOA*) prim->root_ref.get();
        lazy_node = grid->troot;
        pre.grid = grid;
        return false;
      }

      static __forceinline void intersect(const vbool<K>& valid, const Accel::Intersectors* This, Precalculations& pre, RayHitK<K>& ray, IntersectContext* context, const Primitive* prim, size_t ty, size_t& lazy_node)
      {
        if (likely(ty == 0)) GridSOAMBIntersectorK<K>::intersect(valid,pre,ray,context,prim,lazy_node);
        else                 processLazyNode(pre,context,prim,lazy_node);
      }

      static __forceinline vbool<K> occluded(const vbool<K>& valid, const Accel::Intersectors* This, Precalculations& pre, RayK<K>& ray, IntersectContext* context, const Primitive* prim, size_t ty, size_t& lazy_node)
      {
        if (likely(ty == 0)) return GridSOAMBIntersectorK<K>::occluded(valid,pre,ray,context,prim,lazy_node);
        else                 return processLazyNode(pre,context,prim,lazy_node);
      }

      template<int N, int Nx, bool robust>      
      static __forceinline void intersect(const Accel::Intersectors* This, Precalculations& pre, RayHitK<K>& ray, size_t k, IntersectContext* context, const Primitive* prim, size_t ty, const TravRay<N,Nx,robust> &tray, size_t& lazy_node)
      {
        if (likely(ty == 0)) GridSOAMBIntersectorK<K>::intersect(pre,ray,k,context,prim,lazy_node);
        else                 processLazyNode(pre,context,prim,lazy_node);
      }
      
      template<int N, int Nx, bool robust>      
      static __forceinline bool occluded(const Accel::Intersectors* This, Precalculations& pre, RayK<K>& ray, size_t k, IntersectContext* context, const Primitive* prim, size_t ty, const TravRay<N,Nx,robust> &tray, size_t& lazy_node)
      {
        if (likely(ty == 0)) return GridSOAMBIntersectorK<K>::occluded(pre,ray,k,context,prim,lazy_node);
        else                 return processLazyNode(pre,context,prim,lazy_node);
      }
    };

    typedef SubdivPatch1EagerMBIntersectorK<4>  SubdivPatch1EagerMBIntersector4;
    typedef SubdivPatch1EagerMBIntersectorK<8>  SubdivPatch1EagerMBIntersector8;
    typedef SubdivPatch1EagerMBIntersectorK<16> SubdivPatch1EagerMBIntersector16;
  }
}
