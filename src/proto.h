/* *********************************************************************** */
/* *************************  P R O T O T Y P E N  *********************** */
/* *********************************************************************** */
#ifndef _proto_h
#define _proto_h

/* paraplus.c */
#undef EXTERN
#ifdef _paraplus_c
#define EXTERN
#else
#define EXTERN extern
#endif

EXTERN void CalibratedDelay(long);
EXTERN int ShipEmptyCounter;
EXTERN int WaitElevatorCounter;
EXTERN int TimerFlag;
EXTERN int LeftPressed(void);
EXTERN int RightPressed(void);
EXTERN int UpPressed(void);
EXTERN int DownPressed(void);
EXTERN int SpacePressed(void);
EXTERN int QPressed(void);
EXTERN int WPressed(void);
EXTERN void reverse(char*);
EXTERN char* itoa(int,char*,int);
EXTERN char* ltoa(long,char*,int);
EXTERN void delay(int);
EXTERN void gotoxy(int,int);
EXTERN int kbhit(void);
EXTERN int coreleft(void);
EXTERN int MyRandom(int);
EXTERN void Title(void);
EXTERN void InitParaplus(void);
EXTERN void InitNewGame(void);
EXTERN int InitPictures(void);
EXTERN int NoDirectionPressed(void);
EXTERN void PutEnergy(void);
EXTERN void ThouArtDefeated(void);
EXTERN void ThouArtVictorious(void);
EXTERN void StandardEnergieLieferungen(void);

/* influ.c */
#undef EXTERN
#ifdef _influ_c
#define EXTERN
#else
#define EXTERN extern
#endif
EXTERN void MoveInfluence(void);
EXTERN void AdjustSpeed(void);
EXTERN void BounceInfluencer(void);
EXTERN void Reibung(void);
EXTERN void CheckEnergieLevel(void);
EXTERN void AnimateInfluence(void);
EXTERN void InfluenceEnemyCollision(void);
EXTERN void RefreshInfluencer(void);
EXTERN void ExplodeInfluencer(void);

/* bullet.c */
#undef EXTERN
#ifdef _bullet_c
#define EXTERN
#else
#define EXTERN extern
#endif
EXTERN void FireBullet(void);
EXTERN void MoveBullets(void);
EXTERN void DeleteBullet(int num);
EXTERN void StartBlast(int x, int y, int type);
EXTERN void ExplodeBlasts(void);
EXTERN void DeleteBlast(int num);
EXTERN void CheckBulletCollisions(int num);
EXTERN void CheckBlastCollisions(int num);

/* view.c */
#undef EXTERN
#ifdef _view_c
#define EXTERN
#else
#define EXTERN extern
#endif
EXTERN unsigned char *FeindZusammenstellen(const char*,int);
// EXTERN void SetColors(int,int,char*);
EXTERN void RotateBulletColor(void);
EXTERN void GetView(void);
EXTERN void ShowPosition(void);
EXTERN void DisplayView(void);
EXTERN void GetInternFenster(void);
EXTERN void PutInfluence(void);
EXTERN void PutBullet(int);
EXTERN void PutBlast(int);
EXTERN void PutEnemy(int);
EXTERN void PutInternFenster(void);
EXTERN int PutObject(int x, int y, unsigned char *pic, int check );
EXTERN void DrawDigit(unsigned char*,unsigned char*);
EXTERN void RedrawInfluenceNumber(void);
EXTERN void SetUserfenster(int color, unsigned char *screen);
EXTERN void ShowRobotPicture(int PosX,int PosY,int Number,unsigned char* Screen);


/* blocks.c */
#undef EXTERN
#ifdef _blocks_c
#define EXTERN
#else
#define EXTERN extern
#endif

EXTERN void SmallBlock(int,int,int,unsigned char*,int);
EXTERN void SmallEnemy(int,int,int,unsigned char*,int);
EXTERN void SmallBlast(int,int,int,int,unsigned char*,int);
EXTERN void SmallBullet(int,int,int,int,unsigned char*,int);

EXTERN void GetMapBlocks(void);
EXTERN void GetShieldBlocks(void);
EXTERN void IsolateBlock(
	unsigned char *screen,
	unsigned char *target,
	int BlockEckLinks,
	int BlockEckOben,
	int Blockbreite,
	int Blockhoehe);
	
EXTERN unsigned char *GetBlocks(char *picfile, int line, int num);
EXTERN int MergeBlockToWindow(
	register unsigned char *source,
	register unsigned char *target,
	int WinLineLen,
	int check);

EXTERN void GetDigits(void);

EXTERN void DisplayBlock(int x, int y,
	unsigned char *block,
	int len, int height,
	unsigned char *screen);


EXTERN void DisplayMergeBlock(int x, int y,
	unsigned char *block,
	int len, int height,
	unsigned char *screen);

EXTERN void CopyMergeBlock(
	unsigned char *target,
	unsigned char *source,
	int mem);

EXTERN int MergeBlockToWindow(
	unsigned char*,
	unsigned char*,
	int WinLineLen,
	int check);

/* graphics.c */
#undef EXTERN
#ifdef _graphics_c
#define EXTERN
#else
#define EXTERN extern
#endif

EXTERN void SwapScreen(void);
EXTERN void ClearVGAScreen(void);
EXTERN void Monitorsignalunterbrechung(int);
EXTERN void SetPalCol(unsigned int palpos,	unsigned char rot, unsigned char gruen, unsigned char blau );
EXTERN void SetPalCol2(unsigned int palpos, color Farbwert);
EXTERN int InitLevelColorTable(void);
EXTERN int InitPalette(void);
EXTERN void SetLevelColor(int);
EXTERN void LadeLBMBild(char* LBMDateiname,unsigned char* Screen,int LoadPal);
EXTERN void TransparentLadeLBMBild(char* LBMDateiname,unsigned char* Screen,int LoadPal);
EXTERN void SetVideoMode(int Videomodus);
EXTERN void WaitVRetrace(void);
EXTERN void UnfadeLevel(void);
EXTERN void FadeLevel(void);
EXTERN void FadeColors1(void);
EXTERN void FadeColors2(void);
EXTERN void LadeZeichensatz(char* Zeichensatzname);
EXTERN void RotateColors(int,int);
EXTERN void LevelGrauFaerben(void);
EXTERN void ClearGraphMem(unsigned char *screen);
EXTERN void SaveVideoMode(void);
EXTERN void RestoreVideoMode(void);
EXTERN void Flimmern(void);

/* map.c */
#undef EXTERN
#ifdef _map_c
#define EXTERN
#else
#define EXTERN extern
#endif
EXTERN unsigned char GetMapBrick(Level deck, int x, int y);
EXTERN int GetCurrentElevator(void);
EXTERN void ActSpecialField(int x, int y);

EXTERN int LoadShip(char *shipname);
EXTERN Level LevelToStruct(char *data);
EXTERN int GetDoors(Level Lev);
EXTERN int GetWaypoints(Level Lev);
EXTERN int GetRefreshes(Level Lev); 
EXTERN int GetElevatorConnections(char *shipname);
EXTERN int GetCrew(char *shipname);

EXTERN void AnimateRefresh(void);
EXTERN void MoveLevelDoors(void);
EXTERN int IsPassable(int x, int y, int Checkpos);
EXTERN int DruidPassable(int x, int y);
EXTERN int IsVisible(Point objpos);
EXTERN int TranslateMap(Level Lev);

/* sound.c */
#undef EXTERN
#ifdef _sound_c
#define EXTERN
#else
#define EXTERN extern
#endif
EXTERN void StartSound(int);
EXTERN void out_sb(unsigned char, unsigned char);
EXTERN unsigned char in_sb(unsigned char);
EXTERN int Init_OSS(void);
EXTERN void GotHitSound(void);
EXTERN void GotIntoBlastSound(void);
EXTERN void CrySound(void);
EXTERN void RefreshSound(void);
EXTERN void MoveElevatorSound(void);
EXTERN void EnterElevatorSound(void);
EXTERN void LeaveElevatorSound(void);
EXTERN void FireBulletSound(void);
EXTERN void BounceSound(void);
EXTERN int InitModPlayer(void);
EXTERN void StopModPlayer(void);
EXTERN void PlayMod(char *modfile);
EXTERN void sbfm_silence(void); 

/* misc.c */
#undef EXTERN
#ifdef _misc_c
#define EXTERN
#else
#define EXTERN extern
#endif

EXTERN void Armageddon(void);
EXTERN void Teleport(int LNum,int X,int Y);
EXTERN void Cheatmenu(void);
EXTERN void InsertNewMessage(void);
EXTERN void Terminate(int);
EXTERN void KillQueue(void);
EXTERN void PutMessages(void);
EXTERN void InsertMessage(char* MText);
EXTERN void* MyMemcpy(void*,void*,unsigned int);
EXTERN void* MyMalloc(long);
EXTERN int SetTypematicRate(unsigned char);
EXTERN void DirToVect(int dir, Vect vector);
EXTERN void JoystickControl(void);
EXTERN void TurnIntVects(void);
EXTERN void RestoreIntVects(void);
EXTERN void KillTastaturPuffer(void);

EXTERN long my_sqrt(long);
EXTERN int my_abs(int);

EXTERN void ShowDebugInfos(void);

/* enemy.c */
#undef EXTERN
#ifdef _enemy_c
#define EXTERN
#else
#define EXTERN extern
#endif
EXTERN void InitEnemys(void);
EXTERN void ShuffleEnemys(void);
EXTERN int EnemyEnemyCollision(int enemynum);
EXTERN void MoveEnemys(void);
EXTERN void AttackInfluence(int enemynum);
EXTERN void AnimateEnemys(void);
EXTERN void ClearEnemys(void);
EXTERN int ClassOfDruid(int druidtype);

/* ship.c */
#undef EXTERN
#ifdef _ship_c
#define EXTERN
#else
#define EXTERN extern
#endif
EXTERN void EnterElevator(void);
EXTERN void EnterKonsole(void);
EXTERN void AlleLevelsGleichFaerben(void);
EXTERN void HilightLevel(int);
EXTERN void HilightElevator(int);
EXTERN int LevelEmpty(void);
EXTERN int ShipEmpty(void);
EXTERN void ClearUserFenster(void);


/* paratext.c */
#undef EXTERN
#ifdef _paratext_c
#define EXTERN
#else
#define EXTERN extern
#endif

EXTERN void StoreTextEnvironment(void);
EXTERN void RestoreTextEnvironment(void);



EXTERN int InitParaplusFont(void);

EXTERN void SetTextColor(unsigned bg, unsigned fg);
EXTERN void GetTextColor(unsigned int* bg, unsigned int* fg);

EXTERN void SetTextBorder(int left, int upper, int right, int lower, int chars);
EXTERN void ClearTextBorder(unsigned char *screen, int color);
EXTERN void SetTextCursor(int x, int y);
EXTERN void SetLineLength(int);

EXTERN void DisplayText(char *text, int startx, int starty, unsigned char *screen,int EnterCursor);
EXTERN void DisplayChar(unsigned char Zeichen, unsigned char *screen);
EXTERN int ScrollText(char *text, int startx, int starty, int EndLine);

EXTERN void CheckUmbruch(void);
EXTERN void MakeUmbruch(void);
EXTERN int FensterVoll(void);
EXTERN char *PreviousLine(char *textstart, char *text);
EXTERN char *NextLine(char *text);
EXTERN char* GetString(int);



/* rahmen.c */
#undef EXTERN
#ifdef _rahmen_c
#define EXTERN
#else
#define EXTERN extern
#endif
EXTERN void SayLeftInfo( char *text, unsigned char *screen);
EXTERN void SayRightInfo(char *text, unsigned char *screen);
EXTERN void DisplayRahmen(unsigned char *screen);
EXTERN void SetInfoline(void);
EXTERN void UpdateInfoline(void);


/* takeover.c */
#undef EXTERN
#ifdef _takeover_c
#define EXTERN
#else
#define EXTERN extern
#endif

EXTERN int Takeover(int enemynum);
EXTERN void ChooseColor(void);
EXTERN void PlayGame(void);
EXTERN void EnemyMovements(void);

EXTERN int GetTakeoverGraphics(void);
EXTERN void ShowPlayground(void);
EXTERN void InventPlayground(void);

EXTERN void ProcessPlayground(void);
EXTERN void ProcessDisplayColumn(void);
EXTERN void ProcessCapsules(void);

EXTERN void ClearPlayground(void);
EXTERN int IsActive(int color, int row);
EXTERN void InitTakeover(void);


#endif

