/* OBJECT_ATTACHMENTS.H: the part of an object that links it into its
   parent's list of attached children, as object_children_reattach
   (0x13c5a0) reads it */
#ifndef OBJECT_ATTACHMENTS_H
#define OBJECT_ATTACHMENTS_H

#include <stddef.h>
#include "unknown_11c920.h"

/* an object: the next child of its parent, its own first child, and the node
   of the parent it is attached to */
struct s_object_attachment_view
{
	byte unknown00[0xc];
	long next_sibling_index;
	long first_child_index;
	byte unknown14[4];
	signed char attached_node_index;
};
typedef char assert_object_attachment_view_next_sibling[(offsetof(s_object_attachment_view, next_sibling_index) == 0xc) ? 1 : -1];
typedef char assert_object_attachment_view_first_child[(offsetof(s_object_attachment_view, first_child_index) == 0x10) ? 1 : -1];
typedef char assert_object_attachment_view_parent_node[(offsetof(s_object_attachment_view, attached_node_index) == 0x18) ? 1 : -1];

/* an element of the object header data (g_4e0300): the object pointer */
struct s_object_attachment_header
{
	byte unknown00[8];
	s_object_attachment_view *object;
};
typedef char assert_object_attachment_header_size[(sizeof(s_object_attachment_header) == 0xc) ? 1 : -1];

#endif
