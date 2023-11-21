#pragma once

#include <iostream>
#include <string>
#include <iostream>
#include <fstream>
#include <vector>
#include<windows.h>//sous windows
//#include<unistd.h>   // for linux s

const int N_STAT = 12;

// TODO: Référencez ici les en-têtes supplémentaires nécessaires à votre programme.

using namespace std;

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

class Quai
{
public:
	int ligne;
	Quai(int line):ligne(line){}
	int nb_pers = 0;
	void set_nb_pers(int &nb) { nb_pers = nb; }
	int get_nb_pers() { return nb_pers; }
};

class Station 
{
protected:
	int id;
public:
	string name;
	Station(int n_id=-1,string n_name="Inconnu") :id(n_id), name(n_name) {}
	void set_id(int& n_id) { id = n_id; }
	void set_name(string& n_name) { name=n_name; }
	string get_name() { return string(name);  }
	int get_id() { return id; }
	void print() {
		cout << "La station numero "<< id<< "a pour nom"<< name<<"\n\n";
	}
};

class Rame
{
private:
	int id;
	float vitesse;
	float acceleration;
	vector<int> destination;//Tableau qui contient le nombre de personnes par destination. Exemple: 12 personnes vont descendre à la station 3 destination[3]=12 , lors de l'arrivée en gare, on 
	float x_pos;
	float y_pos;
public:
	Rame(int n_id = -1, float n_vit = 0.0, float n_acc = 0.0) :id(n_id), vitesse(n_vit), acceleration(n_acc) {}
	void move(int dx,int dy){}

};
class System
{
public:
	System(string file_name = "bibliotheque_nom_stat.txt", int size=0) :METRO(size) { //organisé ainsi: Republique/Beaux/arts Lille/Flandres ...
		ifstream Name_Stat(file_name);
		for (int i = 0;i < METRO.size();++i)
		{
			string Name;
			if (Name_Stat.peek()!='\n'&&!Name_Stat.eof()) { Name_Stat >> Name; }
			else { Name = "Inconnu";}
			modif_string(Name,'/',' ');
			METRO[i].set_name(Name);
			METRO[i].set_id(i);
			//METRO[i].print();
		}
	}
	void print()
	{
		cout << "Le metro comprend:\n\n";
		int a = 6;
		for (auto i = METRO.begin();i != METRO.end();++i)
			cout << "La station numero"<< (*i).get_id() << " a pour nom"<< (*i).get_name()<<"\n\n";
	}
private:
	vector<Station> METRO;
	vector<Rame> Engins;
};

