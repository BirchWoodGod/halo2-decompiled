// @flags /O2 /arch:SSE /Gr
/* OBJECT_ATTACHMENTS.CPP: moving the children attached to one of an object's
   nodes onto a marker (script function 874: an object and three
   name identifiers) */

#include "object_masks_storage.h"
#include "unknown_11c920.h"
#include "globals.h"
#include "object_markers.h"
#include "object_attachments.h"

short function_b8d30(long object_index, long marker_name, s_object_marker *markers, short count, bool flag);
void function_b9a90(long object_index);
void __stdcall function_b8ee0(long parent_index, long parent_marker_name, long index, long child_marker_name);

/* every child attached to the node of object_index's marker marker_name is
   detached and attached again, at parent_marker_name of object_index and its
   own child_marker_name; the object masks mark the children already moved */
// @retail 0x13c5a0
void object_children_reattach(long object_index, long marker_name, long parent_marker_name, long child_marker_name)
{
	s_object_marker marker;
	if (object_index != NONE && function_b8d30(object_index, marker_name, &marker, 1, false) == 1)
	{
		long child_index = ((s_object_attachment_header *)g_4e0300->data)[object_index & 0xffff].object->first_child_index;
		g_4de2fc++;
		g_4de2f8 = true;
		while (child_index != NONE)
		{
			s_object_attachment_view *child = ((s_object_attachment_header *)g_4e0300->data)[child_index & 0xffff].object;
			long next_index = child->next_sibling_index;
			if (g_4de300[child_index & 0xffff] != g_4de2fc)
			{
				g_4de300[child_index & 0xffff] = g_4de2fc;
				if (child->attached_node_index == marker.node_index)
				{
					function_b9a90(child_index);
					function_b8ee0(object_index, parent_marker_name, child_index, child_marker_name);
				}
			}
			child_index = next_index;
		}
		g_4de2f8 = false;
	}
}
