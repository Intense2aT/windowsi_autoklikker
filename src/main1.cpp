/*
osad asjad ei pruugi töödata 100% ootustele vastavalt!!!
*/

#include <iostream>
#include <fstream>
#include <windows.h>

using namespace std;

int currentMacroLen;
int ekraan[2];


//laeb failist makro
int loadMacro(char macroName[100], INPUT inputarray[]){
	char tempStr[100];
	bool moveType = false;
	bool reading = false;
	int count = 0;
	int size = 0;
	
	char* sona;
	FILE *readfile;
	readfile = fopen("makroStore.txt", "r");
	while (fgets(tempStr, 100, readfile)) {
		tempStr[strcspn(tempStr, "\n")] = '\0';
    	if (strcmp(macroName, tempStr) == 0) {
        	reading = true;
        	continue;
    	}
		if (strcmp("FIN", tempStr) == 0 && reading == true) {
			reading = false;
			break;
		}
		//makro tõlgendamine failist
		if (reading == true) {
			sona = strtok(tempStr ,"#");
			while (sona != NULL) {
				//dekommenteeri, kui soovid rohkem detaile makro lugemise kohta
				//cout << sona << " " << strcmp(sona, "MOUSEEVENTF_LEFTUP") << " " << count << " " << size << endl;
				if (count == 0) {
					if (strcmp(sona, "INPUT_MOUSE") == 0) {
						inputarray[size].type = INPUT_MOUSE;
					}	
				} else if (count == 1) {
					if (strcmp(sona, "MOUSEEVENTF_LEFTDOWN") == 0) {
						inputarray[size].mi.dwFlags = MOUSEEVENTF_LEFTDOWN;
					}
					if (strcmp(sona, "MOUSEEVENTF_LEFTUP") == 0) {
						inputarray[size].mi.dwFlags = MOUSEEVENTF_LEFTUP;
					} 
					if (strcmp(sona, "MOUSEEVENTF_MOVE") == 0) {
						inputarray[size].mi.dwFlags = MOUSEEVENTF_MOVE | MOUSEEVENTF_ABSOLUTE;
						moveType = true;
					}
				} else if (count == 2) {
					if (moveType == true) {
						int tem = (atoi(sona) * 65535) / ekraan[0];
						inputarray[size].mi.dx = tem;
					}
				} else if (count == 3) {
					if (moveType == true) {
						int temp = (atoi(sona) * 65535) / ekraan[1];
						inputarray[size].mi.dy = temp;
					}
				}
				count++;
				sona = strtok(NULL, "#");
			}
			count = 0;
			size++;
			moveType = false;
		}		
	}
	//dekommenteeri, kui soovid rohkem detaile makro lugemise kohta
	//cout << size << endl;
	//system("pause");
	fclose(readfile);
	return size;
}

//parandab failis ebavajaliku tühiku edge case-i uue makro loomisel
void kiirParandus() {
	char line[100] = {0};
	char failihoidja[20000] = {0};

	FILE *readfile;
	readfile = fopen("makroStore.txt", "r");
	while (fgets(line, 100, readfile)) {
		if (strcmp(line, "\n") == 0) {
			continue;
		} else {
			strncat(failihoidja, line, strlen(line));
		}
	}
	fclose(readfile);
	readfile = fopen("makroStore.txt", "w");
	fprintf(readfile, failihoidja);
	fclose(readfile);
}

//listib makrod ning funktsiooni lõpuks annab parameetrile viimase makro nime väärtuseks,
//mis on vajalik deleteMacro funktsiooni FIN lõpu tühiku kirjutamise kontrolliks
void listMacros(char *valja) {
	char line[100];
	char* sona;
	int x[2];
	FILE *readfile;
	readfile = fopen("makroStore.txt", "r");
	while (fgets(line, 100, readfile)) {
		if (strcmp(line, "FIN\n") == 0 || strcmp(line, "FIN") == 0) {
			x[0] = 1;
		} else {
			x[0] = 0;
		}
		sona = strtok(line ,"#");
		if (strcmp(sona, "INPUT_MOUSE") == 0) {
			x[1] = 1;
		} else {
			x[1] = 0;
		}
		
		if (x[0] == 0 && x[1] == 0) {
			strncpy(valja, line, strlen(line));
			cout << line;
		}
	}
	fclose(readfile);
}

//makro lisamine
void addMacro() {
	system("CLS");
	
	POINT mouse_pos;
	char macroNimi[50] = {0};
	char macro[2000] = {0};
	char hoidla[20] = {0};
	bool creating = true;
	bool kirjutatud = false;
	int calc[2];
	
	cout << "Sisesta macro nimi: ";
	cin >> macroNimi;
	while (strcmp(macroNimi, "FIN") == 0 || strcmp(macroNimi, "INPUT_MOUSE") == 0) {
		system("CLS");
		cout << "Sisesta teistsugune nimi: ";
		cin >> macroNimi;
	}
	strncat(macro, macroNimi, strlen(macroNimi));
	strncat(macro, "\n", 2);
	system("CLS");
	cout << "Vajuta L klahvi, et kirjutada makroklikk praegusele hiire asukohale." << endl;
	//makro kirjutamine esiteks muutujasse
	while (creating == true) {
		if (GetKeyState(76) < 0 && kirjutatud == false) {
			GetCursorPos(&mouse_pos);
			calc[0] = mouse_pos.x;
			calc[1] = mouse_pos.y;
			char move[100] = "INPUT_MOUSE#MOUSEEVENTF_MOVE#";
			itoa(calc[0], hoidla, 10);
			strncat(move, hoidla, strlen(hoidla));
			strncat(move, "#", 2);
			itoa(calc[1], hoidla, 10);
			strncat(move, hoidla, strlen(hoidla));
			strncat(move, "\n", 2);
			strncat(move, "INPUT_MOUSE#MOUSEEVENTF_LEFTDOWN", 33);
			strncat(move, "\n", 2);
			strncat(move, "INPUT_MOUSE#MOUSEEVENTF_LEFTUP", 31);
			strncat(move, "\n", 2);
			
			kirjutatud = true;
			strncat(macro, move, strlen(move));
			system("CLS");
			cout << "Vajuta L klahvi, et kirjutada makroklikk praegusele hiire asukohale." << endl;
			cout << "Vajuta S klahvi, et salvestada makro ja jätkata." << endl;
			cout << "Vajuta Z klahvi, et jätkata makrot salvestamata." << endl;
			//dekommenteeri, kui soovid rohkem detaile makrot lisades
			//cout << macro << endl;
		}
		if (GetKeyState(76) >= 0 && kirjutatud == true) {
			kirjutatud = false;
		}
		if (GetKeyState(83) < 0) {
			//makro kirjutamine faili
			FILE *readfile;
			strncat(macro, "FIN", 4);
			readfile = fopen("makroStore.txt", "a");
			fprintf(readfile, "%s", "\n");
			fprintf(readfile, "%s", macro);
			fclose(readfile);
			break;
		}
		if (GetKeyState(90) < 0) {
			break;
		}
	}
	//tühja faili esimese makro kirjutamise halva formateerimise parandus
	kiirParandus();
}

//kustutab makro
void deleteMacro(char nimi[100]) {
	strncat(nimi, "\n", 2);
	char viimane[100];
	listMacros(viimane);
	
	char line[100];
	char failihoidja[10000];
	char* sona;
	bool skipping = false;
	bool viimaneNahtud = false;
	
	FILE *readfile;
	readfile = fopen("makroStore.txt", "r");
	//loeb faili ja salvestab kõik peale kustutatava makro muutujasse
	while (fgets(line, 100, readfile)) {
		if (strcmp(nimi, line) == 0) {
			skipping = true;
		}
		if (strcmp(viimane, line) == 0) {
			viimaneNahtud = true;
		}
		if (skipping == false) {
			if (strcmp(line, "FIN\n") == 0 || strcmp(line, "FIN") == 0) {
				if (viimaneNahtud == true) {
					strncat(failihoidja, "FIN", 4);
				} else {
					strncat(failihoidja, line, strlen(line));
				}
			} else {
				strncat(failihoidja, line, strlen(line));
			}
		} else {
			if (strcmp(line, "FIN\n") == 0 || strcmp(line, "FIN") == 0) {
				skipping = false;
			}
		}
	}
	fclose(readfile);
	viimaneNahtud = false;
	
	//muutuja kirjutatakse faili
	readfile = fopen("makroStore.txt", "w");
	fprintf(readfile, failihoidja);
	fclose(readfile);
	
	//FIN formateerimise probleemi parandus
	strcpy(failihoidja, "");
	listMacros(viimane);
	readfile = fopen("makroStore.txt", "r");
	while (fgets(line, 100, readfile)) {
		if (strcmp(viimane, line) == 0) {
			viimaneNahtud = true;
		}
		if (strcmp(line, "FIN\n") == 0 || strcmp(line, "FIN") == 0) {
			if (viimaneNahtud == true) {
				strncat(failihoidja, "FIN", 4);
			} else {
				strncat(failihoidja, line, strlen(line));
			}
		} else {
			strncat(failihoidja, line, strlen(line));
		}
	}
	fclose(readfile);
	readfile = fopen("makroStore.txt", "w");
	fprintf(readfile, failihoidja);
	fclose(readfile);
}

int main(){
	SetConsoleOutputCP(65001);
	
	//makrot hoidev
	INPUT inputs[100] = {0};
	//hiirt järgiv
	INPUT hiirtjargiv[2] = {0};
	hiirtjargiv[0].type = INPUT_MOUSE;
	hiirtjargiv[0].mi.dwFlags = MOUSEEVENTF_LEFTDOWN;
	hiirtjargiv[1].type = INPUT_MOUSE;
	hiirtjargiv[1].mi.dwFlags = MOUSEEVENTF_LEFTUP;
	
	char nimi[100] = "";
	char laetudOnNimi[100] = "";
	char z[10]; //tühi muutuja loadMacro funktsiooni implementatsiooni jaoks kasutaja elamuse parandamiseks, ei kasuta
	
	//hoiab ära potensiaalseid vigu mitmekorsest kutsumisest, asjad ei vilgu
	bool laetud = false;
	bool laetud1 = false;
	bool laetud2 = false;
	bool laetud3 = false;
	bool laetud4 = false;
	bool laetud5 = false;
	
	//makro jaoks
	bool isClicking = false;
	//hiirt järgiv klikkimine
	bool isClicking2 = false;
	bool textDisplayed = false;
	
	int changable = 0;
	int V_state;
	int makroDelay = 50;
	
	//hoiatus
	cout << "TEGU ON WINDOWSI OPERATSIOONISÜSTEEMILE MÕELDUD PROGRAMMIGA, SEE EI TÖÖTA LINUX/MAC SÜSTEEMIL!" << endl;
	cout << "PALUN ÄRA MUUDA MAKROFAILI KASUTADES MÕNDA MUUD PROGRAMMI, KUI EI MÕISTA KUIDAS PROGRAMM MAKROFAILI LOEB JA KIRJUTAB." << endl;
	cout << "Tegu on vabavaraga, mis on väljastatud nii nagu on." << endl;
	cout << "Antud programmi arendaja(d) ei vastuta: " << endl;
	cout << "- antud programmi väärkasutusest ja üleüldisest kasutusest võimalike tulenevate probleemide eest." << endl;
	cout << "- antud programmi jätkuva töötamise eest tulevikus." << endl;
	cout << "- antud programmi kasutaja(te)le kasutajatoe pakkumise eest." << endl << endl;
	cout << "Vajuta ükskõik millist klahvi, kui mõistad ning soovid jätkata." << endl;
	system("pause");
	//ekraani suuruse eelnev küsimine, et kliki asukoha arvutus ekraanil korrektselt töötaks
	system("CLS");
	cout << "Palun sisesta oma ekraani dimensioonid (nt. 1920 ja 1080 või 1280 ja 720)" << endl;
	cout << "Sisesta ekraani laius: ";
	cin >> ekraan[0];
	cout << "Sisesta ekraani kõrgus: ";
	cin >> ekraan[1];
	//-1 sest numbrid lähevad 0 kuni (suurus-1)
	ekraan[0]--;
	ekraan[1]--;
	
	//main program loop
	while (true) {
		V_state = GetKeyState(86);
		
		if (textDisplayed == false) {
			system("CLS");
			if (isClicking == false) {
				cout << "Laetud on makro nimega \"" << laetudOnNimi << "\", kui selline makro eksisteerib failis." << endl;
				cout << "Vajuta klahvi H, et käivitada hiirt järgiv autoklikker." << endl;
				cout << "Vajuta klahvi V, et käivitada makro." << endl;
				cout << "Vajuta klahvi G, et laadida failist teine makro." << endl;
				cout << "Vajuta klahvi C, et luua uus makro." << endl;
				cout << "Vajuta klahvi D, et kustutada failist olemasolev makro." << endl;
				cout << "Vajuta klahvi Q, et muuta aega makro loopide vahel." << endl;
				cout << "Vajuta klahvi ESC, et lõpetada programmi töö." << endl << endl;
				cout << "Kui makro mõnes makronimes on vigased sümbolid, ava makroStore.txt fail ning kustuta sisu jättes tühja tekstifaili või" << endl;
				cout << "kustuta nimest vigased sümbolid." << endl;
				cout << "Praegu on failis sellised makrod:" << endl;
				listMacros(z);
				textDisplayed = true;
			}
			if (isClicking == true) {
				cout << "Töötab makro autoklikker." << endl;
				cout << "Vajuta klahvi V, et lõpetada klikkeri töö." << endl;
				textDisplayed = true;
			}
			if (isClicking2 == true) {
				cout << "Töötab hiirt järgiv autoklikker." << endl;
				cout << "Vajuta klahvi H, et lõpetada klikkeri töö." << endl;
				textDisplayed = true;
			}
		}
		
		//makro
		while (isClicking == true) {
			V_state = GetKeyState(86);
			if (V_state < 0 && laetud3 == false) {
				isClicking = false;
				textDisplayed = false;
				laetud3 = true;
			} else if (V_state >= 0 && laetud3 == true) {
				laetud3 = false;
			}
			
			SendInput(currentMacroLen, inputs, sizeof(INPUT));
			Sleep(makroDelay);
		}
		
		//hiirt järgiv
		while (isClicking2 == true) {
			if (GetKeyState(72) < 0 && laetud5 == false) {
				isClicking2 = false;
				textDisplayed = false;
				laetud5 = true;
			} else if (GetKeyState(72) >= 0 && laetud5 == true) {
				laetud5 = false;
			}
			
			SendInput(2, hiirtjargiv, sizeof(INPUT));
			Sleep(makroDelay);
		}
		
		//lae failist teine makro (G)
		if (GetKeyState(71) < 0 && laetud == false) {
			laetud = true;
			system("CLS");
			cout << "Sisesta salvestatud makro nimi, mida soovid kasutada: ";
			cin >> nimi;
			strcpy(laetudOnNimi, nimi);
			currentMacroLen = loadMacro(nimi, inputs);
			if (currentMacroLen == 0) {
				cout << "Antud nimega makrot ei eksisteeri failis või on tegu vigase faili/makroga." << endl;
				Sleep(2000);
			} else {
				cout << "Makro laetud edukalt." << endl;
				Sleep(2000);
			}
			textDisplayed = false;
		} else if (GetKeyState(71) >= 0 && laetud == true) {
			laetud = false;
		}
		
		//loo uus makro (C)
		if (GetKeyState(67) < 0 && laetud1 == false) {
			laetud1 = true;
			addMacro();
			//kiirParandus();
			textDisplayed = false;
		} else if (GetKeyState(67) >= 0 && laetud1 == true) {
			laetud1 = false;
		}
		
		//loo uus makro (D)
		if (GetKeyState(68) < 0 && laetud2 == false) {
			laetud2 = true;
			system("CLS");
			listMacros(z);
			cout << "Sisesta makro nimi mille soovid kustutada: ";
			cin >> nimi;
			deleteMacro(nimi);
			textDisplayed = false;
		} else if (GetKeyState(68) >= 0 && laetud2 == true) {
			laetud2 = false;
		}
		
		//muuda klikiaega (Q)
		if (GetKeyState(81) < 0 && laetud4 == false) {
			laetud4 = true;
			system("CLS");
			cout << "Antud ooteaeg on makro loopide, mitte iga eraldi kliki vahel." << endl;
			cout << "Liiga väike aeg loopide vahel ei pruugi hästi töötada aeglasemates arvutites (aeglasema arvutiga soovituslik >10)." << endl;
			cout << "Sisesta aeg (täisarv, millisekundid): ";
			cin >> makroDelay;
			textDisplayed = false;
		} else if (GetKeyState(81) >= 0 && laetud4 == true) {
			laetud4 = false;
		}
		
		//lülita sisse/välja makro (V) 
		if (V_state < 0 && laetud3 == false) {
			isClicking = true;
			textDisplayed = false;
			laetud3 = true;
		} else if (V_state >= 0 && laetud3 == true) {
			laetud3 = false;
		}
		
		//lülita sisse/välja hiirt järgiv klikkimine (H) 
		if (GetKeyState(72) < 0 && laetud5 == false) {
			isClicking2 = true;
			textDisplayed = false;
			laetud5 = true;
		} else if (GetKeyState(72) >= 0 && laetud5 == true) {
			laetud5 = false;
		}

		//välju (ESC)
		if (GetKeyState(27) < 0) {
			break;
		}
	}
	
	return 0;
}