#ifndef _ship_h
#define _ship_h

/*
 *	hier sind die konstanten die die RGB-werte f"ur die vier Menupunkte
 * enthalten. H=hilight G=gray M1C=Palettenwert des 1. Menupunktes
 * BGC=BackGroundColor
 */
#define HR 63
#define HG 63
#define HB 63
#define GR 33
#define GG 33
#define GB 33

/* Der Farbwert f"ur M1C kollidiert mit dem INFLUENCEFARBWERT.
	Dieser Umstand ist aber g"unstig, denn dadurch braucht man
	den Influencer als ersten Menupunkt nicht in einem anderen
	Palettenwert zeichnen als sonst.
	*/

#define M1C INFLUENCEFARBWERT
#define M2C 18
#define M3C 19
#define M4C 20
#define BGC 21

//#define KON_TEXT_RGB 63,63,63
#define KON_BG_COLOR		96
#define KON_TEXT_COLOR	FONT_YELLOW

#define EL_BG_COLOR		EL_STARTCOLOR			/* Hintergrund im Elevator-Bild */
#define EL_FIRSTCOLOR	EL_STARTCOLOR + 1  	/* the color of level 0 */
#define EL_LASTCOLOR		EL_STARTCOLOR +25

#define EL_FIRST_ELEVATOR_COLOR		209

#define WAIT_ELEVATOR		9		/* warte, bevor Lift weitergeht */
#define MENUITEMPOSX 10
#define MENUITEMPOSY 85

#define MENUTEXT_X	100

#endif
