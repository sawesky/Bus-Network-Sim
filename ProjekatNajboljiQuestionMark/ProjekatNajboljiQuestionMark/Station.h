#ifndef STATION_H
#define STATION_H
 
#include<string>
#include<utility>
#include<vector>

using namespace std;

/*
Klasa Station predstavlja cvor u grafu i odgovara jednom stajalistu
*/

class Station {
public:

	//konstruktori
	Station() : name_(""), key_(0), position_(0), in_({}), out_({}), routes_({}) {};
	Station(const string& name,const string& key_s, const int& key, int position) : name_(name), key_s_(key_s), key_(key), position_(position) {};
	
	//blokirani
	Station(const Station& s) = delete;
	Station(Station&& s) = delete;
	
	//destruktor
	~Station();

	//getteri za dohvatanje nekih informacija o stajalistu
	vector<Station*>& getOuts();
	vector<Station*>& getIns();
	vector<pair<string, int> >& getRoutes();
	vector<pair<string, int> >& getStart();
	string getName() const;
	int getPosition() const;
	int getKey() const;
	string getKeyS() const;


protected:
private:
	string name_; //ime stajalista
	string key_s_; //oznaka stajalista string
	int key_; //oznaka stajalista int
	int position_; //pozicija stajalista u vektoru stations_ klase Simulator
	vector<Station*> in_; //stajalista koja uviru u ovo stajaliste (preko nekih linija) 
	vector<Station*> out_; //stajalista koja izviru iz ovog stajalista
	vector<pair<string, int> > routes_; //linije koje prolaze kroz dato stajaliste (string : ime, int : pozicija stajalista u liniji)
	vector<pair<string, int> > start_route_; //linije koje pocinju u ovom stajalistu ili menjaju smer (string : ime , int : 0 ako menja smer, int : 1 ako pocinje u ovom stajalistu)

};

#endif 
