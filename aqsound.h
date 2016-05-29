/***************************************************************************
 *   Copyright (C) 2007 by Alex Levkovich   *
 *   alevkovich@tut.by   *
 *                                                                         *
 *   This program is free software; you can redistribute it and/or modify  *
 *   it under the terms of the GNU General Public License as published by  *
 *   the Free Software Foundation; either version 2 of the License, or     *
 *   (at your option) any later version.                                   *
 *                                                                         *
 *   This program is distributed in the hope that it will be useful,       *
 *   but WITHOUT ANY WARRANTY; without even the implied warranty of        *
 *   MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the         *
 *   GNU General Public License for more details.                          *
 *                                                                         *
 *   You should have received a copy of the GNU General Public License     *
 *   along with this program; if not, write to the                         *
 *   Free Software Foundation, Inc.,                                       *
 *   59 Temple Place - Suite 330, Boston, MA  02111-1307, USA.             *
 ***************************************************************************/
#ifndef _AUDIO_H_
#define _AUDIO_H_

#include <QSound>

#if defined(Q_OS_LINUX)

#include <alsa/asoundlib.h>

#endif

#ifndef FALSE
#define FALSE false
#endif

#ifndef TRUE
#define TRUE true
#endif

typedef  struct {
        quint32  dwSize ;
        quint16  wFormatTag ;
        quint16  wChannels ;
        quint32  dwSamplesPerSec ;
        quint32  dwAvgBytesPerSec ;
        quint16  wBlockAlign ;
        quint16  wBitsPerSample ;
} WAVEFORMAT ;

class QFile;

class QAlsaSound : public QSound {
    Q_OBJECT
public :
        QAlsaSound( const QString& filename, QObject* parent=0) ;
        ~QAlsaSound();

        bool isAvailable() { return is_available ; }
        void play();    
        static void play(const QString& filename);
protected:
        bool initialise() ;
        bool is_available;
        QString Path ;
private:
        char* findchunk(char* pstart, char* fourcc, size_t n);
#if defined(Q_OS_LINUX)
        /* ALSA parameters */
        snd_pcm_t              *handle;
        snd_pcm_sframes_t       frames;
        char                    device[128] ;                        /* playback device */
        snd_pcm_uframes_t chunk_size, buffer_size;
        size_t bits_per_sample, bits_per_frame, chunk_bytes;

        /* File parser */
        QFile * file;                       /* Open file descriptor or -1 */
        WAVEFORMAT waveformat ;
        u_long samples, datastart;
	snd_pcm_hw_params_t *params;
#endif
};

#endif // _AUDIO_H_

