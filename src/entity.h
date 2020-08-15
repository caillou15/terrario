#pragma once

#include <gint/display.h>

#include "inventory.h"
#include "menu.h"

enum PhysicsTypes {
	PHYS_NON_SOLID,
	PHYS_SOLID,
	PHYS_PLATFORM
};

struct Coords {
	int x;
	int y;
};

struct Rect {
	struct Coords TL;
	struct Coords BR;
};

struct EntityPhysicsProps {
	unsigned char width;
	unsigned char height;
	int x;
	int y;
	float xVel;
	float yVel;
	bool touchingTileTop;
	bool dropping;
};

struct AnimationData {
	int animation;
	int animationFrame;
	int direction;
};

enum Entities {
	ENT_SLIME
};

struct EntityBase {
	int id;
//	To store states in, generic so that entities can use it however
	int mem[4];
	struct EntityPhysicsProps props;
	struct AnimationData anim;
	int health;
	bopti_image_t* sprite;

	bool (*behaviour)(struct EntityBase* self);
	void (*init)(struct EntityBase* self);
};

typedef struct EntityBase Entity;

struct Player {
	struct EntityPhysicsProps props;
	struct AnimationData anim;
	int health;
	struct Coords cursor;
	struct Coords cursorTile;
	struct Inventory inventory;

	void (*physics)(struct EntityPhysicsProps* self);
};

extern const struct EntityBase entityTemplates[];

extern struct Player player;

void handlePhysics(struct EntityPhysicsProps* self);