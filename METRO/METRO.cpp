#include "METRO.hpp"

using namespace std;

int main()
{
	srand(std::time(nullptr));
	int delai_1_personne = 0.2; //temps par personne

	string file_name = "biblio_nom_stat.txt";
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

	//Create the main window
	sf::RenderWindow window(sf::VideoMode(2000, 1200), "SFML window");//,sf::Style::Fullscreen

	string ASSETS = "../../../../METRO/assets/";

	//// Load a sprite to display

	sf::Texture texture_ram;
	if (!texture_ram.loadFromFile(ASSETS + "rame2.png"))
		return EXIT_FAILURE;

	sf::Texture texture_stat;
	if (!texture_stat.loadFromFile(ASSETS + "station.png"))
		return EXIT_FAILURE;
	sf::Vector2u stat_size = texture_stat.getSize();

	//cout << "dimensions de la station" << stat_size.x << "," << stat_size.y << endl << endl;


//System M(file_name,10,texture_ram,texture_stat);
//M.print();


	//AFFICHAGE DE LA RAME

	int nb_stat = 5;
	sf::Sprite rame(texture_ram);
	sf::Vector2f rame_size = rame.getGlobalBounds().getSize();
	rame.setOrigin(rame_size.x / 2, rame_size.y / 2);
	rame.setScale(0.25, 0.25);

	//AFFICHAGE STATIONS ET LIGNES
	vector<sf::Sprite> SPRITE_STATIONS;
	vector<sf::CircleShape> QUAIS_HAUT;
	vector<sf::CircleShape> QUAIS_BAS;
	vector<sf::RectangleShape> LIGNE_TRAJET;
	vector<sf::RectangleShape> LIGNE_TRAJET_ALLER;
	vector<sf::RectangleShape> LIGNE_TRAJET_RETOUR;
	vector<sf::Vector2f> STATION; //position des stations

	float decalage = 25.0; //espace ligne du haut et ligne du bas
	float factor_size_stat = 0.1; //redimension

	for (int i = 0; i < nb_stat; ++i)
	{
		SPRITE_STATIONS.push_back(sf::Sprite(texture_stat)); //= sprite +1

		//STATION.push_back(sf::Vector2f((float)(300 + 130 * i), 500)); // = station +1
		STATION.push_back(sf::Vector2f((float)(100 + 400 * i), (float)(400 + 20 * (i - 4) * cos(2 * i)))); // = station +1 (positions modifiables ozef)

		//STATIONS ET QUAIS

		SPRITE_STATIONS.back().setOrigin(stat_size.x / 2, stat_size.y / 2); //place l'origine de l'image en son centre
		SPRITE_STATIONS.back().setPosition(STATION[i].x, STATION[i].y); //position

		QUAIS_HAUT.push_back(sf::CircleShape(10.f)); //quai_haut +1
		QUAIS_HAUT.back().setOrigin(10, 10); //position

		QUAIS_BAS.push_back(sf::CircleShape(10.f)); //quai_bas +1
		QUAIS_BAS.back().setOrigin(10, 10); //position

		if (i == 0 || i == nb_stat - 1) //si premiere ou derniere station
		{
			SPRITE_STATIONS.back().setScale(0, 0); //on fait disparaître la station

			QUAIS_HAUT.back().setPosition(STATION[i]); //quai haut et bas confondus
			QUAIS_BAS.back().setPosition(STATION[i]);
		}

		if (i > 0)
		{
			float norm_vect = sqrt((STATION[i].x - STATION[i - 1].x)*(STATION[i].x - STATION[i - 1].x)  +  (STATION[i].y - STATION[i - 1].y)*(STATION[i].y - STATION[i - 1].y));

			if (i == 1 || i == nb_stat - 1)
			{





				if (i == 1)
				{


					QUAIS_HAUT.back().setPosition(STATION[i] - sf::Vector2f(0.0, decalage));
					QUAIS_BAS.back().setPosition(STATION[i] + sf::Vector2f(0.0, decalage));

					float norm_vect_haut = sqrt((QUAIS_HAUT[i].getPosition().x - QUAIS_HAUT[i - 1].getPosition().x) * (QUAIS_HAUT[i].getPosition().x - QUAIS_HAUT[i - 1].getPosition().x) + (QUAIS_HAUT[i].getPosition().y - QUAIS_HAUT[i - 1].getPosition().y) * (QUAIS_HAUT[i].getPosition().y - QUAIS_HAUT[i - 1].getPosition().y));
					float norm_vect_bas = sqrt((QUAIS_BAS[i].getPosition().x - QUAIS_BAS[i - 1].getPosition().x) * (QUAIS_BAS[i].getPosition().x - QUAIS_BAS[i - 1].getPosition().x) + (QUAIS_BAS[i].getPosition().y - QUAIS_BAS[i - 1].getPosition().y) * (QUAIS_BAS[i].getPosition().y - QUAIS_BAS[i - 1].getPosition().y));
					
					LIGNE_TRAJET_ALLER.push_back(sf::RectangleShape(sf::Vector2f(norm_vect_haut, 2.f)));
					LIGNE_TRAJET_RETOUR.push_back(sf::RectangleShape(sf::Vector2f(norm_vect_bas, 2.f)));

					SPRITE_STATIONS.back().setScale(factor_size_stat, factor_size_stat); //taille

					LIGNE_TRAJET_ALLER.back().setPosition(STATION[i-1]);

					//float angle_debut = tan(abs(QUAIS_HAUT[i].getPosition().y - QUAIS_HAUT[i - 1].getPosition().y) / abs(QUAIS_HAUT[i].getPosition().x - QUAIS_HAUT[i - 1].getPosition().x)) * ((STATION[i].y - STATION[i - 1].y) > 0 ? -1 : 1);
					float angle_haut = atan((QUAIS_HAUT[i - 1].getPosition().y - QUAIS_HAUT[i].getPosition().y) / (QUAIS_HAUT[i - 1].getPosition().x - QUAIS_HAUT[i].getPosition().x));
					float angle_bas = atan((QUAIS_BAS[i - 1].getPosition().y - QUAIS_BAS[i].getPosition().y) / (QUAIS_BAS[i - 1].getPosition().x - QUAIS_BAS[i].getPosition().x));


					LIGNE_TRAJET_ALLER.back().setRotation((float)(angle_haut * 180 / PI));

					LIGNE_TRAJET_RETOUR.back().setPosition(STATION[i-1]);
					LIGNE_TRAJET_RETOUR.back().setRotation((float)(angle_bas * 180 / PI));
				}
				else
				{
					float norm_vect_haut = sqrt((QUAIS_HAUT[i].getPosition().x - QUAIS_HAUT[i - 1].getPosition().x) * (QUAIS_HAUT[i].getPosition().x - QUAIS_HAUT[i - 1].getPosition().x) + (QUAIS_HAUT[i].getPosition().y - QUAIS_HAUT[i - 1].getPosition().y) * (QUAIS_HAUT[i].getPosition().y - QUAIS_HAUT[i - 1].getPosition().y));
					float norm_vect_bas = sqrt((QUAIS_BAS[i].getPosition().x - QUAIS_BAS[i - 1].getPosition().x) * (QUAIS_BAS[i].getPosition().x - QUAIS_BAS[i - 1].getPosition().x) + (QUAIS_BAS[i].getPosition().y - QUAIS_BAS[i - 1].getPosition().y) * (QUAIS_BAS[i].getPosition().y - QUAIS_BAS[i - 1].getPosition().y));

					LIGNE_TRAJET_ALLER.push_back(sf::RectangleShape(sf::Vector2f(norm_vect_haut, 2.f)));
					LIGNE_TRAJET_RETOUR.push_back(sf::RectangleShape(sf::Vector2f(norm_vect_bas, 2.f)));


					float angle_haut = atan((QUAIS_HAUT[i - 1].getPosition().y - QUAIS_HAUT[i].getPosition().y) / (QUAIS_HAUT[i - 1].getPosition().x - QUAIS_HAUT[i].getPosition().x));
					float angle_bas = atan((QUAIS_BAS[i - 1].getPosition().y - QUAIS_BAS[i].getPosition().y) / (QUAIS_BAS[i - 1].getPosition().x - QUAIS_BAS[i].getPosition().x));
				

					LIGNE_TRAJET_ALLER.back().setPosition(STATION[i-1].x,STATION[i-1].y-decalage);
					LIGNE_TRAJET_ALLER.back().setRotation((float)(angle_haut * 180 / PI));

					LIGNE_TRAJET_RETOUR.back().setPosition(STATION[i - 1].x, STATION[i - 1].y + decalage);
					LIGNE_TRAJET_RETOUR.back().setRotation((float)(angle_bas * 180 / PI));
				}
			}
			else
			{
				SPRITE_STATIONS.back().setScale(factor_size_stat, factor_size_stat); //taille

				QUAIS_HAUT.back().setPosition(STATION[i] - sf::Vector2f(0.0, decalage));
				QUAIS_BAS.back().setPosition(STATION[i] + sf::Vector2f(0.0, decalage));

				// cout << norm_vect << endl;

				float angle = acos((STATION[i].x - STATION[i - 1].x) / norm_vect) * ((STATION[i].y - STATION[i - 1].y) > 0 ? 1 : -1); // à modif en atan


				//création des deux lignes séparées


				LIGNE_TRAJET_ALLER.push_back(sf::RectangleShape(sf::Vector2f(norm_vect, 2.f)));
				LIGNE_TRAJET_RETOUR.push_back(sf::RectangleShape(sf::Vector2f(norm_vect, 2.f)));

				LIGNE_TRAJET_ALLER.back().setPosition(STATION[i - 1] - sf::Vector2f(0.0, 25.0));
				LIGNE_TRAJET_ALLER.back().setRotation((float)(angle * 180 / 3.14));

				LIGNE_TRAJET_RETOUR.back().setPosition(STATION[i - 1] + sf::Vector2f(0.0, 25.0));
				LIGNE_TRAJET_RETOUR.back().setRotation((float)(angle * 180 / 3.14));


				// cout << LIGNE_TRAJET[i-1].getSize().x << "," << LIGNE_TRAJET[i-1].getSize().y << endl;
			}
		}
	}

	Rame R1;
	R1.acceleration = 50;

	thread t1(&Rame::start_move, &R1, ref(STATION), decalage);
	t1.detach();


	while (window.isOpen()) //affichage
	{
		//Process events
		sf::Event event;

		while (window.pollEvent(event)) // conditions de fermeture de la fenêtre
		{
			if ((event.type == sf::Event::KeyPressed && event.key.code == sf::Keyboard::Escape) || event.type == sf::Event::Closed) //si on appuie sur échap ou qu'on ferme la fenêtre
			{
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
			window.draw(QUAIS_HAUT[i]);
			window.draw(QUAIS_BAS[i]);
		}
		//rame.setPosition(Vector2f(R1.position.x-rame_size.x/2,R1.position.y - rame_size.y / 2));
		rame.setPosition(R1.position);//+Vector2f(0,decalage)
		rame.setRotation(R1.angle * 180 / 3.14);
		window.draw(rame);
		// Update the window
		window.display();

	}

	return EXIT_SUCCESS;
}