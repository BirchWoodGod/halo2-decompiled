#include "unknown_11c920.h"
#include "slot_handler.h"
#include "globals.h"
// @flags /O2 /Gr

struct s_1c7bb0
{
 byte field_0[2];
 char field_2;
 byte field_3[0x28 - 3];
 short field_28;
 byte field_2a[0x68 - 0x2a];
 long field_68;
 byte field_6c[0x7e - 0x6c];
 short field_7e;
 byte field_80[0x98 - 0x80];
};
bool function_204390(long squad_index);
bool function_1e1500(long actor_index);
void function_203ed0(long actor_index, bool keep_count);
void function_1e22d0(long actor_index, bool conditional);
void function_203cc0(long squad_index);
bool function_1e13f0(long actor_index);
void function_203fb0(long actor_index);
void function_203d70(long actor_index, short squad_index, bool keep_team);
void function_295e60(short arg_0);
void function_210280();
void function_292f10();

PRIVATE __forceinline byte *function_1c7bb1(s_record_pool_iterator *arg_0)
{
 byte *result = NULL;
 if (g_4f55d0->active)
  result = data_iterator_next_calling(arg_0);
 return result;
}

// @retail 0x1c7bb0
void function_1c7bb0()
{
 if (g_4f55d0->active)
 {
  short local_0 = g_4686c4;
  s_record_pool_iterator local_1;
  byte *datum;
  local_1.data = g_51e9d8;
  local_1.index = NONE;
  while ((datum = function_1c7bb1(&local_1)) != NULL)
  {
   long local_2 = local_1.datum_index & 0xffff;
   s_1c7bb0 *local_3 = &((s_1c7bb0 *)g_51e9d8->data)[local_2 & 0xffff];
   if (function_204390(local_2))
   {
    local_3->field_7e = (short)local_0;
    long local_4 = NONE;
    if (g_4f55d0->active)
     local_4 = local_2 == NONE ? g_4f55d0->unknown14 : local_3->field_68;
    while (g_4f55d0->active && local_4 != NONE)
    {
     long local_5 = local_4;
     s_actor_view *local_6 = actor_get(local_5);
     local_4 = local_6->unknown020;
     if (!function_1e1500(local_5))
     {
      *(long *)((byte *)local_6 + 0x34) = local_6->unknown030;
      function_203ed0(local_5, false);
      if (g_4f55d0->active)
      {
       local_6 = actor_get(local_5);
       local_6->unknown020 = g_4f55d0->unknown14;
       g_4f55d0->unknown14 = local_5;
       *((bool *)local_6 + 0xa) = true;
       *(long *)((byte *)local_6 + 0x14) = g_510c54->game_time;
       function_1e22d0(local_5, false);
      }
     }
    }
   }
   else
    local_3->field_7e = NONE;
   if (((s_1c7bb0 *)datum)->field_2 < 0)
   {
    ((s_1c7bb0 *)g_51e9d8->data)[local_2 & 0xffff].field_28 = 0;
    function_203cc0(local_2);
   }
  }
  long local_4 = g_4f55d0->unknown14;
  while (local_4 != NONE)
  {
   long local_5 = local_4;
   s_actor_view *local_6 = actor_get(local_5);
   local_4 = local_6->unknown020;
   if (function_1e13f0(local_5) && function_1e1500(local_5))
   {
    long local_7 = *(long *)((byte *)local_6 + 0x34);
    if (local_7 != NONE && ((s_1c7bb0 *)g_51e9d8->data)[local_7 & 0xffff].field_7e == (short)local_0)
    {
     function_203fb0(local_5);
     function_203d70(local_5, (short)*(long *)((byte *)local_6 + 0x34), true);
    }
   }
   local_6 = actor_get(local_5);
   if (local_6->unknown009)
   {
    local_6->unknown009 = false;
    *(long *)((byte *)local_6 + 0x10) = g_510c54->game_time;
    --*(short *)((byte *)g_4f55d0 + 0x36a);
   }
  }
  function_295e60(local_0);
  function_210280();
  function_292f10();
 }
}
