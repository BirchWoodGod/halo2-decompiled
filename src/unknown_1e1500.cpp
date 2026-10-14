// @flags /O2 /arch:SSE /Gr
#include "unknown_11c920.h"
#include "ai_actor.h"

void function_1e23c0(long actor_index);
void function_1e22d0(long actor_index, bool conditional);
bool function_1e13f0(long actor_index);
void function_26dc20(long actor_index);
struct s_location;
void function_11bed0(s_location *location, point3f const *point);
struct s_bsp3d;
extern s_bsp3d *g_4e033c;
long function_14a280(s_bsp3d *bsp, long index, point3f *point);

struct s_actor_leaf_view
{
 short cluster;
 byte field_2[6];
};
struct s_actor_bsp_view
{
 byte field_0[0x30];
 s_actor_leaf_view *leaves;
};

// @retail 0x1e1500
bool function_1e1500(long actor_index)
{
 const long *index_reference = &actor_index;
 s_actor_view *actor = actor_get(*index_reference);
 function_1e23c0(*index_reference);
 function_1e22d0(*index_reference, false);
 s_actor_view *state = actor_get(*index_reference);
 if (*(long *)((byte *)state + 0x3f8) != NONE)
 {
  if (*(long *)((byte *)state + 0x3f4) != NONE)
   function_26dc20(*index_reference);
  *(long *)((byte *)state + 0x3f8) = NONE;
  *(short *)((byte *)state + 0x3fc) = 0;
 }
 bool result = function_1e13f0(*index_reference);
 bool const *result_reference = &result;
 if (*result_reference)
 {
  *(short *)((byte *)actor + 0x3e) = g_4686c4;
  point3f point;
  point.x = *(real *)((byte *)actor + 0x238) + g_4687b0->i * 0.1f;
  point.y = *(real *)((byte *)actor + 0x23c) + g_4687b0->j * 0.1f;
  point.z = *(real *)((byte *)actor + 0x240) + g_4687b0->k * 0.1f;
  function_11bed0((s_location *)((byte *)actor + 0x250), &point);
  long prop_index = actor_get(*index_reference)->first_prop_index;
  struct prop_step
  {
   static __forceinline s_prop_node_view *next(long *cursor)
   {
    long index = *cursor;
    if (index == NONE) return NULL;
    s_prop_node_view *node = (s_prop_node_view *)(g_502418->data + (index & 0xffff) * 0x3c);
    *cursor = node->next_index;
    return node;
   }
  };
  s_prop_node_view *prop;
  while ((prop = prop_step::next(&prop_index)) != NULL)
  {
   byte *block = (byte *)function_25d690((s_prop_datum *)prop);
   byte *view = NULL;
   if (prop->view_index != NONE)
   {
    byte *entry = g_502414->data + (prop->view_index & 0xffff) * 0x124;
    if (entry) view = entry + 0x70;
   }
   point.x = *(real *)(block + 4) + g_4687b0->i * 0.1f;
   point.y = *(real *)(block + 8) + g_4687b0->j * 0.1f;
   point.z = *(real *)(block + 12) + g_4687b0->k * 0.1f;
   short bsp_index = g_4686c4;
   if (bsp_index == NONE)
   {
    *(long *)(block + 0x28) = NONE;
    *(short *)(block + 0x2c) = NONE;
   }
   else
   {
    long leaf = function_14a280(g_4e033c, 0, &point);
    *(long *)(block + 0x28) = leaf;
    *(short *)(block + 0x2c) = leaf == NONE ? NONE : ((s_actor_bsp_view *)g_4e0348)->leaves[leaf].cluster;
   }
   *(short *)(block + 0x2e) = bsp_index;
   *(long *)(block + 0x44) = NONE;
   if (view && *(short *)(view + 0x70) == 1)
   {
    view[0x68] = false;
    view[0x69] = false;
    view[0x4c] = false;
    *(short *)(view + 0x70) = 0;
    view[0x6c] = true;
    view[0x6d] = true;
    view[0x88] = false;
    *(short *)(view + 0x90) = 0;
    *(short *)(view + 0x8a) = 0;
   }
  }
 }
 return *result_reference;
}
