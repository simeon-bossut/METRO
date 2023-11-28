#include "METRO.hpp"

using namespace std;

int main()
{






	string file_name="biblio_nom_stat.txt";
	///*cout << "Quel est le nom d fichier voulu ? (sans espace svp) \n\n";
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

	int nb_stat = 10;

	//Sprite rame(texture_ram);
	//rame.setScale(0.5, 0.5);
	//Vector2f rame_size = rame.getGlobalBounds().getSize();
	//cout << "dimensions de la rame" << rame_size.x << "," << rame_size.y << endl << endl;
	//rame.setOrigin(rame_size.x / 2, rame_size.y / 2);


	vector<Sprite> LIGNE_STAT;
	vector<RectangleShape> LIGNE_TRAJET;

	vector<Vector2f> Ligne;
	float factor_size_stat = 0.25;
	for (int i = 0;i < nb_stat;++i)
	{
		LIGNE_STAT.push_back(Sprite(texture_stat));
		Ligne.push_back(Vector2f((float)(75 + 180 * i),(float)( 400 + 50 * (i - 4) * cos(2 * i))));
		//cout << 15.0 + 30 * i << " " << 50.0 + 5 * (i - 3) * (i - 3)<<endl;
		LIGNE_STAT.back().setOrigin(stat_size.x * factor_size_stat / 2, stat_size.y * factor_size_stat / 2);
		LIGNE_STAT.back().setPosition(Ligne[i].x, Ligne[i].y);
		LIGNE_STAT.back().setScale(factor_size_stat, factor_size_stat);
		if (i > 0)
		{
			float norm_vect = sqrt((Ligne[i].x - Ligne[i - 1].x) * (Ligne[i].x - Ligne[i - 1].x) + (Ligne[i].y - Ligne[i - 1].y) * (Ligne[i].y - Ligne[i - 1].y));
			// cout << norm_vect<<endl;
			float angle = acos((Ligne[i].x - Ligne[i - 1].x) / norm_vect) * ((Ligne[i].y - Ligne[i - 1].y) > 0 ? 1 : -1);
			LIGNE_TRAJET.push_back(RectangleShape(Vector2f(norm_vect, 2.f)));
			LIGNE_TRAJET.back().setPosition(Ligne[i - 1]);
			LIGNE_TRAJET.back().setRotation((float)(angle * 180 / 3.14));
			// cout << LIGNE_TRAJET[i-1].getSize().x << "," << LIGNE_TRAJET[i-1].getSize().y << endl;
		}
	}




	while (window.isOpen())
	{
		//Process events
		sf::Event event;
		while (window.pollEvent(event))
		{
			// on regarde le type de l'évènement...
			switch (event.type)
			{
				// fenêtre fermée
			case sf::Event::Closed:
				window.close();
				break;
				// on ne traite pas les autres types d'évènements
			default:
				break;
			}
		}

		sf::sleep(sf::milliseconds(10));
		// Clear screen
		window.clear();

		// Draw the sprite
		for (int i = 0;i < LIGNE_STAT.size();++i)
		{

			/*if (i != M.LIGNE.size() - 1)
			{
				window.draw(LIGNE_TRAJET[i]);
			}*/
			window.draw(LIGNE_STAT[i]);
		}
		//rame.setPosition(Vector2f(R1.position.x-rame_size.x/2,R1.position.y - rame_size.y / 2));
		/*rame.setPosition(R1.position);
		rame.setRotation(R1.angle * 180 / 3.14);
		window.draw(rame);*/
		// Update the window
		window.display();

	}

	return EXIT_SUCCESS;
}
