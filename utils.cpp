#include"CMPSO.h"
#include<iostream>
#include<algorithm>
//#include<iomanip>

using namespace std;

/******************************************************/
/* Initialize objective bounds and particles' message */
/******************************************************/
void Initialization() {
	//Initialize objective bounds
	for (int i = 0; i < DIMENSION; ++i) {
		obj_bunds[i].pos_max = 1.0;
		obj_bunds[i].pos_min = 0.0;
		obj_bunds[i].v_max = 0.2 * (obj_bunds[i].pos_max - obj_bunds[i].pos_min);
		obj_bunds[i].v_min = -1 * obj_bunds[i].v_max;
	}
	//Initialize particles' message
	for (int i = 0; i < OBJECTIVE_NUM; ++i) {
		gBest[i].fitness = DBL_MAX;
		for (int j = 0; j < PARTICLE_NUM; ++j) {
			for (int k = 0; k < DIMENSION; ++k) {
				Swarms[i][j].pos.push_back(random(obj_bunds[k].pos_min, obj_bunds[k].pos_max));	//Initialize position
				Swarms[i][j].v.push_back(random(obj_bunds[k].v_min, obj_bunds[k].v_max));		//Initialize velocity
			}
			Swarms[i][j].fitness = CalFitness(Swarms[i][j].pos);	//Fitness evaluate
			Swarms[i][j].pBest = Swarms[i][j].pos;					//Initialize pBest
			Swarms[i][j].pBest_fitness = Swarms[i][j].fitness;
			
			if (Swarms[i][j].pBest_fitness[i] < gBest[i].fitness) {
				gBest[i].pos = Swarms[i][j].pBest;					//Initialize gBest
				gBest[i].fitness = Swarms[i][j].pBest_fitness[i];
			}
		}
	}
}

/***************************************************************************/
/* Objective function implement here, input a particle's position, return  */
/* m dimension vector f. A two dimension objective function ZDT1 is shown  */
/* here, where f[0] means the first objective value, f[1] means the second */
/* objective value.														   */
/***************************************************************************/
vector<double> CalFitness(vector<double> pos) {
	vector<double> f(OBJECTIVE_NUM);
	f[0] = pos[0];
	double gx = 0;
	for (int i = 1; i < DIMENSION; ++i) {
		gx += pos[i];
	}
	gx = 1.0 + gx * (9.0 / (DIMENSION - 1));
	double hx = 1 - sqrt(f[0] / gx);
	f[1] = gx * hx;
	return f;
}

bool dominates(Archive U, Archive W) {
	for (int i = 0; i < OBJECTIVE_NUM; ++i) {
		if (U.fitness[i] > W.fitness[i])
			return false;
	}
	return true;
}

bool operator == (const Archive a, const Archive b) {
	if (a.pos == b.pos) return true;
	else return false;
}

bool operator != (const Archive a, const Archive b) {
	if (a.pos != b.pos) return true;
	else return false;
}

double GetMaxFtns(int swarm_num) {
	double temp = DBL_MIN;
	for (auto ptc : Swarms[swarm_num]) {
		if (ptc.fitness[swarm_num] > temp) temp = ptc.fitness[swarm_num];
	}
	return temp;
}

double GetMinFtns(int swarm_num) {
	double temp = DBL_MAX;
	for (auto ptc : Swarms[swarm_num]) {
		if (ptc.fitness[swarm_num] < temp) temp = ptc.fitness[swarm_num];
	}
	return temp;
}

void SortRwithObjVal(vector<Archive>& R, int obj_num, vector<double>& d) {
	int size = R.size();
	for (int i = size - 1; i > 0; --i) {
		for (int j = 0; j < i; j++) {
			if (R[j].fitness[obj_num] >= R[j + 1].fitness[obj_num]) {
				swap(d[j], d[j + 1]);
				swap(R[j], R[j + 1]);
			}
		}
	}
}

void SortRwithD(vector<Archive>& R, vector<double>& d) {
	int size = d.size();
	for (int i = size - 1; i > 0; --i) {
		for (int j = 0; j < i; j++) {
			if (d[j] <= d[j + 1]) {
				swap(d[j], d[j + 1]);
				swap(R[j], R[j + 1]);
			}
		}
	}
}

double GetWeight() {
	return wMax - (wMax - wMin) * t;
	//return (wMax - wMin) * (t - 1) * (t - 1) + wMin;
}

void ArchiveFilter(vector<Archive>& S) {
	vector<Archive> temp;
	for (auto i : S) {
		bool flag = false;
		for (auto j : temp) {
			if (i == j) {
				flag = true;
				break;
			}
		}
		if (flag == false) temp.push_back(i);
	}
	S = temp;
}

void print(vector<Archive> Arc, int size) {
	for (int i = 0; i < size; ++i) {
		//cout << setiosflags(ios::left) << setw(3) << i + 1 << ": ";
		for (int j = 0; j < OBJECTIVE_NUM; ++j) {
			cout << Arc[i].fitness[j] << " ";
		}
		cout << endl;
	}
}

double Gaussian(double mu, double sigma) {
	const double epsilon = numeric_limits<double>::min();
	const double two_pi = 2.0 * PI;

	static double z0, z1;
	static bool generate;
	generate = !generate;
	if (!generate) return z1 * sigma + mu;

	double u1, u2;
	do {
		u1 = rand() * (1.0 / RAND_MAX);
		u2 = rand() * (1.0 / RAND_MAX);
	} while (u1 <= epsilon);
	z0 = sqrt(-2.0 * log(u1)) * cos(two_pi * u2);
	z1 = sqrt(-2.0 * log(u1)) * sin(two_pi * u2);
	return z0 * sigma + mu;
}