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


double PI = 3.14159265358979323846;

const Vector2f Position_start(-100, -100);

std::mutex myMutex;

float dist(sf::Vector2f A, sf::Vector2f B)//PERMET DE CALCULER LA DISTANCE 2D ENTRE 2 POINTS DU PLAN
{
	return sqrt((A.x - B.x) * (A.x - B.x) + (A.y - B.y) * (A.y - B.y));
}



class Ligne//CLASSE PERMETTANT LA GESTION FACILE DES POINTS DE DEPARTS ET D'ARRIVEE DES RAMES AINSI QUE LA GESTION DES PASSAGERS SUR LES QUAIS
{
public:
	std::vector<sf::Vector2f> ligne;
	Vector2f decalage;
	std::vector<unsigned int> quais;
	const unsigned int max_passager_quai;
	Ligne(int size=0, Vector2f dec=Vector2f(0,0),const int max_pass_q=120) :ligne(std::vector<sf::Vector2f>(size)), decalage(dec) , quais(std::vector<unsigned int>(2*size-2)),max_passager_quai(max_pass_q) {}
	Ligne(std::vector<sf::Vector2f> line, Vector2f dec, const int max_pass_q = 300) :ligne(line), decalage(dec), quais(std::vector<unsigned int>(2*line.size()-2)), max_passager_quai(max_pass_q) {}


	void Gestion_nouv_passager(bool&Run)//AJOUT ALEATOIRE DE PASSAGERS SUR LES QUAIS
	{
		std::mutex my_mutex;
		std::default_random_engine generator;
		std::uniform_int_distribution<unsigned int> distribution(0,3);
		while (Run)
		{
			sf::sleep(sf::milliseconds(500));
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
		}
	}

	void show_quais()//FONCTION UNTILISEE POUR LE DEBUGAGE
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

	//FONCTION INDISPENSABLE A LA CLASSE PERMETTANT D'ACCEDER A LA POSITION DU IEME POINT D'INTERET P0UR LA RAME,POINT OU ELLE DEVRA S'ARRETER
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



																//RAME


class Rame {

private:
	int id;

public:

	//sf::Sprite ram_sprite;

	vector<int> destination;//Tableau qui contient le nombre de personnes par destination. Exemple: 12 personnes vont descendre à la station 3 destination[3]=12 

	float acceleration;//constante

	sf::Vector2f direction;//Vecteur unité qui porte le mouvement de la rame

	sf::Vector2f position;

	double speed;

	const float max_speed;

	double angle;

	int precedente_station;

	const unsigned int max_passagers;

	unsigned int passagers;

	int emergency_stop;//0 si pas d'arrêt, 1 si arrêt provoqué par la rame suivante(distance trop courte) , 2 si arrêt mannuel d'urgence

	Rame(int n_id = -1, sf::Vector2f pos = Position_start, sf::Vector2f dir = sf::Vector2f(), double st_angle = 0.0, float n_acc = 15.f, float max_vit = 120, int pre_stat = 0,int pass=0)
		:position(pos), direction(dir), id(n_id), speed(0.0), angle(st_angle), acceleration(n_acc), max_speed(max_vit), precedente_station(pre_stat),passagers(pass),max_passagers(200),emergency_stop(0){}

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
			if (position == LIGNE[precedente_station])//SI LA RAME EST BIEN ARRIVEE A DESTINATION
			{
				sf::sleep(sf::milliseconds(200));
				if (i + 1 % (LIGNE.ligne.size() - 1) != 0)// SI LA STATION N'EST PAS AUX EXTREMITES (FAUSSES STATIONS)
				{
					int descending_passengers = passagers * ((float)1 / (LIGNE.ligne.size() - 1 - i % (LIGNE.ligne.size() - 1)));
					passagers *= (1 - ((float)1 / (LIGNE.ligne.size() - 1 - i % (LIGNE.ligne.size() - 1))));
					sf::sleep(sf::milliseconds(15 * descending_passengers));
				}
				sf::sleep(sf::milliseconds(200));
				int incoming_passengers = min(max_passagers - passagers, LIGNE.quais[i]);//le min entre la place disponible
				passagers += incoming_passengers;
				LIGNE.quais[i] -= incoming_passengers;
				sf::sleep(sf::milliseconds(15 * incoming_passengers));
			}

			int dist_lim = 350;//distance minimale à avoir entre 2 rames. Si la distance passse ce seuil, la RAME SUIVANTE ralentit et s'arrête d'URGENCE. ELLE REDEMARERA UNE FOIS UNE CERTAINE DISTANCE AVEC LA PROCHAINE ATTEINTE
			int flag = -1;
			Vector2f PointOrig = position;
			Vector2f PointDest = LIGNE[i + 1];

			direction = (PointDest - PointOrig) / dist(PointOrig, PointDest);

			angle = atan(direction.y / direction.x);

			double dist_freinage = min(abs(max_speed * max_speed / (2 * acceleration)), dist(PointOrig, PointDest) / 2);

			float epsilon = 3;
			sf::Time counter(sf::seconds(0));//Pas utilisz actuellement mais permet de savoir le temps théoriquement passé si les instructions étaient instantanees
			sf::Time wait_time = sf::milliseconds(15);
				
			while ((dist(position, PointDest) > epsilon)&&((speed!=0.0)||(emergency_stop==0)))//SI LA RAME EST TRES PROCHE DE SA DESTINATION OU QUE L'ARRET D'URGENCE(MANUEL OU AUTOMATIQUE) EST ACTIVE, ON QUITTE LA BOUCLE
			{
				double Dist_next = dist_to_next(RAMES, LIGNE);
				//if (get_id() == 3) { cout << Dist_next << endl; }

				if ((flag == -1) && (((Dist_next < dist_lim) && (Dist_next >= 0)) || (abs(dist(position, PointDest) - dist_freinage) < 0.5) || (emergency_stop != 0)))//VERIFICATION DE LA DISTANCE ENTRE LA RAME ET CELLE QUI LA PRECEDE
				{																																				//VERIFICATION DU MOMENT OU COMMENCER A FREINER
					if ((Dist_next < dist_lim) && (Dist_next >= 0))
					{
						emergency_stop = 1;
					}
					acceleration *= flag;//INVERSATION DE L'ACCELERATION(FREINAGE)
					flag = 1;
				}
				if (speed + acceleration * wait_time.asSeconds() >= 0) {//VARIATION DE LA VITESSE
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

			acceleration = abs(acceleration);
			speed = 0.0;
			if (emergency_stop == 0)
			{
				precedente_station = (i+1)%LIGNE.quais.size();
				myMutex.lock();
				angle = 0.0;
				position = PointDest;//remet la rame à la position de la station pour éviter des erreurs de calculs futurs
				myMutex.unlock();
			}
		}
		else//ARRET D'URGENCE ACTIF
		{
			
			sf::sleep(sf::seconds(1));
			if ((emergency_stop == 1) && (dist_to_next(RAMES, LIGNE) > 550))
			{
				emergency_stop = 0;//retour à la normale
			}
			//pour le cas d'arrêt manuel, on attend un redémarrage manuel
			
		}
		
	}
}

void Rame::move(sf::Time wait_time) {

	std::lock_guard<std::mutex> locked(myMutex);

	position.x += speed *direction.x* wait_time.asSeconds();	//deplacement selon le vecteur unitaire direction
	position.y += speed * direction.y * wait_time.asSeconds();	
}
