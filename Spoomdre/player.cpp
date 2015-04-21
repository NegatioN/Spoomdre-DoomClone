#include <SDL2/SDL.h>
#include "player.hpp"
#include "gfx_util.hpp"

#include <iostream>

using namespace std;

SDL_Rect sprite;


void Player::init(Vector3f pos, Vector3f vel, Vector3f acc, sector* sec){
	setPosition(pos);
	setVelocity(vel);
	setAcceleration(acc);
	setSector(sec);
	yaw_ = 0;
}

void Player::init(int x, int y, int z){
	yaw_ = 0;

	Vector3f vec(x,y,z);
	setPosition(vec);
	yaw_ = 0;

	sprite.w = 5;
	sprite.h = 5;
	sprite.x = x - sprite.w/2;
	sprite.y = y - sprite.h/2;
	
}

//Take input accelleration-vector?
void Player::update() {
	anglesin_ = sin(angle_);
	anglecos_ = cos(angle_);
	Vector3f vecAddition(0,0,0);

    // keyboard-events
    if (wasd_.at(0)) { vecAddition(0) += anglecos_  * 2; vecAddition(1)  += anglesin_ * 2; } 	// W
    if (wasd_.at(1)) { vecAddition(0) += anglesin_ * 2; vecAddition(1) -= anglecos_  * 2; } 	// A
    if (wasd_.at(2)) { vecAddition(0) -= anglecos_  * 2;  vecAddition(1) -= anglesin_ * 2; } 	// S
    if (wasd_.at(3)) { vecAddition(0) -= anglesin_ * 2; vecAddition(1) += anglecos_  * 2; } 	// D
    if (wasd_.at(4)) { angle_ += 0.1; }									// right
    if (wasd_.at(5)) { angle_ -= 0.1; }									// left
    if (wasd_.at(6)) { yaw_ -= 0.1; }											// up
    if (wasd_.at(7)) { yaw_ += 0.1; }											// down

	angle_ += mouse_x * 0.015f;
    yaw_ += -gfx_util::clamp(-mouse_y * 0.023f, -5, 5);

    //old position-vector += movement.
	//setPosition(position() + vecAddition);

    // set moving to true if movement-key is pressed
    bool pushing = false;
    if(wasd_.at(0) || wasd_.at(1) || wasd_.at(2) || wasd_.at(3))
    	pushing = true;

	float accel = pushing ? 0.4 : 0.2;

    Vector3f vel = velocity();
    vel(0) = vel(0) * (1 - accel) + vecAddition(0) * accel;
    vel(1) = vel(1) * (1 - accel) + vecAddition(1) * accel;

    // set moving to true if movement-key is pressed
    bool moving = false;
    if(pushing)
    	moving = true;

	if(moving) {
	    Vector3f pos = position();
	    float px = pos(0);
	    float py = pos(1);
	    float dx = vel(0);
	    float dy = vel(1);

	    //sector-cross-check here

	    //Set new sector if outside the first one. Still buggy..
		setSector(getSector()->getEnteredSector(px, py, dx, dy));

	   	// std::cout << v.x() << " - " << v.y() << std::endl;
	    move(dx, dy);

	}
}

void Player::move(float dx, float dy) {
	Vector3f pos = position();
	float px = pos(0);
	float py = pos(1);

	// collision-check??


	// update positions and angles
	pos(0) += dx;
	pos(1) += dy;
	anglesin_ = sin(angle_);
	anglecos_ = cos(angle_);
	setPosition(pos);
}

void Player::render(SDL_Renderer* renderer) {
	getSector()->render(renderer, x(), y(), z(), angle(), yaw());
	getSector()->render_map(renderer, x(), y(), z(), angle());
}