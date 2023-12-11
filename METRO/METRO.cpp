#include "METRO.hpp"

using namespace std;

int main()
{






	string file_name="biblio_nom_stat.txt";
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
	cout << "dimensions de la station" << stat_size.x << "," << stat_size.y << endl << endl;


	//System M(file_name,10,texture_ram,texture_stat);
	//M.print();

	int nb_stat = 6;
	Sprite rame(texture_ram);
	Vector2f rame_size = rame.getGlobalBounds().getSize();
	rame.setOrigin(rame_size.x / 2, rame_size.y / 2);
	rame.setScale(0.25, 0.25);


	vector<Sprite> LIGNE_STAT;
	vector<CircleShape> QUAIS_HAUT;
	vector<CircleShape> QUAIS_BAS;
	vector<RectangleShape> LIGNE_TRAJET;
	vector<RectangleShape> LIGNE_TRAJET_ALLER;
	vector<RectangleShape> LIGNE_TRAJET_RETOUR;
	vector<Vector2f> STATION; //position des stations

	float decalage = 25.0;
	float factor_size_stat = 0.1;
	for (int i = 0;i < nb_stat;++i)
	{
		LIGNE_STAT.push_back(Sprite(texture_stat));
		STATION.push_back(Vector2f((float)(300 + 200 * i),(float)( 550 + 50 * (i - 4) * cos(4 * i))));

		// STATIONS

		LIGNE_STAT.back().setOrigin(stat_size.x / 2, stat_size.y / 2); //place l'origine de l'image en son centre
		LIGNE_STAT.back().setPosition(STATION[i].x, STATION[i].y); //position
		LIGNE_STAT.back().setScale(factor_size_stat, factor_size_stat); //taille

		//QUAIS

		QUAIS_HAUT.push_back(sf::CircleShape(10.f));
		QUAIS_HAUT.back().setOrigin(10,10);
		QUAIS_HAUT.back().setPosition(STATION[i] - Vector2f(0.0, decalage));

		QUAIS_BAS.push_back(sf::CircleShape(10.f));
		QUAIS_BAS.back().setOrigin(10, 10);
		QUAIS_BAS.back().setPosition(STATION[i] + Vector2f(0.0, decalage));




		if (i > 0)
		{
			float norm_vect = sqrt((STATION[i].x - STATION[i - 1].x) * (STATION[i].x - STATION[i - 1].x) + (STATION[i].y - STATION[i - 1].y) * (STATION[i].y - STATION[i - 1].y));
			// cout << norm_vect<<endl;
			float angle = acos((STATION[i].x - STATION[i - 1].x) / norm_vect) * ((STATION[i].y - STATION[i - 1].y) > 0 ? 1 : -1);



			//création des deux lignes séparées

			LIGNE_TRAJET_ALLER.push_back(RectangleShape(Vector2f(norm_vect, 2.f)));
			LIGNE_TRAJET_ALLER.back().setPosition(STATION[i - 1] - Vector2f(0.0, 25.0));
			LIGNE_TRAJET_ALLER.back().setRotation((float)(angle * 180 / 3.14));

			LIGNE_TRAJET_RETOUR.push_back(RectangleShape(Vector2f(norm_vect, 2.f)));
			LIGNE_TRAJET_RETOUR.back().setPosition(STATION[i - 1]+Vector2f(0.0,25.0));
			LIGNE_TRAJET_RETOUR.back().setRotation((float)(angle * 180 / 3.14));

			
			// cout << LIGNE_TRAJET[i-1].getSize().x << "," << LIGNE_TRAJET[i-1].getSize().y << endl;
		}
	}

	Rame R1;
	R1.speed = 0;
	R1.acceleration = 50;

	std::thread t1(&Rame::start_move, &R1, std::ref(STATION),decalage);
	t1.detach();


	while (window.isOpen()) //affichage
	{
		//Process events
		sf::Event event;
		//while (window.pollEvent(event))
		//{
		//	// on regarde le type de l'évènement...
		//	switch (event.type)
		//	{
		//		// fenêtre fermée
		//	case sf::Event::Closed:
		//		window.close();
		//		break;
		//		// on ne traite pas les autres types d'évènements
		//	default:
		//		break;
		//	}
		//}

		while (window.pollEvent(event)) // conditions de fermeture de la fenêtre
		{
			if ((event.type == Event::KeyPressed && event.key.code == sf::Keyboard::Escape) || event.type == Event::Closed) //si on appuie sur échap ou qu'on ferme la fenêtre
			{
				window.close();
			}
		}

		sf::sleep(sf::milliseconds(10));
		// Clear screen
		window.clear();

		// Draw the sprite
		for (int i = 0;i < LIGNE_STAT.size();++i)
		{

			if (i != LIGNE_STAT.size() - 1)
			{
				window.draw(LIGNE_TRAJET_ALLER[i]);
				window.draw(LIGNE_TRAJET_RETOUR[i]);
			}
			window.draw(LIGNE_STAT[i]);
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
