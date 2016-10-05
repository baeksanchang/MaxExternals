/**
	@file
	beatshifter -
	baek chang - breakchange@gmail.com

	@ingroup	examples	
*/

#include "ext.h"							// standard Max include, always required
#include "ext_obex.h"						// required for new style Max object

////////////////////////// object struct
typedef struct _beatshifter
{
	t_object	beatshifter; // the object itself (must be first)
	double		tempo;
	int			ticks;
	double		quarter_offset;
	double		quarter_offbeat_offset;
	double		eighth_offset;
	double		eighth_offbeat_offset;
	double		sixteenth_offset;
	double		sixteenth_offbeat_offset;
	double		delay_amount;
	void *		delay_outlet;
} t_beatshifter;

///////////////////////// function prototypes
void *bs_new();
void bs_tempo(t_beatshifter *b, double tempo);
void bs_ticks(t_beatshifter *b, double ticks);
void bs_quarter(t_beatshifter *b, double value);
void bs_quarter_offbeat(t_beatshifter *b, double value);
void bs_8th(t_beatshifter *b, double value);
void bs_8th_offbeat(t_beatshifter *b, double value);
void bs_16th(t_beatshifter *b, double value);
void bs_16th_offbeat(t_beatshifter *b, double value);


void bs_compute_delay(t_beatshifter *b);

//////////////////////// global class pointer variable
static t_class *gThis;


int C74_EXPORT main(void)
{	
	t_class *c;
	
	c = class_new("beatshifter", (method)bs_new, (method)NULL, sizeof(t_beatshifter),
				  0L, A_GIMME, 0);

	class_register(CLASS_BOX, c); /* CLASS_NOBOX */

	class_addmethod(c, (method)bs_tempo, "ft1", A_FLOAT, 0);
	class_addmethod(c, (method)bs_ticks, "ft2", A_FLOAT, 0);
	class_addmethod(c, (method)bs_quarter, "ft3", A_FLOAT, 0);
	class_addmethod(c, (method)bs_quarter_offbeat, "ft4", A_FLOAT, 0);
	class_addmethod(c, (method)bs_8th, "ft5", A_FLOAT, 0);
	class_addmethod(c, (method)bs_8th_offbeat, "ft6", A_FLOAT, 0);
	class_addmethod(c, (method)bs_16th, "ft7", A_FLOAT, 0);
	class_addmethod(c, (method)bs_16th_offbeat, "ft8", A_FLOAT, 0);

	gThis = c;

	return 0;
}

void *bs_new()
{
	t_beatshifter *b = (t_beatshifter *)object_alloc(gThis);

	floatin(b, 8);
	floatin(b, 7);
	floatin(b, 6);
	floatin(b, 5);
	floatin(b, 4);
	floatin(b, 3);
	floatin(b, 2);
	floatin(b, 1);

	b->delay_outlet = floatout((t_object *)b);

	b->tempo = 0;
	b->ticks = 0;
	b->quarter_offset = 0;
	b->quarter_offbeat_offset = 0;
	b->eighth_offset = 0;
	b->eighth_offbeat_offset = 0;
	b->sixteenth_offset = 0;
	b->sixteenth_offbeat_offset = 0;
	b->delay_amount = 0;

	return b;
}

void bs_tempo(t_beatshifter *b, double tempo)
{
//	post("Tempo %f", tempo);
	if (b->tempo != tempo)
	{
		b->tempo = tempo;
		bs_compute_delay(b);
	}
}

void bs_ticks(t_beatshifter *b, double ticks)
{
	int int_ticks = (int)floor((ticks-1.0) * 8);
	if (b->ticks != int_ticks)
	{
		b->ticks = int_ticks;
		bs_compute_delay(b);
	}
}

void bs_quarter(t_beatshifter *b, double value)
{
//	post("quarter offset %f", value);
	b->quarter_offset = value;
	bs_compute_delay(b);
}

void bs_quarter_offbeat(t_beatshifter *b, double value)
{
//	post("quarter offbeat offset %f", value);
	b->quarter_offbeat_offset = value;
	bs_compute_delay(b);
}

void bs_8th(t_beatshifter *b, double value)
{
//	post("8th offset %f", value);
	b->eighth_offset = value;
	bs_compute_delay(b);
}

void bs_8th_offbeat(t_beatshifter *b, double value)
{
//	post("8th offbeat offset %f", value);
	b->eighth_offbeat_offset = value;
	bs_compute_delay(b);
}

void bs_16th(t_beatshifter *b, double value)
{
//	post("16th offset %f", value);
	b->sixteenth_offset = value;
	bs_compute_delay(b);
}

void bs_16th_offbeat(t_beatshifter *b, double value)
{
//	post("16th offbeat offset %f", value);
	b->sixteenth_offbeat_offset = value;
	bs_compute_delay(b);
}

void bs_compute_delay(t_beatshifter *b)
{
	// default delay amount by largest offset which is 8th note
	double delay = 60000.0 / 2.0 / b->tempo;

//	post("computing delay: ticks %d", b->ticks);

	switch (b->ticks)
	{
		case 15:
		case 31:
			delay += (60000.0/b->tempo)*b->quarter_offset;
			break;

		case 7:
		case 23:
			delay += (60000.0/b->tempo)*b->quarter_offbeat_offset;
			break;

		case 3:
		case 19:
			delay += (60000.0/2.0/b->tempo)*b->eighth_offset;
			break;

		case 11:
		case 27:
			delay += (60000.0/2.0/b->tempo)*b->eighth_offbeat_offset;
			break;

		case 1:
		case 9:
		case 17:
		case 25:
			delay += (60000.0/4.0/b->tempo)*b->sixteenth_offset;
			break;

		case 5:
		case 13:
		case 21:
		case 29:
			delay += (60000.0/4.0/b->tempo)*b->sixteenth_offbeat_offset;
			break;

		// use delay value already set
		default:
			break;
	}

	b->delay_amount = delay;
	outlet_float(b->delay_outlet, b->delay_amount);
}
