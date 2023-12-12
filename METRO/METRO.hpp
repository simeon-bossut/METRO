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

#include <SFML/Audio.hpp>
#include <SFML/Graphics.hpp>
#include <SFML/Main.hpp>
#include <SFML/Window.hpp>
#include <SFML/System/Vector2.hpp>

using namespace std;
using namespace chrono_literals;


// TODO: Référencez ici les en-têtes supplémentaires nécessaires à votre programme.

float PI = 3.14159265358979323846;

std::mutex myMutex;
sf::Vector2f calcul_position_stat(int i)
{
	return sf::Vector2f((float)(75 + 180 * i), (float)(400 + 50 * (i - 4) * cos(2 * i))); //c'est quoi ce code de merde poto XD
}

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

int capa_max_rame = 200; //à modifier si besoin


class Rame {

private:
	int id;
	int nb_personnes_dans_rame = 0;
	int nb_descente = 0;

public:

	sf::Sprite ram_sprite;

	vector<int> destination;//Tableau qui contient le nombre de personnes par destination. Exemple: 12 personnes vont descendre à la station 3 destination[3]=12 

	float acceleration;

	sf::Vector2f direction;

	sf::Vector2f position;

	float speed;

	const float max_speed;

	double angle;

	Rame(int n_id = -1, sf::Sprite sprite = sf::Sprite(), float n_speed = 0.0, sf::Vector2f pos = sf::Vector2f(), sf::Vector2f dir = sf::Vector2f(), double st_angle = 0.0, float n_acc = 0.f, float max_vit = 150.0)
		:position(pos), direction(dir), id(n_id), speed(n_speed), angle(st_angle),acceleration(n_acc),ram_sprite(sprite),max_speed(max_vit) {}

	void move();

	void start_move(const std::vector<sf::Vector2f>& Line,float decalage);

};

void Rame::start_move(const std::vector<sf::Vector2f>& Line,float decalage)
{
	acceleration *= -1;
	speed *= -1;
	while (1)
	{
		myMutex.lock();
		position = Line[0] + sf::Vector2f(0, -decalage);
		myMutex.unlock();
		speed *= -1;
		acceleration *= -1;
		for (int i = 0;i < Line.size() - 1;++i) //Line.size()-1
		{
			direction = Line[i + 1] - Line[i];
			angle = atan((direction.y / direction.x) * (direction.y * direction.y > 0 ? 1 : -1));
			std::cout << "From" << Line[i].x << "," << Line[i].y << " moving to" << Line[i + 1].x << "," << Line[i + 1].y << endl << endl;
			int flag = -1;
			while ((abs(position.x - Line[i + 1].x) > 0.5) || (abs(position.y - Line[i + 1].y + decalage) > 0.5))
			{
				sf::Time wait_time;//calcul du temps la période de refresh, maxée à 125ms  donc 8px/sec
				if (speed != 0) {
					wait_time=sf::milliseconds(min(abs((int)(1000000 / speed) / 1000),125));
				}
				else
				{
					wait_time = sf::milliseconds(20);
				}

				//std::cout << "distance de" << abs(position.x - abs(Line[i + 1].x + Line[i].x) / 2) << "," << abs(position.y - abs(Line[i + 1].y + Line[i].y) / 2 + decalage);
				
				if ((abs(position.x - abs(Line[i + 1].x + Line[i].x) / 2) < 0.5)&&(abs(position.y - abs(Line[i + 1].y + Line[i].y) / 2 + decalage) < 0.5))//arrivé à la moitié,désaccélération
				{
					acceleration *= flag;
					flag = 1;
				}

				if (speed + acceleration*wait_time.asSeconds() >= 0) {
					if (speed + acceleration * wait_time.asSeconds() >= max_speed) { speed = max_speed; }
					else { speed += acceleration * wait_time.asSeconds(); }
				}
				//cout << "tps_refresh" << wait_time.asMilliseconds() << endl;
				std::cout << "speed" << speed << endl << endl;
				move();
				//cout << "Période de " << sf::Int64((float)(1000000 / speed) / 1000) <<endl;
				sf::sleep(wait_time);//hyper IMPORTANT. La norme du vecteur de mouvement est constante, on change uniquement la période de refresh
				//exemple. v=50px/s =50px/1000ms-> T(période)=1000/50=20 millisecondes
			}

			acceleration = abs(acceleration);
			speed = 0.0;
			std::cout << "arrive en " << position.x << "," << position.y << "Station N" << i + 1 << endl << endl;
			myMutex.lock();
			position = Line[i+1]+sf::Vector2f(0,-decalage);//remet le tram à la position de la station pour éviter des erreurs de calculs futurs
			angle = 0.0;
			myMutex.unlock();
			sf::sleep(sf::milliseconds(2000));
		}
		//std::mutex locked;
		sf::sleep(sf::milliseconds(1000));
		myMutex.lock();
		position = Line[Line.size() - 1] + sf::Vector2f(0, decalage);
		myMutex.unlock();
		speed *= -1;
		acceleration *= -1;
		sf::sleep(sf::milliseconds(1000));

		speed = 0;
		for (int i = Line.size()-1;i>0;--i) 
		{
			direction = Line[i  -1] - Line[i];
			angle = atan((direction.y / direction.x) * (direction.y * direction.y > 0 ? 1 : -1));
			cout << "From" << Line[i].x << "," << Line[i].y << " moving to" << Line[i - 1].x << "," << Line[i - 1].y << endl << endl;
			int flag = -1;
			while ((abs(position.x - Line[i  -1].x) > 0.5) || (abs(position.y - Line[i - 1].y - decalage) > 0.5))
			{
				sf::Time wait_time;//calcul du temps la période de refresh, maxée à 125ms donc 8px/s 
				if (speed != 0) {
					wait_time = sf::milliseconds(min(abs((int)(1000000 / speed) / 1000), 125));
				}
				else
				{
					wait_time = sf::milliseconds(20);
				}
				std::cout << "distance de" << abs(position.x - ((Line[i - 1].x + Line[i].x) / 2)) << "," << abs(position.y - ((Line[i - 1].y + Line[i].y) / 2) - decalage);
				if ((abs(position.x - (Line[i - 1].x + Line[i].x) / 2) < 0.5) && (abs(position.y - (Line[i - 1].y + Line[i].y) / 2 - decalage) < 0.5))//arrivé à la moitié,désaccélération
				{
					acceleration *= flag;
					flag = 1;
				}

				if (speed + acceleration * wait_time.asSeconds() <= 0) {
					if (speed + acceleration * wait_time.asSeconds() <= -max_speed) { speed = max_speed; }
					else { speed += acceleration * wait_time.asSeconds(); }
				}
				std::cout << "speed" << speed << endl << endl;
				move();
				//cout << "distance à la station: " << abs((position-Line[i-1]).x* (position - Line[i - 1]).x + (position - Line[i - 1]).y* (position - Line[i - 1]).y)<<endl;
				//cout << "Période de " << sf::Int64((float)(1000000 / speed) / 1000) <<endl;
				sf::sleep(wait_time);//hyper IMPORTANT. La norme du vecteur de mouvement est constante, on change uniquement la période de refresh
				//exemple. v=50px/s =50px/1000ms-> T(période)=1000/50=20 millisecondes
			}
			acceleration = -abs(acceleration);
			speed = 0.0;
			std::cout << "arrive en " << position.x << "," << position.y << "Station N" << i + 1 << endl << endl;
			myMutex.lock();
			angle = 0.0;
			position = Line[i - 1] + sf::Vector2f(0, decalage);//remet le tram à la position de la station pour éviter des erreurs de calculs futurs
			myMutex.unlock();
			sf::sleep(sf::milliseconds(2000));
		}
		std::cout << "Finished" << endl;
	}
}

void Rame::move() {

	std::lock_guard<std::mutex> locked(myMutex);


	//cout << "Angle d'orientation: " << angle * 180 / 3.14 << endl ;
	//cout << "Avant a" << position.x << "," << position.y<<"   ";
	position.x += (speed> 0 ? 1 : -1)*(float)(cos(angle));
	position.y += (speed > 0 ? 1 : -1)*(float)(sin(angle));
	//cout << "Maintenant à" << position.x << "," << position.y << endl;
	//sf::sleep(sf::milliseconds(10));
   // cin >> angle;

}



																	//QUAI

int capa_max_quai = 500; //à modifier si besoin

class Quai
{
public:
	int ligne;
	Quai(int line) :ligne(line) {}

	int nb_personnes_sur_quai = 0;
	void set_nb_pers(int& nb) { nb_personnes_sur_quai = nb; }
	int get_nb_pers() { return nb_personnes_sur_quai; }
};


																	//SYSTEM


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