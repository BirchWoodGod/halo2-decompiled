// @flags /O2 /Gr /arch:SSE
/* UNKNOWN_11EED0.CPP: motion smoothing and quantization helpers */

#include "unknown_11c920.h"
#include "globals.h"
#include <math.h>

real function_30bf0(vector3f *v);

// @retail 0x11eed0
bool function_11eed0(
	real *velocity,
	real *position,
	real dt,
	bool wrap,
	real arg_4,
	real a,
	real arg_6,
	real lo,
	real hi)
{
	real volatile &target = arg_4;
	real volatile &b = arg_6;
	bool reached = false;
	real v0 = *velocity;
	real p0 = *position;
	real dt2 = dt * dt;
	real orig = target;
	real d = orig - p0;
	real limit;
	real accel;
	real newv;
	real newp;
	real half;

	target = d;
	if (wrap)
	{
		half = (hi - lo) * 0.5f;
		if (d > half)
			d -= half * 2.f;
		else if (0.f - half > d)
			d = half * 2.f + d;
		target = d;
	}

	real local_0 = a * a * b;
	limit = a * dt;
	if (!(local_0 > limit))
		limit = local_0;

	if (limit >= (real)fabs(target - v0 * a))
	{
		real c = orig;
		if (lo > c)
			c = lo;
		else if (c > hi)
			c = hi;
		reached = true;
		*velocity = 0.f;
		*position = c;
	}
	else
	{
		real x = (real)fabs(target) * b;
		x = x + x;
		target = x >= dt2 ? dt : (real)sqrt(x);
		if (0.f > d)
			target = 0.f - target;

		accel = (target - v0) / a;
		if ((real)fabs(accel) > b)
		{
			long s = accel == 0.f ? 0 : (0.f > accel ? -1 : 1);
			accel = (real)s * b;
		}

		{
			real dv = accel * a;
			newv = dv + v0;
			newp = (dv * 0.5f + newv) * a + p0;
		}
		if (wrap)
		{
			if (lo > newp)
				newp = hi - lo + newp;
			else if (newp > hi)
				newp = newp - (hi - lo);
		}
		if (lo > newp)
			newp = lo;
		else if (newp > hi)
			newp = hi;
		*velocity = newv;
		*position = newp;
	}
	return reached;
}

PRIVATE __forceinline double function_11f0d1(real arg_0)
{
	return acos(arg_0 < -1.0f ? -1.0f : arg_0 > 1.0f ? 1.0f : arg_0);
}

// @retail 0x11f0d0
void function_11f0d0(
	vector3f *position,
	vector3f *forward,
	vector3f const *target,
	real rate,
	real max_angle,
	real scale)
{
	vector3f axis;
	vector3f diff;
	real d;
	double angle;
	real step;
	real dist2;
	real theta;

	if (0.f >= scale && 0.f >= max_angle)
	{
		*position = *g_4687a4;
		*forward = *target;
		return;
	}

	d = forward->i * target->i + (target->j * forward->j + target->k * forward->k);
	d = d < -1.f ? -1.f : d > 1.f ? 1.f : d;
	angle = function_11f0d1(d);
	angle = angle * scale;
	angle = angle + angle;
	max_angle = angle >= max_angle * max_angle ? *(real const volatile *)&max_angle : (real)sqrt(angle);

	axis.i = target->k * forward->j - forward->k * target->j;
	axis.j = target->i * forward->k - target->k * forward->i;
	axis.k = forward->i * target->j - target->i * forward->j;
	function_30bf0(&axis);

	axis.i = axis.i * max_angle;
	axis.j = axis.j * max_angle;
	axis.k = axis.k * max_angle;
	diff.i = axis.i - position->i;
	diff.j = axis.j - position->j;
	diff.k = axis.k - position->k;
	dist2 = diff.k * diff.k + diff.j * diff.j + diff.i * diff.i;
	step = rate * scale;

	if (step * step * 5.f > dist2)
	{
		if (step > max_angle)
		{
			*position = *g_4687a4;
			*forward = *target;
			return;
		}
		*position = axis;
	}
	else
	{
		double s = step / sqrt(dist2);
		position->i = diff.i * s + position->i;
		position->j = diff.j * s + position->j;
		position->k = diff.k * s + position->k;
	}

	axis = *position;
	theta = function_30bf0(&axis) * rate;
	if (theta != 0.f)
	{
		real s = (real)sin(theta);
		real c = (real)cos(theta);
		real dot = (forward->i * axis.i + (forward->j * axis.j + forward->k * axis.k)) * (1.f - c);
		real cx = forward->k * axis.i - forward->i * axis.k;
		real cy = forward->i * axis.j - forward->j * axis.i;
		real cz = forward->j * axis.k - forward->k * axis.j;
		forward->i = forward->i * c + axis.i * dot - cz * s;
		forward->j = forward->j * c + axis.j * dot - cx * s;
		forward->k = forward->k * c + axis.k * dot - cy * s;
		function_30bf0(forward);
	}
}

// @retail 0x11f470
byte function_11f470(real lo, real hi, real value)
{
	real range = hi - lo;
	byte result = 0;

	if (range > 0.0001f)
	{
		result = (byte)((value - lo) / range * 255.f);
		while (result > 0)
		{
			real v;
			if (result == 255)
				v = hi;
			else
				v = range * ((real)result * (1.f / 255.f)) + lo;
			if (!(v > value))
				break;
			result--;
		}
		return result;
	}
	return 0;
}

// @retail 0x11f4f0
void function_11f4f0(
	long bits,
	real const *in,
	real_bounds const *ranges,
	long *out)
{
	long i = 0;

	do
	{
		real v;
		long q;
		real lo;
		real hi;

		if (ranges[i].lo > in[i])
			v = ranges[i].lo;
		else if (in[i] > ranges[i].hi)
			v = ranges[i].hi;
		else
			v = in[i];
		lo = ranges[i].lo;
		hi = ranges[i].hi;
		v = (v - lo) / ((hi - lo) / (real)((1 << bits) - 1));
		__asm
		{
			fld v
			fistp q
		}
		out[i] = q;
		i++;
	}
	while (i < 3);
}

// @retail 0x11f580
void function_11f580(
	real const *ranges,
	long bits,
	real *out,
	long const *in)
{
	long i = 0;
	long max = (1 << bits) - 1;

	do
	{
		long q = in[i];
		real hi = ranges[i * 2 + 1];
		real lo = ranges[i * 2];
		real v;

		if (q == 0)
			v = lo;
		else if (q >= max)
			v = hi;
		else
			v = ((real)(max - q) * lo + (real)q * hi) / (real)max;
		out[i] = v;
		i++;
	}
	while (i < 3);
}


// @retail 0x11f5f0
void function_11f5f0(
	box2f *bounds,
	point2f const *points,
	long count)
{
	long i;

	for (i = 0; i < count; i++)
	{
		point2f const *p = points + i;
		if (bounds->x0 > p->x)
			bounds->x0 = p->x;
		if (p->x > bounds->x1)
			bounds->x1 = p->x;
		if (bounds->y0 > p->y)
			bounds->y0 = p->y;
		if (p->y > bounds->y1)
			bounds->y1 = p->y;
	}
}
