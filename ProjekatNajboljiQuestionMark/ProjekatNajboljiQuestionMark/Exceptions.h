#ifndef EXCEPTIONS_H
#define EXCEPTIONS_H

#include<exception>
using namespace std;

//u slucaju da ne postoji stajaliste u grafu
class ErrorStation :public exception {
public:
	ErrorStation() : exception("Stajaliste koje ste izabrali je nepostojece.\nPovratak u glavni meni...\n") {};
};

//u slucaju da ne postoji neko od stajalista za koje smo trazili ispis rute 
class ErrorStations : public exception {
public:
	ErrorStations() :exception("Jedno od stajalista koje ste izabrali je nepostojece\nPovratak u glavni meni\n") {};
};

//u slucaju da ne postoji linija u grafu
class ErrorRoute :public exception {
public:
	ErrorRoute() :exception("Linija koju ste izabrali je nepostojeca.\nPovratak u glavni meni...\n") {};
};

//u slucaju da ne postoji put izmedju stajalista koje smo izabrali
class ErrorPath :public exception {
public:
	ErrorPath() :exception("Ne postoji putanja izmedju stajalista koje ste izabrali.\nPovratak u glavni meni...\n") {};
};

#endif // !EXCEPTIONS_H

