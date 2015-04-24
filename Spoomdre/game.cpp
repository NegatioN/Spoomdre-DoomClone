#include "game.hpp"
#include "player.hpp"
#include "sector.hpp"
#include "vertex.hpp"

Player player;
std::vector<sector*> sectors;
SDL_Rect rect;

//get window surface
void Game::makeRenderer(){
	//renderer = SDL_GetWindowSurface(window);
	renderer = SDL_CreateRenderer(window, -1, SDL_RENDERER_SOFTWARE);
	/*texture = SDL_CreateTexture(renderer,
        SDL_PIXELFORMAT_ARGB8888, SDL_TEXTUREACCESS_STATIC, 640, 480);*/
}

void Game::initialize(int height, int width) {
	//INIT SDL
	if(SDL_Init( SDL_INIT_VIDEO) < 0){
		printf("SDL Could not initialize! SDL_ERROR: %s\n",
			SDL_GetError());
	}else{
	//Create Window
		window = SDL_CreateWindow("SDL Hello World", 
			SDL_WINDOWPOS_UNDEFINED, SDL_WINDOWPOS_UNDEFINED, 
			width, height, SDL_WINDOW_SHOWN);
		if(window == NULL)
		{
			printf("Window could not be created! SDL_ERROR: %s\n", 
				SDL_GetError());
		}else{
			makeRenderer();
		}
	}

	width_ = width;
	height_ = height;

	createWorld();
	Vector3f position(5, 5, 20);
	Vector3f velocity(0, 0, 0);
	Vector3f acceleration(0, 0, -0.5);
	//player{position};
	//player.init(width/2, height/2, 20); // x, y, z
	player.init(position, velocity, acceleration, sectors[0]); // x, y, z
}

void Game::update(std::vector<bool> keys, int mouse_x, int mouse_y){
	player.setMoveVector(keys);
	player.setMouseValues(mouse_x, mouse_y);
	//player.move(0,0);
	player.update();
}

void Game::render() {
	// empty renderer from previous iteration
    SDL_RenderClear(renderer);

    // render player & world
    //player.render(renderer);

    player.render(renderer);
	for(auto s: sectors)
		s->render_map(renderer, player.x(), player.y(), player.z(), player.angle());

    // render crosshair
    SDL_SetRenderDrawColor(renderer, 0xAA, 0xAA, 0xAA, 0xAA);
    SDL_RenderDrawLine(renderer, width_/2-10, height_/2, width_/2+10, height_/2);
    SDL_RenderDrawLine(renderer, width_/2, height_/2-10, width_/2, height_/2+10);

	SDL_SetRenderDrawColor(renderer, 0,0,0,0); // background-color
    SDL_RenderPresent(renderer); // draw
}

void Game::terminate(){
	//Destroy renderer
	SDL_DestroyRenderer(renderer);

	//Destroy window
	SDL_DestroyWindow(window);

	//quit SDL Subsystems
	SDL_Quit();
}
void Game::wait(int milliseconds){
	SDL_Delay(milliseconds);
}

void fpsCounter(){
	
}

void Game::createWorld(){
	int id = 1;




//_______ NEW-MAP END ________//


	vertex v1 = vertex{0, 0};
	vertex v2 = vertex{10, 0};
	vertex v3 = vertex{50, 0};
	vertex v4 = vertex{60, 0};
	vertex v5 = vertex{10, 10};
	vertex v6 = vertex{50, 10};
	vertex v7 = vertex{10, 30};
	vertex v8 = vertex{50, 30};
	vertex v9 = vertex{10, 40};
	vertex v10 = vertex{50, 40};
	vertex v11 = vertex{10, 50};
	vertex v12 = vertex{50, 50}; 
	vertex v13 = vertex{0, 60};
	vertex v14 = vertex{10, 60};
	vertex v15 = vertex{50, 60};
	vertex v16 = vertex{60, 60};
	vertex v17 = vertex{0, 70};
	vertex v18 = vertex{10, 70};
	vertex v19 = vertex{50, 70};
	vertex v20 = vertex{60, 70};
	vertex v21 = vertex{0, 110};
	vertex v22 = vertex{10, 110};
	vertex v23 = vertex{50, 110};
	vertex v24 = vertex{0, 120};
	vertex v25 = vertex{50, 120};
	vertex v26 = vertex{60, 120};

	static sector s1{id++, 10.f, 80.f}, 
				  s2{id++, 10.f, 80.f},
				  s3{id++, 10.f, 80.f},
				  s4{id++, 15.f, 100.f},
				  s5{id++, 20.f, 100.f},
				  s6{id++, 25.f, 100.f},
				  s7{id++, 30.f, 100.f},

				  s8{id++, 35.f, 100.f},
				  s9{id++, 10.f, 30.f},
				  
				  s10{id++, 35.f, 90.f},
				  s11{id++, 10.f, 30.f},
				  
				  s12{id++, 35.f, 90.f},
				  s13{id++, 10.f, 30.f},

				  s14{id++, 10.f, 110.f},
				  
				  s15{id++, 35.f, 100.f},
				  s16{id++, 10.f, 30.f},
				  
				  s17{id++, 35.f, 100.f},
				  s18{id++, 10.f, 30.f},
				  
				  s19{id++, 35.f, 100.f},
				  s20{id++, 10.f, 30.f};

	s1.addVertex(v1);
	s1.addVertex(v2);
	s1.addVertex(v5);
	s1.addVertex(v7);
	s1.addVertex(v9);
	s1.addVertex(v11);
	s1.addVertex(v14);
	s1.addVertex(v13);


	s2.addVertex(v2);
	s2.addVertex(v3);
	s2.addVertex(v6);
	s2.addVertex(v5);
	

	s3.addVertex(v3);
	s3.addVertex(v4);
	s3.addVertex(v16);
	s3.addVertex(v15);
	s3.addVertex(v12);
	s3.addVertex(v10);
	s3.addVertex(v8);
	s3.addVertex(v6);


	s4.addVertex(v5);
	s4.addVertex(v6);
	s4.addVertex(v7);

	s5.addVertex(v6);
	s5.addVertex(v8);
	s5.addVertex(v7);
	
	s6.addVertex(v7);
	s6.addVertex(v8);
	s6.addVertex(v10);
	s6.addVertex(v9);
	
	s7.addVertex(v9);
	s7.addVertex(v10);
	s7.addVertex(v12);
	s7.addVertex(v11);
	

	s8.addVertex(v11);
	s8.addVertex(v12);
	s8.addVertex(v15);
	s8.addVertex(v19);
	s8.addVertex(v18);
	s8.addVertex(v14);
	
	s9.addVertex(v11);
	s9.addVertex(v12);
	s9.addVertex(v15);
	s9.addVertex(v19);
	s9.addVertex(v18);
	s9.addVertex(v14);


	s10.addVertex(v13);
	s10.addVertex(v14);
	s10.addVertex(v18);
	s10.addVertex(v17);
	
	s11.addVertex(v13);
	s11.addVertex(v14);
	s11.addVertex(v18);
	s11.addVertex(v17);


	s12.addVertex(v15);
	s12.addVertex(v16);
	s12.addVertex(v20);
	s12.addVertex(v19);
	
	s13.addVertex(v15);
	s13.addVertex(v16);
	s13.addVertex(v20);
	s13.addVertex(v19);


	s14.addVertex(v18);
	s14.addVertex(v19);
	s14.addVertex(v23);
	s14.addVertex(v22);
	
	
	s15.addVertex(v17);
	s15.addVertex(v18);
	s15.addVertex(v22);
	s15.addVertex(v21);
	
	s16.addVertex(v17);
	s16.addVertex(v18);
	s16.addVertex(v22);
	s16.addVertex(v21);
	

	s17.addVertex(v19);
	s17.addVertex(v20);
	s17.addVertex(v26);
	s17.addVertex(v25);
	s17.addVertex(v23);
	
	s18.addVertex(v19);
	s18.addVertex(v20);
	s18.addVertex(v26);
	s18.addVertex(v25);
	s18.addVertex(v23);


	s19.addVertex(v21);
	s19.addVertex(v22);
	s19.addVertex(v23);
	s19.addVertex(v25);
	s19.addVertex(v24);
	
	s20.addVertex(v21);
	s20.addVertex(v22);
	s20.addVertex(v23);
	s20.addVertex(v25);
	s20.addVertex(v24);

	s1.addNeighbour(&s2);
	s1.addNeighbour(&s4);
	s1.addNeighbour(&s5);
	s1.addNeighbour(&s6);
	s1.addNeighbour(&s7);
	s1.addNeighbour(&s8);
	s1.addNeighbour(&s9);
	s1.addNeighbour(&s10);
	s1.addNeighbour(&s11);
	

	s2.addNeighbour(&s1);
	s2.addNeighbour(&s4);
	s2.addNeighbour(&s3);
	
	s3.addNeighbour(&s2);
	s3.addNeighbour(&s5);
	s3.addNeighbour(&s6);
	s3.addNeighbour(&s7);
	s3.addNeighbour(&s8);
	s3.addNeighbour(&s9);
	s3.addNeighbour(&s12);
	s3.addNeighbour(&s13);
	
	s4.addNeighbour(&s1);
	s4.addNeighbour(&s2);
	s4.addNeighbour(&s5);

	s5.addNeighbour(&s3);
	s5.addNeighbour(&s4);
	s5.addNeighbour(&s6);


	s6.addNeighbour(&s1);
	s6.addNeighbour(&s3);
	s6.addNeighbour(&s5);
	s6.addNeighbour(&s7);

	s7.addNeighbour(&s1);
	s7.addNeighbour(&s3);
	s7.addNeighbour(&s6);
	s7.addNeighbour(&s8);

	s8.addNeighbour(&s1);
	s8.addNeighbour(&s3);
	s8.addNeighbour(&s7);
	s8.addNeighbour(&s10);
	s8.addNeighbour(&s12);
	s8.addNeighbour(&s14);

	s9.addNeighbour(&s1);
	s9.addNeighbour(&s3);
	s9.addNeighbour(&s7);
	s9.addNeighbour(&s11);
	s9.addNeighbour(&s13);
	s9.addNeighbour(&s14);
	
	s10.addNeighbour(&s1);
	s10.addNeighbour(&s8);
	s10.addNeighbour(&s15);
	
	s11.addNeighbour(&s1);
	s11.addNeighbour(&s9);
	s11.addNeighbour(&s16);

	s12.addNeighbour(&s3);
	s12.addNeighbour(&s8);
	s12.addNeighbour(&s17);

	s13.addNeighbour(&s3);
	s13.addNeighbour(&s9);
	s13.addNeighbour(&s18);

	s14.addNeighbour(&s8);
	s14.addNeighbour(&s9);
	s14.addNeighbour(&s15);
	s14.addNeighbour(&s16);
	s14.addNeighbour(&s17);
	s14.addNeighbour(&s18);
	s14.addNeighbour(&s19);
	s14.addNeighbour(&s20);


	s15.addNeighbour(&s10);
	s15.addNeighbour(&s14);
	s15.addNeighbour(&s19);

	s16.addNeighbour(&s11);
	s16.addNeighbour(&s14);
	s16.addNeighbour(&s20);


	s17.addNeighbour(&s12);
	s17.addNeighbour(&s14);
	s17.addNeighbour(&s19);

	s18.addNeighbour(&s13);
	s18.addNeighbour(&s14);
	s18.addNeighbour(&s20);


	s19.addNeighbour(&s15);
	s19.addNeighbour(&s14);
	s19.addNeighbour(&s17);

	s20.addNeighbour(&s16);
	s20.addNeighbour(&s14);
	s20.addNeighbour(&s18);
	

	sectors.push_back(&s1);
	sectors.push_back(&s2);
	sectors.push_back(&s3);
	sectors.push_back(&s4);
	sectors.push_back(&s5);
	sectors.push_back(&s6);
	sectors.push_back(&s7);
	sectors.push_back(&s8);
	sectors.push_back(&s9);
	sectors.push_back(&s10);
	sectors.push_back(&s11);
	sectors.push_back(&s12);
	sectors.push_back(&s13);
	sectors.push_back(&s14);
	sectors.push_back(&s15);
	sectors.push_back(&s16);
	sectors.push_back(&s17);
	sectors.push_back(&s18);
	sectors.push_back(&s19);
	sectors.push_back(&s20);

	//_______ NEW-MAP END ________//







/*

	//_______ OLD-MAP START ________//

	//Need static to save the sectors. (New causes wierd bug)
	static sector s1{id++, 10.f, 40.f}, 
				  s2{id++, 15.f, 45.f},
				  s3{id++, 5.f, 35.f};


	// vertexes for test-map
	vertex v1 = vertex{50, 50};
	vertex v2 = vertex{90, 50};
	vertex v3 = vertex{120, 75};
	vertex v4 = vertex{120, 100};
	vertex v5 = vertex{50, 100};

	vertex v6 = vertex{90, 10};
	vertex v7 = vertex{50, 10};

	vertex v8= vertex{120, 10};

	vertex v9 = vertex{120, 140};
	vertex v10= vertex{80, 140};
	vertex v11= vertex{40, 120};

	s1.addVertex(v1);
	s1.addVertex(v2);
	s1.addVertex(v3);
	s1.addVertex(v4);
	s1.addVertex(v5);

	s2.addVertex(v1);
	s2.addVertex(v7);
	s2.addVertex(v6);
	s2.addVertex(v2);

	s3.addVertex(v2);
	s3.addVertex(v6);
	s3.addVertex(v8);
	s3.addVertex(v3);


	s1.addNeighbour(&s2);
	s1.addNeighbour(&s3);
	s2.addNeighbour(&s1);
	s2.addNeighbour(&s3);
	s3.addNeighbour(&s1);
	s3.addNeighbour(&s2);

	sectors.push_back(&s1);
	sectors.push_back(&s2);
	sectors.push_back(&s3);


//_______ OLD-MAP END ________//
*/
	
}
