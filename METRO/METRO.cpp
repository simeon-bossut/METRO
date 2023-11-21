#include "METRO.h"

using namespace std;

int main()
{
	string file_name="biblio_nom_stat.txt";
	/*cout << "Quel est le nom d fichier voulu ? (sans espace svp) \n\n";
	cin >> file_name; */
	fstream my_file(file_name,ios::app);
	string stat_name;
	do {
		Sleep(2);
		cout << "Voulez-vous ajouter au fichier un nom de station ? \n si oui, entrez le nom à ajouter au fichier si non tapez N \n\n";
		cin >> stat_name;
		while(cin.peek()!='\n'){
			string add;
			cin >> add;
			stat_name += " ";
			stat_name += add;
		}
		
	    modif_string(stat_name,' ','/');
		if (stat_name != "N")
		{
			my_file << stat_name; //g retiré l'espace en fin de mot
		}

	} while (stat_name != "N");
	my_file.close();

	System M1(file_name,10);
	M1.print();
}
