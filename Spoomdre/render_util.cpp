#include "render_util.hpp"


void render_util::renderView(SDL_Renderer* renderer, Player* player, int screenHeight, int screenWidth){
	float hfov = 0.73f*screenHeight; 		// Horizontal fov (Field of Vision)
	float vfov = 0.2f*screenHeight;    		// Vertical fov (Field of Vision)

	//keeps track of y-coord for neighbour-sector
	int ytop[screenWidth], ybottom[screenWidth];
	for(int i = 0; i < screenWidth; ++i){ ybottom[i] = screenHeight-1; ytop[i] = 0;}

	//get sector of player
	sector* playerSector = player->getSector();
	//define view of first sector (whole screen)
	sectorView playerSectorView{playerSector, 0, screenWidth-1};

	std::queue<sectorView> sectorRenderQueue;
	//std::queue<sector*> sectorRenderQueue;
	sectorRenderQueue.push(playerSectorView);


	while(!sectorRenderQueue.empty()){
		sectorView currentSectorView = sectorRenderQueue.front(); //get front-element from queue
		sector* currentSector = currentSectorView.thisSector;
		sectorRenderQueue.pop();	//remove front-element from queue

		///START RENDER SECTOR
		
		std::vector<vertex> vertices = currentSector->getVertices();	//verticies in sector
		int vertexCount = vertices.size();	//number of verticies in sector

		//draw lines for each vertex in sector
		for (int i = 0; i < vertexCount; i++) {
			vertex a = vertices[i]; vertex b = vertices[0];
			
			if (i < vertexCount-1)
				b = vertices[i+1];


			// x & y of sector-edge endpoints
	        float vertexAx = a.x() - player->x(); float vertexAy = a.y() - player->y();
	        float vertexBx = b.x() - player->x(); float vertexBy = b.y() - player->y();

	        float psin = player->anglesin(); float pcos = player->anglecos();
	      	//player translated coordinates of vertexes
	        float txA = vertexAx * psin - vertexAy * pcos; float tzA = vertexAx * pcos + vertexAy * psin;
	        float txB = vertexBx * psin - vertexBy * pcos; float tzB = vertexBx * pcos + vertexBy * psin;

            // Is wall at least partially in front of player
        	if(tzA <= 0 && tzB <= 0) continue;

        	// If partially behind player, clip it
	        if(tzA <= 0 || tzB <= 0)
	        {
	            float nearz = 1e-4f, farz = 5, nearside = 1e-5f, farside = 20.f;

	            // Intersection between wall and edges of player-view
	            xy i1 = gfx_util::intersect(txA,tzA,txB,tzB, -nearside,nearz, -farside,farz);
	            xy i2 = gfx_util::intersect(txA,tzA,txB,tzB,  nearside,nearz,  farside,farz);
	            if(tzA < nearz) { if(i1.y > 0) { txA = i1.x; tzA = i1.y; } else { txA = i2.x; tzA = i2.y; } }
	            if(tzB < nearz) { if(i1.y > 0) { txB = i1.x; tzB = i1.y; } else { txB = i2.x; tzB = i2.y; } }
	        }
            // Perspective transformation
	        float xscale1 = hfov / tzA; float yscale1 = vfov / tzA;  
	       	float xscale2 = hfov / tzB; float yscale2 = vfov / tzB; 
	       
	        int x1 = screenWidth/2 - (int)(txA * xscale1); 
	        int x2 = screenWidth/2 - (int)(txB * xscale2);

    		// Only render if it's visible (doesn't render the backside of walls)
        	if(x1 >= x2 || x2 < currentSectorView.leftCropX || x1 > currentSectorView.rightCropX) continue;

        	float playerZ = player->z();
	        // Ceiling&floor-height relative to player
        	float yceil  = currentSector->ceiling()  - playerZ;
        	float yfloor = currentSector->floor() - playerZ;

        	float nbrCeil=0; float nbrFloor=0;
        	

        	
        	//are current vertexes shared between neighbour-sector?
        	sector* neighbour = currentSector->getWallNeighbour(a, b);
        	//is this wall a door?
        	door* door_ = currentSector->getWallDoor(a,b);
	        if (neighbour != NULL)
	        {
	            nbrCeil  = neighbour->ceiling()  - playerZ;
	            nbrFloor = neighbour->floor() - playerZ;
	        }
	        float yaw = player->yaw();

	        // Project ceiling and floor heights to screen coordinates
	        int y1Ceil = screenHeight / 2 - (int) ((yceil + tzA * yaw) * yscale1);
	        int y1Floor = screenHeight / 2 - (int) ((yfloor + tzA * yaw) * yscale1);
	        int y2Ceil = screenHeight / 2 - (int) ((yceil + tzB * yaw) * yscale2);
	        int y2Floor = screenHeight / 2 - (int) ((yfloor + tzB * yaw) * yscale2);
	        
	        /* The same for the neighboring sector */
	        int nbrY1Ceil = screenHeight/2 - (int)((nbrCeil+ tzA * yaw) * yscale1);
	        int nbrY1Floor = screenHeight/2 - (int)((nbrFloor + tzA * yaw) * yscale1);
	        int nbrY2Ceil = screenHeight/2 - (int)((nbrCeil + tzB * yaw) * yscale2);
	        int nbrY2Floor = screenHeight/2 - (int)((nbrFloor + tzB * yaw) * yscale2);

            // Render the wall. 
        	int beginx = std::max(x1, currentSectorView.leftCropX);
        	int endx = std::min(x2, currentSectorView.rightCropX);

	        for(int x = beginx; x <= endx; ++x)
	        {
	            int top = ytop[x];
	            int bottom = ybottom[x];

	            int r_, g_, b_;
	            //Paint corners black
	            if (x == beginx || x == endx){ r_ = 5; g_ = 5; b_ = 5; }
	            else {r_ = 0xEE/currentSector->getId(); g_ = 0xBB; b_ = 0x77;}//Wall brown


	            // Calculate the Z coordinate for this point. (Only used for lighting.) 
	            int z_ = ((x - x1) * (tzB-tzA) / (x2-x1) + tzA) * 8;
	            int shade = (z_ - 16) / 4; // calculated from the Z-distance

	            // Acquire the Y coordinates for our ceiling & floor for this X coordinate. 
	            int yCeiling = (x - x1) * (y2Ceil-y1Ceil) / (x2-x1) + y1Ceil;// top
	            int yFloor = (x - x1) * (y2Floor-y1Floor) / (x2-x1) + y1Floor;// bottom
                int cropYCeiling = gfx_util::clamp(yCeiling, top, bottom);
                int cropYFloor = gfx_util::clamp(yFloor, top, bottom);
	            
                //Draw floor and ceiling
                unsigned roofColor = 0x99/currentSector->getId();
                //if(yCeiling > top )
                render_util::drawVLine(renderer, x, top, cropYCeiling-1, roofColor, roofColor, roofColor, 1);
                //if(yFloor < bottom )
                render_util::drawVLine(renderer, x, cropYFloor+1, bottom, 0x66, 0x33, 0x00, 1);
	            
	            /* Is there another sector behind this edge? */
	           if(neighbour != NULL)
	            {
	                //Find their floor and ceiling
	                //int nya = 1;
	                int nbrYCeil = (x - x1) * (nbrY2Ceil-nbrY1Ceil) / (x2-x1) + nbrY1Ceil;
	                int nbrYFloor = (x - x1) * (nbrY2Floor-nbrY1Floor) / (x2-x1) + nbrY1Floor;
	                nbrYCeil = gfx_util::clamp(nbrYCeil, top, bottom);   //clamp ceiling of neighbour to our POV
	                nbrYFloor = gfx_util::clamp(nbrYFloor, top, bottom); //clamp floor of neighbour to our POV

	                // If our ceiling is higher than their ceiling, render upper wall                 
	                render_util::drawVLine(renderer, x, cropYCeiling, nbrYCeil-1, r_, g_, b_, shade);       // Between our and their ceiling

	                // If our floor is lower than their floor, render bottom wall 
	                render_util::drawVLine(renderer,x, nbrYFloor+1, cropYFloor, r_, g_, b_, shade);         // Between their and our floor

	                //std::cout << "Sector=" << neighbour->getId() << " top=" << top << " bottom=" << bottom << std::endl;
	                ytop[x] = gfx_util::clamp(std::max(cropYCeiling, nbrYCeil), top, screenHeight-1);    // Shrink the remaining window below these ceilings
	                ybottom[x] = gfx_util::clamp(std::min(cropYFloor, nbrYFloor), 0, bottom); // Shrink the remaining window above these floors
	            
	            }
	            else{
	                
	                render_util::drawVLine(renderer, x, cropYCeiling, cropYFloor, r_, g_, b_, shade);
	            }



	        }

	        //add sector-neighbours to renderQueue
	        bool isDoorLocked = (door_ != NULL && door_->doorLocked());
	        if(neighbour != NULL && endx >= beginx && !(isDoorLocked)){
	        	sectorView nbrSectorView{neighbour, beginx, endx};
	        	sectorRenderQueue.push(nbrSectorView);
	        }

		}

		///END RENDER SECTOR
/*

		//add sector-neighbours to renderQueue
		for(auto nbrSector : currentSector->getNeighbours()){
			sectorView nbrSectorView{nbrSector, 0, screenWidth-1};
			sectorRenderQueue.push(nbrSectorView);
		}
		*/
	}


}

void render_util::renderSector(sector currentSect){

}


/* vline: Draw a vertical line on screen, with a different color pixel in top & bottom */
/*
void render_util::drawVLine(SDL_Renderer* renderer, Linedef line, float height, float width, int red, int green, int blue, int shade)
{
    //y1 = gfx_util::clamp(y1, 0, height-1);
    //y2 = gfx_util::clamp(y2, 0, height-1);

    vertex left = line.left();
    vertex right = line.right();
    // if calculated shade (color - shade) is under the threshold, set the color as the threshold
    SDL_SetRenderDrawColor(renderer, (red - shade < red / 4) ? red / 4 : red - shade, (green - shade < green / 4) ? green / 4 : green - shade, (blue - shade < blue / 4) ? blue / 4 : blue - shade, 0xFF);//vegg
    SDL_RenderDrawLine(renderer, left.x(), left.y(), right.x(), right.y());

    //Borders?
    SDL_SetRenderDrawColor(renderer, 5, 5, 5, 0.5f);
    SDL_RenderDrawPoint(renderer, left.x(), left.y();
    SDL_RenderDrawPoint(renderer, left.x(), right.y());
}
*/

/* vline: Draw a vertical line on screen, with a different color pixel in top & bottom */
void render_util::drawVLine(SDL_Renderer* renderer, int x1, int y1,int y2, int red, int green, int blue, int shade)
{
	int H = 480; // window-height
	int W = 640; // window-width
    y1 = gfx_util::clamp(y1, 0, H-1);
    y2 = gfx_util::clamp(y2, 0, H-1);

    // if calculated shade (color - shade) is under the threshold, set the color as the threshold
    SDL_SetRenderDrawColor(renderer, (red - shade < red / 4) ? red / 4 : red - shade, (green - shade < green / 4) ? green / 4 : green - shade, (blue - shade < blue / 4) ? blue / 4 : blue - shade, 0xFF);//vegg
    SDL_RenderDrawLine(renderer, x1, y1, x1, y2);

    //Borders?
    SDL_SetRenderDrawColor(renderer, 5, 5, 5, 0.5f);
    SDL_RenderDrawPoint(renderer, x1, y1);
    SDL_RenderDrawPoint(renderer, x1, y2);
}


//Temporary method for showing a top-down view on screen.
void render_util::render_map(SDL_Renderer* renderer, float px, float py, float pz, float angle, std::vector<vertex> vertices){

	int vCount = vertices.size();
    
    int yOffset = -100; // displaces map by a given y
    int xOffset = 150;  // displaces map by a give x

    // Render player on map
    SDL_Rect prect;
    prect.w = 5; prect.h = 5;
    prect.x = (320+xOffset) - prect.w / 2; prect.y = (240+yOffset) - prect.h / 2;

    SDL_SetRenderDrawColor(renderer, 0x55, 0xFF, 0x55, 0xFF); // map-color, Blue/green-ish
    SDL_RenderDrawLine(renderer, 320 + xOffset, 240 + yOffset, 320 + xOffset, 232 + yOffset); // render map 
    SDL_SetRenderDrawColor(renderer, 0xBB, 0xBB, 0xBB, 0xFF); // map-color, Blue/green-ish
    SDL_RenderFillRect(renderer, &prect); // render map 

    // Render map
    for (int i = 0; i < vCount; i++) {
        vertex a = vertices[i];
        vertex b = vertices[0];
        if (i < vCount-1)
            b = vertices[i+1];

        //Calculate 
        float txa = a.x()-px, tya = a.y()-py;
        float txb = b.x()-px, tyb = b.y()-py;

        float tza = txa*cos(angle) + tya*sin(angle);
        float tzb = txb*cos(angle) + tyb*sin(angle);
              txa = txa*sin(angle) - tya*cos(angle);
              txb = txb*sin(angle) - tyb*cos(angle);

        SDL_SetRenderDrawColor(renderer, 0x00, 0x77, 0xFF, 0xFF); // map-color, Blue/green-ish
        SDL_RenderDrawLine(renderer, 320-txa + xOffset, 240-tza + yOffset, 320-txb + xOffset, 240-tzb + yOffset); // render map
        SDL_SetRenderDrawColor(renderer, 0xFF, 0xFF, 0x00, 0xFF); // wall-color, Yellow
    }
}

