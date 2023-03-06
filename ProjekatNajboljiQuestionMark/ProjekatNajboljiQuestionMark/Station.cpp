#include "Station.h"

Station::~Station()
{
	for (auto u : in_)
		u = nullptr;
	for (auto u : out_)
		u = nullptr;
	in_.clear();
	out_.clear();
	routes_.clear();
	start_route_.clear();
}

vector<Station*>& Station::getOuts()
{
	return out_;
}

vector<Station*>& Station::getIns()
{
	return in_;
}

vector<pair<string, int>>& Station::getRoutes()
{
	return routes_;
}

vector<pair<string, int>>& Station::getStart()
{
	return start_route_;
}

string Station::getName() const
{
	return name_;
}

int Station::getPosition() const
{
	return position_;
}

int Station::getKey() const
{
	return key_;
}

string Station::getKeyS() const
{
	return key_s_;
}
