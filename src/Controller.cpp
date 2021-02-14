#include "Controller.h"
#include <cmath>
#include <iostream>
#include <algorithm>

#include <stdlib.h>


//TODO: Create out of frame explosions into consideration


namespace mrsd
{
	//declare global variables
	Prediction pred;
	std::vector<Prediction> predVect;
	const int increVal = 10;

	//declare bin number vector
	int numBins;
	std::vector<int> BinVect; //0 means safe, 1 means danger
	
	int currBin;
	int newBin;

	void Controller::control(const mrsd::Game& g, float t)
	{

		if (p != NULL) {

			//p->x = rand() % g.getWidth() + 1;
			//float time = g.getGameTime();

			//compute length of vector
			numBins = (int) (g.getWidth() / increVal) + 1;

			for (int i = 0; i < numBins; i++) {
				BinVect.push_back(0);
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
			currBin = (int)p->x / increVal;

			if (BinVect[currBin] == 1)
			{
				newBin = pickSafeSpot(g);

				//convert bin to floating point
				p->x = (float) (newBin * increVal) + increVal/2.0;

			}
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
		delTime = (-(velFinal)-p.vy) / grav; //based on v = v_o + at

		if (p.vy > 0)
		{
			//account for the time it takes to got to the air
			delTime = delTime + 2 * (p.vy/abs(grav)); //include the time it takes to convert the position y vel to negative
		}
		
		//compute landing location
		delX = p.vx * delTime; //based on delX = v_o*t + 0.5*a*t^2 where a_x = 0

		pred.t = delTime;
		pred.x = delX + p.x;


		return pred;
	}

	void Controller::determineSafeSpots(const Game& g)
	{

		float timeThres = 1.5;
		int increBin;



		if (BinVect.size() > 0)
		{

			//iterate through prediction vector
			for (std::vector<Prediction>::const_iterator it = predVect.begin(); it != predVect.end(); ++it) {
				//if the projectile's time is less than 0.3, then compute the bin number and push back to vector
				if (it->t < timeThres && it->x < g.getWidth() && it->x > 0) {

					increBin = (int)(it->x / increVal);
					BinVect[increBin] = 1;


				}
				else if (it->t < timeThres && it->x > g.getWidth())
				{
					BinVect.back() = 1;
				}
				else if (it->t < timeThres && it->x < 0)
				{
					BinVect[0] = 1;
				}
			}


			//iterate through explosion vector for 
			for (std::list<Explosion>::const_iterator it = g.getExplosions().begin();
				it != g.getExplosions().end(); ++it) {

				if (it->x < g.getWidth() && it->x > 0)
				{
					//compute the explosion and push back to vector
					increBin = (int)(it->x / increVal);
					BinVect[increBin] = 1;
				}


			}


		}

	}

	int Controller::pickSafeSpot(const Game& g)
	{

		std::vector<int>::iterator it;
		
		it = std::find(BinVect.begin(), BinVect.end(), 0);


		return std::distance(BinVect.begin(),it);
	}
}
