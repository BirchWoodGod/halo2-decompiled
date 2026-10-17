#ifndef OBJECTIVES_H
#define OBJECTIVES_H

/* MISSION_OBJECTIVES.H: the campaign's five objectives, as the pause screen lists
   them (an empty box when shown, a checked box when completed). Their
   states live in the game state at g_502248; the scripts show and complete
   them up to an index. */

enum e_objective_state
{
	_objective_state_hidden,
	_objective_state_shown,
	_objective_state_completed,

	k_objective_state_count
};

enum
{
	k_objective_count = 5
};

void objectives_initialize();
void objectives_show_up_to(long last_index);
long objectives_get_state(long index);
long objectives_get_string_id(long index);

#endif
