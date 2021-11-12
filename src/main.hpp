#include "data/databases.hpp"
#include "../../lib/utilities/src/files.hpp"

extern	short int init_ncurses(void);
extern	void draw_windows(void);

extern void get_data(System[], int rows, FILE*);
extern short get_midiChannel(FILE*);
extern bool get_bridge_state(FILE *);

extern void llenado_displayTable(	System *displayTable[], System dataBase[], 
								const int dbRows, const char keyword[],
								int *dRows);

extern void llenado_favourite( int favourite[], System dataBase[], const int dbRows);

extern void print_displayTable(	WINDOW *, System *[],
								const unsigned int dTop, const short dRows, const int dIndex,
								short int winMode);
extern void print_playlist(		WINDOW *, System [],
								const unsigned int plTop, const short plRows,
								const int indexA, const int indexB,
								const short int mode, short int winMode);
extern void print_zoom(			WINDOW *window, System *linea);
extern void print_lcd	(		WINDOW *window, System *linea, const Variation var);
extern void print_digits(		WINDOW *, System *, int plRows, const short int mode,
								const Variation);
extern void print_mode(			WINDOW *window, const short int mode);
extern void print_computer(		WINDOW *, const short int, const short int, const Variation var);
extern void print_search(		WINDOW *searchWindow, char keyword[]);

extern short int capture(	char keyword[],
							const int 	dRows,  int *dTop,  int *dIndex,
							int 		*plRows, int *plTop, int *plIndexA, int *plIndexB,
							System [], System *[], System *[], 
							System **,
							short int *mode, Variation *var, short int *winMode,
							short int updateWindow[]						);

extern void communicator(Keyboard&, System *row, Variation var, short);
extern void set_device_mode(Keyboard&, const short int toMode, short int);

extern void decrease_index(	int *, const int, int *dIndex, 
							const short int winMode			);
extern void increase_index(	int *, const int, int *dIndex, 
							const short int	winMode			);

extern void korg_drag(	System table[],
						const int rows, const int indexUP, const int indexDOWN, 
						const int caracter					);

extern void save_playlist(System [], const int, const char* fileName);
extern int load_playlist(System [], const char* fileName);

extern short int x, y;
extern short int displayShowResults, playlistShowResults;
