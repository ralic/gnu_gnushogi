/*
 * FILE: tcontrl.h
 *
 * ----------------------------------------------------------------------
 * Copyright (c) 1993, 1994, 1995 Matthias Mutz
 * Copyright (c) 1999 Michael Vanier and the Free Software Foundation
 * Copyright (c) 2008, 2013, 2014 Yann Dirson and the Free Software Foundation
 *
 * GNU SHOGI is based on GNU CHESS
 *
 * Copyright (c) 1988, 1989, 1990 John Stanback
 * Copyright (c) 1992 Free Software Foundation
 *
 * This file is part of GNU SHOGI.
 *
 * GNU Shogi is free software; you can redistribute it and/or modify it
 * under the terms of the GNU General Public License as published by the
 * Free Software Foundation; either version 3 of the License,
 * or (at your option) any later version.
 *
 * GNU Shogi is distributed in the hope that it will be useful, but WITHOUT
 * ANY WARRANTY; without even the implied warranty of MERCHANTABILITY or
 * FITNESS FOR A PARTICULAR PURPOSE.  See the GNU General Public License
 * for more details.
 *
 * You should have received a copy of the GNU General Public License along
 * with GNU Shogi; see the file COPYING. If not, see
 * <http://www.gnu.org/licenses/>.
 * ----------------------------------------------------------------------
 *
 */

/*
 * Recording of the amounts of time used during the last MINGAMEIN moves.
 */
#define MINGAMEIN  4
extern int   timecomp[MINGAMEIN], timeopp[MINGAMEIN];
extern int   compptr, oppptr;

extern long  time0;             /* time of start of turn */
extern long  et; /* "elapsed time": centiseconds elapsed since start of turn */

/* current time controls */
struct TimeControlRec
{
    short moves[2];             /* # of moves to make before end of current interval */
    long  clock[2];             /* centiseconds before end of current interval */
};
extern struct TimeControlRec TimeControl;

/* TimeControl initial values as used by SetTimeControl */
extern short TCmoves, TCminutes, TCseconds;

extern int   TCadd;             /* Fischer clock increment */
extern bool  TCflag;            /* wether timecontrol is active */

extern short OperatorTime;      /* FIXME: ? */

/*
 * Time contro functions
 */

typedef enum
{
    COMPUTE_AND_INIT_MODE = 1, COMPUTE_MODE
#ifdef INTERRUPT_TEST
    , INIT_INTERRUPT_MODE, COMPUTE_INTERRUPT_MODE
#endif
} ElapsedTime_mode;

extern unsigned int urand(void);
extern void gsrand(unsigned int);
extern void TimeCalc(void);
extern void SetResponseTime(short side);
extern void CheckForTimeout(int score, int globalscore,
                            int Jscore, int zwndw);
extern void ElapsedTime(ElapsedTime_mode iop);
extern void SetTimeControl(void);
