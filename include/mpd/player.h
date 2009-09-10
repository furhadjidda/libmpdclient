/* libmpdclient
   (c) 2003-2009 The Music Player Daemon Project
   This project's homepage is: http://www.musicpd.org

   Redistribution and use in source and binary forms, with or without
   modification, are permitted provided that the following conditions
   are met:

   - Redistributions of source code must retain the above copyright
   notice, this list of conditions and the following disclaimer.

   - Redistributions in binary form must reproduce the above copyright
   notice, this list of conditions and the following disclaimer in the
   documentation and/or other materials provided with the distribution.

   THIS SOFTWARE IS PROVIDED BY THE COPYRIGHT HOLDERS AND CONTRIBUTORS
   ``AS IS'' AND ANY EXPRESS OR IMPLIED WARRANTIES, INCLUDING, BUT NOT
   LIMITED TO, THE IMPLIED WARRANTIES OF MERCHANTABILITY AND FITNESS FOR
   A PARTICULAR PURPOSE ARE DISCLAIMED.  IN NO EVENT SHALL THE FOUNDATION OR
   CONTRIBUTORS BE LIABLE FOR ANY DIRECT, INDIRECT, INCIDENTAL, SPECIAL,
   EXEMPLARY, OR CONSEQUENTIAL DAMAGES (INCLUDING, BUT NOT LIMITED TO,
   PROCUREMENT OF SUBSTITUTE GOODS OR SERVICES; LOSS OF USE, DATA, OR
   PROFITS; OR BUSINESS INTERRUPTION) HOWEVER CAUSED AND ON ANY THEORY OF
   LIABILITY, WHETHER IN CONTRACT, STRICT LIABILITY, OR TORT (INCLUDING
   NEGLIGENCE OR OTHERWISE) ARISING IN ANY WAY OUT OF THE USE OF THIS
   SOFTWARE, EVEN IF ADVISED OF THE POSSIBILITY OF SUCH DAMAGE.
*/

#ifndef MPD_PLAYER_H
#define MPD_PLAYER_H

#include <stdbool.h>

struct mpd_connection;

#ifdef __cplusplus
extern "C" {
#endif

/**
 * Fetches the currently selected song (the song referenced by
 * status->song and status->songid).
 */
bool
mpd_send_currentsong(struct mpd_connection *connection);

/**
 * Starts playing the current song from the beginning.
 *
 * @param connection the connection to MPD
 */
bool
mpd_send_play(struct mpd_connection *connection);

/**
 * Starts playing the specified song from the beginning.
 *
 * @param song_pos the position of the song in the queue
 * @param connection the connection to MPD
 * @return true on success, false on error
 */
bool
mpd_send_playpos(struct mpd_connection *connection, unsigned song_pos);

/**
 * Starts playing the specified song from the beginning.
 *
 * @param connection the connection to MPD
 * @param id the id of the song
 * @return true on success, false on error
 */
bool
mpd_send_playid(struct mpd_connection *connection, int id);

bool
mpd_send_stop(struct mpd_connection *connection);

bool
mpd_send_pause(struct mpd_connection *connection, int mode);

bool
mpd_send_next(struct mpd_connection *connection);

bool
mpd_send_previous(struct mpd_connection *connection);

/**
 * Seeks the current song.
 *
 * @param connection the connection to MPD
 * @param t the position within the song, in seconds
 * @return true on success, false on error
 */
bool
mpd_send_seek(struct mpd_connection *connection, int t);

/**
 * Seeks the specified song.
 *
 * @param connection the connection to MPD
 * @param song_pos the position of the song in the queue
 * @param t the position within the song, in seconds
 * @return true on success, false on error
 */
bool
mpd_send_seekpos(struct mpd_connection *connection, unsigned song_pos, int t);

/**
 * Seeks the specified song.
 *
 * @param connection the connection to MPD
 * @param id the id of the song
 * @param t the position within the song, in seconds
 * @return true on success, false on error
 */
bool
mpd_send_seekid(struct mpd_connection *connection, int id, int t);

bool
mpd_send_repeat(struct mpd_connection *connection, int mode);

bool
mpd_send_random(struct mpd_connection *connection, int mode);

bool
mpd_send_single(struct mpd_connection *connection, int mode);

bool
mpd_send_consume(struct mpd_connection *connection, int mode);

bool
mpd_send_crossfade(struct mpd_connection *connection, int seconds);

#ifdef __cplusplus
}
#endif

#endif