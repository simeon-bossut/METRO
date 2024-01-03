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
	srand(std::time(nullptr));

	std::mutex mymutex;
	//Ancien code obsolète mais potentiellement utile si continuation du  projet

	//string file_name = "biblio_nom_stat.txt";
	//*cout << "Quel est le nom du fichier voulu ? (sans espace svp) \n\n";
	//cin >> file_name; */
	//fstream my_file(file_name,ios::app);
	//string stat_name;
	//if (1)
	//{
	//	do {
	//		sf::sleep(sf::milliseconds(2000));
	//		cout << " entrez le nom de la station à ajouter au fichier si non tapez N \n\n";
	//		cin >> stat_name;
	//		while (cin.peek() != '\n') {
	//			string add;
	//			cin >> add;
	//			stat_name += " ";
	//			stat_name += add;
	//		}

	//		modif_string(stat_name, ' ', '/');
	//		if (stat_name != "N")
	//		{
	//			my_file << stat_name << " ";
	//		}

	//	} while (stat_name != "N");
	//	my_file.close();
	//}

	//INITIALISATION
	sf::VideoMode desktopMode = sf::VideoMode::getDesktopMode();
	sf::RenderWindow window(desktopMode, "METRO LILLE SIMULATION", sf::Style::Fullscreen);//,sf::Style::Fullscreen
	string ASSETS = "../../../../METRO/assets/";

	sf::Font font;
	if (!font.loadFromFile(ASSETS + "arial.ttf"))
	{
		return EXIT_FAILURE;
	}

	sf::Texture texture_ram;
	if (!texture_ram.loadFromFile(ASSETS + "rame2.png"))
		return EXIT_FAILURE;

	sf::Texture texture_stat;
	if (!texture_stat.loadFromFile(ASSETS + "station.png"))
		return EXIT_FAILURE;
	sf::Vector2u stat_size = texture_stat.getSize();

	//AFFICHAGE DE LA RAME

	int nb_stat = 10;
	int nb_rames = 6;
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
		SPRITE_STATIONS.push_back(sf::Sprite(texture_stat)); //= sprite +1
		SPRITE_STATIONS.back().setColor(sf::Color(209, 157, 25, 255));
		//STATION.push_back(sf::Vector2f((float)(300 + 130 * i), 500)); //= station +1
		STATION.push_back(Positions_potentielles[i]); //Formule imaginée pour répartir de manière quelque peu chaotique les stations

		//STATIONS ET QUAIS

		SPRITE_STATIONS.back().setOrigin(stat_size.x / 2, stat_size.y / 2); //place l'origine de l'image en son centre
		SPRITE_STATIONS.back().setPosition(STATION[i].x, STATION[i].y); //position

		QUAIS_HAUT_IMG.emplace_back(15.f); //quai_haut +1
		QUAIS_HAUT_IMG.back().setOrigin(15, 15); 
		//QUAIS_HAUT_IMG.back().setFillColor(sf::Color(33, 91, 217,255)); 

		QUAIS_BAS_IMG.emplace_back(15.f); //quai_bas +1
		QUAIS_BAS_IMG.back().setOrigin(15, 15); 
		//QUAIS_BAS_IMG.back().setFillColor(sf::Color(33, 91, 217, 255));

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

					SPRITE_STATIONS.back().setScale(factor_size_stat, factor_size_stat); //taille

					LIGNE_TRAJET_ALLER.back().setPosition(STATION[i-1]);

					//float angle_debut = tan(abs(QUAIS_HAUT_IMG[i].getPosition().y - QUAIS_HAUT_IMG[i - 1].getPosition().y) / abs(QUAIS_HAUT_IMG[i].getPosition().x - QUAIS_HAUT_IMG[i - 1].getPosition().x)) * ((STATION[i].y - STATION[i - 1].y) > 0 ? -1 : 1);
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

					LIGNE_TRAJET_ALLER.emplace_back(sf::Vector2f(norm_vect_haut, 2.f));//LIGNE_TRAJET_ALLER.push_back(sf::RectangleShape(sf::Vector2f(norm_vect_haut, 2.f)));
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
				SPRITE_STATIONS.back().setScale(factor_size_stat, factor_size_stat); //taille

				QUAIS_HAUT_IMG.back().setPosition(STATION[i] - decalage);
				QUAIS_BAS_IMG.back().setPosition(STATION[i] + decalage);

				// cout << norm_vect << endl;

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
	
	Ligne LIGNE(STATION, decalage);//objet contenant les stations et les quais
	LIGNE.decalage = decalage;
	std::vector<Rame> RAMES(nb_rames);
	std::vector<thread> THREADS;
	bool run = true;//mis à false pour arrêter le code
	/*double dist_tot = 0.0;
	for (int i = 0;i < LIGNE.quais.size();++i)
	{
		dist_tot += dist(LIGNE[i], LIGNE[i + 1]);
	}
	cout << dist_tot << endl;*/
	thread Pass(&Ligne::Gestion_nouv_passager, &LIGNE,ref(run));

	thread T(Gestion_Rames, std::ref(RAMES),nb_rames, std::ref(THREADS), std::ref(LIGNE),ref(run));
	


	while (window.isOpen()) //affichage
	{
		//Process events
		sf::Event event;

		while (window.pollEvent(event)) // conditions de fermeture de la fenêtre
		{
			if ((event.type == sf::Event::KeyPressed)&&(26<event.key.code)&& (35 >=event.key.code))//DE 1 à 9
			{
				int id_stop = event.key.code - 26;
				
				if (RAMES[id_stop - 1].emergency_stop == 2)//SI RAME ARRETEE D'URGENCE, REDEMARRAGE MANUEL
				{
					cout << "redemarrage manel de la rame numero " << id_stop << endl;
					RAMES[id_stop - 1].emergency_stop = 0;//REDEMARRAGE MANUEL
				}
				else if (RAMES[id_stop - 1].get_id() != -1) {//si la rame a été mise en service
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
		// Clear screen
		window.clear();

		// Draw the sprite
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

			/*INFO_RAMES[i].setString("Siuu");
			INFO_RAMES[i].setPosition(200, 300);
			window.draw(INFO_RAMES[i]);*/

			if (RAMES[i].get_id() != -1)
			{
				INFO_RAMES[i].setString("La rame NUMERO "+to_string(RAMES[i].get_id()) + " a pour position :\n" + to_string(RAMES[i].position.x) + "," + to_string((RAMES[i].position.y)) + "\n"
					"Elle se situe entre Station numero " + to_string(RAMES[i].precedente_station) + " et station numéro " + to_string(RAMES[i].precedente_station + 1));//AFFICHAGE PASSAGERS DE CHAQUE RAME
				//std::string AH = INFO_RAMES[i].getString();
				//cout << INFO_RAMES[i].getLocalBounds().height<<","<< INFO_RAMES[i].getLocalBounds().width << endl;
				INFO_RAMES[i].setPosition(window.getSize().x - INFO_RAMES[i].getLocalBounds().width, i * (INFO_RAMES[i].getLocalBounds().height+20));
				window.draw(INFO_RAMES[i]);//window.getSize().x-INFO_RAMES[i].getLocalBounds().width
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