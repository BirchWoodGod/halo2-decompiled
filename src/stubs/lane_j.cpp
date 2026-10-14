// stubs for the game functions lane J's code (0x090000..0x09ffff) calls that
// are not decompiled yet
#include "unknown_11c920.h"


struct s_bitstream;
struct s_network_connection;

/* lane D's region */


/* lane D's region: a view's baseline update (a c_simulation_view method) */
class c_class_58d20;
class c_class_938e0;
struct s_session_id;
struct s_session_member_identity;
struct s_type_99af70;

/* lane D's region: the session handlers the message handler calls */



/* kept out of the build in src/unknown_059ad0.cpp: built there, its session
   moves into a register and the matched 0x94700 no longer matches */
// @stub 0x5efd0
bool __stdcall network_session_handle_player_add(c_class_58d20 *session, long remote_index, void const *message)
{
	return false;
}



/* lane D's region: a connection's update */

/* lane D's region: a connection's reconnect */
/* lane J's, kept out of the build in src/unknown_075800.cpp (they
   change lane D's 0x5a520 convention): the session disband and boot handlers */
// @stub 0x94310
void __stdcall function_094310(c_class_938e0 *handler, s_session_id const *message, s_type_99af70 const *address)
{
}

// @stub 0x94330
void __stdcall function_094330(c_class_938e0 *handler, s_session_id const *message, s_type_99af70 const *address)
{
}

/* lane J's out-of-band handlers still to write, and the session disband
   counterparts kept out of the build (src/unknown_075800.cpp) */
struct s_network_message_session_query;

/* outside lane J: the system link reply and the out-of-band session
   handlers (lane D's region) */


class c_simulation_view;
