#ifndef CLASS_PLAYER_H
#define CLASS_PLAYER_H

#include <vector>
#include "entity.hpp"
#include <Eigen/Core>

//represents the player and everything related
//should probably contain the user-input in some way.
class Player : public Entity {
	float angle_, anglesin_, anglecos_, yaw_, mouse_x, mouse_y;
	//sector * sector_; -> is in entity 
	std::vector<bool> wasd_;

public:
	Player(){};
	Player(Vector3f &pos) : Entity(pos){};
	void init(Vector3f pos);
	void init(Vector3f pos, Vector3f vel, Vector3f acc);
	void init(int x, int y, int z);
	void update();
	void move(float dx, float dy);
	void render(SDL_Renderer* renderer);

	void setMoveVector(std::vector<bool> &wasd){ wasd_ = wasd; }
	void setMouseValues(float mx, float my) { mouse_x = mx; mouse_y = my; }

	float angle(){ return angle_; };
	float yaw() { return yaw_; }
	float x(){ return position()(0); };
	float y(){ return position()(1); };
	float z(){ return position()(2); }

};


#endif