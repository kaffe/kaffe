/*
 *	constants_check.h
 */

/*
 *  Copyright (c) 1999 - 2001 by Matthias Pfisterer <Matthias.Pfisterer@gmx.de>
 *
 *   This program is free software; you can redistribute it and/or modify
 *   it under the terms of the GNU Library General Public License as published
 *   by the Free Software Foundation; either version 2 of the License, or
 *   (at your option) any later version.
 *
 *   This program is distributed in the hope that it will be useful,
 *   but WITHOUT ANY WARRANTY; without even the implied warranty of
 *   MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 *   GNU Library General Public License for more details.
 *
 *   You should have received a copy of the GNU Library General Public
 *   License along with this program; if not, write to the Free Software
 *   Foundation, Inc., 675 Mass Ave, Cambridge, MA 02139, USA.
 */


#undef NDEBUG
#include	<assert.h>


void
check_constants()
{
	assert(SND_SEQ_EVENT_SYSTEM == org_tritonus_lowlevel_alsa_AlsaSeq_SND_SEQ_EVENT_SYSTEM);
	assert(SND_SEQ_EVENT_RESULT == org_tritonus_lowlevel_alsa_AlsaSeq_SND_SEQ_EVENT_RESULT);
	assert(SND_SEQ_EVENT_NOTE == org_tritonus_lowlevel_alsa_AlsaSeq_SND_SEQ_EVENT_NOTE);
	assert(SND_SEQ_EVENT_NOTEON == org_tritonus_lowlevel_alsa_AlsaSeq_SND_SEQ_EVENT_NOTEON);
	assert(SND_SEQ_EVENT_NOTEOFF == org_tritonus_lowlevel_alsa_AlsaSeq_SND_SEQ_EVENT_NOTEOFF);
	assert(SND_SEQ_EVENT_KEYPRESS == org_tritonus_lowlevel_alsa_AlsaSeq_SND_SEQ_EVENT_KEYPRESS);
	assert(SND_SEQ_EVENT_CONTROLLER == org_tritonus_lowlevel_alsa_AlsaSeq_SND_SEQ_EVENT_CONTROLLER);
	assert(SND_SEQ_EVENT_PGMCHANGE == org_tritonus_lowlevel_alsa_AlsaSeq_SND_SEQ_EVENT_PGMCHANGE);
	assert(SND_SEQ_EVENT_CHANPRESS == org_tritonus_lowlevel_alsa_AlsaSeq_SND_SEQ_EVENT_CHANPRESS);
	assert(SND_SEQ_EVENT_PITCHBEND == org_tritonus_lowlevel_alsa_AlsaSeq_SND_SEQ_EVENT_PITCHBEND);
	assert(SND_SEQ_EVENT_CONTROL14 ==org_tritonus_lowlevel_alsa_AlsaSeq_SND_SEQ_EVENT_CONTROL14);
	assert(SND_SEQ_EVENT_NONREGPARAM == org_tritonus_lowlevel_alsa_AlsaSeq_SND_SEQ_EVENT_NONREGPARAM);
	assert(SND_SEQ_EVENT_REGPARAM == org_tritonus_lowlevel_alsa_AlsaSeq_SND_SEQ_EVENT_REGPARAM);
	assert(SND_SEQ_EVENT_SONGPOS == org_tritonus_lowlevel_alsa_AlsaSeq_SND_SEQ_EVENT_SONGPOS);
	assert(SND_SEQ_EVENT_SONGSEL == org_tritonus_lowlevel_alsa_AlsaSeq_SND_SEQ_EVENT_SONGSEL);
	assert(SND_SEQ_EVENT_QFRAME == org_tritonus_lowlevel_alsa_AlsaSeq_SND_SEQ_EVENT_QFRAME);
	assert(SND_SEQ_EVENT_TIMESIGN == org_tritonus_lowlevel_alsa_AlsaSeq_SND_SEQ_EVENT_TIMESIGN);
	assert(SND_SEQ_EVENT_KEYSIGN == org_tritonus_lowlevel_alsa_AlsaSeq_SND_SEQ_EVENT_KEYSIGN);
	assert(SND_SEQ_EVENT_START == org_tritonus_lowlevel_alsa_AlsaSeq_SND_SEQ_EVENT_START);
	assert(SND_SEQ_EVENT_CONTINUE == org_tritonus_lowlevel_alsa_AlsaSeq_SND_SEQ_EVENT_CONTINUE);
	assert(SND_SEQ_EVENT_STOP == org_tritonus_lowlevel_alsa_AlsaSeq_SND_SEQ_EVENT_STOP);
	assert(SND_SEQ_EVENT_SETPOS_TICK == org_tritonus_lowlevel_alsa_AlsaSeq_SND_SEQ_EVENT_SETPOS_TICK);
	assert(SND_SEQ_EVENT_SETPOS_TIME == org_tritonus_lowlevel_alsa_AlsaSeq_SND_SEQ_EVENT_SETPOS_TIME);
	assert(SND_SEQ_EVENT_TEMPO == org_tritonus_lowlevel_alsa_AlsaSeq_SND_SEQ_EVENT_TEMPO);
	assert(SND_SEQ_EVENT_CLOCK == org_tritonus_lowlevel_alsa_AlsaSeq_SND_SEQ_EVENT_CLOCK);
	assert(SND_SEQ_EVENT_TICK == org_tritonus_lowlevel_alsa_AlsaSeq_SND_SEQ_EVENT_TICK);
// gives an error with ALSA 0.9.0rc2
//	assert(SND_SEQ_EVENT_SYNC == org_tritonus_lowlevel_alsa_AlsaSeq_SND_SEQ_EVENT_SYNC);
	assert(SND_SEQ_EVENT_SYNC_POS == org_tritonus_lowlevel_alsa_AlsaSeq_SND_SEQ_EVENT_SYNC_POS);
	assert(SND_SEQ_EVENT_TUNE_REQUEST == org_tritonus_lowlevel_alsa_AlsaSeq_SND_SEQ_EVENT_TUNE_REQUEST);
	assert(SND_SEQ_EVENT_RESET == org_tritonus_lowlevel_alsa_AlsaSeq_SND_SEQ_EVENT_RESET);
	assert(SND_SEQ_EVENT_SENSING == org_tritonus_lowlevel_alsa_AlsaSeq_SND_SEQ_EVENT_SENSING);
	assert(SND_SEQ_EVENT_ECHO == org_tritonus_lowlevel_alsa_AlsaSeq_SND_SEQ_EVENT_ECHO);
	assert(SND_SEQ_EVENT_OSS == org_tritonus_lowlevel_alsa_AlsaSeq_SND_SEQ_EVENT_OSS);
	assert(SND_SEQ_EVENT_CLIENT_START == org_tritonus_lowlevel_alsa_AlsaSeq_SND_SEQ_EVENT_CLIENT_START);
	assert(SND_SEQ_EVENT_CLIENT_EXIT == org_tritonus_lowlevel_alsa_AlsaSeq_SND_SEQ_EVENT_CLIENT_EXIT);
	assert(SND_SEQ_EVENT_CLIENT_CHANGE == org_tritonus_lowlevel_alsa_AlsaSeq_SND_SEQ_EVENT_CLIENT_CHANGE);
	assert(SND_SEQ_EVENT_PORT_START == org_tritonus_lowlevel_alsa_AlsaSeq_SND_SEQ_EVENT_PORT_START);
	assert(SND_SEQ_EVENT_PORT_EXIT == org_tritonus_lowlevel_alsa_AlsaSeq_SND_SEQ_EVENT_PORT_EXIT);
	assert(SND_SEQ_EVENT_PORT_CHANGE == org_tritonus_lowlevel_alsa_AlsaSeq_SND_SEQ_EVENT_PORT_CHANGE);
	assert(SND_SEQ_EVENT_PORT_SUBSCRIBED == org_tritonus_lowlevel_alsa_AlsaSeq_SND_SEQ_EVENT_PORT_SUBSCRIBED);
	assert(SND_SEQ_EVENT_PORT_UNSUBSCRIBED == org_tritonus_lowlevel_alsa_AlsaSeq_SND_SEQ_EVENT_PORT_UNSUBSCRIBED);
	assert(SND_SEQ_EVENT_SAMPLE == org_tritonus_lowlevel_alsa_AlsaSeq_SND_SEQ_EVENT_SAMPLE);
	assert(SND_SEQ_EVENT_SAMPLE_CLUSTER == org_tritonus_lowlevel_alsa_AlsaSeq_SND_SEQ_EVENT_SAMPLE_CLUSTER);
	assert(SND_SEQ_EVENT_SAMPLE_START == org_tritonus_lowlevel_alsa_AlsaSeq_SND_SEQ_EVENT_SAMPLE_START);
	assert(SND_SEQ_EVENT_SAMPLE_STOP == org_tritonus_lowlevel_alsa_AlsaSeq_SND_SEQ_EVENT_SAMPLE_STOP);
	assert(SND_SEQ_EVENT_SAMPLE_FREQ == org_tritonus_lowlevel_alsa_AlsaSeq_SND_SEQ_EVENT_SAMPLE_FREQ);
	assert(SND_SEQ_EVENT_SAMPLE_VOLUME == org_tritonus_lowlevel_alsa_AlsaSeq_SND_SEQ_EVENT_SAMPLE_VOLUME);
	assert(SND_SEQ_EVENT_SAMPLE_LOOP == org_tritonus_lowlevel_alsa_AlsaSeq_SND_SEQ_EVENT_SAMPLE_LOOP);
	assert(SND_SEQ_EVENT_SAMPLE_POSITION == org_tritonus_lowlevel_alsa_AlsaSeq_SND_SEQ_EVENT_SAMPLE_POSITION);
	assert(SND_SEQ_EVENT_SAMPLE_PRIVATE1 == org_tritonus_lowlevel_alsa_AlsaSeq_SND_SEQ_EVENT_SAMPLE_PRIVATE1);
	assert(SND_SEQ_EVENT_USR0 == org_tritonus_lowlevel_alsa_AlsaSeq_SND_SEQ_EVENT_USR0);
	assert(SND_SEQ_EVENT_USR1 == org_tritonus_lowlevel_alsa_AlsaSeq_SND_SEQ_EVENT_USR1);
	assert(SND_SEQ_EVENT_USR2 == org_tritonus_lowlevel_alsa_AlsaSeq_SND_SEQ_EVENT_USR2);
	assert(SND_SEQ_EVENT_USR3 == org_tritonus_lowlevel_alsa_AlsaSeq_SND_SEQ_EVENT_USR3);
	assert(SND_SEQ_EVENT_USR4 == org_tritonus_lowlevel_alsa_AlsaSeq_SND_SEQ_EVENT_USR4);
	assert(SND_SEQ_EVENT_USR5 == org_tritonus_lowlevel_alsa_AlsaSeq_SND_SEQ_EVENT_USR5);
	assert(SND_SEQ_EVENT_USR6 == org_tritonus_lowlevel_alsa_AlsaSeq_SND_SEQ_EVENT_USR6);
	assert(SND_SEQ_EVENT_USR7 == org_tritonus_lowlevel_alsa_AlsaSeq_SND_SEQ_EVENT_USR7);
	assert(SND_SEQ_EVENT_USR8 == org_tritonus_lowlevel_alsa_AlsaSeq_SND_SEQ_EVENT_USR8);
	assert(SND_SEQ_EVENT_USR9 == org_tritonus_lowlevel_alsa_AlsaSeq_SND_SEQ_EVENT_USR9);
	assert(SND_SEQ_EVENT_INSTR_BEGIN == org_tritonus_lowlevel_alsa_AlsaSeq_SND_SEQ_EVENT_INSTR_BEGIN);
	assert(SND_SEQ_EVENT_INSTR_END == org_tritonus_lowlevel_alsa_AlsaSeq_SND_SEQ_EVENT_INSTR_END);
	assert(SND_SEQ_EVENT_INSTR_INFO == org_tritonus_lowlevel_alsa_AlsaSeq_SND_SEQ_EVENT_INSTR_INFO);
	assert(SND_SEQ_EVENT_INSTR_INFO_RESULT == org_tritonus_lowlevel_alsa_AlsaSeq_SND_SEQ_EVENT_INSTR_INFO_RESULT);
	assert(SND_SEQ_EVENT_INSTR_FINFO == org_tritonus_lowlevel_alsa_AlsaSeq_SND_SEQ_EVENT_INSTR_FINFO);
	assert(SND_SEQ_EVENT_INSTR_FINFO_RESULT == org_tritonus_lowlevel_alsa_AlsaSeq_SND_SEQ_EVENT_INSTR_FINFO_RESULT);
	assert(SND_SEQ_EVENT_INSTR_RESET == org_tritonus_lowlevel_alsa_AlsaSeq_SND_SEQ_EVENT_INSTR_RESET);
	assert(SND_SEQ_EVENT_INSTR_STATUS == org_tritonus_lowlevel_alsa_AlsaSeq_SND_SEQ_EVENT_INSTR_STATUS);
	assert(SND_SEQ_EVENT_INSTR_STATUS_RESULT == org_tritonus_lowlevel_alsa_AlsaSeq_SND_SEQ_EVENT_INSTR_STATUS_RESULT);
	assert(SND_SEQ_EVENT_INSTR_PUT == org_tritonus_lowlevel_alsa_AlsaSeq_SND_SEQ_EVENT_INSTR_PUT);
	assert(SND_SEQ_EVENT_INSTR_GET == org_tritonus_lowlevel_alsa_AlsaSeq_SND_SEQ_EVENT_INSTR_GET);
	assert(SND_SEQ_EVENT_INSTR_GET_RESULT == org_tritonus_lowlevel_alsa_AlsaSeq_SND_SEQ_EVENT_INSTR_GET_RESULT);
	assert(SND_SEQ_EVENT_INSTR_FREE == org_tritonus_lowlevel_alsa_AlsaSeq_SND_SEQ_EVENT_INSTR_FREE);
	assert(SND_SEQ_EVENT_INSTR_LIST == org_tritonus_lowlevel_alsa_AlsaSeq_SND_SEQ_EVENT_INSTR_LIST);
	assert(SND_SEQ_EVENT_INSTR_LIST_RESULT == org_tritonus_lowlevel_alsa_AlsaSeq_SND_SEQ_EVENT_INSTR_LIST_RESULT);
	assert(SND_SEQ_EVENT_INSTR_CLUSTER == org_tritonus_lowlevel_alsa_AlsaSeq_SND_SEQ_EVENT_INSTR_CLUSTER);
	assert(SND_SEQ_EVENT_INSTR_CLUSTER_GET == org_tritonus_lowlevel_alsa_AlsaSeq_SND_SEQ_EVENT_INSTR_CLUSTER_GET);
	assert(SND_SEQ_EVENT_INSTR_CLUSTER_RESULT == org_tritonus_lowlevel_alsa_AlsaSeq_SND_SEQ_EVENT_INSTR_CLUSTER_RESULT);
	assert(SND_SEQ_EVENT_INSTR_CHANGE == org_tritonus_lowlevel_alsa_AlsaSeq_SND_SEQ_EVENT_INSTR_CHANGE);
	assert(SND_SEQ_EVENT_SYSEX == org_tritonus_lowlevel_alsa_AlsaSeq_SND_SEQ_EVENT_SYSEX);
	assert(SND_SEQ_EVENT_BOUNCE == org_tritonus_lowlevel_alsa_AlsaSeq_SND_SEQ_EVENT_BOUNCE);
	assert(SND_SEQ_EVENT_USR_VAR0 == org_tritonus_lowlevel_alsa_AlsaSeq_SND_SEQ_EVENT_USR_VAR0);
	assert(SND_SEQ_EVENT_USR_VAR1 == org_tritonus_lowlevel_alsa_AlsaSeq_SND_SEQ_EVENT_USR_VAR1);
	assert(SND_SEQ_EVENT_USR_VAR2 == org_tritonus_lowlevel_alsa_AlsaSeq_SND_SEQ_EVENT_USR_VAR2);
	assert(SND_SEQ_EVENT_USR_VAR3 == org_tritonus_lowlevel_alsa_AlsaSeq_SND_SEQ_EVENT_USR_VAR3);
	assert(SND_SEQ_EVENT_USR_VAR4 == org_tritonus_lowlevel_alsa_AlsaSeq_SND_SEQ_EVENT_USR_VAR4);
	assert(SND_SEQ_EVENT_NONE == org_tritonus_lowlevel_alsa_AlsaSeq_SND_SEQ_EVENT_NONE);
	assert(SND_SEQ_ADDRESS_UNKNOWN == org_tritonus_lowlevel_alsa_AlsaSeq_SND_SEQ_ADDRESS_UNKNOWN);
	assert(SND_SEQ_ADDRESS_SUBSCRIBERS == org_tritonus_lowlevel_alsa_AlsaSeq_SND_SEQ_ADDRESS_SUBSCRIBERS);
	assert(SND_SEQ_ADDRESS_BROADCAST == org_tritonus_lowlevel_alsa_AlsaSeq_SND_SEQ_ADDRESS_BROADCAST);
	assert(SND_SEQ_QUEUE_DIRECT == org_tritonus_lowlevel_alsa_AlsaSeq_SND_SEQ_QUEUE_DIRECT);
	assert(SND_SEQ_TIME_STAMP_TICK == org_tritonus_lowlevel_alsa_AlsaSeq_SND_SEQ_TIME_STAMP_TICK);
	assert(SND_SEQ_TIME_STAMP_REAL == org_tritonus_lowlevel_alsa_AlsaSeq_SND_SEQ_TIME_STAMP_REAL);
	assert(SND_SEQ_TIME_STAMP_MASK == org_tritonus_lowlevel_alsa_AlsaSeq_SND_SEQ_TIME_STAMP_MASK);
	assert(SND_SEQ_TIME_MODE_ABS == org_tritonus_lowlevel_alsa_AlsaSeq_SND_SEQ_TIME_MODE_ABS);
	assert(SND_SEQ_TIME_MODE_REL == org_tritonus_lowlevel_alsa_AlsaSeq_SND_SEQ_TIME_MODE_REL);
	assert(SND_SEQ_TIME_MODE_MASK == org_tritonus_lowlevel_alsa_AlsaSeq_SND_SEQ_TIME_MODE_MASK);
	assert(SND_SEQ_EVENT_LENGTH_FIXED == org_tritonus_lowlevel_alsa_AlsaSeq_SND_SEQ_EVENT_LENGTH_FIXED);
	assert(SND_SEQ_EVENT_LENGTH_VARIABLE == org_tritonus_lowlevel_alsa_AlsaSeq_SND_SEQ_EVENT_LENGTH_VARIABLE);
	assert(SND_SEQ_EVENT_LENGTH_VARUSR == org_tritonus_lowlevel_alsa_AlsaSeq_SND_SEQ_EVENT_LENGTH_VARUSR);
// gives an error with ALSA 0.9.0rc2
//	assert(SND_SEQ_EVENT_LENGTH_VARIPC == org_tritonus_lowlevel_alsa_AlsaSeq_SND_SEQ_EVENT_LENGTH_VARIPC);
	assert(SND_SEQ_EVENT_LENGTH_MASK == org_tritonus_lowlevel_alsa_AlsaSeq_SND_SEQ_EVENT_LENGTH_MASK);
	assert(SND_SEQ_PRIORITY_NORMAL == org_tritonus_lowlevel_alsa_AlsaSeq_SND_SEQ_PRIORITY_NORMAL);
	assert(SND_SEQ_PRIORITY_HIGH == org_tritonus_lowlevel_alsa_AlsaSeq_SND_SEQ_PRIORITY_HIGH);
	assert(SND_SEQ_PRIORITY_MASK == org_tritonus_lowlevel_alsa_AlsaSeq_SND_SEQ_PRIORITY_MASK);
	assert(SND_SEQ_CLIENT_SYSTEM == org_tritonus_lowlevel_alsa_AlsaSeq_SND_SEQ_CLIENT_SYSTEM);
	assert(SND_SEQ_CLIENT_DUMMY == org_tritonus_lowlevel_alsa_AlsaSeq_SND_SEQ_CLIENT_DUMMY);
	assert(SND_SEQ_CLIENT_OSS == org_tritonus_lowlevel_alsa_AlsaSeq_SND_SEQ_CLIENT_OSS);
/* 	assert(SND_SEQ_FILTER_BROADCAST == org_tritonus_lowlevel_alsa_AlsaSeq_SND_SEQ_FILTER_BROADCAST); */
/* 	assert(SND_SEQ_FILTER_MULTICAST == org_tritonus_lowlevel_alsa_AlsaSeq_SND_SEQ_FILTER_MULTICAST); */
/* 	assert(SND_SEQ_FILTER_BOUNCE == org_tritonus_lowlevel_alsa_AlsaSeq_SND_SEQ_FILTER_BOUNCE); */
/* 	assert(SND_SEQ_FILTER_USE_EVENT == org_tritonus_lowlevel_alsa_AlsaSeq_SND_SEQ_FILTER_USE_EVENT); */

	assert(SND_SEQ_REMOVE_DEST== org_tritonus_lowlevel_alsa_AlsaSeq_SND_SEQ_REMOVE_DEST);
	assert(SND_SEQ_REMOVE_DEST_CHANNEL== org_tritonus_lowlevel_alsa_AlsaSeq_SND_SEQ_REMOVE_DEST_CHANNEL);
	assert(SND_SEQ_REMOVE_TIME_BEFORE== org_tritonus_lowlevel_alsa_AlsaSeq_SND_SEQ_REMOVE_TIME_BEFORE);
	assert(SND_SEQ_REMOVE_TIME_AFTER== org_tritonus_lowlevel_alsa_AlsaSeq_SND_SEQ_REMOVE_TIME_AFTER);
	assert(SND_SEQ_REMOVE_EVENT_TYPE== org_tritonus_lowlevel_alsa_AlsaSeq_SND_SEQ_REMOVE_EVENT_TYPE);
	assert(SND_SEQ_REMOVE_IGNORE_OFF== org_tritonus_lowlevel_alsa_AlsaSeq_SND_SEQ_REMOVE_IGNORE_OFF);
	assert(SND_SEQ_REMOVE_TAG_MATCH== org_tritonus_lowlevel_alsa_AlsaSeq_SND_SEQ_REMOVE_TAG_MATCH);
	assert(SND_SEQ_PORT_SYSTEM_TIMER == org_tritonus_lowlevel_alsa_AlsaSeq_SND_SEQ_PORT_SYSTEM_TIMER);
	assert(SND_SEQ_PORT_SYSTEM_ANNOUNCE == org_tritonus_lowlevel_alsa_AlsaSeq_SND_SEQ_PORT_SYSTEM_ANNOUNCE);
	assert(SND_SEQ_PORT_CAP_READ == org_tritonus_lowlevel_alsa_AlsaSeq_SND_SEQ_PORT_CAP_READ);
	assert(SND_SEQ_PORT_CAP_WRITE == org_tritonus_lowlevel_alsa_AlsaSeq_SND_SEQ_PORT_CAP_WRITE);
	assert(SND_SEQ_PORT_CAP_SYNC_READ == org_tritonus_lowlevel_alsa_AlsaSeq_SND_SEQ_PORT_CAP_SYNC_READ);
	assert(SND_SEQ_PORT_CAP_SYNC_WRITE == org_tritonus_lowlevel_alsa_AlsaSeq_SND_SEQ_PORT_CAP_SYNC_WRITE);
	assert(SND_SEQ_PORT_CAP_DUPLEX == org_tritonus_lowlevel_alsa_AlsaSeq_SND_SEQ_PORT_CAP_DUPLEX);
	assert(SND_SEQ_PORT_CAP_SUBS_READ == org_tritonus_lowlevel_alsa_AlsaSeq_SND_SEQ_PORT_CAP_SUBS_READ);
	assert(SND_SEQ_PORT_CAP_SUBS_WRITE == org_tritonus_lowlevel_alsa_AlsaSeq_SND_SEQ_PORT_CAP_SUBS_WRITE);
	assert(SND_SEQ_PORT_CAP_NO_EXPORT == org_tritonus_lowlevel_alsa_AlsaSeq_SND_SEQ_PORT_CAP_NO_EXPORT);
	assert(SND_SEQ_PORT_TYPE_SPECIFIC == org_tritonus_lowlevel_alsa_AlsaSeq_SND_SEQ_PORT_TYPE_SPECIFIC);
	assert(SND_SEQ_PORT_TYPE_MIDI_GENERIC == org_tritonus_lowlevel_alsa_AlsaSeq_SND_SEQ_PORT_TYPE_MIDI_GENERIC);
	assert(SND_SEQ_PORT_TYPE_MIDI_GM == org_tritonus_lowlevel_alsa_AlsaSeq_SND_SEQ_PORT_TYPE_MIDI_GM);
	assert(SND_SEQ_PORT_TYPE_MIDI_GS == org_tritonus_lowlevel_alsa_AlsaSeq_SND_SEQ_PORT_TYPE_MIDI_GS);
	assert(SND_SEQ_PORT_TYPE_MIDI_XG == org_tritonus_lowlevel_alsa_AlsaSeq_SND_SEQ_PORT_TYPE_MIDI_XG);
	assert(SND_SEQ_PORT_TYPE_MIDI_MT32 == org_tritonus_lowlevel_alsa_AlsaSeq_SND_SEQ_PORT_TYPE_MIDI_MT32);
	assert(SND_SEQ_PORT_TYPE_SYNTH == org_tritonus_lowlevel_alsa_AlsaSeq_SND_SEQ_PORT_TYPE_SYNTH);
	assert(SND_SEQ_PORT_TYPE_DIRECT_SAMPLE == org_tritonus_lowlevel_alsa_AlsaSeq_SND_SEQ_PORT_TYPE_DIRECT_SAMPLE);
	assert(SND_SEQ_PORT_TYPE_SAMPLE == org_tritonus_lowlevel_alsa_AlsaSeq_SND_SEQ_PORT_TYPE_SAMPLE);
	assert(SND_SEQ_PORT_TYPE_APPLICATION == org_tritonus_lowlevel_alsa_AlsaSeq_SND_SEQ_PORT_TYPE_APPLICATION);
/* 	assert(SND_SEQ_PORT_FLG_GIVEN_PORT == org_tritonus_lowlevel_alsa_AlsaSeq_SND_SEQ_PORT_FLG_GIVEN_PORT); */
/* 	assert(SND_SEQ_QUEUE_FLG_SYNC == org_tritonus_lowlevel_alsa_AlsaSeq_SND_SEQ_QUEUE_FLG_SYNC); */
/* 	assert(SND_SEQ_QUEUE_FLG_SYNC_LOST == org_tritonus_lowlevel_alsa_AlsaSeq_SND_SEQ_QUEUE_FLG_SYNC_LOST); */
/* 	assert(SND_SEQ_SYNC_TICK == org_tritonus_lowlevel_alsa_AlsaSeq_SND_SEQ_SYNC_TICK); */
/* 	assert(SND_SEQ_SYNC_TIME == org_tritonus_lowlevel_alsa_AlsaSeq_SND_SEQ_SYNC_TIME); */
/* 	assert(SND_SEQ_SYNC_MODE == org_tritonus_lowlevel_alsa_AlsaSeq_SND_SEQ_SYNC_MODE); */
/* 	assert(SND_SEQ_SYNC_FMT_PRIVATE_CLOCK == org_tritonus_lowlevel_alsa_AlsaSeq_SND_SEQ_SYNC_FMT_PRIVATE_CLOCK); */
/* 	assert(SND_SEQ_SYNC_FMT_PRIVATE_TIME == org_tritonus_lowlevel_alsa_AlsaSeq_SND_SEQ_SYNC_FMT_PRIVATE_TIME); */
/* 	assert(SND_SEQ_SYNC_FMT_MIDI_CLOCK == org_tritonus_lowlevel_alsa_AlsaSeq_SND_SEQ_SYNC_FMT_MIDI_CLOCK); */
/* 	assert(SND_SEQ_SYNC_FMT_MTC == org_tritonus_lowlevel_alsa_AlsaSeq_SND_SEQ_SYNC_FMT_MTC); */
/* 	assert(SND_SEQ_SYNC_FMT_DTL == org_tritonus_lowlevel_alsa_AlsaSeq_SND_SEQ_SYNC_FMT_DTL); */
/* 	assert(SND_SEQ_SYNC_FMT_SMPTE == org_tritonus_lowlevel_alsa_AlsaSeq_SND_SEQ_SYNC_FMT_SMPTE); */
/* 	assert(SND_SEQ_SYNC_FMT_MIDI_TICK == org_tritonus_lowlevel_alsa_AlsaSeq_SND_SEQ_SYNC_FMT_MIDI_TICK); */
/* 	assert(SND_SEQ_SYNC_FPS_24 == org_tritonus_lowlevel_alsa_AlsaSeq_SND_SEQ_SYNC_FPS_24); */
/* 	assert(SND_SEQ_SYNC_FPS_25 == org_tritonus_lowlevel_alsa_AlsaSeq_SND_SEQ_SYNC_FPS_25); */
/* 	assert(SND_SEQ_SYNC_FPS_30_DP == org_tritonus_lowlevel_alsa_AlsaSeq_SND_SEQ_SYNC_FPS_30_DP); */
/* 	assert(SND_SEQ_SYNC_FPS_30_NDP == org_tritonus_lowlevel_alsa_AlsaSeq_SND_SEQ_SYNC_FPS_30_NDP); */
/* 	assert(SND_SEQ_TIMER_ALSA == org_tritonus_lowlevel_alsa_AlsaSeq_SND_SEQ_TIMER_ALSA); */
/* 	assert(SND_SEQ_TIMER_MIDI_CLOCK == org_tritonus_lowlevel_alsa_AlsaSeq_SND_SEQ_TIMER_MIDI_CLOCK); */
/* 	assert(SND_SEQ_TIMER_MIDI_TICK == org_tritonus_lowlevel_alsa_AlsaSeq_SND_SEQ_TIMER_MIDI_TICK); */
	assert(SND_SEQ_QUERY_SUBS_READ == org_tritonus_lowlevel_alsa_AlsaSeq_SND_SEQ_QUERY_SUBS_READ);
	assert(SND_SEQ_QUERY_SUBS_WRITE == org_tritonus_lowlevel_alsa_AlsaSeq_SND_SEQ_QUERY_SUBS_WRITE);
/* 	assert(SND_SEQ_INSTR_ATYPE_DATA == org_tritonus_lowlevel_alsa_AlsaSeq_SND_SEQ_INSTR_ATYPE_DATA); */
/* 	assert(SND_SEQ_INSTR_ATYPE_ALIAS == org_tritonus_lowlevel_alsa_AlsaSeq_SND_SEQ_INSTR_ATYPE_ALIAS); */
/* 	assert(SND_SEQ_INSTR_TYPE0_DLS1 == org_tritonus_lowlevel_alsa_AlsaSeq_SND_SEQ_INSTR_TYPE0_DLS1); */
/* 	assert(SND_SEQ_INSTR_TYPE0_DLS2 == org_tritonus_lowlevel_alsa_AlsaSeq_SND_SEQ_INSTR_TYPE0_DLS2); */
/* 	assert(SND_SEQ_INSTR_TYPE1_SIMPLE == org_tritonus_lowlevel_alsa_AlsaSeq_SND_SEQ_INSTR_TYPE1_SIMPLE); */
/* 	assert(SND_SEQ_INSTR_TYPE1_SOUNDFONT == org_tritonus_lowlevel_alsa_AlsaSeq_SND_SEQ_INSTR_TYPE1_SOUNDFONT); */
/* 	assert(SND_SEQ_INSTR_TYPE1_GUS_PATCH == org_tritonus_lowlevel_alsa_AlsaSeq_SND_SEQ_INSTR_TYPE1_GUS_PATCH); */
/* 	assert(SND_SEQ_INSTR_TYPE1_INTERWAVE == org_tritonus_lowlevel_alsa_AlsaSeq_SND_SEQ_INSTR_TYPE1_INTERWAVE); */
/* 	assert(SND_SEQ_INSTR_TYPE2_OPL2_3 == org_tritonus_lowlevel_alsa_AlsaSeq_SND_SEQ_INSTR_TYPE2_OPL2_3); */
/* 	assert(SND_SEQ_INSTR_TYPE2_OPL4 == org_tritonus_lowlevel_alsa_AlsaSeq_SND_SEQ_INSTR_TYPE2_OPL4); */
/* 	assert(SND_SEQ_INSTR_PUT_CMD_CREATE == org_tritonus_lowlevel_alsa_AlsaSeq_SND_SEQ_INSTR_PUT_CMD_CREATE); */
/* 	assert(SND_SEQ_INSTR_PUT_CMD_REPLACE == org_tritonus_lowlevel_alsa_AlsaSeq_SND_SEQ_INSTR_PUT_CMD_REPLACE); */
/* 	assert(SND_SEQ_INSTR_PUT_CMD_MODIFY == org_tritonus_lowlevel_alsa_AlsaSeq_SND_SEQ_INSTR_PUT_CMD_MODIFY); */
/* 	assert(SND_SEQ_INSTR_PUT_CMD_ADD == org_tritonus_lowlevel_alsa_AlsaSeq_SND_SEQ_INSTR_PUT_CMD_ADD); */
/* 	assert(SND_SEQ_INSTR_PUT_CMD_REMOVE == org_tritonus_lowlevel_alsa_AlsaSeq_SND_SEQ_INSTR_PUT_CMD_REMOVE); */
/* 	assert(SND_SEQ_INSTR_GET_CMD_FULL == org_tritonus_lowlevel_alsa_AlsaSeq_SND_SEQ_INSTR_GET_CMD_FULL); */
/* 	assert(SND_SEQ_INSTR_GET_CMD_PARTIAL == org_tritonus_lowlevel_alsa_AlsaSeq_SND_SEQ_INSTR_GET_CMD_PARTIAL); */
/* 	assert(SND_SEQ_INSTR_QUERY_FOLLOW_ALIAS == org_tritonus_lowlevel_alsa_AlsaSeq_SND_SEQ_INSTR_QUERY_FOLLOW_ALIAS); */
/* 	assert(SND_SEQ_INSTR_FREE_CMD_ALL == org_tritonus_lowlevel_alsa_AlsaSeq_SND_SEQ_INSTR_FREE_CMD_ALL); */
/* 	assert(SND_SEQ_INSTR_FREE_CMD_PRIVATE == org_tritonus_lowlevel_alsa_AlsaSeq_SND_SEQ_INSTR_FREE_CMD_PRIVATE); */
/* 	assert(SND_SEQ_INSTR_FREE_CMD_CLUSTER == org_tritonus_lowlevel_alsa_AlsaSeq_SND_SEQ_INSTR_FREE_CMD_CLUSTER); */
/* 	assert(SND_SEQ_INSTR_FREE_CMD_SINGLE == org_tritonus_lowlevel_alsa_AlsaSeq_SND_SEQ_INSTR_FREE_CMD_SINGLE); */
}



/*** constants_check.h ***/
