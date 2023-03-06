#ifndef SIMULATOR_H
#define SIMULATOR_H

#include "Exceptions.h"
#include "Routes.h"
#include "Station.h"

#include<algorithm>
#include<fstream>
#include<iomanip>
#include<queue>
#include<set>
#include<stack>
#include<string>
#include<tuple>
#include<vector>

/*
Klasa Simulator prestavlja bestezinski usmeren graf u kom svaki cvor predstavlja jedno stajaliste(Station)
a cvorovi su povezani granama koje su linije(Routes)
*/

class Simulator {
public:

	//konstruktor i destruktor
	Simulator() :stations_({}), full_routes_({}), station_in_(""), route_in_(""), current_(nullptr), processed_({}), distance_({}), num_routes_(0), num_stations_(0), parent_({}), current_r_(nullptr) {};
	~Simulator();

	//blokirani premestajuci i kopirajuci konstruktor
	Simulator(const Simulator& s) = delete;
	Simulator(Simulator&& s) = delete;

	//za pravljenje grafa
	void buildStations();
	void makeRoutes();

	//za prikaz informacija o mrezi
	string stationInformation(const int& key);
	void routeInformation(const string& route);
	void statisticsInformation(const string& route);

	//implementacija druge strategije(najmanji broj stajalista)
	pair<string, string> dijkstra(const int& start,const int& end);

	//implementacija trece strategije(najmanje presedanja)
	pair<string, string> strategy3(const int& start, const int& end);

	//setteri za nazive fajlova iz kojih citamo nesto
	void setRoutesIn(const string& in);
	void setStationsIn(const string& in);


	//void dfs(int station);

protected:
private:

	vector<Station*> stations_; //vektor svih stajalista u grafu
	vector<Routes*> full_routes_; // vektor svih linija u grafu
	vector<string> routes_; //vektor imena linija
	Routes* current_r_; //trenutna linija po kojoj se krecemo
	Station* current_; //trenutno stajaliste u kom se nalazimo
	string station_in_; //ime fajla sa stajalistima
	string route_in_; //ime fajla sa linijama
	int num_routes_; //broj linija
	int num_stations_; // broj stajalista
	vector<bool> processed_; //procesirana stajalista
	vector<int> distance_; //distanca svakog stajalista od onog od kog smo krenuli
	vector<int> parent_; //oznake stajalista iz kojih smo dosli do odredjenog stajalista
	set<string> name_routes_; //imena linija

	//help funkcija za ispitivanje da li je doslo do greske (da li uopste postoji trazena linija ili stajaliste u grafu)
	int stationException(const int& key);
	int routeException(const string& route);

	//kreiranje jedne stanice i funkcija za pretvaranje stringa u int
	void createNewStation(const string& s, int& position);
	int createKey(const string& s);

	//ubacivanje jedne linije u graf
	void insertRoute(const string& s);

	//help funkcija za statistiku
	void statisticsHelp(string* occur_s, int* occur, int& max);

	//funkcije koje ispisuju neku od dve strategije u txt fajl
	void outputStrategy(stack<int>& s, const string& start, const string& end);
	void outputStrategy3(stack<pair<string,int> >& s, const string& start, const string& end);

};
#endif // !SIMULATOR_H
