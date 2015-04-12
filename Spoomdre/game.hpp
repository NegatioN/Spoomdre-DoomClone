#ifndef CLASS_GAME_H
#define CLASS_GAME_H


#include <SDL2/SDL.h>
#include <stdio.h>
#include <cstdlib>
#include <iostream>


//Defines the underlaying rendering-process
class Game{

	SDL_Window* window = NULL; 			//Window to render to
	SDL_Surface* screenSurface = NULL; 	//surface of window

	void makeSurface();

public:

	//Game(int height, int width);
	void initialize(int height, int width); //initialize the screen and surface
	void update();					//updates the surface
	void fillRect(SDL_Rect rect);	//renders rectangle on screen
	void terminate();				//terminate screen
	void wait(int seconds);			//pause render for x seconds
	

};


#endif