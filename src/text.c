#define _paratext_c

#include <stdio.h>
#include <conio.h>
#include <alloc.h>
#include <mem.h>
#include <string.h>
#include <dos.h>
#include <math.h>
#include <process.h>

#include "defs.h"
#include "struct.h"
#include "proto.h"
#include "global.h"
#include "paratext.h"

#define WAITCHAR 220

int CharLenList[100]={
	0,0,0,0,0, 0,0,0,0,0,		/* ' !"#$%&'()' */
	0,0,0,0,0, 0,0,0,0,0,		/* '*+,-./0123' */
	0,0,0,0,0, 0,0,0,0,0,		/* '456789:;<=' */
	0,0,0,1,1, 1,1,1,1,1 ,		/* '>?@ABCDEFG' */
	1,1,1,1,1, 1,1,1,1,1,		/* 'HIJKLMNOPQ' */
	1,1,1,1,1, 1,1,1,1,0,		/* 'RSTUVWXYZ[' */
	0,0,0,0,0, 0,0,0,0,0,		/* '\]^_`abcde' */
	0,0,0,0,0, 0,0,1,0,0,		/* 'fghijklmno' */
	0,0,0,0,0, 0,0,1,0,0,		/* 'pqrstuvwxy' */
	0,0,0,0,0, 0,0,0,0,1			/* 'z{|}~ ... ' */
};
		
char* Wordpointer;
unsigned char* Fontpointer;
unsigned char* Zeichenpointer[110]; /* Pointer-Feld auf Buchstaben-Icons */
unsigned int CurrentFontFG = FIRST_FONT_FG;		/* Momentane Schrift-Farben */
unsigned int CurrentFontBG = FIRST_FONT_BG;

/* Text-Fenster */
int LeftTextBorder;
int RightTextBorder;
int LowerTextBorder;
int UpperTextBorder;

int CharsPerLine; /* Zeilenlaenge: veraltet */

/* Aktuelle Text-Einfuege-Position: */
int MyCursorX; 
int MyCursorY;

/* Buffer fuer Text-Environment */
int StoreCursorX;
int StoreCursorY;
int StoreLeftTextBorder;
int StoreRightTextBorder;
int StoreLowerTextBorder;
int StoreUpperTextBorder;
unsigned int StoreTextBG;
unsigned int StoreTextFG;


extern int TimerFlag; /* Timerflag- wer haette das gedacht ! */


/*@Function============================================================
@Desc:  This function stores the textenvironment

@Ret: 
@Int:
* $Function----------------------------------------------------------*/
void StoreTextEnvironment(void){
	StoreLeftTextBorder=LeftTextBorder;
	StoreRightTextBorder=RightTextBorder;
	StoreLowerTextBorder=LowerTextBorder;
	StoreUpperTextBorder=UpperTextBorder;
	StoreCursorX=MyCursorX;
	StoreCursorY=MyCursorY;
	GetTextColor(&StoreTextBG,&StoreTextFG);
}

/*@Function============================================================
@Desc:  This function restores the textenvironment

@Ret: 
@Int:
* $Function----------------------------------------------------------*/
void RestoreTextEnvironment(void){
	LeftTextBorder=StoreLeftTextBorder;
	RightTextBorder=StoreRightTextBorder;
	LowerTextBorder=StoreLowerTextBorder;
	UpperTextBorder=StoreUpperTextBorder;
	MyCursorX=StoreCursorX;
	MyCursorY=StoreCursorY;
	SetTextColor(StoreTextBG,StoreTextFG);
}

/*@Function============================================================
@Desc: Initialisierne des Text-Moduls: Buchstaben-Icons einlesen

@Ret: OK/Exit
@Int:
* $Function----------------------------------------------------------*/
int InitParaplusFont(void){
	int i,j;
	int FontXPos=0;
	unsigned char *FontMem;
	
	if( RealScreen == NULL ) 
		RealScreen = MK_FP(SCREENADDRESS,0000);	
	if( InternalScreen == NULL ) 
		InternalScreen = (unsigned char*)MyMalloc(SCREENHOEHE*SCREENBREITE);
	if( InternalScreen == NULL) {
		printf("No memory left !!");
		Terminate(-1);
	}
	
	LadeLBMBild(FONTBILD,InternalScreen,FALSE);

	FontMem = (unsigned char*)MyMalloc(FONTANZAHL*FONTMEM*2+10);
	
	for(j=0;j<10;j++){
		FontXPos = 0;
		
		for(i=0;i<10;i++){
			Zeichenpointer[j*10+i]=FontMem + (j*10+i)*FONTMEM*2;
	 		IsolateBlock(InternalScreen, Zeichenpointer[j*10+i],FontXPos,j*(FONTHOEHE+1),
				(CharLenList[j*10+i] == 0 ) ? FONTBREITE : 2*FONTBREITE, FONTHOEHE);
			FontXPos += FONTBREITE+1 + (CharLenList[j*10+i]*(FONTBREITE+1));
		}
		
	}
	
	return (OK);
}

/*@Function============================================================
@Desc: SetTextColor(unsigned bg, unsigned fg)
Setzt die Palettenwerten (und nicht die RGB-Werte) der Proportionalschrift
Null als Farbwert bewirkt keinen Effekt
Sicherheitsabfrage, ob die schrift nicht durch Kontrastzerst"orung vernichtet
wird

				sets only, if color != 0 and other then old color
@Ret: void
@Int:
* $Function----------------------------------------------------------*/
void SetTextColor(unsigned bg, unsigned fg)
{
	register unsigned int i;
	static unsigned LastBg = FIRST_FONT_BG;
	static unsigned LastFg = FIRST_FONT_FG;
	register unsigned char *source;

	/* Sicherheitsabrage bez. Schriftzerst"orung durch Kontrastausl"oschung */
	if ((bg == LastFg) || (bg == fg)) {
		printf(" WARNING ! Die Schrift wird durch diesen Aufruf vernichtet !\n");
		getch();
	}
	CurrentFontFG=fg;
	CurrentFontBG=bg;
	
	source = Zeichenpointer[0];
	if( (bg != 0) && (bg != LastBg)) {
		for(i=0; i<FONTANZAHL*FONTMEM*2; i++, source ++) 
			if( *source == LastBg ) *source = bg;
		LastBg = bg;
	}

	source = Zeichenpointer[0];
	if( fg && (fg != LastFg)) {
		for( i=0; i<FONTANZAHL*FONTMEM*2; i++, source ++)
			if( *source == LastFg ) *source = fg;
		LastFg = fg;
	}

	return;
} /* SetTextcolor */

/* ====================================================================== 
   Diese Funktion soll die momentane Farbsituation des Textes wiedergeben.
	Dazu werden zwei Pointer "ubergeben, damit sie auch ver"andert werden.
		
	Grund f"ur die Funktion: so sparen wir zwei weitere globale Variablen
   ---------------------------------------------------------------------- */
void GetTextColor(unsigned int* bg,unsigned int* fg){
	*bg=CurrentFontBG;
	*fg=CurrentFontFG;
}

/*@Function============================================================
@Desc: SetTextBorder(): setzt die Bezugs-daten fuer die folgenden
							Text-ausgaben
					RightTextBorder:
					LeftTextBorder:
					UpperTextBorder:
					LowerTextBorder:

@Ret: 
@Int:
* $Function----------------------------------------------------------*/
void SetTextBorder(
	int left,
	int upper,
	int right,
	int lower,
	int chars_per_line)
{
	LeftTextBorder = left;
	RightTextBorder = right;
	UpperTextBorder = upper;
	LowerTextBorder = lower;

	CharsPerLine = chars_per_line;
	
	return;
} /* SetTextBorder */

/*@Function============================================================
@Desc: ClearTextBorder(*screen): Loescht am Screen den Bereich des
					gesetzten Textfensters (textborder)

@Ret: voidN
@Int:
* $Function----------------------------------------------------------*/
void ClearTextBorder(unsigned char *screen, int color) {
	register int i;
	register int height = LowerTextBorder - UpperTextBorder;
	register int xlen = RightTextBorder - LeftTextBorder;
	unsigned char *target;

	target = screen + UpperTextBorder*SCREENBREITE + LeftTextBorder;
				
	for( i=0; i<height; i++) {
		memset(target,color, xlen);
		target += SCREENBREITE;
	}
	return;
}

/*@Function============================================================
@Desc: SetTextCursor(x, y): Setzt Cursor fuer folgende Textausgaben

@Ret: 
@Int:
* $Function----------------------------------------------------------*/
void SetTextCursor(int x, int y) {
	MyCursorX = x;
	MyCursorY = y;

	return;
} 

/*@Function============================================================
@Desc: ScrollText(Text, startx, starty):

@Ret: 
@Int:
* $Function----------------------------------------------------------*/
int ScrollText(char *Text, int startx, int starty, int EndLine) {
	int lines=0;		/* Anzahl der Textzeilen */
	long TextHeight;		/* Hoehe des Gesamt-Textes in Pixel */
	char *textpt;	/* bewegl. Textpointer */
	int InsertLine = starty;
	int speed = +2;
	int maxspeed = 4;

	/* Zeilen zaehlen */
	textpt = Text;
	while(*textpt++)
		if(*textpt == '\n') lines ++;

	/* Texthoehe berechnen */
	TextHeight = lines*(FONTHOEHE+ZEILENABSTAND);

	while( !SpacePressed && ((InsertLine+TextHeight) > EndLine)) {
		
		JoystickControl();
		
		if( UpPressed ) {
			speed--;
			if( speed < -maxspeed ) speed = -maxspeed;
			UpPressed = FALSE;
		}
		if( DownPressed ) {
			speed ++;
			if( speed > maxspeed ) speed = maxspeed;
			DownPressed = FALSE;
		}
		
		KillTastaturPuffer();
		
		if( !TimerFlag ) continue;		/* Synchronisierung */
		TimerFlag = FALSE;

		ClearTextBorder(InternalScreen,CurrentFontBG );
		DisplayText(Text, startx, InsertLine, InternalScreen, FALSE);
		InsertLine -= speed;
		
		/* Nicht bel. nach unten wegscrollen */
		if( InsertLine > SCREENHOEHE-10  && (speed < 0) ) {
			InsertLine = SCREENHOEHE-10;
			speed = 0;
		}
		
		SwapScreen();  /* Show it */
	}

	SpacePressed = FALSE;
	return OK;
}	



/*@Function============================================================
@Desc: void DisplayText(char *Text, int startx, int starty):
				gibt Text beginnend bei startx/starty aus
				Zeilenumbruch bei voller Zeile oder bei '\n'
				
				*screen: Pointer to the screen to use
				EnterCursor: TRUE/FALSE Cursor darstellen ja/nein.
@Ret: void
@Int:
* $Function----------------------------------------------------------*/
void DisplayText(
	char* Text,
	int startx,
	int starty,
	unsigned char *screen,
	int EnterCursor
) {
	char *tmp; /* Beweg. Zeiger auf aktuelle Position im Ausgabe-Text */
	int i;

	MyCursorX = startx;		/* akt. Schreib-Position */
	MyCursorY = starty;

	tmp = Text; /* Auf Text-Begin setzen */
	
	while(!FensterVoll()) {

		if( *tmp == '\0' ) { /* Textende erreicht */
			/* Cursor darstellen, falls erwuenscht */
			if( EnterCursor ) DisplayChar(CURSOR_ICON, screen);
			break;
		}

		if( *tmp == '\n' ) { 					/* Zeilenende erreicht */
			MyCursorX = startx; 					/* "Wagenruecklauf" */
			MyCursorY += FONTHOEHE+ZEILENABSTAND; 		/* naechste Zeile */
			tmp ++; 									/* skip the newline-char !! */
			continue;
		}

		if((unsigned char) *tmp == WAITCHAR ) {
			tmp++;
			while (UpPressed || DownPressed || LeftPressed || RightPressed) JoystickControl();
			while (!UpPressed && !DownPressed && !LeftPressed && !RightPressed && !SpacePressed) JoystickControl();
			if (SpacePressed) continue;
			while (UpPressed || DownPressed || LeftPressed || RightPressed) JoystickControl();
			MyCursorX = startx;
			MyCursorY = starty;
			ClearUserFenster();
			continue;
		}
		
		/* Normales Zeichen ausgeben: */
		DisplayChar(*tmp, screen);
		tmp ++;

		CheckUmbruch();		/* dont write over RightBorder */
		
	} /* while Fenster nicht voll */

}

/*@Function============================================================
@Desc: 
			Wird nicht gebraucht ???
@Ret: 
@Int:
* $Function----------------------------------------------------------*/
void DisplayWord(char* Worttext){
	int i;

	gotoxy(1,1);
	printf("\nDisplayWord() called !!!");
	getch();
	
	if (FensterVoll()) return;
	
	for(i=0;i<strlen(Worttext);i++){
		DisplayChar(*(Worttext+i), RealScreen);
	}
	MyCursorX += FONTBREITE;
	CheckUmbruch();
}

/*@Function============================================================
@Desc: Stellt ein Zeichen an der Pos MyCursorX/Y INNERHALB des
		 Text-Borders dar. Hinausragende Teile des Zeichens werden
		 "abgeschnitten".
		 Der Einfuegepunkt MyCursorX/Y wird nach der Ausgabe um die
		 Zeichenbreite nach rechts verschoben.

		 Einfuegepunkt: LINKS OBEN

@Ret: void
@Int:
* $Function----------------------------------------------------------*/
void DisplayChar(unsigned char Zeichen, unsigned char *screen){
	int i,j;
	int ZNum = Zeichen - ' ';
	int ZLen = CharLenList[ZNum];

	unsigned char *target;	/* Pointer auf Ausgabeposition am Screen */


	if (Zeichen == '\n') {
		MakeUmbruch();
		return;
	}

	if( (Zeichen < ' ') || (Zeichen > 131)) {
		printf("Illegal Char an DisplayChar() uebergeben!");
		getch();
		return;
	}
	
	for(i=0;i<FONTHOEHE;i++){
		if( MyCursorY + FONTHOEHE <= UpperTextBorder ) break;
		if ( (MyCursorY+i) <= UpperTextBorder) continue; 
		if ( (MyCursorY + i) >= LowerTextBorder) break;
		if((MyCursorX > RightTextBorder) ||
			(MyCursorX+FONTBREITE < LeftTextBorder) )
			break;

		target = screen+MyCursorX+MyCursorY*SCREENBREITE + i*SCREENBREITE;
		MyMemcpy(target, Zeichenpointer[ZNum] + i*(1+ZLen)*FONTBREITE,
			FONTBREITE * (1+ZLen) );
	} /* for */
	
	MyCursorX += FONTBREITE*(1+ZLen); /* Intern-Cursor weiterbewegen */

	return;
}

/*@Function============================================================
@Desc: 

@Ret: 
@Int:
* $Function----------------------------------------------------------*/
void CheckUmbruch(void){
	if (MyCursorX > LeftTextBorder+CharsPerLine*FONTBREITE)
		MakeUmbruch();
}


/*@Function============================================================
@Desc: 

@Ret: 
@Int:
* $Function----------------------------------------------------------*/
void MakeUmbruch(void){
	MyCursorX=LeftTextBorder;
	MyCursorY+=FONTHOEHE + ZEILENABSTAND;
}


/*@Function============================================================
@Desc: 

@Ret: 
@Int:
* $Function----------------------------------------------------------*/
int FensterVoll(void){
	if (MyCursorY > LowerTextBorder) return (TRUE);
	return (FALSE);
}

/*@Function============================================================
@Desc: GetString(): Liest einen String vom User ein
						MaxLen: max. Laenge des Strings

@Ret: char *: String wird HIER reserviert !!! (dont forget to free it !)
@Int:
* $Function----------------------------------------------------------*/
char* GetString(int MaxLen){
	char *instring;		/* Pointer auf eingegebenen String */
	char *loeschstring;	/* String zum Loeschen der Eingabe-Zeile */
	char taste;				/* eingeg. Zeichen */
	int charcounter = 0;	/* zaehlt eingeg. Zeichen mit */
	int TextOutX, TextOutY;	/* Einfuegepunkt zum Darstellen der Eingabe */

	/* Texteingabe an momentaner Cursor-Pos. */
	TextOutX = MyCursorX; 
	TextOutY = MyCursorY;
	
	/* Speicher fuer Eingabe reservieren */
	if( (instring = MyMalloc(MaxLen + 10)) == NULL) {
		printf("\nNo Memory left !!");
		getch();
		Terminate(-1);
	}
	/* LoeschString reservieren */
	if( (loeschstring=MyMalloc(2*MaxLen)) == NULL) {
		printf("\nNo Memory left !!");
		getch();
		Terminate(-1);
	}
	memset(loeschstring, ' ', 2*MaxLen - 1); /* Loeschstring mit SPACE fuellen */
	loeschstring[MaxLen-1]='\0';	/* Loeschstring abschliessen */
	
	instring[0] = '\0'; /* sicherheitshalber abschliessen */

	/* Leeren String mit Cursor ausgeben */
	DisplayText(instring, TextOutX, TextOutY, RealScreen, TRUE);
	
	/* Zeichen einlesen und anzeigen, bis RET gedrueckt */
	while( (taste = getch()) != RETURN_ASCII) {
	
		/* Sondertasten mit ext. Code ignorieren: */
		if( taste == 0 ) {getch(); continue;}

		/* Regulaeres Zeichen: aufnehmen, falls MaxLen noch nicht erreicht */
		if( (taste >= ' ') && (taste <= 'z') && (charcounter < MaxLen) ) {
			instring[charcounter ++] = taste;	/* Zeichen aufnehmen */
			instring[charcounter] = '\0';			/* und abschliessen */
		}

		/* Backspace: ausfuehren */
		if( taste == BACKSPACE_ASCII ) {
			/* Wenn nicht schon am Beginn des STrings: */
			if( charcounter ) {
				charcounter --;					/* Zeichen zurueck */
				instring[charcounter] = '\0';	/* und abschliessen */
			}
		}

		/* alte eingabe-Zeile loeschen: */
		/* es wird eine Laenge MaxLen -1 geloescht */
		DisplayText(loeschstring, TextOutX, TextOutY, RealScreen, FALSE);		
		
		
		/* Eingabe mit Cursor ausgeben */
		DisplayText(instring, TextOutX, TextOutY, RealScreen, TRUE);

	} /* While nicht RET gedrueckt */

	return(instring);
	
} /* GetString() */


#undef _paratext_c
