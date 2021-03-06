#ifndef CLASS_PLAYER_H
#define CLASS_PLAYER_H

#include <vector>
#include "entity.hpp"
#include "enemy.hpp"
#include "gfx_util.hpp"
#include "vertex.hpp"
#include "door.hpp"
#include <Eigen/Core>
#include <math.h>


//represents the player and everything related
//should probably contain the user-input in some way.
class Player : public Entity {
	//Player variables
	float angle_, anglesin_, anglecos_, yaw_, mouse_x, mouse_y;
	float default_z;
	std::vector<bool> keys_;
	bool isFalling = false;
	float activespeed_ = normalspeed_;
	int hp_ = max_hp;
	int doorCountdown = 0;
	int portalCountdown = 0; // The next time the player can check for a level-portal

	//Player constants
	float const normalspeed_ = 1.2f;
	float const sprintspeed_ = 2.0f;
	float const max_yaw = 5.5;
	float const HEADSIZE = 3;
	float const BODYHEIGHT = 10; //size of player
	float const CROUCHHEIGHT = BODYHEIGHT/2; //size of player
	float const KNEEHEIGHT = (BODYHEIGHT/2);
	int const event_radius = 5;
	int const max_hp = 100;

private:
	void move(Vector3f velo);			//add velocity to position of player
	bool checkForWall(Vector3f& velo);	//collision-detection
	bool checkForPortal(sector* n, Vector3f& velo, vertex a, vertex b);
	void crouchMove(bool isCrouch);		//crouch
	void jump(Vector3f& velo);			//jumps. Updates Z-axis etc
	void checkForEvent();				//checks for events close by
	void updatePOV();					//updates player POV (angle+yaw)
	void respawn();						//respawns player - set position to startPos_
	void takeDamage();					//deal dmg to player

	std::vector<float> closestToEvent(vertex a, vertex b, float middle_x, float middle_y);

public:
	Player(){};
	Player(Vector3f &pos) : Entity(pos){};
	void init(Vector3f pos);
	void init(Vector3f pos, Vector3f vel, Vector3f acc, sector* sec);
	void update();						//updates all vectors and values for player
	void shoot(std::vector<Enemy*>* enemies);
	void setMoveVector(std::vector<bool> &keys){ keys_ = keys; }
	void setMouseValues(float mx, float my) { mouse_x = mx; mouse_y = my; }

	float anglesin(){return anglesin_;};
	float anglecos(){return anglecos_;};
	float angle(){ return angle_; };
	float yaw() { return yaw_; }
	float x(){ return position()(0); };
	float y(){ return position()(1); };
	float z(){ return position()(2); }
	int hp(){ return hp_; };

};


#endif