#include <stdio.h>
#include <errno.h>
#include <string.h>
#include <stdlib.h>
#include <windows.h>
#include <conio.h>

#define LINE_COUNT 100
#define LINE_BUFFER 512
#define TELEPORTS_MAX 20
#define TELEPORT_KEY_CHARS 5

struct Teleport {
    int x, y;
    char key[TELEPORT_KEY_CHARS];
};

const char RESET_COLOR[] = "\033[0m";
const char PLAYER_BLOCK = '&';
const char PLAYER_BLOCK_COLOR[] = "\033[0;35m"; // purple
const char SOLID_BLOCK = 'X';
const char SOLID_BLOCK_COLOR[] = "\033[0m";
const char EMPTY_BLOCK = ' ';
const char GRASS_BLOCK = '_';
const char GRASS_BLOCK_COLOR[] = "\033[0;32m"; // green
const char DANGEROUS_OBJECT_BLOCK = '!';
const char DANGEROUS_OBJECT_BLOCK_COLOR[] = "\033[0;31m"; // red
const char TELEPORT_BLOCK = 'O';
const char TELEPORT_BLOCK_COLOR[] = "\033[0;36m"; // cyan
int g_window_len; // Console len in chars ( symbols )
int g_window_interval; // Console interval in chars ( symbols )
char g_map[LINE_COUNT][LINE_BUFFER]; // Map of the game
int g_map_line_count = 0;
int g_line_len = 0;
struct Teleport g_teleports[TELEPORTS_MAX];

void remove_spaces(char* s) {
    char* d = s;
    do {
        while (*d == ' ' || *d == '\t') {
            ++d;
        }
    } while (*s++ = *d++);
}

void slice(const char* str, char* buffer, size_t start, size_t end) {
    size_t j = 0;
    for (size_t i = start; i <= end; ++i) {
        buffer[j++] = str[i];
    }
    buffer[j] = 0;
}

char ConvertMapSymbol(char sym) { // Converts map number to symbol to display in console
    switch (sym) {
    case '0': return EMPTY_BLOCK; break;
    case '1': return SOLID_BLOCK; break;
    case '2': return GRASS_BLOCK; break;
    case '3': return DANGEROUS_OBJECT_BLOCK; break;
    case '4': return TELEPORT_BLOCK; break;
    default: return sym;
    }
}

int find(char* str, char* substr) {
    return strstr(str, substr) - str;
}

void ParseKeyword(char* line){ // Parses line into string value between "=" and "\n"
    remove_spaces(line);
    int begin_ind = find(line, "=") + 1;
    int end_ind = find(line, "\n") - 1;
    slice(line, line, begin_ind, end_ind);
}

void ParseMapLine(char* line, int begin, int* map_i, __int8* is_map_began) { // Parses line of map and stores it to map
    remove_spaces(line);
    int end;
    if (find(line, "]") < 0) {
        end = find(line, "\n") - 1;
    }
    else {
        end = find(line, "]") - 1;
        *is_map_began = 0;
    }
    slice(line, line, begin, end);
    for (int j = 0; j < strlen(line); j++) {
        g_map[*map_i][j] = line[j];
    }
    (*map_i)++;
    g_map_line_count++;
}

char* ParseTeleportKey(char* source, const int curx) { // !!!!!!
    char key_buffer[TELEPORT_KEY_CHARS]; // make a buffer of teleport key to save it
    int lasti = curx;
    while (source[lasti] != ')' && lasti - curx != TELEPORT_KEY_CHARS) lasti++;
    slice(source, key_buffer, curx, lasti - 1);
    int keylen = lasti - curx;
    for (int i = curx - 1; i < strlen(source); i++) source[i] = source[i + keylen + 2];
    return key_buffer;
}

void ConvertIntMapToSymbolMap(void) { // Converts every integer (char) of map into symbol by their analogs
    int teleport_count = 0;
    for (int i = 0; i < g_map_line_count; i++)
        for (int j = 0; j < strlen(g_map[i]); j++) {
            g_map[i][j] = ConvertMapSymbol(g_map[i][j]);
            if (g_map[i][j] == TELEPORT_BLOCK) {
                g_teleports[teleport_count].x = j;
                g_teleports[teleport_count].y = i;
                char* buff = ParseTeleportKey(g_map[i], j + 2);
                for (int k = 0; k < TELEPORT_KEY_CHARS; k++) g_teleports[teleport_count].key[k] = buff[k];
                teleport_count++;
            }
        }
}

void ParseSourceTXT(FILE* file) { // Map must be rectangle!!!!!
    const char keywords[][20] = { "WINDOW_LEN", "WINDOW_INTERVAL", "MAP"};
    char line[LINE_BUFFER];
    char ch;
    int map_i = 0; // Counts map's current index for next symbol to parse
    int i = 0;
    __int8 is_map_began = 0; // Boolean, checks if is the parsing of map has began
    while ((ch = getc(file)) != EOF) {
        line[i] = ch;
        i++;
        if (ch == '\n') {// If char is the end of the line
            for (int k = 0; k < sizeof(keywords) / sizeof(keywords)[0]; k++) {// for i in keywords
                if (strstr(line, keywords[k]) != NULL && strcmp(keywords[k], "WINDOW_LEN") == 0) {
                    // Checks if there is a keyword <WINDOW_LEN> in line
                    ParseKeyword(line);
                    g_window_len = atoi(line);
                    break;
                }
                else if (strstr(line, keywords[k]) != NULL && strcmp(keywords[k], "WINDOW_INTERVAL") == 0) {
                    // Checks if there is a keyword <WINDOW_INTERVAL> in line
                    ParseKeyword(line);
                    g_window_interval = atoi(line);
                    break;
                }
                else if (strstr(line, keywords[k]) != NULL && strcmp(keywords[k], "MAP") == 0) {
                    is_map_began = 1;
                    ParseMapLine(line, find(line, "[") - 1, &map_i, &is_map_began);
                    break;
                }
                else {
                    if (is_map_began) {
                        ParseMapLine(line, 0, &map_i, &is_map_began);
                        break;
                    }
                }
            }
            i = 0;
        }
    }
    if (is_map_began) ParseMapLine(line, 0, &map_i, &is_map_began);
}

__int8 isSymInMapLimits(const int x, const int y) {
    if (x > -2 && y > -2 && x < g_line_len && y < g_map_line_count) return 1; else return 0;
}

char* GetSymbolColor(const char sym) {
    if (sym == EMPTY_BLOCK) return "";
    else if (sym == SOLID_BLOCK) return SOLID_BLOCK_COLOR;
    else if (sym == GRASS_BLOCK) return GRASS_BLOCK_COLOR;
    else if (sym == DANGEROUS_OBJECT_BLOCK) return DANGEROUS_OBJECT_BLOCK_COLOR;
    else if (sym == PLAYER_BLOCK) return PLAYER_BLOCK_COLOR;
    else if (sym == TELEPORT_BLOCK) return TELEPORT_BLOCK_COLOR;
    else return " "; // !!!!!!!!-1 -1 -1 -1 -1 -1 !!!!!!!!
}

void DrawScene(const int plx, const int ply) { // Draws a frame
    system("cls");
    for (int y = ply - g_window_interval / 2; y <= ply + g_window_interval / 2; y++) {
        for (int x = plx - g_window_len / 2; x <= plx + g_window_len / 2; x++) {
            if (isSymInMapLimits(x, y)) printf("%s%c%s", GetSymbolColor(g_map[y][x]), g_map[y][x], RESET_COLOR); // !!! -1 -1
            else putchar(' ');
        }
        putchar('\n');
    }
}

__int8 isSolidBlock(const int x, const int y) {
    if (g_map[y][x] == SOLID_BLOCK) return 1; else return 0;
}

__int8 isDangerousObjectBlock(const int x, const int y) {
    if (g_map[y][x] == DANGEROUS_OBJECT_BLOCK) return 1; else return 0;
}

__int8 isTeleportBlock(const int x, const int y) {
    if (g_map[y][x] == TELEPORT_BLOCK) return 1; else return 0;
}

void GameOver(void) {
    system("cls");
    printf("\033[0;31mGame over! You have been died.\n\033[0m");
}

void StartGame(void) {
    SMALL_RECT windowSize = { 0 , 0 , g_window_len + 2 , g_window_interval + 2 };
    SetConsoleWindowInfo(GetStdHandle(STD_OUTPUT_HANDLE), TRUE, &windowSize);
    HWND consoleWindow = GetConsoleWindow();
    SetWindowLong(consoleWindow, GWL_STYLE, GetWindowLong(consoleWindow, GWL_STYLE) & ~WS_MAXIMIZEBOX & ~WS_SIZEBOX);
    int plx = 4;
    int ply = 16;
    int lplx = plx;
    int lply = ply;
    int lsym = g_map[ply][plx];
    g_line_len = strlen(g_map[0]);
    char move;
    g_map[ply][plx] = PLAYER_BLOCK;
    DrawScene(plx, ply);
    while ((move = _getch()) != EOF) {
        lplx = plx;
        lply = ply;
        switch (move)
        {
        case 'a': {
            if (plx - 1 > -1 && !isSolidBlock(plx - 1, ply))
                plx--;
                break;
        }
        case 'd': {
            if (plx + 1 < g_line_len && !isSolidBlock(plx + 1, ply))
                plx++;
                break;
        }
        case 'w': {
            if (ply - 1 > -1 && !isSolidBlock(plx, ply - 1))
                ply--;
                break;
        }
        case 's': {
            if (ply + 1 < g_map_line_count && !isSolidBlock(plx, ply + 1))
                ply++;
                break;
        }
        }
        if (!isDangerousObjectBlock(plx, ply)) { // Checks if is the place not dangerous block
            g_map[lply][lplx] = lsym;
            if (isTeleportBlock(plx, ply)) {
                int curr_tel_ind;
                for (int i = 0; i < TELEPORTS_MAX; i++) {
                    if (g_teleports[i].x == plx && g_teleports[i].y == ply) {
                        curr_tel_ind = i;
                    }
                }
                for (int i = 0; i < TELEPORTS_MAX; i++) {
                    if (strcmp(g_teleports[i].key, g_teleports[curr_tel_ind].key) == 0 && i != curr_tel_ind) {
                        plx = g_teleports[i].x;
                        ply = g_teleports[i].y;
                    }
                }
            }
            lsym = g_map[ply][plx];
            g_map[ply][plx] = PLAYER_BLOCK;
            DrawScene(plx, ply);
        }
        else {
            GameOver();
            break;
        }
    }
}

int main()
{
    const char FILE_NAME[] = "Source.txt";
    errno_t error;
    FILE* file;
    if ((error = fopen_s(&file, FILE_NAME, "r")) != 0) { // Error occured
        printf("File, <%s> was not found!\n", FILE_NAME);
        MessageBox(NULL, L"File was not found!", L"Error occured!", MB_ICONERROR);
        system("pause");
    }
    else { // There is no error
        ParseSourceTXT(file);
        _fcloseall();
        ConvertIntMapToSymbolMap();
        StartGame();
        system("pause");
    }
	return 0;
}