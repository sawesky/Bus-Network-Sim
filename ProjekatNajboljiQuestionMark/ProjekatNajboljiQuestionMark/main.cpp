#include "Simulator.h"

#include<fstream>
#include<iostream>

using namespace std;

int main()
{
	cout << "Dobrodosli u simulator mreze gradskog prevoza. Molimo Vas, odaberite opciju:" << endl;
	cout << "1. Ucitavanje podataka o mrezi gradskog prevoza" << endl;
	cout << "0. Kraj rada" << endl;
	int choice;
	while (true) {
		cin >> choice;
		if (choice == 1)
		{
			Simulator* simulator = new Simulator;
			string station_in, route_in;

			cout << "Molimo Vas, unesite putanju do fajla sa stajalistima:" << endl;
			cin >> station_in;
			simulator->setStationsIn(station_in);

			cout << "Molimo Vas, unesite putanju do fajla sa linijama gradskog prevoza:" << endl;
			cin >> route_in;
			simulator->setRoutesIn(route_in);

			simulator->buildStations();
			simulator->makeRoutes();

			cout << "Mreza gradskog prevoza je uspesno ucitana. Molimo Vas, odaberite opciju:" << endl;
			while (true) {
				cout << "1. Prikaz informacija o stajalistu" << endl;
				cout << "2. Prikaz osnovnih informacija o liniji gradskog prevoza" << endl;
				cout << "3. Prikaz statistickih informacija o liniji gradskog prevoza" << endl;
				cout << "4. Pronalazak najkrace putanje (najmanji broj stajalista) izmedju dva stajalista" << endl;
				cout << "5. Pronalazak putanje sa najmanje presedanja izmedju dva stajalista" << endl;
				cout << "0. Kraj rada" << endl;

				int another_choice;
				cin >> another_choice;
				string output = "Generisan je fajl ";
				string route = "";
				pair<string, string> keys;
				int key_start, key_end;
				switch (another_choice)
				{
				case 1:
					cout << "Molimo Vas, unesite oznaku stajalista cije informacije zelite da prikazete" << endl;
					int key;
					cin >> key;
					try{
						route = simulator->stationInformation(key);
						output += "stajaliste_" + route + ".txt sa informacijama o stajalistu " + route;
						cout << output << endl;
					}
					catch (ErrorStation & e) {
						output = e.what();
						cout << output << endl;
					}
					break;
				case 2:
					cout << "Molimo Vas, unesite oznaku linije cije osnovne informacije zelite da prikazete" << endl;
					cin >> route;
					try {
						simulator->routeInformation(route);
						output += "linija_" + route + ".txt sa osnovnim informacijama o liniji " + route;
						cout << output << endl;
					}
					catch (ErrorRoute & e) {
						output = e.what();
						cout << output << endl;
					}
					break;
				case 3:
					cout << "Molimo Vas, unesite oznaku linije ciju statistiku zelite da prikazete" << endl;
					cin >> route;
					try {
						simulator->statisticsInformation(route);
						output += "statistika_" + route + ".txt sa statistikom linije " + route;
						cout << output << endl;
					}
					catch (ErrorRoute & e) {
						output = e.what();
						cout << output << endl;
					}
					break;
				case 4:
					cout << "Molimo Vas, unesite oznake pocetnog i krajnjeg stajalista" << endl;
					cin >> key_start >> key_end;
					try {
						keys = simulator->dijkstra(key_start, key_end);
						output += "putanja_" + keys.first + '_' + keys.second + ".txt sa informacijama o putanji od stajalista " + keys.first + " do stajalista " + keys.second;
						cout << output << endl;
					}
					catch (ErrorStations & e) {
						output = e.what();
						cout << output << endl;
					}
					catch (ErrorPath & e) {
						output = e.what();
						cout << output << endl;
					}
					break;
				case 5:
					cout << "Molimo Vas, unesite oznake pocetnog i krajnjeg stajalista" << endl;
					cin >> key_start >> key_end;
					try {
						keys = simulator->strategy3(key_start, key_end);
						output += "putanja3_" + keys.first + '_' + keys.second + ".txt sa informacijama o putanji od stajalista " + keys.first + " do stajalista " + keys.second;
						cout << output << endl;
					}
					catch (ErrorStations & e) {
						output = e.what();
						cout << output << endl;
					}
					catch (ErrorPath & e) {
						output = e.what();
						cout << output << endl;
					}
					break;
				case 0:
					exit(0);
				default:
					cout << "Nemoguc izbor, unesite broj od 1 do 4 ili 0 za kraj rada" << endl;
					break;
				}

			}
			
		}
		else if (choice == 0)
			break;
		else
			cout << "Nemoguc izbor, izaberite izmedju opcije 0 i 1" << endl;
	}
	return 0;
}