#include <SDL2/SDL.h>

uint8_t note_period_tab[2048] = {
	  0, /* off  */
	141, /* A-10 */
	129, /* A-9  */
	122, /* D-9  */
	117, /* A-8  */
	113, /* F-8  */
	110, /* D-8  */
	107, /* B-7  */
	105, /* A-7  */
	103, /* G-7  */
	101, /* F-7  */
	 99, /* D#7  */
	 98, /* D-7  */
	 96, /* C-7  */
	 95, /* B-6  */
	 94, /* A#6  */
	 93, /* A-6  */
	 92, /* G#6  */
	 91, /* G-6  */
	 90, /* F#6  */
	 89, /* F-6  */
	 88, /* E-6  */
	 87, /* D#6  */
	 86, /* D-6  */
	 86, /* D-6  */
	 85, /* C#6  */
	 84, /* C-6  */
	 84, /* C-6  */
	 83, /* B-5  */
	 82, /* A#5  */
	 82, /* A#5  */
	 81, /* A-5  */
	 81, /* A-5  */
	 80, /* G#5  */
	 80, /* G#5  */
	 79, /* G-5  */
	 79, /* G-5  */
	 78, /* F#5  */
	 78, /* F#5  */
	 77, /* F-5  */
	 77, /* F-5  */
	 76, /* E-5  */
	 76, /* E-5  */
	 76, /* E-5  */
	 75, /* D#5  */
	 75, /* D#5  */
	 74, /* D-5  */
	 74, /* D-5  */
	 74, /* D-5  */
	 73, /* C#5  */
	 73, /* C#5  */
	 73, /* C#5  */
	 72, /* C-5  */
	 72, /* C-5  */
	 72, /* C-5  */
	 71, /* B-4  */
	 71, /* B-4  */
	 71, /* B-4  */
	 70, /* A#4  */
	 70, /* A#4  */
	 70, /* A#4  */
	 70, /* A#4  */
	 69, /* A-4  */
	 69, /* A-4  */
	 69, /* A-4  */
	 68, /* G#4  */
	 68, /* G#4  */
	 68, /* G#4  */
	 68, /* G#4  */
	 67, /* G-4  */
	 67, /* G-4  */
	 67, /* G-4  */
	 67, /* G-4  */
	 66, /* F#4  */
	 66, /* F#4  */
	 66, /* F#4  */
	 66, /* F#4  */
	 66, /* F#4  */
	 65, /* F-4  */
	 65, /* F-4  */
	 65, /* F-4  */
	 65, /* F-4  */
	 64, /* E-4  */
	 64, /* E-4  */
	 64, /* E-4  */
	 64, /* E-4  */
	 64, /* E-4  */
	 63, /* D#4  */
	 63, /* D#4  */
	 63, /* D#4  */
	 63, /* D#4  */
	 63, /* D#4  */
	 62, /* D-4  */
	 62, /* D-4  */
	 62, /* D-4  */
	 62, /* D-4  */
	 62, /* D-4  */
	 62, /* D-4  */
	 61, /* C#4  */
	 61, /* C#4  */
	 61, /* C#4  */
	 61, /* C#4  */
	 61, /* C#4  */
	 60, /* C-4  */
	 60, /* C-4  */
	 60, /* C-4  */
	 60, /* C-4  */
	 60, /* C-4  */
	 60, /* C-4  */
	 60, /* C-4  */
	 59, /* B-3  */
	 59, /* B-3  */
	 59, /* B-3  */
	 59, /* B-3  */
	 59, /* B-3  */
	 59, /* B-3  */
	 58, /* A#3  */
	 58, /* A#3  */
	 58, /* A#3  */
	 58, /* A#3  */
	 58, /* A#3  */
	 58, /* A#3  */
	 58, /* A#3  */
	 57, /* A-3  */
	 57, /* A-3  */
	 57, /* A-3  */
	 57, /* A-3  */
	 57, /* A-3  */
	 57, /* A-3  */
	 57, /* A-3  */
	 56, /* G#3  */
	 56, /* G#3  */
	 56, /* G#3  */
	 56, /* G#3  */
	 56, /* G#3  */
	 56, /* G#3  */
	 56, /* G#3  */
	 56, /* G#3  */
	 55, /* G-3  */
	 55, /* G-3  */
	 55, /* G-3  */
	 55, /* G-3  */
	 55, /* G-3  */
	 55, /* G-3  */
	 55, /* G-3  */
	 55, /* G-3  */
	 54, /* F#3  */
	 54, /* F#3  */
	 54, /* F#3  */
	 54, /* F#3  */
	 54, /* F#3  */
	 54, /* F#3  */
	 54, /* F#3  */
	 54, /* F#3  */
	 54, /* F#3  */
	 53, /* F-3  */
	 53, /* F-3  */
	 53, /* F-3  */
	 53, /* F-3  */
	 53, /* F-3  */
	 53, /* F-3  */
	 53, /* F-3  */
	 53, /* F-3  */
	 53, /* F-3  */
	 52, /* E-3  */
	 52, /* E-3  */
	 52, /* E-3  */
	 52, /* E-3  */
	 52, /* E-3  */
	 52, /* E-3  */
	 52, /* E-3  */
	 52, /* E-3  */
	 52, /* E-3  */
	 52, /* E-3  */
	 51, /* D#3  */
	 51, /* D#3  */
	 51, /* D#3  */
	 51, /* D#3  */
	 51, /* D#3  */
	 51, /* D#3  */
	 51, /* D#3  */
	 51, /* D#3  */
	 51, /* D#3  */
	 51, /* D#3  */
	 50, /* D-3  */
	 50, /* D-3  */
	 50, /* D-3  */
	 50, /* D-3  */
	 50, /* D-3  */
	 50, /* D-3  */
	 50, /* D-3  */
	 50, /* D-3  */
	 50, /* D-3  */
	 50, /* D-3  */
	 50, /* D-3  */
	 49, /* C#3  */
	 49, /* C#3  */
	 49, /* C#3  */
	 49, /* C#3  */
	 49, /* C#3  */
	 49, /* C#3  */
	 49, /* C#3  */
	 49, /* C#3  */
	 49, /* C#3  */
	 49, /* C#3  */
	 49, /* C#3  */
	 48, /* C-3  */
	 48, /* C-3  */
	 48, /* C-3  */
	 48, /* C-3  */
	 48, /* C-3  */
	 48, /* C-3  */
	 48, /* C-3  */
	 48, /* C-3  */
	 48, /* C-3  */
	 48, /* C-3  */
	 48, /* C-3  */
	 48, /* C-3  */
	 48, /* C-3  */
	 47, /* B-2  */
	 47, /* B-2  */
	 47, /* B-2  */
	 47, /* B-2  */
	 47, /* B-2  */
	 47, /* B-2  */
	 47, /* B-2  */
	 47, /* B-2  */
	 47, /* B-2  */
	 47, /* B-2  */
	 47, /* B-2  */
	 47, /* B-2  */
	 47, /* B-2  */
	 46, /* A#2  */
	 46, /* A#2  */
	 46, /* A#2  */
	 46, /* A#2  */
	 46, /* A#2  */
	 46, /* A#2  */
	 46, /* A#2  */
	 46, /* A#2  */
	 46, /* A#2  */
	 46, /* A#2  */
	 46, /* A#2  */
	 46, /* A#2  */
	 46, /* A#2  */
	 45, /* A-2  */
	 45, /* A-2  */
	 45, /* A-2  */
	 45, /* A-2  */
	 45, /* A-2  */
	 45, /* A-2  */
	 45, /* A-2  */
	 45, /* A-2  */
	 45, /* A-2  */
	 45, /* A-2  */
	 45, /* A-2  */
	 45, /* A-2  */
	 45, /* A-2  */
	 45, /* A-2  */
	 45, /* A-2  */
	 44, /* G#2  */
	 44, /* G#2  */
	 44, /* G#2  */
	 44, /* G#2  */
	 44, /* G#2  */
	 44, /* G#2  */
	 44, /* G#2  */
	 44, /* G#2  */
	 44, /* G#2  */
	 44, /* G#2  */
	 44, /* G#2  */
	 44, /* G#2  */
	 44, /* G#2  */
	 44, /* G#2  */
	 44, /* G#2  */
	 43, /* G-2  */
	 43, /* G-2  */
	 43, /* G-2  */
	 43, /* G-2  */
	 43, /* G-2  */
	 43, /* G-2  */
	 43, /* G-2  */
	 43, /* G-2  */
	 43, /* G-2  */
	 43, /* G-2  */
	 43, /* G-2  */
	 43, /* G-2  */
	 43, /* G-2  */
	 43, /* G-2  */
	 43, /* G-2  */
	 43, /* G-2  */
	 43, /* G-2  */
	 42, /* F#2  */
	 42, /* F#2  */
	 42, /* F#2  */
	 42, /* F#2  */
	 42, /* F#2  */
	 42, /* F#2  */
	 42, /* F#2  */
	 42, /* F#2  */
	 42, /* F#2  */
	 42, /* F#2  */
	 42, /* F#2  */
	 42, /* F#2  */
	 42, /* F#2  */
	 42, /* F#2  */
	 42, /* F#2  */
	 42, /* F#2  */
	 42, /* F#2  */
	 41, /* F-2  */
	 41, /* F-2  */
	 41, /* F-2  */
	 41, /* F-2  */
	 41, /* F-2  */
	 41, /* F-2  */
	 41, /* F-2  */
	 41, /* F-2  */
	 41, /* F-2  */
	 41, /* F-2  */
	 41, /* F-2  */
	 41, /* F-2  */
	 41, /* F-2  */
	 41, /* F-2  */
	 41, /* F-2  */
	 41, /* F-2  */
	 41, /* F-2  */
	 41, /* F-2  */
	 40, /* E-2  */
	 40, /* E-2  */
	 40, /* E-2  */
	 40, /* E-2  */
	 40, /* E-2  */
	 40, /* E-2  */
	 40, /* E-2  */
	 40, /* E-2  */
	 40, /* E-2  */
	 40, /* E-2  */
	 40, /* E-2  */
	 40, /* E-2  */
	 40, /* E-2  */
	 40, /* E-2  */
	 40, /* E-2  */
	 40, /* E-2  */
	 40, /* E-2  */
	 40, /* E-2  */
	 40, /* E-2  */
	 40, /* E-2  */
	 39, /* D#2  */
	 39, /* D#2  */
	 39, /* D#2  */
	 39, /* D#2  */
	 39, /* D#2  */
	 39, /* D#2  */
	 39, /* D#2  */
	 39, /* D#2  */
	 39, /* D#2  */
	 39, /* D#2  */
	 39, /* D#2  */
	 39, /* D#2  */
	 39, /* D#2  */
	 39, /* D#2  */
	 39, /* D#2  */
	 39, /* D#2  */
	 39, /* D#2  */
	 39, /* D#2  */
	 39, /* D#2  */
	 39, /* D#2  */
	 38, /* D-2  */
	 38, /* D-2  */
	 38, /* D-2  */
	 38, /* D-2  */
	 38, /* D-2  */
	 38, /* D-2  */
	 38, /* D-2  */
	 38, /* D-2  */
	 38, /* D-2  */
	 38, /* D-2  */
	 38, /* D-2  */
	 38, /* D-2  */
	 38, /* D-2  */
	 38, /* D-2  */
	 38, /* D-2  */
	 38, /* D-2  */
	 38, /* D-2  */
	 38, /* D-2  */
	 38, /* D-2  */
	 38, /* D-2  */
	 38, /* D-2  */
	 38, /* D-2  */
	 37, /* C#2  */
	 37, /* C#2  */
	 37, /* C#2  */
	 37, /* C#2  */
	 37, /* C#2  */
	 37, /* C#2  */
	 37, /* C#2  */
	 37, /* C#2  */
	 37, /* C#2  */
	 37, /* C#2  */
	 37, /* C#2  */
	 37, /* C#2  */
	 37, /* C#2  */
	 37, /* C#2  */
	 37, /* C#2  */
	 37, /* C#2  */
	 37, /* C#2  */
	 37, /* C#2  */
	 37, /* C#2  */
	 37, /* C#2  */
	 37, /* C#2  */
	 37, /* C#2  */
	 37, /* C#2  */
	 36, /* C-2  */
	 36, /* C-2  */
	 36, /* C-2  */
	 36, /* C-2  */
	 36, /* C-2  */
	 36, /* C-2  */
	 36, /* C-2  */
	 36, /* C-2  */
	 36, /* C-2  */
	 36, /* C-2  */
	 36, /* C-2  */
	 36, /* C-2  */
	 36, /* C-2  */
	 36, /* C-2  */
	 36, /* C-2  */
	 36, /* C-2  */
	 36, /* C-2  */
	 36, /* C-2  */
	 36, /* C-2  */
	 36, /* C-2  */
	 36, /* C-2  */
	 36, /* C-2  */
	 36, /* C-2  */
	 36, /* C-2  */
	 36, /* C-2  */
	 35, /* B-1  */
	 35, /* B-1  */
	 35, /* B-1  */
	 35, /* B-1  */
	 35, /* B-1  */
	 35, /* B-1  */
	 35, /* B-1  */
	 35, /* B-1  */
	 35, /* B-1  */
	 35, /* B-1  */
	 35, /* B-1  */
	 35, /* B-1  */
	 35, /* B-1  */
	 35, /* B-1  */
	 35, /* B-1  */
	 35, /* B-1  */
	 35, /* B-1  */
	 35, /* B-1  */
	 35, /* B-1  */
	 35, /* B-1  */
	 35, /* B-1  */
	 35, /* B-1  */
	 35, /* B-1  */
	 35, /* B-1  */
	 35, /* B-1  */
	 35, /* B-1  */
	 34, /* A#1  */
	 34, /* A#1  */
	 34, /* A#1  */
	 34, /* A#1  */
	 34, /* A#1  */
	 34, /* A#1  */
	 34, /* A#1  */
	 34, /* A#1  */
	 34, /* A#1  */
	 34, /* A#1  */
	 34, /* A#1  */
	 34, /* A#1  */
	 34, /* A#1  */
	 34, /* A#1  */
	 34, /* A#1  */
	 34, /* A#1  */
	 34, /* A#1  */
	 34, /* A#1  */
	 34, /* A#1  */
	 34, /* A#1  */
	 34, /* A#1  */
	 34, /* A#1  */
	 34, /* A#1  */
	 34, /* A#1  */
	 34, /* A#1  */
	 34, /* A#1  */
	 34, /* A#1  */
	 33, /* A-1  */
	 33, /* A-1  */
	 33, /* A-1  */
	 33, /* A-1  */
	 33, /* A-1  */
	 33, /* A-1  */
	 33, /* A-1  */
	 33, /* A-1  */
	 33, /* A-1  */
	 33, /* A-1  */
	 33, /* A-1  */
	 33, /* A-1  */
	 33, /* A-1  */
	 33, /* A-1  */
	 33, /* A-1  */
	 33, /* A-1  */
	 33, /* A-1  */
	 33, /* A-1  */
	 33, /* A-1  */
	 33, /* A-1  */
	 33, /* A-1  */
	 33, /* A-1  */
	 33, /* A-1  */
	 33, /* A-1  */
	 33, /* A-1  */
	 33, /* A-1  */
	 33, /* A-1  */
	 33, /* A-1  */
	 33, /* A-1  */
	 32, /* G#1  */
	 32, /* G#1  */
	 32, /* G#1  */
	 32, /* G#1  */
	 32, /* G#1  */
	 32, /* G#1  */
	 32, /* G#1  */
	 32, /* G#1  */
	 32, /* G#1  */
	 32, /* G#1  */
	 32, /* G#1  */
	 32, /* G#1  */
	 32, /* G#1  */
	 32, /* G#1  */
	 32, /* G#1  */
	 32, /* G#1  */
	 32, /* G#1  */
	 32, /* G#1  */
	 32, /* G#1  */
	 32, /* G#1  */
	 32, /* G#1  */
	 32, /* G#1  */
	 32, /* G#1  */
	 32, /* G#1  */
	 32, /* G#1  */
	 32, /* G#1  */
	 32, /* G#1  */
	 32, /* G#1  */
	 32, /* G#1  */
	 32, /* G#1  */
	 32, /* G#1  */
	 31, /* G-1  */
	 31, /* G-1  */
	 31, /* G-1  */
	 31, /* G-1  */
	 31, /* G-1  */
	 31, /* G-1  */
	 31, /* G-1  */
	 31, /* G-1  */
	 31, /* G-1  */
	 31, /* G-1  */
	 31, /* G-1  */
	 31, /* G-1  */
	 31, /* G-1  */
	 31, /* G-1  */
	 31, /* G-1  */
	 31, /* G-1  */
	 31, /* G-1  */
	 31, /* G-1  */
	 31, /* G-1  */
	 31, /* G-1  */
	 31, /* G-1  */
	 31, /* G-1  */
	 31, /* G-1  */
	 31, /* G-1  */
	 31, /* G-1  */
	 31, /* G-1  */
	 31, /* G-1  */
	 31, /* G-1  */
	 31, /* G-1  */
	 31, /* G-1  */
	 31, /* G-1  */
	 31, /* G-1  */
	 31, /* G-1  */
	 30, /* F#1  */
	 30, /* F#1  */
	 30, /* F#1  */
	 30, /* F#1  */
	 30, /* F#1  */
	 30, /* F#1  */
	 30, /* F#1  */
	 30, /* F#1  */
	 30, /* F#1  */
	 30, /* F#1  */
	 30, /* F#1  */
	 30, /* F#1  */
	 30, /* F#1  */
	 30, /* F#1  */
	 30, /* F#1  */
	 30, /* F#1  */
	 30, /* F#1  */
	 30, /* F#1  */
	 30, /* F#1  */
	 30, /* F#1  */
	 30, /* F#1  */
	 30, /* F#1  */
	 30, /* F#1  */
	 30, /* F#1  */
	 30, /* F#1  */
	 30, /* F#1  */
	 30, /* F#1  */
	 30, /* F#1  */
	 30, /* F#1  */
	 30, /* F#1  */
	 30, /* F#1  */
	 30, /* F#1  */
	 30, /* F#1  */
	 30, /* F#1  */
	 29, /* F-1  */
	 29, /* F-1  */
	 29, /* F-1  */
	 29, /* F-1  */
	 29, /* F-1  */
	 29, /* F-1  */
	 29, /* F-1  */
	 29, /* F-1  */
	 29, /* F-1  */
	 29, /* F-1  */
	 29, /* F-1  */
	 29, /* F-1  */
	 29, /* F-1  */
	 29, /* F-1  */
	 29, /* F-1  */
	 29, /* F-1  */
	 29, /* F-1  */
	 29, /* F-1  */
	 29, /* F-1  */
	 29, /* F-1  */
	 29, /* F-1  */
	 29, /* F-1  */
	 29, /* F-1  */
	 29, /* F-1  */
	 29, /* F-1  */
	 29, /* F-1  */
	 29, /* F-1  */
	 29, /* F-1  */
	 29, /* F-1  */
	 29, /* F-1  */
	 29, /* F-1  */
	 29, /* F-1  */
	 29, /* F-1  */
	 29, /* F-1  */
	 29, /* F-1  */
	 29, /* F-1  */
	 29, /* F-1  */
	 28, /* E-1  */
	 28, /* E-1  */
	 28, /* E-1  */
	 28, /* E-1  */
	 28, /* E-1  */
	 28, /* E-1  */
	 28, /* E-1  */
	 28, /* E-1  */
	 28, /* E-1  */
	 28, /* E-1  */
	 28, /* E-1  */
	 28, /* E-1  */
	 28, /* E-1  */
	 28, /* E-1  */
	 28, /* E-1  */
	 28, /* E-1  */
	 28, /* E-1  */
	 28, /* E-1  */
	 28, /* E-1  */
	 28, /* E-1  */
	 28, /* E-1  */
	 28, /* E-1  */
	 28, /* E-1  */
	 28, /* E-1  */
	 28, /* E-1  */
	 28, /* E-1  */
	 28, /* E-1  */
	 28, /* E-1  */
	 28, /* E-1  */
	 28, /* E-1  */
	 28, /* E-1  */
	 28, /* E-1  */
	 28, /* E-1  */
	 28, /* E-1  */
	 28, /* E-1  */
	 28, /* E-1  */
	 28, /* E-1  */
	 28, /* E-1  */
	 28, /* E-1  */
	 27, /* D#1  */
	 27, /* D#1  */
	 27, /* D#1  */
	 27, /* D#1  */
	 27, /* D#1  */
	 27, /* D#1  */
	 27, /* D#1  */
	 27, /* D#1  */
	 27, /* D#1  */
	 27, /* D#1  */
	 27, /* D#1  */
	 27, /* D#1  */
	 27, /* D#1  */
	 27, /* D#1  */
	 27, /* D#1  */
	 27, /* D#1  */
	 27, /* D#1  */
	 27, /* D#1  */
	 27, /* D#1  */
	 27, /* D#1  */
	 27, /* D#1  */
	 27, /* D#1  */
	 27, /* D#1  */
	 27, /* D#1  */
	 27, /* D#1  */
	 27, /* D#1  */
	 27, /* D#1  */
	 27, /* D#1  */
	 27, /* D#1  */
	 27, /* D#1  */
	 27, /* D#1  */
	 27, /* D#1  */
	 27, /* D#1  */
	 27, /* D#1  */
	 27, /* D#1  */
	 27, /* D#1  */
	 27, /* D#1  */
	 27, /* D#1  */
	 27, /* D#1  */
	 27, /* D#1  */
	 27, /* D#1  */
	 26, /* D-1  */
	 26, /* D-1  */
	 26, /* D-1  */
	 26, /* D-1  */
	 26, /* D-1  */
	 26, /* D-1  */
	 26, /* D-1  */
	 26, /* D-1  */
	 26, /* D-1  */
	 26, /* D-1  */
	 26, /* D-1  */
	 26, /* D-1  */
	 26, /* D-1  */
	 26, /* D-1  */
	 26, /* D-1  */
	 26, /* D-1  */
	 26, /* D-1  */
	 26, /* D-1  */
	 26, /* D-1  */
	 26, /* D-1  */
	 26, /* D-1  */
	 26, /* D-1  */
	 26, /* D-1  */
	 26, /* D-1  */
	 26, /* D-1  */
	 26, /* D-1  */
	 26, /* D-1  */
	 26, /* D-1  */
	 26, /* D-1  */
	 26, /* D-1  */
	 26, /* D-1  */
	 26, /* D-1  */
	 26, /* D-1  */
	 26, /* D-1  */
	 26, /* D-1  */
	 26, /* D-1  */
	 26, /* D-1  */
	 26, /* D-1  */
	 26, /* D-1  */
	 26, /* D-1  */
	 26, /* D-1  */
	 26, /* D-1  */
	 26, /* D-1  */
	 26, /* D-1  */
	 25, /* C#1  */
	 25, /* C#1  */
	 25, /* C#1  */
	 25, /* C#1  */
	 25, /* C#1  */
	 25, /* C#1  */
	 25, /* C#1  */
	 25, /* C#1  */
	 25, /* C#1  */
	 25, /* C#1  */
	 25, /* C#1  */
	 25, /* C#1  */
	 25, /* C#1  */
	 25, /* C#1  */
	 25, /* C#1  */
	 25, /* C#1  */
	 25, /* C#1  */
	 25, /* C#1  */
	 25, /* C#1  */
	 25, /* C#1  */
	 25, /* C#1  */
	 25, /* C#1  */
	 25, /* C#1  */
	 25, /* C#1  */
	 25, /* C#1  */
	 25, /* C#1  */
	 25, /* C#1  */
	 25, /* C#1  */
	 25, /* C#1  */
	 25, /* C#1  */
	 25, /* C#1  */
	 25, /* C#1  */
	 25, /* C#1  */
	 25, /* C#1  */
	 25, /* C#1  */
	 25, /* C#1  */
	 25, /* C#1  */
	 25, /* C#1  */
	 25, /* C#1  */
	 25, /* C#1  */
	 25, /* C#1  */
	 25, /* C#1  */
	 25, /* C#1  */
	 25, /* C#1  */
	 25, /* C#1  */
	 25, /* C#1  */
	 24, /* C-1  */
	 24, /* C-1  */
	 24, /* C-1  */
	 24, /* C-1  */
	 24, /* C-1  */
	 24, /* C-1  */
	 24, /* C-1  */
	 24, /* C-1  */
	 24, /* C-1  */
	 24, /* C-1  */
	 24, /* C-1  */
	 24, /* C-1  */
	 24, /* C-1  */
	 24, /* C-1  */
	 24, /* C-1  */
	 24, /* C-1  */
	 24, /* C-1  */
	 24, /* C-1  */
	 24, /* C-1  */
	 24, /* C-1  */
	 24, /* C-1  */
	 24, /* C-1  */
	 24, /* C-1  */
	 24, /* C-1  */
	 24, /* C-1  */
	 24, /* C-1  */
	 24, /* C-1  */
	 24, /* C-1  */
	 24, /* C-1  */
	 24, /* C-1  */
	 24, /* C-1  */
	 24, /* C-1  */
	 24, /* C-1  */
	 24, /* C-1  */
	 24, /* C-1  */
	 24, /* C-1  */
	 24, /* C-1  */
	 24, /* C-1  */
	 24, /* C-1  */
	 24, /* C-1  */
	 24, /* C-1  */
	 24, /* C-1  */
	 24, /* C-1  */
	 24, /* C-1  */
	 24, /* C-1  */
	 24, /* C-1  */
	 24, /* C-1  */
	 24, /* C-1  */
	 24, /* C-1  */
	 23, /* B-0  */
	 23, /* B-0  */
	 23, /* B-0  */
	 23, /* B-0  */
	 23, /* B-0  */
	 23, /* B-0  */
	 23, /* B-0  */
	 23, /* B-0  */
	 23, /* B-0  */
	 23, /* B-0  */
	 23, /* B-0  */
	 23, /* B-0  */
	 23, /* B-0  */
	 23, /* B-0  */
	 23, /* B-0  */
	 23, /* B-0  */
	 23, /* B-0  */
	 23, /* B-0  */
	 23, /* B-0  */
	 23, /* B-0  */
	 23, /* B-0  */
	 23, /* B-0  */
	 23, /* B-0  */
	 23, /* B-0  */
	 23, /* B-0  */
	 23, /* B-0  */
	 23, /* B-0  */
	 23, /* B-0  */
	 23, /* B-0  */
	 23, /* B-0  */
	 23, /* B-0  */
	 23, /* B-0  */
	 23, /* B-0  */
	 23, /* B-0  */
	 23, /* B-0  */
	 23, /* B-0  */
	 23, /* B-0  */
	 23, /* B-0  */
	 23, /* B-0  */
	 23, /* B-0  */
	 23, /* B-0  */
	 23, /* B-0  */
	 23, /* B-0  */
	 23, /* B-0  */
	 23, /* B-0  */
	 23, /* B-0  */
	 23, /* B-0  */
	 23, /* B-0  */
	 23, /* B-0  */
	 23, /* B-0  */
	 23, /* B-0  */
	 23, /* B-0  */
	 22, /* A#0  */
	 22, /* A#0  */
	 22, /* A#0  */
	 22, /* A#0  */
	 22, /* A#0  */
	 22, /* A#0  */
	 22, /* A#0  */
	 22, /* A#0  */
	 22, /* A#0  */
	 22, /* A#0  */
	 22, /* A#0  */
	 22, /* A#0  */
	 22, /* A#0  */
	 22, /* A#0  */
	 22, /* A#0  */
	 22, /* A#0  */
	 22, /* A#0  */
	 22, /* A#0  */
	 22, /* A#0  */
	 22, /* A#0  */
	 22, /* A#0  */
	 22, /* A#0  */
	 22, /* A#0  */
	 22, /* A#0  */
	 22, /* A#0  */
	 22, /* A#0  */
	 22, /* A#0  */
	 22, /* A#0  */
	 22, /* A#0  */
	 22, /* A#0  */
	 22, /* A#0  */
	 22, /* A#0  */
	 22, /* A#0  */
	 22, /* A#0  */
	 22, /* A#0  */
	 22, /* A#0  */
	 22, /* A#0  */
	 22, /* A#0  */
	 22, /* A#0  */
	 22, /* A#0  */
	 22, /* A#0  */
	 22, /* A#0  */
	 22, /* A#0  */
	 22, /* A#0  */
	 22, /* A#0  */
	 22, /* A#0  */
	 22, /* A#0  */
	 22, /* A#0  */
	 22, /* A#0  */
	 22, /* A#0  */
	 22, /* A#0  */
	 22, /* A#0  */
	 22, /* A#0  */
	 22, /* A#0  */
	 22, /* A#0  */
	 21, /* A-0  */
	 21, /* A-0  */
	 21, /* A-0  */
	 21, /* A-0  */
	 21, /* A-0  */
	 21, /* A-0  */
	 21, /* A-0  */
	 21, /* A-0  */
	 21, /* A-0  */
	 21, /* A-0  */
	 21, /* A-0  */
	 21, /* A-0  */
	 21, /* A-0  */
	 21, /* A-0  */
	 21, /* A-0  */
	 21, /* A-0  */
	 21, /* A-0  */
	 21, /* A-0  */
	 21, /* A-0  */
	 21, /* A-0  */
	 21, /* A-0  */
	 21, /* A-0  */
	 21, /* A-0  */
	 21, /* A-0  */
	 21, /* A-0  */
	 21, /* A-0  */
	 21, /* A-0  */
	 21, /* A-0  */
	 21, /* A-0  */
	 21, /* A-0  */
	 21, /* A-0  */
	 21, /* A-0  */
	 21, /* A-0  */
	 21, /* A-0  */
	 21, /* A-0  */
	 21, /* A-0  */
	 21, /* A-0  */
	 21, /* A-0  */
	 21, /* A-0  */
	 21, /* A-0  */
	 21, /* A-0  */
	 21, /* A-0  */
	 21, /* A-0  */
	 21, /* A-0  */
	 21, /* A-0  */
	 21, /* A-0  */
	 21, /* A-0  */
	 21, /* A-0  */
	 21, /* A-0  */
	 21, /* A-0  */
	 21, /* A-0  */
	 21, /* A-0  */
	 21, /* A-0  */
	 21, /* A-0  */
	 21, /* A-0  */
	 21, /* A-0  */
	 21, /* A-0  */
	 21, /* A-0  */
	 20, /* G#0  */
	 20, /* G#0  */
	 20, /* G#0  */
	 20, /* G#0  */
	 20, /* G#0  */
	 20, /* G#0  */
	 20, /* G#0  */
	 20, /* G#0  */
	 20, /* G#0  */
	 20, /* G#0  */
	 20, /* G#0  */
	 20, /* G#0  */
	 20, /* G#0  */
	 20, /* G#0  */
	 20, /* G#0  */
	 20, /* G#0  */
	 20, /* G#0  */
	 20, /* G#0  */
	 20, /* G#0  */
	 20, /* G#0  */
	 20, /* G#0  */
	 20, /* G#0  */
	 20, /* G#0  */
	 20, /* G#0  */
	 20, /* G#0  */
	 20, /* G#0  */
	 20, /* G#0  */
	 20, /* G#0  */
	 20, /* G#0  */
	 20, /* G#0  */
	 20, /* G#0  */
	 20, /* G#0  */
	 20, /* G#0  */
	 20, /* G#0  */
	 20, /* G#0  */
	 20, /* G#0  */
	 20, /* G#0  */
	 20, /* G#0  */
	 20, /* G#0  */
	 20, /* G#0  */
	 20, /* G#0  */
	 20, /* G#0  */
	 20, /* G#0  */
	 20, /* G#0  */
	 20, /* G#0  */
	 20, /* G#0  */
	 20, /* G#0  */
	 20, /* G#0  */
	 20, /* G#0  */
	 20, /* G#0  */
	 20, /* G#0  */
	 20, /* G#0  */
	 20, /* G#0  */
	 20, /* G#0  */
	 20, /* G#0  */
	 20, /* G#0  */
	 20, /* G#0  */
	 20, /* G#0  */
	 20, /* G#0  */
	 20, /* G#0  */
	 20, /* G#0  */
	 19, /* G-0  */
	 19, /* G-0  */
	 19, /* G-0  */
	 19, /* G-0  */
	 19, /* G-0  */
	 19, /* G-0  */
	 19, /* G-0  */
	 19, /* G-0  */
	 19, /* G-0  */
	 19, /* G-0  */
	 19, /* G-0  */
	 19, /* G-0  */
	 19, /* G-0  */
	 19, /* G-0  */
	 19, /* G-0  */
	 19, /* G-0  */
	 19, /* G-0  */
	 19, /* G-0  */
	 19, /* G-0  */
	 19, /* G-0  */
	 19, /* G-0  */
	 19, /* G-0  */
	 19, /* G-0  */
	 19, /* G-0  */
	 19, /* G-0  */
	 19, /* G-0  */
	 19, /* G-0  */
	 19, /* G-0  */
	 19, /* G-0  */
	 19, /* G-0  */
	 19, /* G-0  */
	 19, /* G-0  */
	 19, /* G-0  */
	 19, /* G-0  */
	 19, /* G-0  */
	 19, /* G-0  */
	 19, /* G-0  */
	 19, /* G-0  */
	 19, /* G-0  */
	 19, /* G-0  */
	 19, /* G-0  */
	 19, /* G-0  */
	 19, /* G-0  */
	 19, /* G-0  */
	 19, /* G-0  */
	 19, /* G-0  */
	 19, /* G-0  */
	 19, /* G-0  */
	 19, /* G-0  */
	 19, /* G-0  */
	 19, /* G-0  */
	 19, /* G-0  */
	 19, /* G-0  */
	 19, /* G-0  */
	 19, /* G-0  */
	 19, /* G-0  */
	 19, /* G-0  */
	 19, /* G-0  */
	 19, /* G-0  */
	 19, /* G-0  */
	 19, /* G-0  */
	 19, /* G-0  */
	 19, /* G-0  */
	 19, /* G-0  */
	 19, /* G-0  */
	 19, /* G-0  */
	 18, /* F#0  */
	 18, /* F#0  */
	 18, /* F#0  */
	 18, /* F#0  */
	 18, /* F#0  */
	 18, /* F#0  */
	 18, /* F#0  */
	 18, /* F#0  */
	 18, /* F#0  */
	 18, /* F#0  */
	 18, /* F#0  */
	 18, /* F#0  */
	 18, /* F#0  */
	 18, /* F#0  */
	 18, /* F#0  */
	 18, /* F#0  */
	 18, /* F#0  */
	 18, /* F#0  */
	 18, /* F#0  */
	 18, /* F#0  */
	 18, /* F#0  */
	 18, /* F#0  */
	 18, /* F#0  */
	 18, /* F#0  */
	 18, /* F#0  */
	 18, /* F#0  */
	 18, /* F#0  */
	 18, /* F#0  */
	 18, /* F#0  */
	 18, /* F#0  */
	 18, /* F#0  */
	 18, /* F#0  */
	 18, /* F#0  */
	 18, /* F#0  */
	 18, /* F#0  */
	 18, /* F#0  */
	 18, /* F#0  */
	 18, /* F#0  */
	 18, /* F#0  */
	 18, /* F#0  */
	 18, /* F#0  */
	 18, /* F#0  */
	 18, /* F#0  */
	 18, /* F#0  */
	 18, /* F#0  */
	 18, /* F#0  */
	 18, /* F#0  */
	 18, /* F#0  */
	 18, /* F#0  */
	 18, /* F#0  */
	 18, /* F#0  */
	 18, /* F#0  */
	 18, /* F#0  */
	 18, /* F#0  */
	 18, /* F#0  */
	 18, /* F#0  */
	 18, /* F#0  */
	 18, /* F#0  */
	 18, /* F#0  */
	 18, /* F#0  */
	 18, /* F#0  */
	 18, /* F#0  */
	 18, /* F#0  */
	 18, /* F#0  */
	 18, /* F#0  */
	 18, /* F#0  */
	 18, /* F#0  */
	 18, /* F#0  */
	 18, /* F#0  */
	 17, /* F-0  */
	 17, /* F-0  */
	 17, /* F-0  */
	 17, /* F-0  */
	 17, /* F-0  */
	 17, /* F-0  */
	 17, /* F-0  */
	 17, /* F-0  */
	 17, /* F-0  */
	 17, /* F-0  */
	 17, /* F-0  */
	 17, /* F-0  */
	 17, /* F-0  */
	 17, /* F-0  */
	 17, /* F-0  */
	 17, /* F-0  */
	 17, /* F-0  */
	 17, /* F-0  */
	 17, /* F-0  */
	 17, /* F-0  */
	 17, /* F-0  */
	 17, /* F-0  */
	 17, /* F-0  */
	 17, /* F-0  */
	 17, /* F-0  */
	 17, /* F-0  */
	 17, /* F-0  */
	 17, /* F-0  */
	 17, /* F-0  */
	 17, /* F-0  */
	 17, /* F-0  */
	 17, /* F-0  */
	 17, /* F-0  */
	 17, /* F-0  */
	 17, /* F-0  */
	 17, /* F-0  */
	 17, /* F-0  */
	 17, /* F-0  */
	 17, /* F-0  */
	 17, /* F-0  */
	 17, /* F-0  */
	 17, /* F-0  */
	 17, /* F-0  */
	 17, /* F-0  */
	 17, /* F-0  */
	 17, /* F-0  */
	 17, /* F-0  */
	 17, /* F-0  */
	 17, /* F-0  */
	 17, /* F-0  */
	 17, /* F-0  */
	 17, /* F-0  */
	 17, /* F-0  */
	 17, /* F-0  */
	 17, /* F-0  */
	 17, /* F-0  */
	 17, /* F-0  */
	 17, /* F-0  */
	 17, /* F-0  */
	 17, /* F-0  */
	 17, /* F-0  */
	 17, /* F-0  */
	 17, /* F-0  */
	 17, /* F-0  */
	 17, /* F-0  */
	 17, /* F-0  */
	 17, /* F-0  */
	 17, /* F-0  */
	 17, /* F-0  */
	 17, /* F-0  */
	 17, /* F-0  */
	 17, /* F-0  */
	 17, /* F-0  */
	 16, /* E-0  */
	 16, /* E-0  */
	 16, /* E-0  */
	 16, /* E-0  */
	 16, /* E-0  */
	 16, /* E-0  */
	 16, /* E-0  */
	 16, /* E-0  */
	 16, /* E-0  */
	 16, /* E-0  */
	 16, /* E-0  */
	 16, /* E-0  */
	 16, /* E-0  */
	 16, /* E-0  */
	 16, /* E-0  */
	 16, /* E-0  */
	 16, /* E-0  */
	 16, /* E-0  */
	 16, /* E-0  */
	 16, /* E-0  */
	 16, /* E-0  */
	 16, /* E-0  */
	 16, /* E-0  */
	 16, /* E-0  */
	 16, /* E-0  */
	 16, /* E-0  */
	 16, /* E-0  */
	 16, /* E-0  */
	 16, /* E-0  */
	 16, /* E-0  */
	 16, /* E-0  */
	 16, /* E-0  */
	 16, /* E-0  */
	 16, /* E-0  */
	 16, /* E-0  */
	 16, /* E-0  */
	 16, /* E-0  */
	 16, /* E-0  */
	 16, /* E-0  */
	 16, /* E-0  */
	 16, /* E-0  */
	 16, /* E-0  */
	 16, /* E-0  */
	 16, /* E-0  */
	 16, /* E-0  */
	 16, /* E-0  */
	 16, /* E-0  */
	 16, /* E-0  */
	 16, /* E-0  */
	 16, /* E-0  */
	 16, /* E-0  */
	 16, /* E-0  */
	 16, /* E-0  */
	 16, /* E-0  */
	 16, /* E-0  */
	 16, /* E-0  */
	 16, /* E-0  */
	 16, /* E-0  */
	 16, /* E-0  */
	 16, /* E-0  */
	 16, /* E-0  */
	 16, /* E-0  */
	 16, /* E-0  */
	 16, /* E-0  */
	 16, /* E-0  */
	 16, /* E-0  */
	 16, /* E-0  */
	 16, /* E-0  */
	 16, /* E-0  */
	 16, /* E-0  */
	 16, /* E-0  */
	 16, /* E-0  */
	 16, /* E-0  */
	 16, /* E-0  */
	 16, /* E-0  */
	 16, /* E-0  */
	 16, /* E-0  */
	 16, /* E-0  */
	 15, /* D#0  */
	 15, /* D#0  */
	 15, /* D#0  */
	 15, /* D#0  */
	 15, /* D#0  */
	 15, /* D#0  */
	 15, /* D#0  */
	 15, /* D#0  */
	 15, /* D#0  */
	 15, /* D#0  */
	 15, /* D#0  */
	 15, /* D#0  */
	 15, /* D#0  */
	 15, /* D#0  */
	 15, /* D#0  */
	 15, /* D#0  */
	 15, /* D#0  */
	 15, /* D#0  */
	 15, /* D#0  */
	 15, /* D#0  */
	 15, /* D#0  */
	 15, /* D#0  */
	 15, /* D#0  */
	 15, /* D#0  */
	 15, /* D#0  */
	 15, /* D#0  */
	 15, /* D#0  */
	 15, /* D#0  */
	 15, /* D#0  */
	 15, /* D#0  */
	 15, /* D#0  */
	 15, /* D#0  */
	 15, /* D#0  */
	 15, /* D#0  */
	 15, /* D#0  */
	 15, /* D#0  */
	 15, /* D#0  */
	 15, /* D#0  */
	 15, /* D#0  */
	 15, /* D#0  */
	 15, /* D#0  */
	 15, /* D#0  */
	 15, /* D#0  */
	 15, /* D#0  */
	 15, /* D#0  */
	 15, /* D#0  */
	 15, /* D#0  */
	 15, /* D#0  */
	 15, /* D#0  */
	 15, /* D#0  */
	 15, /* D#0  */
	 15, /* D#0  */
	 15, /* D#0  */
	 15, /* D#0  */
	 15, /* D#0  */
	 15, /* D#0  */
	 15, /* D#0  */
	 15, /* D#0  */
	 15, /* D#0  */
	 15, /* D#0  */
	 15, /* D#0  */
	 15, /* D#0  */
	 15, /* D#0  */
	 15, /* D#0  */
	 15, /* D#0  */
	 15, /* D#0  */
	 15, /* D#0  */
	 15, /* D#0  */
	 15, /* D#0  */
	 15, /* D#0  */
	 15, /* D#0  */
	 15, /* D#0  */
	 15, /* D#0  */
	 15, /* D#0  */
	 15, /* D#0  */
	 15, /* D#0  */
	 15, /* D#0  */
	 15, /* D#0  */
	 15, /* D#0  */
	 15, /* D#0  */
	 15, /* D#0  */
	 15, /* D#0  */
	 14, /* D-0  */
	 14, /* D-0  */
	 14, /* D-0  */
	 14, /* D-0  */
	 14, /* D-0  */
	 14, /* D-0  */
	 14, /* D-0  */
	 14, /* D-0  */
	 14, /* D-0  */
	 14, /* D-0  */
	 14, /* D-0  */
	 14, /* D-0  */
	 14, /* D-0  */
	 14, /* D-0  */
	 14, /* D-0  */
	 14, /* D-0  */
	 14, /* D-0  */
	 14, /* D-0  */
	 14, /* D-0  */
	 14, /* D-0  */
	 14, /* D-0  */
	 14, /* D-0  */
	 14, /* D-0  */
	 14, /* D-0  */
	 14, /* D-0  */
	 14, /* D-0  */
	 14, /* D-0  */
	 14, /* D-0  */
	 14, /* D-0  */
	 14, /* D-0  */
	 14, /* D-0  */
	 14, /* D-0  */
	 14, /* D-0  */
	 14, /* D-0  */
	 14, /* D-0  */
	 14, /* D-0  */
	 14, /* D-0  */
	 14, /* D-0  */
	 14, /* D-0  */
	 14, /* D-0  */
	 14, /* D-0  */
	 14, /* D-0  */
	 14, /* D-0  */
	 14, /* D-0  */
	 14, /* D-0  */
	 14, /* D-0  */
	 14, /* D-0  */
	 14, /* D-0  */
	 14, /* D-0  */
	 14, /* D-0  */
	 14, /* D-0  */
	 14, /* D-0  */
	 14, /* D-0  */
	 14, /* D-0  */
	 14, /* D-0  */
	 14, /* D-0  */
	 14, /* D-0  */
	 14, /* D-0  */
	 14, /* D-0  */
	 14, /* D-0  */
	 14, /* D-0  */
	 14, /* D-0  */
	 14, /* D-0  */
	 14, /* D-0  */
	 14, /* D-0  */
	 14, /* D-0  */
	 14, /* D-0  */
	 14, /* D-0  */
	 14, /* D-0  */
	 14, /* D-0  */
	 14, /* D-0  */
	 14, /* D-0  */
	 14, /* D-0  */
	 14, /* D-0  */
	 14, /* D-0  */
	 14, /* D-0  */
	 14, /* D-0  */
	 14, /* D-0  */
	 14, /* D-0  */
	 14, /* D-0  */
	 14, /* D-0  */
	 14, /* D-0  */
	 14, /* D-0  */
	 14, /* D-0  */
	 14, /* D-0  */
	 14, /* D-0  */
	 14, /* D-0  */
	 14, /* D-0  */
	 13, /* C#0  */
	 13, /* C#0  */
	 13, /* C#0  */
	 13, /* C#0  */
	 13, /* C#0  */
	 13, /* C#0  */
	 13, /* C#0  */
	 13, /* C#0  */
	 13, /* C#0  */
	 13, /* C#0  */
	 13, /* C#0  */
	 13, /* C#0  */
	 13, /* C#0  */
	 13, /* C#0  */
	 13, /* C#0  */
	 13, /* C#0  */
	 13, /* C#0  */
	 13, /* C#0  */
	 13, /* C#0  */
	 13, /* C#0  */
	 13, /* C#0  */
	 13, /* C#0  */
	 13, /* C#0  */
	 13, /* C#0  */
	 13, /* C#0  */
	 13, /* C#0  */
	 13, /* C#0  */
	 13, /* C#0  */
	 13, /* C#0  */
	 13, /* C#0  */
	 13, /* C#0  */
	 13, /* C#0  */
	 13, /* C#0  */
	 13, /* C#0  */
	 13, /* C#0  */
	 13, /* C#0  */
	 13, /* C#0  */
	 13, /* C#0  */
	 13, /* C#0  */
	 13, /* C#0  */
	 13, /* C#0  */
	 13, /* C#0  */
	 13, /* C#0  */
	 13, /* C#0  */
	 13, /* C#0  */
	 13, /* C#0  */
	 13, /* C#0  */
	 13, /* C#0  */
	 13, /* C#0  */
	 13, /* C#0  */
	 13, /* C#0  */
	 13, /* C#0  */
	 13, /* C#0  */
	 13, /* C#0  */
	 13, /* C#0  */
	 13, /* C#0  */
	 13, /* C#0  */
	 13, /* C#0  */
	 13, /* C#0  */
	 13, /* C#0  */
	 13, /* C#0  */
	 13, /* C#0  */
	 13, /* C#0  */
	 13, /* C#0  */
	 13, /* C#0  */
	 13, /* C#0  */
	 13, /* C#0  */
	 13, /* C#0  */
	 13, /* C#0  */
	 13, /* C#0  */
	 13, /* C#0  */
	 13, /* C#0  */
	 13, /* C#0  */
	 13, /* C#0  */
	 13, /* C#0  */
	 13, /* C#0  */
	 13, /* C#0  */
	 13, /* C#0  */
	 13, /* C#0  */
	 13, /* C#0  */
	 13, /* C#0  */
	 13, /* C#0  */
	 13, /* C#0  */
	 13, /* C#0  */
	 13, /* C#0  */
	 13, /* C#0  */
	 13, /* C#0  */
	 13, /* C#0  */
	 13, /* C#0  */
	 13, /* C#0  */
	 13, /* C#0  */
	 13, /* C#0  */
	 12, /* C-0  */
	 12, /* C-0  */
	 12, /* C-0  */
	 12, /* C-0  */
	 12, /* C-0  */
	 12, /* C-0  */
	 12, /* C-0  */
	 12, /* C-0  */
	 12, /* C-0  */
	 12, /* C-0  */
	 12, /* C-0  */
	 12, /* C-0  */
	 12, /* C-0  */
	 12, /* C-0  */
	 12, /* C-0  */
	 12, /* C-0  */
	 12, /* C-0  */
	 12, /* C-0  */
	 12, /* C-0  */
	 12, /* C-0  */
	 12, /* C-0  */
	 12, /* C-0  */
	 12, /* C-0  */
	 12, /* C-0  */
	 12, /* C-0  */
	 12, /* C-0  */
	 12, /* C-0  */
	 12, /* C-0  */
	 12, /* C-0  */
	 12, /* C-0  */
	 12, /* C-0  */
	 12, /* C-0  */
	 12, /* C-0  */
	 12, /* C-0  */
	 12, /* C-0  */
	 12, /* C-0  */
	 12, /* C-0  */
	 12, /* C-0  */
	 12, /* C-0  */
	 12, /* C-0  */
	 12, /* C-0  */
	 12, /* C-0  */
	 12, /* C-0  */
	 12, /* C-0  */
	 12, /* C-0  */
	 12, /* C-0  */
	 12, /* C-0  */
	 12, /* C-0  */
	 12, /* C-0  */
	 12, /* C-0  */
	 12, /* C-0  */
	 12, /* C-0  */
	 12, /* C-0  */
	 12, /* C-0  */
	 12, /* C-0  */
	 12, /* C-0  */
	 12, /* C-0  */
	 12, /* C-0  */
	 12, /* C-0  */
	 12, /* C-0  */
	 12, /* C-0  */
	 12, /* C-0  */
	 12, /* C-0  */
	 12, /* C-0  */
	 12, /* C-0  */
	 12, /* C-0  */
	 12, /* C-0  */
	 12, /* C-0  */
	 12, /* C-0  */
	 12, /* C-0  */
	 12, /* C-0  */
	 12, /* C-0  */
	 12, /* C-0  */
	 12, /* C-0  */
	 12, /* C-0  */
	 12, /* C-0  */
	 12, /* C-0  */
	 12, /* C-0  */
	 12, /* C-0  */
	 12, /* C-0  */
	 12, /* C-0  */
	 12, /* C-0  */
	 12, /* C-0  */
	 12, /* C-0  */
	 12, /* C-0  */
	 12, /* C-0  */
	 12, /* C-0  */
	 12, /* C-0  */
	 12, /* C-0  */
	 12, /* C-0  */
	 12, /* C-0  */
	 12, /* C-0  */
	 12, /* C-0  */
	 12, /* C-0  */
	 12, /* C-0  */
	 12, /* C-0  */
	 12, /* C-0  */
	 12, /* C-0  */
	 11, /* B-0  */
	 11, /* B-0  */
	 11, /* B-0  */
	 11, /* B-0  */
	 11, /* B-0  */
	 11, /* B-0  */
	 11, /* B-0  */
	 11, /* B-0  */
	 11, /* B-0  */
	 11, /* B-0  */
	 11, /* B-0  */
	 11, /* B-0  */
	 11, /* B-0  */
	 11, /* B-0  */
	 11, /* B-0  */
	 11, /* B-0  */
	 11, /* B-0  */
	 11, /* B-0  */
	 11, /* B-0  */
	 11, /* B-0  */
	 11, /* B-0  */
	 11, /* B-0  */
	 11, /* B-0  */
	 11, /* B-0  */
	 11, /* B-0  */
	 11, /* B-0  */
	 11, /* B-0  */
	 11, /* B-0  */
	 11, /* B-0  */
	 11, /* B-0  */
	 11, /* B-0  */
	 11, /* B-0  */
	 11, /* B-0  */
	 11, /* B-0  */
	 11, /* B-0  */
	 11, /* B-0  */
	 11, /* B-0  */
	 11, /* B-0  */
	 11, /* B-0  */
	 11, /* B-0  */
	 11, /* B-0  */
	 11, /* B-0  */
	 11, /* B-0  */
	 11, /* B-0  */
	 11, /* B-0  */
	 11, /* B-0  */
	 11, /* B-0  */
	 11, /* B-0  */
	 11, /* B-0  */
	 11, /* B-0  */
	 11, /* B-0  */
	 11, /* B-0  */
	 11, /* B-0  */
	 11, /* B-0  */
	 11, /* B-0  */
	 11, /* B-0  */
	 11, /* B-0  */
	 11, /* B-0  */
	 11, /* B-0  */
	 11, /* B-0  */
	 11, /* B-0  */
	 11, /* B-0  */
	 11, /* B-0  */
	 11, /* B-0  */
	 11, /* B-0  */
	 11, /* B-0  */
	 11, /* B-0  */
	 11, /* B-0  */
	 11, /* B-0  */
	 11, /* B-0  */
	 11, /* B-0  */
	 11, /* B-0  */
	 11, /* B-0  */
	 11, /* B-0  */
	 11, /* B-0  */
	 11, /* B-0  */
	 11, /* B-0  */
	 11, /* B-0  */
	 11, /* B-0  */
	 11, /* B-0  */
	 11, /* B-0  */
	 11, /* B-0  */
	 11, /* B-0  */
	 11, /* B-0  */
	 11, /* B-0  */
	 11, /* B-0  */
	 11, /* B-0  */
	 11, /* B-0  */
	 11, /* B-0  */
	 11, /* B-0  */
	 11, /* B-0  */
	 11, /* B-0  */
	 11, /* B-0  */
	 11, /* B-0  */
	 11, /* B-0  */
	 11, /* B-0  */
	 11, /* B-0  */
	 11, /* B-0  */
	 11, /* B-0  */
	 11, /* B-0  */
	 11, /* B-0  */
	 11, /* B-0  */
	 11, /* B-0  */
	 11, /* B-0  */
	 10, /* A#0  */
	 10, /* A#0  */
	 10, /* A#0  */
	 10, /* A#0  */
	 10, /* A#0  */
	 10, /* A#0  */
	 10, /* A#0  */
	 10, /* A#0  */
	 10, /* A#0  */
	 10, /* A#0  */
	 10, /* A#0  */
	 10, /* A#0  */
	 10, /* A#0  */
	 10, /* A#0  */
	 10, /* A#0  */
	 10, /* A#0  */
	 10, /* A#0  */
	 10, /* A#0  */
	 10, /* A#0  */
	 10, /* A#0  */
	 10, /* A#0  */
	 10, /* A#0  */
	 10, /* A#0  */
	 10, /* A#0  */
	 10, /* A#0  */
	 10, /* A#0  */
	 10, /* A#0  */
	 10, /* A#0  */
	 10, /* A#0  */
	 10, /* A#0  */
	 10, /* A#0  */
	 10, /* A#0  */
	 10, /* A#0  */
	 10, /* A#0  */
	 10, /* A#0  */
	 10, /* A#0  */
	 10, /* A#0  */
	 10, /* A#0  */
	 10, /* A#0  */
	 10, /* A#0  */
	 10, /* A#0  */
	 10, /* A#0  */
	 10, /* A#0  */
	 10, /* A#0  */
	 10, /* A#0  */
	 10, /* A#0  */
	 10, /* A#0  */
	 10, /* A#0  */
	 10, /* A#0  */
	 10, /* A#0  */
	 10, /* A#0  */
	 10, /* A#0  */
	 10, /* A#0  */
	 10, /* A#0  */
	 10, /* A#0  */
	 10, /* A#0  */
	 10, /* A#0  */
	 10, /* A#0  */
	 10, /* A#0  */
	 10, /* A#0  */
	 10, /* A#0  */
	 10, /* A#0  */
	 10, /* A#0  */
	 10, /* A#0  */
	 10, /* A#0  */
	 10, /* A#0  */
	 10, /* A#0  */
	 10, /* A#0  */
	 10, /* A#0  */
	 10, /* A#0  */
	 10, /* A#0  */
	 10, /* A#0  */
	 10, /* A#0  */
	 10, /* A#0  */
	 10, /* A#0  */
	 10, /* A#0  */
	 10, /* A#0  */
	 10, /* A#0  */
	 10, /* A#0  */
	 10, /* A#0  */
	 10, /* A#0  */
	 10, /* A#0  */
	 10, /* A#0  */
	 10, /* A#0  */
	 10, /* A#0  */
	 10, /* A#0  */
	 10, /* A#0  */
	 10, /* A#0  */
	 10, /* A#0  */
	 10, /* A#0  */
	 10, /* A#0  */
	 10, /* A#0  */
	 10, /* A#0  */
	 10, /* A#0  */
	 10, /* A#0  */
	 10, /* A#0  */
	 10, /* A#0  */
	 10, /* A#0  */
	 10, /* A#0  */
	 10, /* A#0  */
	 10, /* A#0  */
	 10, /* A#0  */
	 10, /* A#0  */
	 10, /* A#0  */
	 10, /* A#0  */
	 10, /* A#0  */
	 10, /* A#0  */
	 10, /* A#0  */
	 10, /* A#0  */
	 10, /* A#0  */
	  9, /* A-0  */
	  9, /* A-0  */
	  9, /* A-0  */
	  9, /* A-0  */
	  9, /* A-0  */
	  9, /* A-0  */
	  9, /* A-0  */
	  9, /* A-0  */
	  9, /* A-0  */
	  9, /* A-0  */
	  9, /* A-0  */
	  9, /* A-0  */
	  9, /* A-0  */
	  9, /* A-0  */
	  9, /* A-0  */
	  9, /* A-0  */
	  9, /* A-0  */
	  9, /* A-0  */
	  9, /* A-0  */
	  9, /* A-0  */
	  9, /* A-0  */
	  9, /* A-0  */
	  9, /* A-0  */
	  9, /* A-0  */
	  9, /* A-0  */
	  9, /* A-0  */
	  9, /* A-0  */
	  9, /* A-0  */
	  9, /* A-0  */
	  9, /* A-0  */
	  9, /* A-0  */
	  9, /* A-0  */
	  9, /* A-0  */
	  9, /* A-0  */
	  9, /* A-0  */
	  9, /* A-0  */
	  9, /* A-0  */
	  9, /* A-0  */
	  9, /* A-0  */
	  9, /* A-0  */
	  9, /* A-0  */
	  9, /* A-0  */
	  9, /* A-0  */
	  9, /* A-0  */
	  9, /* A-0  */
	  9, /* A-0  */
	  9, /* A-0  */
	  9, /* A-0  */
	  9, /* A-0  */
	  9, /* A-0  */
	  9, /* A-0  */
	  9, /* A-0  */
	  9, /* A-0  */
	  9, /* A-0  */
	  9, /* A-0  */
	  9, /* A-0  */
	  9, /* A-0  */
	  9, /* A-0  */
	  9, /* A-0  */
	  9, /* A-0  */
	  9, /* A-0  */
	  9, /* A-0  */
	  9, /* A-0  */
	  9, /* A-0  */
	  9, /* A-0  */
	  9, /* A-0  */
	  9, /* A-0  */
	  9, /* A-0  */
	  9, /* A-0  */
	  9, /* A-0  */
	  9, /* A-0  */
	  9, /* A-0  */
	  9, /* A-0  */
	  9, /* A-0  */
	  9, /* A-0  */
	  9, /* A-0  */
	  9, /* A-0  */
	  9, /* A-0  */
	  9, /* A-0  */
	  9, /* A-0  */
	  9, /* A-0  */
	  9, /* A-0  */
	  9, /* A-0  */
	  9, /* A-0  */
	  9, /* A-0  */
	  9, /* A-0  */
	  9, /* A-0  */
	  9, /* A-0  */
	  9  /* A-0  */
};
