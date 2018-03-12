#include <Windows.h>
#include "resource.h"
#include <fstream>
#include <string>
//para el mcisendstring
#include <mmsystem.h>
#pragma comment(lib, "winmm.lib")

using namespace std;
HINSTANCE HRep, HMen ;
HWND HwRep;
HMENU HwMen;
int ID=0;//numeros
int C_Listas=0;//Cantidad de listas
int C_Canciones=0;//Cantidad de Canciones
struct Canciones
{
	Canciones *ant;
	int ID;
	char Nombre[30], Artista[30], Genero[30], Año[5];
	char Song[MAX_PATH];
	char Pic[MAX_PATH];
	Canciones *sig;
};

struct Listas
{
	Listas *ant;
	int ID;
	char Nombre[20];
	char Pic[MAX_PATH];
	Listas *sig;
	Canciones *Lista;
};
Listas *Inicio=NULL;
Listas *Lista_Actual = Inicio;
Canciones *Cancion_Actual = NULL;
Canciones *Cancion_playing = NULL;
void save_data();
void load_data();
//menu clic derecho
HMENU hMenu;
POINT point;
#define IDM_FILE_El 1
#define IDM_FILE_Mod 2
#define IDM_FILE_ElC 3
#define IDM_FILE_ModC 4
#define IDM_reproduce 666
#define IDM_reproduceALL 667
BOOL CALLBACK Reproductor(HWND hwndw, UINT msg, WPARAM wParam, LPARAM lParam);
BOOL CALLBACK Lista(HWND hwndL, UINT msg, WPARAM wParam, LPARAM lParam);
BOOL CALLBACK MLista(HWND hwndL, UINT msg, WPARAM wParam, LPARAM lParam);
BOOL CALLBACK Cancion(HWND hwndC, UINT msg, WPARAM wParam, LPARAM lParam);
BOOL CALLBACK MCancion(HWND hwndC, UINT msg, WPARAM wParam, LPARAM lParam);
void Eliminar_cancion();
int WINAPI WinMain(HINSTANCE hInst, HINSTANCE hPrevInst, LPSTR, int nShowCMD)

{
	HRep = hInst;
	HwRep = CreateDialog(HRep, MAKEINTRESOURCE(IDD_PLAYER), NULL, Reproductor);
	

	HwMen = LoadMenu(HMen, MAKEINTRESOURCE(IDR_MenuPrincipal));
	SetMenu(HwRep, HwMen);

	ShowWindow(HwRep, nShowCMD);


	MSG msg;
	while (GetMessage(&msg, NULL, 0, 0) > 0)
	{
		TranslateMessage(&msg);
		DispatchMessage(&msg);
		

	}return msg.wParam;

}
int GetCountSongs(Listas *Lista){
	Canciones *aux = Lista->Lista;
	
	//Listas *aux = Lista;
	if (aux == NULL)
	return 0;
	if (aux != NULL){
		int i = 0;
		while (aux != NULL){
			i++;
			aux = aux->sig;
		
		}return i;
	}
	return 0;
}
void Lisbox_Canciones(HWND HCanN, HWND HCanNo, HWND  HCanA, HWND HCanG, HWND HCanD){

	SendMessage(HCanN, LB_RESETCONTENT, 0, 0);
	SendMessage(HCanNo, LB_RESETCONTENT, 0, 0);
	SendMessage(HCanA, LB_RESETCONTENT, 0, 0);
	SendMessage(HCanG, LB_RESETCONTENT, 0, 0);
	SendMessage(HCanD, LB_RESETCONTENT, 0, 0);
	char N_c[3];
	if (Lista_Actual != NULL){
		if (Lista_Actual->Lista != NULL){
			Canciones *aux = Lista_Actual->Lista;
			while (aux != NULL){
				strcpy(N_c, "");
				_itoa(aux->ID, N_c, 10);
				SendMessage(HCanN, LB_ADDSTRING, 0, (LPARAM)N_c);
				SendMessage(HCanNo, LB_ADDSTRING, 0, (LPARAM)aux->Nombre);
				SendMessage(HCanA, LB_ADDSTRING, 0, (LPARAM)aux->Artista);
				SendMessage(HCanG, LB_ADDSTRING, 0, (LPARAM)aux->Genero);
				SendMessage(HCanD, LB_ADDSTRING, 0, (LPARAM)aux->Año);
				aux = aux->sig;
			}
		}
	}
}



void Lisbox_Listas(HWND hwndw, HWND HListN, HWND HListNo){

	Listas *aux = Inicio; char ID_L[3];
		HListN = GetDlgItem(hwndw, IDC_LISTAS);
		HListNo = GetDlgItem(hwndw, IDC_LISTAS2);
	
	SendMessage(HListN, LB_RESETCONTENT, 0, 0);
	SendMessage(HListNo, LB_RESETCONTENT, 0, 0);
	while (aux != NULL){
		
		SendMessage(HListNo, LB_ADDSTRING, 0, (LPARAM)aux->Nombre);
		strcpy(ID_L, "");
		_itoa(aux->ID, ID_L, 10);
		SendMessage(HListN, LB_ADDSTRING, 0, (LPARAM)ID_L);


		aux = aux->sig;
	}

}
Listas buscar_lista(char idb[3]){
	int IDb = atoi(idb);
	if (Inicio != NULL){
		Listas *aux = Inicio;
		while (aux->sig != NULL){
			if (IDb == aux->ID){
				MessageBox(0, aux->Nombre, "NOMBRE de la lista", 0);
				return *aux;
			}
			aux = aux->sig;
		}
	}
	return *Inicio;
};
HWND actualizar;
char Playsong[MAX_PATH];
void play(){
	if (Cancion_playing != NULL)
	{

		strcpy_s(Playsong, "");
		strcat_s(Playsong, "open \"");
		strcat_s(Playsong, Cancion_playing->Song);
		strcat_s(Playsong, "\" alias mp3");
		mciSendString(Playsong, NULL, 0, actualizar);
		static HBITMAP bmpc1, bmpc2;
		// play 
		mciSendString("play mp3 notify", NULL, 0, 0);
		bmpc2 = (HBITMAP)LoadImage(NULL, Cancion_playing->Pic, IMAGE_BITMAP, 130, 130, LR_LOADFROMFILE);
		SendDlgItemMessage(actualizar, IDC_Pic_cancion, STM_SETIMAGE, IMAGE_BITMAP, (LPARAM)bmpc2);
	}

}
char gneros[80][20]{"Blues", "Classic Rock", "Country", "Dance", "Disco", "Funk", "Grunge", "Hip - Hop", "Jazz", "Metal", "New Age", "Oldies", "Other", "Pop", "R&B", "Rap", "Reggae", "Rock", "Techno", "Industrial", "Alternative", "Ska", "Death Metal", "Pranks", "Soundtrack", "Euro - Techno", "Ambient", "Trip - Hop", "Vocal", "Jazz + Funk", "Fusion", "Trance", "Classical", "Instrumental", "Acid", "House", "Game", "Sound Clip", "Gospel", "Noise", "AlternRock", "Bass", "Soul", "Punk", "Space", "Meditative", "Instrumental Pop", "Instrumental Rock", "Ethnic", "Gothic", "Darkwave", "Techno - Industrial", "Electronic", "Pop - Folk", "Eurodance", "Dream", "Southern Rock", "Comedy", "Cult", "Gangsta", "Top 40", "Christian Rap", "Pop / Funk", "Jungle", "Native American", "Cabaret", "New Wave", "Psychadelic", "Rave", "Showtunes", "Trailer", "Lo - Fi", "Tribal", "Acid Punk", "Acid Jazz", "Polka", "Retro", "Musical", "Rock & Roll", "Hard Rock"};





bool playing;




BOOL CALLBACK Reproductor(HWND hwndw, UINT msg, WPARAM wParam, LPARAM lParam)
{
	static HWND BTPause, BTPlay;
	actualizar = hwndw;
	static HWND HListN, HListNo;
	static HWND HCanN, HCanNo, HCanA, HCanG, HCanD;
	int cursel1=0, cursel=0;
	static HBITMAP bmp1, bmp2;
	static HBITMAP bmpc1, bmpc2;
	static HWND HG;
	
	switch (msg)
	{
	case WM_INITDIALOG: {
							playing = false;
		load_data();
		BTPause = GetDlgItem(actualizar, IDC_PAUSE);
		BTPlay = GetDlgItem(actualizar, IDC_PLAY);
		
			ShowWindow(BTPause, SW_HIDE);
		
		HListN = GetDlgItem(actualizar, IDC_LISTAS);
		HListNo = GetDlgItem(actualizar, IDC_LISTAS2);
			
		HCanN = GetDlgItem(actualizar, IDC_LN);
		HCanNo = GetDlgItem(actualizar, IDC_LNo);
		HCanA = GetDlgItem(actualizar, IDC_LA);
		HCanG = GetDlgItem(actualizar, IDC_LG);
		HCanD = GetDlgItem(actualizar, IDC_LD);
		cursel = SendMessage(HListNo, LB_GETCURSEL, 0, 0);
			Lisbox_Listas(hwndw, HListN, HListNo);

		
		Lista_Actual = Inicio;
		Lisbox_Canciones( HCanN, HCanNo, HCanA, HCanG, HCanD);

		SendMessage(HListN, LB_SETCURSEL, 0, 0);
		SendMessage(HListNo, LB_SETCURSEL, 0, 0);
		SendMessage(HCanN, LB_SETCURSEL, 0, 0);
		SendMessage(HCanNo, LB_SETCURSEL, 0, 0);
		SendMessage(HCanA, LB_SETCURSEL, 0, 0);
		SendMessage(HCanG, LB_SETCURSEL, 0, 0);
		SendMessage(HCanD, LB_SETCURSEL, 0, 0);

		

		bmp1 = (HBITMAP)SendDlgItemMessage(hwndw, IDC_COVER, STM_GETIMAGE, IMAGE_BITMAP, 0);
		//bmp2 = (HBITMAP)LoadImage(NULL, "C:\\Users\\OzielLeos\\Documents\\mover\\PF PROGRAMACION II Reproductor\\PF PROGRAMACION II 3a\\ALL.bmp", IMAGE_BITMAP, 130, 130, LR_LOADFROMFILE);
		//strcpy(Lista_Actual->Pic, "C:\\Users\\OzielLeos\\Documents\\mover\\PF PROGRAMACION II Reproductor\\PF PROGRAMACION II 3a\\ALL.bmp");
		bmp2 = (HBITMAP)LoadImage(NULL, Lista_Actual->Pic, IMAGE_BITMAP, 130, 130, LR_LOADFROMFILE);
		SendDlgItemMessage(hwndw, IDC_Pic_list, STM_SETIMAGE, IMAGE_BITMAP, (LPARAM)bmp2);


		bmpc1 = (HBITMAP)SendDlgItemMessage(hwndw, IDC_COVER, STM_GETIMAGE, IMAGE_BITMAP, 0);
		bmpc2 = (HBITMAP)LoadImage(NULL, "error.bmp", IMAGE_BITMAP, 130, 130, LR_LOADFROMFILE);
		SendDlgItemMessage(hwndw, IDC_Pic_cancion, STM_SETIMAGE, IMAGE_BITMAP, (LPARAM)bmpc2);

		if (Lista_Actual!=NULL)
		{
			if (Lista_Actual->Lista != NULL)
			{
				Cancion_Actual = Lista_Actual->Lista;
				bmpc2 = (HBITMAP)LoadImage(NULL, Cancion_Actual->Pic, IMAGE_BITMAP, 130, 130, LR_LOADFROMFILE);
				SendDlgItemMessage(hwndw, IDC_Pic_cancion, STM_SETIMAGE, IMAGE_BITMAP, (LPARAM)bmpc2);
			}
		}

		HG = GetDlgItem(actualizar, IDC_Genero);
		for (int i = 0; i < 80; i++)
		{
			SendMessage(HG, CB_ADDSTRING, 0, (LPARAM)gneros[i]);
			
		}

	}return 1;

	case WM_COMMAND:
	{


		
		switch (LOWORD(wParam)) {
		

			//LISTBOXS//
		case IDC_LISTAS:{
							
							 cursel = SendMessage(HListN, LB_GETCURSEL, 0, 0);
							SendMessage(HListNo, LB_SETCURSEL, cursel, 0);
							char tempo2[3];
							strcpy(tempo2, "");
							SendMessage(HListN, LB_GETTEXT, cursel, (LPARAM)tempo2);
							
							
							int IDb = atoi(tempo2);
							if (Inicio != NULL){
								Listas *aux = Inicio;
								while (aux/*->sig*/ != NULL){
									if (IDb == aux->ID){
										//MessageBox(0, aux->Nombre, "NOMBRE de la lista", 0);
										Lista_Actual = aux;
										Lisbox_Canciones(HCanN, HCanNo, HCanA, HCanG, HCanD);
										bmp2 = (HBITMAP)LoadImage(NULL, Lista_Actual->Pic, IMAGE_BITMAP, 130, 130, LR_LOADFROMFILE);
										SendDlgItemMessage(hwndw, IDC_Pic_list, STM_SETIMAGE, IMAGE_BITMAP, (LPARAM)bmp2);
										return 0;
										break;
									}
									aux = aux->sig;
								}
							}
							
		}return true;
		case IDC_LISTAS2:{
							  cursel = SendMessage(HListNo, LB_GETCURSEL, 0, 0);
							SendMessage(HListN, LB_SETCURSEL, cursel, 0);
							char tempo2[3];
							strcpy(tempo2, "");
							SendMessage(HListN, LB_GETTEXT, cursel, (LPARAM)tempo2);


							int IDb = atoi(tempo2);
							if (Inicio != NULL){
								Listas *aux = Inicio;
								while (aux/*->sig*/ != NULL){
									if (IDb == aux->ID){
										//MessageBox(0, aux->Nombre, "NOMBRE de la lista", 0);
										Lista_Actual = aux;
										Lisbox_Canciones(HCanN, HCanNo, HCanA, HCanG, HCanD);
										bmp2 = (HBITMAP)LoadImage(NULL, Lista_Actual->Pic, IMAGE_BITMAP, 130, 130, LR_LOADFROMFILE);
										SendDlgItemMessage(hwndw, IDC_Pic_list, STM_SETIMAGE, IMAGE_BITMAP, (LPARAM)bmp2);
										return 0;
										break;
									}
									aux = aux->sig;
								}
							}
							
							
		}return true;
	//	canciones
		case IDC_LN:{
						
						 cursel1 = SendMessage(HCanN, LB_GETCURSEL, 0, 0);
						SendMessage(HCanNo, LB_SETCURSEL, cursel1, 0);
						SendMessage(HCanA, LB_SETCURSEL, cursel1, 0);
						SendMessage(HCanG, LB_SETCURSEL, cursel1, 0);
						SendMessage(HCanD, LB_SETCURSEL, cursel1, 0);
						//
						char tempo2[3];
						strcpy(tempo2, "");
						SendMessage(HCanN, LB_GETTEXT, cursel1, (LPARAM)tempo2);
						int IDb = atoi(tempo2);
						if (Inicio != NULL){
							
							Canciones *aux = Lista_Actual->Lista;
							while (aux/*->sig*/ != NULL){
								if (IDb == aux->ID){
									
									Cancion_Actual = aux;
									bmpc2 = (HBITMAP)LoadImage(NULL, Cancion_Actual->Pic, IMAGE_BITMAP, 130, 130, LR_LOADFROMFILE);
									SendDlgItemMessage(hwndw, IDC_Pic_cancion, STM_SETIMAGE, IMAGE_BITMAP, (LPARAM)bmpc2);
									//Lisbox_Canciones(HCanN, HCanNo, HCanA, HCanG, HCanD);
									return 0;
									break;
								}
								aux = aux->sig;
							}
						}
						//
		


		}return true;
		case IDC_LNo:{
							  cursel1 = SendMessage(HCanNo, LB_GETCURSEL, 0, 0);
							 SendMessage(HCanN, LB_SETCURSEL, cursel1, 0);
							 SendMessage(HCanA, LB_SETCURSEL, cursel1, 0);
							 SendMessage(HCanG, LB_SETCURSEL, cursel1, 0);
							 SendMessage(HCanD, LB_SETCURSEL, cursel1, 0);
							 //
							 char tempo2[3];
							 strcpy(tempo2, "");
							 SendMessage(HCanN, LB_GETTEXT, cursel1, (LPARAM)tempo2);
							 int IDb = atoi(tempo2);
							 if (Inicio != NULL){

								 Canciones *aux = Lista_Actual->Lista;
								 while (aux/*->sig*/ != NULL){
									 if (IDb == aux->ID){

										 Cancion_Actual = aux;
										 bmpc2 = (HBITMAP)LoadImage(NULL, Cancion_Actual->Pic, IMAGE_BITMAP, 130, 130, LR_LOADFROMFILE);
										 SendDlgItemMessage(hwndw, IDC_Pic_cancion, STM_SETIMAGE, IMAGE_BITMAP, (LPARAM)bmpc2);
										 //Lisbox_Canciones(HCanN, HCanNo, HCanA, HCanG, HCanD);
										 return 0;
										 break;
									 }
									 aux = aux->sig;
								 }
							 }
							 //

		}return true;
		case IDC_LA:{
						 cursel1 = SendMessage(HCanA, LB_GETCURSEL, 0, 0);
						SendMessage(HCanNo, LB_SETCURSEL, cursel1, 0);
						SendMessage(HCanN, LB_SETCURSEL, cursel1, 0);
						SendMessage(HCanG, LB_SETCURSEL, cursel1, 0);
						SendMessage(HCanD, LB_SETCURSEL, cursel1, 0);

						//
						char tempo2[3];
						strcpy(tempo2, "");
						SendMessage(HCanN, LB_GETTEXT, cursel1, (LPARAM)tempo2);
						int IDb = atoi(tempo2);
						if (Inicio != NULL){

							Canciones *aux = Lista_Actual->Lista;
							while (aux/*->sig*/ != NULL){
								if (IDb == aux->ID){

									Cancion_Actual = aux;
									bmpc2 = (HBITMAP)LoadImage(NULL, Cancion_Actual->Pic, IMAGE_BITMAP, 130, 130, LR_LOADFROMFILE);
									SendDlgItemMessage(hwndw, IDC_Pic_cancion, STM_SETIMAGE, IMAGE_BITMAP, (LPARAM)bmpc2);
									//Lisbox_Canciones(HCanN, HCanNo, HCanA, HCanG, HCanD);
									return 0;
									break;
								}
								aux = aux->sig;
							}
						}
						//

		}return true;
		case IDC_LG:{
						  cursel1 = SendMessage(HCanG, LB_GETCURSEL, 0, 0);
						 SendMessage(HCanN, LB_SETCURSEL, cursel1, 0);
						 SendMessage(HCanA, LB_SETCURSEL, cursel1, 0);
						 SendMessage(HCanNo, LB_SETCURSEL, cursel1, 0);
						 SendMessage(HCanD, LB_SETCURSEL, cursel1, 0);

						 //
						 char tempo2[3];
						 strcpy(tempo2, "");
						 SendMessage(HCanN, LB_GETTEXT, cursel1, (LPARAM)tempo2);
						 int IDb = atoi(tempo2);
						 if (Inicio != NULL){

							 Canciones *aux = Lista_Actual->Lista;
							 while (aux/*->sig*/ != NULL){
								 if (IDb == aux->ID){

									 Cancion_Actual = aux;
									 bmpc2 = (HBITMAP)LoadImage(NULL, Cancion_Actual->Pic, IMAGE_BITMAP, 130, 130, LR_LOADFROMFILE);
									 SendDlgItemMessage(hwndw, IDC_Pic_cancion, STM_SETIMAGE, IMAGE_BITMAP, (LPARAM)bmpc2);
									 //Lisbox_Canciones(HCanN, HCanNo, HCanA, HCanG, HCanD);
									 return 0;
									 break;
								 }
								 aux = aux->sig;
							 }
						 }
						 //
		}return true;
		case IDC_LD:{
						 cursel1 = SendMessage(HCanD, LB_GETCURSEL, 0, 0);
						SendMessage(HCanN, LB_SETCURSEL, cursel1, 0);
						SendMessage(HCanA, LB_SETCURSEL, cursel1, 0);
						SendMessage(HCanG, LB_SETCURSEL, cursel1, 0);
						SendMessage(HCanNo, LB_SETCURSEL, cursel1, 0);

						//
						char tempo2[3];
						strcpy(tempo2, "");
						SendMessage(HCanN, LB_GETTEXT, cursel1, (LPARAM)tempo2);
						int IDb = atoi(tempo2);
						if (Inicio != NULL){

							Canciones *aux = Lista_Actual->Lista;
							while (aux/*->sig*/ != NULL){
								if (IDb == aux->ID){

									Cancion_Actual = aux;
									bmpc2 = (HBITMAP)LoadImage(NULL, Cancion_Actual->Pic, IMAGE_BITMAP, 130, 130, LR_LOADFROMFILE);
									SendDlgItemMessage(hwndw, IDC_Pic_cancion, STM_SETIMAGE, IMAGE_BITMAP, (LPARAM)bmpc2);
									//Lisbox_Canciones(HCanN, HCanNo, HCanA, HCanG, HCanD);
									return 0;
									break;
								}
								aux = aux->sig;
							}
						}
						//
		}return true;

			//LISTBOXS//


			//  case IDM_FILE_NEW:
		case IDM_FILE_Mod:{
							  if (Lista_Actual!=Inicio)
							  {
								DialogBox(HRep, MAKEINTRESOURCE(IDD_Listas), hwndw, MLista);
							  }
			
		}return 1;
		case IDM_FILE_El:{
							 if (Lista_Actual == Inicio)
							 {
								 MessageBox(hwndw, "No se puede eliminar esta Lista de Reproduccion", "Denegado", MB_OK | MB_ICONWARNING);
							 }
							 else if (Lista_Actual != NULL)
							 {
								 char temp[200];
								 strcpy_s(temp, "¿De verdad desea eliminar  la lista de reproduccion: ");
								 strcat(temp, Lista_Actual->Nombre);
								 strcat(temp, " ?");
								 int quest = MessageBox(hwndw, temp, "Eliminar lista", MB_YESNO | MB_ICONQUESTION);
								 if (quest==6)
								 {
									 if (Lista_Actual->sig==NULL)
									 {
										 Lista_Actual->ID = 333;
										 Lista_Actual=Lista_Actual->ant;

										 Lista_Actual->sig = NULL;

									 }
									 else
									 {
										 Lista_Actual->sig->ant = Lista_Actual->ant;
										 Lista_Actual->ant->sig = Lista_Actual->sig;
									 }
									 Lista_Actual = Inicio;
									 Lisbox_Listas(hwndw, HListN, HListNo);
									 Lisbox_Canciones(HCanN, HCanNo, HCanA, HCanG, HCanD);
									 C_Listas--;
								 }
								 
								 
							 }
							 cursel1 = -1;
			
		}return 1;
		case IDM_FILE_ModC:{
							   if (Cancion_Actual != NULL)
							   {
									DialogBox(HRep, MAKEINTRESOURCE(IDD_Canciones), hwndw, MCancion);
							   }
							   
		}return 1;
		case IDM_reproduce:{
		////				
							   playing = false;
							   if (playing == false)
							   {
								   mciSendString("close mp3", NULL, 0, 0);
								   Cancion_playing = Cancion_Actual;
								   play();
								   playing = true;
							   }

							   else
							   {
								   mciSendString("resume mp3", NULL, 0, 0);
							   }

							   if (playing)
							   {
								   ShowWindow(BTPlay, SW_HIDE);
								   ShowWindow(BTPause, SW_SHOW);
							   }
		/////
		}return 1;
		case IDM_reproduceALL:{
							   ////				
							   playing = false;
							   if (playing == false)
							   {
								   mciSendString("close mp3", NULL, 0, 0);
								   Cancion_playing = Lista_Actual->Lista;
								   play();
								   playing = true;
							   }

							   else
							   {
								   mciSendString("resume mp3", NULL, 0, 0);
							   }

							   if (playing)
							   {
								   ShowWindow(BTPlay, SW_HIDE);
								   ShowWindow(BTPause, SW_SHOW);
							   }
							   /////
		}return 1;
		case IDM_FILE_ElC:{
							  bool bCan = true;
							  char temp[100];
							  if (Lista_Actual == Inicio)
							  {
								  Listas *aux = Inicio->sig;
								  while (aux != NULL)
								  {
									  Canciones *auxc = aux->Lista;
									  while (auxc != NULL)
									  {
										  if (Cancion_Actual->ID == auxc->ID)
										  {
											  strcpy_s(temp, "imposible borrar la cancion: ");
											  strcat(temp, Cancion_Actual->Nombre);
											  strcat(temp, " porque pertenece a la lista: ");
											  strcat(temp, aux->Nombre);
											  MessageBox(hwndw, temp, "No se puede realizar la operacion", MB_OK | MB_ICONEXCLAMATION);
											   bCan = false;
											   break;
											  break;
											  return 1;
											 
										  }
										  
											  auxc = auxc->sig;
										   
									  }
									  aux = aux->sig;
								  }
								  if (bCan==true)
								  {
									  Eliminar_cancion();
								  }
							  }
							  else
							  {
								Eliminar_cancion();
							  }
							  //
							  Lisbox_Canciones(HCanN, HCanNo, HCanA, HCanG, HCanD);
							  Cancion_Actual = NULL;
							  cursel1 = -1;
			
		}return 1;
		}

		switch (wParam)
		{
			case ID_NUEVO_LIST:{
				DialogBox(HRep, MAKEINTRESOURCE(IDD_Listas), hwndw, Lista);
			}return 1;

			case ID_NUEVO_CANCION:{
				DialogBox(HRep, MAKEINTRESOURCE(IDD_Canciones), hwndw, Cancion);
			}return 1;
			case IDC_PLAY:{
							
							  
							  if (playing==false)
							  {
								  mciSendString("close mp3", NULL, 0, 0);
								  Cancion_playing = Cancion_Actual;
								  play();
								  playing = true;
							  }
							   
							 else
							  {
								  mciSendString("resume mp3", NULL, 0, 0);
							  }
							  
							  if (playing)
							  {
								  ShowWindow(BTPlay, SW_HIDE);
								  ShowWindow(BTPause, SW_SHOW);
							  }

							  if (Cancion_playing!=NULL && playing ==true)
							  {
								  play();
								  playing = true;
							  }
							 // play();

			}return 1;
			case IDC_PAUSE:{
							   /*if (playing)
							   {
							   }*/
							   ShowWindow(BTPause, SW_HIDE);							   
							   ShowWindow(BTPlay, SW_SHOW);
							  
							   mciSendString("pause mp3", NULL, 0, 0);
							   playing = true;
			}return 1;
			case IDC_STOP:{
							  Cancion_playing = Cancion_Actual;
							
							  mciSendString("close mp3", NULL, 0, 0);
							  ShowWindow(BTPause, SW_HIDE);
							  ShowWindow(BTPlay, SW_SHOW);
							  playing = true;
			} return 1;
			case IDC_NEXT:{
							  if (Cancion_playing!=NULL)
							  {
								  if (Cancion_playing->sig != NULL){
									  mciSendString("close mp3", NULL, 0, 0);

									  Cancion_playing = Cancion_playing->sig;
									  Cancion_Actual = Cancion_playing;
									  play();
								  }
							  }
							 

			}return 1;
			case IDC_PREV:{
							  if (Cancion_playing != NULL)
							  {
								  if (Cancion_playing->ant != NULL){
									  mciSendString("close mp3", NULL, 0, 0);
									 
									  Cancion_playing = Cancion_playing->ant;
									  Cancion_Actual = Cancion_playing;
									  play();
								  }
							  }

			}return 1;
			case IDC_BUTTON15:{
							 char bus[30];
							 GetWindowText(GetDlgItem(actualizar, IDC_EDIT1), bus, sizeof(bus));
							 int Buss = SendMessage(HListNo, LB_FINDSTRINGEXACT, 0, (LPARAM)bus);
							 SendMessage(HListN, LB_SETCURSEL, Buss, 0);
							 SendMessage(HListNo, LB_SETCURSEL, Buss, 0);
							
			}return 1;
			case IDC_BUTTON16:{
								  SendMessage(HCanN, LB_RESETCONTENT, 0, 0);
								  SendMessage(HCanNo, LB_RESETCONTENT, 0, 0);
								  SendMessage(HCanA, LB_RESETCONTENT, 0, 0);
								  SendMessage(HCanG, LB_RESETCONTENT, 0, 0);
								  SendMessage(HCanD, LB_RESETCONTENT, 0, 0);
								  char bus[30];
								  strcpy(bus, "");
								 // SetWindowText(GetDlgItem(actualizar, IDC_EDIT2), bus);
								  GetWindowText(GetDlgItem(actualizar, IDC_EDIT2), bus, sizeof(bus));
								  Canciones *xu = Lista_Actual->Lista;
								  while (xu!=NULL)
								  {
									 
									  char N_c[3];

									  if (strcmp(bus,xu->Artista)==0)
									 /* if (*(xu->Artista)==*(bus))*/
									  {
										  strcpy(N_c, "");
										  _itoa(xu->ID, N_c,10);
										  SendMessage(HCanN, LB_ADDSTRING, 0, (LPARAM)N_c);
										  SendMessage(HCanNo, LB_ADDSTRING, 0, (LPARAM)xu->Nombre);
										  SendMessage(HCanA, LB_ADDSTRING, 0, (LPARAM)xu->Artista);
										  SendMessage(HCanG, LB_ADDSTRING, 0, (LPARAM)xu->Genero);
										  SendMessage(HCanD, LB_ADDSTRING, 0, (LPARAM)xu->Año);
									  }
									  
									  xu = xu->sig;
								  }
			}return 1;
			case IDC_BG: {
					char genero[20];
					strcpy(genero, "");
					GetWindowText(HG, genero, sizeof(genero));
					SendMessage(HCanN, LB_RESETCONTENT, 0, 0);
					SendMessage(HCanNo, LB_RESETCONTENT, 0, 0);
					SendMessage(HCanA, LB_RESETCONTENT, 0, 0);
					SendMessage(HCanG, LB_RESETCONTENT, 0, 0);
					SendMessage(HCanD, LB_RESETCONTENT, 0, 0);

					Canciones *xu = Lista_Actual->Lista;
					while (xu != NULL)
					{

						char N_c[3];

						if (strcmp(genero, xu->Genero) == 0)
							/* if (*(xu->Artista)==*(bus))*/
						{
							strcpy(N_c, "");
							_itoa(xu->ID, N_c, 10);
							SendMessage(HCanN, LB_ADDSTRING, 0, (LPARAM)N_c);
							SendMessage(HCanNo, LB_ADDSTRING, 0, (LPARAM)xu->Nombre);
							SendMessage(HCanA, LB_ADDSTRING, 0, (LPARAM)xu->Artista);
							SendMessage(HCanG, LB_ADDSTRING, 0, (LPARAM)xu->Genero);
							SendMessage(HCanD, LB_ADDSTRING, 0, (LPARAM)xu->Año);
						}

						xu = xu->sig;
					}
					
				}return 1;


		}
	}return 1;
	//
	
	case WM_CONTEXTMENU:{
							
							cursel = SendMessage(HListN, LB_GETCURSEL, 0, 0);
							cursel1 = SendMessage(HCanN, LB_GETCURSEL, 0, 0);
							if (cursel!=-1)
							{
								if ((HWND)wParam == HListNo || (HWND)wParam == HListN) {
									//  m_hMenu = CreatePopupMenu();
									//InsertMenu(m_hMenu, 0, MF_BYCOMMAND | MF_STRING | MF_ENABLED, 1, "Hello");
									//TrackPopupMenu(m_hMenu, TPM_TOPALIGN | TPM_LEFTALIGN, GET_X_LPARAM(lParam), GET_Y_LPARAM(lParam), 0, m_hWnd, NULL); 
									POINT point2;
									////////
									GetCursorPos(&point);
									point.x -= 274;
									point.y -= 230;

									/*point2.x = LOWORD(lParam);
									point2.y = HIWORD(lParam);
									*/
									hMenu = CreatePopupMenu();
									ClientToScreen(HListNo, &point);
									SetCursorPos(point.x, point.y);
									AppendMenuW(hMenu, MF_STRING, IDM_reproduceALL, L"&Reproducir");
									AppendMenuW(hMenu, MF_SEPARATOR, 0, NULL);
									AppendMenuW(hMenu, MF_STRING, IDM_FILE_Mod, L"&Modificar");
									AppendMenuW(hMenu, MF_STRING, IDM_FILE_El, L"&Eliminar");
									AppendMenuW(hMenu, MF_SEPARATOR, 0, NULL);

									TrackPopupMenu(hMenu, TPM_LEFTBUTTON, point.x, point.y, 0, hwndw, NULL);
									DestroyMenu(hMenu);
									break;
								}
							}

							if (cursel1 != -1)
							{

							

								if ((HWND)wParam == HCanN || (HWND)wParam == HCanNo || (HWND)wParam == HCanA || (HWND)wParam == HCanG || (HWND)wParam == HCanD) {

									GetCursorPos(&point);
									point.x -= 274;
									point.y -= 230;
									//
									//point.x = LOWORD(lParam);
									//point.y = HIWORD(lParam);
									//
									hMenu = CreatePopupMenu();
									ClientToScreen(HListNo, &point);
									SetCursorPos(point.x, point.y);
									AppendMenuW(hMenu, MF_STRING, IDM_reproduce, L"&Reproducir");
									AppendMenuW(hMenu, MF_SEPARATOR, 0, NULL);
									AppendMenuW(hMenu, MF_STRING, IDM_FILE_ModC, L"&Modificar");
									AppendMenuW(hMenu, MF_STRING, IDM_FILE_ElC, L"&Eliminar");
									
									



									TrackPopupMenu(hMenu, TPM_LEFTBUTTON, point.x, point.y, 0, hwndw, NULL);
									DestroyMenu(hMenu);
									break;
								}
							}
	}return 1;

	//
	case MM_MCINOTIFY:{
						  switch (wParam){
						  case MCI_NOTIFY_SUCCESSFUL:
						  {
														if (Cancion_playing != NULL)
														{
															if (Cancion_playing->sig != NULL){
																mciSendString("close mp3", NULL, 0, 0);

																Cancion_playing = Cancion_playing->sig;
																Cancion_Actual = Cancion_playing;
																play();
															}
														}
						  }return 1;
						  }
	
	}return 1;
	case WM_CLOSE:
	{
					 save_data();
		DestroyWindow(hwndw);
		PostQuitMessage(0);
	}return 1;


	}return 0;
}
void AbrirArchivo(char *archi, int filtro)
{
	strcpy(archi, "");
	OPENFILENAME ofn;       // common dialog box structure

	char szFile[260];       // buffer for file name
	HANDLE hf;              // file handle

	// Initialize OPENFILENAME
	ZeroMemory(&ofn, sizeof(ofn));
	ofn.lStructSize = sizeof(ofn);


	ofn.hwndOwner = HwRep;
	ofn.lpstrFile = szFile;
	// Set lpstrFile[0] to '\0' so that GetOpenFileName does not 
	// use the contents of szFile to initialize itself.
	ofn.lpstrFile[0] = '\0';
	ofn.nMaxFile = sizeof(szFile);
	ofn.lpstrFilter = "ALL\0*.*\0Bitmaps\0*.bmp\0Sound Files\0*.mp3\0Wav Files\0*.wav\0Jpg Images\0*.jpg\0";
	ofn.nFilterIndex = filtro;
	ofn.lpstrFileTitle = NULL;
	ofn.nMaxFileTitle = 0;
	ofn.lpstrInitialDir = NULL;
	ofn.Flags = OFN_PATHMUSTEXIST | OFN_FILEMUSTEXIST | OFN_NOCHANGEDIR;
	if (GetOpenFileName(&ofn) == TRUE)
		strcpy(archi, ofn.lpstrFile);
	else
		MessageBox(0, "No eligio archivo", "Aviso - Proyecto", MB_OK | MB_ICONINFORMATION);

};
bool Validar_Listas(Listas *Validar){
	if (Validar->ID != 0){
		if (strcmp(Validar->Nombre, "") != 0){
			if (strcmp(Validar->Pic, "") != 0){
				if (GetCountSongs(Validar)>=3)
				{
					return 1;
				}
				else
				{
					MessageBox(0, "debe de contener almenos 3 canciones", "Aviso Faltan datos", MB_ICONEXCLAMATION + MB_OK);
				}
				

			}
			else {
				MessageBox(0, "Foto", "Aviso Faltan datos", MB_ICONEXCLAMATION + MB_OK);
			}
		}





		else {
			MessageBox(0, "Nombre", "Aviso Faltan datos", MB_ICONEXCLAMATION + MB_OK);
		}
	}
	else {
		MessageBox(0, "Numero", "Aviso Faltan datos", MB_ICONEXCLAMATION + MB_OK);
	}
	return 0;

};
BOOL CALLBACK Lista(HWND hwndL, UINT msg, WPARAM wParam, LPARAM lParam)
{
	static HWND HListN, HListNo;
	HListN = GetDlgItem(actualizar, IDC_LISTAS);
	HListNo = GetDlgItem(actualizar, IDC_LISTAS2);

	Lisbox_Listas(actualizar, HListN, HListNo);

	char ID_c[3];  int IDc = 0;
	Listas *nueva = new Listas;
	nueva->ant = NULL;
	nueva->sig = NULL;
	nueva->ID = 0;
	strcpy(nueva->Nombre, "");
	strcpy(nueva->Pic, "");
	nueva->Lista = NULL;
	IDc = ID+1;
	static HWND HN, HNo, HP,HPP;
	/*static HWND HListN, HListNo;*/
	static HWND HCanN, HCanNo, HCanA, HCanG, HCanD;
	static HWND HCanNL, HCanNoL, HCanAL, HCanGL, HCanDL;
	int cursel1 = -1; 
	int cursel = -1;
	char numCancion[3] = "";
	char numCancionL[3] = "";
	switch (msg)
	{
	case WM_INITDIALOG: {
							
							
		Lista_Actual = Inicio;
		HN = GetDlgItem(hwndL, IDC_Numero);
		HNo = GetDlgItem(hwndL, IDC_Nombre);
		HP = GetDlgItem(hwndL, IDC_COVER);
		HPP = GetDlgItem(hwndL, IDC_Dir);
		
		_itoa(IDc, ID_c, 10);
		SetDlgItemText(hwndL, IDC_Numero, ID_c);
		SetDlgItemText(hwndL, IDC_Nombre, "");
		SetDlgItemText(hwndL, IDC_Dir, "");

		

		HCanN = GetDlgItem(hwndL, IDC_LNT);
		HCanNo = GetDlgItem(hwndL, IDC_LNoT);
		HCanA = GetDlgItem(hwndL, IDC_LAT);
		HCanG = GetDlgItem(hwndL, IDC_LGT);
		HCanD = GetDlgItem(hwndL, IDC_LDT);

		HCanNL = GetDlgItem(hwndL, IDC_LNL);
		HCanNoL = GetDlgItem(hwndL, IDC_LNoL);
		HCanAL = GetDlgItem(hwndL, IDC_LAL);
		HCanGL = GetDlgItem(hwndL, IDC_LGL);
		HCanDL = GetDlgItem(hwndL, IDC_LDL);
		
		Lisbox_Canciones( HCanN, HCanNo, HCanA, HCanG, HCanD);
		



	}return 1;

	case WM_COMMAND:
	{
					   switch (LOWORD(wParam))
					   {
					//
					   case IDC_LNT:{
									   cursel1 = SendMessage(HCanN, LB_GETCURSEL, 0, 0);
									   SendMessage(HCanNo, LB_SETCURSEL, cursel1, 0);
									   SendMessage(HCanA, LB_SETCURSEL, cursel1, 0);
									   SendMessage(HCanG, LB_SETCURSEL, cursel1, 0);
									   SendMessage(HCanD, LB_SETCURSEL, cursel1, 0);
									   strcpy(numCancion, "");
									   SendMessage(HCanN, LB_GETTEXT, cursel1, (LPARAM)numCancion);
									   
					   }return true;
					   case IDC_LNoT:{
										cursel1 = SendMessage(HCanNo, LB_GETCURSEL, 0, 0);
										SendMessage(HCanN, LB_SETCURSEL, cursel1, 0);
										SendMessage(HCanA, LB_SETCURSEL, cursel1, 0);
										SendMessage(HCanG, LB_SETCURSEL, cursel1, 0);
										SendMessage(HCanD, LB_SETCURSEL, cursel1, 0);
										strcpy(numCancion, "");
										SendMessage(HCanN, LB_GETTEXT, cursel1, (LPARAM)numCancion);
					   }return true;
					   case IDC_LAT:{
									   cursel1 = SendMessage(HCanA, LB_GETCURSEL, 0, 0);
									   SendMessage(HCanNo, LB_SETCURSEL, cursel1, 0);
									   SendMessage(HCanN, LB_SETCURSEL, cursel1, 0);
									   SendMessage(HCanG, LB_SETCURSEL, cursel1, 0);
									   SendMessage(HCanD, LB_SETCURSEL, cursel1, 0);
									   strcpy(numCancion, "");
									   SendMessage(HCanN, LB_GETTEXT, cursel1, (LPARAM)numCancion);

					   }return true;
					   case IDC_LGT:{
									   cursel1 = SendMessage(HCanG, LB_GETCURSEL, 0, 0);
									   SendMessage(HCanN, LB_SETCURSEL, cursel1, 0);
									   SendMessage(HCanA, LB_SETCURSEL, cursel1, 0);
									   SendMessage(HCanNo, LB_SETCURSEL, cursel1, 0);
									   SendMessage(HCanD, LB_SETCURSEL, cursel1, 0);
									   strcpy(numCancion, "");
									   SendMessage(HCanN, LB_GETTEXT, cursel1, (LPARAM)numCancion);
					   }return true;
					   case IDC_LDT:{
									   cursel1 = SendMessage(HCanD, LB_GETCURSEL, 0, 0);
									   SendMessage(HCanN, LB_SETCURSEL, cursel1, 0);
									   SendMessage(HCanA, LB_SETCURSEL, cursel1, 0);
									   SendMessage(HCanG, LB_SETCURSEL, cursel1, 0);
									   SendMessage(HCanNo, LB_SETCURSEL, cursel1, 0);
									   strcpy(numCancion, "");
									   SendMessage(HCanN, LB_GETTEXT, cursel1, (LPARAM)numCancion);
					   }return true;

					//
					   case IDC_LNL:{
										cursel = SendMessage(HCanNL, LB_GETCURSEL, 0, 0);
										SendMessage(HCanNoL, LB_SETCURSEL, cursel, 0);
										SendMessage(HCanAL, LB_SETCURSEL, cursel, 0);
										SendMessage(HCanGL, LB_SETCURSEL, cursel, 0);
										SendMessage(HCanDL, LB_SETCURSEL, cursel, 0);
										strcpy(numCancionL, "");
										SendMessage(HCanNL, LB_GETTEXT, cursel, (LPARAM)numCancionL);

					   }return true;
					   case IDC_LNoL:{
										 cursel = SendMessage(HCanNoL, LB_GETCURSEL, 0, 0);
										 SendMessage(HCanNL, LB_SETCURSEL, cursel, 0);
										 SendMessage(HCanAL, LB_SETCURSEL, cursel, 0);
										 SendMessage(HCanGL, LB_SETCURSEL, cursel, 0);
										 SendMessage(HCanDL, LB_SETCURSEL, cursel, 0);
										 strcpy(numCancionL, "");
										 SendMessage(HCanNL, LB_GETTEXT, cursel, (LPARAM)numCancionL);
					   }return true;
					   case IDC_LAL:{
										cursel = SendMessage(HCanAL, LB_GETCURSEL, 0, 0);
										SendMessage(HCanNoL, LB_SETCURSEL, cursel, 0);
										SendMessage(HCanNL, LB_SETCURSEL, cursel, 0);
										SendMessage(HCanGL, LB_SETCURSEL, cursel, 0);
										SendMessage(HCanDL, LB_SETCURSEL, cursel, 0);
										strcpy(numCancionL, "");
										SendMessage(HCanNL, LB_GETTEXT, cursel, (LPARAM)numCancionL);

					   }return true;
					   case IDC_LGL:{
										cursel = SendMessage(HCanGL, LB_GETCURSEL, 0, 0);
										SendMessage(HCanNL, LB_SETCURSEL, cursel, 0);
										SendMessage(HCanAL, LB_SETCURSEL, cursel, 0);
										SendMessage(HCanNoL, LB_SETCURSEL, cursel, 0);
										SendMessage(HCanDL, LB_SETCURSEL, cursel, 0);
										strcpy(numCancionL, "");
										SendMessage(HCanNL, LB_GETTEXT, cursel, (LPARAM)numCancionL);
					   }return true;
					   case IDC_LDL:{
										cursel = SendMessage(HCanDL, LB_GETCURSEL, 0, 0);
										SendMessage(HCanNL, LB_SETCURSEL, cursel, 0);
										SendMessage(HCanAL, LB_SETCURSEL, cursel, 0);
										SendMessage(HCanGL, LB_SETCURSEL, cursel, 0);
										SendMessage(HCanNoL, LB_SETCURSEL, cursel, 0);
										strcpy(numCancionL, "");
										SendMessage(HCanNL, LB_GETTEXT, cursel, (LPARAM)numCancionL);
					   }return true;
					//
					   case ID_Buscar:{
										  char Imagen[MAX_PATH];
										  strcpy(Imagen, "");


										  AbrirArchivo(Imagen, 2);
										  strcpy(nueva->Pic, Imagen);
										  SetDlgItemText(hwndL, IDC_Dir, nueva->Pic);
										  static HBITMAP bmp1, bmp2;

										  bmp1 = (HBITMAP)SendDlgItemMessage(hwndL, IDC_COVER, STM_GETIMAGE, IMAGE_BITMAP, 0);
										  bmp2 = (HBITMAP)LoadImage(NULL, Imagen, IMAGE_BITMAP, 180, 180, LR_LOADFROMFILE);
										  SendDlgItemMessage(hwndL, IDC_COVER, STM_SETIMAGE, IMAGE_BITMAP, (LPARAM)bmp2);


					   }return 1;
					   case IDC_OK:{

									   GetWindowText(HN, ID_c, sizeof(ID_c));
									   nueva->ID = atoi(ID_c);
									   GetWindowText(HNo, nueva->Nombre, sizeof(nueva->Nombre));
									   GetWindowText(HPP, nueva->Pic, sizeof(nueva->Pic));

									   //
									   int hh = SendMessage(HCanNL, LB_GETCOUNT, 0, 0);
									   if (Inicio!=NULL)
									   {
										   
										   if (Inicio->Lista!=NULL)
										   {	 
											   Canciones *help = Inicio->Lista;
											   char IDs[5]; int IDto = -22;
											   for (int i = 0; i < hh; i++)
											   {
												   strcpy_s(IDs, "");
												   SendMessage(HCanNL, LB_GETTEXT, i, (LPARAM)IDs);
												   IDto=atoi(IDs);
												  
												   while (help != NULL)
												   {
													   if (IDto==help->ID)
													   {
														   Canciones *nuevaC = new Canciones;
														   nuevaC->ant = NULL;
														   nuevaC->sig = NULL;
														   nuevaC->ID = help->ID;
														   strcpy(nuevaC->Año, help->Año);
														   strcpy(nuevaC->Artista, help->Artista);
														   strcpy(nuevaC->Genero, help->Genero);
														   strcpy(nuevaC->Nombre, help->Nombre);
														   strcpy(nuevaC->Pic, help->Pic);
														   strcpy(nuevaC->Song, help->Song);

														   if (nueva->Lista == NULL){
															   nueva->Lista = nuevaC;
															   nueva->Lista->sig = NULL;
															   nueva->Lista->ant = NULL;
														   }
														   else{
															   Canciones *Aux = nueva->Lista;
															   while (Aux->sig != NULL)
															   {
																   Aux = Aux->sig;
															   }
															   Aux->sig = nuevaC;
															   Aux->sig->ant = Aux;
														   }
													   }
													   help = help->sig;
												   }
												   help = Inicio->Lista;
											   }
											   /*Canciones *help = Inicio->Lista;
											   while (help!=NULL)
											   {

												   help = help->sig;
											   }*/
										   }
									   }

									   //

									   if (Validar_Listas(nueva))
									   {

									   if (Inicio == NULL){
										   Inicio = nueva;
										   Inicio->sig = NULL;
										   Inicio->ant = NULL;
									   }
									   else{
										   Listas *Aux = Inicio;
										   while (Aux->sig != NULL)
										   {
											   Aux = Aux->sig;
										   }
										   Aux->sig = nueva;
										   Aux->sig->ant = Aux;
									   }
									   MessageBox(hwndL, "Lista agregada correctamente", "Agregado exitoso!", MB_OK);

									   ID = atoi(ID_c);
									   //ID++;
									   C_Listas++;
									  /* save_data();
									   load_data();*/

									   EndDialog(hwndL, 0);
					   }
					   }return 1;
					   case IDC_mas:{////////boton mas
											 strcpy(numCancion, "");
											 cursel1 = SendMessage(HCanN, LB_GETCURSEL, 0, 0);
											 SendMessage(HCanN, LB_GETTEXT, cursel1, (LPARAM)numCancion);
											 if (strcmp(numCancion, "") != 0){

												 int numCancionint = atoi(numCancion);
												 if (Inicio != NULL){
													 if (Inicio->Lista != NULL)
													 {


														 Canciones *Aux = Inicio->Lista;
														 while (Aux != NULL)
														 {//AuxC es para recorrer la nueva lista, Aux es para recorrer la lista principal
															 if (numCancionint == Aux->ID){
																 char N_c[3];
																 strcpy(N_c, "");
																 _itoa(Aux->ID, N_c, 10);
																 SendMessage(HCanNL, LB_ADDSTRING, 0, (LPARAM)N_c);
																 SendMessage(HCanNoL, LB_ADDSTRING, 0, (LPARAM)Aux->Nombre);
																 SendMessage(HCanAL, LB_ADDSTRING, 0, (LPARAM)Aux->Artista);
																 SendMessage(HCanGL, LB_ADDSTRING, 0, (LPARAM)Aux->Genero);
																 SendMessage(HCanDL, LB_ADDSTRING, 0, (LPARAM)Aux->Año);

															 }
															 Aux = Aux->sig;
														 }
													 }
												 }


											 }
											 else{
												 MessageBox(hwndL, "Seleccione una cancion", "Advertencia!", MB_OK);
											 }

					   }return 1;
					   case IDC_menos:{
										  strcpy(numCancionL, "");
										  cursel = SendMessage(HCanNL, LB_GETCURSEL, 0, 0);
										  SendMessage(HCanNL, LB_GETTEXT, cursel, (LPARAM)numCancionL);
										  if (strcmp(numCancionL, "") != 0){

											 cursel = SendMessage(HCanNL, LB_GETCURSEL, 0, 0);
											 SendMessage(HCanNL, LB_DELETESTRING, cursel, 0);
											 SendMessage(HCanNoL, LB_DELETESTRING, cursel, 0);
											 SendMessage(HCanAL, LB_DELETESTRING, cursel, 0);
											 SendMessage(HCanGL, LB_DELETESTRING, cursel, 0);
											 SendMessage(HCanDL, LB_DELETESTRING, cursel, 0);
										  }
										  else{
											  MessageBox(hwndL, "Seleccione una cancion", "Advertencia!", MB_OK);
										  }
					   
					   }return 1;
					   
	
		}
	}return 1;


	case WM_CLOSE:
	{
		EndDialog(hwndL, 0);
		
	}return 1;


	}return 0;
}

bool Validar_Cancion(Canciones *Validar){
	if (Validar->ID != 0){
		if (strcmp(Validar->Nombre, "") != 0){
			if (strcmp(Validar->Artista, "") != 0){
				if (strcmp(Validar->Genero, "") != 0){
					if (strcmp(Validar->Año, "") != 0){
						if (strcmp(Validar-> Pic, "") != 0){
							if (strcmp(Validar-> Song, "") != 0){
								return 1;
							}
							else {
								MessageBox(0, "Cancion", "Aviso Faltan datos", MB_ICONEXCLAMATION + MB_OK);
							}
						}
						else {
							MessageBox(0, "Foto", "Aviso Faltan datos", MB_ICONEXCLAMATION + MB_OK);
						}
					}
					else {
						MessageBox(0, "Año", "Aviso Faltan datos", MB_ICONEXCLAMATION + MB_OK);
					}
				}
				else {
					MessageBox(0, "Genero", "Aviso Faltan datos", MB_ICONEXCLAMATION + MB_OK);
				}
			}
			else {
				MessageBox(0, "Artista", "Aviso Faltan datos", MB_ICONEXCLAMATION + MB_OK);
			}
		}
		else {
			MessageBox(0, "Nombre", "Aviso Faltan datos", MB_ICONEXCLAMATION + MB_OK);
		}
	}
	else {
		MessageBox(0, "Numero", "Aviso Faltan datos", MB_ICONEXCLAMATION + MB_OK);
	}
	return 0;

};

void Eliminar_cancion(){
	if (Lista_Actual != NULL)
	{
		if (Cancion_Actual != NULL)
		{
			if ((GetCountSongs(Lista_Actual) - 1) < 3)
			{
				char tem[100];
				strcpy(tem, "");
				strcpy(tem, "La lista: ");
				strcat(tem, Lista_Actual->Nombre);
				strcat(tem, " no debe quedar con menos de 3 canciones.");
				MessageBox(actualizar, tem, "Denegado!", MB_OK | MB_ICONEXCLAMATION);
			}
			else
			{
				char tem[100];
				strcpy(tem, "");
				strcpy(tem, "¿Seguro que desea eliminar la cancion: ");
				strcat(tem, Cancion_Actual->Nombre);
				strcat(tem, " ?");
				int quest = MessageBox(actualizar, tem, "¿Desea eliminar?", MB_YESNO | MB_ICONQUESTION);

				if (quest == 6)
				{
					Canciones *aux = Lista_Actual->Lista;
					Lista_Actual->Lista = NULL;
					while (aux != NULL)
					{
						if (aux != Cancion_Actual)
						{
							Canciones *nueva = new Canciones;
							nueva->ant = NULL;
							nueva->sig = NULL;
							strcpy_s(nueva->Año, aux->Año);
							strcpy_s(nueva->Artista, aux->Artista);
							strcpy_s(nueva->Genero, aux->Genero);
							strcpy_s(nueva->Nombre, aux->Nombre);
							strcpy_s(nueva->Pic, aux->Pic);
							strcpy_s(nueva->Song, aux->Song);
							nueva->ID = aux->ID;
							if (Lista_Actual->Lista == NULL)
							{
								Lista_Actual->Lista = nueva;
							}
							else
							{
								Canciones *Aux = Lista_Actual->Lista;
								while (Aux->sig != NULL)
								{
									Aux = Aux->sig;
								}
								Aux->sig = nueva;
								Aux->sig->ant = Aux;
							}


						}

						aux = aux->sig;
					}
				}

			}
		}
	}

}
BOOL CALLBACK Cancion(HWND hwndC, UINT msg, WPARAM wParam, LPARAM lParam)
{
	char ID_c[3];
	static HWND Hn, Hno, Ha, HG, HD, HP, HPP, HI, HIP;
	int IDc = 0;

	switch (msg)
	{
	case WM_INITDIALOG: {
							IDc = ID + 1;
		_itoa((IDc), ID_c, 10);

		Hn = GetDlgItem(hwndC, IDC_Numero);
		Hno = GetDlgItem(hwndC, IDC_Nombre);
		Ha = GetDlgItem(hwndC, IDC_Artista);
		HG = GetDlgItem(hwndC, IDC_Genero);
		HD = GetDlgItem(hwndC, IDC_date);
		HI = GetDlgItem(hwndC, IDC_Pic_Song);
		HPP = GetDlgItem(hwndC, IDC_Dir);
		HIP = GetDlgItem(hwndC, IDC_Pic_Path);


		SetDlgItemText(hwndC, IDC_Numero, "");
		SetDlgItemText(hwndC, IDC_Nombre, "");
		SetDlgItemText(hwndC, IDC_Artista, "");
		SetDlgItemText(hwndC, IDC_Genero, "");
		SetDlgItemText(hwndC, IDC_date, "");
		SetDlgItemText(hwndC, IDC_Dir, "");
		SetDlgItemText(hwndC, IDC_Pic_Path, "");

		SendMessage(HD, CB_RESETCONTENT, 0, 0);
		char año[5];
		for (int i = 1800; i < 2016; i++){
			strcpy(año, "");
			_itoa(i, año, 10);
			SendMessage(HD, CB_ADDSTRING, 0, (LPARAM)año);
		}

		SendMessage(HG, CB_RESETCONTENT, 0, 0);
		for (int i = 0; i < 80; i++)
		{
			SendMessage(HG, CB_ADDSTRING, 0, (LPARAM)gneros[i]);
		}
		
		
		SetWindowText(Hn, ID_c);

		
	}return 1;

		case WM_COMMAND:
		{
		switch (wParam)
		{

		case ID_Buscar:{
			char Song[MAX_PATH];
			strcpy(Song, "");
			AbrirArchivo(Song, 3);
			///recortar nombre
			//char NombreN[MAX_PATH];
			//strcpy(NombreN, (ExtNombre(Cancion, NombreN)));
			SetDlgItemText(hwndC, IDC_Dir, Song);

		
						}return 1;
		case IDC_Imagen:{
			char Pic[MAX_PATH];
			strcpy(Pic, "");
			AbrirArchivo(Pic, 2);
			SetDlgItemText(hwndC, IDC_Pic_Path, Pic);
			static HBITMAP bmp1, bmp2;

			bmp1 = (HBITMAP)SendDlgItemMessage(hwndC, IDC_Pic_Song, STM_GETIMAGE, IMAGE_BITMAP, 0);
			bmp2 = (HBITMAP)LoadImage(NULL, Pic, IMAGE_BITMAP, 140, 140, LR_LOADFROMFILE);
			SendDlgItemMessage(hwndC, IDC_Pic_Song, STM_SETIMAGE, IMAGE_BITMAP, (LPARAM)bmp2);

						}return 1;
		case IDC_OK:{
			
			_itoa((IDc), ID_c, 10);
			Canciones *nueva = new Canciones;
			nueva->ant = NULL;
			nueva->sig = NULL;
			nueva->ID = -1;
			strcpy(nueva->Nombre, "");
			strcpy(nueva->Artista, "");
			strcpy(nueva->Genero, "");
			strcpy(nueva->Año, "");
			strcpy(nueva->Song, "");
			strcpy(nueva->Pic, "");

			GetWindowText(Hn, ID_c, sizeof(ID_c));
			nueva->ID = atoi(ID_c);

			GetWindowText(Hno, nueva->Nombre, sizeof(nueva->Nombre));
			GetWindowText(Ha, nueva->Artista, sizeof(nueva->Artista));
			GetWindowText(HG, nueva->Genero, sizeof(nueva->Genero));
			GetWindowText(HD, nueva->Año, sizeof(nueva->Año));

			GetWindowText(HPP, nueva->Song, sizeof(nueva->Song));
			GetWindowText(HIP, nueva->Pic, sizeof(nueva->Pic));
			if (Validar_Cancion(nueva)){
				if (Inicio->Lista == NULL){
					Inicio->Lista = nueva;
					Inicio->Lista->sig = NULL;
					Inicio->Lista->ant = NULL;
				}
				else{
					Canciones *Aux = Inicio->Lista;
					while (Aux->sig != NULL)
					{
						Aux = Aux->sig;
					}
					Aux->sig = nueva;
					Aux->sig->ant = Aux;
				}
				ID = atoi(ID_c);
			
					//ID++;
					//C_Listas++;
					//save_data();
					static HWND HCanN,HCanNo, HCanA, HCanG, HCanD;
					HCanN = GetDlgItem(actualizar, IDC_LN);
					HCanNo = GetDlgItem(actualizar, IDC_LNo);
					HCanA = GetDlgItem(actualizar, IDC_LA);
					HCanG = GetDlgItem(actualizar, IDC_LG);
					HCanD = GetDlgItem(actualizar, IDC_LD);

					Lista_Actual = Inicio;
					Lisbox_Canciones(HCanN, HCanNo, HCanA, HCanG, HCanD);

					EndDialog(hwndC, 0);
			}
		
		}return 1;
		}
		}return 1;
		

	case WM_CLOSE:
	{
		EndDialog(hwndC, 0);

	}return 1;


	}return 0;
}

void load_databkp(){

	Inicio = new Listas;
	ifstream Database;
	Database.open("Base de datos.bin", ios::binary | ios::in);
		char ID_c[3], CL_c[3],CC_c[3];
		char ID_aux[3]="";
	if (Database.is_open()){
		strcpy(ID_c,"");
		Database.read(ID_c, sizeof(ID_c));
		ID = atoi(ID_c);//se lee y asigna el ultimo ID al programa
		strcpy(CL_c, "");
		Database.read(CL_c, sizeof(CL_c));
		C_Listas = atoi(CL_c);// se lee cuantas listas existen en el programa
			
			strcpy(ID_c, "");
			Database.read(ID_c, sizeof(ID_c));
			Inicio->ID = atoi(ID_c);
			strcpy(Inicio->Nombre, "");
			Database.read(Inicio->Nombre, sizeof(Inicio->Nombre));
			strcpy(Inicio->Pic, "");
			Database.read(Inicio->Pic, sizeof(Inicio->Pic));
			strcpy(CC_c, "");
			Database.read(CC_c, sizeof(CC_c));
			C_Canciones = atoi(CC_c);
			Inicio->Lista = NULL;
			strcpy(ID_c, "");
			char tempo[3];
			strcpy(tempo, "");
			for (int i=0; i < C_Canciones; i++){
				Canciones *nuevo = new Canciones;
				nuevo->ant = NULL;
				nuevo->ant = NULL;
				strcpy_s(nuevo->Nombre, "");
				strcpy_s(nuevo->Artista, "");
				strcpy_s(nuevo->Genero, "");
				strcpy_s(nuevo->Año, "");
				strcpy_s(nuevo->Pic, "");
				strcpy_s(nuevo->Song, "");
				strcpy_s(tempo, "");
				_itoa(nuevo->ID, tempo, 10);
				Database.read(tempo, sizeof(tempo));
				nuevo->ID = atoi(ID_c);
				strcpy(tempo, "");
				Database.read(nuevo->Nombre, sizeof(nuevo->Nombre));
				Database.read(nuevo->Artista, sizeof(nuevo->Artista));
				Database.read(nuevo->Genero, sizeof(nuevo->Genero));
				Database.read(nuevo->Año, sizeof(nuevo->Año));
				Database.read(nuevo->Pic, sizeof(nuevo->Pic));
				Database.read(nuevo->Song, sizeof(nuevo->Song));
				if (Inicio->Lista == NULL){
					Inicio->Lista = nuevo;
					Inicio->Lista->sig = NULL;
					Inicio->Lista->ant = NULL;
				}
				else{
					Canciones *Aux = Inicio->Lista;
					while (Aux->sig != NULL)
					{
						Aux = Aux->sig;
					}
					Aux->sig = nuevo;
					Aux->sig->ant = Aux;
				}


										

			}
			Inicio->ant = NULL;
			Inicio->sig = NULL;
			if (C_Listas > 0){
				
				Listas *Aux = Inicio;// se iniciializa un auxiliar para leer
				
				/*for (int i = 1; i < C_Listas; i++)
				{	
					Listas *nuevo = new Listas;
					strcpy(ID_c, "");
					Database.read(ID_c, sizeof(ID_c));
					nuevo->ID = atoi(ID_c);
					strcpy(ID_c, "");
					strcpy(nuevo->Nombre, "");
					Database.read(nuevo->Nombre, sizeof(nuevo->Nombre));
					strcpy(nuevo->Pic, "");
					Database.read(nuevo->Pic, sizeof(nuevo->Pic));
					Database.read(CC_c, sizeof(CC_c));
					nuevo->ant = Aux;
					nuevo->sig = NULL;
					Aux->sig = nuevo;
					Aux = Aux->sig;
				}*/
			}
		
	}
	else
	{
		MessageBox(0, "No se pudo cargar la base de datos", "Advertencia",MB_OK);
		Inicio->sig = NULL;
		Inicio->ant = NULL;
		Inicio->ID = 0;
		strcpy(Inicio->Nombre, "Todas Las Canciones");
		strcpy(Inicio->Pic, "ALL.bmp");
		Inicio->Lista = NULL;
		ID = 0;
		C_Listas = 1;
		C_Canciones = 0;
	}
	Database.close();
	//MessageBox(0, texto, texto1, 0);
	
};
void load_data(){
	char Int_C[3];
	ifstream Database;
	Database.open("Base de datos.bin", ios::binary | ios::in);
	if (Database.is_open()){
		strcpy(Int_C, "");
		Database.read(Int_C, sizeof(Int_C));
		ID = atoi(Int_C);//se lee y asigna el ultimo ID al programa
		strcpy(Int_C, "");
		Database.read(Int_C, sizeof(Int_C));
		C_Listas = atoi(Int_C);
		for (int i = 0; i < C_Listas; i++)
		{		
		Listas *nueval = new Listas;
		nueval->ant = NULL;
		nueval->sig = NULL;
		nueval->ID = -1;
		strcpy(nueval->Nombre, "");
		strcpy(nueval->Pic, "");
		nueval->Lista = NULL;
		strcpy(Int_C, "");
		Database.read(Int_C, sizeof(Int_C));
		nueval->ID = atoi(Int_C);
		Database.read(nueval->Nombre, sizeof(nueval->Nombre));
		Database.read(nueval->Pic, sizeof(nueval->Pic));
		strcpy(Int_C, "");
		Database.read(Int_C, sizeof(Int_C));
		C_Canciones = 0;
		C_Canciones = atoi(Int_C);
		///leer canciones
		for (int i = 0; i < C_Canciones; i++)
		{
			Canciones *nueva = new Canciones;
			nueva->ant = NULL;
			nueva->sig = NULL;
			nueva->ID = -1;
			strcpy(nueva->Nombre, "");
			strcpy(nueva->Artista, "");
			strcpy(nueva->Genero, "");
			strcpy(nueva->Año, "");
			strcpy(nueva->Song, "");
			strcpy(nueva->Pic, "");

			strcpy(Int_C, "");
			Database.read(Int_C, sizeof(Int_C));
			nueva->ID = atoi(Int_C);
			Database.read(nueva->Nombre, sizeof(nueva->Nombre));
			Database.read(nueva-> Artista, sizeof(nueva-> Artista));
			Database.read(nueva->Genero, sizeof(nueva->Genero));
			Database.read(nueva->Año, sizeof(nueva->Año));
			Database.read(nueva->Pic, sizeof(nueva->Pic));
			Database.read(nueva->Song, sizeof(nueva->Song));
			if (nueval->Lista == NULL){
				nueval->Lista = nueva;
				nueval->Lista->sig = NULL;
				nueval->Lista->ant = NULL;
			}
			else{
				Canciones *Aux = nueval->Lista;
				while (Aux->sig != NULL)
				{
					Aux = Aux->sig;
				}
				Aux->sig = nueva;
				Aux->sig->ant = Aux;
			}
			//termino de asignar las canciones
		}
		///
		if (Inicio == NULL){
			Inicio = nueval;
			Inicio->sig = NULL;
			Inicio->ant = NULL; 
			Cancion_Actual = Inicio->Lista;
		}
		else{
			Listas *Aux = Inicio;
			while (Aux->sig != NULL)
			{
				Aux = Aux->sig;
			}
			Aux->sig = nueval;
			Aux->sig->ant = Aux;
		}

		}
		//cerrar archivo
		Database.close();
	}
	else
	{
		MessageBox(0, "No se pudo cargar la base de datos", "Advertencia", MB_OK);
		Inicio = new Listas;
		Inicio->sig = NULL;
		Inicio->ant = NULL;
		Inicio->ID = 0;
		strcpy(Inicio->Nombre, "Todas Las Canciones");
		strcpy(Inicio->Pic, "C:\\Users\\OzielLeos\\Documents\\mover\\PF PROGRAMACION II Reproductor\\PF PROGRAMACION II 3a\\ALL.bmp");
		Inicio->Lista = NULL;
		ID = 0;
		C_Listas = 1;
		C_Canciones = 0;
		Cancion_Actual = Inicio->Lista;
	}

}


void save_data(){
	/*Listas *Inicio = new Listas;
	Inicio->ID = 10;
	strcpy_s(Inicio->Nombre, "Todas las canciones");
	*/
	ofstream Database;
	Database.open("Base de datos.bin", ios::trunc | ios::binary | ios::out);
	char tempo[3];
	char ID_c[3];
	char Li_c[3];
	char CC_c[3];
	if (Database.is_open()){
		strcpy(ID_c, "");
		_itoa(ID, ID_c, 10);
		Database.write(ID_c, sizeof(ID_c));//Ultimo id del programa
		strcpy(Li_c, "");
		_itoa(C_Listas, Li_c, 10);
		Database.write(Li_c, sizeof(Li_c));//Cantidad de listas
		Listas *aux = Inicio;
		if (aux != NULL)
		{
			/*if (aux->sig == NULL){
			strcpy_s(tempo, "");
			_itoa(aux->ID, tempo, 10);
			Database.write(tempo, sizeof(tempo));
			Database.write(aux->Nombre, sizeof(aux->Nombre));
			Database.write(aux->Pic, sizeof(aux->Pic));
			}
			else{*/

			while (aux != NULL){
				strcpy_s(tempo, "");
				_itoa(aux->ID, tempo, 10);
				Database.write(tempo, sizeof(tempo));
				Database.write(aux->Nombre, sizeof(aux->Nombre));
				Database.write(aux->Pic, sizeof(aux->Pic));
				C_Canciones = GetCountSongs(aux);
				_itoa(C_Canciones, CC_c, 10);
				Database.write(CC_c, sizeof(CC_c));
				for (int i = 0; i < C_Canciones; i++)
				{
					strcpy_s(tempo, "");
					_itoa(aux->Lista->ID, tempo, 10);
					Database.write(tempo, sizeof(tempo));
					Database.write(aux->Lista->Nombre, sizeof(aux->Lista->Nombre));
					Database.write(aux->Lista->Artista, sizeof(aux->Lista->Artista));
					Database.write(aux->Lista->Genero, sizeof(aux->Lista->Genero));
					Database.write(aux->Lista->Año, sizeof(aux->Lista->Año));
					Database.write(aux->Lista->Pic, sizeof(aux->Lista->Pic));
					Database.write(aux->Lista->Song, sizeof(aux->Lista->Song));
					aux->Lista = aux->Lista->sig;
				}
				aux = aux->sig;
			}//Database.write("FIN", sizeof(tempo));


			//	}


		}/*
		 strcpy_s(tempo, "");
		 _itoa(aux->ID,tempo , 10);
		 Database.write(tempo, sizeof(tempo));
		 Database.write(aux->Nombre, sizeof(aux->Nombre));
		 Database.write(aux->Pic, sizeof(aux->Pic));*/

	}

	Database.close();
};


BOOL CALLBACK MCancion(HWND hwndC, UINT msg, WPARAM wParam, LPARAM lParam)
{
	char ID_c[3];
	static HWND Hn, Hno, Ha, HG, HD, HP, HPP, HI, HIP;
	int IDc = 0;
	static HBITMAP bmp1, bmp2;
	switch (msg)
	{
	case WM_INITDIALOG: {
							IDc = ID + 1;
							_itoa(Cancion_Actual->ID, ID_c, 10);

							Hn = GetDlgItem(hwndC, IDC_Numero);
							Hno = GetDlgItem(hwndC, IDC_Nombre);
							Ha = GetDlgItem(hwndC, IDC_Artista);
							HG = GetDlgItem(hwndC, IDC_Genero);
							HD = GetDlgItem(hwndC, IDC_date);
							HI = GetDlgItem(hwndC, IDC_Pic_Song);
							HPP = GetDlgItem(hwndC, IDC_Dir);
							HIP = GetDlgItem(hwndC, IDC_Pic_Path);


							SetDlgItemText(hwndC, IDC_Numero, ID_c);
							SetDlgItemText(hwndC, IDC_Nombre, Cancion_Actual->Nombre);
							SetDlgItemText(hwndC, IDC_Artista, Cancion_Actual->Artista);
							SetDlgItemText(hwndC, IDC_Genero, Cancion_Actual->Genero);
							SetDlgItemText(hwndC, IDC_date, Cancion_Actual->Año);
							SetDlgItemText(hwndC, IDC_Dir, Cancion_Actual->Song);
							SetDlgItemText(hwndC, IDC_Pic_Path, Cancion_Actual->Pic);
							
							ShowWindow(GetDlgItem(hwndC, ID_Buscar) , SW_HIDE);
							SendMessage(Ha, EM_SETREADONLY, 1, 0);

							SendMessage(HD, CB_RESETCONTENT, 0, 0);
							int cursel = -1; int cursel1=-1;
							char año[5];
							for (int i = 1800; i < 2016; i++){
								strcpy(año, "");
								_itoa(i, año, 10);
								cursel = SendMessage(HD, CB_FINDSTRING, 0, (LPARAM)Cancion_Actual->Año);
								SendMessage(HD, CB_ADDSTRING, 0, (LPARAM)año);
								
							}
							
							SendMessage(HD, CB_SETCURSEL, cursel, 0);

							SendMessage(HG, CB_RESETCONTENT, 0, 0);
							for (int i = 0; i < 80; i++)
							{
								SendMessage(HG, CB_ADDSTRING, 0, (LPARAM)gneros[i]);
								cursel1 = SendMessage(HG, CB_FINDSTRINGEXACT, 0, (LPARAM)Cancion_Actual->Genero);
							}
							SendMessage(HG, CB_SETCURSEL, cursel1, 0);
							EnableWindow(HG, false);
							//SendMessage(HG, WS_DISABLED, 1, 0);
							SetWindowText(Hn, ID_c);
							bmp1 = (HBITMAP)SendDlgItemMessage(hwndC, IDC_Pic_Song, STM_GETIMAGE, IMAGE_BITMAP, 0);
							bmp2 = (HBITMAP)LoadImage(NULL, Cancion_Actual->Pic, IMAGE_BITMAP, 140, 140, LR_LOADFROMFILE);
							SendDlgItemMessage(hwndC, IDC_Pic_Song, STM_SETIMAGE, IMAGE_BITMAP, (LPARAM)bmp2);
							SetDlgItemText(hwndC, IDC_Pic_Path, Cancion_Actual->Pic);

	}return 1;

	case WM_COMMAND:
	{
					   switch (wParam)
					   {

					   case ID_Buscar:{
										  char Song[MAX_PATH];
										  strcpy(Song, "");
										  AbrirArchivo(Song, 3);
										  ///recortar nombre
										  //char NombreN[MAX_PATH];
										  //strcpy(NombreN, (ExtNombre(Cancion, NombreN)));
										  SetDlgItemText(hwndC, IDC_Dir, Song);


					   }return 1;
					   case IDC_Imagen:{
										   char Pic[MAX_PATH];
										   strcpy(Pic, "");
										   AbrirArchivo(Pic, 2);
										   SetDlgItemText(hwndC, IDC_Pic_Path, Pic);
										   

										   bmp1 = (HBITMAP)SendDlgItemMessage(hwndC, IDC_Pic_Song, STM_GETIMAGE, IMAGE_BITMAP, 0);
										   bmp2 = (HBITMAP)LoadImage(NULL, Pic, IMAGE_BITMAP, 140, 140, LR_LOADFROMFILE);
										   SendDlgItemMessage(hwndC, IDC_Pic_Song, STM_SETIMAGE, IMAGE_BITMAP, (LPARAM)bmp2);

					   }return 1;
					   case IDC_OK:{
									   strcpy_s(Cancion_Actual->Nombre, "");
									   strcpy_s(Cancion_Actual->Año, "");
									   strcpy_s(Cancion_Actual->Pic, "");
									   GetWindowText(Hno, Cancion_Actual->Nombre, sizeof(Cancion_Actual->Nombre));
									int   cursel = SendMessage(HD, CB_GETCURSEL, 0, 0);
									SendMessage(HD, CB_GETLBTEXT, cursel, (LPARAM)Cancion_Actual->Año);
									GetWindowText(GetDlgItem(hwndC,IDC_Pic_Path), Cancion_Actual->Pic, sizeof(Cancion_Actual->Pic));
					   //

									Listas *aux = Inicio;
									while (aux != NULL)
									{
										Canciones *auxc = aux->Lista;
										while (auxc != NULL)
										{
											if (Cancion_Actual->ID == auxc->ID)
											{
												strcpy_s(auxc->Nombre, Cancion_Actual->Nombre);
												strcpy_s(auxc->Año, Cancion_Actual->Año);
												strcpy_s(auxc->Pic, Cancion_Actual->Pic);

											}

											auxc = auxc->sig;

										}
										aux = aux->sig;
									}

									
									Lisbox_Canciones(GetDlgItem(actualizar, IDC_LN), GetDlgItem(actualizar, IDC_LNo), GetDlgItem(actualizar, IDC_LA), GetDlgItem(actualizar, IDC_LG), GetDlgItem(actualizar, IDC_LD));
									bmp1 = (HBITMAP)SendDlgItemMessage(hwndC, IDC_Pic_cancion, STM_GETIMAGE, IMAGE_BITMAP, 0);
									bmp2 = (HBITMAP)LoadImage(NULL, Cancion_Actual->Pic, IMAGE_BITMAP, 130, 130, LR_LOADFROMFILE);
									SendDlgItemMessage(actualizar, IDC_Pic_cancion, STM_SETIMAGE, IMAGE_BITMAP, (LPARAM)bmp2);
									EndDialog(hwndC, 0);

						//
					   }return 1;
					   }
	}return 1;


	case WM_CLOSE:
	{
					 EndDialog(hwndC, 0);

	}return 1;


	}return 0;
}

int dd = 1000;
BOOL CALLBACK MLista(HWND hwndL, UINT msg, WPARAM wParam, LPARAM lParam)
{
	
	static HWND HListN, HListNo;
	HListN = GetDlgItem(actualizar, IDC_LISTAS);
	HListNo = GetDlgItem(actualizar, IDC_LISTAS2);

	Lisbox_Listas(actualizar, HListN, HListNo);

	char ID_c[3];  int IDc = 0;
	Listas *nueva = new Listas;
	nueva->ant = NULL;
	nueva->sig = NULL;
	nueva->ID = 0;
	strcpy(nueva->Nombre, "");
	strcpy(nueva->Pic, "");
	nueva->Lista = NULL;
	IDc = ID + 1;
	static HWND HN, HNo, HP, HPP;
	/*static HWND HListN, HListNo;*/
	static HWND HCanN, HCanNo, HCanA, HCanG, HCanD;
	static HWND HCanNL, HCanNoL, HCanAL, HCanGL, HCanDL;
	int cursel1 = -1;
	int cursel = -1;
	char numCancion[3] = "";
	char numCancionL[3] = "";
	switch (msg)
	{
	case WM_INITDIALOG: {

							dd = Lista_Actual->ID;
							
							HN = GetDlgItem(hwndL, IDC_Numero);
							HNo = GetDlgItem(hwndL, IDC_Nombre);
							HP = GetDlgItem(hwndL, IDC_COVER);
							HPP = GetDlgItem(hwndL, IDC_Dir);

							_itoa(Lista_Actual->ID, ID_c, 10);
							 dd = Lista_Actual->ID;
							SetDlgItemText(hwndL, IDC_Numero, ID_c);
							SetDlgItemText(hwndL, IDC_Nombre, Lista_Actual->Nombre);
							SetDlgItemText(hwndL, IDC_Dir, Lista_Actual->Pic);



							HCanN = GetDlgItem(hwndL, IDC_LNT);
							HCanNo = GetDlgItem(hwndL, IDC_LNoT);
							HCanA = GetDlgItem(hwndL, IDC_LAT);
							HCanG = GetDlgItem(hwndL, IDC_LGT);
							HCanD = GetDlgItem(hwndL, IDC_LDT);

							HCanNL = GetDlgItem(hwndL, IDC_LNL);
							HCanNoL = GetDlgItem(hwndL, IDC_LNoL);
							HCanAL = GetDlgItem(hwndL, IDC_LAL);
							HCanGL = GetDlgItem(hwndL, IDC_LGL);
							HCanDL = GetDlgItem(hwndL, IDC_LDL);

							SendMessage(HCanN, LB_RESETCONTENT, 0, 0);
							SendMessage(HCanNo, LB_RESETCONTENT, 0, 0);
							SendMessage(HCanA, LB_RESETCONTENT, 0, 0);
							SendMessage(HCanG, LB_RESETCONTENT, 0, 0);
							SendMessage(HCanD, LB_RESETCONTENT, 0, 0);
							char N_c[3];
							if (Inicio != NULL){
								if (Inicio->Lista != NULL){
									Canciones *aux = Inicio->Lista;
									while (aux != NULL){
										strcpy(N_c, "");
										_itoa(aux->ID, N_c, 10);
										SendMessage(HCanN, LB_ADDSTRING, 0, (LPARAM)N_c);
										SendMessage(HCanNo, LB_ADDSTRING, 0, (LPARAM)aux->Nombre);
										SendMessage(HCanA, LB_ADDSTRING, 0, (LPARAM)aux->Artista);
										SendMessage(HCanG, LB_ADDSTRING, 0, (LPARAM)aux->Genero);
										SendMessage(HCanD, LB_ADDSTRING, 0, (LPARAM)aux->Año);
										aux = aux->sig;
									}
								}
							}
							static HBITMAP bmp1, bmp2;

							bmp1 = (HBITMAP)SendDlgItemMessage(hwndL, IDC_COVER, STM_GETIMAGE, IMAGE_BITMAP, 0);
							bmp2 = (HBITMAP)LoadImage(NULL, Lista_Actual->Pic, IMAGE_BITMAP, 180, 180, LR_LOADFROMFILE);
							SendDlgItemMessage(hwndL, IDC_COVER, STM_SETIMAGE, IMAGE_BITMAP, (LPARAM)bmp2);
							SetDlgItemText(hwndL, IDC_Dir, Lista_Actual->Pic);
							if (Inicio->sig!=NULL)
							{
								Listas *haux = Inicio->sig;
								while (haux!=NULL)
								{
									if (haux->ID==dd)
									{
										Lista_Actual = haux;
									}
									haux = haux-> sig;
								}
							}
							Lisbox_Canciones(HCanNL, HCanNoL, HCanAL, HCanGL, HCanDL);



	}return 1;

	case WM_COMMAND:
	{
					   switch (LOWORD(wParam))
					   {
						   //
					   case IDC_LNT:{
										cursel1 = SendMessage(HCanN, LB_GETCURSEL, 0, 0);
										SendMessage(HCanNo, LB_SETCURSEL, cursel1, 0);
										SendMessage(HCanA, LB_SETCURSEL, cursel1, 0);
										SendMessage(HCanG, LB_SETCURSEL, cursel1, 0);
										SendMessage(HCanD, LB_SETCURSEL, cursel1, 0);
										strcpy(numCancion, "");
										SendMessage(HCanN, LB_GETTEXT, cursel1, (LPARAM)numCancion);

					   }return true;
					   case IDC_LNoT:{
										 cursel1 = SendMessage(HCanNo, LB_GETCURSEL, 0, 0);
										 SendMessage(HCanN, LB_SETCURSEL, cursel1, 0);
										 SendMessage(HCanA, LB_SETCURSEL, cursel1, 0);
										 SendMessage(HCanG, LB_SETCURSEL, cursel1, 0);
										 SendMessage(HCanD, LB_SETCURSEL, cursel1, 0);
										 strcpy(numCancion, "");
										 SendMessage(HCanN, LB_GETTEXT, cursel1, (LPARAM)numCancion);
					   }return true;
					   case IDC_LAT:{
										cursel1 = SendMessage(HCanA, LB_GETCURSEL, 0, 0);
										SendMessage(HCanNo, LB_SETCURSEL, cursel1, 0);
										SendMessage(HCanN, LB_SETCURSEL, cursel1, 0);
										SendMessage(HCanG, LB_SETCURSEL, cursel1, 0);
										SendMessage(HCanD, LB_SETCURSEL, cursel1, 0);
										strcpy(numCancion, "");
										SendMessage(HCanN, LB_GETTEXT, cursel1, (LPARAM)numCancion);

					   }return true;
					   case IDC_LGT:{
										cursel1 = SendMessage(HCanG, LB_GETCURSEL, 0, 0);
										SendMessage(HCanN, LB_SETCURSEL, cursel1, 0);
										SendMessage(HCanA, LB_SETCURSEL, cursel1, 0);
										SendMessage(HCanNo, LB_SETCURSEL, cursel1, 0);
										SendMessage(HCanD, LB_SETCURSEL, cursel1, 0);
										strcpy(numCancion, "");
										SendMessage(HCanN, LB_GETTEXT, cursel1, (LPARAM)numCancion);
					   }return true;
					   case IDC_LDT:{
										cursel1 = SendMessage(HCanD, LB_GETCURSEL, 0, 0);
										SendMessage(HCanN, LB_SETCURSEL, cursel1, 0);
										SendMessage(HCanA, LB_SETCURSEL, cursel1, 0);
										SendMessage(HCanG, LB_SETCURSEL, cursel1, 0);
										SendMessage(HCanNo, LB_SETCURSEL, cursel1, 0);
										strcpy(numCancion, "");
										SendMessage(HCanN, LB_GETTEXT, cursel1, (LPARAM)numCancion);
					   }return true;

						   //
					   case IDC_LNL:{
										cursel = SendMessage(HCanNL, LB_GETCURSEL, 0, 0);
										SendMessage(HCanNoL, LB_SETCURSEL, cursel, 0);
										SendMessage(HCanAL, LB_SETCURSEL, cursel, 0);
										SendMessage(HCanGL, LB_SETCURSEL, cursel, 0);
										SendMessage(HCanDL, LB_SETCURSEL, cursel, 0);
										strcpy(numCancionL, "");
										SendMessage(HCanNL, LB_GETTEXT, cursel, (LPARAM)numCancionL);

					   }return true;
					   case IDC_LNoL:{
										 cursel = SendMessage(HCanNoL, LB_GETCURSEL, 0, 0);
										 SendMessage(HCanNL, LB_SETCURSEL, cursel, 0);
										 SendMessage(HCanAL, LB_SETCURSEL, cursel, 0);
										 SendMessage(HCanGL, LB_SETCURSEL, cursel, 0);
										 SendMessage(HCanDL, LB_SETCURSEL, cursel, 0);
										 strcpy(numCancionL, "");
										 SendMessage(HCanNL, LB_GETTEXT, cursel, (LPARAM)numCancionL);
					   }return true;
					   case IDC_LAL:{
										cursel = SendMessage(HCanAL, LB_GETCURSEL, 0, 0);
										SendMessage(HCanNoL, LB_SETCURSEL, cursel, 0);
										SendMessage(HCanNL, LB_SETCURSEL, cursel, 0);
										SendMessage(HCanGL, LB_SETCURSEL, cursel, 0);
										SendMessage(HCanDL, LB_SETCURSEL, cursel, 0);
										strcpy(numCancionL, "");
										SendMessage(HCanNL, LB_GETTEXT, cursel, (LPARAM)numCancionL);

					   }return true;
					   case IDC_LGL:{
										cursel = SendMessage(HCanGL, LB_GETCURSEL, 0, 0);
										SendMessage(HCanNL, LB_SETCURSEL, cursel, 0);
										SendMessage(HCanAL, LB_SETCURSEL, cursel, 0);
										SendMessage(HCanNoL, LB_SETCURSEL, cursel, 0);
										SendMessage(HCanDL, LB_SETCURSEL, cursel, 0);
										strcpy(numCancionL, "");
										SendMessage(HCanNL, LB_GETTEXT, cursel, (LPARAM)numCancionL);
					   }return true;
					   case IDC_LDL:{
										cursel = SendMessage(HCanDL, LB_GETCURSEL, 0, 0);
										SendMessage(HCanNL, LB_SETCURSEL, cursel, 0);
										SendMessage(HCanAL, LB_SETCURSEL, cursel, 0);
										SendMessage(HCanGL, LB_SETCURSEL, cursel, 0);
										SendMessage(HCanNoL, LB_SETCURSEL, cursel, 0);
										strcpy(numCancionL, "");
										SendMessage(HCanNL, LB_GETTEXT, cursel, (LPARAM)numCancionL);
					   }return true;
						   //
					   case ID_Buscar:{
										  char Imagen[MAX_PATH];
										  strcpy(Imagen, "");


										  AbrirArchivo(Imagen, 2);
										  strcpy(nueva->Pic, Imagen);
										  SetDlgItemText(hwndL, IDC_Dir, nueva->Pic);
										  static HBITMAP bmp1, bmp2;

										  bmp1 = (HBITMAP)SendDlgItemMessage(hwndL, IDC_COVER, STM_GETIMAGE, IMAGE_BITMAP, 0);
										  bmp2 = (HBITMAP)LoadImage(NULL, Imagen, IMAGE_BITMAP, 180, 180, LR_LOADFROMFILE);
										  SendDlgItemMessage(hwndL, IDC_COVER, STM_SETIMAGE, IMAGE_BITMAP, (LPARAM)bmp2);


					   }return 1;
					   case IDC_OK:{
									  
									
									   nueva->Lista=NULL;
									   GetWindowText(HN, ID_c, sizeof(ID_c));
									   nueva->ID = atoi(ID_c);
									   GetWindowText(HNo, nueva->Nombre, sizeof(Lista_Actual->Nombre));
									   GetWindowText(HPP, nueva->Pic, sizeof(Lista_Actual->Pic));

									   //
									   int hh = SendMessage(HCanNL, LB_GETCOUNT, 0, 0);
									   if (Inicio != NULL)
									   {

										   if (Inicio->Lista != NULL)
										   {
											   Canciones *help = Inicio->Lista;
											   char IDs[5]; int IDto = -22;
											   for (int i = 0; i < hh; i++)
											   {
												   strcpy_s(IDs, "");
												   SendMessage(HCanNL, LB_GETTEXT, i, (LPARAM)IDs);
												   IDto = atoi(IDs);

												   while (help != NULL)
												   {
													   if (IDto == help->ID)
													   {
														   Canciones *nuevaC = new Canciones;
														   nuevaC->ant = NULL;
														   nuevaC->sig = NULL;
														   nuevaC->ID = help->ID;
														   strcpy(nuevaC->Año, help->Año);
														   strcpy(nuevaC->Artista, help->Artista);
														   strcpy(nuevaC->Genero, help->Genero);
														   strcpy(nuevaC->Nombre, help->Nombre);
														   strcpy(nuevaC->Pic, help->Pic);
														   strcpy(nuevaC->Song, help->Song);

														   if (nueva->Lista == NULL){
															   nueva->Lista = nuevaC;
															   nueva->Lista->sig = NULL;
															   nueva->Lista->ant = NULL;
														   }
														   else{
															   Canciones *Aux = nueva->Lista;
															   while (Aux->sig != NULL)
															   {
																   Aux = Aux->sig;
															   }
															   Aux->sig = nuevaC;
															   Aux->sig->ant = Aux;
														   }
													   }
													   help = help->sig;
												   }
												   help = Inicio->Lista;
											   }
											   /*Canciones *help = Inicio->Lista;
											   while (help!=NULL)
											   {

											   help = help->sig;
											   }*/
										   }
									   }

									   //

									   if (Validar_Listas(nueva))
									   {
										   if (Inicio->sig != NULL)
										   {
											   Listas *haux = Inicio->sig;
											   while (haux != NULL)
											   {
												   if (haux->ID == dd)
												   {
													   Lista_Actual = haux;
												   }
												   haux = haux->sig;
											   }
										   }
										   nueva->ant = Lista_Actual->ant;
										   nueva->sig = Lista_Actual->sig;

										   if (Lista_Actual->sig!=NULL)
										   {
											   Lista_Actual->sig->ant = nueva;
										   }
										   if (Lista_Actual->ant != NULL)
										   {
											   Lista_Actual->ant->sig = nueva;
										   }
										   
										   MessageBox(hwndL, "Lista Modificada correctamente", "Agregado exitoso!", MB_OK);

										   //ID = atoi(ID_c);
										   ////ID++;
										   //C_Listas++;
										   /* save_data();
										   load_data();*/

										   EndDialog(hwndL, 0);
									   }
					   }return 1;
					   case IDC_mas:{////////boton mas
										strcpy(numCancion, "");
										cursel1 = SendMessage(HCanN, LB_GETCURSEL, 0, 0);
										SendMessage(HCanN, LB_GETTEXT, cursel1, (LPARAM)numCancion);
										if (strcmp(numCancion, "") != 0){

											int numCancionint = atoi(numCancion);
											if (Inicio != NULL){
												if (Inicio->Lista != NULL)
												{


													Canciones *Aux = Inicio->Lista;
													while (Aux != NULL)
													{//AuxC es para recorrer la nueva lista, Aux es para recorrer la lista principal
														if (numCancionint == Aux->ID){
															char N_c[3];
															strcpy(N_c, "");
															_itoa(Aux->ID, N_c, 10);
															SendMessage(HCanNL, LB_ADDSTRING, 0, (LPARAM)N_c);
															SendMessage(HCanNoL, LB_ADDSTRING, 0, (LPARAM)Aux->Nombre);
															SendMessage(HCanAL, LB_ADDSTRING, 0, (LPARAM)Aux->Artista);
															SendMessage(HCanGL, LB_ADDSTRING, 0, (LPARAM)Aux->Genero);
															SendMessage(HCanDL, LB_ADDSTRING, 0, (LPARAM)Aux->Año);

														}
														Aux = Aux->sig;
													}
												}
											}


										}
										else{
											MessageBox(hwndL, "Seleccione una cancion", "Advertencia!", MB_OK);
										}

					   }return 1;
					   case IDC_menos:{
										  strcpy(numCancionL, "");
										  cursel = SendMessage(HCanNL, LB_GETCURSEL, 0, 0);
										  SendMessage(HCanNL, LB_GETTEXT, cursel, (LPARAM)numCancionL);
										  if (strcmp(numCancionL, "") != 0){

											  cursel = SendMessage(HCanNL, LB_GETCURSEL, 0, 0);
											  SendMessage(HCanNL, LB_DELETESTRING, cursel, 0);
											  SendMessage(HCanNoL, LB_DELETESTRING, cursel, 0);
											  SendMessage(HCanAL, LB_DELETESTRING, cursel, 0);
											  SendMessage(HCanGL, LB_DELETESTRING, cursel, 0);
											  SendMessage(HCanDL, LB_DELETESTRING, cursel, 0);
										  }
										  else{
											  MessageBox(hwndL, "Seleccione una cancion", "Advertencia!", MB_OK);
										  }

					   }return 1;


					   }
	}return 1;


	case WM_CLOSE:
	{
					 EndDialog(hwndL, 0);

	}return 1;


	}return 0;
}