#include "Routes.h"

Routes::~Routes()
{
	name_stations_.clear();
	position_stations_.clear();
}

vector<int>& Routes::getStationsPosition()
{
	return position_stations_;
}

vector<string>& Routes::getStationsNames()
{
	return name_stations_;
}

string Routes::getName() const
{
	return name_;
}

int Routes::getNumOfStations() const
{
	return num_of_stations_;
}

int Routes::getPosOfSwitch() const
{
	return pos_of_switch_;
}

void Routes::setPosOfSwitch(const int& pos)
{
	pos_of_switch_ = pos;
}

void Routes::setNumOfStations(const int& count)
{
	num_of_stations_ = count;
}
