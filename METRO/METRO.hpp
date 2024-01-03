#pragma once

#include <thread>
#include <fstream>
#include <iostream>
#include <cstdlib>
#include <ctime>
#include <random>
#include <memory>
#include <set>
#include <chrono>
#include <array>
#include <mutex>
#include <string>
#include <vector>
#include <cmath>
#include <functional>

#include <SFML/Audio.hpp>
#include <SFML/Graphics.hpp>
#include <SFML/Main.hpp>
#include <SFML/Window.hpp>
#include <SFML/System/Vector2.hpp>

using namespace std;
using namespace chrono_literals;
using namespace sf;


// TODO: Référencez ici les en-têtes supplémentaires nécessaires à votre programme.

double PI = 3.14159265358979323846;

const Vector2f Position_start(-100, -100);

std::mutex myMutex;

sf::Vector2f calcul_position_stat(int i)
{
	return sf::Vector2f((float)(75 + 180 * i), (float)(400 + 50 * (i - 4) * cos(2 * i))); //c'est quoi ce code de merde poto XD
}

float dist(sf::Vector2f A, sf::Vector2f B)
{
	return sqrt((A.x - B.x) * (A.x - B.x) + (A.y - B.y) * (A.y - B.y));
}



class Ligne
{
public:
	std::vector<sf::Vector2f> ligne;
	Vector2f decalage;
	std::vector<unsigned int> quais;
	const unsigned int max_passager_quai;
	Ligne(int size=0, Vector2f dec=Vector2f(0,0),const int max_pass_q=120) :ligne(std::vector<sf::Vector2f>(size)), decalage(dec) , quais(std::vector<unsigned int>(2*size-2)),max_passager_quai(max_pass_q) {}
	Ligne(std::vector<sf::Vector2f> line, Vector2f dec, const int max_pass_q = 120) :ligne(line), decalage(dec), quais(std::vector<unsigned int>(2*line.size()-2)), max_passager_quai(max_pass_q) {}


	void Gestion_nouv_passager(bool&Run)
	{
		std::mutex my_mutex;
		std::default_random_engine generator;
		std::uniform_int_distribution<unsigned int> distribution(0,2);
		//srand(time(NULL));
		sf::Clock clock;
		while (Run)
		{
			sf::sleep(sf::seconds(1));
			for(int i=1;i<ligne.size()-2;++i){//on ne prend pas le quai 0 et le (fin de ligne1)-1
				my_mutex.lock();
				quais[i] = min(quais[i] + distribution(generator), max_passager_quai);
				my_mutex.unlock();
			}
			for (int i = ligne.size();i < quais.size()-1;++i) {//on ne prend pas le(fin de ligne2)-1
				my_mutex.lock();
				quais[i] = min(quais[i] + distribution(generator), max_passager_quai);
				my_mutex.unlock();
			}
			if (clock.getElapsedTime() >= sf::seconds(5)) {
				//show_quais();
				clock.restart();
			}
		}
	}

	void show_quais()
	{
		std::mutex my_mutex;
		myMutex.lock();
		cout << endl;
		for (int i = 0;i < quais.size();++i)
		{
			cout << quais[i] << " ";
			if (i == ligne.size() - 1) { cout << endl; }
		}
		myMutex.unlock();
	}


	sf::Vector2f operator[](int indice)//idée d'aller de 0 jusqu'à 2*size
	{
		indice %= (2 * ligne.size() - 2);
		float div = (float)indice/(ligne.size()-1);
		if (div == (int)div)//division entière donc à des noeuds de changement de ligne 
		{
			return ligne[indice];
		}
		if (div < 1)//ligne 1/aller
		{
			return ligne[indice] - decalage;
		}
		if (div > 1)
		{
			return ligne[2*(ligne.size() - 1)-indice] + decalage;
		}
	}
};


void modif_string(string&text,char swap1,char swap2)
{
	for (auto i = text.begin();i != text.end();++i)
	{
		if (*i == swap1)
		{
			*i=swap2;
		}
	}
}

																//STATION

class Station 
{
protected:
	int id;
	 
public:
	string name;

	sf::Sprite stat_sprite;
	const sf::Vector2f position;

	Station(int n_id = -1, string n_name = "Inconnu", sf::Vector2f n_position = sf::Vector2f(0, 0),sf::Sprite sprite = sf::Sprite()) 
	:id(n_id), name(n_name), position(n_position),stat_sprite(sprite) {}

	void set_id(int& n_id) { id = n_id; }
	void set_name(string& n_name) { name=n_name; }
	string get_name() { return string(name);  }
	int get_id() { return id; }
	void print() {
		std::cout << "La station numero "<< id+1 << "a pour nom"<< name<<"\n\n";
	}
};



																//RAME


class Rame {

private:
	int id;
   
public:

	//sf::Sprite ram_sprite;

	vector<int> destination;//Tableau qui contient le nombre de personnes par destination. Exemple: 12 personnes vont descendre à la station 3 destination[3]=12 

	float acceleration;

	sf::Vector2f direction;

	sf::Vector2f position;

	double speed;

	const float max_speed;

	double angle;

	int precedente_station;

	const unsigned int max_passagers;

	unsigned int passagers;

	int emergency_stop;//0 si pas d'arrêt, 1 si arrêt provoqué par la rame suivante , 2 si arrêt mannuel d'urgence

	Rame(int n_id = -1, sf::Vector2f pos = Position_start, sf::Vector2f dir = sf::Vector2f(), double st_angle = 0.0, float n_acc = 15.f, float max_vit = 80, int pre_stat = 0,int pass=0)
		:position(pos), direction(dir), id(n_id), speed(0.0), angle(st_angle), acceleration(n_acc), max_speed(max_vit), precedente_station(pre_stat),passagers(pass),max_passagers(80),emergency_stop(0){}

	void move(sf::Time wait_time);

	void start_move(std::vector<Rame>&RAMES,Ligne&LIGNE, bool& Run);

	void set_id(int n_id) {
		id = n_id;
	}

	int get_id()
	{
		return id;
	}

	double dist_to_next(vector<Rame>& RAMES,Ligne& LIGNE)//calcule la distance avec la prochaine Rame
	{
		int true_size= 0;
		while ((true_size<RAMES.size())&&(RAMES[true_size].get_id() != -1)) { ++true_size; }//calcul du nombre réel de rames en circulation
		
		if (true_size == 1) { return -1.0; }
		int id = get_id();
		int ind_next((get_id() > 1) ? get_id() - 2 : true_size - 1);
		Rame next(RAMES[ind_next]);

		if ((precedente_station == next.precedente_station))
		{
			return dist(position, next.position);
		}

		double result = 0.0;
		result += dist(position, LIGNE[precedente_station + 1]);
		result += dist(LIGNE[next.precedente_station], next.position);
		if ((next.precedente_station - precedente_station == 1) || (next.precedente_station - precedente_station == 1 - LIGNE.quais.size()))
		{
			return result;
		}
		int i = precedente_station + 1;
		while (i != next.precedente_station) {
			result += dist(LIGNE[i],LIGNE[i+1]);
			i = (i + 1) % LIGNE.quais.size();
		}
		
		
		return result;
	}
	
};


void Rame::start_move( std::vector<Rame>& RAMES,Ligne & LIGNE, bool& Run)
{
	myMutex.lock();
	position = LIGNE[0];
	myMutex.unlock();
	while (Run)
	{
		if (emergency_stop == 0)
		{
			int i = precedente_station;
			if (position == LIGNE[precedente_station])
			{
				if (i + 1 % (LIGNE.ligne.size() - 1) != 0)
				{
					int descending_passengers = passagers * ((float)1 / (LIGNE.ligne.size() - 1 - i % (LIGNE.ligne.size() - 1)));
					passagers *= (1 - ((float)1 / (LIGNE.ligne.size() - 1 - i % (LIGNE.ligne.size() - 1))));
					sf::sleep(sf::milliseconds(15 * descending_passengers));
				}

				int incoming_passengers = min(max_passagers - passagers, LIGNE.quais[i]);//le min entre la place disponible
				passagers += incoming_passengers;
				LIGNE.quais[i] -= incoming_passengers;
				sf::sleep(sf::milliseconds(15 * incoming_passengers));
			}

			int flag = -1;
			Vector2f PointOrig = position;
			Vector2f PointDest = LIGNE[i + 1];

			direction = (PointDest - PointOrig) / dist(PointOrig, PointDest);

			angle = atan(direction.y / direction.x);

			double dist_freinage = min(abs(max_speed * max_speed / (2 * acceleration)), dist(PointOrig, PointDest) / 2);

			float epsilon = 3;
			//sf::Clock clock;
			sf::Time counter(sf::seconds(0));
				
			while ((dist(position, PointDest) > epsilon)&&((speed!=0.0)||(emergency_stop==0)))//((abs(position.x - PointDest.x) > epsilon) || (abs(position.y - PointDest.y) > epsilon))
			{
				sf::Time wait_time = sf::milliseconds(15);//calcul du temps la période de refresh, maxée à 125ms donc 8px/s 
				double Dist_next;


				Dist_next = dist_to_next(RAMES, LIGNE);
				//if (get_id() == 3) { cout << Dist_next << endl; }

				if ((flag == -1) && (((Dist_next < 300) && (Dist_next >= 0)) || (abs(dist(position, PointDest) - dist_freinage) < 0.5) || (emergency_stop != 0)))
				{
					if ((Dist_next < 300) && (Dist_next >= 0))
					{
						emergency_stop = 1;
					}
					acceleration *= flag;
					flag = 1;
				}
				if (speed + acceleration * wait_time.asSeconds() >= 0) {
					if (speed + acceleration * wait_time.asSeconds() >= max_speed) {
						speed = max_speed;
					}
					else { speed += acceleration * wait_time.asSeconds(); }
				}
				else
				{
					speed = 0.0;
				}
				if (Run == false) { return; }
				move(wait_time);
				counter += wait_time;
				sf::sleep(wait_time);
			}

			//std::cout <<"La rame "<< id << "arrive en " << position.x << "," << position.y << "Station N" << i + 1 << endl;
			acceleration = abs(acceleration);
			speed = 0.0;
			if (emergency_stop == 0)
			{
				precedente_station = (i+1)%LIGNE.quais.size();
				myMutex.lock();
				angle = 0.0;
				position = PointDest;//remet le tram à la position de la station pour éviter des erreurs de calculs futurs
				myMutex.unlock();
			}
			sf::sleep(sf::milliseconds(400));
		}
		else
		{
			
			sf::sleep(sf::seconds(1));
			//cout <<"LE "<<get_id()<<" "<< dist_to_next(RAMES, LIGNE) << endl; 
			if ((emergency_stop == 1) && (dist_to_next(RAMES, LIGNE) > 500))
			{
				emergency_stop = 0;//retour à la normale
			}
			//pour le cas d'arrêt manuel, on attend un redémarrage manuel
			
		}
		
	}
}

void Rame::move(sf::Time wait_time) {

	std::lock_guard<std::mutex> locked(myMutex);

	position.x += speed *direction.x* wait_time.asSeconds();	//speed*(double)(cos(angle))*wait_time.asSeconds();
	position.y += speed * direction.y * wait_time.asSeconds();	//speed*(double)(sin(angle)) * wait_time.asSeconds();
}



																	//QUAI


//classe qui gère les quais et nouveaux passagers
class Quai
{
public:
	int ligne;
	Quai(int line) :ligne(line) {}
	int nb_pers = 0;
	void set_nb_pers(int& nb) { nb_pers = nb; }
	int get_nb_pers() { return nb_pers; }
};


																	//SYSTEM

//ancien code plus utilisé
class System
{

private:
	
public:

	vector<Station> LIGNE;
	vector<Rame> Engins;


	System(string file_name = "bibliotheque_nom_stat.txt", int size=0, sf::Texture text_ram= sf::Texture(), sf::Texture text_stat= sf::Texture()) :LIGNE(size) { //organisé ainsi: Republique/Beaux/arts Lille/Flandres ...
		ifstream Name_Stat(file_name);
		for (int i = 0;i < LIGNE.size();++i)
		{
			string Name;
			if (Name_Stat.peek()!='\n'&&!Name_Stat.eof()) { Name_Stat >> Name; }
			else { Name = "Inconnu";}
			modif_string(Name,'/',' ');
			LIGNE.push_back(Station(i+1,Name,calcul_position_stat(i),sf::Sprite(text_stat)));

		}
	}
	
	//void print();
};

//void System::print()
//{
//	cout << "Le metro comprend:\n\n";
//	int a = 6;
//	for (auto i = LIGNE.begin();i != LIGNE.end();++i)
//	{
//		cout << "La station numero" << (*i).get_id() << " a pour nom " << (*i).get_name() << "\n\n";
//	}
//}  