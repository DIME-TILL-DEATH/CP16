/*
 * fades.cc
 *
 *  Created on: Oct 15, 2024
 *      Author: dime
 */

#include "fades.h"

bool fade_processing = false;
bool is_fade_direction_down;
float fade_coef = 1.0f;

void calc_fade_step()
{

	if(!fade_processing) return;

	if(is_fade_direction_down)
	{
		if(fade_coef > 0.0f)
		{
			fade_coef -= 0.01f;
		}
		else
		{
			fade_coef = 0.0f;
			fade_processing = false;
		}
	}
	else
	{
		if(fade_coef < 1.0f)
		{
			fade_coef += 0.01f;
		}
		else
		{
			fade_coef = 1.0f;
			fade_processing = false;
		}
	}
}

void fade_out()
{
	fade_processing = true;
	is_fade_direction_down = true;
}

void fade_in()
{
	fade_processing = true;
	is_fade_direction_down = false;
}

bool is_fade_complete()
{
	return !fade_processing;
}

float get_fade_coef()
{
	return fade_coef;
}

