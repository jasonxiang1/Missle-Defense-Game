#include "Controller.h"
#include <cmath>
#include <iostream>
#include <algorithm>
#include <vector>

#include <stdlib.h>


//TODO: Create out of frame explosions into consideration


namespace mrsd
{
	//declare global variables
	Prediction pred;
	std::vector<Prediction> predVect;
	// const int g.explosionSize = 5;

	//declare bin number vector
	int numBins;
	std::vector<int> BinVect; //0 means safe, 1 means danger
	float binInterval = 2.5;
	
	int currBin;
	int newBin;
	int minRad = 5;
	int safetyRad = 4;
	bool teleportationCheat = false;
	static int dirMem = 2; //0 for left,1 for right, 2 for none
	static bool dirConfirm = false;


	void Controller::control(const mrsd::Game& g, float t)
	{

		if (p != NULL) {

			//p->x = rand() % g.getWidth() + 1;
			//float time = g.getGameTime();

			//compute length of vector
			numBins = (int) (g.getWidth() / binInterval) + 1;

			for (int i = 0; i < numBins; i++) {
				if (i <= minRad || i >= (numBins-minRad))
				{
					BinVect.push_back(1);
				}else
				{
					BinVect.push_back(0);
				}

			}

			for (std::list<Projectile>::const_iterator it = g.getProjectiles().begin();
			it != g.getProjectiles().end(); ++it) {

				////randomized location algorithm
				//if (std::abs(p->x - it->x) < 20.0 && it->y <= 30.0) {
				//	p->x = it->x + 10.0 * (rand() % 2) - 2;

				//}

				//update each projectile's prediction

				//pass projectile information to prediction helper function
				pred = trackProjectile(*it,g); // will return an object with both pred.t and pred.x
				//push_back to predLoc variable
				predVect.push_back(pred);

				/*
				if(it->x + 20.0 >= g.getWidth()) {
					continue;
				}
				else if (it->x - 20.0 <= 0.0) {
					continue;
				}
				else {
					p->x = it->x + 10.0 * (rand() % 2) * 2 - 1;
					p->x = it->x + 5.0 * (rand() % 2) * 2 - 1;
				}
				*/

			}

			//run determineSafeSpots function
			determineSafeSpots(g);

			//compute the current bin the player is at
			currBin = (int)p->x / binInterval;


			if (teleportationCheat)
			{
				if (BinVect[currBin] == 1)
					{
						newBin = pickSafeSpot(g);
						//convert bin to floating point
						p->x = (float) (newBin * binInterval) + binInterval/2.0;
					}
			}else
			{
				if (BinVect[currBin] == 1)
					{
						if (!dirConfirm)
						{
							dirMem = pickSafeSpot(g);
							dirConfirm = true;
						}

						if (dirMem == 0) //move left
						{

							p->x = p->x - g.playerSpeed;


						}
						else if(dirMem == 1) //move right
						{

							p->x = p->x + g.playerSpeed;

						}
					}
				else if (BinVect[currBin] == 0)
				{
					dirMem = 2;
					dirConfirm = false;

				}
			}



			// if(currBin < safetyRad)
			// {
			// 	if (BinVect[currBin] == 1 && BinVect[currBin+safetyRad] == 1)
			// 	{
			// 		newBin = pickSafeSpot(g);
			// 		//convert bin to floating point
			// 		p->x = (float) (newBin * g.explosionSize) + g.explosionSize/2.0;
			// 	}

			// }
			// else if(currBin > (numBins-safetyRad))
			// {
			// 	if (BinVect[currBin] == 1 && BinVect[currBin-safetyRad] == 1)
			// 	{
			// 		newBin = pickSafeSpot(g);
			// 		//convert bin to floating point
			// 		p->x = (float) (newBin * g.explosionSize) + g.explosionSize/2.0;
			// 	}
				
			// }
			// else //if you are somewhere in the middle of the map
			// {
			// 	if (BinVect[currBin] == 1 || BinVect[currBin-safetyRad] == 1 || BinVect[currBin+safetyRad] == 1)
			// 	{
			// 		newBin = pickSafeSpot(g);
			// 		//convert bin to floating point
			// 		p->x = (float) (newBin * g.explosionSize) + g.explosionSize/2.0;
			// 	}

			// }

			predVect.clear();
			BinVect.clear();
			
		}

	}

	void Controller::createPlayer(Game& g)
	{
		if(p == 0)
		{
			p = new Player();
			p->dead = true;
		}
		if(p->dead)
		{
			p->dead = false;
			p->x = g.getWidth()/2;
			g.newPlayer(p);
		}
	}

	Prediction Controller::trackProjectile(const Projectile& p, const Game& g)
	{
		Prediction pred;
		float velFinal, delTime, delX, grav;

		//get gravity
		grav = g.getGravity();

		//compute predicted landing location based on each projectile's attributes
		//compute remaining air time of the projectile
		
		//float test = (2.0 * grav * p.y);
		velFinal = sqrt((p.vy * p.vy) + (2.0 * grav * -p.y)); //based on v^2 = v_o^2 + 2a(delX)
		delTime = (-(velFinal)+fabs(p.vy)) / grav; //based on v = v_o + at

		if (p.vy > 0)
		{
			//account for the time it takes to got to the air
			//based on the 
			delTime = delTime + 2 * (p.vy/fabs(grav)); //include the time it takes to convert the position y vel to negative
		}
		
		//compute landing location
		delX = p.vx * delTime; //based on delX = v_o*t + 0.5*a*t^2 where a_x = 0

		pred.t = delTime;
		pred.x = delX + p.x;


		return pred;
	}

	//create helper function to set a range of numbers to one
	void dangerSetter(int increBinLower, int increBinUpper)
	{
		for (int i = increBinLower; i <= increBinUpper; i++)
		{
			BinVect[i] = 1;
		}
	}

	void Controller::determineSafeSpots(const Game& g)
	{

		float timeThres = 25; //determine minimum time to start considering the projectiles
		int increBin;



		if (BinVect.size() > 0)
		{

			//iterate through prediction vector
			for (std::vector<Prediction>::const_iterator it = predVect.begin(); it != predVect.end(); ++it) {
				//if the projectile's time is less than 0.3, then compute the bin number and push back to vector
				if (it->t < timeThres && it->x < g.getWidth() && it->x > 0) {

					increBin = (int)(it->x / binInterval);
					dangerSetter(increBin-safetyRad,increBin+safetyRad);


				}
				else if (it->t < timeThres && it->x > g.getWidth())
				{
					dangerSetter(BinVect.size()-1-safetyRad,BinVect.size()-1);
				}
				else if (it->t < timeThres && it->x < 0)
				{
					dangerSetter(0,safetyRad);
				}
			}


			//iterate through explosion vector for 
			for (std::list<Explosion>::const_iterator it = g.getExplosions().begin();
				it != g.getExplosions().end(); ++it) {

				if (it->x < g.getWidth() && it->x > 0)
				{
					//compute the explosion and push back to vector
					increBin = (int)(it->x / binInterval);
					dangerSetter(increBin-safetyRad,increBin+safetyRad);
				}
				else if (it->x >= g.getWidth())
				{
					dangerSetter(BinVect.size()-1-safetyRad,BinVect.size()-1);
				}
				else if (it->x <= 0)
				{
					dangerSetter(0,safetyRad);

				}


			}


		}

	}

	int Controller::pickSafeSpot(const Game& g)
	{

		if(teleportationCheat)
		{
			std::vector<int>::iterator it;
			it = std::find(BinVect.begin(), BinVect.end(), 0);
			return std::distance(BinVect.begin(),it);
		}else
		{
			// int confRange = 4;
			//pick a direction that you want to go based on player speed
			//check left and right size for number of zeros
			int countLeft = std::count(BinVect.begin(),BinVect.begin()+currBin,0);
			int countRight = std::count(BinVect.begin()+currBin,BinVect.end(),0);
			if (countLeft > countRight) //if there is more zeros on the left side
			{

				return 0;

			}
			else if(countRight > countLeft) //if there is more zeros on the right size
			{


				return 1;

			}
			else
			{
				return std::rand()%2;
			}


		}


	}
}
