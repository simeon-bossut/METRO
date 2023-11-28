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
using namespace sf;

std::mutex myMutex;

// TODO: Référencez ici les en-têtes supplémentaires nécessaires à votre programme.

using namespace std;

sf::Vector2f calcul_position_stat(int i)
{
	return sf::Vector2f((float)(75 + 180 * i), (float)(400 + 50 * (i - 4) * cos(2 * i)));
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



class Station 
{
protected:
	int id;
	 
public:
	string name;

	sf::Sprite stat_sprite;
	const sf::Vector2f position;

	Station(int n_id = -1, string n_name = "Inconnu", sf::Vector2f n_position = sf::Vector2f(0, 0),sf::Sprite sprite=Sprite()) 
	:id(n_id), name(n_name), position(n_position),stat_sprite(sprite) {}

	void set_id(int& n_id) { id = n_id; }
	void set_name(string& n_name) { name=n_name; }
	string get_name() { return string(name);  }
	int get_id() { return id; }
	void print() {
		cout << "La station numero "<< id+1 << "a pour nom"<< name<<"\n\n";
	}
};

class Rame {

private:
	int id;
   
public:

	sf::Sprite ram_sprite;

	vector<int> destination;//Tableau qui contient le nombre de personnes par destination. Exemple: 12 personnes vont descendre à la station 3 destination[3]=12 

	float acceleration;

	sf::Vector2f direction;

	sf::Vector2f position;

	float speed;

	double angle;

	Rame(int n_id = -1,sf::Sprite sprite=Sprite(), float n_speed = 0.0, sf::Vector2f pos = sf::Vector2f(), sf::Vector2f dir = sf::Vector2f(), double st_angle = 0.0, float n_acc = 0.f)
		:position(pos), direction(dir), id(n_id), speed(n_speed), angle(st_angle),acceleration(n_acc),ram_sprite(sprite) {}

	void move();

	void start_move(const std::vector<sf::Vector2f>& Line);

};

void Rame::start_move(const std::vector<sf::Vector2f>& Line)
{
	for (int i = 0;i < Line.size() - 1;++i) //Line.size()-1
	{
		direction = Line[i + 1] - Line[i];
		angle = atan((direction.y / direction.x) * (direction.y * direction.y > 0 ? 1 : -1));
		cout << "From" << Line[i].x << "," << Line[i].y << " moving to" << Line[i + 1].x << "," << Line[i + 1].y << endl << endl;
		while ((abs(position.x - Line[i + 1].x) > 1) || (abs(position.y - Line[i + 1].y) > 1))
		{
			move();
			sf::sleep(sf::milliseconds(10));
		}
		cout << "arrive en " << position.x << "," << position.y << "Station N" << i + 1 << endl << endl;
		sf::sleep(sf::milliseconds(100));
	}
	cout << "Finished" << endl;
}

void Rame::move() {

	std::lock_guard<std::mutex> locked(myMutex);
	if (speed + acceleration >= 0) { speed += acceleration; }


	//cout << "Angle d'orientation: " << angle * 180 / 3.14 << endl ;
	//cout << "Avant a" << position.x << "," << position.y<<"   ";
	position.x += (float)(speed * cos(angle));
	position.y += (float)(speed * sin(angle));
	//cout << "Maintenant à" << position.x << "," << position.y << endl;
	//sf::sleep(sf::milliseconds(10));
   // cin >> angle;

}



class Quai
{
public:
	int ligne;
	Quai(int line) :ligne(line) {}
	int nb_pers = 0;
	void set_nb_pers(int& nb) { nb_pers = nb; }
	int get_nb_pers() { return nb_pers; }
};





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
			LIGNE.push_back(Station(i+1,Name,calcul_position_stat(i),Sprite(text_stat)));
			//METRO.back().stat_sprite.setOrigin(METRO.back().position.x, METRO.back().position.x);
			//METRO[i].print();
		}
	}
	
	void print();
};

void System::print()
{
	cout << "Le metro comprend:\n\n";
	int a = 6;
	for (auto i = LIGNE.begin();i != LIGNE.end();++i)
	{
		cout << "La station numero" << (*i).get_id() << " a pour nom " << (*i).get_name() << "\n\n";
	}
}