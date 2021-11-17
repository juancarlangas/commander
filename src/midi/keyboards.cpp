#include <stdio.h>
#include <stdlib.h>
#include <time.h>
#include "midi/keyboards.hpp"
#include <ncurses.h>

Keyboard::Keyboard() :
	device( NULL ),
	port( "hw:1,0,0" )
{
	sprintf(name, "");
	activeMode = false;
	passiveMode = false;
	part = 1;
}

void Keyboard::set_buffer( const struct System &_Buffer ) noexcept
{
	buffer = _Buffer;
	variacion = buffer.variacion_inicial;
}

void Keyboard::prev_variation() noexcept
{
	if ( variacion > 0 )
		set_variation( variacion - 1 );
}

void Keyboard::next_variation() noexcept
{
	if ( variacion < buffer.n_variaciones - 1 )
		set_variation( variacion + 1 );
}

void Keyboard::set_variation( const int16_t _Variacion ) noexcept
{
	variacion = _Variacion;
	// sleep
	static struct timespec keyboardTimer;
		keyboardTimer.tv_sec 	= 0;
		keyboardTimer.tv_nsec	= 200000000;

	// apagar sonidos
	unsigned char allSoundsOff[3] = {0xB0, 0x7B, 0x7F};

	// cambio de página
	unsigned char pageSysEx[2][7] =    {{0xF0, 0x42, 0x30, 0x7A, 0x4E, 0x00, 0xF7},
										{0xF0, 0x42, 0x30, 0x7A, 0x4E, 0x01, 0xF7}};

	// Todos los canales OFF
	unsigned char x50_on_off[8][13] = 
	{	{0xF0, 0x42, 0x30, 0x7A, 0x41, 0x01, 0x00, 0x01, 0x00, 0x03, 0x00, 0x01, 0xF7},
		{0xF0, 0x42, 0x30, 0x7A, 0x41, 0x01, 0x00, 0x02, 0x00, 0x03, 0x00, 0x01, 0xF7},
		{0xF0, 0x42, 0x30, 0x7A, 0x41, 0x01, 0x00, 0x03, 0x00, 0x03, 0x00, 0x01, 0xF7},
		{0xF0, 0x42, 0x30, 0x7A, 0x41, 0x01, 0x00, 0x04, 0x00, 0x03, 0x00, 0x01, 0xF7},
		{0xF0, 0x42, 0x30, 0x7A, 0x41, 0x01, 0x00, 0x05, 0x00, 0x03, 0x00, 0x01, 0xF7},
		{0xF0, 0x42, 0x30, 0x7A, 0x41, 0x01, 0x00, 0x06, 0x00, 0x03, 0x00, 0x01, 0xF7},
		{0xF0, 0x42, 0x30, 0x7A, 0x41, 0x01, 0x00, 0x07, 0x00, 0x03, 0x00, 0x01, 0xF7},
		{0xF0, 0x42, 0x30, 0x7A, 0x41, 0x01, 0x00, 0x08, 0x00, 0x03, 0x00, 0x01, 0xF7}	};

	for ( int16_t i = 0; i < 8; ++i )
		if ( buffer.variacion[variacion].track[i].status == ON )
			x50_on_off[i][11] = 0x00; // -> ON

	snd_rawmidi_open( NULL, &device, port, SND_RAWMIDI_SYNC );
		snd_rawmidi_write( device, allSoundsOff, 3 );
		snd_rawmidi_write( device, pageSysEx[TIMBRE], 7 );
		nanosleep( &keyboardTimer, NULL );
		for ( int16_t channel = 0; channel < 8; ++channel )
			snd_rawmidi_write( device, x50_on_off[ channel ], 13 );
		snd_rawmidi_write( device, pageSysEx[COMBI], 7 );
	snd_rawmidi_close(device);
	
	device = NULL;
}

void Keyboard::set_name(const char *id)
{
	sprintf(name, id);
}

void Keyboard::set_modality(short toMode)
{
	snd_rawmidi_t *device = NULL;
	const char *port = "hw:1,0,0";
	unsigned char sysex[7] = {0xF0, 0x42, 0x00, 0x7A, 0x4E, 0x00, 0xF7}; //Ch = sysex[2] (LSB)

	if (toMode == MULTI)
		sysex[5] = 0x04;

	
	snd_rawmidi_open(NULL, &device, port, SND_RAWMIDI_SYNC);
		snd_rawmidi_write(device, sysex, 7);
	snd_rawmidi_close(device);
	
	device = NULL;
}

void Keyboard::set_program(const char bnk, const short num)
{
	unsigned char msb[3] = {0xB0, 0x00, 0x3F}, lsb[3] = {0xB0, 0x20, 0x00}, pc[2] = {0xC0, 0x00};
	// Midi Ch = msb[0], lsb[0], pc[0] igual, solo el ultimo digito

	snd_rawmidi_open(NULL, &device, port, SND_RAWMIDI_SYNC);
		snd_rawmidi_write(device, msb, 3);
		lsb[2] = bnk - 65; /*LSB*/
			snd_rawmidi_write(device, lsb, 3);
		pc[1] = num; /*PC*/
			snd_rawmidi_write(device, pc, 2);
	snd_rawmidi_close(device); /*CLOSE*/
	device = NULL;

	part = 1;

	set_variation( buffer.variacion_inicial );
}

void Keyboard::set_song(const char song)
{
	snd_rawmidi_t *device = NULL;
	const char *port = "hw:1,0,0";
	unsigned char songSelect[2] = {0xF3, 0x00};

	
	snd_rawmidi_open(NULL, &device, port, SND_RAWMIDI_SYNC); /*Open*/
		songSelect[1] = song; /*SongSelect */
			snd_rawmidi_write(device, songSelect, 2);	
	snd_rawmidi_close(device); /*CLOSE*/

	device = NULL;
}
