#define _CRT_SECURE_NO_WARNINGS
#include <windows.h>
#include <shlobj.h>
#include <string.h>
#include <stdio.h>
#include <stdlib.h>

#include <pdcurses.h>
#include <locale.h>
#include <wchar.h>

#pragma comment(lib, "Shell32.lib")

/* --- Protot --- */
void clean_temp(void);
void clean_win_temp(void);
void clean_recycle_bin(void);
void delete_dir(const char *path);

int confirm_yesno(const char *title, const char *msg);

/* --- Unicode “yhtenäiset viivat” (ei ACS_*) --- */
static void draw_box_unicode(WINDOW *w, int y, int x, int h, int wid);
static void draw_hline_unicode(WINDOW *w, int y, int x, int len);

int main(void)
{
    initscr();
    SetConsoleTitleA("lCleaner");
    Sleep(50);
    setlocale(LC_ALL, "");   /* jotta Unicode-viivat toimii */
    curs_set(0);
    noecho();
    cbreak();
    keypad(stdscr, TRUE);

    int choice = 0;

    while (1)
    {
        clear();

        const char *title = "lCleaner - Easy Windows cleaner -";
        mvprintw(1, (COLS - (int)strlen(title)) / 2 + 2, "%s", title);

        /* Menu / box sijoitukset (pidetty tosi samana ku sulla) */
        int menu_y = LINES / 2 - 3;
        int menu_x = COLS  / 2 - 10;

        int box_y = menu_y - 1;
        int box_x = menu_x - 3;
        int box_h = 8;   /* ylä + 5 riviä + ala */
        int box_w = 30;

        /* Piirrä valikon kehys “yhtenäisillä viivoilla” */
        draw_box_unicode(stdscr, box_y, box_x, box_h, box_w);

        mvprintw(menu_y + 0, menu_x, choice == 0 ? "> Clear All"          : "  Clear All");
        mvprintw(menu_y + 1, menu_x, choice == 1 ? "> Clear TEMP"         : "  Clear TEMP");
        mvprintw(menu_y + 2, menu_x, choice == 2 ? "> Clear Windows Temp" : "  Clear Windows Temp");
        mvprintw(menu_y + 3, menu_x, choice == 3 ? "> Clear bin"          : "  Clear bin");
        mvprintw(menu_y + 4, menu_x, choice == 4 ? "> About"              : "  About");
        mvprintw(menu_y + 5, menu_x, choice == 5 ? "> Exit"               : "  Exit");


        const char *version = "Version 1.1.7";
        mvprintw(LINES - 2, (COLS - (int)strlen(version)) / 2, "%s", version);

        refresh();

        int ch = getch();

        if (ch == KEY_UP && choice > 0) choice--;
        if (ch == KEY_DOWN && choice < 5) choice++;

        if (ch == '\n' || ch == KEY_ENTER)
        {
            if (choice == 0) /* Clear All */
            {
                if (confirm_yesno("Clear All",
                                  "This will clear TEMP, Windows Temp and Recycle Bin.\nContinue?"))
                {
                    const char *m1 = "Clearing TEMP...";
                    clear();
                    mvprintw(LINES/2 - 1, (COLS - (int)strlen(m1)) / 2, "%s", m1);
                    refresh();
                    clean_temp();

                    const char *m2 = "Clearing Windows Temp...";
                    clear();
                    mvprintw(LINES/2 - 1, (COLS - (int)strlen(m2)) / 2, "%s", m2);
                    refresh();
                    clean_win_temp();

                    const char *m3 = "Clearing Recycle Bin...";
                    clear();
                    mvprintw(LINES/2 - 1, (COLS - (int)strlen(m3)) / 2, "%s", m3);
                    refresh();
                    clean_recycle_bin();

                    const char *done  = "All cleaning done!";
                    const char *press = "Press any key to continue.";
                    clear();
                    mvprintw(LINES/2 - 1, (COLS - (int)strlen(done)) / 2, "%s", done);
                    mvprintw(LINES/2 + 1, (COLS - (int)strlen(press)) / 2, "%s", press);
                    refresh();
                    getch();
                }
            }
            else if (choice == 1) /* Clear TEMP */
            {
                const char *msg1 = "Clearing TEMP...";
                clear();
                mvprintw(LINES/2 - 1, (COLS - (int)strlen(msg1)) / 2, "%s", msg1);
                refresh();

                clean_temp();

                const char *msg2 = "Temp cleared!";
                const char *msg3 = "Press any key to continue.";
                clear();
                mvprintw(LINES/2 - 1, (COLS - (int)strlen(msg2)) / 2, "%s", msg2);
                mvprintw(LINES/2 + 1, (COLS - (int)strlen(msg3)) / 2, "%s", msg3);
                refresh();
                getch();
            }
            else if (choice == 2) /* Clear Windows Temp */
            {
                const char *msg1 = "Clearing Windows Temp...";
                clear();
                mvprintw(LINES/2 - 1, (COLS - (int)strlen(msg1)) / 2, "%s", msg1);
                refresh();

                clean_win_temp();

                const char *msg2 = "Windows Temp cleaned!";
                const char *msg3 = "Press any key to continue.";
                clear();
                mvprintw(LINES/2 - 1, (COLS - (int)strlen(msg2)) / 2, "%s", msg2);
                mvprintw(LINES/2 + 1, (COLS - (int)strlen(msg3)) / 2, "%s", msg3);
                refresh();
                getch();
            }
            else if (choice == 3) /* Clear bin */
            {
                const char *msg1 = "Clearing Recycle Bin...";
                clear();
                mvprintw(LINES/2 - 1, (COLS - (int)strlen(msg1)) / 2, "%s", msg1);
                refresh();

                clean_recycle_bin();

                const char *msg2 = "Recycle Bin cleared!";
                const char *msg3 = "Press any key to continue.";
                clear();
                mvprintw(LINES/2 - 1, (COLS - (int)strlen(msg2)) / 2, "%s", msg2);
                mvprintw(LINES/2 + 1, (COLS - (int)strlen(msg3)) / 2, "%s", msg3);
                refresh();
                getch();
            }
            else if(choice == 4)
            {
                    int y=6;
                    clear();
                    mvprintw(y++, 7, "lCleaner - Easy Windows cleaner");
                    mvprintw(y++, 7, "Version 1.1.7");
                    mvprintw(y++, 7, "Made with C + PDCurses");
                    mvprintw(y++, 7, "Cleans TEMP, Windows Temp");
                    mvprintw(y++, 7, "and Recycle Bin.");
                    mvprintw(y++, 7, "Press any key to return.");
                    refresh();
                    getch();
            }
            else /* Exit */
            {
                break;
            }
        }
    }

    endwin();
    return 0;
}

/* --- Siivoukset --- */

void clean_temp(void)
{
    char temp[MAX_PATH];
    GetTempPathA(MAX_PATH, temp);
    delete_dir(temp);
}

void clean_win_temp(void)
{
    char winDir[MAX_PATH];
    char winTemp[MAX_PATH];

    UINT n = GetWindowsDirectoryA(winDir, MAX_PATH);
    if (n == 0 || n >= MAX_PATH) {
        strcpy(winTemp, "C:\\Windows\\Temp");
    } else {
        snprintf(winTemp, MAX_PATH, "%s\\Temp", winDir);
    }

    delete_dir(winTemp);
}

void clean_recycle_bin(void)
{
    SHEmptyRecycleBinA(
        NULL,
        NULL,
        SHERB_NOCONFIRMATION |
        SHERB_NOPROGRESSUI   |
        SHERB_NOSOUND
    );
}

void delete_dir(const char *path)
{
    char search[MAX_PATH];
    WIN32_FIND_DATAA fd;
    HANDLE h;

    snprintf(search, MAX_PATH, "%s\\*", path);

    h = FindFirstFileA(search, &fd);
    if (h == INVALID_HANDLE_VALUE)
        return;

    do {
        if (!strcmp(fd.cFileName, ".") || !strcmp(fd.cFileName, ".."))
            continue;

        char full[MAX_PATH];
        snprintf(full, MAX_PATH, "%s\\%s", path, fd.cFileName);

        if (fd.dwFileAttributes & FILE_ATTRIBUTE_DIRECTORY) {
            delete_dir(full);
            RemoveDirectoryA(full);
        } else {
            SetFileAttributesA(full, FILE_ATTRIBUTE_NORMAL);
            DeleteFileA(full);
        }
    } while (FindNextFileA(h, &fd));

    FindClose(h);
}

/* --- YES/NO dialogi (overlay + yhtenäiset viivat) --- */
int confirm_yesno(const char *title, const char *msg)
{
    int w = 60;
    int h = 9;
    int x0 = (COLS - w) / 2;
    int y0 = (LINES - h) / 2;

    /* Koko ruudun overlay -> ei “reunojen kusemista” taustan takia */
    WINDOW *ov = newwin(LINES, COLS, 0, 0);
    keypad(ov, TRUE);
    wbkgd(ov, ' ');

    /* Split msg kahteen riviin \n kohdalta (max 2 riviä) */
    const char *nl = strchr(msg, '\n');
    char line1[256], line2[256];

    if (nl) {
        size_t n1 = (size_t)(nl - msg);
        if (n1 >= sizeof(line1)) n1 = sizeof(line1) - 1;
        memcpy(line1, msg, n1);
        line1[n1] = '\0';

        strncpy(line2, nl + 1, sizeof(line2) - 1);
        line2[sizeof(line2) - 1] = '\0';
    } else {
        strncpy(line1, msg, sizeof(line1) - 1);
        line1[sizeof(line1) - 1] = '\0';
        line2[0] = '\0';
    }

    int sel = 0; /* 0=YES, 1=NO */

    while (1)
    {
        werase(ov);

        draw_box_unicode(ov, y0, x0, h, w);

        mvwprintw(ov, y0 + 1, x0 + 2, "%s", title);
        draw_hline_unicode(ov, y0 + 2, x0 + 1, w - 2);

        mvwprintw(ov, y0 + 4, x0 + 2, "%s", line1);
        if (line2[0] != '\0')
            mvwprintw(ov, y0 + 5, x0 + 2, "%s", line2);

        if (sel == 0) wattron(ov, A_REVERSE);
        mvwprintw(ov, y0 + 6, x0 + w/2 - 12, "  YES  ");
        if (sel == 0) wattroff(ov, A_REVERSE);

        if (sel == 1) wattron(ov, A_REVERSE);
        mvwprintw(ov, y0 + 6, x0 + w/2 + 4, "  NO   ");
        if (sel == 1) wattroff(ov, A_REVERSE);

        wrefresh(ov);

        int ch = wgetch(ov);

        if (ch == KEY_LEFT || ch == KEY_RIGHT || ch == KEY_UP || ch == KEY_DOWN || ch == '\t')
            sel = 1 - sel;
        else if (ch == 'y' || ch == 'Y') { delwin(ov); touchwin(stdscr); refresh(); return 1; }
        else if (ch == 'n' || ch == 'N') { delwin(ov); touchwin(stdscr); refresh(); return 0; }
        else if (ch == '\n' || ch == KEY_ENTER) {
            int ret = (sel == 0);
            delwin(ov);
            touchwin(stdscr);
            refresh();
            return ret;
        }
        else if (ch == 27) { delwin(ov); touchwin(stdscr); refresh(); return 0; } /* ESC */
    }
}

/* --- Unicode “yhtenäiset viivat” piirtimet --- */
static void draw_box_unicode(WINDOW *w, int y, int x, int h, int wid)
{
    /* ┌ ┐ └ ┘ ─ │ */
    mvwaddwstr(w, y, x,             L"┌");
    for (int i = 1; i < wid - 1; i++) mvwaddwstr(w, y, x + i, L"─");
    mvwaddwstr(w, y, x + wid - 1,   L"┐");

    for (int i = 1; i < h - 1; i++) {
        mvwaddwstr(w, y + i, x,           L"│");
        mvwaddwstr(w, y + i, x + wid - 1, L"│");
    }

    mvwaddwstr(w, y + h - 1, x,           L"└");
    for (int i = 1; i < wid - 1; i++) mvwaddwstr(w, y + h - 1, x + i, L"─");
    mvwaddwstr(w, y + h - 1, x + wid - 1, L"┘");
}

static void draw_hline_unicode(WINDOW *w, int y, int x, int len)
{
    for (int i = 0; i < len; i++)
        mvwaddwstr(w, y, x + i, L"─");
}
