// @flags /O2 /arch:SSE /Gr
/* UNKNOWN_17D9A0.CPP: decal placement and rendering (0x17d9a0 onwards) */

#include "unknown_11c920.h"
#include "unknown_0259d0.h"
#include <float.h>
#include "globals.h"
#include <string.h>
#include <math.h>

extern long g_4e7414;
extern bool g_4e7411;
short __stdcall function_14a5b0(short cluster_index, point3f const *point, real radius, long maximum_count, short *clusters);

struct s_decal_cluster_17e490
{
	byte unknown00[0x98];
	long count;
	short *indices;
	byte unknowna0[0x10];
};

struct s_decal_bound_17e490
{
	byte unknown00[0x3c];
	point3f center;
	real radius;
	byte unknown4c[0xc];
};

struct s_decal_bsp_17e490
{
	byte unknown00[0xa0];
	s_decal_cluster_17e490 *clusters;
	byte unknowna4[0xa0];
	s_decal_bound_17e490 *bounds;
};

// @retail 0x17e490
long function_17e490(long excluded, short cluster_index, point3f const *point, real radius, long maximum_count, s_decal_bound_17e490 **output)
{
	short clusters[512];
	short count = 0;
	if (cluster_index != NONE)
	{
		if (radius > 0.0f)
		{
			g_4e7414++;
			g_4e7411 = true;
			count = function_14a5b0(cluster_index, point, radius, 512, clusters);
			g_4e7411 = false;
			if (count > 512)
				count = 512;
		}
		else
		{
			clusters[0] = cluster_index;
			count = 1;
		}
	}
	dword visited[32];
	memset(visited, 0, sizeof(visited));
	long result = 0;
	if (excluded != NONE)
		visited[excluded >> 5] |= 1 << (excluded & 31);
	for (long i = 0; i < count; ++i)
	{
		s_decal_cluster_17e490 *cluster = &((s_decal_bsp_17e490 *)g_4e0348)->clusters[clusters[i]];
		for (long j = 0; j < cluster->count; ++j)
		{
			long index = cluster->indices[j];
			dword bit = 1 << (index & 31);
			if (!(visited[index >> 5] & bit) && result < maximum_count)
			{
				s_decal_bound_17e490 *bound = &((s_decal_bsp_17e490 *)g_4e0348)->bounds[index];
				real x = point->x - bound->center.x;
				real z = point->z - bound->center.z;
				real y = point->y - bound->center.y;
				real distance = x * x + z * z + y * y;
				real total_radius = bound->radius + radius;
				if (total_radius * total_radius >= distance)
				{
					output[result++] = bound;
					visited[index >> 5] |= bit;
				}
			}
		}
	}
	return result;
}

struct s_decal_mesh_face
{
	short plane_index;
	word first_edge;
	byte unknown04[4];
};

struct s_decal_quad_17d9a0
{
	transform4x3f matrix;
	box2f bounds;
	plane3f plane;
	short axis;
	bool positive;
	byte unknown57;
	point2f corners[4];
	struct s_edge { real i, j; } edges[2];
	real inverse_determinant;
};

short function_120850(vector3f const *v);

PRIVATE inline void decal_project_corner_17d9a0(transform4x3f const *matrix, real const &x, real const &y, s_decal_quad_17d9a0 *quad, point2f *out)
{
	point3f point;
	point.x = matrix->forward.i * x + matrix->left.i * y + matrix->position.x;
	point.y = matrix->left.j * y + matrix->forward.j * x + matrix->position.y;
	point.z = matrix->forward.k * x + matrix->left.k * y + matrix->position.z;
	short const *axes = g_440b94[quad->axis * 2 + quad->positive];
	out->x = ((real const *)&point)[axes[0]];
	out->y = ((real const *)&point)[axes[1]];
}

// @retail 0x17d9a0
void function_17d9a0(transform4x3f const *matrix, real const *bounds, s_decal_quad_17d9a0 *quad)
{
	s_decal_quad_17d9a0 *const *quad_reference = &quad;
	quad = *quad_reference;
	quad->matrix = *matrix;
	quad->bounds = *(box2f const *)bounds;
	plane3f *plane = &quad->plane;
	plane->n = matrix->up;
	plane->d = dot3f(&plane->n, (vector3f const *)&matrix->position);
	quad->axis = function_120850(&plane->n);
	quad->positive = ((real const *)&quad->plane.n)[quad->axis] > 0.0f;
	decal_project_corner_17d9a0(matrix, bounds[0], bounds[2], quad, &quad->corners[0]);
	decal_project_corner_17d9a0(matrix, bounds[1], bounds[2], quad, &quad->corners[1]);
	decal_project_corner_17d9a0(matrix, bounds[1], bounds[3], quad, &quad->corners[2]);
	decal_project_corner_17d9a0(matrix, bounds[0], bounds[3], quad, &quad->corners[3]);
	quad->edges[0].i = quad->corners[1].x - quad->corners[0].x;
	quad->edges[0].j = quad->corners[1].y - quad->corners[0].y;
	quad->edges[1].i = quad->corners[3].x - quad->corners[0].x;
	quad->edges[1].j = quad->corners[3].y - quad->corners[0].y;
	if (quad->edges[0].i * quad->edges[0].i + quad->edges[0].j * quad->edges[0].j > 0.0001f &&
		quad->edges[1].i * quad->edges[1].i + quad->edges[1].j * quad->edges[1].j > 0.0001f)
		quad->inverse_determinant = 1.0f / (quad->edges[1].j * quad->edges[0].i - quad->edges[0].j * quad->edges[1].i);
	else
		quad->inverse_determinant = 0.0f;
}

struct s_decal_mesh_edge
{
	word vertices[2];
	word next_edges[2];
	short faces[2];
};

struct s_decal_mesh_vertex
{
	point3f position;
	byte unknown0c[4];
};

struct s_decal_mesh_view
{
	byte unknown00[0xc];
	plane3f const *planes;
	byte unknown10[0x18];
	long face_count;
	s_decal_mesh_face const *faces;
	long edge_count;
	s_decal_mesh_edge const *edges;
	long vertex_count;
	s_decal_mesh_vertex const *vertices;
};

// @retail 0x17d900
real function_17d900(s_decal_mesh_view const *mesh, long face_index, plane3f const *plane)
{
	s_decal_mesh_face const *face = &mesh->faces[face_index];
	real minimum = FLT_MAX;
	long edge_index = face->first_edge;
	s_decal_mesh_face const *first_face = face;

	do
	{
		s_decal_mesh_edge const *edge = &mesh->edges[edge_index];
		bool reverse = edge->faces[1] == face_index;
		real distance = (real)fabs(plane_distance_to_point(plane, &mesh->vertices[edge->vertices[!reverse]].position));

		if (!(distance > minimum))
			minimum = distance;
		edge_index = edge->next_edges[reverse];
	} while (edge_index != first_face->first_edge);
	return minimum;
}

struct s_decal_vertex_17dd80
{
	point3f position;
	point2f texture;
};

struct s_decal_output_17dd80
{
	s_decal_vertex_17dd80 vertices[1024];
	short vertex_count;
	short polygon_sizes[1024];
	short polygon_count;
};

struct s_decal_limits_17dd80
{
	real maximum_angle;
	real adjacent_angle;
	real radius_scale;
	long flags;
};

PRIVATE s_decal_limits_17dd80 const g_444a38[4] =
{
	{ 40.0f, 110.0f, 1.5f, 1 },
	{ 40.0f, 110.0f, 1.5f, 1 },
	{ 40.0f, 110.0f, 1.5f, 1 },
	{ 10.0f, 10.0f, 1.5f, 0 }
};
point2f g_55ec10[2][12];

struct s_bsp3d;
struct vector2f { real i, j; };
struct plane2f { vector2f n; real d; };
plane3f *bsp3d_get_plane(s_bsp3d const *bsp, short plane_index, plane3f *plane);
real function_11cf50(vector3f const *a, vector3f const *b);
bool function_11e5e0(point3f const *origin, point3f const *center, vector3f const *direction, real radius);
real *function_120790(real *out, real const *plane, real const *point, short axis, byte side);
short function_23a6f0(point2f *output, short count, point2f const *points,
	plane2f const *plane, short capacity, dword *point_mask, bool *clipped, real epsilon);

PRIVATE __forceinline void decal_add_adjacent_17dd80(s_decal_mesh_view const *mesh, s_decal_mesh_edge const *edge,
	bool reverse, s_decal_quad_17d9a0 const *quad, real radius, short mode, long *adjacent, short *count)
{
	if (*count < 1024)
	{
		point3f const *start = &mesh->vertices[edge->vertices[!reverse]].position;
		point3f const *end = &mesh->vertices[edge->vertices[reverse]].position;
		vector3f direction;
		direction.i = end->x - start->x;
		direction.j = end->y - start->y;
		direction.k = end->z - start->z;
		if (function_11e5e0(&quad->matrix.position, start, &direction, g_444a38[mode].radius_scale * radius))
		{
			long face = edge->faces[!reverse];
			short i = 0;
			while (face != NONE && i < *count)
			{
				if (adjacent[i] == face)
					face = NONE;
				++i;
			}
			if (face != NONE)
				adjacent[(*count)++] = face;
		}
	}
}

// @retail 0x17dd80
void __stdcall function_17dd80(s_decal_mesh_view const *mesh, s_decal_quad_17d9a0 const *quad,
	s_decal_output_17dd80 *output, long face_index, bool gather, real radius, short mode,
	long *adjacent, short *adjacent_count, long *secondary, short *secondary_count)
{
	if (face_index == NONE)
		return;
	s_decal_mesh_face const *face = &mesh->faces[face_index];
	short count, other_count;
	if (gather)
	{
		count = *adjacent_count;
		other_count = *secondary_count;
	}
	plane3f plane;
	bsp3d_get_plane((s_bsp3d const *)mesh, face->plane_index, &plane);
	real angle = function_11cf50(&quad->plane.n, &plane.n);
	if (g_444a38[mode].maximum_angle * 0.017453292f >= angle &&
		radius * 0.05f > function_17d900(mesh, face_index, &quad->plane))
	{
		long edge_index = face->first_edge;
		short iteration = 0;
		point2f const *points = quad->corners;
		short point_count = 4;
		dword point_mask = 0;
		point2f previous;
		point2f *polygon_points;
		do
		{
			s_decal_mesh_edge const *edge = &mesh->edges[edge_index];
			bool reverse = edge->faces[1] == face_index;
			point3f const *vertex = &mesh->vertices[edge->vertices[!reverse]].position;
			polygon_points = g_55ec10[iteration & 1];
			short const *axes = g_440b94[quad->axis * 2 + quad->positive];
			if (iteration == 0)
			{
				point3f const *first = &mesh->vertices[edge->vertices[reverse]].position;
				previous.x = ((real const *)first)[axes[0]];
				previous.y = ((real const *)first)[axes[1]];
			}
			point2f current;
			current.x = ((real const *)vertex)[axes[0]];
			current.y = ((real const *)vertex)[axes[1]];
			plane2f clip;
			clip.n.i = previous.y - current.y;
			clip.n.j = current.x - previous.x;
			real length = (real)sqrt(clip.n.i * clip.n.i + clip.n.j * clip.n.j);
			if (!(fabs(length) < 0.0001f))
			{
				real inverse = 1.0f / length;
				clip.n.i *= inverse;
				clip.n.j *= inverse;
			}
			else
				length = 0.0f;
			if (length != 0.0f)
			{
				clip.d = clip.n.j * current.y + clip.n.i * current.x;
				bool clipped;
				point_count = function_23a6f0(polygon_points, point_count, points, &clip, 12, &point_mask, &clipped, 0.0f);
				if (gather && clipped)
					decal_add_adjacent_17dd80(mesh, edge, reverse, quad, radius, mode, adjacent, &count);
			}
			else
			{
				clip.d = 0.0f;
				point_count = 0;
			}
			++iteration;
			previous = current;
			edge_index = edge->next_edges[reverse];
			points = polygon_points;
		} while (edge_index != face->first_edge && point_count > 0);
		if (point_count >= 3 && point_count <= 1024 - output->vertex_count && !(face->unknown04[0] & 0x2b))
		{
			output->polygon_sizes[output->polygon_count++] = point_count;
			for (short i = 0; i < point_count; ++i)
			{
				real x = polygon_points[i].x - quad->corners[0].x;
				real y = polygon_points[i].y - quad->corners[0].y;
				real v = 0.0f - (quad->edges[0].j * x - quad->edges[0].i * y) * quad->inverse_determinant;
				real u = (x * quad->edges[1].j - quad->edges[1].i * y) * quad->inverse_determinant;
				output->vertices[output->vertex_count].texture.x = u;
				output->vertices[output->vertex_count].texture.y = v;
				function_120790((real *)&output->vertices[output->vertex_count].position,
					(real const *)&plane, (real const *)&polygon_points[i], quad->axis, quad->positive);
				if (!(point_mask & (1 << i)))
				{
					point3f *position = &output->vertices[output->vertex_count].position;
					position->x += plane.n.i * 0.00390625f;
					position->y += plane.n.j * 0.00390625f;
					position->z += plane.n.k * 0.00390625f;
				}
				++output->vertex_count;
			}
		}
	}
	else
	{
		if (!gather)
			return;
		long edge_index = face->first_edge;
		do
		{
			s_decal_mesh_edge const *edge = &mesh->edges[edge_index];
			bool reverse = edge->faces[1] == face_index;
			decal_add_adjacent_17dd80(mesh, edge, reverse, quad, radius, mode, adjacent, &count);
			edge_index = edge->next_edges[reverse];
		} while (edge_index != face->first_edge);
		if (g_444a38[mode].adjacent_angle * 0.017453292f >= angle && other_count < 1024)
			secondary[other_count++] = face_index;
	}
	if (gather)
	{
		*adjacent_count = count;
		*secondary_count = other_count;
	}
}

/* the state a decal is placed with (0x5c bytes) */
struct s_decal_placement
{
	long unknown00;
	long unknown04;
	point3f position;
	long unknown14;
	long unknown18;
	long unknown1c;
	long unknown20;
	short unknown24;
	byte unknown26[2];
	plane3f plane;
	long unknown38;
	long unknown3c;
	long unknown40;
	short unknown44;
	short unknown46;
	long unknown48;
	long unknown4c;
	long unknown50;
	long unknown54;
	byte unknown58;
	byte unknown59;
	short unknown5a;
};

// @retail 0x17ed70
void decal_placement_copy(s_decal_placement const *in, s_decal_placement *out)
{
	out->unknown00 = in->unknown00;
	out->unknown04 = in->unknown04;
	out->position = in->position;
	out->unknown14 = in->unknown14;
	out->unknown18 = in->unknown18;
	out->unknown1c = in->unknown1c;
	out->unknown20 = in->unknown20;
	out->unknown24 = in->unknown24;
	out->plane = in->plane;
	out->unknown38 = in->unknown38;
	out->unknown3c = in->unknown3c;
	out->unknown40 = in->unknown40;
	out->unknown44 = in->unknown44;
	out->unknown46 = in->unknown46;
	out->unknown48 = in->unknown48;
	out->unknown4c = in->unknown4c;
	out->unknown50 = in->unknown50;
	out->unknown54 = in->unknown54;
	out->unknown58 = in->unknown58;
	out->unknown59 = in->unknown59;
	out->unknown5a = in->unknown5a;
}

struct s_decal_projection_17fd20
{
	s_decal_quad_17d9a0 quad;
	real orientation_bounds[6];
	real radius;
	transform4x3f matrix;
	box2f bounds;
	box2f texture_bounds;
	short sequence_index;
	short frame_index;
	short bitmap_index;
	short unknown102;
	byte *bitmap_group;
	byte *sequence;
	byte *frame;
};

PRIVATE __forceinline void decal_face_plane_17fd20(s_decal_mesh_view const *mesh, long face_index, plane3f *plane)
{
	short index = mesh->faces[face_index].plane_index;
	plane3f const *source = &mesh->planes[index & 0x7fff];
	if (index < 0)
	{
		plane->n.i = 0.0f - source->n.i;
		plane->n.j = 0.0f - source->n.j;
		plane->n.k = 0.0f - source->n.k;
		plane->d = 0.0f - source->d;
	}
	else
		*plane = *source;
}

PRIVATE __forceinline real decal_normal_angle_17fd20(vector3f const *a, vector3f const *b)
{
	if (!memcmp(a, b, sizeof(*a)))
		return 0.0f;
	real cosine = a->i * b->i + a->k * b->k + a->j * b->j;
	cosine = cosine < -1.0f ? -1.0f : cosine > 1.0f ? 1.0f : cosine;
	return (real)acos(cosine < -1.0f ? -1.0f : cosine > 1.0f ? 1.0f : cosine);
}

PRIVATE __forceinline void decal_rotate_vector_17fd20(vector3f const *forward, vector3f const *left,
	vector3f const *up, vector3f const *in, vector3f *out)
{
	out->i = up->i * in->k + left->i * in->j + forward->i * in->i;
	out->j = up->j * in->k + left->j * in->j + forward->j * in->i;
	out->k = up->k * in->k + left->k * in->j + forward->k * in->i;
}

// @retail 0x17fd20
void function_17fd20(s_decal_mesh_view const *mesh, long tag_index,
	s_decal_output_17dd80 *output, s_decal_projection_17fd20 *projection, s_decal_placement const *placement)
{
	s_decal_mesh_view const *const *mesh_reference = &mesh;
	long const *tag_index_alias = &tag_index;
	s_decal_output_17dd80 *const *output_reference = &output;
	s_decal_projection_17fd20 *const *projection_reference = &projection;
	mesh = *mesh_reference;
	tag_index = *tag_index_alias;
	output = *output_reference;
	projection = *projection_reference;
	byte const *definition = g_4e3b44[tag_index & 0xffff].bytes;
	output->polygon_count = 0;
	output->vertex_count = 0;
	long faces[1024];
	long secondary[1024];
	short face_count = 1;
	short secondary_count = 0;
	faces[0] = placement->unknown50;
	for (short i = 0; i < face_count; ++i)
	{
		function_17dd80(mesh, &projection->quad, output, faces[i], true, projection->radius,
			*(short const *)(definition + 2), faces, &face_count, secondary, &secondary_count);
	}
	short mode = *(short const *)(definition + 2);
	if ((byte)g_444a38[mode].flags && secondary_count > 0)
	{
		short remaining = secondary_count;
		do
		{
			long group_count = 0;
			for (short i = 0; i < secondary_count && !(short)group_count; ++i)
			{
				long first_face = secondary[i];
				if (first_face == NONE)
					continue;
				plane3f first_plane;
				decal_face_plane_17fd20(mesh, first_face, &first_plane);
				faces[(short)group_count++] = first_face;
				secondary[i] = NONE;
				for (short j = i + 1; j < secondary_count; ++j)
				{
					long face = secondary[j];
					if (face != NONE)
					{
						plane3f plane;
						decal_face_plane_17fd20(mesh, face, &plane);
						if (g_444a38[mode].maximum_angle * 0.017453292f >= decal_normal_angle_17fd20(&first_plane.n, &plane.n) &&
							projection->radius * 0.05f > function_17d900(mesh, face, &first_plane) &&
							projection->radius * 0.05f > function_17d900(mesh, first_face, &plane))
						{
							faces[(short)group_count++] = face;
							secondary[j] = NONE;
						}
					}
				}
				long selected_face = NONE;
				real selected_minimum, selected_maximum;
				point3f start, end;
				plane3f selected_plane;
				for (short j = 0; j < (short)group_count; ++j)
				{
					long face = faces[j];
					long edge_index = mesh->faces[face].first_edge;
					long first_edge = edge_index;
					do
					{
						s_decal_mesh_edge const *edge = &mesh->edges[edge_index];
						bool reverse = edge->faces[1] == face;
						point3f const *a = &mesh->vertices[edge->vertices[!reverse]].position;
						point3f const *b = &mesh->vertices[edge->vertices[reverse]].position;
						real da = (real)fabs(plane_distance_to_point(&projection->quad.plane, a));
						real db = (real)fabs(plane_distance_to_point(&projection->quad.plane, b));
						real minimum, maximum;
						if (da > db)
						{
							minimum = db;
							maximum = da;
						}
						else
						{
							minimum = da;
							maximum = db;
						}
						if (selected_face == NONE || (selected_minimum >= minimum && selected_maximum >= maximum))
						{
							decal_face_plane_17fd20(mesh, face, &selected_plane);
							start = *a;
							end = *b;
							selected_minimum = minimum;
							selected_maximum = maximum;
							selected_face = face;
						}
						edge_index = edge->next_edges[reverse];
					} while (edge_index != first_edge);
				}
				vector3f axis;
				axis.i = end.x - start.x;
				axis.j = end.y - start.y;
				axis.k = end.z - start.z;
				real length = (real)sqrt(axis.i * axis.i + axis.k * axis.k + axis.j * axis.j);
				if (!(fabs(length) < 0.0001f))
				{
					real inverse = 1.0f / length;
					axis.i *= inverse;
					axis.j *= inverse;
					axis.k *= inverse;
				}
				else
					length = 0.0f;
				s_decal_quad_17d9a0 quad;
				if (length > 0.0f)
				{
					vector3f cross;
					cross.i = selected_plane.n.j * projection->quad.plane.n.k - selected_plane.n.k * projection->quad.plane.n.j;
					cross.j = selected_plane.n.k * projection->quad.plane.n.i - selected_plane.n.i * projection->quad.plane.n.k;
					cross.k = selected_plane.n.i * projection->quad.plane.n.j - selected_plane.n.j * projection->quad.plane.n.i;
					real sign = cross.i * axis.i + cross.k * axis.k + cross.j * axis.j < 0.0f ? 1.0f : -1.0f;
					real angle = decal_normal_angle_17fd20(&selected_plane.n, &projection->quad.plane.n) * sign;
					real sine = (real)sin(angle);
					real cosine = (real)cos(angle);
					real one_minus_cosine = 1.0f - cosine;
					vector3f forward, left, up;
					forward.i = axis.i * axis.i + (1.0f - axis.i * axis.i) * cosine;
					forward.j = one_minus_cosine * axis.j * axis.i + axis.k * sine;
					forward.k = one_minus_cosine * axis.k * axis.i - axis.j * sine;
					left.i = one_minus_cosine * axis.j * axis.i - axis.k * sine;
					left.j = axis.j * axis.j + (1.0f - axis.j * axis.j) * cosine;
					left.k = one_minus_cosine * axis.k * axis.j + axis.i * sine;
					up.i = one_minus_cosine * axis.k * axis.i + axis.j * sine;
					up.j = one_minus_cosine * axis.k * axis.j - axis.i * sine;
					up.k = axis.k * axis.k + (1.0f - axis.k * axis.k) * cosine;
					transform4x3f matrix;
					vector3f offset, position;
					offset.i = projection->matrix.position.x - start.x;
					offset.j = projection->matrix.position.y - start.y;
					offset.k = projection->matrix.position.z - start.z;
					decal_rotate_vector_17fd20(&forward, &left, &up, &offset, &position);
					decal_rotate_vector_17fd20(&forward, &left, &up, &projection->matrix.forward, &matrix.forward);
					decal_rotate_vector_17fd20(&forward, &left, &up, &projection->matrix.left, &matrix.left);
					decal_rotate_vector_17fd20(&forward, &left, &up, &projection->matrix.up, &matrix.up);
					matrix.position.x = position.i + start.x;
					matrix.position.y = position.j + start.y;
					matrix.position.z = position.k + start.z;
					matrix.scale = 1.0f;
					function_17d9a0(&matrix, (real const *)&projection->bounds, &quad);
					for (long k = 0; k < 3; ++k)
					{
						real value = ((real const *)&matrix.up)[k];
						projection->orientation_bounds[k * 2] = value > projection->orientation_bounds[k * 2] ? projection->orientation_bounds[k * 2] : value;
						projection->orientation_bounds[k * 2 + 1] = value > projection->orientation_bounds[k * 2 + 1] ? value : projection->orientation_bounds[k * 2 + 1];
					}
				}
				else
					quad = projection->quad;
				for (short j = 0; j < (short)group_count; ++j)
				{
					function_17dd80(mesh, &quad, output, faces[j], false, projection->radius,
						mode, 0, 0, 0, 0);
				}
				remaining -= (short)group_count;
			}
		} while (remaining > 0);
	}
}


#include "unknown_03bcb0.h"
#include <float.h>
struct s_decal_globals;
extern s_decal_globals *g_4ea94c;
extern __int64 g_485aa0;
point3f g_510c80;
long g_46dd4c = NONE;
PRIVATE short const g_468cac[8] = {1, 1, -1, -1, 0, 1, 0, -1};
PRIVATE short const g_468cc0[8] = {1, -1, -1, 1, 1, 0, -1, 0};
void function_142390(plane3f const *plane, transform4x3f *out);
struct s_decal_sprite_definition;
bool function_17cb00(s_decal_sprite_definition const *, short, short, real, box2f *, box2f *);
extern long g_4e6488;

struct s_decal_random_17ef10
{
    bool initialized;
    byte unknown01[3];
    real scale;
    byte unknown08[12];
    real angle;
    long sequence;
};
struct s_decal_history_17ef10
{
    long tag_index;
    point3f position;
    real radius;
    point3f source;
    long owner;
    real expiration;
    dword stamp;
};
PRIVATE __forceinline real decal_random_17ef10()
{
    g_4e7408->seed = g_4e7408->seed * 1664525 + 1013904223;
    return (real)(g_4e7408->seed >> 16) * (1.0f / 65535.0f);
}
PRIVATE __forceinline void decal_normalize_17ef10(vector3f *v)
{
    real length = (real)sqrt(v->k * v->k + v->j * v->j + v->i * v->i);
    if (!(fabs(length) < 0.0001f))
    {
        real inverse = 1.0f / length;
        v->i *= inverse; v->j *= inverse; v->k *= inverse;
    }
}

// @retail 0x17ef10
bool function_17ef10(transform4x3f const *matrix, long tag_index, s_decal_placement const *placement,
    vector3f const *direction, real scale, bool skip_texture, short sequence_index, long skip_history,
    s_decal_projection_17fd20 *projection, s_decal_random_17ef10 *random)
{
    (void)&tag_index; (void)&placement; (void)&direction; (void)&scale;
    (void)&skip_texture; (void)&sequence_index; (void)&skip_history; (void)&projection; (void)&random;
    byte const *definition = g_4e3b44[tag_index & 0xffff].bytes;
    point3f center = placement->position;
    if (matrix)
    {
        real x = center.x, y = center.y, z = center.z;
        if (matrix->scale != 1.0f) { x *= matrix->scale; y *= matrix->scale; z *= matrix->scale; }
        center.x = matrix->up.i * z + matrix->left.i * y + matrix->forward.i * x + matrix->position.x;
        center.y = matrix->up.j * z + matrix->left.j * y + matrix->forward.j * x + matrix->position.y;
        center.z = matrix->up.k * z + matrix->left.k * y + matrix->forward.k * x + matrix->position.z;
    }
    if (*(word const *)(definition + 6) > 0 && !skip_history)
    {
        real now = g_510c54 && g_510c54->active ? g_510c54->game_time * g_510c54->rate : 0.0f;
        real multiplier = *(real const *)(definition + 0x18);
        if (multiplier == 0.0f) multiplier = 1.0f;
        s_decal_history_17ef10 *history = (s_decal_history_17ef10 *)((byte *)g_4ea94c + 0x380c);
        long overlap_count = 0;
        for (long i = 0; i < 24; ++i)
        {
            s_decal_history_17ef10 *entry = &history[i];
            if (!entry->tag_index || (entry->expiration != 0.0f && entry->expiration <= now) ||
                entry->tag_index != tag_index || entry->radius <= 0.0f) continue;
            real x = entry->position.x - center.x, y = entry->position.y - center.y, z = entry->position.z - center.z;
            real radius = *(real const *)(definition + 0x14) * multiplier + entry->radius;
            if (radius * radius >= x * x + z * z + y * y && ++overlap_count >= *(word const *)(definition + 6)) return false;
        }
        long selected = NONE;
        dword oldest = 0;
        for (long i = 0; i < 24; ++i)
        {
            s_decal_history_17ef10 *entry = &history[i];
            if (!entry->tag_index || (entry->expiration > 0.0f && now > entry->expiration)) { selected = i; break; }
            if (entry->stamp <= oldest || !oldest) { oldest = entry->stamp; selected = i; }
        }
        if (selected != NONE)
        {
            s_decal_history_17ef10 *entry = &history[selected];
            entry->tag_index = tag_index;
            entry->position = center;
            entry->source = g_510c80;
            entry->owner = g_46dd4c;
            entry->radius = *(real const *)(definition + 0x14) * multiplier;
            entry->expiration = 0.0f;
            entry->stamp = (dword)g_485aa0;
            real duration = *(real const *)(definition + 0x38);
            if (duration != 0.0f) entry->expiration = now + duration;
        }
    }
    vector3f normal = placement->plane.n;
    vector3f a, b;
    real cosine, sine;
    word flags = *(word const *)definition;
    if ((flags & 8) && -0.0001f > normal.j * direction->j + normal.k * direction->k + normal.i * direction->i)
    {
        cosine = -1.0f; sine = 0.0f;
        if (flags & 0x20)
        {
            transform4x3f basis;
            function_142390(&placement->plane, &basis);
            vector3f local = *direction;
            if (basis.scale != 1.0f)
            {
                real inverse = 1.0f / basis.scale;
                local.i *= inverse; local.j *= inverse; local.k *= inverse;
            }
            real x = basis.forward.k * local.k + basis.forward.j * local.j + basis.forward.i * local.i;
            real y = basis.left.k * local.k + basis.left.j * local.j + basis.left.i * local.i;
            long chosen = 4;
            real minimum = FLT_MAX;
            for (long i = 0; i < 8; ++i)
            {
                vector3f candidate = {(real)g_468cac[i], (real)g_468cc0[i], 0.0f};
                decal_normalize_17ef10(&candidate);
                real dot = candidate.j * y + candidate.i * x;
                if (minimum > dot) { minimum = dot; chosen = i; }
            }
            vector3f first = {(real)g_468cac[chosen], (real)g_468cc0[chosen], 0.0f};
            vector3f second = {first.j, -first.i, 0.0f};
            if (basis.scale != 1.0f)
            {
                first.i *= basis.scale; first.j *= basis.scale; first.k *= basis.scale;
                second.i *= basis.scale; second.j *= basis.scale; second.k *= basis.scale;
            }
            decal_rotate_vector_17fd20(&basis.forward, &basis.left, &basis.up, &first, &a);
            decal_rotate_vector_17fd20(&basis.forward, &basis.left, &basis.up, &second, &b);
        }
        else
        {
            a.i = normal.j * direction->k - direction->j * normal.k;
            a.j = normal.k * direction->i - normal.i * direction->k;
            a.k = normal.i * direction->j - direction->i * normal.j;
            b.i = normal.k * a.j - normal.j * a.k;
            b.j = normal.i * a.k - normal.k * a.i;
            b.k = normal.j * a.i - normal.i * a.j;
        }
    }
    else
    {
        if (!random->initialized) random->angle = decal_random_17ef10() * 6.2831855f;
        cosine = (real)cos(random->angle);
        sine = (real)sin(random->angle);
        real x = (real)fabs(normal.i), y = (real)fabs(normal.j), z = (real)fabs(normal.k);
        if (y >= x && z >= x) { a.i = 0.0f; a.j = normal.k; a.k = -normal.j; }
        else if (z >= y) { a.i = -normal.k; a.j = 0.0f; a.k = normal.i; }
        else { a.i = normal.j; a.j = -normal.i; a.k = 0.0f; }
        b.i = normal.k * a.j - normal.j * a.k;
        b.j = normal.i * a.k - normal.k * a.i;
        b.k = normal.j * a.i - normal.i * a.j;
    }
    decal_normalize_17ef10(&a);
    decal_normalize_17ef10(&b);
    projection->matrix.position = placement->position;
    projection->matrix.up = normal;
    projection->matrix.forward.i = b.i * cosine - a.i * sine;
    projection->matrix.forward.j = b.j * cosine - a.j * sine;
    projection->matrix.forward.k = b.k * cosine - a.k * sine;
    projection->matrix.left.i = b.i * sine + a.i * cosine;
    projection->matrix.left.j = b.j * sine + a.j * cosine;
    projection->matrix.left.k = b.k * sine + a.k * cosine;
    byte *bitmap_group = g_4e3b44[*(long const *)(definition + 0x8c) & 0xffff].bytes;
    projection->bitmap_group = bitmap_group;
    long sequence_count = *(long *)(bitmap_group + 0x3c);
    if (sequence_count > 0)
    {
        if (sequence_index != NONE) projection->sequence_index = sequence_index;
        else
        {
            if (!random->initialized)
            {
                g_4e7408->seed = g_4e7408->seed * 1664525 + 1013904223;
                random->sequence = (short)(((g_4e7408->seed >> 16) * (short)(sequence_count - 1)) >> 16);
            }
            long index = random->sequence;
            if (index < 0) index = 0;
            else if (index > sequence_count - 1) index = sequence_count - 1;
            projection->sequence_index = (short)index;
        }
    }
    else projection->sequence_index = 0;
    projection->frame_index = 0;
    if (scale == 0.0f) scale = 1.0f;
    if (!random->initialized) random->scale = decal_random_17ef10();
    projection->radius = (*(real const *)(definition + 0x14) - *(real const *)(definition + 0x10)) * random->scale + *(real const *)(definition + 0x10);
    projection->radius *= scale;
    if (*(short *)bitmap_group == 3 && function_17cb00((s_decal_sprite_definition const *)definition,
        projection->sequence_index, 0, projection->radius, &projection->texture_bounds, &projection->bounds))
    {
        projection->sequence = *(byte **)(bitmap_group + 0x40) + projection->sequence_index * 0x3c;
        projection->frame = *(byte **)(projection->sequence + 0x38) + projection->frame_index * 0x20;
        projection->bitmap_index = *(short *)projection->frame;
    }
    else
    {
        real aspect = 1.0f;
        if (flags & 0x100)
        {
            byte *bitmap = *(byte **)(bitmap_group + 0x48) + projection->bitmap_index * 0x74;
            aspect = (real)*(short *)(bitmap + 6) / (real)*(short *)(bitmap + 4);
        }
        projection->bounds.x0 = 0.0f - projection->radius;
        projection->bounds.x1 = projection->radius;
        projection->bounds.y0 = 0.0f - projection->radius * aspect;
        projection->bounds.y1 = projection->radius * aspect;
        real *texture = (real *)((byte *)projection + 0xec);
        texture[0] = 0.0f; texture[1] = 1.0f; texture[2] = 0.0f; texture[3] = 1.0f;
    }
    if (!skip_texture)
    {
        byte *bitmap = *(byte **)(bitmap_group + 0x48) + projection->bitmap_index * 0x74;
        if (*(long *)(bitmap + 0x70) <= g_4e6488 || !*(long *)(bitmap + 0x50))
            if (!texture_cache_bitmap_get_texture((s_bitmap_data *)bitmap, 2, 0.0f) &&
                !function_12ce00((s_bitmap_data *)bitmap, 2, 0.0f)) return false;
    }
    function_17d9a0(&projection->matrix, (real const *)&projection->bounds, &projection->quad);
    projection->orientation_bounds[0] = projection->orientation_bounds[1] = projection->matrix.up.i;
    projection->orientation_bounds[2] = projection->orientation_bounds[3] = projection->matrix.up.j;
    projection->orientation_bounds[4] = projection->orientation_bounds[5] = projection->matrix.up.k;
    return true;
}



struct s_180d81;
struct s_180d83;
struct s_180d84;
struct s_cluster_query;
long __stdcall function_180d80(s_180d84 const *, transform4x3f const *, long, s_cluster_query const *, bool, s_180d81 const *, s_180d83 *);

// @retail 0x17ee20
bool function_17ee20(long tag_index, s_decal_placement const *placement, s_decal_mesh_view const *mesh,
    transform4x3f const *matrix, vector3f const *direction, real scale, bool skip_texture,
    short sequence_index, long skip_history)
{
    (void)&mesh; (void)&matrix; (void)&direction; (void)&scale;
    (void)&skip_texture; (void)&sequence_index; (void)&skip_history;
    s_decal_random_17ef10 random;
    memset(&random, 0, sizeof(random));
    bool result = false;
    s_decal_projection_17fd20 projection;
    s_decal_output_17dd80 output;
    while (tag_index != NONE && (short)placement->unknown20 != NONE)
    {
        memset(&projection, 0, sizeof(projection));
        if (!function_17ef10(matrix, tag_index, placement, direction, scale, skip_texture,
            sequence_index, skip_history, &projection, &random)) break;
        output.polygon_count = 0;
        function_17fd20(mesh, tag_index, &output, &projection, placement);
        tag_index = function_180d80((s_180d84 const *)&projection, matrix, tag_index,
            (s_cluster_query const *)placement, skip_texture, (s_180d81 const *)&output, (s_180d83 *)&random);
        if (output.polygon_count > 0) result = true;
    }
    return result;
}



#include "unknown_1428b0.h"
struct s_effect_source;
bool g_46dd48 = true;
struct s_slot_entry_list;
extern s_slot_entry_list *g_4e0340;
real function_30bf0(vector3f *);
plane3f *function_1428b0(transform4x3f const *, plane3f const *, plane3f *);
int __stdcall function_1429d0(transform4x3f const *, long, point3f const *, point3f *);
struct s_collision_result_1697c0;
bool __stdcall function_1697c0(long, point3f const *, vector3f const *, long, long, s_collision_result_1697c0 *);

// @retail 0x17e670
void function_17e670(s_effect_source *source, point3f const *point, long tag_index,
    vector3f const *vector, real radius, long unknown0, long unknown1, long unknown2)
{
    (void)&tag_index; (void)&vector; (void)&radius;
    (void)&unknown0; (void)&unknown1; (void)&unknown2;
    if (!g_46dd48) return;
    byte *definition = g_4e3b44[tag_index & 0xffff].bytes;
    dword saved_seed = g_4e7408->seed;
    bool deterministic = (byte)unknown0 != 0;
    short sequence = (short)unknown1;
    point3f *output = (point3f *)unknown2;
    bool world_surface = false;
    long excluded = NONE;
    short cluster = NONE;
    point3f center;
    if (deterministic) g_4e7408->seed = *(dword *)&point->x ^ *(dword *)&point->y ^ *(dword *)&point->z ^ 0xdeadc0de;
    vector3f direction = *vector;
    function_30bf0(&direction);
    point3f start = {point->x-direction.i*0.01f, point->y-direction.j*0.01f, point->z-direction.k*0.01f};
    s_decal_placement collision;
    collision.unknown24 = NONE;
    s_decal_placement const *placement = (s_decal_placement const *)source;
    if (!placement || placement->unknown00 == 4)
    {
        if (!function_1697c0(0x20800007, &start, vector, NONE, NONE, (s_collision_result_1697c0 *)&collision)) goto extra_surfaces;
        placement = &collision;
    }
    cluster = (short)placement->unknown20;
    if (placement->unknown00 == 1)
    {
        world_surface = true;
        center = placement->position;
        g_510c80 = center;
        g_46dd4c = 0;
        function_17ee20(tag_index, placement, (s_decal_mesh_view *)g_4e0340, NULL, vector, radius,
            deterministic, sequence, unknown2);
    }
    else if (placement->unknown00 == 3)
    {
        byte *bsp = (byte *)g_4e0348;
        byte *bound = *(byte **)(bsp + 0x144) + placement->unknown3c*0x58;
        byte *section = *(byte **)(bsp + 0x13c) + *(short *)(bound + 0x34)*0xc8;
        if (placement != &collision) decal_placement_copy(placement, &collision);
        excluded = placement->unknown3c;
        center = placement->position;
        transform4x3f const *matrix = (transform4x3f const *)bound;
        function_142700(matrix, &collision.position, &collision.position);
        function_1428b0(matrix, &collision.plane, &collision.plane);
        vector3f local;
        function_1427f0(matrix, vector, &local);
        g_510c80 = center;
        g_46dd4c = 1;
        if (function_17ee20(tag_index, &collision, (s_decal_mesh_view *)(section + 0x70), matrix,
            &local, radius, deterministic, sequence, unknown2) && output)
            function_1429d0(matrix, 4, output, output);
    }
extra_surfaces:
    if (*(real *)(definition + 0x14)*2.0f <= 0.2f || deterministic)
    {
        if (deterministic) g_4e7408->seed = saved_seed;
        return;
    }
    if (!world_surface && excluded == NONE) return;
    s_decal_bound_17e490 *bounds[1024];
    real reach = radius * *(real *)(definition + 0x14) * 0.25f;
    long count = function_17e490(excluded, cluster, &center, reach, 1024, bounds);
    for (long i = 0; i < count; ++i)
    {
        byte *bound = (byte *)bounds[i];
        byte *bsp = (byte *)g_4e0348;
        byte *section = *(byte **)(bsp + 0x13c) + *(short *)(bound + 0x34)*0xc8;
        s_decal_placement hit;
        hit.unknown24 = NONE;
        direction.i = bounds[i]->center.x-center.x;
        direction.j = bounds[i]->center.y-center.y;
        direction.k = bounds[i]->center.z-center.z;
        if (function_30bf0(&direction) < 0.0001f) continue;
        start.x = center.x + direction.i*0.01f;
        start.y = center.y + direction.j*0.01f;
        start.z = center.z + direction.k*0.01f;
        real length = reach*2.0f;
        direction.i *= length; direction.j *= length; direction.k *= length;
        if (!function_1697c0(0x800005, &start, &direction, NONE, NONE, (s_collision_result_1697c0 *)&hit) || hit.unknown00 != 3) continue;
        if (*(byte **)(bsp + 0x144) + hit.unknown3c*0x58 != bound) continue;
        transform4x3f const *matrix = (transform4x3f const *)bound;
        function_142700(matrix, &hit.position, &hit.position);
        function_1428b0(matrix, &hit.plane, &hit.plane);
        function_1427f0(matrix, &direction, &direction);
        g_46dd4c = 2;
        function_17ee20(tag_index, &hit, (s_decal_mesh_view *)(section + 0x70), matrix, &direction,
            radius, false, sequence, 0);
    }
    if (world_surface) return;
    static vector3f const axes[6] = {{1,0,0},{-1,0,0},{0,1,0},{0,-1,0},{0,0,1},{0,0,-1}};
    for (long i = 0; i < 6; ++i)
    {
        direction = axes[i];
        start.x = center.x + direction.i*0.01f;
        start.y = center.y + direction.j*0.01f;
        start.z = center.z + direction.k*0.01f;
        real length = reach*2.0f;
        direction.i *= length; direction.j *= length; direction.k *= length;
        s_decal_placement hit;
        hit.unknown24 = NONE;
        if (function_1697c0(0x20800007, &start, &direction, NONE, NONE, (s_collision_result_1697c0 *)&hit) && hit.unknown00 == 1)
        {
            g_46dd4c = 3;
            if (function_17ee20(tag_index, &hit, (s_decal_mesh_view *)g_4e0340, NULL, &direction,
                radius, false, sequence, 0)) return;
        }
    }
}
