/* Runtime catalogs for the menu and battle texture labels. */
#include "menu_translation.h"
#include "menu_text.h"
#include "ui_text_extract.h"
#include "ui_text_translate.h"
#ifndef _WIN32
#include <dirent.h>
#else
#include <io.h>
#endif
#include <pthread.h>
#include <stdint.h>
#include <limits.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <sys/stat.h>
#include <unistd.h>
#ifdef _WIN32
#include <direct.h>
#include <windows.h> /* GetModuleFileNameA */
#elif defined(__APPLE__)
#include <mach-o/dyld.h> /* _NSGetExecutablePath */
#endif

enum { TR_MAX_FILE = 1024 * 1024, TR_MAX_ROWS = 640, TR_MAX_GROUPS = 160 };
enum { TR_PORT_TEXT_COUNT = 130, TR_PORT_TEXT_MAX = 1024, TR_PORT_MAX_FILE = 64 * 1024 };
static const struct { const char *id, *original; } kPortText[TR_PORT_TEXT_COUNT] = {
    {PORT_TEXT_QUIT_CONFIRM, "Close the game?\nUnsaved progress will be lost."},
    {PORT_TEXT_NET_CONNECTING, "Connecting to your rival..."},
    {PORT_TEXT_NET_RIVAL, "Your rival \"{name}\" is here! The match begins."},
    {PORT_TEXT_TBO_TITLE, "Tenkaichi Budokai Online"},
    {PORT_TEXT_TBO_HUB_QUICK, "Quick Match"},
    {PORT_TEXT_TBO_HUB_RANKED, "Ranked Match"},
    {PORT_TEXT_TBO_HUB_CREATE, "Create Room"},
    {PORT_TEXT_TBO_HUB_JOIN, "Join Room"},
    {PORT_TEXT_TBO_HUB_FRIENDS, "Friends"},
    {PORT_TEXT_TBO_HUB_TOURNAMENT, "Tournament"},
    {PORT_TEXT_TBO_HUB_REPLAYS, "Replays"},
    {PORT_TEXT_TBO_HUB_LEADERBOARD, "Leaderboard"},
    {PORT_TEXT_TBO_HUB_PROFILE, "Profile"},
    {PORT_TEXT_TBO_PAGE_SEARCHING, "Searching"},
    {PORT_TEXT_TBO_PAGE_TRAIN, "Train While Waiting"},
    {PORT_TEXT_TBO_SAY_QUICK, "Fight whoever is searching right now! No time limit, and you can train while you wait."},
    {PORT_TEXT_TBO_SAY_RANKED, "Fights that move your rank, season by season. That one is coming soon!"},
    {PORT_TEXT_TBO_SAY_CREATE, "Open a room with your own rules, then give your rival its code. No port forwarding needed!"},
    {PORT_TEXT_TBO_SAY_JOIN, "Got a room code from your rival? Type it in and jump right into their room!"},
    {PORT_TEXT_TBO_SAY_FRIENDS, "Your rivals, who is online, and invites. That one is coming soon!"},
    {PORT_TEXT_TBO_SAY_TOURNAMENT, "The Tenkaichi Budokai itself: brackets of 4 to 16 players. That one is coming soon!"},
    {PORT_TEXT_TBO_SAY_REPLAYS, "Your last matches, saved to watch again. That one is coming soon!"},
    {PORT_TEXT_TBO_SAY_LEADERBOARD, "The strongest in the world, among your friends, and with each fighter. Coming soon!"},
    {PORT_TEXT_TBO_SAY_PROFILE, "The name your rivals see."},
    {PORT_TEXT_TBO_TIME_60, "60 SECONDS"},
    {PORT_TEXT_TBO_TIME_90, "90 SECONDS"},
    {PORT_TEXT_TBO_TIME_180, "180 SECONDS"},
    {PORT_TEXT_TBO_TIME_240, "240 SECONDS"},
    {PORT_TEXT_TBO_TIME_NOLIMIT, "NO LIMIT"},
    {PORT_TEXT_TBO_NOTICE_RIVAL, "Tenkaichi Budokai Online: your opponent is here! The match begins."},
    {PORT_TEXT_TBO_NOTICE_CLOSED, "Tenkaichi Budokai Online: the room was closed."},
    {PORT_TEXT_TBO_SOON, "SOON"},
    {PORT_TEXT_TBO_BTN_DELETE, "DELETE"},
    {PORT_TEXT_TBO_BTN_PASTE, "PASTE"},
    {PORT_TEXT_TBO_BTN_JOIN, "JOIN"},
    {PORT_TEXT_TBO_BTN_DONE, "DONE"},
    {PORT_TEXT_TBO_BTN_SPACE, "SPACE"},
    {PORT_TEXT_TBO_BTN_BACK, "BACK"},
    {PORT_TEXT_TBO_PROMPT_CHANGE, "Change"},
    {PORT_TEXT_TBO_PROMPT_NEXT, "Next"},
    {PORT_TEXT_TBO_PROMPT_BACK, "Back"},
    {PORT_TEXT_TBO_PROMPT_ENTER, "Enter"},
    {PORT_TEXT_TBO_PROMPT_COPY, "Copy Code"},
    {PORT_TEXT_TBO_PROMPT_CANCEL, "Cancel"},
    {PORT_TEXT_TBO_PROMPT_OPEN, "Open Room"},
    {PORT_TEXT_TBO_PROMPT_TRAIN, "Start Training"},
    {PORT_TEXT_TBO_PROMPT_SEARCH, "Search"},
    {PORT_TEXT_TBO_CR_SETTINGS, "ROOM SETTINGS"},
    {PORT_TEXT_TBO_CR_BATTLE, "Battle"},
    {PORT_TEXT_TBO_CR_TIME, "Time Limit"},
    {PORT_TEXT_TBO_CR_DELAY, "Input Delay"},
    {PORT_TEXT_TBO_CR_ROLL, "Rollback"},
    {PORT_TEXT_TBO_CR_OPEN, "Open Room"},
    {PORT_TEXT_TBO_VAL_1V1, "ONE ON ONE"},
    {PORT_TEXT_TBO_VAL_TEAMS, "TEAMS (UP TO 5)"},
    {PORT_TEXT_TBO_VAL_TEAMS_SHORT, "TEAMS"},
    {PORT_TEXT_TBO_VAL_AUTO, "AUTOMATIC"},
    {PORT_TEXT_TBO_VAL_N_FRAME, "%d FRAME"},
    {PORT_TEXT_TBO_VAL_N_FRAMES, "%d FRAMES"},
    {PORT_TEXT_TBO_VAL_NOT_PC, "NOT ON THIS PC"},
    {PORT_TEXT_TBO_VAL_OFF, "OFF"},
    {PORT_TEXT_TBO_VAL_UP_TO, "UP TO %d FRAMES"},
    {PORT_TEXT_TBO_HELP_CREATE, "Set the rules of your tournament! Then your rival joins with the room code."},
    {PORT_TEXT_TBO_HELP_DELAY, "How late your moves come out. Automatic measures the line when the match starts."},
    {PORT_TEXT_TBO_HELP_ROLL, "Your moves come out at once and the game corrects itself. More frames cope with a slower line."},
    {PORT_TEXT_TBO_HELP_JOIN, "Enter your rival's room code, or Paste the one they sent you! Then Join."},
    {PORT_TEXT_TBO_HELP_WAIT, "Looking for a worthy challenger... Warm up in Training meanwhile!"},
    {PORT_TEXT_TBO_HELP_WAIT_OK, "Here we go! The match is starting!"},
    {PORT_TEXT_TBO_HELP_BLOCK, "Sorry! Online fights have to be fair, so only the game straight off the disc can play."},
    {PORT_TEXT_TBO_HELP_SPAR, "Warm up at the tournament while we search! Pick yourself and a sparring partner."},
    {PORT_TEXT_TBO_HELP_QUICK, "Pick a battle and we'll find you a rival! No time limit, and the game runs a little faster."},
    {PORT_TEXT_TBO_HELP_NAME, "Pick your letters, then Done! Square deletes, Start finishes."},
    {PORT_TEXT_TBO_HELP_PROFILE, "Show everyone who you are! This is the name your rivals see."},
    {PORT_TEXT_TBO_WAIT_QM, "QUICK MATCH"},
    {PORT_TEXT_TBO_WAIT_PAIRS1, "THE SERVICE PAIRS YOU"},
    {PORT_TEXT_TBO_WAIT_PAIRS2, "WITH THE NEXT RIVAL"},
    {PORT_TEXT_TBO_WAIT_N_QUEUE, "%d IN THIS QUEUE"},
    {PORT_TEXT_TBO_WAIT_CODE, "ROOM CODE"},
    {PORT_TEXT_TBO_WAIT_GET_CODE, "GETTING A CODE..."},
    {PORT_TEXT_TBO_WAIT_GIVE1, "GIVE THIS CODE TO"},
    {PORT_TEXT_TBO_WAIT_GIVE2, "YOUR RIVAL"},
    {PORT_TEXT_TBO_WAIT_ASK1, "ASKING THE"},
    {PORT_TEXT_TBO_WAIT_ASK2, "MATCHMAKING SERVICE"},
    {PORT_TEXT_TBO_WAIT_OTHER, "ANOTHER VERSION TRIED"},
    {PORT_TEXT_TBO_WAIT_JOINING, "JOINING ROOM"},
    {PORT_TEXT_TBO_WAIT_LOOK1, "LOOKING FOR"},
    {PORT_TEXT_TBO_WAIT_LOOK2, "THE HOST..."},
    {PORT_TEXT_TBO_WAIT_HERE, "YOUR RIVAL IS HERE!"},
    {PORT_TEXT_TBO_WAIT_SEARCH, "SEARCHING...  %d:%02d"},
    {PORT_TEXT_TBO_NAME_TYPE, "TYPE YOUR NAME"},
    {PORT_TEXT_TBO_SPAR_TITLE, "SPARRING AT THE TOURNAMENT"},
    {PORT_TEXT_TBO_SPAR_YOU, "You"},
    {PORT_TEXT_TBO_SPAR_PARTNER, "Partner"},
    {PORT_TEXT_TBO_SPAR_START, "Start Training"},
    {PORT_TEXT_TBO_SPAR_QM, "QUICK MATCH SEARCH"},
    {PORT_TEXT_TBO_SPAR_OPEN, "YOUR ROOM IS OPEN"},
    {PORT_TEXT_TBO_SPAR_JOIN, "JOINING A ROOM"},
    {PORT_TEXT_TBO_SPAR_SEARCH, "SEARCHING  %d:%02d"},
    {PORT_TEXT_TBO_SPAR_START1, "THE MATCH STARTS THE MOMENT"},
    {PORT_TEXT_TBO_SPAR_START2, "YOUR RIVAL ARRIVES"},
    {PORT_TEXT_TBO_QK_PICK, "PICK YOUR BATTLE"},
    {PORT_TEXT_TBO_QK_BATTLE, "Battle"},
    {PORT_TEXT_TBO_QK_SEARCH, "Search"},
    {PORT_TEXT_TBO_QK_WAITING, "WAITING RIGHT NOW"},
    {PORT_TEXT_TBO_QK_TEAMS, "Teams"},
    {PORT_TEXT_TBO_QK_1V1, "One on One"},
    {PORT_TEXT_TBO_QK_N_WAIT, "%d WAITING"},
    {PORT_TEXT_TBO_QK_SPEED, "NO TIME LIMIT - %d%% SPEED"},
    {PORT_TEXT_TBO_PR_CARD, "PLAYER CARD"},
    {PORT_TEXT_TBO_PR_NAME, "Name"},
    {PORT_TEXT_TBO_BLOCK_TITLE, "Cannot Go Online"},
    {PORT_TEXT_TBO_BLOCK_L1, "Online matches are only between"},
    {PORT_TEXT_TBO_BLOCK_L2, "two unmodded games."},
    {PORT_TEXT_TBO_BLOCK_CHANGED, "Changed: "},
    {PORT_TEXT_TBO_BLOCK_NOT_DISC, "Your game data is not the disc's."},
    {PORT_TEXT_TBO_BLOCK_CHECK, "Your game data could not be checked."},
    {PORT_TEXT_TBO_BLOCK_MODS, "Take out what you added in gamedata/mods."},
    {PORT_TEXT_TBO_BLOCK_FIGHTER, "(Fighter mods switch off by themselves online.)"},
    {PORT_TEXT_TBO_NOTE_CHECK, "CHECKING YOUR GAME DATA AGAINST THE DISC... TRY AGAIN IN A MOMENT"},
    {PORT_TEXT_TBO_NOTE_NET, "THE NETWORK COULD NOT BE OPENED ON THIS PC"},
    {PORT_TEXT_TBO_NOTE_RELAY, "THE TWO GAMES COULD NOT REACH EACH OTHER, EVEN THROUGH THE RELAY"},
    {PORT_TEXT_TBO_NOTE_ROOM, "THE ROOM COULD NOT BE OPENED"},
    {PORT_TEXT_TBO_NOTE_COPIED, "COPIED: PASTE THE CODE TO YOUR RIVAL"},
    {PORT_TEXT_TBO_NOTE_SOON, "NOT OPEN YET: IT NEEDS MORE FROM THE MATCHMAKING SERVICE THAN ROOM CODES"},
    {PORT_TEXT_TBO_NOTE_CODE, "A ROOM CODE HAS SIX LETTERS AND NUMBERS"},
    {PORT_TEXT_TBO_NOTE_SAVED, "SAVED: YOUR RIVALS WILL SEE THIS NAME"},
    {PORT_TEXT_TBO_SPAR_SIGN_SEARCH, "Searching for a rival...  %d:%02d"},
    {PORT_TEXT_TBO_SPAR_SIGN_QM, "Quick Match: "},
    {PORT_TEXT_TBO_SPAR_SIGN_ROOM, "Your room: "},
    {PORT_TEXT_TBO_SPAR_SIGN_JOIN, "Joining room "}
};
typedef struct TrPortCatalog {
    char text[TR_PORT_TEXT_COUNT][TR_PORT_TEXT_MAX];
    unsigned char present[TR_PORT_TEXT_COUNT];
} TrPortCatalog;
typedef struct TrRow {
    char *id, *original, *text, *from;
    const char *resolved;
} TrRow;
typedef struct TrGroup { char *id, *text; } TrGroup;
typedef struct TrParser { const char *p, *end; } TrParser;

static TrRow sRows[TR_MAX_ROWS];
static TrGroup sGroups[TR_MAX_GROUPS];
static const char *sStage[36], *sSong[24], *sItem, *sItemOriginal;
static char sMain[1024], sMainOriginal[1024], sLocale[16];
static char sPortText[TR_PORT_TEXT_COUNT][TR_PORT_TEXT_MAX];
static int sNRows, sNGroups, sReady;
static int sBootChoiceMade;
static pthread_mutex_t sLock = PTHREAD_MUTEX_INITIALIZER;
extern void Port_SettingText(const char *name, const char *def, char *out, int size);
extern int Port_Setting(const char *name, int def);
extern void Port_SettingSave(const char *name, int value);
extern void Port_SettingTextSave(const char *name, const char *value);
extern void Port_SettingsWrite(void);

static void tr_space(TrParser *p) {
    while (p->p < p->end && (*p->p == ' ' || *p->p == '\n' || *p->p == '\r' || *p->p == '\t')) p->p++;
}
static int tr_take(TrParser *p, char c) {
    tr_space(p);
    if (p->p >= p->end || *p->p != c) return 0;
    p->p++;
    return 1;
}
static int tr_hex(char c) {
    if (c >= '0' && c <= '9') return c - '0';
    if (c >= 'a' && c <= 'f') return c - 'a' + 10;
    if (c >= 'A' && c <= 'F') return c - 'A' + 10;
    return -1;
}
static int tr_u4(TrParser *p, unsigned *out) {
    int i, h;
    *out = 0;
    for (i = 0; i < 4; i++) {
        if (p->p >= p->end || (h = tr_hex(*p->p++)) < 0) return 0;
        *out = (*out << 4) | (unsigned)h;
    }
    return 1;
}
static char *tr_string(TrParser *p) {
    char *out, *w;
    const char *q;
    if (!tr_take(p, '"')) return NULL;
    /* As much as the string can need: up to its closing quote (an escape keeps the next character from closing it;
       decoded, an escape is never longer than written). It was the rest of the file: with 20,000 lines kept, tens of
       gigabytes asked for in all. Linux and Wine give such memory only when it is written to; Windows counts all of
       it against memory and the page file, and on a machine with less of them the catalog was refused ("invalid UI
       translation catalog": the memory card box in English). */
    for (q = p->p; q < p->end && *q != '"'; q += *q == '\\' && q + 1 < p->end ? 2 : 1) {
    }
    out = malloc((size_t)(q - p->p) + 1);
    if (out == NULL) return NULL;
    w = out;
    while (p->p < p->end) {
        unsigned char c = (unsigned char)*p->p++;
        if (c == '"') { *w = 0; return out; }
        if (c < 32) break;
        if (c == '\\') {
            unsigned u, low;
            if (p->p >= p->end) break;
            c = (unsigned char)*p->p++;
            if (c == 'u') {
                if (!tr_u4(p, &u)) break;
                if (u >= 0xD800 && u <= 0xDBFF) {
                    if (p->end - p->p < 2 || p->p[0] != '\\' || p->p[1] != 'u') break;
                    p->p += 2;
                    if (!tr_u4(p, &low) || low < 0xDC00 || low > 0xDFFF) break;
                    u = 0x10000 + ((u - 0xD800) << 10) + low - 0xDC00;
                } else if (u >= 0xDC00 && u <= 0xDFFF) break;
                if (u == 0) break;
                if (u < 0x80) *w++ = (char)u;
                else if (u < 0x800) { *w++ = (char)(0xC0 | (u >> 6)); *w++ = (char)(0x80 | (u & 63)); }
                else if (u < 0x10000) { *w++ = (char)(0xE0 | (u >> 12)); *w++ = (char)(0x80 | ((u >> 6) & 63)); *w++ = (char)(0x80 | (u & 63)); }
                else { *w++ = (char)(0xF0 | (u >> 18)); *w++ = (char)(0x80 | ((u >> 12) & 63)); *w++ = (char)(0x80 | ((u >> 6) & 63)); *w++ = (char)(0x80 | (u & 63)); }
                continue;
            }
            if (c == 'n') c = '\n';
            else if (c == 'r') c = '\r';
            else if (c == 't') c = '\t';
            else if (c == 'b') c = '\b';
            else if (c == 'f') c = '\f';
            else if (c != '"' && c != '\\' && c != '/') break;
        }
        *w++ = (char)c;
    }
    free(out);
    return NULL;
}

static int tr_skip(TrParser *p, int depth) {
    char *s;
    char close;
    if (depth > 16) return 0;
    tr_space(p);
    if (p->p >= p->end) return 0;
    if (*p->p == '"') { s = tr_string(p); if (s == NULL) return 0; free(s); return 1; }
    if (*p->p == '{' || *p->p == '[') {
        close = *p->p++ == '{' ? '}' : ']';
        if (tr_take(p, close)) return 1;
        do {
            if (close == '}') { s = tr_string(p); if (s == NULL || !tr_take(p, ':')) { free(s); return 0; } free(s); }
            if (!tr_skip(p, depth + 1)) return 0;
            if (tr_take(p, close)) return 1;
        } while (tr_take(p, ','));
        return 0;
    }
    while (p->p < p->end && *p->p != ',' && *p->p != '}' && *p->p != ']' && *p->p != ' ' && *p->p != '\n') p->p++;
    return 1;
}

static void tr_clear(void) {
    int i;
    for (i = 0; i < sNRows; i++) { free(sRows[i].id); free(sRows[i].original); free(sRows[i].text); free(sRows[i].from); }
    for (i = 0; i < sNGroups; i++) { free(sGroups[i].id); free(sGroups[i].text); }
    memset(sRows, 0, sizeof(sRows));
    memset(sGroups, 0, sizeof(sGroups));
    memset(sStage, 0, sizeof(sStage));
    memset(sSong, 0, sizeof(sSong));
    sNRows = sNGroups = 0;
    __atomic_store_n(&sReady, 0, __ATOMIC_RELEASE);
    sItem = sItemOriginal = NULL; sMain[0] = sMainOriginal[0] = 0; sLocale[0] = 0;
    memset(sPortText, 0, sizeof(sPortText));
}
static TrRow *tr_find(const char *id) {
    int i;
    for (i = 0; i < sNRows; i++) if (strcmp(sRows[i].id, id) == 0) return &sRows[i];
    return NULL;
}
static int tr_row(TrParser *p, char *id) {
    TrRow *r;
    char *key, *value;
    if (sNRows == TR_MAX_ROWS || tr_find(id) != NULL || !tr_take(p, '{')) return 0;
    r = &sRows[sNRows++]; r->id = id;
    do {
        key = tr_string(p);
        if (key == NULL || !tr_take(p, ':')) { free(key); return 0; }
        if (strcmp(key, "original") == 0 || strcmp(key, "text") == 0 || strcmp(key, "text_from") == 0) {
            value = tr_string(p);
            if (value == NULL) { free(key); return 0; }
            if (strcmp(key, "original") == 0) { free(r->original); r->original = value; }
            else if (strcmp(key, "text") == 0) { free(r->text); r->text = value; }
            else { free(r->from); r->from = value; }
        } else if (!tr_skip(p, 0)) { free(key); return 0; }
        free(key);
        if (tr_take(p, '}')) break;
    } while (tr_take(p, ','));
    return r->original != NULL && r->text != NULL && r->original[0] != 0;
}
static int tr_parse(const char *buf, size_t size, const char *locale) {
    TrParser p = {buf, buf + size};
    char *key, *id;
    int labels = 0, schema = 0, matching = 0;
    if (!tr_take(&p, '{')) return 0;
    do {
        key = tr_string(&p);
        if (key == NULL || !tr_take(&p, ':')) { free(key); return 0; }
        if (strcmp(key, "labels") == 0) {
            if (!tr_take(&p, '{')) { free(key); return 0; }
            if (!tr_take(&p, '}')) do {
                id = tr_string(&p);
                if (id == NULL || !tr_take(&p, ':')) { free(id); free(key); return 0; }
                if (!tr_row(&p, id)) { free(key); return 0; }
                if (tr_take(&p, '}')) break;
            } while (tr_take(&p, ','));
            labels = sNRows > 0;
        } else if (strcmp(key, "locale") == 0) {
            char *v = tr_string(&p);
            matching = v != NULL && strcmp(v, locale) == 0;
            free(v);
        } else if (strcmp(key, "schema_version") == 0) {
            tr_space(&p);
            schema = p.p < p.end && *p.p == '1';
            if (schema) p.p++;
        } else if (!tr_skip(&p, 0)) { free(key); return 0; }
        free(key);
        if (tr_take(&p, '}')) { tr_space(&p); return labels && schema && matching && p.p == p.end; }
    } while (tr_take(&p, ','));
    return 0;
}
/* Port-owned strings use the same JSON shape as label translations, but have their own IDs and limits. */
static int tr_port_row(TrParser *p, const char *id, TrPortCatalog *catalog) {
    char *key, *original = NULL, *text = NULL;
    int closed = 0, i, ok = 0;
    if (!tr_take(p, '{')) return 0;
    if (tr_take(p, '}')) closed = 1;
    else do {
        key = tr_string(p);
        if (key == NULL || !tr_take(p, ':')) { free(key); goto done; }
        if (!strcmp(key, "original")) {
            free(original); original = tr_string(p);
            if (!original) { free(key); goto done; }
        } else if (!strcmp(key, "text")) {
            free(text); text = tr_string(p);
            if (!text) { free(key); goto done; }
        } else if (!tr_skip(p, 0)) { free(key); goto done; }
        free(key);
        if (tr_take(p, '}')) { closed = 1; break; }
    } while (tr_take(p, ','));
    if (!closed || !original || !text) goto done;
    for (i = 0; i < TR_PORT_TEXT_COUNT; i++) {
        if (!strcmp(id, kPortText[i].id) && !strcmp(original, kPortText[i].original) &&
            text[0] && strlen(text) < TR_PORT_TEXT_MAX) {
            snprintf(catalog->text[i], TR_PORT_TEXT_MAX, "%s", text);
            catalog->present[i] = 1;
        }
    }
    ok = 1;
done:
    free(original); free(text);
    return ok;
}
static int tr_port_parse(const char *buf, size_t size, const char *locale, TrPortCatalog *catalog) {
    TrParser p = {buf, buf + size};
    char *key, *id;
    int schema = 0, matching = 0, labels = 0, closed;
    memset(catalog, 0, sizeof(*catalog));
    if (!tr_take(&p, '{')) return 0;
    do {
        key = tr_string(&p);
        if (!key || !tr_take(&p, ':')) { free(key); return 0; }
        if (!strcmp(key, "labels")) {
            if (!tr_take(&p, '{')) { free(key); return 0; }
            closed = tr_take(&p, '}');
            if (!closed) do {
                id = tr_string(&p);
                if (!id || !tr_take(&p, ':')) { free(id); free(key); return 0; }
                { int rowOk = tr_port_row(&p, id, catalog);
                  if (!rowOk) { free(id); free(key); return 0; } }
                free(id);
                if (tr_take(&p, '}')) { closed = 1; break; }
            } while (tr_take(&p, ','));
            if (!closed) { free(key); return 0; }
            labels = 1;
        } else if (!strcmp(key, "locale")) {
            char *value = tr_string(&p);
            matching = value && !strcmp(value, locale);
            free(value);
        } else if (!strcmp(key, "schema_version")) {
            tr_space(&p);
            schema = p.p < p.end && *p.p == '1';
            if (schema) p.p++; else { free(key); return 0; }
        } else if (!tr_skip(&p, 0)) { free(key); return 0; }
        free(key);
        if (tr_take(&p, '}')) { tr_space(&p); return schema && matching && labels && p.p == p.end; }
    } while (tr_take(&p, ','));
    return 0;
}
static int tr_port_read(const char *outDir, const char *locale, TrPortCatalog *catalog) {
    char path[1200], *data;
    FILE *fp;
    long size;
    int ok;
    if (snprintf(path, sizeof(path), "%s/translations/port/%s.json", outDir, locale) >= (int)sizeof(path)) return 0;
    fp = fopen(path, "rb");
    if (!fp) return 0;
    if (fseek(fp, 0, SEEK_END) || (size = ftell(fp)) < 1 || size > TR_PORT_MAX_FILE || fseek(fp, 0, SEEK_SET)) { fclose(fp); return 0; }
    data = malloc((size_t)size + 1);
    if (!data) { fclose(fp); return 0; }
    ok = fread(data, 1, (size_t)size, fp) == (size_t)size;
    fclose(fp);
    if (ok) { data[size] = 0; ok = tr_port_parse(data, (size_t)size, locale, catalog); }
    free(data);
    return ok;
}
static void tr_port_load(const char *outDir, const char *locale) {
    TrPortCatalog catalog;
    int i;
    if (tr_port_read(outDir, "en", &catalog)) {
        for (i = 0; i < TR_PORT_TEXT_COUNT; i++) if (catalog.present[i])
            snprintf(sPortText[i], sizeof(sPortText[i]), "%s", catalog.text[i]);
    }
    if (strcmp(locale, "en") && tr_port_read(outDir, locale, &catalog)) {
        for (i = 0; i < TR_PORT_TEXT_COUNT; i++) if (catalog.present[i])
            snprintf(sPortText[i], sizeof(sPortText[i]), "%s", catalog.text[i]);
    }
}
static const char *tr_resolve(TrRow *r, int depth) {
    TrRow *src;
    if (r == NULL || depth > 8) return NULL;
    if (r->resolved != NULL) return r->resolved;
    src = r->from != NULL ? tr_find(r->from) : NULL;
    return r->resolved = r->from != NULL ? tr_resolve(src, depth + 1) :
                         r->text[0] != 0 ? r->text : r->original;
}
static int tr_prepare(void) {
    int i, j, max;
    char id[96];
    for (i = 0; i < sNRows; i++) {
        TrRow *r = &sRows[i];
        if (tr_resolve(r, 0) == NULL || strlen(r->resolved) > 255 || strchr(r->resolved, '|') != NULL ||
            strlen(r->original) > 255 || strchr(r->original, '|') != NULL) return 0;
        for (j = 0; j < sNGroups; j++) if (strncmp(r->id, sGroups[j].id, strlen(sGroups[j].id)) == 0 && r->id[strlen(sGroups[j].id)] == '/') break;
        if (j == sNGroups) {
            const char *slash = strchr(r->id, '/');
            if (slash == NULL || sNGroups == TR_MAX_GROUPS) return 0;
            sGroups[sNGroups].id = malloc((size_t)(slash - r->id) + 1);
            if (sGroups[sNGroups].id == NULL) return 0;
            memcpy(sGroups[sNGroups].id, r->id, (size_t)(slash - r->id));
            sGroups[sNGroups].id[slash - r->id] = 0;
            sNGroups++;
        }
    }
    for (i = 0; i < sNGroups; i++) {
        size_t bytes = 1;
        max = -1;
        for (j = 0; j < sNRows; j++) {
            int t, row, n = 0;
            if (sscanf(sRows[j].id, "%95[^/]/%d/%d%n", id, &t, &row, &n) == 3 && n == (int)strlen(sRows[j].id) && strcmp(id, sGroups[i].id) == 0 && t == 0 && row >= 0 && row < 40) {
                if (row > max) max = row;
                bytes += strlen(sRows[j].original) + 1;
            }
        }
        if (max < 0) return 0;
        sGroups[i].text = calloc(bytes + (size_t)max + 1, 1);
        if (sGroups[i].text == NULL) return 0;
        for (j = 0; j <= max; j++) {
            TrRow *r;
            snprintf(id, sizeof(id), "%s/0/%d", sGroups[i].id, j);
            r = tr_find(id);
            if (j != 0) strcat(sGroups[i].text, "|");
            if (r != NULL) strcat(sGroups[i].text, r->original);
        }
    }
    for (i = 0; i < 36; i++) {
        snprintf(id, sizeof(id), "stage/%d/%d", i / 4, i % 4);
        if (tr_find(id) == NULL) return 0;
        sStage[i] = tr_find(id)->original;
    }
    for (i = 0; i < 24; i++) {
        snprintf(id, sizeof(id), "song/%d/%d", i / 8, i % 8);
        if (tr_find(id) == NULL) return 0;
        sSong[i] = tr_find(id)->original;
    }
    if (tr_find("timer-white/0/10") == NULL || tr_find("win-counter/0/0") == NULL ||
        tr_find("combo-hits/0/0") == NULL) return 0;
    sMain[0] = sMainOriginal[0] = 0;
    for (i = 0; i < 12; i++) {
        TrRow *r;
        snprintf(id, sizeof(id), "main-6/%d/%d", i / 8, i % 8);
        r = tr_find(id);
        if (r == NULL) return 0;
        if (i == 11) { sItem = r->resolved; sItemOriginal = r->original; }
        else {
            if (strlen(sMain) + strlen(r->resolved) + 2 >= sizeof(sMain) ||
                strlen(sMainOriginal) + strlen(r->original) + 2 >= sizeof(sMainOriginal)) return 0;
            if (i != 0) { strcat(sMain, "|"); strcat(sMainOriginal, "|"); }
            strcat(sMain, r->resolved);
            strcat(sMainOriginal, r->original);
        }
    }
    return 1;
}
static int tr_read(const char *path, const char *locale) {
    FILE *fp = fopen(path, "rb");
    long n;
    char *buf;
    int ok;
    if (fp == NULL) return 0;
    if (fseek(fp, 0, SEEK_END) != 0 || (n = ftell(fp)) < 1 || n > TR_MAX_FILE || fseek(fp, 0, SEEK_SET) != 0) { fclose(fp); return 0; }
    buf = malloc((size_t)n + 1);
    if (buf == NULL) { fclose(fp); return 0; }
    ok = fread(buf, 1, (size_t)n, fp) == (size_t)n;
    fclose(fp);
    if (ok) { buf[n] = 0; ok = tr_parse(buf, (size_t)n, locale) && tr_prepare(); }
    free(buf);
    return ok;
}
static int tr_locale_name(const char *name, char *locale, size_t cap) {
    size_t n = strlen(name), i, first;
    if (n < 7 || strcmp(name + n - 5, ".json") != 0 || n - 5 >= cap) return 0;
    first = n - 5;
    for (i = 0; i < first && name[i] >= 'a' && name[i] <= 'z'; i++) {}
    if (i < 2 || i > 3) return 0;
    if (i < first) {
        size_t j;
        if (name[i++] != '-' || first - i < 2 || first - i > 3) return 0;
        for (j = i; j < first; j++) if (!((name[j] >= 'A' && name[j] <= 'Z') || (name[j] >= 'a' && name[j] <= 'z'))) return 0;
    }
    memcpy(locale, name, first);
    locale[first] = 0;
    return 1;
}
static int tr_copy_one(const char *source, const char *destination) {
    FILE *in, *out;
    struct stat st;
    char tmp[1300], buf[8192];
    size_t n;
    int ok = 1;
    if (stat(destination, &st) == 0) return 1;
    in = fopen(source, "rb");
    if (in == NULL) return 0;
    snprintf(tmp, sizeof(tmp), "%s.%ld.part", destination, (long)getpid());
    out = fopen(tmp, "wb");
    if (out == NULL) { fclose(in); return 0; }
    while ((n = fread(buf, 1, sizeof(buf), in)) != 0) if (fwrite(buf, 1, n, out) != n) { ok = 0; break; }
    if (ferror(in)) ok = 0;
    fclose(in);
    if (fclose(out) != 0) ok = 0;
    if (ok) ok = rename(tmp, destination) == 0;
    if (!ok) unlink(tmp);
    return ok;
}
static void tr_copy_catalogs(const char *root, const char *kind, const char *destinationDir) {
    char source[1200], destination[1200], sourceDir[1200], locale[16];
    if (snprintf(sourceDir, sizeof(sourceDir), "%s/%s", root, kind) >= (int)sizeof(sourceDir)) return;
#ifdef _WIN32
    {
        struct _finddata_t entry;
        intptr_t handle;
        if (snprintf(source, sizeof(source), "%s/*.json", sourceDir) >= (int)sizeof(source)) return;
        handle = _findfirst(source, &entry);
        if (handle == -1) return;
        do {
            if (!tr_locale_name(entry.name, locale, sizeof(locale))) continue;
            if (snprintf(source, sizeof(source), "%s/%s", sourceDir, entry.name) >= (int)sizeof(source) ||
                snprintf(destination, sizeof(destination), "%s/%s", destinationDir, entry.name) >= (int)sizeof(destination)) continue;
            tr_copy_one(source, destination);
        } while (_findnext(handle, &entry) == 0);
        _findclose(handle);
    }
#else
    {
        DIR *dir = opendir(sourceDir);
        struct dirent *entry;
        if (!dir) return;
        while ((entry = readdir(dir)) != NULL) {
            if (!tr_locale_name(entry->d_name, locale, sizeof(locale))) continue;
            if (snprintf(source, sizeof(source), "%s/%s", sourceDir, entry->d_name) >= (int)sizeof(source) ||
                snprintf(destination, sizeof(destination), "%s/%s", destinationDir, entry->d_name) >= (int)sizeof(destination)) continue;
            tr_copy_one(source, destination);
        }
        closedir(dir);
    }
#endif
}
/* The folder of the bundled catalogs: from the working folder (a source checkout's port/translations, a release's
   translations), or else from the program's own folder (a release started from elsewhere: a shortcut, a terminal in
   another folder; a checkout's port/build/<program>), as the fonts are found (menu_text.c). 0: none. */
static int tr_bundled_dir(char *out, size_t size) {
    static const char *const kHere[] = {"port/translations", "translations"};
    static const char *const kByProgram[] = {"translations", "../translations"};
    char exe[1024], path[1300], *slash;
    long n;
    int i;
    for (i = 0; i < 2; i++) {
        snprintf(path, sizeof(path), "%s/labels/en.json", kHere[i]);
        if (access(path, 0) == 0) { snprintf(out, size, "%s", kHere[i]); return 1; }
    }
#ifdef _WIN32
    n = (long)GetModuleFileNameA(NULL, exe, sizeof(exe) - 1);
#elif defined(__APPLE__)
    { uint32_t cap = sizeof(exe) - 1; n = _NSGetExecutablePath(exe, &cap) == 0 ? (long)strlen(exe) : -1; }
#else
    n = (long)readlink("/proc/self/exe", exe, sizeof(exe) - 1);
#endif
    if (n <= 0 || n >= (long)sizeof(exe) - 1) return 0;
    exe[n] = 0;
    slash = strrchr(exe, '/');
#ifdef _WIN32
    if (strrchr(exe, '\\') > slash) slash = strrchr(exe, '\\');
#endif
    if (slash == NULL) return 0;
    slash[1] = 0;
    for (i = 0; i < 2; i++) {
        snprintf(path, sizeof(path), "%s%s/labels/en.json", exe, kByProgram[i]);
        if (access(path, 0) == 0) { snprintf(out, size, "%s%s", exe, kByProgram[i]); return 1; }
    }
    return 0;
}
static void tr_copy_all(const char *outDir, const char *labelsDir) {
    const char *src = NULL;
    char catalogDir[1200], bundled[1100];
    int i;
    static const char *const extraCatalogs[] = {"ui", "port"};
    if (tr_bundled_dir(bundled, sizeof(bundled))) src = bundled;
    if (src == NULL) return;
    tr_copy_catalogs(src, "labels", labelsDir);
    for (i = 0; i < (int)(sizeof(extraCatalogs) / sizeof(extraCatalogs[0])); i++) {
        if (snprintf(catalogDir, sizeof(catalogDir), "%s/translations/%s", outDir, extraCatalogs[i]) >= (int)sizeof(catalogDir)) continue;
#ifdef _WIN32
        _mkdir(catalogDir);
#else
        mkdir(catalogDir, 0755);
#endif
        tr_copy_catalogs(src, extraCatalogs[i], catalogDir);
    }
}
static int tr_translation_dir(const char *outDir, char *dir, size_t size) {
    char parent[1200];
    if (snprintf(parent, sizeof(parent), "%s/translations", outDir) >= (int)sizeof(parent) ||
        snprintf(dir, size, "%s/labels", parent) >= (int)size) return 0;
#ifdef _WIN32
    _mkdir(outDir);
    _mkdir(parent);
    _mkdir(dir);
#else
    mkdir(outDir, 0755);
    mkdir(parent, 0755);
    mkdir(dir, 0755);
#endif
    return 1;
}
/* Move catalogs from an older folder without replacing user edits in translations/labels. */
static void tr_migrate(const char *outDir, const char *dir) {
    char oldPath[1200], newPath[1200], locale[16];
#ifdef _WIN32
    struct _finddata_t entry;
    intptr_t handle;
    if (snprintf(oldPath, sizeof(oldPath), "%s/*.json", outDir) >= (int)sizeof(oldPath)) return;
    handle = _findfirst(oldPath, &entry);
    if (handle == -1) return;
    do {
        if (!tr_locale_name(entry.name, locale, sizeof(locale))) continue;
        if (snprintf(oldPath, sizeof(oldPath), "%s/%s", outDir, entry.name) >= (int)sizeof(oldPath) ||
            snprintf(newPath, sizeof(newPath), "%s/%s", dir, entry.name) >= (int)sizeof(newPath)) continue;
        if (access(newPath, 0) != 0) rename(oldPath, newPath);
    } while (_findnext(handle, &entry) == 0);
    _findclose(handle);
#else
    DIR *folder = opendir(outDir);
    struct dirent *entry;
    if (folder == NULL) return;
    while ((entry = readdir(folder)) != NULL) {
        if (!tr_locale_name(entry->d_name, locale, sizeof(locale))) continue;
        if (snprintf(oldPath, sizeof(oldPath), "%s/%s", outDir, entry->d_name) >= (int)sizeof(oldPath) ||
            snprintf(newPath, sizeof(newPath), "%s/%s", dir, entry->d_name) >= (int)sizeof(newPath)) continue;
        if (access(newPath, 0) != 0) rename(oldPath, newPath);
    }
    closedir(folder);
#endif
}
static void tr_migrate_all(const char *outDir, const char *labelsDir) {
    char oldDir[1200];
    /* translations/ui now holds disc UI/dialog text, not legacy label catalogs. */
    if (snprintf(oldDir, sizeof(oldDir), "%s/translations", outDir) < (int)sizeof(oldDir))
        tr_migrate(oldDir, labelsDir);
    tr_migrate(outDir, labelsDir);
}
static int tr_order(TrParser *p, int *out) {
    int value = 0;
    tr_space(p);
    if (p->p >= p->end || *p->p < '0' || *p->p > '9') return 0;
    do {
        int digit = *p->p++ - '0';
        if (value > (INT_MAX - digit) / 10) return 0;
        value = value * 10 + digit;
    } while (p->p < p->end && *p->p >= '0' && *p->p <= '9');
    *out = value;
    return 1;
}

static int tr_metadata(const char *path, const char *locale, MenuTranslationOption *out) {
    FILE *fp = fopen(path, "rb");
    TrParser p;
    char *buf, *key, *value;
    long bytes;
    int schema = 0, matching = 0, named = 0, labels = 0, ok = 0;
    if (fp == NULL) return 0;
    if (fseek(fp, 0, SEEK_END) != 0 || (bytes = ftell(fp)) < 1 || bytes > TR_MAX_FILE || fseek(fp, 0, SEEK_SET) != 0) { fclose(fp); return 0; }
    buf = malloc((size_t)bytes + 1);
    if (buf == NULL) { fclose(fp); return 0; }
    if (fread(buf, 1, (size_t)bytes, fp) != (size_t)bytes) { fclose(fp); free(buf); return 0; }
    fclose(fp);
    p.p = buf; p.end = buf + bytes;
    if (tr_take(&p, '{')) do {
        key = tr_string(&p);
        if (key == NULL || !tr_take(&p, ':')) { free(key); break; }
        if (strcmp(key, "name") == 0 || strcmp(key, "locale") == 0) {
            value = tr_string(&p);
            if (value == NULL) { free(key); break; }
            if (strcmp(key, "name") == 0 && value[0] != 0 && strlen(value) < sizeof(out->name)) {
                snprintf(out->name, sizeof(out->name), "%s", value); named = 1;
            }
            if (strcmp(key, "locale") == 0) matching = strcmp(value, locale) == 0;
            free(value);
        } else if (strcmp(key, "order") == 0) {
            if (!tr_order(&p, &out->order)) { free(key); break; }
        } else if (strcmp(key, "schema_version") == 0) {
            tr_space(&p);
            schema = p.p < p.end && *p.p == '1';
            if (schema) p.p++; else { free(key); break; }
        } else {
            if (strcmp(key, "labels") == 0) labels = 1;
            if (!tr_skip(&p, 0)) { free(key); break; }
        }
        free(key);
        if (tr_take(&p, '}')) { tr_space(&p); ok = schema && matching && named && labels && p.p == p.end; break; }
    } while (tr_take(&p, ','));
    free(buf);
    return ok;
}
static int tr_option(const char *dir, const char *name, MenuTranslationOption *out) {
    char path[1200], locale[16];
    if (!tr_locale_name(name, locale, sizeof(locale)) || snprintf(path, sizeof(path), "%s/%s", dir, name) >= (int)sizeof(path)) return 0;
    memset(out, 0, sizeof(*out));
    out->order = INT_MAX;
    if (!tr_metadata(path, locale, out)) return 0;
    snprintf(out->locale, sizeof(out->locale), "%s", locale);
    return 1;
}
/* The UI catalog is much larger than a label catalog. Its locale metadata is before "labels",
   so the language list can read just the header rather than every translated line. */
static int tr_ui_option(const char *dir, const char *name, MenuTranslationOption *out) {
    char path[1200], locale[16], buf[4096], *key, *value;
    TrParser p;
    FILE *fp;
    size_t n;
    int schema = 0, matching = 0, named = 0, labels = 0;
    if (!tr_locale_name(name, locale, sizeof(locale)) || snprintf(path, sizeof(path), "%s/%s", dir, name) >= (int)sizeof(path)) return 0;
    fp = fopen(path, "rb");
    if (!fp) return 0;
    n = fread(buf, 1, sizeof(buf), fp);
    fclose(fp);
    p.p = buf; p.end = buf + n;
    memset(out, 0, sizeof(*out)); out->order = INT_MAX;
    if (!tr_take(&p, '{')) return 0;
    do {
        key = tr_string(&p);
        if (!key || !tr_take(&p, ':')) { free(key); return 0; }
        if (!strcmp(key, "labels")) { labels = tr_take(&p, '{'); free(key); break; }
        if (!strcmp(key, "name") || !strcmp(key, "locale")) {
            value = tr_string(&p);
            if (!value) { free(key); return 0; }
            if (!strcmp(key, "name") && value[0] && strlen(value) < sizeof(out->name)) {
                snprintf(out->name, sizeof(out->name), "%s", value); named = 1;
            }
            if (!strcmp(key, "locale")) matching = !strcmp(value, locale);
            free(value);
        } else if (!strcmp(key, "order")) {
            if (!tr_order(&p, &out->order)) { free(key); return 0; }
        } else if (!strcmp(key, "schema_version")) {
            tr_space(&p); schema = p.p < p.end && *p.p == '1';
            if (schema) p.p++; else { free(key); return 0; }
        } else if (!tr_skip(&p, 0)) { free(key); return 0; }
        free(key);
        if (tr_take(&p, '}')) break;
    } while (tr_take(&p, ','));
    if (!schema || !matching || !named || !labels) return 0;
    snprintf(out->locale, sizeof(out->locale), "%s", locale);
    return 1;
}
static void tr_list_extra(const char *dirPath, MenuTranslationOption *out, int capacity, int *count, int large) {
#ifdef _WIN32
    struct _finddata_t entry;
    intptr_t handle;
    char pattern[1200];
    if (snprintf(pattern, sizeof(pattern), "%s/*.json", dirPath) >= (int)sizeof(pattern) ||
        (handle = _findfirst(pattern, &entry)) == -1) return;
    do {
        MenuTranslationOption option;
        int i;
        if (!(large ? tr_ui_option(dirPath, entry.name, &option) : tr_option(dirPath, entry.name, &option))) continue;
        for (i = 0; i < *count && strcmp(out[i].locale, option.locale) != 0; i++) {}
        if (i == *count && *count < capacity) out[(*count)++] = option;
    } while (_findnext(handle, &entry) == 0);
    _findclose(handle);
#else
    DIR *dir = opendir(dirPath);
    struct dirent *entry;
    if (!dir) return;
    while ((entry = readdir(dir)) != NULL) {
        MenuTranslationOption option;
        int i;
        if (!(large ? tr_ui_option(dirPath, entry->d_name, &option) : tr_option(dirPath, entry->d_name, &option))) continue;
        for (i = 0; i < *count && strcmp(out[i].locale, option.locale) != 0; i++) {}
        if (i == *count && *count < capacity) out[(*count)++] = option;
    }
    closedir(dir);
#endif
}
int MenuTranslation_List(const char *outDir, MenuTranslationOption *out, int capacity) {
    char dirPath[1200], extraDir[1200];
    int count = 0, i, j;
    if (outDir == NULL || out == NULL || capacity <= 0) return 0;
    if (!tr_translation_dir(outDir, dirPath, sizeof(dirPath))) return 0;
    tr_migrate_all(outDir, dirPath);
    tr_copy_all(outDir, dirPath);
    UiText_ExtractOnStartup(outDir);
#ifdef _WIN32
    {
        struct _finddata_t entry;
        intptr_t handle;
        char pattern[1200];
        snprintf(pattern, sizeof(pattern), "%s/*.json", dirPath);
        handle = _findfirst(pattern, &entry);
        if (handle != -1) {
            do { if (count < capacity && tr_option(dirPath, entry.name, &out[count])) count++; }
            while (_findnext(handle, &entry) == 0);
            _findclose(handle);
        }
    }
#else
    {
        DIR *dir = opendir(dirPath);
        struct dirent *entry;
        if (dir != NULL) {
            while ((entry = readdir(dir)) != NULL) if (count < capacity && tr_option(dirPath, entry->d_name, &out[count])) count++;
            closedir(dir);
        }
    }
#endif
    if (snprintf(extraDir, sizeof(extraDir), "%s/translations/ui", outDir) < (int)sizeof(extraDir))
        tr_list_extra(extraDir, out, capacity, &count, 1);
    if (snprintf(extraDir, sizeof(extraDir), "%s/translations/port", outDir) < (int)sizeof(extraDir))
        tr_list_extra(extraDir, out, capacity, &count, 0);
    /* Explicit order first, then name and locale for missing or duplicate order values. */
    for (i = 0; i < count; i++) for (j = i + 1; j < count; j++) {
        int byName = strcmp(out[j].name, out[i].name);
        if (out[j].order < out[i].order ||
            (out[j].order == out[i].order &&
             (byName < 0 || (byName == 0 && strcmp(out[j].locale, out[i].locale) < 0)))) {
            MenuTranslationOption swap = out[i]; out[i] = out[j]; out[j] = swap;
        }
    }
    return count;
}
int MenuTranslation_Available(MenuTranslationOption *out, int capacity) {
    const char *data = getenv("BT3_DATA"), *folder = getenv("BT3_MENU_FONT");
    char path[1200];
    if (folder == NULL || folder[0] == '\0') {
        snprintf(path, sizeof(path), "%s/game-text", data != NULL ? data : "gamedata");
        folder = path;
    }
    return MenuTranslation_List(folder, out, capacity);
}
int MenuTranslation_ShouldPrompt(const MenuTranslationOption *options, int count) {
    char saved[16];
    int i;
    const char *override = getenv("BT3_MENU_LANG");
    if (count <= 1 || options == NULL || (override != NULL && override[0] != '\0') ||
        getenv("BT3_NET_SESSION") != NULL || getenv("BT3_PAD_PLAY") != NULL ||
        getenv("BT3_PAD_TABLE") != NULL || getenv("BT3_SYNCTEST") != NULL) return 0;
    if (__atomic_load_n(&sBootChoiceMade, __ATOMIC_ACQUIRE)) return 0;
    if (Port_Setting("language_prompt_always", 0) != 0) return 1;
    Port_SettingText("menu_lang", "", saved, sizeof(saved));
    for (i = 0; i < count; i++) if (strcmp(options[i].locale, saved) == 0) return 0;
    return 1;
}
int MenuTranslation_BootPending(void) {
    MenuTranslationOption options[256];
    int count = MenuTranslation_Available(options, (int)(sizeof(options) / sizeof(options[0])));
    return MenuTranslation_ShouldPrompt(options, count);
}
void MenuTranslation_SaveChoice(const char *locale) {
    if (locale == NULL || locale[0] == '\0') return;
    Port_SettingTextSave("menu_lang", locale);
    __atomic_store_n(&sBootChoiceMade, 1, __ATOMIC_RELEASE);
    if (strcmp(locale, "en") != 0) {
        if (Port_Setting("menu_font_id", 0) == 0) Port_SettingSave("menu_font_id", MenuText_FirstFontId(MENU_TEXT_MENU));
        if (Port_Setting("battle_font_id", 0) == 0) Port_SettingSave("battle_font_id", MenuText_FirstFontId(MENU_TEXT_BATTLE));
    }
    Port_SettingsWrite();
}
int MenuTranslation_Load(const char *outDir, char *why, size_t whyLen) {
    const char *wanted = getenv("BT3_MENU_LANG");
    char saved[16];
    char path[1200], dirPath[1200], filename[32], locale[16], uiPath[1200], portPath[1200];
    int ok = 0;
    if (wanted == NULL || wanted[0] == 0) {
        Port_SettingText("menu_lang", "en", saved, sizeof(saved));
        wanted = saved;
    }
    if (snprintf(filename, sizeof(filename), "%s.json", wanted) >= (int)sizeof(filename) ||
        !tr_locale_name(filename, locale, sizeof(locale))) {
        if (why != NULL) snprintf(why, whyLen, "invalid menu language code");
        return 0;
    }
    if (outDir == NULL || outDir[0] == 0 || !tr_translation_dir(outDir, dirPath, sizeof(dirPath)) ||
        snprintf(path, sizeof(path), "%s/%s", dirPath, filename) >= (int)sizeof(path)) return 0;
    pthread_mutex_lock(&sLock);
    /* Text pointers are used by the renderer, so keep the first loaded catalog for this run. */
    if (__atomic_load_n(&sReady, __ATOMIC_ACQUIRE)) { ok = 1; goto done; }
    tr_clear();
    tr_migrate_all(outDir, dirPath);
    tr_copy_all(outDir, dirPath);
    if (!tr_read(path, locale)) {
        int hasUi = snprintf(uiPath, sizeof(uiPath), "%s/translations/ui/%s.json", outDir, locale) < (int)sizeof(uiPath) && access(uiPath, 0) == 0;
        int hasPort = snprintf(portPath, sizeof(portPath), "%s/translations/port/%s.json", outDir, locale) < (int)sizeof(portPath) && access(portPath, 0) == 0;
        /* A language can supply UI or port text alone; its menu labels use English. */
        tr_clear();
        if (!strcmp(locale, "en") || (!hasUi && !hasPort) ||
            snprintf(path, sizeof(path), "%s/en.json", dirPath) >= (int)sizeof(path) || !tr_read(path, "en")) {
            if (why != NULL) snprintf(why, whyLen, "%s is missing or is not a valid label catalog", path);
            tr_clear();
            goto done;
        }
    }
    UiText_Load(outDir, locale);
    tr_port_load(outDir, locale);
    snprintf(sLocale, sizeof(sLocale), "%s", locale);
    __atomic_store_n(&sReady, 1, __ATOMIC_RELEASE);
    ok = 1;
done:
    pthread_mutex_unlock(&sLock);
    return ok;
}
int MenuTranslation_Ensure(void) {
    const char *dir = getenv("BT3_MENU_FONT"), *root = getenv("BT3_DATA");
    char path[1200];
    if (__atomic_load_n(&sReady, __ATOMIC_ACQUIRE)) return 1;
    if (dir == NULL) {
        snprintf(path, sizeof(path), "%s/game-text", root != NULL ? root : "gamedata");
        dir = path;
    }
    return MenuTranslation_Load(dir, NULL, 0);
}
const char *MenuTranslation_Locale(void) { return __atomic_load_n(&sReady, __ATOMIC_ACQUIRE) ? sLocale : NULL; }
const char *MenuTranslation_Text(const char *id) { TrRow *r = __atomic_load_n(&sReady, __ATOMIC_ACQUIRE) ? tr_find(id) : NULL; return r != NULL ? r->resolved : NULL; }
const char *MenuTranslation_PortText(const char *id) {
    int i;
    if (!__atomic_load_n(&sReady, __ATOMIC_ACQUIRE) || !id) return NULL;
    for (i = 0; i < TR_PORT_TEXT_COUNT; i++) if (!strcmp(id, kPortText[i].id))
        return sPortText[i][0] ? sPortText[i] : NULL;
    return NULL;
}
void MenuTranslation_PortTextCopy(const char *id, const char *fallback, char *out, int size) {
    const char *t;
    if (out == NULL || size <= 0) return;
    out[0] = '\0';
    t = MenuTranslation_PortText(id);
    if (t == NULL) t = fallback;
    if (t == NULL) return;
    snprintf(out, (size_t)size, "%s", t);
}
const char *MenuTranslation_Original(const char *id) { TrRow *r = __atomic_load_n(&sReady, __ATOMIC_ACQUIRE) ? tr_find(id) : NULL; return r != NULL ? r->original : NULL; }
const char *MenuTranslation_Group(const char *id) { int i; if (!__atomic_load_n(&sReady, __ATOMIC_ACQUIRE)) return NULL; for (i = 0; i < sNGroups; i++) if (strcmp(sGroups[i].id, id) == 0) return sGroups[i].text; return NULL; }
const char *const *MenuTranslation_StageNames(void) { return __atomic_load_n(&sReady, __ATOMIC_ACQUIRE) ? sStage : NULL; }
const char *const *MenuTranslation_SongNames(void) { return __atomic_load_n(&sReady, __ATOMIC_ACQUIRE) ? sSong : NULL; }
const char *MenuTranslation_Main(void) { return __atomic_load_n(&sReady, __ATOMIC_ACQUIRE) ? sMain : NULL; }
const char *MenuTranslation_Item(void) { return __atomic_load_n(&sReady, __ATOMIC_ACQUIRE) ? sItem : NULL; }
const char *MenuTranslation_MainOriginal(void) { return __atomic_load_n(&sReady, __ATOMIC_ACQUIRE) ? sMainOriginal : NULL; }
const char *MenuTranslation_ItemOriginal(void) { return __atomic_load_n(&sReady, __ATOMIC_ACQUIRE) ? sItemOriginal : NULL; }
