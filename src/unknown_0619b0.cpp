// @flags /O2 /arch:SSE /Gr
#include "unknown_11c920.h"
#include <xtl.h>
#include <xonline.h>
#include "unknown_059ad0.h"
#include "unknown_0662e0.h"
#include <math.h>

inline long count_bits(dword value);

// @retail 0x619b0
bool function_619b0(const s_session_member *members, long second_index, long first_index, long player_count)
{
 const s_session_member *first = &members[first_index];
 const s_session_member *second = &members[second_index];
 long first_bits = count_bits(*(const dword *)first->properties.unknown74);
 long second_bits = count_bits(*(const dword *)second->properties.unknown74);
 long local_1 = first->player_count;
 long local_2 = second->player_count;
 if (first_bits > second_bits)
  goto local_0;
 if (first_bits < second_bits)
  return false;
 if (local_1 != local_2)
 {
  if (local_1 >= local_2)
   return false;
local_0:
  return true;
 }
 {
  long local_3 = *(const long *)(first->properties.unknown74 + 8);
  long maximum = g_network_configuration.value84[player_count];
  long first_value = first->unknown94;
  long second_value = second->unknown94;
  if (first_value > maximum) first_value = maximum;
  if (second_value <= maximum) maximum = second_value;
  real score = (real)(first_value - maximum) * g_network_configuration.real34;
  real delta = (real)(*(const long *)(second->properties.unknown74 + 8) - local_3) * g_network_configuration.real38;
  if (delta > 0.f)
   score += (real)pow(delta, g_network_configuration.real3c);
  else
   score -= (real)pow(-delta, g_network_configuration.real3c);
  if (score > 0.3f)
   goto local_0;
  return false;
 }
}
