#include <SDL2/SDL.h>
#include "player.hpp"
#include <iostream>

using namespace std;

SDL_Rect sprite;

bool isCrouching = false;

void Player::init(Vector3f pos, Vector3f vel, Vector3f acc, sector* sec){
	setPosition(pos); setVelocity(vel); setAcceleration(acc);
	setSector(sec); setStartPos(pos); setStartSector(sec);
	default_z = sec->floor() + BODYHEIGHT; 
	yaw_ = 0;
	hp_ = max_hp;
}

//Take input accelleration-vector?
void Player::update() {
	isFalling = false; isCrouching = false; bool isJumping = false;
	if(doorCountdown > 0) --doorCountdown;
	if(portalCountdown > 0) --portalCountdown;
	
	anglesin_ = sin(angle_); anglecos_ = cos(angle_); 	//define angle
	Vector3f vecAddition(0,0,0);						//vector to add movement to

	updatePOV(); //update POV based on angle + yaw

	//if player z is higher than sector z+bodyheight, we should be falling
	if(z() > default_z)
		isFalling = true;
	else
		setVelocity(vecAddition); //set velocity (0,0,0). No sliding movement

	if(isFalling){
		 //Crouching while jumping. Not visible, but makes it possible to jump into small openings.
		if (keys_.at(8)) { isCrouching = true;}

		Vector3f fallingVelo = velocity(); Vector3f pos = position();

		float maxHeight = getSector()->ceiling();
		//if player bumps against roof of sector, set z-velo to zero and z to maxHeight
		if(pos(2) + fallingVelo(2) >= maxHeight){
			//fallingVelo(2) = 0; //player no longer acends if he hits roof
			pos(2) = maxHeight; // 
			setPosition(pos);
		}
		move(fallingVelo);
		fallingVelo += accelleration();	//gravity
		checkForWall(fallingVelo);
		setVelocity(fallingVelo);
	}else{
	    // keyboard-events
	    if (keys_.at(0)) { vecAddition(0) += anglecos_  * activespeed_; vecAddition(1)  += anglesin_ * activespeed_; } 	// W
	    if (keys_.at(1)) { vecAddition(0) += anglesin_ * activespeed_; vecAddition(1) -= anglecos_  * activespeed_; } 	// A
	    if (keys_.at(2)) { vecAddition(0) -= anglecos_  * activespeed_; vecAddition(1) -= anglesin_ * activespeed_; } 	// S
	    if (keys_.at(3)) { vecAddition(0) -= anglesin_ * activespeed_; vecAddition(1) += anglecos_  * activespeed_; } 	// D
	    if (keys_.at(8)) { isCrouching = true;}													//Crouch, Z-axis
	    if (keys_.at(9)) { isJumping = true; } 													//Jump, Z-axis
	    if (keys_.at(11)){ if(doorCountdown == 0) checkForEvent(); }							//Interact
	    if (keys_.at(12)){ activespeed_ = sprintspeed_; } else {activespeed_ = normalspeed_;}	//Sprint
	    if (keys_.at(13)){ respawn();}	

	    // set moving to true if movement-key is pressed
	    bool pushing = false;
	    if(keys_.at(0) || keys_.at(1) || keys_.at(2) || keys_.at(3))
	    	pushing = true;

		float accel = pushing ? 0.4 : 0.2;

	    Vector3f vel = velocity();
	    //Vector3f crouchVelocity = velocity();
	    vel(0) = vel(0) * (1 - accel) + vecAddition(0) * accel;
	    vel(1) = vel(1) * (1 - accel) + vecAddition(1) * accel;

	    // set moving to true if movement-key is pressed
	    bool moving = false;
	    if(pushing) moving = true; //if player pushing button, we're moving
	    	
		if(moving) {
		    checkForWall(vel); //Is the player hitting a wall?
		    move(vel); //change position
		}
		if(isJumping){
			jump(vel);
		}
		else
			crouchMove(isCrouching);
	}
}
//true, hits wall or goto next sector
bool Player::checkForWall(Vector3f& velo){
	std::vector<vertex> vertices = getSector()->getVertices();
	std::vector<sector*> neighbours = getSector()->getNeighbours();

	// Check if the player is about to cross one of the sector's edges 
    for(int i = 0; i < vertices.size(); ++i){
        vertex a = vertices[i]; vertex b = vertices[i+1];
	    
	    //Loop around for last corner
	    if (i == vertices.size()-1) b = vertices[0];

        if( gfx_util::intersectBox(x(), y(), x()+velo(0),y()+velo(1), a.x(), a.y(), b.x(), b.y()) && 
            gfx_util::pointSide(x()+velo(0), y()+velo(1), a.x(), a.y(), b.x(), b.y()) < 0)
        { 
			for (sector* n: neighbours){
				if(checkForPortal(n, velo, a, b)) //did we hit a portal?
					return true;
			}
			//Bumps into a wall! Slide along the wall. 
			// This formula is from Wikipedia article "vector projection". 
			float xd = b.x() - a.x(), yd = b.y() - a.y();
			velo(0) =  xd * (velo(0)*xd + yd*velo(1)) / (xd*xd + yd*yd);
			velo(1) =  yd * (velo(0)*xd + yd*velo(1)) / (xd*xd + yd*yd);

			// //will you slide past this wall?
			if( (min(a.x(), b.x()) > x()+velo(0) || x()+velo(0) > max(a.x(), b.x())) && 
				(min(a.y(), b.y()) > y()+velo(1) || y()+velo(1) > max(a.y(), b.y()))  ){
				velo(0) = 0; velo(1) = 0; //if so, stop player
			}
		}
    }
    return false;
}

bool Player::checkForPortal(sector* n, Vector3f& velo, vertex a, vertex b){
	if (n->containsVertices(a, b)){ 
		float hole_low  = n < 0 ?  9e9 : max(getSector()->floor(), n->floor());//height of the heigest floor - gives opening
		float hole_high = n < 0 ? -9e9 : min(getSector()->ceiling(),  n->ceiling());//height of the lowest floor- gives opening
		float floor_diff = n->floor() - getSector()->floor();// height differens of sector floors
		
		//is this wall a door? and if so, is it locked?
		door* door_ = n->getWallDoor(a,b);
		bool isDoorLocked = (door_ != NULL && door_->doorLocked());

		// can player walk/jump through opening?
    	if(((hole_high - hole_low) >= ((isCrouching ? CROUCHHEIGHT : BODYHEIGHT)+HEADSIZE)) && (z() <= hole_high) && !isDoorLocked &&
    		((!isFalling && floor_diff <= KNEEHEIGHT) || (isFalling && z()-KNEEHEIGHT >= hole_low))) 
		{
		   	setSector(n);
		   	//after changing sector, will you hit a wall?
			if( (min(a.x(), b.x()) > x()+velo(0) || x()+velo(0) > max(a.x(), b.x())) && 
				(min(a.y(), b.y()) > y()+velo(1) || y()+velo(1) > max(a.y(), b.y()))  ){
					velo(0) = 0; velo(1) = 0; //if so, stop player
			}
			move(velo);

		   	//sets default_z to floor + BodyHeight. Player will move towards this next frame
		   	default_z = getSector()->floor() + BODYHEIGHT;
		   	setVelocity(velo);		//if we fall after sector-change we fall forward.
	    	return true;
   		}	
	}
	return false;
}

void Player::crouchMove(bool isCrouch){
	Vector3f crouch(0,0,0.9);
	//we need to modify default_z when moving up/down on z-plane
	//if lower than highlimit, and player not crouching

	if(z() < default_z && !isCrouch){
		//to avoid triggering isFalling by going over default_z
		Vector3f pos = position();
		pos += crouch;
		float dz = pos(2) - default_z;
		if(pos(2) >= default_z)
			crouch(2) -= (pos(2) - default_z); //subtracts z-overflow. sets to default_z

		move(crouch);
	}
	//if higher than lowlimit, and player crouching
	else if(z() >= (default_z - 8) && isCrouch)
		move(-crouch);
}

//make player jump, move player according to velo
void Player::jump(Vector3f& velo){
	velo(2) = 2.8; //set z-velocity to 2.8
	setVelocity(velo);

	Vector3f pos = position();
	pos(2) = default_z + 0.1; //make Z one higher than default to trigger falling-check.

	checkForWall(velo); //are we right about to change sectors?
	setPosition(pos);	
}


void Player::move(Vector3f velo) {
	Vector3f pos = position();
	pos += velo;
	setPosition(pos);
}

/**
*This method is called if player presses 'e'
*Checks for any "events"(for now just doors) close by
*if it finds any event, activate it(open door)
*/
void Player::checkForEvent(){
	//so door cant be spammed
	doorCountdown = 10;
	//all doors
	std::vector<door*> doors = getSector()->getDoors();
	//sector has door // should be has "event" (when we add more)
	if(doors.size() > 0)
	{
		door* active = NULL;
		float margin_x = 0, margin_y = 0; float x_, y_;
		//loops through all doors, one in range. if more, finds closest
		for(door* e : doors)
		{
			vertex a = e->getDoorA();
			vertex b = e->getDoorB();

			float x_middle, y_middle;

			//mid point of event(door)
			x_middle = (std::abs(b.x()-a.x())/2) + min(a.x(),b.x());
			y_middle = (std::abs(b.y()-a.y())/2) + min(a.y(),b.y());

			//returns closest of the three points, a, middle or b
			std::vector<float> closest_point = closestToEvent(a,b,x_middle,y_middle);

			float temp_x = closest_point.at(0);
			float temp_y = closest_point.at(1);

			//checks if in range, and if closer than the others
			if(temp_x <= event_radius && temp_y <= event_radius){
				if(active == NULL)
				{
					margin_x = temp_x;
					margin_y =temp_y;
					active = e;
				}
				else if(temp_x < margin_x && temp_x < margin_y)
				{
					margin_x = temp_x;
					margin_y =temp_y;
					active = e;
				}
			}
		}

		//if a event it found, use it
		if(active != NULL)
		{
			active->setDoorLocked();
		}
	}
}

//checks which point is closest to player(a-middle-b)
std::vector<float> Player::closestToEvent(vertex a, vertex b, float middle_x, float middle_y){
	float margin_a_x, margin_a_y, margin_middle_x,margin_middle_y, margin_b_x,margin_b_y,
		total_a, total_b, total_middle;
	std:vector<float> margin;

	Vector3f pos = position();
	float x = pos(0);
	float y = pos(1);

	//margin from player to point a
	margin_a_x = std::abs(x - a.x()); margin_a_y = std::abs(y - a.y()); total_a = margin_a_x + margin_a_y;

	//margin from player to point middle
	margin_middle_x = std::abs(x - middle_x); margin_middle_y = std::abs(y - middle_y); total_middle = margin_middle_x + margin_middle_y;

	//margin from player to point b
	margin_b_x = std::abs(x - b.x()); margin_b_y = std::abs(y - b.y()); total_b = margin_b_x + margin_b_y;

	//returns the point with the least margin to player
	if(total_a < total_b && total_a < total_middle) //a is closest
	{
	
		margin.push_back(margin_a_x);
		margin.push_back(margin_a_y);
	}else if(total_b < total_middle && total_b < total_a){ //middle is closest
		margin.push_back(margin_b_x);
		margin.push_back(margin_b_y);
	}else{ 											//b is closest
		margin.push_back(margin_middle_x);
		margin.push_back(margin_middle_y);
	}
	return margin;
}

void Player::updatePOV(){
	//Keyboard-events for POV (Yaw+angle)

    if (keys_.at(4)) { angle_ += 0.1; }		// right
    if (keys_.at(5)) { angle_ -= 0.1; }		// left
    if (keys_.at(6)) { if(yaw_ > -max_yaw ) yaw_ -= 0.1; }		// up
    if (keys_.at(7)) { if(yaw_ < max_yaw ) yaw_ += 0.1; }		// down

    //Mouse-events for POV (Yaw+Angle)
	if(mouse_x != 0) angle_ = mouse_x * 0.015f;
	if(mouse_y != 0) yaw_ = -gfx_util::clamp(-mouse_y * 0.023f, -5, 5);
}


void Player::shoot(std::vector<Enemy*>* enemies){
    Vector3f playerPos = position(); Vector3f direction(0,0,0);
    direction(0) += anglecos();  direction(1)  += anglesin(); //create direction-vector
    direction.normalize();

    for(int i = 0; i < enemies->size(); i++){

    	Enemy* enemy = enemies->at(i);
    	if(enemy->getRender()){ //if enemy is rendered, we can check hitScan
	    	bool isHit = gfx_util::hitScan(playerPos, enemy->position(),enemy->getRect(), direction);
	        if(isHit){
	        	enemies->erase(enemies->begin()+i); //kill enemy
	        	break;
	        }
	    }
    }
}

void Player::respawn(){ // resets all important values(for player) for respawn
	setSector(getStartSector()); setPosition(getStartPos());
	default_z = getSector()->floor() + BODYHEIGHT; 
	yaw_ = 0; angle_ = 0; anglesin_ = 0; anglecos_ = 0;
	hp_ = max_hp;
}

void Player::takeDamage(){
	hp_ -= (max_hp/3);

	if(hp_ <= 1){
		std::cout << "Player is dead!" << std::endl;
		respawn();
	}
}