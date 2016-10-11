/**
	@file
	beatshifter -
	baek chang - breakchange@gmail.com

	@ingroup	examples	
*/

#include "ext.h"							// standard Max include, always required
#include "ext_obex.h"						// required for new style Max object

typedef struct _interval_data
{
	double onbeat_offset;
	double offbeat_offset;
	double onbeat_chance;
	double offbeat_chance;
} t_interval_data;

////////////////////////// object struct
typedef struct _beatshifter
{
	t_object	beatshifter; // the object itself (must be first)
	double		tempo;
	int			ticks;

	t_interval_data quarter;
	t_interval_data eighth;
	t_interval_data sixteenth;

	double		delay_amount;
	void *		delay_outlet;
} t_beatshifter;

///////////////////////// function prototypes
void *bs_new();
void bs_tempo(t_beatshifter *b, double tempo);
void bs_ticks(t_beatshifter *b, double ticks);
void parse_values(t_beatshifter *b, t_symbol *sym, long argc, t_atom *argv);
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

	class_addmethod(c, (method)parse_values, "anything", A_GIMME, 0);

	gThis = c;

	return 0;
}

void *bs_new()
{
	t_beatshifter *b = (t_beatshifter *)object_alloc(gThis);

	inlet_new(b, NULL);
	floatin(b, 2);
	floatin(b, 1);

	b->delay_outlet = floatout((t_object *)b);

	b->tempo = 0;
	b->ticks = 0;

	b->quarter.onbeat_offset = 0.0;
	b->quarter.offbeat_offset = 0.0;
	b->quarter.onbeat_chance = 100.0;
	b->quarter.offbeat_chance = 100.0;

	b->eighth.onbeat_offset = 0.0;
	b->eighth.offbeat_offset = 0.0;
	b->eighth.onbeat_chance = 100.0;
	b->eighth.offbeat_chance = 100.0;

	b->sixteenth.onbeat_offset = 0.0;
	b->sixteenth.offbeat_offset = 0.0;
	b->sixteenth.onbeat_chance = 100.0;
	b->sixteenth.offbeat_chance = 100.0;

	b->delay_amount = 0;

	return b;
}

void bs_tempo(t_beatshifter *b, double tempo)
{
	if (b->tempo != tempo)
	{
		b->tempo = tempo;
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

void parse_values(t_beatshifter *b, t_symbol *sym, long argc, t_atom *argv)
{
	long i;
	t_atom *ap;

	t_interval_data *interval_data = NULL;

	const char *sym_name = sym->s_name;
	if (strcmp("quarter", sym_name) == 0)
	{
		interval_data = &b->quarter;
	}
	else if (strcmp("eighth", sym_name) == 0)
	{
		interval_data = &b->eighth;
	}
	else if (strcmp("sixteenth", sym_name) == 0)
	{
		interval_data = &b->sixteenth;
	}

	if (interval_data == NULL)
	{
		post("ERROR, interval data is NULL!");
		return;
	}

	for (i = 0, ap = argv; i < argc; i++, ap++)
	{
		switch (atom_gettype(ap))
		{
			case A_FLOAT:
			{
				switch (i)
				{
					case 0:
						interval_data->onbeat_offset = argv[i].a_w.w_float;
						break;

					case 1:
						interval_data->offbeat_offset = argv[i].a_w.w_float;
						break;

					case 2:
						interval_data->onbeat_chance = argv[i].a_w.w_float;
						break;

					case 3:
						interval_data->offbeat_chance = argv[i].a_w.w_float;
						break;

					default:
						break;
				}
				break;
			}

			default:
				post("%ld: unknown atom type (%ld)", i+1, atom_gettype(ap));
				break;
		}
	}
}

void bs_compute_delay(t_beatshifter *b)
{
	// default delay amount by largest offset which is 8th note
	double delay = 60000.0 / 2.0 / b->tempo;

	srand(time(NULL));
	double rand_chance_value = (double)(rand() % 100);

	switch (b->ticks)
	{
		case 15:
		case 31:
			if (rand_chance_value <= b->quarter.onbeat_chance)
			{
				delay += (60000.0/b->tempo)*b->quarter.onbeat_offset;
			}
			break;

		case 7:
		case 23:
			if (rand_chance_value <= b->quarter.offbeat_chance)
			{
				delay += (60000.0/b->tempo)*b->quarter.offbeat_offset;
			}
			break;

		case 3:
		case 19:
			if (rand_chance_value <= b->eighth.onbeat_chance)
			{
				delay += (60000.0/2.0/b->tempo)*b->eighth.onbeat_offset;
			}
			break;

		case 11:
		case 27:
			if (rand_chance_value <= b->eighth.offbeat_chance)
			{
				delay += (60000.0/2.0/b->tempo)*b->eighth.offbeat_offset;
			}
			break;

		case 1:
		case 9:
		case 17:
		case 25:
			if (rand_chance_value <= b->sixteenth.onbeat_chance)
			{
				delay += (60000.0/4.0/b->tempo)*b->sixteenth.onbeat_offset;
			}
			break;

		case 5:
		case 13:
		case 21:
		case 29:
			if (rand_chance_value <= b->sixteenth.offbeat_chance)
			{
				delay += (60000.0/4.0/b->tempo)*b->sixteenth.offbeat_offset;
			}
			break;

		// use delay value already set
		default:
			break;
	}

	b->delay_amount = delay;
	outlet_float(b->delay_outlet, b->delay_amount);
}
