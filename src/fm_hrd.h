/* Headerfile FM_HRD.H zum Module FM_HRD.C
   beinhaltet hardwarespezifische Funktionen zur Programmierung
   der FM-Kan„le des Soundblasters
*/

#ifndef __fm_hrd

/* Typdefinitionen */
#ifndef TRUE
  #define TRUE 1
#endif
#ifndef FALSE
  #define FALSE 0
#endif
#ifndef ON
  #define ON 1
#endif
#ifndef OFF
  #define OFF 0
#endif

#ifndef _BYTE
  typedef unsigned char byte;
#define _BYTE
#endif
#ifndef _WORD
  typedef unsigned word;
#define _WORD
#endif
#ifndef _BOOL
  typedef char bool;
#define _BOOL
#endif
typedef struct
{ byte mod_charac;
  byte tra_charac;
  byte mod_ampl;
  byte tra_ampl;
  byte mod_attdec;
  byte tra_attdec;
  byte mod_susrel;
  byte tra_susrel;
  byte mod_welle;
  byte tra_welle;
  byte kan_rueck;
  byte dummy[5];
} instrument;


/* Zuordnungstabellen */

extern byte fm_cell_offset[18];
extern byte fm_modulator[9];
extern byte fm_carrier[9];
extern byte fm_mod_offset[9];
extern byte fm_carr_offset[9];

/* globale Daten. VORSICHT bei der Žnderung!!! */

extern byte __sb_int;
extern word __sb_port;
extern word __sb_time3;
extern word __sb_time23;
extern byte __Lfrequ[128];
extern byte __Hfrequ[128];

/* Elementarfunktionen */

extern int sbfm_init(void);
extern byte sbfm_in(byte sb_reg);
extern void sbfm_out(byte sb_reg,byte sb_value);
extern void sbfm_silence(void);

/* Zellenfunktionen */

extern void sbfm_tremolo(byte zelle,bool on);
extern void sbfm_vibrato(byte zelle,bool on);
extern void sbfm_tonart(byte zelle,bool kontin);
extern void sbfm_huelldaempf(byte zelle,bool on);
extern void sbfm_zeitfakt(byte zelle,byte fakt);
extern void sbfm_ampldaempf(byte zelle,byte val);
extern void sbfm_daempfung(byte zelle,byte val);
extern void sbfm_attack(byte zelle,byte val);
extern void sbfm_decay(byte zelle,byte val);
extern void sbfm_sustain(byte zelle,byte val);
extern void sbfm_release(byte zelle,byte val);
extern void sbfm_welle(byte zelle,byte val);

/* Kanalfunktionen */

extern void sbfm_frequ(byte kanal,word frequ);
extern void sbfm_oktave(byte kanal,byte oktave);
extern void sbfm_ton(byte kanal,bool on);
extern void sbfm_verbind(byte kanal,bool parallel);
extern void sbfm_rueckkoppl(byte kanal,byte val);

/* komplexe Funktionen */

extern void sbfm_instrument_out(byte channel_number,instrument far *sb_inst);
extern void sbfm_key_off(byte channel,byte key,byte pitch);
extern void sbfm_key_on(byte channel,byte key,byte pitch);


#else

/* Typdefinitionen */
#ifndef TRUE
  #define TRUE 1
#endif
#ifndef FALSE
  #define FALSE 0
#endif
#ifndef ON
  #define ON 1
#endif
#ifndef OFF
  #define OFF 0
#endif

#ifndef _BYTE
  typedef unsigned char byte;
#define _BYTE
#endif
#ifndef _WORD
  typedef unsigned word;
#define _WORD
#endif
#ifndef _BOOL
  typedef char bool;
#define _BOOL
#endif
typedef struct
{ byte mod_charac;
  byte tra_charac;
  byte mod_ampl;
  byte tra_ampl;
  byte mod_attdec;
  byte tra_attdec;
  byte mod_susrel;
  byte tra_susrel;
  byte mod_welle;
  byte tra_welle;
  byte kan_rueck;
  byte dummy[5];
} instrument;


/* Zuordnungstabellen */

extern byte fm_cell_offset[18];
extern byte fm_modulator[9];
extern byte fm_carrier[9];
extern byte fm_mod_offset[9];
extern byte fm_carr_offset[9];

/* globale Daten. VORSICHT bei der Žnderung!!! */

extern byte __sb_int;
extern word __sb_port;
extern word __sb_time3;
extern word __sb_time23;
extern byte __Lfrequ[128];
extern byte __Hfrequ[128];

/* Elementarfunktionen */

int sbfm_init(void);
byte sbfm_in(byte sb_reg);
void sbfm_out(byte sb_reg,byte sb_value);
void sbfm_silence(void);

/* Zellenfunktionen */

void sbfm_tremolo(byte zelle,bool on);
void sbfm_vibrato(byte zelle,bool on);
void sbfm_tonart(byte zelle,bool kontin);
void sbfm_huelldaempf(byte zelle,bool on);
void sbfm_zeitfakt(byte zelle,byte fakt);
void sbfm_ampldaempf(byte zelle,byte val);
void sbfm_daempfung(byte zelle,byte val);
void sbfm_attack(byte zelle,byte val);
void sbfm_decay(byte zelle,byte val);
void sbfm_sustain(byte zelle,byte val);
void sbfm_release(byte zelle,byte val);
void sbfm_welle(byte zelle,byte val);

/* Kanalfunktionen */

void sbfm_frequ(byte kanal,word frequ);
void sbfm_oktave(byte kanal,byte oktave);
void sbfm_ton(byte kanal,bool on);
void sbfm_verbind(byte kanal,bool parallel);
void sbfm_rueckkoppl(byte kanal,byte val);

/* komplexe Funktionen */

void sbfm_instrument_out(byte channel_number,instrument far *sb_inst);
void sbfm_key_off(byte channel,byte key,byte pitch);
void sbfm_key_on(byte channel,byte key,byte pitch);

#endif

#undef __fm_hrd
