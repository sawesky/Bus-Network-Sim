#ifndef ROUTES_H
#define ROUTES_H

#include<string>
#include<vector>

using namespace std;
/*
Klasa Routes predstavlja linije u grafu
*/
class Routes {
public:
	//konstruktori
	Routes() : position_stations_({}), name_stations_({}), num_of_stations_(0), name_(""), pos_of_switch_(0) {};
	Routes(const string& name) :name_(name), num_of_stations_(0), pos_of_switch_(0), position_stations_({}), name_stations_({}) {};

	//destruktori
	~Routes();

	//blokirano
	Routes(const Routes& r) = delete;
	Routes(Routes&& r) = delete;
	
	//getteri i setteri promenljivih objekta klase Routes
	vector<int>& getStationsPosition();
	vector<string>& getStationsNames();
	string getName() const;
	int getNumOfStations() const;
	int getPosOfSwitch() const;

	void setPosOfSwitch(const int& pos);
	void setNumOfStations(const int& count);
	
private:
	string name_; // ime linije
	int num_of_stations_; // broj stajalista
	int pos_of_switch_; // pozicija stajalista gde se menja smer
	vector<int> position_stations_; // pozicije stajalista u klasi Simulator gde se nalaze u vektoru stations_
	vector<string> name_stations_; // imena stajalista kroz koje linija prolazi
};

#endif // !ROUTES_H
