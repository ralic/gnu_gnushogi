/*
 * init.c - C source for GNU SHOGI
 *
 * Copyright (c) 1993 Matthias Mutz
 *
 * GNU SHOGI is based on GNU CHESS
 *
 * Copyright (c) 1988,1989,1990 John Stanback
 * Copyright (c) 1992 Free Software Foundation
 *
 * This file is part of GNU SHOGI.
 *
 * GNU Shogi is free software; you can redistribute it and/or modify
 * it under the terms of the GNU General Public License as published by
 * the Free Software Foundation; either version 1, or (at your option)
 * any later version.
 *
 * GNU Shogi is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 * GNU General Public License for more details.
 *
 * You should have received a copy of the GNU General Public License
 * along with GNU Shogi; see the file COPYING.  If not, write to
 * the Free Software Foundation, 675 Mass Ave, Cambridge, MA 02139, USA.
 */
 
#include "gnushogi.h"

#if defined HASGETTIMEOFDAY && !defined THINK_C
#include <sys/time.h>
#endif

extern unsigned int TTadd;

/* .... MOVE GENERATION VARIABLES AND INITIALIZATIONS .... */


#if !defined K32SEGMENTS
#include "init_data1.c"
#include "init_data2.c"
#include "init_data3.c"
#include "init_data4.c"
#include "init_data1b.c"
#include "init_data2b.c"
#include "init_data3b.c"
#include "init_data4b.c"
#endif


#ifdef USE_PATTERN
#include "pattern.h"
#endif


#ifdef THINK_C                
#define abs(a) (((a)<0)?-(a):(a))
#endif
#define max(a,b) (((a)<(b))?(b):(a))
#define odd(a) ((a) & 1)



const small_short piece_of_ptype[NO_PTYPE_PIECES] =
{ pawn, lance, knight, silver, gold, bishop, rook, pbishop, prook, king,
    pawn, lance, knight, silver, gold };


const small_short side_of_ptype[NO_PTYPE_PIECES] =
{ black, black, black, black, black, black, black, black, black, black,
    white, white, white, white, white };



short
ptype_distance (short ptyp, short f, short t)

/*
 * Determine the minimum number of moves for a piece from
 * square "f" to square "t". If the piece cannot reach "t",
 * the count is set to CANNOT_REACH.
 */

#define csquare(sq) ((side == black) ? sq : (NO_SQUARES-1-sq))
#define crow(sq) row(csquare(sq))
#define ccol(sq) column(csquare(sq))

{
  short side, piece;
  short colf, colt, rowf, rowt, dcol, drow;

  if ( f == t )
    return (0);

  piece = piece_of_ptype[ptyp];
  side  = side_of_ptype[ptyp];

  dcol = (colt = ccol(t)) - (colf = ccol(f));
  drow = (rowt = crow(t)) - (rowf = crow(f));

  switch ( piece ) {

    case pawn:
	if ( (dcol != 0) || (drow != 1) )
	  return (CANNOT_REACH);
	else
	  return (1);

    case lance:
	if ( (dcol != 0) || (drow < 1) )
	  return (CANNOT_REACH);
	else
	  return (drow);

    case knight:
	if ( odd(drow) || (odd(drow / 2) != odd(dcol)) )
	  return (CANNOT_REACH);
	else if ( (drow == 0) || ((drow / 2) < abs(dcol)) )
	  return (CANNOT_REACH);
	else
	  return (drow / 2);

    case silver:
	if ( odd(drow) == odd(dcol) )
	  return max(abs(drow),abs(dcol));
	else if ( drow > 0 )
	  if ( abs(dcol) <= drow )
	    return (drow);
	  else
	    return (max(abs(drow),abs(dcol))+1);
	else if ( dcol == 0 )
	  return (2-drow);
	else
	  return (max(abs(drow),abs(dcol))+1);

    case gold:
    case ppawn:
    case pknight:
    case plance:
    case psilver:
	if ( abs(dcol) == 0 )
	  return (abs(drow));
	else if ( drow >= 0 )
	  return max(drow,abs(dcol));
	else
	  return (abs(dcol)-drow);

    case bishop:
	if ( odd(dcol) != odd(drow) )
	  return (CANNOT_REACH);
	else
	  return ((abs(dcol) == abs(drow)) ? 1 : 2);

    case pbishop:
	if ( odd(dcol) != odd(drow) )
	  if ( (abs(dcol) <= 1) && (abs(drow) <= 1) )
	    return (1);
	  else if ( abs(abs(dcol) - abs(drow)) == 1 )
	    return (2);
	  else
	    return (3);
	else
	  return ((abs(dcol) == abs(drow)) ? 1 : 2);

    case rook:
	if ( (dcol == 0) || (drow == 0) )
	  return (1);
	else
	  return (2);

    case prook:
	if ( (dcol == 0) || (drow == 0) )
	  return (1);
	else if ( (abs(dcol) == 1) && (abs(drow) == 1) )
	  return (1);
	else
	  return (2);

    case king:
	return max(abs(drow),abs(dcol));

    default:
	/* should never occur */
	return (CANNOT_REACH);

  }

}


#ifndef SAVE_PTYPE_DISTDATA
distdata_array *ptype_distdata[NO_PTYPE_PIECES];
#endif


void
Initialize_dist (void)
{
  register short a, b, d, di, ptyp;
#ifndef SAVE_DISTDATA
  for (a = 0; a < NO_SQUARES; a++)
    for (b = 0; b < NO_SQUARES; b++)
      {
	d = abs (column (a) - column (b));
	di = abs (row (a) - row (b));
	distdata[a][b] = (small_short)((d > di) ? d : di);
      }
#endif
#ifndef SAVE_PTYPE_DISTDATA
  for (ptyp = 0; ptyp < NO_PTYPE_PIECES; ptyp++)
    {
      if ( (ptype_distdata[ptyp] = (distdata_array *) malloc (sizeof(next_array))) == NULL )
        {
          ShowMessage("cannot allocate ptype_distdata space...");
          exit(1);
        }
      for (a = 0; a < NO_SQUARES; a++)
        for (b = 0; b < NO_SQUARES; b++)
          (*ptype_distdata[ptyp])[a][b] = ptype_distance(ptyp,a,b);
    }
#endif
}


/*
 * nextpos[piece][from-square] , nextdir[piece][from-square] gives vector of
 * positions reachable from from-square in ppos with piece such that the
 * sequence	ppos = nextpos[piece][from-square]; pdir =
 * nextdir[piece][from-square]; u = ppos[sq]; do { u = ppos[u]; if(color[u]
 * != neutral) u = pdir[u]; } while (sq != u); will generate the sequence of
 * all squares reachable from sq.
 *
 * If the path is blocked u = pdir[sq] will generate the continuation of the
 * sequence in other directions.
 */


extern next_array pawn_nextdir, lance_nextdir, knight_nextdir;
extern next_array silver_nextdir, gold_nextdir, bishop_nextdir;
extern next_array rook_nextdir, pbishop_nextdir, prook_nextdir;
extern next_array king_nextdir, wpawn_nextdir, wlance_nextdir;
extern next_array wknight_nextdir, wsilver_nextdir, wgold_nextdir;

extern next_array pawn_nextpos, lance_nextpos, knight_nextpos;
extern next_array silver_nextpos, gold_nextpos, bishop_nextpos;
extern next_array rook_nextpos, pbishop_nextpos, prook_nextpos;
extern next_array king_nextpos, wpawn_nextpos, wlance_nextpos;
extern next_array wknight_nextpos, wsilver_nextpos, wgold_nextpos;



next_array *nextdir[NO_PTYPE_PIECES] = 
{ &pawn_nextdir, &lance_nextdir, &knight_nextdir, 
  &silver_nextdir, &gold_nextdir, &bishop_nextdir, &rook_nextdir,
  &pbishop_nextdir, &prook_nextdir,
  &king_nextdir, &wpawn_nextdir, &wlance_nextdir, &wknight_nextdir,
  &wsilver_nextdir, &wgold_nextdir };

next_array *nextpos[NO_PTYPE_PIECES] = 
{ &pawn_nextpos, &lance_nextpos, &knight_nextpos, 
  &silver_nextpos, &gold_nextpos, &bishop_nextpos, &rook_nextpos,
  &pbishop_nextpos, &prook_nextpos,
  &king_nextpos, &wpawn_nextpos, &wlance_nextpos, &wknight_nextpos,
  &wsilver_nextpos, &wgold_nextpos };

/*
 * ptype is used to separate black and white pawns, like this; ptyp =
 * ptype[side][piece] piece can be used directly in nextpos/nextdir when
 * generating moves for pieces that are not white pawns.
 */

const small_short ptype[2][NO_PIECES] =
{ ptype_no_piece, ptype_pawn, ptype_lance, ptype_knight, 
    ptype_silver, ptype_gold, ptype_bishop, ptype_rook,
    ptype_gold, ptype_gold, ptype_gold, ptype_gold, 
    ptype_pbishop, ptype_prook, ptype_king,
  ptype_no_piece, ptype_wpawn, ptype_wlance, ptype_wknight, 
    ptype_wsilver, ptype_wgold, ptype_bishop, ptype_rook,
    ptype_wgold, ptype_wgold, ptype_wgold, ptype_wgold, 
    ptype_pbishop, ptype_prook, ptype_king};

const small_short promoted[NO_PIECES] =
{ no_piece, ppawn, plance, pknight, psilver, gold, pbishop, prook,
    ppawn, plance, pknight, psilver, pbishop, prook, king };
    
const small_short unpromoted[NO_PIECES] =
{ no_piece, pawn, lance, knight, silver, gold, bishop, rook,
    pawn, lance, knight, silver, bishop, rook, king };
    
const small_short is_promoted[NO_PIECES] =
{ false, false, false, false, false, false, false, false,
    true, true, true, true, true, true, false };

/* data used to generate nextpos/nextdir */
static const small_short direc[NO_PTYPE_PIECES][8] =
{
   11,  0,  0,  0,  0,  0,  0,  0 ,   /*  0 ptype_pawn */
   11,  0,  0,  0,  0,  0,  0,  0 ,   /*  1 ptype_lance */
   21, 23,  0,  0,  0,  0,  0,  0 ,   /*  2 ptype_knight */
   10, 11, 12,-12,-10,  0,  0,  0 ,   /*  3 ptype_silver */
   10, 11, 12, -1,  1,-11,  0,  0 ,   /*  4 ptype_gold */
   10, 12,-12,-10,  0,  0,  0,  0 ,   /*  5 ptype_bishop */
   11, -1,  1,-11,  0,  0,  0,  0 ,   /*  6 ptype_rook */
   10, 11, 12, -1,  1,-12,-11,-10 ,   /*  7 ptype_pbishop */
   10, 11, 12, -1,  1,-12,-11,-10 ,   /*  8 ptype_prook */
   10, 11, 12, -1,  1,-12,-11,-10 ,   /*  9 ptype_king */
  -11,  0,  0,  0,  0,  0,  0,  0 ,   /* 10 ptype_wpawn */
  -11,  0,  0,  0,  0,  0,  0,  0 ,   /* 11 ptype_wlance */
  -21,-23,  0,  0,  0,  0,  0,  0 ,   /* 12 ptype_wknight */
  -10,-11,-12, 12, 10,  0,  0,  0 ,   /* 13 ptype_wsilver */
  -10,-11,-12,  1, -1, 11,  0,  0 };  /* 14 ptype_wgold */


small_short diagonal(short d) 
{ return(abs(d) == 10 || abs(d) == 12);
}

   
static const small_short max_steps[NO_PTYPE_PIECES] = 
{1, 8, 1, 1, 1, 8, 8, 8, 8, 1, 1, 8, 1, 1, 1 };

static const small_short nunmap[(NO_COLS+2)*(NO_ROWS+4)] =
{
  -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1,
  -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, 
  -1,  0,  1,  2,  3,  4,  5,  6,  7,  8, -1,
  -1,  9, 10, 11, 12, 13, 14, 15, 16, 17, -1,
  -1, 18, 19, 20, 21, 22, 23, 24, 25, 26, -1,
  -1, 27, 28, 29, 30, 31, 32, 33, 34, 35, -1,
  -1, 36, 37, 38, 39, 40, 41, 42, 43, 44, -1,
  -1, 45, 46, 47, 48, 49, 50, 51, 52, 53, -1,
  -1, 54, 55, 56, 57, 58, 59, 60, 61, 62, -1,
  -1, 63, 64, 65, 66, 67, 68, 69, 70, 71, -1,
  -1, 72, 73, 74, 75, 76, 77, 78, 79, 80, -1,
  -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1,
  -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1};


int InitFlag = false;

void
Initialize_moves (void)

/*
 * This procedure pre-calculates all moves for every piece from every square.
 * This data is stored in nextpos/nextdir and used later in the move
 * generation routines.
 */

{
  short ptyp, po, p0, d, di, s, delta;
  unsigned char *ppos, *pdir;       
  short dest[8][9];
  short sorted[9];              
  short steps[8];
  short fpo=23,tpo=120;

  for (ptyp = 0; ptyp < NO_PTYPE_PIECES; ptyp++)
    for (po = 0; po < NO_SQUARES; po++)
      for (p0 = 0; p0 < NO_SQUARES; p0++)
	{ 
	  (*nextpos[ptyp])[po][p0] = (unsigned char) po;
	  (*nextdir[ptyp])[po][p0] = (unsigned char) po;
	}
	
  for (ptyp = 0; ptyp < NO_PTYPE_PIECES; ptyp++) 
    for (po = fpo; po < tpo; po++)
      if (nunmap[po] >= 0)
	{ 
	  ppos = (*nextpos[ptyp])[nunmap[po]];
	  pdir = (*nextdir[ptyp])[nunmap[po]];
	  /* dest is a function of direction and steps */
	  for (d = 0; d < 8; d++)
	    {
	      dest[d][0] = nunmap[po];
	      delta = direc[ptyp][d];
	      if (delta != 0)
		{
		  p0 = po;
		  for (s = 0; s < max_steps[ptyp]; s++)
		    {
		      p0 = p0 + delta;

		      /*
		       * break if (off board) or (promoted rooks wishes to 
                       * move two steps diagonal) or (promoted
                       * bishops wishes to move two steps non-diagonal) 
		       */                     
		      if ( (nunmap[p0] < 0) ||
                           ((ptyp == ptype_prook) && (s > 0) && diagonal(delta)) ||
                           ((ptyp == ptype_pbishop) && (s > 0) && !diagonal(delta)) )
			break;
		      else
			dest[d][s] = nunmap[p0];
		    }
		}
	      else
		s = 0;

	      /*
	       * sort dest in number of steps order currently no sort
	       * is done due to compability with the move generation
	       * order in old gnu chess
	       */
	      steps[d] = s;
	      for (di = d; s > 0 && di > 0; di--)
		if (steps[sorted[di - 1]] == 0)	/* should be: < s */
		  sorted[di] = sorted[di - 1];
		else
		  break;
	      sorted[di] = d;
	    }

	  /*
	   * update nextpos/nextdir
	   */
	  p0 = nunmap[po];
	  pdir[p0] = (unsigned char) dest[sorted[0]][0];
	  for (d = 0; d < 8; d++)
	      for (s = 0; s < steps[sorted[d]]; s++)
		  {
		    ppos[p0] = (unsigned char) dest[sorted[d]][s];
		    p0 = dest[sorted[d]][s];
		    if (d < 7)
		      pdir[p0] = (unsigned char) dest[sorted[d + 1]][0];

		    /*
		     * else is already initialized
		     */
		  }
	}
}


void
NewGame (void)

/*
 * Reset the board and other variables to start a new game.
 */

{
  short l, c, p;
#ifdef HASGETTIMEOFDAY
  struct timeval tv;
#endif
  compptr = oppptr = -1;
  stage = stage2 = -1;		/* the game is not yet started */
  flag.illegal = flag.mate = flag.post = flag.quit = flag.reverse = flag.bothsides = flag.onemove = flag.force = false;
  flag.material = flag.coords = flag.hash = flag.easy = flag.beep = flag.rcptr = true;
  flag.stars = flag.shade = flag.back = flag.musttimeout = false;
  flag.gamein = false;
#if defined(MSDOS) && !defined(SEVENBIT)
  flag.rv = false;
#else
  flag.rv = true;
#endif /* MSDOS && !SEVENBIT */
  mycnt1 = mycnt2 = 0;
  GenCnt = NodeCnt = et0 = dither =  XCmore = 0;
  WAwindow = WAWNDW;
  WBwindow = WBWNDW;
  BAwindow = BAWNDW;
  BBwindow = BBWNDW;
  xwndw = BXWNDW;
  if (!MaxSearchDepth)
    MaxSearchDepth = MAXDEPTH - 1;
  contempt = 0;
  GameCnt = 0;
  Game50 = 1;
  CptrFlag[0] = false;
  hint = OPENING_HINT;
  ZeroRPT ();
  GameType[0] = GameType[1] = UNKNOWN;
  Pscore[0] = Tscore[0] = (SCORE_LIMIT+3000);
  opponent = black;
  computer = white;
  for (l = 0; l < TREE; l++)
    Tree[l].f = Tree[l].t = 0;
  gsrand ((unsigned int) 1);
  if (!InitFlag)
    {
      for (c = black; c <= white; c++)
	for (p = pawn; p <= king; p++)
	  for (l = 0; l < (NO_SQUARES+(2*NO_PIECES)); l++)
	    {
	      hashcode[c][p][l].key = (((unsigned long) urand ()));
	      hashcode[c][p][l].key += (((unsigned long) urand ()) << 16);
	      hashcode[c][p][l].bd = (((unsigned long) urand ()));
	      hashcode[c][p][l].bd += (((unsigned long) urand ()) << 16);
#ifdef LONG64
	      hashcode[c][p][l].key += (((unsigned long) urand ()) << 32);
	      hashcode[c][p][l].key += (((unsigned long) urand ()) << 48);
	      hashcode[c][p][l].bd += (((unsigned long) urand ()) << 32);
	      hashcode[c][p][l].bd += (((unsigned long) urand ()) << 48);
#endif
	    }
    }
  for (l = 0; l < NO_SQUARES; l++)
    {
      board[l] = Stboard[l];
      color[l] = Stcolor[l];
      Mvboard[l] = 0;
    }
  ClearCaptured ();
  ClrScreen ();
  InitializeStats ();
#ifdef HASGETTIMEOFDAY
  gettimeofday(&tv, NULL);
  time0 = tv.tv_sec*100+tv.tv_usec/10000;
#else
  time0 = time ((long *) 0);
#endif
  ElapsedTime (1);
  flag.regularstart = true;
  Book = BOOKFAIL;
  if (!InitFlag)
    {
	char sx[256];
	strcpy(sx,"level");
      if (TCflag)
	SetTimeControl ();
      else if (MaxResponseTime == 0)
	SelectLevel (sx);
      UpdateDisplay (0, 0, 1, 0);
      GetOpenings ();
#ifdef USE_PATTERN
      GetOpeningPatterns ();
      /* ShowOpeningPatterns (); */
#endif
#if ttblsz
      Initialize_ttable();
#endif
      InitFlag = true;
    }
  hashbd = hashkey = 0;
#if ttblsz
  ZeroTTable ();
  TTadd = 0;
#endif /* ttblsz */
}

void
InitConst (char *lang)
{
  FILE *constfile;
  char s[256];
  char sl[5];
  int len, entry;
  char *p, *q;
  constfile = fopen (LANGFILE, "r");
  if (!constfile)
    {
      printf ("NO LANGFILE\n");
      exit (1);
    }
  while (fgets (s, sizeof (s), constfile))
    {
      if (s[0] == '!')
	continue;
      len = strlen (s);
      for (q = &s[len]; q > &s[8]; q--)
	if (*q == '}')
	  break;
      if (q == &s[8])
	{
	  printf ("{ error in cinstfile\n");
	  exit (1);
	}
      *q = '\0';
      if (s[3] != ':' || s[7] != ':' || s[8] != '{')
	{
	  printf ("Langfile format error %s\n", s);
	  exit (1);
	}
      s[3] = s[7] = '\0';
      if (lang == NULL)
	{
	  lang = sl;
	  strcpy (sl, &s[4]);
	}
      if (strcmp (&s[4], lang))
	continue;
      entry = atoi (s);
      if (entry < 0 || entry >= CPSIZE)
	{
	  printf ("Langfile number error\n");
	  exit (1);
	}
      for (q = p = &s[9]; *p; p++)
	{
	  if (*p != '\\')
	    {
	      *q++ = *p;
	    }
	  else if (*(p + 1) == 'n')
	    {
	      *q++ = '\n';
	      p++;
	    }
	}
      *q = '\0';
      if (entry < 0 || entry > 255)
	{
	  printf ("Langfile error %d\n", entry);
	  exit (0);
	}
      CP[entry] = (char *) malloc ((unsigned) strlen (&s[9]) + 1);
      if (CP[entry] == NULL)
	{
	  perror ("malloc");
	  exit (0);
	}
      strcpy (CP[entry], &s[9]);

    }
  fclose (constfile);
}

#if ttblsz
void

Initialize_ttable ()
{
  int doit = true;

  if (rehash < 0)
    rehash = MAXrehash;

  while ( doit && ttblsize > 0 ) {
    ttable[0] = (struct hashentry *)malloc((unsigned)(sizeof(struct hashentry))*(ttblsize+rehash));
    ttable[1] = (struct hashentry *)malloc((unsigned)(sizeof(struct hashentry))*(ttblsize+rehash));
    if (ttable[0] == NULL || ttable[1] == NULL) {
      if (ttable[0] != NULL) free(ttable[0]);
      if (ttable[1] != NULL) free(ttable[1]);
      ttblsize = ttblsize >> 1;
    } else doit = false;
  }

  if (ttable[0] == NULL || ttable[1] == NULL) {
    perror("memory alloc");
    exit(1);
  }

#if !defined BAREBONES && defined NONDSP
  { char s[80];
    sprintf(s,"transposition table is %d",ttblsize);
    ShowMessage(s);
  }
#endif
}

#endif /* ttblsz */

