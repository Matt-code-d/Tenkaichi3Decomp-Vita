/*
 * Hold SELECT (~1 s, without L/R) for an Adrenaline-style pause menu.
 *
 * Drawn with vitaGL (same swap path that made MsgDialog visible): a dark sheet and a
 * green list over the game. psvDebugScreen is not used mid-game — it freezes the display.
 */
#include "vita_settings.h"
#include "vita_eu.h"
#include <psp2/ctrl.h>
#include <psp2/kernel/processmgr.h>
#include <psp2/kernel/threadmgr.h>
#include <psp2/power.h>
#include <vitaGL.h>
#include <stdio.h>
#include <string.h>
#include <stdlib.h>

#define HOME "ux0:data/bt3"
#define HOLD_US (1000 * 1000)
#define MENU_W 960.0f
#define MENU_H 544.0f

extern int gVitaWide;
extern void Vita_Log(const char *fmt, ...);
extern void vglSwapBuffers(GLboolean has_commondialog);

static int sOpen;
static unsigned long long sSelectDownUs;
static int sSelectWasDown;
static GLuint sFontTex;
static int sFontReady;

int Vita_SettingsOpen(void) {
    return sOpen;
}

/* ---- tiny 8x8 ASCII font (32..127), 1 bpp packed rows ---- */
static const unsigned char kFont8[96][8] = {
    {0,0,0,0,0,0,0,0}, {0x18,0x3C,0x3C,0x18,0x18,0,0x18,0}, {0x36,0x36,0x24,0,0,0,0,0},
    {0x36,0x36,0x7F,0x36,0x7F,0x36,0x36,0}, {0x0C,0x3E,0x03,0x1E,0x30,0x1F,0x0C,0}, {0,0x63,0x33,0x18,0x0C,0x66,0x63,0},
    {0x1C,0x36,0x1C,0x6E,0x3B,0x33,0x6E,0}, {0x0C,0x0C,0x06,0,0,0,0,0}, {0x18,0x0C,0x06,0x06,0x06,0x0C,0x18,0},
    {0x06,0x0C,0x18,0x18,0x18,0x0C,0x06,0}, {0,0x66,0x3C,0xFF,0x3C,0x66,0,0}, {0,0x0C,0x0C,0x3F,0x0C,0x0C,0,0},
    {0,0,0,0,0,0x0C,0x0C,0x06}, {0,0,0,0x3F,0,0,0,0}, {0,0,0,0,0,0x0C,0x0C,0}, {0x60,0x30,0x18,0x0C,0x06,0x03,0x01,0},
    {0x3E,0x63,0x73,0x7B,0x6F,0x67,0x3E,0}, {0x0C,0x0E,0x0C,0x0C,0x0C,0x0C,0x3F,0}, {0x1E,0x33,0x30,0x1C,0x06,0x33,0x3F,0},
    {0x1E,0x33,0x30,0x1C,0x30,0x33,0x1E,0}, {0x38,0x3C,0x36,0x33,0x7F,0x30,0x78,0}, {0x3F,0x03,0x1F,0x30,0x30,0x33,0x1E,0},
    {0x1C,0x06,0x03,0x1F,0x33,0x33,0x1E,0}, {0x3F,0x33,0x30,0x18,0x0C,0x0C,0x0C,0}, {0x1E,0x33,0x33,0x1E,0x33,0x33,0x1E,0},
    {0x1E,0x33,0x33,0x3E,0x30,0x18,0x0E,0}, {0,0x0C,0x0C,0,0,0x0C,0x0C,0}, {0,0x0C,0x0C,0,0,0x0C,0x0C,0x06},
    {0x18,0x0C,0x06,0x03,0x06,0x0C,0x18,0}, {0,0,0x3F,0,0x3F,0,0,0}, {0x06,0x0C,0x18,0x30,0x18,0x0C,0x06,0},
    {0x1E,0x33,0x30,0x18,0x0C,0,0x0C,0}, {0x3E,0x63,0x7B,0x7B,0x7B,0x03,0x1E,0}, {0x0C,0x1E,0x33,0x33,0x3F,0x33,0x33,0},
    {0x3F,0x66,0x66,0x3E,0x66,0x66,0x3F,0}, {0x3C,0x66,0x03,0x03,0x03,0x66,0x3C,0}, {0x1F,0x36,0x66,0x66,0x66,0x36,0x1F,0},
    {0x7F,0x46,0x16,0x1E,0x16,0x46,0x7F,0}, {0x7F,0x46,0x16,0x1E,0x16,0x06,0x0F,0}, {0x3C,0x66,0x03,0x03,0x73,0x66,0x7C,0},
    {0x33,0x33,0x33,0x3F,0x33,0x33,0x33,0}, {0x1E,0x0C,0x0C,0x0C,0x0C,0x0C,0x1E,0}, {0x78,0x30,0x30,0x30,0x33,0x33,0x1E,0},
    {0x67,0x66,0x36,0x1E,0x36,0x66,0x67,0}, {0x0F,0x06,0x06,0x06,0x46,0x66,0x7F,0}, {0x63,0x77,0x7F,0x7F,0x6B,0x63,0x63,0},
    {0x63,0x67,0x6F,0x7B,0x73,0x63,0x63,0}, {0x1C,0x36,0x63,0x63,0x63,0x36,0x1C,0}, {0x3F,0x66,0x66,0x3E,0x06,0x06,0x0F,0},
    {0x1E,0x33,0x33,0x33,0x3B,0x1E,0x38,0}, {0x3F,0x66,0x66,0x3E,0x36,0x66,0x67,0}, {0x1E,0x33,0x07,0x0E,0x38,0x33,0x1E,0},
    {0x3F,0x2D,0x0C,0x0C,0x0C,0x0C,0x1E,0}, {0x33,0x33,0x33,0x33,0x33,0x33,0x3F,0}, {0x33,0x33,0x33,0x33,0x33,0x1E,0x0C,0},
    {0x63,0x63,0x63,0x6B,0x7F,0x77,0x63,0}, {0x63,0x63,0x36,0x1C,0x1C,0x36,0x63,0}, {0x33,0x33,0x33,0x1E,0x0C,0x0C,0x1E,0},
    {0x7F,0x63,0x31,0x18,0x4C,0x66,0x7F,0}, {0x1E,0x06,0x06,0x06,0x06,0x06,0x1E,0}, {0x03,0x06,0x0C,0x18,0x30,0x60,0x40,0},
    {0x1E,0x18,0x18,0x18,0x18,0x18,0x1E,0}, {0x08,0x1C,0x36,0x63,0,0,0,0}, {0,0,0,0,0,0,0,0xFF},
    {0x0C,0x0C,0x18,0,0,0,0,0}, {0,0,0x1E,0x30,0x3E,0x33,0x6E,0}, {0x07,0x06,0x06,0x3E,0x66,0x66,0x3B,0},
    {0,0,0x1E,0x33,0x03,0x33,0x1E,0}, {0x38,0x30,0x30,0x3e,0x33,0x33,0x6E,0}, {0,0,0x1E,0x33,0x3f,0x03,0x1E,0},
    {0x1C,0x36,0x06,0x0f,0x06,0x06,0x0F,0}, {0,0,0x6E,0x33,0x33,0x3E,0x30,0x1F}, {0x07,0x06,0x36,0x6E,0x66,0x66,0x67,0},
    {0x0C,0,0x0E,0x0C,0x0C,0x0C,0x1E,0}, {0x30,0,0x30,0x30,0x30,0x33,0x33,0x1E}, {0x07,0x06,0x66,0x36,0x1E,0x36,0x67,0},
    {0x0E,0x0C,0x0C,0x0C,0x0C,0x0C,0x1E,0}, {0,0,0x33,0x7F,0x7F,0x6B,0x63,0}, {0,0,0x1F,0x33,0x33,0x33,0x33,0},
    {0,0,0x1E,0x33,0x33,0x33,0x1E,0}, {0,0,0x3B,0x66,0x66,0x3E,0x06,0x0F}, {0,0,0x6E,0x33,0x33,0x3E,0x30,0x78},
    {0,0,0x3B,0x6E,0x66,0x06,0x0F,0}, {0,0,0x3E,0x03,0x1E,0x30,0x1F,0}, {0x08,0x0C,0x3E,0x0C,0x0C,0x2C,0x18,0},
    {0,0,0x33,0x33,0x33,0x33,0x6E,0}, {0,0,0x33,0x33,0x33,0x1E,0x0C,0}, {0,0,0x63,0x6B,0x7F,0x7F,0x36,0},
    {0,0,0x63,0x36,0x1C,0x36,0x63,0}, {0,0,0x33,0x33,0x33,0x3E,0x30,0x1F}, {0,0,0x3F,0x19,0x0C,0x26,0x3F,0},
    {0x38,0x0C,0x0C,0x07,0x0C,0x0C,0x38,0}, {0x18,0x18,0x18,0x18,0x18,0x18,0x18,0}, {0x07,0x0C,0x0C,0x38,0x0C,0x0C,0x07,0},
    {0x6E,0x3B,0,0,0,0,0,0}, {0,0,0,0,0,0,0,0},
};

static void font_ensure(void) {
    unsigned char rgba[96 * 8 * 8 * 4];
    int c, y, x, i = 0;

    if (sFontReady) {
        return;
    }
    for (c = 0; c < 96; c++) {
        for (y = 0; y < 8; y++) {
            unsigned char row = kFont8[c][y];
            for (x = 0; x < 8; x++) {
                int on = (row >> x) & 1;
                rgba[i++] = on ? 255 : 0;
                rgba[i++] = on ? 255 : 0;
                rgba[i++] = on ? 255 : 0;
                rgba[i++] = on ? 255 : 0;
            }
        }
    }
    glGenTextures(1, &sFontTex);
    glBindTexture(GL_TEXTURE_2D, sFontTex);
    glTexImage2D(GL_TEXTURE_2D, 0, GL_RGBA, 8, 96 * 8, 0, GL_RGBA, GL_UNSIGNED_BYTE, rgba);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_NEAREST);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_NEAREST);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE);
    sFontReady = 1;
}

static void bt3_set(const char *name, const char *value) {
    char path[300], tmp[300], line[256], out[8192];
    FILE *fp;
    size_t n = 0;
    int found = 0;
    size_t nameLen = strlen(name);

    snprintf(path, sizeof(path), "%s/bt3.txt", HOME);
    snprintf(tmp, sizeof(tmp), "%s/bt3.txt.tmp", HOME);
    out[0] = '\0';
    fp = fopen(path, "r");
    if (fp != NULL) {
        while (fgets(line, sizeof(line), fp) != NULL) {
            char *eq = strchr(line, '=');
            if (eq != NULL && (size_t)(eq - line) == nameLen && strncmp(line, name, nameLen) == 0) {
                n += (size_t)snprintf(out + n, sizeof(out) - n, "%s=%s\n", name, value);
                found = 1;
            } else {
                n += (size_t)snprintf(out + n, sizeof(out) - n, "%s", line);
            }
            if (n + 64 >= sizeof(out)) {
                break;
            }
        }
        fclose(fp);
    }
    if (!found) {
        n += (size_t)snprintf(out + n, sizeof(out) - n, "%s=%s\n", name, value);
    }
    fp = fopen(tmp, "w");
    if (fp == NULL) {
        return;
    }
    fputs(out, fp);
    fclose(fp);
    remove(path);
    rename(tmp, path);
}

/* Pixel top-left (0,0) → NDC (vitaGL fixed pipeline, same idea as the movie blit). */
static void px_to_ndc(float x, float y, float *ox, float *oy) {
    *ox = (x / MENU_W) * 2.0f - 1.0f;
    *oy = 1.0f - (y / MENU_H) * 2.0f;
}

static void draw_quad(float x0, float y0, float x1, float y1, float r, float g, float b, float a) {
    float v[8], ax, ay, bx, by, cx, cy, dx, dy;

    px_to_ndc(x0, y0, &ax, &ay);
    px_to_ndc(x1, y0, &bx, &by);
    px_to_ndc(x0, y1, &cx, &cy);
    px_to_ndc(x1, y1, &dx, &dy);
    v[0] = ax; v[1] = ay;
    v[2] = bx; v[3] = by;
    v[4] = cx; v[5] = cy;
    v[6] = dx; v[7] = dy;

    glDisable(GL_TEXTURE_2D);
    glColor4f(r, g, b, a);
    glEnableClientState(GL_VERTEX_ARRAY);
    glDisableClientState(GL_TEXTURE_COORD_ARRAY);
    glDisableClientState(GL_COLOR_ARRAY);
    glVertexPointer(2, GL_FLOAT, 0, v);
    glDrawArrays(GL_TRIANGLE_STRIP, 0, 4);
}

static void draw_text(float x, float y, float scale, float r, float g, float b, const char *s) {
    float cw = 8.0f * scale, ch = 8.0f * scale;
    float atlasH = 96.0f * 8.0f;

    font_ensure();
    glEnable(GL_TEXTURE_2D);
    glBindTexture(GL_TEXTURE_2D, sFontTex);
    glTexEnvi(GL_TEXTURE_ENV, GL_TEXTURE_ENV_MODE, GL_MODULATE);
    glColor4f(r, g, b, 1.0f);
    glEnableClientState(GL_VERTEX_ARRAY);
    glEnableClientState(GL_TEXTURE_COORD_ARRAY);
    glDisableClientState(GL_COLOR_ARRAY);
    while (*s) {
        int chn = (unsigned char)*s++;
        float v[8], t[8], ax, ay, bx, by, cx, cy, dx, dy, u0, u1, v0, v1;
        int row;

        if (chn < 32 || chn > 127) {
            chn = '?';
        }
        row = chn - 32;
        u0 = 0.0f;
        u1 = 1.0f;
        v0 = (float)(row * 8) / atlasH;
        v1 = (float)(row * 8 + 8) / atlasH;
        px_to_ndc(x, y, &ax, &ay);
        px_to_ndc(x + cw, y, &bx, &by);
        px_to_ndc(x, y + ch, &cx, &cy);
        px_to_ndc(x + cw, y + ch, &dx, &dy);
        v[0] = ax; v[1] = ay;
        v[2] = bx; v[3] = by;
        v[4] = cx; v[5] = cy;
        v[6] = dx; v[7] = dy;
        t[0] = u0; t[1] = v0;
        t[2] = u1; t[3] = v0;
        t[4] = u0; t[5] = v1;
        t[6] = u1; t[7] = v1;
        glVertexPointer(2, GL_FLOAT, 0, v);
        glTexCoordPointer(2, GL_FLOAT, 0, t);
        glDrawArrays(GL_TRIANGLE_STRIP, 0, 4);
        x += cw;
    }
}

static unsigned buttons(void) {
    SceCtrlData pad;

    memset(&pad, 0, sizeof(pad));
    sceCtrlPeekBufferPositive(0, &pad, 1);
    return pad.buttons;
}

static void draw_menu(int cursor, int lang, int wide, int langDirty, int editingLang) {
    int count = VitaEu_LocaleCount();
    int i;
    float px = 220.0f, py = 120.0f, pw = 520.0f, ph = editingLang ? 320.0f : 280.0f;
    char line[96];

    glViewport(0, 0, (GLsizei)MENU_W, (GLsizei)MENU_H);
    glUseProgram(0);
    glDisable(GL_DEPTH_TEST);
    glDisable(GL_STENCIL_TEST);
    glDisable(GL_CULL_FACE);
    glDisable(GL_ALPHA_TEST);
    glEnable(GL_BLEND);
    glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);
    glColorMask(GL_TRUE, GL_TRUE, GL_TRUE, GL_TRUE);

    /* Dim whole screen (Adrenaline-like). */
    draw_quad(0, 0, MENU_W, MENU_H, 0.0f, 0.0f, 0.0f, 0.55f);
    /* Panel */
    draw_quad(px, py, px + pw, py + ph, 0.05f, 0.08f, 0.05f, 0.92f);
    draw_quad(px, py, px + pw, py + 36.0f, 0.0f, 0.35f, 0.12f, 1.0f);
    draw_text(px + 140.0f, py + 10.0f, 2.0f, 0.6f, 1.0f, 0.6f, "BT3 SETTINGS");

    if (editingLang) {
        draw_text(px + 24.0f, py + 50.0f, 2.0f, 0.8f, 0.9f, 0.8f, "Language");
        for (i = 0; i < count; i++) {
            float yy = py + 90.0f + (float)i * 28.0f;
            int sel = (i == lang);
            if (sel) {
                draw_quad(px + 16.0f, yy - 4.0f, px + pw - 16.0f, yy + 22.0f, 0.0f, 0.45f, 0.15f, 1.0f);
            }
            snprintf(line, sizeof(line), "%s %s", sel ? ">" : " ", VitaEu_LocaleName(i));
            draw_text(px + 32.0f, yy, 2.0f, sel ? 0.5f : 0.85f, 1.0f, sel ? 0.5f : 0.85f, line);
        }
        draw_text(px + 24.0f, py + ph - 40.0f, 1.5f, 0.7f, 0.85f, 0.5f, "UP/DOWN  X:save  O:back");
        return;
    }

    {
        const char *rows[3];
        char a[64], b[64];
        snprintf(a, sizeof(a), "Language: %s", VitaEu_Available() ? VitaEu_LocaleName(lang) : "(no pzs3eu1)");
        snprintf(b, sizeof(b), "Widescreen: %s", wide ? "On" : "Off");
        rows[0] = a;
        rows[1] = b;
        rows[2] = "Resume game";
        for (i = 0; i < 3; i++) {
            float yy = py + 70.0f + (float)i * 40.0f;
            int sel = (i == cursor);
            if (sel) {
                draw_quad(px + 16.0f, yy - 6.0f, px + pw - 16.0f, yy + 28.0f, 0.0f, 0.45f, 0.15f, 1.0f);
            }
            snprintf(line, sizeof(line), "%s %s", sel ? ">" : " ", rows[i]);
            draw_text(px + 32.0f, yy, 2.0f, sel ? 0.5f : 0.9f, 1.0f, sel ? 0.5f : 0.9f, line);
        }
    }
    draw_text(px + 24.0f, py + ph - 50.0f, 1.5f, 0.7f, 0.85f, 0.5f, "UP/DOWN  X:OK  O:close");
    if (langDirty) {
        draw_text(px + 24.0f, py + ph - 28.0f, 1.5f, 1.0f, 0.85f, 0.2f, "Restart to apply language");
    }
}

static void run_overlay(void) {
    int count = VitaEu_LocaleCount();
    int lang = 0, i, wide = gVitaWide != 0;
    int cursor = 0;
    int langDirty = 0;
    int editingLang = 0;
    const char *cur = VitaEu_Locale();
    unsigned prev = 0, now, pressed;

    for (i = 0; i < count; i++) {
        if (strcmp(VitaEu_LocaleAt(i), cur) == 0) {
            lang = i;
            break;
        }
    }

    sOpen = 1;
    Vita_Log("vita: settings menu open\n");
    prev = buttons();

    for (;;) {
        now = buttons();
        pressed = now & ~prev;
        prev = now;

        if (editingLang) {
            if (pressed & (SCE_CTRL_UP | SCE_CTRL_LEFT)) {
                if (VitaEu_Available()) {
                    lang = (lang + count - 1) % count;
                }
            } else if (pressed & (SCE_CTRL_DOWN | SCE_CTRL_RIGHT)) {
                if (VitaEu_Available()) {
                    lang = (lang + 1) % count;
                }
            } else if (pressed & SCE_CTRL_CROSS) {
                if (VitaEu_Available()) {
                    const char *loc = VitaEu_LocaleAt(lang);
                    if (strcmp(loc, VitaEu_Locale()) != 0) {
                        VitaEu_SetLocale(loc);
                        langDirty = 1;
                        Vita_Log("vita: settings language %s (restart to apply)\n", loc);
                    }
                }
                editingLang = 0;
            } else if (pressed & (SCE_CTRL_CIRCLE | SCE_CTRL_SELECT)) {
                editingLang = 0;
            }
        } else {
            if (pressed & SCE_CTRL_UP) {
                cursor = (cursor + 2) % 3;
            } else if (pressed & SCE_CTRL_DOWN) {
                cursor = (cursor + 1) % 3;
            } else if (pressed & (SCE_CTRL_LEFT | SCE_CTRL_RIGHT)) {
                if (cursor == 1) {
                    wide = !wide;
                    gVitaWide = wide;
                    bt3_set("wide", wide ? "1" : "0");
                    Vita_Log("vita: settings widescreen %d\n", wide);
                } else if (cursor == 0) {
                    editingLang = 1;
                }
            } else if (pressed & SCE_CTRL_CROSS) {
                if (cursor == 0) {
                    editingLang = 1;
                } else if (cursor == 1) {
                    wide = !wide;
                    gVitaWide = wide;
                    bt3_set("wide", wide ? "1" : "0");
                    Vita_Log("vita: settings widescreen %d\n", wide);
                } else {
                    break;
                }
            } else if (pressed & (SCE_CTRL_CIRCLE | SCE_CTRL_SELECT)) {
                break;
            }
        }

        draw_menu(cursor, lang, wide, langDirty, editingLang);
        vglSwapBuffers(GL_FALSE);
        sceKernelPowerTick(SCE_KERNEL_POWER_TICK_DISABLE_AUTO_SUSPEND);
        sceKernelDelayThread(16000);
    }

    sOpen = 0;
    Vita_Log("vita: settings menu close\n");
}

void Vita_SettingsPoll(void) {
    SceCtrlData pad;
    unsigned long long now;
    int selectAlone;

    if (sOpen) {
        return;
    }
    memset(&pad, 0, sizeof(pad));
    sceCtrlPeekBufferPositive(0, &pad, 1);
    selectAlone = (pad.buttons & SCE_CTRL_SELECT) != 0 &&
                  (pad.buttons & (SCE_CTRL_LTRIGGER | SCE_CTRL_RTRIGGER | SCE_CTRL_START)) == 0;
    now = sceKernelGetProcessTimeWide();
    if (selectAlone) {
        if (!sSelectWasDown) {
            sSelectDownUs = now;
            sSelectWasDown = 1;
        } else if (now - sSelectDownUs >= HOLD_US) {
            /* Open as soon as the hold reaches 1s (do not wait for release).
             * SELECT is already down, so it will not count as a fresh press to close. */
            sSelectWasDown = 0;
            sSelectDownUs = 0;
            run_overlay();
        }
    } else {
        sSelectWasDown = 0;
        sSelectDownUs = 0;
    }
}
