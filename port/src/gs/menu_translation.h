/* Menu and battle text loaded from a locale JSON in game-text. */
#ifndef BT3_MENU_TRANSLATION_H
#define BT3_MENU_TRANSLATION_H
#include <stddef.h>
#ifdef __cplusplus
extern "C" {
#endif

typedef struct MenuTranslationOption {
    char locale[16];
    char name[64];
    int order; /* smaller numbers appear first; missing order sorts after numbered languages */
} MenuTranslationOption;

#define PORT_TEXT_QUIT_CONFIRM "quit/confirm"
/* The start of an online match (gs/net.c): before the other player is there, and once they are ({name}: theirs). */
#define PORT_TEXT_NET_CONNECTING "net/connecting"
#define PORT_TEXT_NET_RIVAL "net/rival_here"
/* Tenkaichi Budokai Online hub / pages (src/menu/port_tbo.c): same locale as menu_lang. */
#define PORT_TEXT_TBO_TITLE "tbo/title"
#define PORT_TEXT_TBO_HUB_QUICK "tbo/hub/quick_match"
#define PORT_TEXT_TBO_HUB_RANKED "tbo/hub/ranked"
#define PORT_TEXT_TBO_HUB_CREATE "tbo/hub/create_room"
#define PORT_TEXT_TBO_HUB_JOIN "tbo/hub/join_room"
#define PORT_TEXT_TBO_HUB_FRIENDS "tbo/hub/friends"
#define PORT_TEXT_TBO_HUB_TOURNAMENT "tbo/hub/tournament"
#define PORT_TEXT_TBO_HUB_REPLAYS "tbo/hub/replays"
#define PORT_TEXT_TBO_HUB_LEADERBOARD "tbo/hub/leaderboard"
#define PORT_TEXT_TBO_HUB_PROFILE "tbo/hub/profile"
#define PORT_TEXT_TBO_PAGE_SEARCHING "tbo/page/searching"
#define PORT_TEXT_TBO_PAGE_TRAIN "tbo/page/train"
#define PORT_TEXT_TBO_SAY_QUICK "tbo/say/quick_match"
#define PORT_TEXT_TBO_SAY_RANKED "tbo/say/ranked"
#define PORT_TEXT_TBO_SAY_CREATE "tbo/say/create_room"
#define PORT_TEXT_TBO_SAY_JOIN "tbo/say/join_room"
#define PORT_TEXT_TBO_SAY_FRIENDS "tbo/say/friends"
#define PORT_TEXT_TBO_SAY_TOURNAMENT "tbo/say/tournament"
#define PORT_TEXT_TBO_SAY_REPLAYS "tbo/say/replays"
#define PORT_TEXT_TBO_SAY_LEADERBOARD "tbo/say/leaderboard"
#define PORT_TEXT_TBO_SAY_PROFILE "tbo/say/profile"
#define PORT_TEXT_TBO_TIME_60 "tbo/time/60"
#define PORT_TEXT_TBO_TIME_90 "tbo/time/90"
#define PORT_TEXT_TBO_TIME_180 "tbo/time/180"
#define PORT_TEXT_TBO_TIME_240 "tbo/time/240"
#define PORT_TEXT_TBO_TIME_NOLIMIT "tbo/time/nolimit"
#define PORT_TEXT_TBO_NOTICE_RIVAL "tbo/notice/rival_here"
#define PORT_TEXT_TBO_NOTICE_CLOSED "tbo/notice/room_closed"
#define PORT_TEXT_TBO_SOON "tbo/soon"
#define PORT_TEXT_TBO_BTN_DELETE "tbo/btn/delete"
#define PORT_TEXT_TBO_BTN_PASTE "tbo/btn/paste"
#define PORT_TEXT_TBO_BTN_JOIN "tbo/btn/join"
#define PORT_TEXT_TBO_BTN_DONE "tbo/btn/done"
#define PORT_TEXT_TBO_BTN_SPACE "tbo/btn/space"
#define PORT_TEXT_TBO_BTN_BACK "tbo/btn/back"
#define PORT_TEXT_TBO_PROMPT_CHANGE "tbo/prompt/change"
#define PORT_TEXT_TBO_PROMPT_NEXT "tbo/prompt/next"
#define PORT_TEXT_TBO_PROMPT_BACK "tbo/prompt/back"
#define PORT_TEXT_TBO_PROMPT_ENTER "tbo/prompt/enter"
#define PORT_TEXT_TBO_PROMPT_COPY "tbo/prompt/copy_code"
#define PORT_TEXT_TBO_PROMPT_CANCEL "tbo/prompt/cancel"
#define PORT_TEXT_TBO_PROMPT_OPEN "tbo/prompt/open_room"
#define PORT_TEXT_TBO_PROMPT_TRAIN "tbo/prompt/start_training"
#define PORT_TEXT_TBO_PROMPT_SEARCH "tbo/prompt/search"
#define PORT_TEXT_TBO_CR_SETTINGS "tbo/create/room_settings"
#define PORT_TEXT_TBO_CR_BATTLE "tbo/create/battle"
#define PORT_TEXT_TBO_CR_TIME "tbo/create/time_limit"
#define PORT_TEXT_TBO_CR_DELAY "tbo/create/input_delay"
#define PORT_TEXT_TBO_CR_ROLL "tbo/create/rollback"
#define PORT_TEXT_TBO_CR_OPEN "tbo/create/open_room"
#define PORT_TEXT_TBO_VAL_1V1 "tbo/val/one_on_one"
#define PORT_TEXT_TBO_VAL_TEAMS "tbo/val/teams"
#define PORT_TEXT_TBO_VAL_TEAMS_SHORT "tbo/val/teams_short"
#define PORT_TEXT_TBO_VAL_AUTO "tbo/val/automatic"
#define PORT_TEXT_TBO_VAL_N_FRAME "tbo/val/n_frame"
#define PORT_TEXT_TBO_VAL_N_FRAMES "tbo/val/n_frames"
#define PORT_TEXT_TBO_VAL_NOT_PC "tbo/val/not_on_pc"
#define PORT_TEXT_TBO_VAL_OFF "tbo/val/off"
#define PORT_TEXT_TBO_VAL_UP_TO "tbo/val/up_to_frames"
#define PORT_TEXT_TBO_HELP_CREATE "tbo/help/create"
#define PORT_TEXT_TBO_HELP_DELAY "tbo/help/create_delay"
#define PORT_TEXT_TBO_HELP_ROLL "tbo/help/create_roll"
#define PORT_TEXT_TBO_HELP_JOIN "tbo/help/join"
#define PORT_TEXT_TBO_HELP_WAIT "tbo/help/wait"
#define PORT_TEXT_TBO_HELP_WAIT_OK "tbo/help/wait_found"
#define PORT_TEXT_TBO_HELP_BLOCK "tbo/help/block"
#define PORT_TEXT_TBO_HELP_SPAR "tbo/help/spar"
#define PORT_TEXT_TBO_HELP_QUICK "tbo/help/quick"
#define PORT_TEXT_TBO_HELP_NAME "tbo/help/name"
#define PORT_TEXT_TBO_HELP_PROFILE "tbo/help/profile"
#define PORT_TEXT_TBO_WAIT_QM "tbo/wait/quick_match"
#define PORT_TEXT_TBO_WAIT_PAIRS1 "tbo/wait/pairs_1"
#define PORT_TEXT_TBO_WAIT_PAIRS2 "tbo/wait/pairs_2"
#define PORT_TEXT_TBO_WAIT_N_QUEUE "tbo/wait/n_in_queue"
#define PORT_TEXT_TBO_WAIT_CODE "tbo/wait/room_code"
#define PORT_TEXT_TBO_WAIT_GET_CODE "tbo/wait/getting_code"
#define PORT_TEXT_TBO_WAIT_GIVE1 "tbo/wait/give_1"
#define PORT_TEXT_TBO_WAIT_GIVE2 "tbo/wait/give_2"
#define PORT_TEXT_TBO_WAIT_ASK1 "tbo/wait/ask_1"
#define PORT_TEXT_TBO_WAIT_ASK2 "tbo/wait/ask_2"
#define PORT_TEXT_TBO_WAIT_OTHER "tbo/wait/other_version"
#define PORT_TEXT_TBO_WAIT_JOINING "tbo/wait/joining"
#define PORT_TEXT_TBO_WAIT_LOOK1 "tbo/wait/look_1"
#define PORT_TEXT_TBO_WAIT_LOOK2 "tbo/wait/look_2"
#define PORT_TEXT_TBO_WAIT_HERE "tbo/wait/rival_here"
#define PORT_TEXT_TBO_WAIT_SEARCH "tbo/wait/searching"
#define PORT_TEXT_TBO_NAME_TYPE "tbo/name/type"
#define PORT_TEXT_TBO_SPAR_TITLE "tbo/spar/title"
#define PORT_TEXT_TBO_SPAR_YOU "tbo/spar/you"
#define PORT_TEXT_TBO_SPAR_PARTNER "tbo/spar/partner"
#define PORT_TEXT_TBO_SPAR_START "tbo/spar/start"
#define PORT_TEXT_TBO_SPAR_QM "tbo/spar/quick_search"
#define PORT_TEXT_TBO_SPAR_OPEN "tbo/spar/room_open"
#define PORT_TEXT_TBO_SPAR_JOIN "tbo/spar/joining"
#define PORT_TEXT_TBO_SPAR_SEARCH "tbo/spar/searching"
#define PORT_TEXT_TBO_SPAR_START1 "tbo/spar/starts_1"
#define PORT_TEXT_TBO_SPAR_START2 "tbo/spar/starts_2"
#define PORT_TEXT_TBO_QK_PICK "tbo/quick/pick"
#define PORT_TEXT_TBO_QK_BATTLE "tbo/quick/battle"
#define PORT_TEXT_TBO_QK_SEARCH "tbo/quick/search"
#define PORT_TEXT_TBO_QK_WAITING "tbo/quick/waiting_now"
#define PORT_TEXT_TBO_QK_TEAMS "tbo/quick/teams"
#define PORT_TEXT_TBO_QK_1V1 "tbo/quick/one_on_one"
#define PORT_TEXT_TBO_QK_N_WAIT "tbo/quick/n_waiting"
#define PORT_TEXT_TBO_QK_SPEED "tbo/quick/no_time_limit"
#define PORT_TEXT_TBO_PR_CARD "tbo/profile/card"
#define PORT_TEXT_TBO_PR_NAME "tbo/profile/name"
#define PORT_TEXT_TBO_BLOCK_TITLE "tbo/block/title"
#define PORT_TEXT_TBO_BLOCK_L1 "tbo/block/line1"
#define PORT_TEXT_TBO_BLOCK_L2 "tbo/block/line2"
#define PORT_TEXT_TBO_BLOCK_CHANGED "tbo/block/changed"
#define PORT_TEXT_TBO_BLOCK_NOT_DISC "tbo/block/not_disc"
#define PORT_TEXT_TBO_BLOCK_CHECK "tbo/block/unchecked"
#define PORT_TEXT_TBO_BLOCK_MODS "tbo/block/remove_mods"
#define PORT_TEXT_TBO_BLOCK_FIGHTER "tbo/block/fighter_note"
#define PORT_TEXT_TBO_NOTE_CHECK "tbo/note/checking"
#define PORT_TEXT_TBO_NOTE_NET "tbo/note/net_fail"
#define PORT_TEXT_TBO_NOTE_RELAY "tbo/note/relay_fail"
#define PORT_TEXT_TBO_NOTE_ROOM "tbo/note/room_fail"
#define PORT_TEXT_TBO_NOTE_COPIED "tbo/note/copied"
#define PORT_TEXT_TBO_NOTE_SOON "tbo/note/not_open"
#define PORT_TEXT_TBO_NOTE_CODE "tbo/note/code_len"
#define PORT_TEXT_TBO_NOTE_SAVED "tbo/note/saved_name"
#define PORT_TEXT_TBO_SPAR_SIGN_SEARCH "tbo/spar_sign/searching"
#define PORT_TEXT_TBO_SPAR_SIGN_QM "tbo/spar_sign/quick"
#define PORT_TEXT_TBO_SPAR_SIGN_ROOM "tbo/spar_sign/room"
#define PORT_TEXT_TBO_SPAR_SIGN_JOIN "tbo/spar_sign/joining"

/* Copies bundled label and port catalogs without replacing local edits, then loads
   BT3_MENU_LANG or the saved menu_lang setting (English if neither is set). */
int MenuTranslation_Load(const char *outDir, char *why, size_t whyLen);
int MenuTranslation_Ensure(void);
int MenuTranslation_List(const char *outDir, MenuTranslationOption *out, int capacity);
int MenuTranslation_Available(MenuTranslationOption *out, int capacity);
int MenuTranslation_ShouldPrompt(const MenuTranslationOption *options, int count);
int MenuTranslation_BootPending(void);
void MenuTranslation_SaveChoice(const char *locale);
const char *MenuTranslation_Locale(void);
const char *MenuTranslation_Text(const char *id);
/* Port-owned UI text, decoded from translations/port/<locale>.json; NULL uses the built-in English string. */
const char *MenuTranslation_PortText(const char *id);
/* Copy port text into a game-side buffer. Game code must not keep the pointer from
   MenuTranslation_PortText (host addresses are truncated to 32 bits and crash). */
void MenuTranslation_PortTextCopy(const char *id, const char *fallback, char *out, int size);
const char *MenuTranslation_Original(const char *id);
const char *MenuTranslation_Group(const char *id);
const char *const *MenuTranslation_StageNames(void); /* 36 entries */
const char *const *MenuTranslation_SongNames(void);  /* 24 entries */
const char *MenuTranslation_Main(void);              /* first 11 main menu items, separated by | */
const char *MenuTranslation_Item(void);              /* the PC's twelfth item */
const char *MenuTranslation_MainOriginal(void);      /* wording painted on the disc */
const char *MenuTranslation_ItemOriginal(void);

#ifdef __cplusplus
}
#endif
#endif
