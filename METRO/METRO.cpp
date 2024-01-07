#include "METRO.hpp"

using namespace std;


void Gestion_Rames(vector<Rame>&RAMES,int nb_rames, vector<thread>& THREADS,Ligne & LIGNE,bool&Run)
{

	RAMES[0].set_id(1);
	RAMES[0].position = LIGNE[0];
	THREADS.emplace_back(& Rame::start_move, &RAMES[0],std::ref(RAMES), std::ref(LIGNE), ref(Run));

	for (int i = 1; i < nb_rames;++i)
	{
		while (RAMES[i-1].precedente_station!=2){//quand la rame précédente atteint la 2ème station, on en lance une nouvelle (rame)
			if (Run == false)
			{
				return; 
			}
			sf::sleep(sf::milliseconds(500));
		}
		RAMES[i].set_id(i + 1);
		RAMES[i].position = LIGNE[0];
		THREADS.emplace_back(&Rame::start_move, &RAMES[i], std::ref(RAMES), std::ref(LIGNE), ref(Run));
		
	}
}

int main()
{
	std::mutex mymutex;

	//INITIALISATION DE LA VIDEO ET IMPORTATION DES IMAGES/POLICES 
	sf::VideoMode desktopMode = sf::VideoMode::getDesktopMode();
	sf::RenderWindow window(desktopMode, "METRO LILLE SIMULATION", sf::Style::Fullscreen);//,sf::Style::Fullscreen
	string ASSETS = "../../../../METRO/assets/";

	sf::Font font;
	if (!font.loadFromFile(ASSETS + "arial.ttf")){
		return EXIT_FAILURE;
	}

	sf::Texture texture_ram;
	if (!texture_ram.loadFromFile(ASSETS + "rame2.png"))
		return EXIT_FAILURE;

	sf::Texture texture_stat;
	if (!texture_stat.loadFromFile(ASSETS + "station.png"))
		return EXIT_FAILURE;
	sf::Vector2u stat_size = texture_stat.getSize();


	//NOMBRE DE STATION MAJORE à 10
	int nb_stat = 10; 
	//NOMBRE DE RAMES DANS LE CIRCUIT MAJOREE à 9
	int nb_rames = 6;


	//INTITIALISATION DE TOUS LES SPRITES, DE  LEUR TAILLE,ORIGINE ET NOMBRE (LIGNE 63--> LIGNE 237)

	vector<sf::Sprite> sprite_rames;
	for (int i = 0;i < nb_rames;++i)
	{
		sprite_rames.emplace_back(texture_ram);
		sf::Vector2f rame_size = sprite_rames[i].getGlobalBounds().getSize();
		sprite_rames[i].setOrigin(rame_size.x / 2, rame_size.y / 2);
		sprite_rames[i].setScale(0.4, 0.4);
	}
	//GESTION AFFICHAGE PASSAGERS
	vector<sf::Text> PASSAGERS_QUAI;
	for (int i = 0;i < 2*nb_stat-2;++i) {
		PASSAGERS_QUAI.emplace_back("0", font);
		PASSAGERS_QUAI[i].setFillColor(sf::Color::Red);

		sf::FloatRect textRect = PASSAGERS_QUAI[i].getLocalBounds();
		PASSAGERS_QUAI[i].setOrigin(textRect.left + textRect.width / 2.0f, textRect.top + textRect.height / 2.0f);
	}

	//AFFICHAGE INFORMATION SUR COMMANDES D'ARRET
	sf::Text info_stop("Pour arreter une rame de facon manuelle, par exempe en cas d'urgence, veuillez appuyez sur la touche associée à son ordre d'entrée\n Par exemple, pour arrêter la première rame, appuyez sur 1.\nSi vous voulez relancer la rame, réappuyez sur 1(pour l'exemple) sans quoi elle ne redémarrera jamais", font);
	info_stop.setCharacterSize(20);
	info_stop.setOrigin(info_stop.getGlobalBounds().width/2,0);
	info_stop.setPosition(window.getSize().x / 2-200, 0);

	//AFFICHAGE (TEXTE) POSITION RAMES 
	vector<sf::Text> INFO_RAMES;

	vector<sf::Text> PASSAGERS_RAME;
	for (int i = 0;i < nb_rames;++i) {
		PASSAGERS_RAME.emplace_back("0", font);
		PASSAGERS_RAME[i].setFillColor(sf::Color(120, 17, 99,255));//7, 148, 45

		INFO_RAMES.emplace_back("0",font);
		INFO_RAMES[i].setCharacterSize(12);
		INFO_RAMES[i].setFillColor(sf::Color(209, 157, 25, 255));//209, 157, 25

	}
	

	//AFFICHAGE STATIONS ET LIGNES
	vector<sf::Sprite> SPRITE_STATIONS;
	vector<sf::CircleShape> QUAIS_HAUT_IMG;
	vector<sf::CircleShape> QUAIS_BAS_IMG;

	vector<sf::RectangleShape> LIGNE_TRAJET;
	vector<sf::RectangleShape> LIGNE_TRAJET_ALLER;
	vector<sf::RectangleShape> LIGNE_TRAJET_RETOUR;

	vector<sf::Vector2f> STATION; //position des stations

	Vector2f decalage(0, 35); //espace ligne du haut et ligne du bas
	float factor_size_stat = (float)0.15; //redimension

	//POSITIONS DES STATIONS PAR CHOIX ARBITRAIRE
	vector<Vector2f> Positions_potentielles(10);
	Positions_potentielles[0] = Vector2f(30, 500);
	Positions_potentielles[1] = Vector2f(150, 180);
	Positions_potentielles[2] = Vector2f(300, 300);
	Positions_potentielles[3] = Vector2f(380, 550);
	Positions_potentielles[4] = Vector2f(520, 800);
	Positions_potentielles[5] = Vector2f(700, 600);
	Positions_potentielles[6] = Vector2f(1000, 950);
	Positions_potentielles[7] = Vector2f(1250, 420);
	Positions_potentielles[8] = Vector2f(1320, 200);
	Positions_potentielles[9] = Vector2f(1600, 600);

	for (int i = 0; i < nb_stat; ++i)
	{
		SPRITE_STATIONS.push_back(sf::Sprite(texture_stat)); 
		SPRITE_STATIONS.back().setColor(sf::Color(209, 157, 25, 255));
		STATION.push_back(Positions_potentielles[i]); 

		//STATIONS ET QUAIS

		SPRITE_STATIONS.back().setOrigin(stat_size.x / 2, stat_size.y / 2); //place l'origine de l'image en son centre
		SPRITE_STATIONS.back().setPosition(STATION[i].x, STATION[i].y); //position

		QUAIS_HAUT_IMG.emplace_back(15.f); 
		QUAIS_HAUT_IMG.back().setOrigin(15, 15); 

		QUAIS_BAS_IMG.emplace_back(15.f);
		QUAIS_BAS_IMG.back().setOrigin(15, 15); 
		
		if (i == 0 || i == nb_stat - 1) //si premiere ou derniere station
		{
			SPRITE_STATIONS.back().setScale(0, 0); //on fait disparaître la station

			QUAIS_HAUT_IMG.back().setPosition(STATION[i]); //quai haut et bas confondus
			QUAIS_BAS_IMG.back().setPosition(STATION[i]);
		}

		if (i > 0)
		{
			float norm_vect = sqrt((STATION[i].x - STATION[i - 1].x)*(STATION[i].x - STATION[i - 1].x)  +  (STATION[i].y - STATION[i - 1].y)*(STATION[i].y - STATION[i - 1].y));

			if (i == 1 || i == nb_stat - 1)
			{


				if (i == 1)
				{


					QUAIS_HAUT_IMG.back().setPosition(STATION[i] - decalage);
					QUAIS_BAS_IMG.back().setPosition(STATION[i] + decalage);

					float norm_vect_haut = sqrt((QUAIS_HAUT_IMG[i].getPosition().x - QUAIS_HAUT_IMG[i - 1].getPosition().x) * (QUAIS_HAUT_IMG[i].getPosition().x - QUAIS_HAUT_IMG[i - 1].getPosition().x) + (QUAIS_HAUT_IMG[i].getPosition().y - QUAIS_HAUT_IMG[i - 1].getPosition().y) * (QUAIS_HAUT_IMG[i].getPosition().y - QUAIS_HAUT_IMG[i - 1].getPosition().y));
					float norm_vect_bas = sqrt((QUAIS_BAS_IMG[i].getPosition().x - QUAIS_BAS_IMG[i - 1].getPosition().x) * (QUAIS_BAS_IMG[i].getPosition().x - QUAIS_BAS_IMG[i - 1].getPosition().x) + (QUAIS_BAS_IMG[i].getPosition().y - QUAIS_BAS_IMG[i - 1].getPosition().y) * (QUAIS_BAS_IMG[i].getPosition().y - QUAIS_BAS_IMG[i - 1].getPosition().y));
					
					LIGNE_TRAJET_ALLER.push_back(sf::RectangleShape(sf::Vector2f(norm_vect_haut, 2.f)));
					LIGNE_TRAJET_RETOUR.push_back(sf::RectangleShape(sf::Vector2f(norm_vect_bas, 2.f)));

					SPRITE_STATIONS.back().setScale(factor_size_stat, factor_size_stat);

					LIGNE_TRAJET_ALLER.back().setPosition(STATION[i-1]);

					float angle_haut = atan((QUAIS_HAUT_IMG[i - 1].getPosition().y - QUAIS_HAUT_IMG[i].getPosition().y) / (QUAIS_HAUT_IMG[i - 1].getPosition().x - QUAIS_HAUT_IMG[i].getPosition().x));
					float angle_bas = atan((QUAIS_BAS_IMG[i - 1].getPosition().y - QUAIS_BAS_IMG[i].getPosition().y) / (QUAIS_BAS_IMG[i - 1].getPosition().x - QUAIS_BAS_IMG[i].getPosition().x));


					LIGNE_TRAJET_ALLER.back().setRotation((float)(angle_haut * 180 / PI));

					LIGNE_TRAJET_RETOUR.back().setPosition(STATION[i-1]);
					LIGNE_TRAJET_RETOUR.back().setRotation((float)(angle_bas * 180 / PI));
				}
				else
				{
					float norm_vect_haut = sqrt((QUAIS_HAUT_IMG[i].getPosition().x - QUAIS_HAUT_IMG[i - 1].getPosition().x) * (QUAIS_HAUT_IMG[i].getPosition().x - QUAIS_HAUT_IMG[i - 1].getPosition().x) + (QUAIS_HAUT_IMG[i].getPosition().y - QUAIS_HAUT_IMG[i - 1].getPosition().y) * (QUAIS_HAUT_IMG[i].getPosition().y - QUAIS_HAUT_IMG[i - 1].getPosition().y));
					float norm_vect_bas = sqrt((QUAIS_BAS_IMG[i].getPosition().x - QUAIS_BAS_IMG[i - 1].getPosition().x) * (QUAIS_BAS_IMG[i].getPosition().x - QUAIS_BAS_IMG[i - 1].getPosition().x) + (QUAIS_BAS_IMG[i].getPosition().y - QUAIS_BAS_IMG[i - 1].getPosition().y) * (QUAIS_BAS_IMG[i].getPosition().y - QUAIS_BAS_IMG[i - 1].getPosition().y));

					LIGNE_TRAJET_ALLER.emplace_back(sf::Vector2f(norm_vect_haut, 2.f));
					LIGNE_TRAJET_RETOUR.emplace_back(sf::Vector2f(norm_vect_bas, 2.f));


					float angle_haut = atan((QUAIS_HAUT_IMG[i - 1].getPosition().y - QUAIS_HAUT_IMG[i].getPosition().y) / (QUAIS_HAUT_IMG[i - 1].getPosition().x - QUAIS_HAUT_IMG[i].getPosition().x));
					float angle_bas = atan((QUAIS_BAS_IMG[i - 1].getPosition().y - QUAIS_BAS_IMG[i].getPosition().y) / (QUAIS_BAS_IMG[i - 1].getPosition().x - QUAIS_BAS_IMG[i].getPosition().x));
				

					LIGNE_TRAJET_ALLER.back().setPosition(STATION[i-1]-decalage);
					LIGNE_TRAJET_ALLER.back().setRotation((float)(angle_haut * 180 / PI));

					LIGNE_TRAJET_RETOUR.back().setPosition(STATION[i - 1]+ decalage);
					LIGNE_TRAJET_RETOUR.back().setRotation((float)(angle_bas * 180 / PI));
				}
			}
			else
			{
				SPRITE_STATIONS.back().setScale(factor_size_stat, factor_size_stat); 

				QUAIS_HAUT_IMG.back().setPosition(STATION[i] - decalage);
				QUAIS_BAS_IMG.back().setPosition(STATION[i] + decalage);

				float angle = acos((STATION[i].x - STATION[i - 1].x) / norm_vect) * ((STATION[i].y - STATION[i - 1].y) > 0 ? 1 : -1); // à modif en atan


				//création des deux lignes séparées

				LIGNE_TRAJET_ALLER.emplace_back(sf::Vector2f(norm_vect, 2.f));
				LIGNE_TRAJET_RETOUR.emplace_back(sf::Vector2f(norm_vect, 2.f));

				LIGNE_TRAJET_ALLER.back().setPosition(STATION[i - 1] - decalage);
				LIGNE_TRAJET_ALLER.back().setRotation((float)(angle * 180 / PI));

				LIGNE_TRAJET_RETOUR.back().setPosition(STATION[i - 1] + decalage);
				LIGNE_TRAJET_RETOUR.back().setRotation((float)(angle * 180 / PI));

			}
			LIGNE_TRAJET_ALLER.back().setFillColor(sf::Color(33, 91, 217, 255));
			LIGNE_TRAJET_RETOUR.back().setFillColor(sf::Color(33, 91, 217, 255));
		}
	}

	//FIN INTIALISATION SPRITES 
	






	//CREATION D'UN OBJET CENTRAL DU CODE PERMETTANT LA GESTION, LA MODIFICATION ET L'ECHANGE FACILE DES DONNES DES STATIONS/QUAIS
	Ligne LIGNE(STATION, decalage);

	//PRECISION DU PARAMETRE RESPONSABLE DU DECALAGE DANS LA POSITION DES STATIONS ET DE LEUR QUAIS
	LIGNE.decalage = decalage;

	//VECTEUR CONTENANT TOUTES LES RAMES
	std::vector<Rame> RAMES(nb_rames);

	//VECTEUR DE THREADS, CHACUNE D'ENTRE ELLE EST ASSOCIEE A LA RAME AYANT LE MEME INDICE DANS LE VECTEUR PRECEDENT
	std::vector<thread> THREADS;

	//VARIABLE PERMETTANT L'ARRET TOTAL DU PROGRAMME. CE DERNIER SE FINIRA ALORS
	bool run = true;

	//THREADS GERANT L'ARRIVEEE ALEATOIRE DE NOUVEAU PASSAGERS SUR LES QUAIS
	thread Pass(&Ligne::Gestion_nouv_passager, &LIGNE,ref(run));

	//LE THREAD LE PLUS IMPORTANT QUI VA APPELER TOUS LES THREADS DU "std::vector<Rame> RAMES" QUI VONT EUX-MEMES GERER L'INTEGRALITE DES RAMES
	thread T(Gestion_Rames, std::ref(RAMES),nb_rames, std::ref(THREADS), std::ref(LIGNE),ref(run));
	


	//THREAD DU main() QUI GERE L'AFFICHAGE
	while (window.isOpen()) //affichage
	{
		//Process events
		sf::Event event;

		while (window.pollEvent(event))
		{
			if ((event.type == sf::Event::KeyPressed)&&(26<event.key.code)&& (26+nb_rames >=event.key.code))//SI L'INPUT EST ENTRE 1 ET nb_rames, IL S'AGIT D'UN ARRET D'URGENCE
			{
				int id_stop = event.key.code - 26;//RECUPERATION ET TRADUCTION DE L4INPUT EN INTEGER
				
				if (RAMES[id_stop - 1].emergency_stop == 2)//SI RAME ASSOCIEE DEJA ARRETEE D'URGENCE, ON LA REDEMARRAGE MANUEL
				{
					cout << "redemarrage manel de la rame numero " << id_stop << endl;
					RAMES[id_stop - 1].emergency_stop = 0;//REDEMARRAGE MANUEL
				}
				else if (RAMES[id_stop - 1].get_id() != -1) {//ON VERIFIE QUE LA RAME EST BIEN SUR LE CIRCUIT
					cout << "arret force de la rame numero" << id_stop << endl;
					RAMES[id_stop - 1].emergency_stop = 2;//ARRET D'URGENCE
				}
			}
			if ((event.type == sf::Event::KeyPressed && event.key.code == sf::Keyboard::Escape) || event.type == sf::Event::Closed) //si on appuie sur échap ou qu'on ferme la fenêtre
			{
				mymutex.lock();
				run = false;
				mymutex.unlock();
				window.close(); //fermeture de la fenetre
			}
			
		}

		sf::sleep(sf::milliseconds(10)); //mise à jour de l'écran toutes les 10 millisecondes
		window.clear();

		for (int i = 0; i < SPRITE_STATIONS.size(); ++i)
		{
			
			if (i != SPRITE_STATIONS.size() - 1)
			{
				window.draw(LIGNE_TRAJET_ALLER[i]);
				window.draw(LIGNE_TRAJET_RETOUR[i]);

			}
			window.draw(SPRITE_STATIONS[i]);
			window.draw(QUAIS_HAUT_IMG[i]);
			window.draw(QUAIS_BAS_IMG[i]);
		}
		for (int i = 0;i < LIGNE.quais.size();++i)
		{
			PASSAGERS_QUAI[i].setString(to_string(LIGNE.quais[i]));

			sf::FloatRect textRect = PASSAGERS_QUAI[i].getLocalBounds();
			PASSAGERS_QUAI[i].setOrigin(textRect.left + textRect.width / 2.0f, textRect.top + textRect.height / 2.0f);
			PASSAGERS_QUAI[i].setPosition(LIGNE[i]);

			window.draw(PASSAGERS_QUAI[i]);
		}
		for (int i = 0;i < RAMES.size();++i)
		{
			sprite_rames[i].setPosition(RAMES[i].position);//AFFICHAGE RAME
			sprite_rames[i].setRotation((float)(RAMES[i].angle * 180 / 3.14159));
			window.draw(sprite_rames[i]);

			PASSAGERS_RAME[i].setString(to_string(RAMES[i].passagers));//AFFICHAGE PASSAGERS DE CHAQUE RAME
			
			sf::FloatRect textRect = PASSAGERS_RAME[i].getLocalBounds();
			PASSAGERS_RAME[i].setOrigin(textRect.left + textRect.width / 2.0f, textRect.top + textRect.height / 2.0f);
			PASSAGERS_RAME[i].setPosition(RAMES[i].position);

			window.draw(PASSAGERS_RAME[i]);

			if (RAMES[i].get_id() != -1)
			{
				INFO_RAMES[i].setString("La rame NUMERO "+to_string(RAMES[i].get_id()) + " a pour position :\n" + to_string(RAMES[i].position.x) + "," + to_string((RAMES[i].position.y)) + "\n"
					"Elle se situe entre Station numero " + to_string(RAMES[i].precedente_station) + " et station numéro " + to_string(RAMES[i].precedente_station + 1));
				
				INFO_RAMES[i].setPosition(window.getSize().x - INFO_RAMES[i].getLocalBounds().width, i * (INFO_RAMES[i].getLocalBounds().height+20));
				window.draw(INFO_RAMES[i]);
			}

		}
		window.draw(info_stop);
		// Update the window
		window.display();

	}
	//verifier que toutes les threads se finissent bien
	for (int i = 0;i < THREADS.size();++i)
	{
		THREADS[i].join();
	}
	Pass.join();
	T.join();
return EXIT_SUCCESS;
}