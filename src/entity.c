#include <gint/keyboard.h>
#include <gint/defs/util.h>
#include <gint/gray.h>
#include <stdbool.h>
#include <math.h>
#include <gint/timer.h>
#include <gint/gint.h>
#include <gint/std/stdlib.h>

#include "entity.h"
#include "defs.h"
#include "world.h"
#include "render.h"
#include "inventory.h"
#include "menu.h"

extern bopti_image_t
img_ent_slime;

void slimeInit(struct EntityBase* self)
{
	self->mem[2] = rand() % 2;
}

bool slimeBehaviour(struct EntityBase* self)
{
	int* jumpTimer = &self->mem[0];
	int* animTimer = &self->mem[1];
	int* direction = &self->mem[2];

	handlePhysics(&self->props);

	if(self->props.touchingTileTop && *jumpTimer == 0)
	{
		*jumpTimer = 240;
	}
	else if(*jumpTimer > 0)
	{
		(*jumpTimer)--;
		if(*jumpTimer == 0)
		{
			self->props.yVel = -4.5;
			self->props.xVel = *direction == 0 ? -2 : 2;
			self->anim.animationFrame = 1;
		}
	}
	else if(!self->props.touchingTileTop) *jumpTimer = 0;

	if(*animTimer == 0 && self->props.touchingTileTop)
	{
		self->anim.animationFrame = !self->anim.animationFrame;
		if(*jumpTimer > 0 && *jumpTimer <= 60) *animTimer = 8;
		else *animTimer = 32;
	}
	else if(self->props.touchingTileTop) (*animTimer)--;
	return true;
}

const struct EntityBase entityTemplates[] = {
	{	ENT_SLIME, { 0 },	{16, 12},	{ 0 },	20,	&img_ent_slime,	&slimeBehaviour,	&slimeInit	}	// ENT_SLIME
};

/* Having a generic physics property struct lets me have one function to handle
collisions, instead of one for each entity/player struct */
void handlePhysics(struct EntityPhysicsProps* self)
{
	struct Rect tileCheckBox = {
		{
			max(0, (self->x >> 3) - 1),
			max(0, (self->y >> 3) - 1)
		},
		{
			min(WORLD_WIDTH - 1, ((self->x + self->width) >> 3) + 1),
			min(WORLD_HEIGHT - 1, ((self->y + self->height) >> 3) + 1)
		}
	};

	int checkLeft, checkRight, checkTop, checkBottom;
	int overlapX, overlapY;

	self->yVel = min(10, self->yVel + GRAVITY_ACCEL);
	if((self->xVel < 0 ? -self->xVel : self->xVel) < 0.5) self->xVel = 0;
	self->x += roundf(self->xVel);
	self->y += roundf(self->yVel);
	self->y++;

	self->touchingTileTop = false;
	for(int y = tileCheckBox.TL.y; y <= tileCheckBox.BR.y; y++)
	{
		for(int x = tileCheckBox.TL.x; x <= tileCheckBox.BR.x; x++)
		{
			if(tiles[world.tiles[y * WORLD_WIDTH + x].idx].physics != PHYS_NON_SOLID)
			{
				if(tiles[world.tiles[y * WORLD_WIDTH + x].idx].physics == PHYS_PLATFORM && (y < ((self->y + self->height) >> 3) || self->dropping)) continue;

				struct Rect entBox = {
					{
						self->x,
						self->y
					},
					{
						self->x + self->width - 1,
						self->y + self->height - 1
					}
				};

//				These can be changed when I implement non-fullheight blocks
				checkLeft = x << 3;
				checkRight = checkLeft + 7;
				checkTop = y << 3;
				checkBottom = checkTop + 7;

				overlapX = max(0, min(entBox.BR.x, checkRight + 1) - max(entBox.TL.x, checkLeft - 1));
				overlapY = max(0, min(entBox.BR.y, checkBottom + 1) - max(entBox.TL.y, checkTop - 1));

				if(overlapX && overlapY)
				{
					if(overlapX >= overlapY)
					{
						self->yVel = 0;
						if(entBox.TL.y >= checkTop)
						{
							self->y += overlapY;
						} 
						else 
						{
							self->y -= overlapY;
							self->touchingTileTop = true;
						}
					}
					else
					{
						//self->xVel = 0;
						if(entBox.TL.x <= checkLeft)
						{
							self->x -= overlapX;
						} 
						else
						{
							self->x += overlapX;
						}
					}
				}
			}
		}
	}
	if(self->touchingTileTop) self->xVel *= 0.7;
	else self->xVel *= 0.95;
	self->x = min(max(self->x, 0), 8 * WORLD_WIDTH - self->width);
	self->y = min(max(self->y, 0), 8 * WORLD_HEIGHT - self->height);
	if(self->y + self->height >= (WORLD_HEIGHT << 3) - 1)
	{
		self->yVel = 0;
		self->touchingTileTop = true;
	}
}