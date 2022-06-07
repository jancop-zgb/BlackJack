#include <iostream>
#include <fstream>
#include <vector>
#include <string>
#include <cstdlib>
#include <ctime>
#include <iomanip>
using namespace std;

//enum types for the card suits
enum boja {
	P, H, K, T
};

//Global Variables - yes I know they are bad but rather than pass this everywhere I put it here. Also allows me to change the min bet in one spot rather than everywhere
int MIN_ULOG = 5; //Minimum bet is $5

//Structures
struct racun { //account structure. This will store all the player information
	string ime; //The name on the account
	string email; //the email address registered to this account
	string korisnicko_ime; //the username of this account
	string lozinka; //pasword for the account

	int novac; //in case they decide to quit playing when they come back their money will be there may not use this but we'll see


	int pobjede; //games stats
	int odigrano; //game stats
	double postotak_pobjeda; //game stats
};

struct karta { //how the cards are stored
	int vrijednost; //value of the card
	boja suit; //suit of the card we have. Not really that important in this game but might as well have it
	bool up; //for the dealer since one card is face down
};

struct igrac { //basis of each player in the game including the dealer
	racun info; //player's account info
	vector<karta> hand; //the players hand
	int ulog; //how much the user is betting
	int oklada_na_osiguranje; //if they take the insurance
};

//prototypes
karta dijeli();
karta Jos();
int rezultat(vector<karta>);
bool imaAsa(vector<karta>);
void printajKarte(vector<karta>);
char printajBoju(karta);
void Duplo_il_Nista(igrac, igrac&);
int pobjednik(igrac, igrac&);
void isplata(igrac, igrac&);
void sijeci(igrac&, igrac&);
void ulog(igrac&);
void osiguranje(vector<igrac>&);
racun ucitaj();
void spremi(igrac);
racun stvori();
void igraj(vector<igrac>&);
void ocisti(vector<karta>&);
void diler_igra(igrac&);
void prikaziMeni();
void prikaziPravila();

int main() {
	unsigned int time_seed = time(0); //so we can get a random number
	srand(time_seed); //seeding the random number
	igrac korisnik; //creates a player called user
	igrac diler; //creates the dealer
	vector<igrac> igraci; //creates a vector of players
	char input; //for the Input

	diler.info.korisnicko_ime = "Diler"; //sets up the dealer info
	igraci.push_back(diler); //puts the dealer in the vector

	int meni; // so we can take in input

	do {
		prikaziMeni(); //displays the menu
		cin >> meni; //take in the user input

		if (meni == 1) { //if the user wants to read the rules
			prikaziPravila(); //displays the rules
		}
		else if (meni == 2) { //if the user wants to play
			cout << "Koliko igraca (1 ili 2)" << endl; //asks how many players
			int numPlayers; //so we can take in how many players
			cin >> numPlayers; //takes in the input
			for (int i = 0; i < numPlayers; i++) { //cycles through the players
				cout << "Jeste li novi igrac(N) or postojeci igrac(P)?" << endl; //asks if they are a new or existig player
				cin >> input; //takes in their input

				if (input == 'N' || input == 'n') { //if they are new
					korisnik.info = stvori(); //creates new account info
					spremi(korisnik); //saves the player
					igraci.push_back(korisnik); //pushes the player on the vector
				}
				else if (input == 'P' || input == 'p') { //if they are a existing player
					korisnik.info = ucitaj(); //loads their account info
					igraci.push_back(korisnik); //pushes them on the stack
				}

			}
		}
		else { //if the user can't follow directions
			cout << "Molim Vas unesite 1 ili 2" << endl; //reiterates their only options
			meni = 0; //resets menu
			cin.ignore(); //ignores what's in the buffer
		}
	} while (meni != 2); //while they don't choose play we will repeat this until we finish the never ending story

	bool cond = false; //so we can ask if they want to continue
	int size = igraci.size();
	do {
		for (int i = 1; i < igraci.size(); i++) {
			if (igraci[i].info.novac < 5) { //checks to see if they player has any money if not they can add more
				cout << igraci[i].info.korisnicko_ime << " je trenutno bez novaca. Zelite li uplatiti jos, da(D) ili ne(N)?" << endl; //Lets them know they are out of funds and if they want to add some
				cin >> input; //takes in input
				if (input == 'D' || input == 'd') { //if they say yes
					cout << "Koliko zelite dodati?" << endl; //how much
					cin >> igraci[i].info.novac; //takes it in
				}
				else { //if not we save their info and leave
					spremi(igraci[i]); //saves info
					igraci.erase(igraci.begin() + (i)); //erases player from vector
					i--;

				}
			}

			if (cond && igraci.size() > 1) {//if they have played at least once
				cout << igraci[i].info.korisnicko_ime << " Zelite li nastaviti(N) ili odustati(O)" << endl;
				cin >> input; //takes in input
				if (input == 'o' || input == 'O') { //player want to quit
					spremi(igraci[i]); //saves info
					igraci.erase(igraci.begin() + (i)); //erases player from vector
					i--;
				}
			}
		}
		igraj(igraci); //and we play
		cond = true; //they finished one hand
	} while (igraci.size() > 1);
	//!(input == 'q' || input == 'Q')

	return 0;
}
/**
	Simple method that displays the rules of the game only
*/
void prikaziPravila() {
	cout << "Blackjack se igra kompletom karata, preciznije kompletom francuskih karata. Igrate protiv krupjea, a vaš je cilj skupiti zbroj (vrijednost) karata koji je veæi od vrijednosti karata krupjea, ali tako da on ne iznosi više od 21. Zašto baš 21? Zato što je najveæi zbroj dvije karte 21, tj. to je vrijednost asa i karte koja vrijedi 10. Ta se pobjednièka kombinacija naziva blackjack. Ako vrijednost karata premašuje 21, gubite.Uvijek imajte na umu sljedeæe : Cilj je pobijediti krupjea, a ne dobiti 21. Navest æemo nekoliko naèina za postizanje tog cilja.Jedan je da završite rundu s veæom vrijednosti karata od one krupjea ili tako da na poèetku dobijete as i kartu èija je vrijednost 10. Krupje takoðer može izgubiti ako vrijednost njegovih karata premašuje 21. Karte od 2 do 10 imaju svoju nazivnu vrijednost, što znaèi da imaju istu vrijednost koja je na njima oznaèena. Karte J (deèko), Q (dama) i K (kralj) imaju vrijednost 10.As (A) može vrijediti 1 ili 11, ovisno o tome što vam odgovara. Uzimanje karata (Hit) – Igraè dobiva još jedu kartu od krupjea. Zaustavljanje (Stand) – Igraè preskaèe svoj red, ako ste zadovoljni kartama možete završiti svoju igru te u tom sluèaju neæete više dobiti nijednu kartu. Podvostruèenje (Double) – Možete udvostruèiti svoj ulog. Tada vam krupje daje još jednu kartu, a igra završava. Razdvajanje karata (Split) – Igraèi razdvajaju igru. Igru možete razdvojiti samo ako imate karte s istom vrijednošæu. U tom sluèaju, igrat æete zasebnu igru sa svakom od vaših karata. Meðutim, trebate udvostruèiti svoj ulog. Predaja (Surrender) – Igraè odustaje od igre. Možete odustati ako vam se ne sviðaju prve dvije karte koje ste dobili. Zadržavate jednu polovinu uloga, a drugu zadržava kasino. Osiguranje – Igraè stvara osiguranje za svoj ulog. Ako je prva karta krupjea as, možete se „osigurati” u sluèaju da krupje dobije blackjack, no to morate uèiniti prije nego što krupje uzme drugu kartu. Za osiguranje možete iskoristiti najviše polovinu svog uloga, a isplata se dobiva u omjeru 2 : 1." << endl; //displays the rules
}
/**
	Simple method that displays the menu for the game only
*/
void prikaziMeni() {
	cout << "Dobrodosli u igru Blackjack" << endl; //output
	cout << "Molim vas odaberite opciju." << endl; //output
	cout << "1) Pravila" << endl; //output
	cout << "2) Igraj" << endl; //output
}
/**
	Rules for the dealer. They hit on everything up to 17 including a soft 17
*/
void diler_igra(igrac& dealer) {
	if ((rezultat(dealer.hand) < 17) || (rezultat(dealer.hand) == 17 && imaAsa(dealer.hand))) { //dealer hits at less than 17 and on soft 17
		dealer.hand.push_back(Jos()); //dealer gets a card
	}
}
/**
 how we will play
 This method takes care of all the play portions of the game. Taking hits, splitting, double down, etc
 Takes in the vector of players by reference so we can make changes directly to the players
*/
void igraj(vector<igrac>& players) {
	char input; //for our input

	for (int i = 1; i < players.size(); i++) { //cycles through the players
		ulog(players[i]); //players bet here
		cout << "Novac: " << players[i].info.novac << setw(10) << "Uplata: " << players[1].ulog << endl; //display their money and bet amount
	}

	/**the below function goes through each player and deals them two cards. To make it more realistic
		I actually went through every player and gave them one card a time. So it starts with the dealer
	*/
	for (int i = 0; i < (players.size() * 2); i++) {
		players[(i % players.size())].hand.push_back(dijeli());
		if ((i % players.size()) == 0 && (i % 2) == 0) { //the dealers first card
			players[(i % players.size())].hand[(i % 2)].up = false; //is set to false since it's face down
		}
	}
	/**
		The below function shows each players score but the dealers
	*/
	for (int i = 1; i < players.size(); i++) {
		cout << players[i].info.korisnicko_ime << " ima: " << rezultat(players[i].hand) << setw(10) << endl;
	}

	/**
		The below function displays each persons cards
	*/
	for (int i = 0; i < players.size(); i++) {
		cout << players[i].info.korisnicko_ime << " Karte:" << endl;
		printajKarte(players[i].hand);
	}

	/**
		The below function  actually consists of the playing
	*/
	bool cont = true; //we will use this
	for (int i = 1; i < players.size(); i++) { //we cycle through all the players since each player plays on their own
		do {
			if (players[0].hand[1].vrijednost == 1 && cont) { //if the dealer has an ace and cont is true - basically this only happens the first time if the dealer doesn't have blackjack
				osiguranje(players); //asks player if they want insurance
				if (rezultat(players[0].hand) == 21) { //checks to see if the dealer has blackjack - we know the first card is an A
					players[0].hand[0].up = true; //if they do we set the first card to face up

					printajKarte(players[0].hand); //prints the dealers cards

					/**
						The below function cycles through the players and pays them out since they lost
					*/
					for (int i = 1; i < players.size(); i++) {
						isplata(players[0], players[i]);
					}
					input = 'O'; //sets input to stay since they just lost
				}
				cont = false; //if the dealer didn't have blackjack this is now false
			}
			if (players[0].hand[1].vrijednost >= 10 && cont) { //if the dealer has a 10 or face card showing we don't check for insurance but if they have blackjack that's game
				if (rezultat(players[0].hand) == 21) { //if they have blackjack
					players[0].hand[0].up = true; //puts the dealers first card face up

					printajKarte(players[0].hand); //prints the dealers card
					/**
						The below function pays out the players since they just lost
					*/
					for (int i = 1; i < players.size(); i++) {
						isplata(players[0], players[i]);
					}
					input = 'O'; //input is now S since the players lost					
				}
				cont = false; //if the dealer doesn't have 21 we don't care about this anymore
			}
			/**
				As long as the players score is less than 21
			*/
			if (rezultat(players[0].hand) <= 21) {
				/**
				The most complicated first - if they have a pair of 5's they can split, double down, hit, or stay
				*/
				if (((players[i].hand[0].vrijednost >= 10 && players[i].hand[1].vrijednost >= 10) || players[i].hand[0].vrijednost == players[i].hand[1].vrijednost) && players[i].hand.size() == 2 && rezultat(players[i].hand) == 10) {
					cout << players[i].info.korisnicko_ime << " rezultat: " << rezultat(players[i].hand) << endl; //shows them their score
					cout << "Zelite li duplo ili nista(D), podijeli(P), hit(H), ili ostani(O), zadano je da ostanes?" << endl; //ask them
				}
				else if (((players[i].hand[0].vrijednost >= 10 && players[i].hand[1].vrijednost >= 10) || (players[i].hand[0].vrijednost == players[i].hand[1].vrijednost)) && players[i].hand.size() == 2) { //if they can split their cards
					cout << players[i].info.korisnicko_ime << " rezultat: " << rezultat(players[i].hand) << endl; //shows them their score
					cout << "Zelite li podijeliti(P) svoje karte, uzeti hit(H), ili ostati(O), zadano je da ostanes?" << endl; //ask them
				}
				else if (players[i].hand.size() == 2 && rezultat(players[i].hand) >= 9 && rezultat(players[i].hand) <= 11 && !(imaAsa(players[i].hand))) { //can they double down
					cout << players[i].info.korisnicko_ime << " rezultat: " << rezultat(players[i].hand) << endl; //shows them their score
					cout << "Zelite li duplo ili nista(D), uzeti hit(H), ili ostati(O), zadano je da ostanete?" << endl; //asks them
				}
				else { //they can't do anything special
					cout << players[i].info.korisnicko_ime << " rezultat: " << rezultat(players[i].hand) << endl; //shows them their score
					cout << "Hit(H) ili ostani(O), zadano je da ostanete?"; //asks them what they want to do
				}
				cin >> input; //takes in the input
				switch (input) { //what did they choose?
				case 'P': //they wanted to split
					sijeci(players[0], players[i]); //we split them
					printajKarte(players[i].hand); //reprint their cards in case they forgot
					break;
				case 'D':
					Duplo_il_Nista(players[0], players[i]); //they double down
					input = 'O'; //sets input to S since now they are done
					break;
				case 'H':
					players[i].hand.push_back(Jos()); //we give them one more card for their hit
					printajKarte(players[i].hand); //reprint their cards
					cout << players[i].info.korisnicko_ime << " rezultat je sada " << rezultat(players[i].hand) << endl; //reprint their score
					break;
				default: //this is here for people can't follow directions
					input = 'o'; //input is S
				}
				if (rezultat(players[i].hand) > 21) { //if they bust they are done
					input = 'O'; //so we can quit
				}
			}
		} while (!(input == 'O' || input == 'o')); //we continue doing this until they are want to stay
	}

	diler_igra(players[0]); //now the dealer plays

	players[0].hand[0].up = true; //now the everybody can see the first card

	/**
		The below method shows everybody's score and cards including dealers
	*/
	for (int i = 0; i < players.size(); i++) {
		cout << players[i].info.korisnicko_ime << " rezultat: " << rezultat(players[i].hand) << " Karte: ";
		printajKarte(players[i].hand);
	}

	/**
		The below method pays everybody out
	*/
	for (int i = 1; i < players.size(); i++) {
		if (rezultat(players[i].hand) > 21) { //if the player busted we tell them
			cout << "Izgubio si! ";
		}
		int win = pobjednik(players[0], players[i]); //we figure out who wins
		if (win == 1) {
			players[i].info.pobjede += 1; //if the player wins we add one to their win record
		}
		isplata(players[0], players[i]); //we payout everybody
		ocisti(players[i].hand); //we clear out their hands
		players[i].info.odigrano += 1; //adds one to the total played	
	}

	ocisti(players[0].hand); //clear out the dealers hand
}
/**
	The below method clears out a players hand
*/
void ocisti(vector<karta>& hand) {
	hand.clear();
}
/**
Creates new accounts for new players
*/
racun stvori() {
	racun user; //creates a new account
	cout << "Koje korisnicko ime zelite koristiti?" << endl;
	fstream input; //so we can take in the input
	string filename; //what file we will open

	/**
		The below method basically checks to see if a username is available by checking to see if the file can even be open
	*/
	do {
		cin >> user.korisnicko_ime; //takes in the username
		filename = user.korisnicko_ime + ".dat"; //adds the extension to the file
		input.open(filename); //opens the file
		if (!input.fail()) { //if it didn't fail the name is taken
			cout << "Ovo korisnicko ime je zauzeto, odaberite drugo." << endl; //we tell the user
		}
	} while (!input.fail()); //repeat until it doesn't fail

	cout << "Unesite lozinku." << endl; //asks for password
	cin.ignore(); // so cin will work
	getline(cin, user.lozinka); //takes in the line b/c it may be more than one word
	cin.ignore(); //so the next line will wokr

	cout << "Unesite svoje ime." << endl; //asks for the users name
	getline(cin, user.ime); //takes it in

	cout << "Unesite svoju email adresu." << endl; //asks for email address
	cin >> user.email; //takes it in

	cout << "Unesite koliko novaca zelite uplatiti." << endl; //asks for money
	cin >> user.novac; //takes it in

	user.odigrano = 0; //sets these to 0
	user.pobjede = 0; //sets these to 0
	user.postotak_pobjeda = 0; //set these to 0

	input.close(); //closes the input stream
	return user; //returns the account
}
/**
	For saving account info for when you're done
*/
void spremi(igrac user) {
	ofstream output; //output stream
	string filename = user.info.korisnicko_ime + ".dat"; //so we know what file to save to

	output.open(filename, ios::out); //we want to erase all previous data

	if (!output) {//if we can't open output
		cerr << "Output file will not open and data could not be saved. " << endl;
	}

	double percent = 0; //for the win percentage
	if (user.info.odigrano == 0) {
		percent = 0; //so we don't divide by zero
	}
	else {
		percent = ((double)user.info.pobjede / user.info.odigrano) * 100; //win percentage
	}
	output << user.info.korisnicko_ime << endl << user.info.lozinka << endl << user.info.ime << endl << user.info.email << endl << user.info.novac << endl << user.info.odigrano << endl << user.info.pobjede << endl << setprecision(3) << percent; //writes the data to the file

	output.close(); //closes the output stream
}

/**
For loading account info
*/
racun ucitaj() {
	string username, password; //so we know what the username and password is
	string filename; //how we will save the data
	racun user; //sets up an account so we can take in the data
	ifstream input; //file stream 

	/**
		We get the users username and password
	*/
	do {
		cout << "Koje je vase korisnicko ime?" << endl; //asks for username
		cin >> username; //take it in
		filename = username + ".dat"; //append the file extension
		input.open(filename); //opens the file //opens the file
	} while (input.fail()); //if it fails to open they probably put in the wrong username so we keep doing this until they get it right

	getline(input, user.korisnicko_ime); //take in the username
	getline(input, user.lozinka); //take in the password

	do {
		cout << "Koja je vasa lozinka?" << endl; //asks for the password
		cin >> password; //takes it in
	} while (!(password == user.lozinka)); //checks password and keeps checking until the user gets it right

	/**
		Takes in all the user info
	*/
	getline(input, user.ime);
	getline(input, user.email);
	input >> user.novac;
	input >> user.odigrano;
	input >> user.pobjede;
	input >> user.postotak_pobjeda;

	input.close(); //close out the file
	return user; //returns the account info
}
/**
	if the dealer is displaying an Ace it asks all the players if they want to take out insurance
*/
void osiguranje(vector<igrac>& players) {
	int size = players.size(); //gets the size of the vector

	for (int i = 1; i < size; i++) {
		int bet;
		players[i].oklada_na_osiguranje = 0;
		if ((players[i].info.novac - (players[i].ulog / 2)) >= 0) { //can the player even make an insurance bet
			do {
				cin.ignore();
				cout << "Koliko zelite uplatiti za sigurnost, do " << (players[i].ulog / 2) << players[i].info.korisnicko_ime << "?(0 ako se ne zelite osigurati)" << endl; //asks if they want to make an insurance bet
				cin >> bet; //takes it in
			} while (!(bet <= (players[i].ulog / 2) && bet >= 0)); //continues to do this until they input the right stuff

			if (bet != 0) { //did they make a insurance bet
				players[i].oklada_na_osiguranje = bet; //we have the insurance bet set
				players[i].info.novac -= bet; //we take it out of their money pile
			}
		}
	}

	if (rezultat(players[0].hand) == 21) { //the dealer has 21
		for (int i = 1; i < size; i++) {
			if (players[i].oklada_na_osiguranje != 0) { //if they took out insurance
				players[i].info.novac += (players[i].oklada_na_osiguranje * 2); //pays out 2 to 1
				players[i].oklada_na_osiguranje = 0; //clears out the insurance bet
			}
		}
	}
	else {
		for (int i = 1; i < size; i++) {
			if (players[i].oklada_na_osiguranje != 0) { //if they took out insurance
				players[i].oklada_na_osiguranje = 0; //clears out the insurance bet
			}
		}
	}
}

/**
   Asks the user what they would like to bet and returns it
*/
void ulog(igrac& user) {
	int bet;
	do {
		cout << user.info.korisnicko_ime << endl; //so we know what player is betting
		cout << "Koliko novaca ulazete u igru? (Mora biti vece od " << MIN_ULOG << " i manje od " << user.info.novac << ")" << endl; //we tell them what they can bet
		cin >> bet; //takes in their bet
	} while (!(bet >= MIN_ULOG && bet <= user.info.novac)); //repeat until they get it right
	user.info.novac -= bet; //subtract the bet from their money stock pile
	user.ulog = bet; //set their bet
}
/**
	If the user chooses to split their cards
	The split hand is completely played here
*/
void sijeci(igrac& dealer, igrac& user) {
	igrac split;
	vector<igrac> players; //creates a new player for the split
	players.push_back(dealer);

	split.ulog = user.ulog; // takes the extra bet
	user.info.novac -= user.ulog; //takes out the bet from the user money pile
	split.hand.push_back(user.hand[0]); //takes the first card from the user and gives it to the split player
	split.hand.push_back(dijeli()); //gives the split person a new card
	user.hand[0] = dijeli(); //gives the user a new card
	split.info.korisnicko_ime = user.info.korisnicko_ime;
	players.push_back(split); //pushes the split player onto the vect

	printajKarte(players[1].hand); //prints out the new cards

	char input; //what the input is
	do {
		for (int i = 1; i < players.size(); i++) {
			if (rezultat(players[i].hand) > 21) { //if they bust
				input = 'O'; //they are done
			}
			else {
				cout << "Hit(H) ili Ostani(O):"; //otherwise we ask them if they want to take a hit
				cin >> input; //take in their input
			}
			/**
				If they take a hit the below code gives them a new card, prints out their new cards and their new score
			*/
			if (input == 'H' || input == 'h') {
				players[i].hand.push_back(Jos());
				printajKarte(players[i].hand);
				cout << players[i].info.korisnicko_ime << " rezultat je sad " << rezultat(players[i].hand) << endl;
			}
		}
	} while (!(input == 'O' || input == 'o')); //we repeat this until they stop taking hits or bust

	diler_igra(players[0]); //dealer plays now since we need to take care of this now rather than later

	/**
		prints the split players cards and score again
	*/
	for (int i = 1; i < players.size(); i++) {
		cout << players[i].info.korisnicko_ime << " rezultat: " << rezultat(players[i].hand) << " Karte:" << endl;
		printajKarte(players[i].hand);
	}

	/**
		payouts the split player
	*/
	for (int i = 1; i < players.size(); i++) {
		if (rezultat(players[i].hand) > 21) {
			cout << "Izgubio si!"; //tells them they busted
		}
		isplata(players[0], players[i]); //plays out the player
	}
}
/**
	Typical pay out rules. If the player wins he gets 2 to 1 odds.
*/
void isplata(igrac dealer, igrac& user) {
	if (pobjednik(dealer, user) == 1) { //if the player won
		if (rezultat(user.hand) == 21 && imaAsa(user.hand) && user.hand.size() == 2) { //if the player has blackjack it's a 3:2 payout
			user.info.novac += ((user.ulog * 3) / 2); //I think this is 3:2 odds
			user.ulog = 0; //clears out the bet
			cout << user.info.korisnicko_ime << " je pobjedio!" << endl;
		}
		else {
			user.info.novac += (user.ulog * 2); //adds the bet to the players stash of cash
			user.ulog = 0; //clears out the bet
			cout << user.info.korisnicko_ime << " je pobjedio!" << endl;
		}
	}
	else if (pobjednik(dealer, user) == 0) { //they tied
		user.info.novac += user.ulog; //players money goes back to his pile
		user.ulog = 0; //clears out the bet
		cout << user.info.korisnicko_ime << "je izjednacio!" << endl;
	}
	else { //the dealer won.
		user.ulog = 0; //player didn't win so all we need to do is clear out. 
		cout << user.info.korisnicko_ime << " je izgubio!" << endl;
	}
}
/**
	Figures out if the dealer or the user won
	returns 1 if the player won
	returns -1 if the dealer won
	returns 0 if they tie
*/
int pobjednik(igrac dealer, igrac& user) {
	if (rezultat(dealer.hand) == rezultat(user.hand)) { //they had a stand off
		return 0;
	}
	else if (((rezultat(dealer.hand) < rezultat(user.hand)) && (rezultat(user.hand) <= 21)) || (rezultat(dealer.hand) > 21 && rezultat(user.hand) <= 21)) { //user won
		return 1;
	}
	else { //dealer typically wins
		return -1;
	}
}
/**
	Double down
	If the user chooses to double down we take in the new amount betted, deal one more card, and figure out if the user won
	takes in the user and dealer players and returns nothing
*/
void Duplo_il_Nista(igrac dealer, igrac& user) {
	int bet; //so we can store the new bet
	do {
		cout << "Koliko zelite uloziti? (Mora biti vise od " << MIN_ULOG << " i manje od " << user.info.novac << ")" << endl; //asks how much they want to bet
		cin >> bet; //takes it in
	} while (!(bet > MIN_ULOG && bet <= user.ulog && bet <= user.info.novac)); //we repeat this until they get it right

	user.ulog += bet; //add the new bet to the original
	user.info.novac -= bet; //takes out their double down bet from the money
	user.hand.push_back(dijeli()); //gives the user one more card
	isplata(dealer, user); //pays out the player based on if he/she won		
}
/**
Returns the Character version of the suit
Fairly self-explanatory...takes in a number between 0-4 and returns the suit based off that number
*/
char printajBoju(karta new_card) {
	switch (new_card.suit) {
	case 0:
		return 'P';
	case 1:
		return 'H';
	case 2:
		return 'K';
	case 3:
		return 'T';
	}
}

/**
Prints the cards to the screen
*/
void printajKarte(vector<karta> hand) {
	const string CARD_VALUES[14] = { "A", "2", "3", "4", "5", "6", "7", "8", "9", "10", "J", "Q", "K", "X" }; //makes it easier to print

	for (int i = 0; i < hand.size(); i++) {
		/**cout << "----------" <<  endl
			<< "|" << ((hand[i].up)?(hand[i].value):('X')) << ((hand[i].up)?((printSuit(hand[i]))):('X')) << setw(6) << "|" << endl
				<< "|" << setw(8) << "|" << endl
				<< "|" << setw(8) << "|" << endl
				<< "|" << setw(8) << "|" << endl
				<< "|" << setw(6) << ((hand[i].up)?(hand[i].value):('X')) <<  ((hand[i].up)?(printSuit(hand[i])):('X')) << "|" << endl
				<< "----------" << endl;*/
		if (hand[i].up) { //if the hand is face up we print this
			cout << CARD_VALUES[(hand[i].vrijednost - 1)] << printajBoju(hand[i]) << " ";
		}
		else { //if it's face down we print XX
			cout << CARD_VALUES[13] << CARD_VALUES[13] << " ";
		}
	}
	cout << endl;
}
/**
	Lets us know if the hand has an ace
*/
bool imaAsa(vector<karta> hand) {
	bool has_ace = false; //For now we say there is no ace in the hand
	for (int i = 0; i < hand.size(); i++) {
		if (hand[i].vrijednost == 1) { //we have an ace
			has_ace = true; //so we set this to true
		}
	}

	return has_ace;
}
/**
	Gets the score for the user
	We treat Aces initially as 1 and then later check to see if the hand contains an Ace
*/
int rezultat(vector<karta> hand) {
	int total = 0; //setting up the total value
	for (int i = 0; i < hand.size(); i++) {
		if (hand[i].vrijednost >= 10) { //if it's 10, J, Q, or K
			total += 10; //adds 10 to the toal
		}
		else {
			total += hand[i].vrijednost; //adds the value to the total 
		}
	}

	if (imaAsa(hand) && total <= 11) { //if the hand has an ace and we won't bust
		total += 10; //add 10
	}

	return total; //return the total
}
//gets a new card for the player
karta Jos() {
	return dijeli(); //add another card to the players hand
}
/**
	takes in nothing and returns a card
	makes a new card and assigns it a random value between 1-13
	assigns a suit to the card as well
	returns the card
*/
karta dijeli() {
	karta new_card; //card we will be returning

	new_card.vrijednost = 1 + rand() % 13; //makes sure the random number is between 1 and 13 for the value
	int suit = rand() % 4;
	switch (suit) { //makes sure the random number is between 1 and 4 for the suit
	case 0:
		new_card.suit = P;
		break;
	case 1:
		new_card.suit = H;
		break;
	case 2:
		new_card.suit = K;
		break;
	case 3:
		new_card.suit = T;
		break;
	}

	new_card.up = true; //we'll change it later if it's the dealers face down card

	return new_card; //returning the card
}