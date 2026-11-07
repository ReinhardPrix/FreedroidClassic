#include "system.h"

#include "defs.h"
#include "global.h"
#include "proto.h"
#include "map.h"
#include <assert.h>

Level CurLevel;
ship curShip;
influence_t Me;
enemy AllEnemys[MAX_ENEMYS_ON_SHIP];
int NumEnemys;
SDL_Rect Block_Rect;
float LevelDoorsNotMovedTime;
float Time_For_Each_Phase_Of_Door_Movement;
static const char *file_override;

/* Link with map.c, level_editor.c, enemy.c and fd_memory.c using section GC. */
void DebugPrintf (int level, const char *fmt, ...)
{
  (void)level;
  (void)fmt;
}

void Terminate (int code) { exit (code); }
void InitInfluPositionHistory (void) {}
void DeleteBullet (int num) { (void)num; }
void DeleteBlast (int num) { (void)num; }

int MyRandom (int max)
{
  static unsigned int next;
  assert (max >= 0);
  return next++ % (max + 1);
}

char *find_file (const char *name, const char *subdir, int theme, int critical)
{
  static char path[1024];
  (void)theme;
  (void)critical;
  snprintf (path, sizeof(path), "%s/%s", subdir, name);
  return path;
}

char *ReadAndMallocAndTerminateFile (const char *name, const char *end)
{
  FILE *file;
  long size;
  char *data;
  if (file_override)
    {
      assert (strstr (file_override, end));
      data = MyMalloc (strlen(file_override) + 1);
      strcpy (data, file_override);
      return data;
    }
  file = fopen (name, "rb");
  assert (file);
  assert (fseek (file, 0, SEEK_END) == 0);
  size = ftell (file);
  assert (size >= 0 && fseek (file, 0, SEEK_SET) == 0);
  data = MyMalloc (size + 1);
  assert (fread (data, 1, size, file) == (size_t)size);
  fclose (file);
  data[size] = 0;
  assert (strstr (data, end));
  return data;
}

char *ReadAndMallocStringFromData (const char *data, const char *start, const char *end)
{
  const char *first = strstr (data, start);
  const char *last;
  char *result;
  assert (first);
  first += strlen (start);
  last = strstr (first, end);
  assert (last);
  result = MyMalloc (last - first + 1);
  memcpy (result, first, last - first);
  return result;
}

const char *LocateStringInData (const char *data, const char *label)
{
  const char *result = strstr (data, label);
  assert (result);
  return result;
}

void ReadValueFromString (const char *data, const char *label, const char *format, void *dst)
{
  const char *value = strstr (data, label);
  assert (value && sscanf (value + strlen(label), format, dst) == 1);
}

static void check_resize (void)
{
  level lev = {0};
  CurLevel = &lev;
  ResizeLevel (10, 10);
  lev.map[1][1] = V_GANZTUERE;
  lev.map[8][8] = H_ZUTUERE;
  lev.map[7][7] = REFRESH3;
  lev.map[9][9] = ALERT_RED;
  lev.num_waypoints = 3;
  lev.AllWaypoints[0].x = lev.AllWaypoints[0].y = 1;
  lev.AllWaypoints[1].x = lev.AllWaypoints[1].y = 8;
  lev.AllWaypoints[2].x = lev.AllWaypoints[2].y = 2;
  lev.AllWaypoints[0].num_connections = 2;
  lev.AllWaypoints[0].connections[0] = 1;
  lev.AllWaypoints[0].connections[1] = 2;
  NumEnemys = 3;
  for (int i = 0; i < NumEnemys; i++)
    {
      AllEnemys[i].status = MOBILE;
      AllEnemys[i].pos.x = AllEnemys[i].pos.y = 8;
    }
  ResetLevelMap (&lev);
  InterpretMap (&lev);
  assert (lev.doors[1].x == 8 && lev.refreshes[0].x == 7);
  Me.pos.x = Me.pos.y = 9;
  Me.speed.x = Me.speed.y = 1;
  ResizeLevel (6, 6);
  assert (lev.num_waypoints == 2);
  assert (lev.AllWaypoints[0].num_connections == 1);
  assert (lev.AllWaypoints[0].connections[0] == 1);
  assert (lev.doors[0].x == 1 && lev.doors[1].x == -1);
  assert (lev.refreshes[0].x == -1 && lev.alerts[0].x == -1);
  assert (Me.pos.x == 5 && Me.pos.y == 5 && Me.speed.x == 0);
  assert (AllEnemys[0].pos.x < 6 && AllEnemys[1].pos.x < 6);
  assert (AllEnemys[2].status == OUT);
  LevelDoorsNotMovedTime = 1;
  MoveLevelDoors ();
  ResizeLevel (10, 10);
  LevelDoorsNotMovedTime = 1;
  MoveLevelDoors ();
  for (int y = 0; y < 10; y++)
    for (int x = 0; x < 10; x++)
      if (x >= 6 || y >= 6)
        assert (lev.map[y][x] == VOID);
  ResizeLevel (1, 1);
  assert (lev.num_waypoints == 0 && lev.doors[0].x == -1);
  for (int i = 0; i < NumEnemys; i++)
    assert (AllEnemys[i].status == OUT);
  ResizeLevel (0, 0);
  assert (lev.xlen == 1 && lev.ylen == 1);
  ResizeLevel (10, 10);
  FreeLevelMemory (&lev);
}

static void check_level_input (int width, int height, const char *waypoints, bool valid)
{
  char data[16384];
  Level lev;
  snprintf (data, sizeof(data),
            "Levelnumber: 0\nxlen of this level: %d\nylen of this level: %d\n"
            "color of this level: 0\nName of this level=test\n"
            "Name of background song for this level=BYCOLOR\n"
            "Comment of the Influencer on entering this level=\"test\n"
            "begin_map\n0 0\n0 0\nbegin_waypoints\n%send_level\n",
            width, height, waypoints);
  lev = LevelToStruct (data);
  assert ((lev != NULL) == valid);
  if (lev)
    {
      FreeLevelMemory (lev);
      MyFree (lev);
    }
}

static void check_lift_input (int label, int x, int up, bool valid)
{
  char data[1024];
  snprintf (data, sizeof(data),
            "*** Beginning of elevator rectangles ***\n"
            "Elevator Number=0 ElRowX=0 ElRowY=0 ElRowW=1 ElRowH=1\n"
            "DeckNr=0 RectNumber=0 DeckX=0 DeckY=0 DeckW=1 DeckH=1\n"
            "*** Beginning of Lift Data ***\n"
            "Label=%d Deck=0 PosX=%d PosY=7 LevelUp=%d LevelDown=-1 LiftRow=0\n"
            "*** End of elevator specification file ***\n", label, x, up);
  file_override = data;
  assert ((GetLiftConnections ("test.elevators") == OK) == valid);
  file_override = NULL;
}

static void check_loading (void)
{
  assert (LoadShip ("Paradroid.maps") == OK);
  assert (GetLiftConnections ("Paradroid.elevators") == OK);
  assert (LiftDestinationValid (0));
  assert (!LiftDestinationValid (-1) && !LiftDestinationValid (MAX_LIFTS));
  check_lift_input (0, 17, -1, TRUE);
  check_lift_input (0, 38, -1, FALSE);
  check_lift_input (0, -1, -1, FALSE);
  check_lift_input (0, 17, 1, FALSE);
  check_lift_input (MAX_LIFTS, 17, -1, FALSE);
  assert (GetLiftConnections ("Paradroid.elevators") == OK);
  CurLevel = curShip.AllLevels[curShip.AllLifts[0].level];
  CurLevel->map[curShip.AllLifts[0].y][curShip.AllLifts[0].x] = FLOOR;
  assert (!LiftDestinationValid (0));
  assert (GetLiftConnections ("Paradroid.elevators") == ERR);
  FreeShipMemory ();
  FreeShipMemory ();
}

int main (void)
{
  check_resize ();
  check_level_input (2, 2, "", TRUE);
  check_level_input (0, 2, "", FALSE);
  check_level_input (2, MAX_MAP_ROWS + 1, "", FALSE);
  check_level_input (2, 2, "Nr.=0 x=2 y=0 connections: \n", FALSE);
  check_level_input (2, 2, "Nr.=0 x=-1 y=0 connections: \n", FALSE);
  check_level_input (2, 2, "Nr.=0 x=256 y=0 connections: \n", FALSE);
  check_level_input (2, 2, "Nr.=0 x=0 y=0 connections: 1\n", FALSE);
  check_level_input (2, 2, "Nr.=0 x=0 y=0 connections: 0\n", TRUE);
  check_level_input (2, 2, "Nr.=0 x=0 y=0\n", FALSE);
  check_loading ();
  assert (LoadShip ("Paradroid.maps") == OK);
  FreeShipMemory ();
  MyCheckMemoryLeaks ();
  return 0;
}
