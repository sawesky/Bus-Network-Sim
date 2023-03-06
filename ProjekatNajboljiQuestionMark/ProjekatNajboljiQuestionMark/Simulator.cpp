#include "Simulator.h"


Simulator::~Simulator()
{
	for (auto u : stations_)
		delete u;//fali nullptr? (NE FALI CLEARUJES DOLE VEKTOR)
	for (auto u : full_routes_)
		delete u;
	current_ = nullptr;
	stations_.clear();
	full_routes_.clear();
	routes_.clear();
	processed_.clear();
	parent_.clear();
	distance_.clear();
	name_routes_.clear();
}

void Simulator::buildStations() //pravljenje vektora stations_
{
	fstream input_stations(station_in_, ios::in);
	int position = 0; //dodeljivanje stajalistu neki broj da bismo posle mogli lakse da se krecemo
	while (input_stations.peek() != EOF) { //ucitavanje liniju po liniju iz fajla
		string station;
		getline(input_stations, station);
		createNewStation(station, position);
		position++;
	}
	num_stations_ = position;
	input_stations.close();
}

void Simulator::createNewStation(const string& s, int& position) //kreiranje jednog stajalista
{
	int pos = s.find(' ');
	string key = s.substr(0, pos);
	string name = s.substr(pos + 1);
	int actual_key = createKey(key);
	Station* new_station = new Station(name, key, actual_key, position);
	stations_.push_back(new_station);
}

int Simulator::createKey(const string& s) //pretvaranje oznake stajalista(string) u oznaku(int)
{
	int key = 0;
	int pos = 0;
	while (s[pos] != '\0')
	{
		key = key * 10 + (s[pos] - '0');
		pos++;
	}
	return key;
}

void Simulator::makeRoutes() //povezivanje stajalista medjusobno (ucitavanje linija)
{
	int num = 0;
	fstream input_routes(route_in_, ios::in);
	while (input_routes.peek() != EOF) {
		string route;
		getline(input_routes, route);
		insertRoute(route);
		num++;
	}
	num_routes_ = num;
	input_routes.close();
}

void Simulator::insertRoute(const string& s)
{
	int route_pos = s.find(' '); // pozicija za ime linije
	string route_name = s.substr(0, route_pos); //izdvajanje imena linije
	string route = s.substr(route_pos + 1); // linija
	int pos_in_route = 0; //pozicija trenutnog stajalista u liniji
	routes_.push_back(route_name);

	//pravi novu liniju
	Routes* new_route = new Routes(route_name);

	//prva stanica
	int start_pos = route.find(' ');
	string start_station = route.substr(0, start_pos);
	int start_key = createKey(start_station);
	for (auto u : stations_) { //nadji stajaliste
		if (u->getKey() == start_key) {

			pair<string, int> start = make_pair(route_name, 1); 
			pair<string, int> first_station = make_pair(route_name, pos_in_route); 
			u->getStart().push_back(start); //dodaj u start_routes_ Stationa pocetak
			u->getRoutes().push_back(first_station); //dodaj linije u stations_

			new_route->getStationsNames().push_back(u->getKeyS()); //dodaj u name_stations_ Routesa
			new_route->getStationsPosition().push_back(u->getPosition()); //dodaj u position_stations_ Routesa

			current_ = u;
			pos_in_route++;
			break;
		}
	}

	//ostale stanice
	int last = start_pos + 1; //poslednji razmak + 1 da mozemo da odvojimo sledece stajaliste
	int flag = 0; //flag ako naidjemo na promenu smera

	for (int i = start_pos + 1; i < route.length(); i++) {
		if (route[i] == '#') {
			route[i] = ' ';
			flag = 1;
		}
		if (route[i] == ' ') { //ako naidjes na razmak procesiraj
			string station_key_s = route.substr(last, i - last);
			int station_key = createKey(station_key_s);
			int length_station = station_key_s.length(); //izdvoji oznaku

			if (flag) { //ako si naisao na # preskoci na prvi razmak posle onog stajalista gde si menjao znak
				i += length_station;
			}
			last = i + 1;
			for (auto u : stations_) { //trazenje stanice sa izdvojenom oznakom 
				if (u->getKey() == station_key) {
					string possible = route_name;
					pair<string, int> through = make_pair(route_name, pos_in_route); //dodavanje linija u routes_ Simulatora i povezivanje cvorova medjusobno
					u->getRoutes().push_back(through);
					current_->getOuts().push_back(u);
					u->getIns().push_back(current_);

					//dodavanje u liniji sledece stajaliste
					int possible_position = u->getPosition();
					new_route->getStationsNames().push_back(u->getKeyS());
					new_route->getStationsPosition().push_back(u->getPosition());

					current_ = u; //prebaci se na cvor koji si upravo procesirao
					if (flag) { // u slucaju da si zamenio smer dodaj u start_routes_ Stationa
						int k = 0;
						pair<string, int> switch_direction = make_pair(possible, 0); 
						current_->getStart().push_back(switch_direction);
						flag = 0;

						new_route->setPosOfSwitch(possible_position); //podesi poziciju u Routes na kojoj si zamenio smer
					}
					pos_in_route++;
					break;
				}
			}
		}
	}

	//poslednja stanica ali to je ona koja ne dovodi do pocetnog stajalista nego ona pre nje 
	//jer smo uvek procesirali ono ispred nas sto znaci da je ostalo samo da uvezemo poslednje u smeru B(pocetno uopste) i pretposlednje u smeru B
	string last_station_s = route.substr(last);
	int last_key = createKey(last_station_s);
	for (auto u : stations_) {
		if (u->getKey() == last_key) {
			pair<string, int> last_station = make_pair(route_name, pos_in_route);
			current_->getOuts().push_back(u);
			u->getIns().push_back(current_);
			current_ = u;
		}

	}
	new_route->setNumOfStations(pos_in_route); // podesi broj stajalista neke linije
	full_routes_.push_back(new_route);
}

string Simulator::stationInformation(const int& key)
{
	try {
		int e_flag = 0;
		e_flag = stationException(key);
		if (!e_flag) // ako ne postoji stajaliste sa datom oznakom obavesti korisnika
			throw ErrorStation();
		vector<string> stations;

		//nadji stajaliste po oznaci u intu
		for (auto u : current_->getRoutes()) { 
			stations.push_back(u.first);
		}
		//sortiraj linije koja prolaze kroz dato stajaliste 
		sort(stations.begin(), stations.end());

		//ispis
		string output = "";
		output += current_->getKeyS() + ' ' + current_->getName() + " [";
		for (auto s : stations) {
			output += s;
			output += ' ';
		}
		output = output.substr(0, output.length() - 1);
		output += ']';
		string output_file = "stajaliste_";
		output_file = output_file + current_->getKeyS();
		output_file += ".txt";
		fstream output_stream(output_file, ios::out);
		output_stream << output << endl;
		output_stream.close();
		return current_->getKeyS();
	}
	catch(ErrorStation& e){
		throw;
	}
}

void Simulator::routeInformation(const string& route)
{
	try {
		int e_flag = 0;
		e_flag = routeException(route);
		if (!e_flag) // obavesti korisnika da ne postoji izabrana linija
			throw ErrorRoute();
		Station* starting = nullptr; //pocetno 
		Station* ending = nullptr; //i krajnje stajaliste smer A 
		for (auto u : stations_) { //nadji ih
			for (auto v : u->getStart()) {
				if (v.first == route) {
					if (v.second == 1) {
						current_ = u;
						starting = u;
					}
					else if (v.second == 0) {
						ending = u;
					}
				}
			}
		}

		//spremanje ispisa
		string output_file = "linija_";
		output_file += route;
		output_file += ".txt";
		fstream output_stream(output_file, ios::out);

		//ispisi u prvom redu liniju i imena pocetnog i krajnjeg stajalista u smeru A 
		string first_line = route;
		first_line += ' ';
		first_line += starting->getName();
		first_line += "->";
		first_line += ending->getName();
		output_stream << first_line << endl;
		string start_station = starting->getKeyS() + ' ' + starting->getName();
		output_stream << start_station << endl;

		//ispis cele linije
		int flag = 0;
		string mid_station = "";
		int position = 1;
		int k = position;
		do {
			for (auto u : current_->getOuts()) { //vidi decu trenutnog
				for (auto v : u->getRoutes()) { //vidi linije koje prolaze kroz trenutni
					if (v.first == route && v.second == position) //ako se poklopi ime sa rutom i pozicija na kojoj se mi nalazimo bude jednaka poziciji stajalista u liniji, ispisi
					{
						current_ = u;
						mid_station = current_->getKeyS() + ' ' + current_->getName();
						output_stream << mid_station << endl; // ispisi
						position++;
						if (current_ == ending) // dodatno ako dodjemo do kraja u smeru A ispisi jos jednom
							output_stream << mid_station << endl;
						flag = 1; //iskoci iz petlje ako si nasao stajaliste u liniji
						break;
					}
				}
				if (flag) { //iskoci jos jednom
					flag = 0;
					break;
				}
			}
			k++; 
			if (k > position) // moze se desiti da pocnemo da se vrtimo u krug sto znaci da u jednom trenutku moramo iskociti iz petlje
				break;
		} while (current_ != starting); // radi sve dok pocetni nije jednak krajnjem tj dok ne prodjes sva stajalista jedne linije
		output_stream << start_station << endl;
		output_stream.close();
	}
	catch (ErrorRoute & e) {
		throw;
	}
}

void Simulator::statisticsInformation(const string& route)
{
	try {

		int e_flag = 0;
		e_flag = routeException(route);
		if (!e_flag) //obavesi korisnika da je uneo nepostojecu liniju
			throw ErrorRoute(); 

		Station* starting = nullptr; //pocetno i krajnje stajaliste smer A

		int occur[100] = { 0 }; //broj zajednickih (mozda treba da bude vece)
		string occur_s[100] = { "xxxxxxxx" }; //imena zajednickih
		int max = 1; //broj zajednickih stajalista

		//priprema ispisa
		string output_file = "statistika_";
		output_file += route;
		output_file += ".txt";
		fstream output_stream(output_file, ios::out);
		output_stream << route << endl;

		//procesiraj prvu stanicu
		for (auto u : stations_) {
			for (auto v : u->getStart()) {
				if (v.first == route) { // ako postoji linija
					if (v.second == 1) { // i pocinje u smeru A procesiraj
						current_ = u;
						statisticsHelp(occur_s, occur, max);
						starting = u;
					}
				}
			}
		}

		int position = 1;
		int k = position;

		//procesiraj ostale stanice
		do {
			for (auto u : current_->getOuts()) {
				for (auto v : u->getRoutes()) {
					if (v.first == route && v.second == position) { //ako pogodis isto ime stajalista i poziciju u liniji procesiraj
						current_ = u;
						statisticsHelp(occur_s, occur, max);
						position++;
					}
				}
			}
			k++;
			if (k > position) //isto kao za informacije o liniji izbaci da se ne ide u krug
				break;
		} while (current_ != starting); // radi sve dok nisi na pocetnom stajalistu smer A

		//trazenje linije sa najvecim brojem ponavljanja i ispisvanje svih linija
		int max_occur = 0;
		for (int i = 0; i < max; i++) {
			if (occur_s[i] != route && occur_s[i] != "xxxxxxxx") {
				output_stream << occur_s[i] << " ";
				if (occur[i] > max_occur) {
					max_occur = occur[i];
				}
			}
		}
		output_stream << endl;

		//ispisivanje svih linija sa najvecim brojem ponavljanja
		int flag_comma = 0;
		for (int i = 0; i < max; i++) {
			if (occur[i] == max_occur && occur_s[i] != route)
				if (flag_comma == 0) //ako je prva linija nemoj da stavlja zarez ispred
					output_stream << occur_s[i];//flag_comma = 1;?
				else // u suprotnom stavi zarez ispred
					output_stream << ", " << occur_s[i];
		}
		output_stream.close();
	}
	catch (ErrorRoute & e) {
		throw;
	}


}

void Simulator::statisticsHelp(string* occur_s, int* occur, int& max)
{
	int flag = 0;
	//prodji kroz linije trenutnog vidi da li se poklapa negde
	for (auto u : current_->getRoutes()) {  
		for (int i = 0; i < max; i++) { //prodji kroz dosadasnje linije
			if (u.first == occur_s[i]) { //ako postoji povecaj ponavljanje za 1 
				occur[i]++;
				flag = 1;
			}
				
		}
		if (!flag)// ako se ne pojavljuje dodaj liniju i stavi ponavljanje na 1
		{
			occur_s[max] = u.first;
			occur[max] = 1;
			max++;
		}
		flag = 0;
	}
}

pair<string, string> Simulator::dijkstra(const int& start,const int& end)
{
	try {
		int e_flag_start = 0;
		int e_flag_end = 0;
		e_flag_start = stationException(start);
		e_flag_end = stationException(end);
		if (!e_flag_start || !e_flag_end) // proveri ima li stajalista u grafu i obavesti ako nema
			throw ErrorStations();

		//ocisti vektore od starih stanja i pokretanja i podesi ih da mogu da se procesiraju
		processed_.clear();
		parent_.clear();
		distance_.clear();
		name_routes_.clear();
		for (int i = 0; i < num_stations_; i++) {
			processed_.push_back(false);
			parent_.push_back(-1);
			distance_.push_back(1000000);
		}

		//nadji pocetno stajaliste 
		int source = 0;
		for (auto u : stations_)
		{
			if (u->getKey() == start)
				source = u->getPosition();
		}
		
		//postavi udaljenost pocetnog na 0
		distance_[source] = 0;

		//obican dijkstra sa priority queueom 
		priority_queue<pair<int, int> > q;

		//gurni na pq pocetno stajaliste i njegovu udaljenost
		q.push({ 0, source });

		//radi dok ne prodjes sve cvorove u grafu (ili bar ono sto mozes da dosegnes)
		while (!q.empty()) {
			int curr = q.top().second; //uzmi sa vrha pqa poziciju trenutnog stajalista u stations_ 
			q.pop();
			if (processed_[curr]) // ako je procesirano stajaliste nastavi
				continue;
			processed_[curr] = true; //ako nije postavi da je procesirano
			for (auto u : stations_[curr]->getOuts()) { // prodji kroz decu
				int next = u->getPosition(); //uzmi poziciju deteta u stations_
				if (distance_[curr] + 1 < distance_[next]) { //ako je distanca trenutnog stajalista + 1 manja od distance deteta
					distance_[next] = distance_[curr] + 1; //stavi distancu deteta na distancu trenutnog stajalista + 1
					parent_[next] = curr; // postavi roditelja deteta na poziciju trenutnog stajalista
					q.push({ -distance_[next], next }); // gurni dete na pq (pq stavlja na top uvek najveci element a mi trazimo najmanji tako da moramo da posaljemo negativnu distancu)
				}
			}
		}
		stack<int> route; //stek za pisanje putanje
		int current = 0;
		int sink = 0;
		for (auto u : stations_) { //nadji poziciju poslednjeg stajalista u stations_
			if (u->getKey() == end) {
				current = u->getPosition();
				sink = current;
				break;
			}
		}
		if (distance_[sink] == 1000000) //ako je prevelika distanca obavesti korisnika da ne postoji putanja (mozda moze i !processed_[sink])
			throw ErrorPath();
		while (parent_[current] != -1) { // idi od krajnjeg stajalista i guraj na stek poziciju trenutnog cvora
			route.push(current);
			current = parent_[current]; // prebaci se na roditelja
		}

		//gurni na stek poziciju pocetnog stajalista 
		for (auto u : stations_) {
			if (u->getKey() == start) {
				route.push(source);
				break;
			}

		}

		string start_s = stations_[source]->getKeyS();
		string end_s = stations_[sink]->getKeyS();
		
		//ispisi rutu
		outputStrategy(route, start_s, end_s);
		parent_.clear();
		processed_.clear();
		distance_.clear();
		return { start_s, end_s };
	}
	catch (ErrorStations & e) {
		throw;
	}
	catch (ErrorPath & e) {
		throw;
	}
}

pair<string, string> Simulator::strategy3(const int& start, const int& end)
{
	try {
		int e_flag_start = 0;
		int e_flag_end = 0;
		e_flag_start = stationException(start);
		e_flag_end = stationException(end);
		if (!e_flag_start || !e_flag_end) // obavesti korisnika ako ne postoji neko od zatrazenih stajalista
			throw ErrorStations();
		//ocisti prethodno i podesi da mogu da se procesiraju
		processed_.clear();
		parent_.clear();
		distance_.clear();
		name_routes_.clear();
		for (int i = 0; i < num_stations_; i++) {
			processed_.push_back(false);
			parent_.push_back(-1);
			distance_.push_back(1000000);
		}
		//nadji pocetno stajaliste
		int source = 0;
		for (auto u : stations_)
		{
			if (u->getKey() == start)
				source = u->getPosition();
		}
		int num_of_switches = 0; //broj presedanja
		distance_[source] = num_of_switches; //stavi da ne treba presedanje u pocetnom stajalistu
		int p_routes = 0; // broj linija koje smo obisli

		//queueovi koji nam pokazuju (string: ime linije, int: pozicija cvora u stations_, int: broj presedanja)
		queue<tuple<string, int, int> >q;
		queue<tuple<string, int, int>> reserve;

		//linije sa najmanje presedanja koje prolaze kroz stajalista
		vector<vector<string>> lines;

		//vektor svakog stajalista koji u sebi sadrzi (string: ime linije i int: pozicija cvora roditelja)
		vector<vector<pair<string, int> > > parents;
		for (int i = 0; i < num_stations_; i++) {
			vector<string> v;
			lines.push_back(v);
			vector<pair<string,int> > v_p;
			parents.push_back(v_p);
		}
		//uzmi trenutno stajaliste i ubaci u queue sve linije koje prolaze kroz njega
		for (auto u : stations_[source]->getRoutes()) {
			tuple<string, int, int> t = make_tuple(u.first, source, num_of_switches);
			lines[source].push_back(u.first); //gurni u linije datog stajalista sve te linije
			parents[source].push_back({ u.first, -1 }); // podesi da ako ide preko te linije nema roditelja
			name_routes_.insert(u.first); // ubaci u set linija (nesto kao procesiranje linija bice nam potrebno kasnije da ne prolazimo kroz liniju kroz koju smo vec prosli)
			q.push(t);
			p_routes++;
		}
		int flag_first = 0;
		while (p_routes <= num_routes_) // radi sve dok sve linije u grafu nisu procesirane
		{
			while (!q.empty()) { // dok queue nije prazan
				string thru_route = get<0>(q.front()); //uzmi sa vrha trenutnu liniju
				int curr = get<1>(q.front());
				int switches = get<2>(q.front());
				int k;
				for (auto u : full_routes_) { // nadji trenutnu liniju
					if (thru_route == u->getName()) {
						current_r_ = u;
						break;
					}
				}
				int num_stations = current_r_->getNumOfStations(); // uzmi broj stajalsta kroz koji trenutna linija prolazi
				for (int i = 0; i < num_stations; i++) {
					if (curr == current_r_->getStationsPosition()[i]) // ako pogodis poziciju trenutnog stajalista u liniji sa pozicijom stajalista koje procesiras oznaci ga sa k
					{
						k = i;
						break;
					}
				}
				int pom; //promenljiva koja ce mnogo da pomogne
				vector<int> positions = current_r_->getStationsPosition(); // uzmi pozicije stajalista u liniji 
				for (int u = 0; u < num_stations; u++) { // prodji kroz celu liniju
					pom = (u + k) % num_stations; // ali tako da namestis da ti prvo stajaliste bude ono od kog si ti krenuo a ne od pocetnog stajalista linije
					processed_[positions[pom]] = true; //oznaci stajaliste kao procesirano (mozda fali continue ako je vec procesirano kao gore u dijkstri)
					if (switches <= distance_[positions[pom]]) { // ako je broj presedanja do tog stajalista <= proslom minimalnom broju presedanja na tom stajalistu onda procesiraj
						if(positions[pom] != source)//ako nije pocetno stajaliste dodaj roditelje a nemoj ako jeste(njih smo gore ubacili i stavili -1)
						parents[positions[pom]].push_back({thru_route, curr}); // dodaj u roditelje trenutno stajaliste sa linijom (pazi da ne ostane staro al ne bi trebalo kad menjas)
						lines[positions[pom]].push_back(thru_route); //dodaj liniju 
						distance_[positions[pom]] = switches; // promeni broj presedanja
					}
					curr = positions[pom]; // promeni trenutno stajaliste
				}
				for (int u = 0; u < num_stations; u++) { //prodji kroz stajalista linije
					pom = (u + k) % num_stations;
					for (auto v : stations_[positions[pom]]->getOuts()) { //i pokupi decu stajlista svakog stajalista u liniji
						int a = v->getPosition();
						if (!processed_[a]) { // ako je dete stajaliste procesirano ne radi nista
							for (auto w : v->getRoutes()) { //u suprotnom prodji kroz linije koje prolaze kroz dete
								auto x = name_routes_.find(w.first); // probaj da nadjes da li je trenutna linija procesirana
								if (x == name_routes_.end()) { //ako nisi nasao da je linija procesirana ubaci u rezervni queue

									tuple<string, int, int> t = make_tuple(w.first, positions[pom], num_of_switches + 1);
									reserve.push(t);
									name_routes_.insert(w.first); // ubaci u set trenutnu liniju
									p_routes++; // povecaj broj procesiranih linija
								}
							}
						}
					}
				}
				q.pop();//skini sa queuea liniju
			}
			flag_first = 1;
			while (!reserve.empty()) { //sve dok rezervni queue nije prazan prebacuj sa rezerve na q
				tuple<string, int, int> t = reserve.front();
				reserve.pop();
				q.push(t);
			}
			num_of_switches++; // povecaj broj presedanja
			if (p_routes == num_routes_ && q.empty()) //ako si sve linije zavrsio i ako nemas vise nista u q iskoci
				break;
		}
		stack<pair<string,int>> s;
		int current = 0;
		int sink = 0;
		for (auto u : stations_) { //nadji poziciju krajnjeg stajalista u stations_
			if (u->getKey() == end) {
				current = u->getPosition();
				sink = current;
				break;
			}
		}
		if (distance_[sink] == 1000000) // ako nisi nasao rutu iskoci
			throw ErrorPath();
		string last_route = lines[sink][0]; //namesti da kreces od prve linije krajnjeg stajalista kao prethodnu liniju
		s.push({ last_route,sink }); // gurni liniju i poziciju krajnjeg stajalista na stek
		current = parents[current][0].second; //postavi trenutno stajaliste kao roditelja prve linije krajnjeg stajalista
		int flag_switch = 0;
		int min_switch = distance_[current];//postavi najmanji broj presedanja na broj presedanja trenutnog stajalista
		while (current != source) { //sve dok ne dodjes do pocetnog stajalista
			for (auto u : lines[current]) { //prolazi kroz sve linije sa najmanje presedanja datog stajalista 
				if (u == last_route) // ako je isto ime linije kao prosla linija
				{
					s.push({last_route,current}); // gurni na stek proslu liniju i poziciju trenutnog stajalista 
					for (auto v : parents[current]) { //preskoci na roditelja trenutnog stajalista ali tako da se poklope imena linija 
						if (v.first == last_route)
							current = v.second;
					}
					break;
				}
				else { // ako nisi nasao istu liniju kao sto je prosla
					if (distance_[current] == min_switch - 1) { //ako je broj presedanja za 1 manji od broja trenutnog broja presedanja to znaci da treba da presednes
						s.push({ u,current }); // gurni na stek trenutnu liniju i poziciju trenutnog stajalista
						last_route = u; // promeni ime prosle linije u trenutnu
						min_switch--; //smanji broj trenutnih presedanja za 1
						for (auto v : parents[current]) {// sada prodji kroz roditelje trenutnog stajalista i ako se poklope imena linija skoci na poziciju roditelja preko trenutne linije
							if (v.first == u) {
								current = v.second;
								flag_switch = 1;
								break;
							}
						}
					}
					if (flag_switch)//preskoci ostale linije ako si preseo
						break;
				}
			}
			if (flag_switch)//idi dalje ako si preseo 
			{
				flag_switch = 0;
				continue;
			}
		}
		for (auto u : stations_) {//gurni na stek pocetno stajaliste i njegovu poziciju iz stajalista_
			if (u->getKey() == start) {
				s.push({last_route,source});
				break;
			}
		}
		string start_s = stations_[source]->getKeyS();
		string end_s = stations_[sink]->getKeyS();
		//ispisi strategiju
		outputStrategy3(s, start_s, end_s);

		return{ start_s,end_s };


	}
	catch (ErrorStations & e) {
		throw;
	}
	catch (ErrorPath & e) {
		throw;
	}

}

void Simulator::outputStrategy(stack<int>& s,const string& start,const string& end)
{	
	//pripremi ispis
	string output_file = "putanja_";
	output_file += start + '_' + end + ".txt";
	fstream output_stream(output_file, ios::out);
	vector<string> output_stations; // vektor neispisanih a prodjenih stajalista
	string curr_route_s = ""; // ime trenutne linije
	string next_route_s = ""; //ime sledece linije
	int flag = 0;
	int first_station = 1;
	while (!s.empty()) { // dok ne ispraznis stek uzimaj sa vrha
		int curr = s.top();
		Station* curr_station = stations_[curr]; //trenutno stajaliste
		s.pop();
		if (!s.empty()) { //bitno za poslednje i pretposlednje stajaliste
			int next = s.top(); 
			Station* next_station = stations_[next]; //sledece stajaliste 
			for (auto u : curr_station->getRoutes()) { //probaj da nadjes poklopis imena linija koja prolaze kroz trenutno i sledece stajaliste
				for (auto v : next_station->getRoutes()) {
					if (u.first == v.first) {
						if (curr_route_s == v.first){ // ako postoji takva linija iskoci
							flag = 1;
							break;
						}
						else
							next_route_s = v.first; //ako ne postoji trazi dalje ali podesi da ti sledece bude ta linija
					}
				}
				if (flag)
					break;
			}
			if (flag) { // postoji poklopljeno, onda samo dodajemo
				output_stations.push_back(curr_station->getKeyS());
			}
			else {
				if (!first_station) { // ako nisi na prvom stajalistu gurni u neispisana stajalista trenutno stajaliste
					output_stations.push_back(curr_station->getKeyS());
					for (auto u : output_stations) { //ispisi neispisana stajalista
						output_stream << u << " ";//moz da se popravi
					}
					output_stations.clear();//isprazni neispisana stajalista
					output_stream << endl;
				}
				string switch_route = curr_route_s + "->" + next_route_s; //u svakom slucaju ispisi presedanje
				output_stream << switch_route << endl;
				output_stations.push_back(curr_station->getKeyS());// i gurni na neispisana stajalista trenutno
				curr_route_s = next_route_s; //stavi da je trenutna linija u stvari ona sledeca (u koju smo preseli)
				first_station = 0;

			}
			flag = 0;
		}
		else{
			break;
		}
	}
	for (auto u : output_stations) { //kada se sve zavrsilo ostaje da ispisemo neispisana stajalista
		output_stream << u << " ";
	}
	output_stream << end;
	output_stream.close();
}

void Simulator::outputStrategy3(stack<pair<string,int> >& s, const string& start, const string& end)
{

	//pripremi ispis
	string output_file = "putanja3_";
	output_file += start + '_' + end + ".txt";
	fstream output_stream(output_file, ios::out);
	vector<string> output_stations; // neispisane stanice
	string last_route = s.top().first;//uzmi sa steka
	int name_station = s.top().second;
	output_stream << "->" << last_route << endl; //ispisi prvo presedanje
	output_stations.push_back(stations_[name_station]->getKeyS()); // gurni u neispisane stanice trenutnu
	s.pop();
	while (!s.empty()) {// dok stek nije prazan
		string route = s.top().first;
		int station = s.top().second;
		if (route == last_route) {// ako je sa vrha isto ime linije kao ime prethodne linije
			output_stations.push_back(stations_[station]->getKeyS());//gurni u neispisane trenutnu
		}
		else {
			output_stations.push_back(stations_[station]->getKeyS());//gurni u neispisane trenutnu
			for (auto u : output_stations) {// ispisi neispisane stanice
				output_stream << u << " ";
			}
			output_stream << endl;
			output_stations.clear();
			output_stations.push_back(stations_[station]->getKeyS());//gurni u neispisane trenutnu 
			output_stream << last_route << "->" << route << endl;//ispisi presedanje
			last_route = route;//stavi da je prethodna linija trenutna
		}
		s.pop();
	}
	for (auto u : output_stations) {//ispisi sve neispisane stanice
		output_stream << u << " ";
	}
	output_stations.clear();
	output_stream.close();

}

void Simulator::setRoutesIn(const string& in)
{
	route_in_ = in;
}

void Simulator::setStationsIn(const string& in)
{
	station_in_ = in;
}

int Simulator::stationException(const int& key) // provera da li stajaliste postoji
{
	int e_flag = 0;
	for (auto u : stations_) {
		if (u->getKey() == key) {
			current_ = u;
			e_flag = 1;
			break;
		}
	}
	return e_flag;
}

int Simulator::routeException(const string& route) //provera da li linija postoji
{
	int e_flag = 0;
	for (auto u : routes_) {
		if (u == route)
			e_flag = 1;
	}
	return e_flag;
}

/*void Simulator::dfs(int station)
{
	if (!unvisited[station]) {
		unvisited[station] = true;
		for (auto u : stations_[station]->getOuts()) {
			dfs(u->getPosition());
		}
	}
}*/

/*for (auto u : lines_[current]) {
	if (u == last_route) {
		s.push({ last_route,current });
		break;
	}
	else {
		if (distance_[current] == min_switch - 1) {
			min_switch--;
			s.push({ u,current });
			break;
		}
	}
}
current = parent_[current];*/