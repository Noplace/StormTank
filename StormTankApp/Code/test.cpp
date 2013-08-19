/*
  karplus strong algoritmus
  daniel bartos (c) 2002

  karplus strong verze 4

  ***********
  karplus.cpp


pole odpovida normalni wave tabulce, ktere ale bude
dalsimi pruchody upravovano karplus strong algoritmem

441 samplu dlouhe pole bude zakladem pro ton o vysce 100Hz pri
uvazovane vzorkovaci frekvenci 44100Hz (sto pruchodu polem za sekundu)

pocet samplu udava delku souboru - pri 44100Hz to bude jedna sekunda

zvuk je ulozen v .raw podobe do souboru spinet.raw ve stejnem adresari:
spinet.raw 44100Hz, 16bit, mono

pole lze take krome nahodnych hodnot naplnit samply ze souboru
sample.raw 44100Hz, 16bit, mono, ktery musi byt ve stejnem adresari
zmena se muze projevit v pomeru vysokych frekvenci a v hlasitosti
vysledneho tonu

.raw soubory lze editovat v sound forge a jinych sample editorech
je treba vedet, ze vsechny .raw soubory, ktere jsou pouzity a zapisovany
jsou: 44100Hz, 16bit - signed integer, little endian order (Intel), mono

co program obsahuje>

1 - zadani delky pole a vypis prvku pole
2 - vypis sumy prvku a jejich prumeru = distribuce nahodnych cisel
	 (idealne by prumer mel byt 0)
3 - zadani delky vysledneho souboru
4 - vypis vsech samplu, ktere se ukladaji do souboru .raw,
	 program se da zastavit klavesou "pause" a daji se porovnat
	 upravy v tabulce

5 - !!fuknce insert_sample umoznuje naplnit pole hodnotami ze souboru
	 sample.raw, ktery je ve stejnem adresari
6 - rozhodovaci rozcesti:
	 A: pouzit sample.raw nebo nahodna cisla k naplneni tabulky
	 B: povolit vypisovani vsech samplu?
			* pokud vypisuji vsechny samply mohu sledovat kdy samply zacnou
			konvergovat k nule
			* bez vypisovani cas zapisu 1Mb souboru je radove v sekundach zatimco
			s vypisem muze trvat 15 minut

7 - ukladani zvuku do souboru .WAV
*/


#include <stdio.h>
#include <stdlib.h>
#include <conio.h>
#include <memory.h>
#include <time.h>

class karplus{
	int delka;
	int *pole;
	FILE *raw;
	FILE *wav;
	FILE *in;


	//aktualni = index samplu v poli
	unsigned int aktualni;
	//poradi samplu v souboru, pocet samplu v souboru
	unsigned long cislo_samplu, velikost_souboru;
	//funguji jako boolean
	int choice_sample, choice_vypis_samplu;

public:
	void start();
	void init();
	void write(int index, int sample);
	void display();
	void noise();
	void insert_sample();

	int  strong(int pozice);
	void run();

	void open_file();
	void write_to_file(int sample);
	void close_file();

	void open_wav();
	void write_to_wav(int sample);
	void close_wav();

	void freemem();

	void choice_sum_versus_sample();
	void choice_vypis();


};

//------------------------------------------------------------
//ziskani udaju o velikosti tabulky a delce vysledneho souboru
void	karplus::start(){


			//printf("daniel bartos (c) 2002, karplus/strong algoritmus\n");

			////printf("velikost pole: ");
			//scanf("%d", &delka);
      delka = 44100 / 440;
			////printf("pocet samplu: ");
			//scanf("%lu", &velikost_souboru);

      velikost_souboru = 44100;

			//alokace pole pro samply
      pole = new int[delka * 2];
			if (pole == nullptr){
				//printf("malo pameti!! \n");
				exit(1);
			}
}

//--------------------------------
//nastavi vsechny samply pole na 0
void	karplus::init(){
			for(int i = 0; i < delka; i++){
			write(i, 0);
			}

}

//---------------------------------------
//zapis samplu do pole na vybranem indexu
void	karplus::write(int index, int sample){
			pole[index] = sample;
			}

//---------------------------
//informace o alokovanem poli
void  karplus::display(){
			//printf("\nvypis pole:\n");

			int suma=0, prumer=0;

			for(int i = 0; i < delka; i++){
				//printf("index: %d ma hodnotu: %d \n", i, pole[i]);
				suma += pole[i];
				}

			//printf("\nalokovane pole ma delku: %d \n", delka);
			//printf("velikost pole v bajtech: %d \n", (delka * 2));
			//printf("pocet samplu: %lu\n", velikost_souboru);
			//printf("velikost souboru RAW v bajtech: %lu\n", (velikost_souboru * 2));
			//printf("velikost souboru WAV v bajtech: %lu\n", (velikost_souboru * 2)+44);

			prumer = suma / delka;
			//printf("\nsuma prvku: %d    prumer = rovnomernost distribuce: %d\n",
//							suma, prumer);
}

//-------------------------------
//naplni pole nahodnymi hodnotami
void	karplus::noise(){

		//nahodna cisla budou vzdy jina, pouziva se systemovy cas
		srand((unsigned int)time(0));

		//rozptyl hodnot <-7500, 7500>
		for(int i = 0; i < delka; i++)
		//write(i, ((rand() % 15000) - 7500) );


		write(i, ((rand() % 30000) - 15000) );


		}

//---------------------------------------------------------------------
//vklada hodnoty ze souboru sample.raw do tabulky misto nahodnych cisel
void  karplus::insert_sample(){

		//****rozcesti*******************
		//rozhodnuti o pouziti sample.raw
		if(choice_sample == 1);
		else return;
		//****rozcesti*******************

		int sample;
		unsigned long delka_sample;


		//pokus o otevreni sample.raw
		if((in = fopen("sample.raw","rb"))==NULL)
		{
		//printf("\nsoubor sample.raw nejde otevrit\n");
		//printf("pouziji se nahodna cisla\n");

		while(!_kbhit());

		return;
		}

		//zjisti jestli sample.raw naplni celou tabulku
		fseek(in, 0, SEEK_END);
		delka_sample = ftell(in);
		//ukazovatko opet na zacatek!!
		fseek(in, 0, SEEK_SET);

		//porovnavam pocet hodnot int - kazda ma 2 bajty
		if( ((delka_sample) / 2) < (unsigned long) delka)
			{
			//printf("\nsample.raw nenaplnil celou tabulku!");
			//printf("\npouziji se nahodna cisla\n");

			while(!_kbhit());

			}

		else
			{
			//pruchod pres vsechny indexy
			for(int i = 0; i < delka; i++){
				fread(&sample, 2, 1, in);

				//printf("\nindex: %d    sample: %d", i, sample);

				write(i, sample);
				}

			//printf("\n\nsoubor sample.raw naplnil tabulku! ok..\n");

			while(!_kbhit());


			}
		//zavira sample.raw
		fclose(in);
}

//-----------------------
//vlastni smycka nastroje
//pocet pruchodu urcuje velikost souboru
//vola fci strong - vlastni karplus/strong algoritmus
//pak vola zapis do souboru
void  karplus::run(){

		int output;
		aktualni = 0;

			for( unsigned long a = 0; a < velikost_souboru; a++){

				if(aktualni == delka)
					{
					aktualni = 0;
					}

			cislo_samplu = a;

			output = strong(aktualni);
			//write_to_file(output);
			write_to_wav(output);

			aktualni++;
			}

			//printf("\nvytvoren soubor spinet.raw a spinet.wav!\n\n");
}

//-------------------------
//karplus/strong algoritmus
//precte hodnotu z pole, zprumeruje ji s
//predchazejici hodnotou a zapise misto predchazejici hodnoty.
//hodnoty pri vetsim poli konverguji k nule - jinak konverguji
//k nejake jine hodnote, ktera bude na vsech samplech stejna,
//jinymi slovy ton nastroje dozni
int   karplus::strong(int pozice){

		int out, previous;

		if(pozice == 0)
			{
			out = pole[pozice];
			previous = pole[delka - 1];
			pole[delka-1] = (out + previous)/2;
			}

		else
			{
			out = pole[pozice];
			previous = pole[pozice - 1];
			pole[pozice-1] = (out + previous)/2;
			}

		return out;
		}

//-------------------------------------
//otevira soubor pro vystup: spinet.raw
void  karplus::open_file(){

	if((raw = fopen("spinet.raw","wb"))==NULL)
		{
		//printf("soubor spinet.raw nejde otevrit\n");
		exit(1);
		}
}

//----------------
//zapis do souboru
void  karplus::write_to_file(int sample){

		//****rozcesti*2****
		if(choice_vypis_samplu == 1){
		//****rozcesti*2****
		//printf("sample cislo: %lu   na indexu: %u   ma hodnotu: %d\n",
					//cislo_samplu, aktualni, sample);
		}

		fwrite(&sample, 2, 1, raw);
}

//---------------
//zavreni souboru
void  karplus::close_file(){

		fclose(raw);
		}

//-------------------------------------
//otevira WAV soubor pro vystup: spinet.wav
void  karplus::open_wav(){

	if((wav = fopen("spinet.wav","wb"))==NULL)
		{
		//printf("soubor spinet.wav nejde otevrit\n");
		exit(1);
		}

		//unsigned long int raw_delka, pocetsamplu;
		//float sekundy;

				//udaje o poctu bajtu v raw souboru
				//nastaveni pozice v souboru na konec
		//fseek(rawread, 0, SEEK_END);
				//urceni polohy v souboru = delka v bajtech
		//raw_delka = ftell(rawread);


		//************WAV*promenne*****************
		//inicializace promennych pro hlavicku .WAV
		unsigned long int datasize, datachunksize;
		unsigned long int samplerate, bytespersec;
		int kanaly, bytes_in_one_time, bitspersample;
				//hodnoty vzdy stejne:
		unsigned long int format_chunk_size;
		int format;


		//hlavicka pro mono, 44100, 16 bit
		samplerate = 44100;
		bytespersec = 88200;
		bytes_in_one_time = 2;
		bitspersample = 16;
		kanaly = 1;

		format_chunk_size = 16;
		format = 1;

				//nastaveni promennych podle informaci o .RAW souboru
				//vlastni velikost RAW dat
				//v nasem pripade pocet samplu * 2bajty (je to 16bit!)
		datachunksize = velikost_souboru * 2;
				//pripocteni zbyvajicich 36 bajtu hlavicky k syrovym datum
				//hlavicka ma 44 bajtu dohromady bez prvnich 8 tedy 36
		datasize = (long)36 + datachunksize;


		//*************WAV*hlavicka**************
		//zapis hlavicky .WAV se vsemi promennymi
		//riff chunk, dohromady 12bytu>
		//byte 0-3
		fputs("RIFF",wav);

		//delka souboru co nasleduje, bez prvnich 8bytu
		//(to jsou: oznaceni riff a velikost)
		//byte 4-7
		fwrite(&datasize, 4, 1, wav);

		//byte 8-11
		fputs("WAVE",wav);

		//format chunk, dohromady 24bytu>
		//oznaceni fmt - 4byty
		//byte 12-15
		fputs("fmt ",wav);

		//delka format chunk, vzdy je 0x10, long int 16.
		//byte 16-19
		fwrite(&format_chunk_size, 4, 1, wav);

		//vzdy 1, 0x01, int 1.
		//byte 20-21
		fwrite(&format, 2, 1, wav);

		//mono stereo, 0x01 mono, 0x02 stereo...
		//byte 22-23
		fwrite(&kanaly, 2, 1, wav);

		//sample rate, 44100 binarne 44 AC 00 00
		//byte 24-27
		fwrite(&samplerate, 4, 1, wav);

		//bytes per second
		//mono 44100 8bit=     44100 bytu za vterinu
		//mono 44100 16bit=    88200 bytu za vterinu
		//stereo 44100 16bit= 176400 bytu za vterinu
		//byte 28-31
		fwrite(&bytespersec, 4, 1, wav);

		//bytes per sample
		//1 - 8bit mono, 2 - 8bit stereo nebo 16bit mono, 4 - 16bit stereo
		//byte 32-33
		fwrite(&bytes_in_one_time, 2, 1, wav);

		//bits per sample
		//byte 34-35
		fwrite(&bitspersample, 2, 1, wav);

		//chunk data
		//byte 0-3 of data chunk
		//oznaceni data je nekdy dal, mezi mohou byt ulozena dalsi info
		//jako treba copyright

		fputs("data",wav);

		//length of data to follow
		//byte 4-7 of data chunk
		fwrite(&datachunksize, 4, 1, wav);


		//****Samply*****************************************
		//zapis vlastnich samplu fci write_to_wav(int sample)
		//byte 8 - end of file


}

//----------------
//zapis do souboru
void  karplus::write_to_wav(int sample){

		fwrite(&sample, 2, 1, wav);
}

//---------------
//zavreni souboru
void  karplus::close_wav(){

		fclose(wav);
		}

//-------------------------------
//uvolni alokovanou pamet pro pole
void	karplus::freemem(){
			//free((void *) pole);
      delete [] pole;
			pole = NULL;
		}


//--------------------------------
//metody, ktere vytvareji rozcesti
//pouze nastavuji dve globalni promenne, ktere podminuji vykonani fci
// * choice_sample podminuje vykonani insert_sample
// * choice_vypis_samplu podminuje vypisovani samplu ve fci write_file

void  karplus::choice_sum_versus_sample(){
			//****rozcesti****
			//rozhodnuti o pouziti metody insert sample
			//printf("\npouzit k naplneni tabulky sample.raw?\n");
			//printf("1  =  nahodna cisla        2  =  pouzit sample.raw\n\n");

			int c; //scanf("%d", &c);
      c = 1;
			if(c == 1) choice_sample = 0;
			if(c == 2) choice_sample = 1;
			else choice_sample = 0;
			//****rozcesti****
}

void  karplus::choice_vypis(){
			//****rozcesti****
			//bez vypisu jednotlivych samplu se urychli ukladani do souboru
			//printf("\nvypisovat vsechny samply?\n");
			//printf("1  =  ne         2  =  ano\n\n");

			int c=1; //scanf("%d", &c);
			if(c == 1)  choice_vypis_samplu = 0;
			if(c == 2){ choice_vypis_samplu = 1;
							//printf("vytvarim soubor spinet.raw ...\n\n");
						 }

			//****rozcesti****
}




void test_karplusStrong() {

karplus alfa;
memset(&alfa,0,sizeof(alfa));
alfa.start();
alfa.init();
alfa.noise();
//------------------------------
alfa.choice_sum_versus_sample();
//------------------------------
alfa.insert_sample();
alfa.display();
//------------------
alfa.choice_vypis();
//------------------
//alfa.open_file();
alfa.open_wav();
alfa.run();

//alfa.close_file();
alfa.close_wav();
alfa.freemem();

}