#include <3ds.h>
#include <3ds/applets/swkbd.h>
#include <3ds/services/ptmu.h>
#include <3ds/services/mcuhwc.h>
#include <citro2d.h>
#include <stdio.h>
#include <time.h>
#include <math.h>
#include <stdlib.h>
#include <string.h>
#include <sys/stat.h>
#include <malloc.h>
typedef enum {
    FOREST_PHASE_NIGHT = 0,
    FOREST_PHASE_DAWN,
    FOREST_PHASE_DAY,
    FOREST_PHASE_DUSK,
    FOREST_PHASE_COUNT
} ForestPhase;
static C2D_Image forestPhaseImages[FOREST_PHASE_COUNT] = {0};
static C2D_Image cloudImage = {0};
static C2D_Image cloudImage2 = {0};
static C2D_Image owlImage1 = {0};
static C2D_Image owlImage2 = {0};
static C2D_Image owlImage3dx = {0};
static C2D_Image owlImage3sx = {0};
static C2D_Image owlImage3dxd = {0};
static C2D_Image owlImage3sxd = {0};
static C2D_Image hedgehogImageDx = {0};
static C2D_Image hedgehogImageSx = {0};
static C2D_Image hedgehogImageDxB = {0};
static C2D_Image hedgehogImageSxB = {0};
static C2D_Image cloudImage3 = {0};
static C2D_Image cloudImage4 = {0};
static C2D_Image cloudImage5 = {0};
static C2D_Image robinImage = {0};
static C2D_Image robinImage2 = {0};
static C2D_Image robinFlyImageSx = {0};
static C2D_Image robinFlyImageDx = {0};
static C2D_Image robinFlyImageSxB = {0};
static C2D_Image robinFlyImageDxB = {0};
static C2D_Image robinPeckDx = {0};
static C2D_Image robinPeckSx = {0};
static C2D_Image greyBirdImageDx = {0};
static C2D_Image greyBirdImageSx = {0};
static C2D_Image greyBirdPeckDx = {0};
static C2D_Image greyBirdPeckSx = {0};
static C2D_Image greyBirdFlyImageDx = {0};
static C2D_Image greyBirdFlyImageSx = {0};
static C2D_Image greyBirdFlyImageDxB = {0};
static C2D_Image greyBirdFlyImageSxB = {0};
static C2D_Image hareWalk1Image = {0};
static C2D_Image hareWalk2Image = {0};
static C2D_Image hareStandSxImage = {0};
static C2D_Image hareStandDxImage = {0};
static C2D_Image hareDx1Image = {0};
static C2D_Image hareDx2Image = {0};
static C2D_Image hareGrazeSxImage = {0};
static C2D_Image hareGrazeDxImage = {0};
static C2D_Image hareGrazeSx2Image = {0};
static C2D_Image hareGrazeDx2Image = {0};
static C2D_Image hareStandSxBImage = {0};
static C2D_Image hareStandDxBImage = {0};
static C2D_Image foxSx1Image = {0};
static C2D_Image foxSx2Image = {0};
static C2D_Image foxSx3Image = {0};
static C2D_Image foxSx4Image = {0};
static C2D_Image foxSx5Image = {0};
static C2D_Image foxSxStandImage = {0};
static C2D_Image greyBirdSleepImage = {0};
static C2D_Image seasonSpringDayImage = {0};
static C2D_Image seasonSpringDawnImage = {0};
static C2D_Image seasonSpringDuskImage = {0};
static C2D_Image seasonSpringNightImage = {0};
static C2D_Image seasonAutumnDayImage = {0};
static C2D_Image seasonAutumnDawnImage = {0};
static C2D_Image seasonAutumnDuskImage = {0};
static C2D_Image seasonAutumnNightImage = {0};
static C2D_Image seasonWinterDayImage = {0};
static C2D_Image seasonWinterDawnImage = {0};
static C2D_Image seasonWinterDuskImage = {0};
static C2D_Image seasonWinterNightImage = {0};
static C2D_Image nestImage = {0};
static C2D_Image robinSleepImage = {0};
static ForestPhase currentForestPhase  = FOREST_PHASE_DAY;
static ForestPhase previousForestPhase = FOREST_PHASE_DAY;
static float forestPhaseTransition = 0.0f;
#define FOREST_PHASE_TRANSITION_SECONDS 2.5f
static bool forestPhaseManualOverride = false;
static bool settingsDirty;
static int activePresetSlot = -1; // -1 = nessuno richiamato in questa sessione
static bool presetSaveMode = false; // true = in attesa che l'utente scelga uno slot 1-4 per salvare
static bool  debugPointerActive = false;
static bool  debugResetConfirmPending = false;
static int debugPointerDisplayMode = 0;
static float debugPointerX = 200.0f, debugPointerY = 120.0f;
static C2D_Image forestPreviousBgImage = {0};
static float forestBgTransitionDuration = FOREST_PHASE_TRANSITION_SECONDS; // normalmente la costante standard, allungata solo arrivando a Winter (foglie->spoglio, merita una dissolvenza più dolce)
static bool forestBgFadeSymmetric = false; // false = tecnica fasi (vecchia sempre opaca, nuova sopra sfuma); true = tecnica stagioni (entrambe sfumano, necessaria per i buchi nella chioma che altrimenti scattano di colpo a fine transizione)
static u32 currentEnvSeason = 1; // 1 = Summer; valore RISOLTO runtime (0=Spring,1=Summer,2=Autumn,3=Winter), come currentForestPhase — settings.envSeasonMode può essere Auto/Cycle/Random, questo è sempre un indice valido 0-3
static C2D_Image getForestBackgroundImage(ForestPhase phase); // definita più sotto nel file
static C2D_Image getForestBackgroundImageForSeason(ForestPhase phase, u32 season); // definita più sotto nel file
#define SAVE_PATH_RECORDS  "sdmc:/3ds/ForestClock/records.bin"
#define SAVE_PATH_SETTINGS "sdmc:/3ds/ForestClock/settings.bin"
#define SAVE_PATH_ALARM    "sdmc:/3ds/ForestClock/alarm.bin"
#define SAVE_PATH_PRESETS  "sdmc:/3ds/ForestClock/presets.bin"
#define SAVE_PATH_DIARY    "sdmc:/3ds/ForestClock/diary.bin"
#define SAVE_PATH_NOTES    "sdmc:/3ds/ForestClock/notes.bin"
#define SAVE_DIR           "sdmc:/3ds/ForestClock"
#define THEME_TRANSITION_SPEED 0.05f
#define COLOR_TRANSITION_SPEED 0.08f
#define COMPILER_COLOR(r,g,b,a) (((u32)(a)<<24)|((u32)(b)<<16)|((u32)(g)<<8)|(u32)(r))
#define BEEP_SAMPLE_RATE   22050
#define BEEP_TONE_HZ       880.0f
#define BEEP_COUNT         5
#define BEEP_DURATION_MS   120
#define BEEP_GAP_MS        30
#define BEEP_VOLUME        0.75f
#define ALARM_REPEAT_PAUSE_FRAMES 120
#define ALARM_FLASH_SPEED  6.0f
typedef enum {
    SCREEN_MAIN,
    SCREEN_SETTINGS,
    SCREEN_RECORDS,
    SCREEN_DEBUG,
    SCREEN_CREDITS,
    SCREEN_ALARM,
    SCREEN_TIMER,
    SCREEN_ENVIRONMENT,
    SCREEN_WEATHER_DEBUG,
    SCREEN_CALENDAR,
    SCREEN_CALENDAR_SETTINGS,
    SCREEN_DIARY_ENTRY,
    SCREEN_CALENDAR_MONTHYEAR,
    SCREEN_NOTES_LIST,
    SCREEN_NOTE_VIEW
} MenuScreen;
typedef enum {
    TIMER_IDLE,
    TIMER_RUNNING,
    TIMER_PAUSED
} TimerState;
typedef struct {
    u32 totalTimeSeconds;
} AppRecords;
typedef struct {
    u32 clockColorIndex;
    u32 timeFormat24h;
    u32 dateFormat;
    u32 bgThemeIndex;
    u32 clockSizePreset;
    u32 clockMode;
    float clockOffsetX;
    float clockOffsetY;
    u32 envPhaseMode;
    u32 envPhaseCycleIntervalIndex;
    u32 envCloudSpeedLevel;
    u32 envCloudDirection;
    u32 envSeasonMode; // 0=Auto,1=Spring,2=Summer,3=Autumn,4=Winter,5=Cycle,6=Random
    u32 envSeasonCycleIntervalIndex;
    char location[32]; // città per il meteo reale, digitata con la tastiera di sistema; vuota = non impostata
    u32 envWeatherMode; // 0=Clear,1-4=Rain,5-7=Snow (come weatherType), 8=Real Time (usa il risultato del fetch da internet)
    u32 weatherScheduleMode; // 0=Never,1=10min,2=30min,3=1h — intervallo di ricontrollo automatico del meteo reale
    u32 calFirstDayMonday; // 0=Domenica, 1=Lunedì — primo giorno della settimana nel Calendario
} AppSettings;
static AppSettings settings;
typedef struct {
    u32 enabled;
    u32 hour;
    u32 minute;
} AppAlarm;
typedef struct {
    float topR, topG, topB;
    float botR, botG, botB;
    u32 textMenuColor;
    u32 bubbleColor;
    const char* name;
} DepthTheme;
static const DepthTheme phaseThemes[FOREST_PHASE_COUNT][5] = {
    {
        {4.0f,   8.0f,  20.0f, 10.0f, 16.0f, 30.0f, COMPILER_COLOR(150, 170, 210, 255), COMPILER_COLOR(220, 225, 245, 255), "Starry Night"},
        {14.0f, 16.0f,  26.0f, 20.0f, 22.0f, 32.0f, COMPILER_COLOR(140, 145, 160, 255), COMPILER_COLOR(210, 210, 220, 255), "Cloudy Night"},
        {20.0f, 28.0f,  48.0f, 30.0f, 38.0f, 58.0f, COMPILER_COLOR(170, 190, 220, 255), COMPILER_COLOR(230, 235, 250, 255), "Moonlight"},
        {2.0f,   3.0f,   8.0f,  5.0f,  7.0f, 14.0f, COMPILER_COLOR(110, 120, 145, 255), COMPILER_COLOR(190, 195, 215, 255), "Deep Night"},
        {140.0f, 140.0f, 145.0f, 140.0f, 140.0f, 145.0f, COMPILER_COLOR(90, 90, 95, 255), COMPILER_COLOR(200, 200, 205, 255), "Gray Sky"}
    },
    {
        {150.0f, 110.0f, 140.0f, 210.0f, 160.0f, 170.0f, COMPILER_COLOR(160, 100, 120, 255), COMPILER_COLOR(255, 220, 225, 255), "Pink Dawn"},
        {200.0f, 150.0f, 110.0f, 230.0f, 190.0f, 150.0f, COMPILER_COLOR(150, 110, 60,  255), COMPILER_COLOR(255, 235, 200, 255), "Golden Dawn"},
        {180.0f, 175.0f, 190.0f, 210.0f, 205.0f, 210.0f, COMPILER_COLOR(120, 120, 130, 255), COMPILER_COLOR(240, 240, 245, 255), "Misty Dawn"},
        {220.0f, 170.0f, 110.0f, 245.0f, 200.0f, 150.0f, COMPILER_COLOR(160, 100, 40,  255), COMPILER_COLOR(255, 230, 190, 255), "First Light"},
        {140.0f, 140.0f, 145.0f, 140.0f, 140.0f, 145.0f, COMPILER_COLOR(90, 90, 95, 255), COMPILER_COLOR(200, 200, 205, 255), "Gray Sky"}
    },
    {
        {150.0f, 195.0f, 235.0f, 210.0f, 225.0f, 205.0f, COMPILER_COLOR(60, 110, 70, 255),  COMPILER_COLOR(255, 250, 230, 255), "Clear Sky"},
        {90.0f,  150.0f, 235.0f, 170.0f, 205.0f, 235.0f, COMPILER_COLOR(40, 90, 150, 255),  COMPILER_COLOR(230, 245, 255, 255), "Blue Sky"},
        {130.0f, 180.0f, 230.0f, 190.0f, 215.0f, 220.0f, COMPILER_COLOR(50, 100, 90, 255),  COMPILER_COLOR(255, 255, 240, 255), "Midday"},
        {195.0f, 205.0f, 210.0f, 220.0f, 220.0f, 210.0f, COMPILER_COLOR(80, 90, 85, 255),   COMPILER_COLOR(255, 255, 250, 255), "Pearl Sky"},
        {140.0f, 140.0f, 145.0f, 140.0f, 140.0f, 145.0f, COMPILER_COLOR(90, 90, 95, 255), COMPILER_COLOR(200, 200, 205, 255), "Gray Sky"}
    },
    {
        {210.0f, 90.0f,  60.0f, 235.0f, 140.0f, 90.0f,  COMPILER_COLOR(160, 60, 30,  255), COMPILER_COLOR(255, 200, 160, 255), "Fiery Sunset"},
        {120.0f, 70.0f, 120.0f, 180.0f, 110.0f, 150.0f, COMPILER_COLOR(110, 60, 110, 255), COMPILER_COLOR(230, 190, 230, 255), "Purple Sunset"},
        {225.0f, 190.0f, 150.0f, 235.0f, 210.0f, 175.0f, COMPILER_COLOR(140, 90, 50, 255), COMPILER_COLOR(255, 235, 190, 255), "Golden Backlight"},
        {70.0f,  60.0f, 110.0f, 120.0f, 100.0f, 150.0f, COMPILER_COLOR(90, 80, 130, 255),  COMPILER_COLOR(200, 195, 230, 255), "Twilight"},
        {140.0f, 140.0f, 145.0f, 140.0f, 140.0f, 145.0f, COMPILER_COLOR(90, 90, 95, 255), COMPILER_COLOR(200, 200, 205, 255), "Gray Sky"}
    }
};
#define CLOUD_MAX 60 // dimensione massima dell'array, capiente per il valore regolabile da Debug
typedef struct {
    float x, y;
    float speed;
    float scale;
    float layer;
    float wobble;
} Cloud;
static Cloud clouds[CLOUD_MAX];
static int activeCloudCount = 10; // quante nuvole sono davvero attive, regolabile da Debug (1-20)
typedef struct {
    float x;
    bool  active;
    bool  movingRight;
    int   spawnTimer;
} RareCloud;
static RareCloud rareCloud;
#define RARE_CLOUD_Y     95.0f
#define RARE_CLOUD_SPEED 0.035f
#define RARE_CLOUD_SCALE 1.0f
#define FOREST_DAWN_START  6.0f
#define FOREST_DAY_START   8.0f
#define FOREST_DUSK_START  18.0f
#define FOREST_NIGHT_START 20.0f
static ForestPhase getForestPhaseForHour(float hourFrac) {
    if (hourFrac >= FOREST_DAWN_START  && hourFrac < FOREST_DAY_START)   return FOREST_PHASE_DAWN;
    if (hourFrac >= FOREST_DAY_START   && hourFrac < FOREST_DUSK_START)  return FOREST_PHASE_DAY;
    if (hourFrac >= FOREST_DUSK_START  && hourFrac < FOREST_NIGHT_START) return FOREST_PHASE_DUSK;
    return FOREST_PHASE_NIGHT;
}
static const u32 envIntervalMinutes[4] = {1, 5, 10, 30};
static float getForestCloudSpeedMultiplier() {
    return (float)settings.envCloudSpeedLevel / 6.0f;
}
static float getForestCloudDirMul(int cloudIndex) {
    u32 dir = settings.envCloudDirection;
    if (dir == 0) return -1.0f;
    if (dir == 1) return 1.0f;
    if (dir == 2) return (cloudIndex % 2 == 0) ? 1.0f : -1.0f;
    return (cloudIndex % 2 == 0) ? -1.0f : 1.0f;
}
static float envPhaseCycleTimer = 0.0f;
static void updateEnvPhaseCycle() {
    u32 mode = settings.envPhaseMode;
    if (mode != 5 && mode != 6) { envPhaseCycleTimer = 0.0f; return; }
    if (forestPhaseManualOverride) return;
    envPhaseCycleTimer += 0.0166f;
    float intervalSeconds = (float)envIntervalMinutes[settings.envPhaseCycleIntervalIndex % 4] * 60.0f;
    if (envPhaseCycleTimer >= intervalSeconds) {
        envPhaseCycleTimer -= intervalSeconds;
        static const ForestPhase order[4] = { FOREST_PHASE_DAWN, FOREST_PHASE_DAY, FOREST_PHASE_DUSK, FOREST_PHASE_NIGHT };
        ForestPhase next;
        if (mode == 5) {
            int curIdx = 0;
            for (int k = 0; k < 4; k++) if (order[k] == currentForestPhase) curIdx = k;
            next = order[(curIdx + 1) % 4];
        } else {
            do { next = order[rand() % 4]; } while (next == currentForestPhase);
        }
        if (next != currentForestPhase) {
            forestPreviousBgImage = getForestBackgroundImage(currentForestPhase);
            previousForestPhase = currentForestPhase;
            currentForestPhase = next;
            forestPhaseTransition = forestBgTransitionDuration = FOREST_PHASE_TRANSITION_SECONDS;
            forestBgFadeSymmetric = false;
        }
    }
}
// Stagione dalla data reale (mese+giorno), non solo il mese: usa le date
// astronomiche approssimate di equinozi/solstizi (fisse, senza il lieve
// slittamento di un giorno o due che varia anno per anno — trascurabile
// per questo scopo). Convenzione emisfero Nord. Ordine indici:
// 0=Spring,1=Summer,2=Autumn,3=Winter.
// Essendo controllata ogni frame con l'ora reale, il cambio scatta
// esattamente al passaggio di data (anche a mezzanotte), quindi se in
// quel momento è Notte, la fase notturna riflette già la nuova stagione.
static u32 getSeasonForDate(int month1to12, int day1to31) {
    if (month1to12 == 3)  return (day1to31 >= 20) ? 0 : 3; // Primavera da 20 marzo
    if (month1to12 == 4 || month1to12 == 5) return 0;
    if (month1to12 == 6)  return (day1to31 >= 21) ? 1 : 0; // Estate da 21 giugno
    if (month1to12 == 7 || month1to12 == 8) return 1;
    if (month1to12 == 9)  return (day1to31 >= 22) ? 2 : 1; // Autunno da 22 settembre
    if (month1to12 == 10 || month1to12 == 11) return 2;
    if (month1to12 == 12) return (day1to31 >= 21) ? 3 : 2; // Inverno da 21 dicembre
    return 3; // gennaio, febbraio
}
// Supporto calendario: giorni nel mese (con anno bisestile) e giorno
// della settimana (0=domenica) del primo del mese, per disegnare la
// griglia 6x7.
static int getDaysInMonth(int month0to11, int year) {
    static const int days[12] = {31,28,31,30,31,30,31,31,30,31,30,31};
    if (month0to11 == 1) {
        bool leap = (year % 4 == 0 && (year % 100 != 0 || year % 400 == 0));
        return leap ? 29 : 28;
    }
    return days[month0to11];
}
static int getFirstWeekdayOfMonth(int month0to11, int year) {
    struct tm firstDayTm = {0};
    firstDayTm.tm_mday = 1;
    firstDayTm.tm_mon  = month0to11;
    firstDayTm.tm_year = year - 1900;
    firstDayTm.tm_hour = 12; // evita ambiguità di fuso/ora legale a mezzanotte
    mktime(&firstDayTm); // normalizza e calcola tm_wday
    return firstDayTm.tm_wday; // 0 = domenica
}
// Mese/anno attualmente mostrati nel Calendario — separati dal mese
// reale così si può navigare avanti/indietro con le freccette. -1
// significa "non ancora inizializzato": verranno impostati al mese
// corrente ogni volta che si entra nella schermata dal menu principale.
static int calDisplayMonth = -1;
static int calDisplayYear  = -1;

/* -------------------- DIARIO / AGENDA -------------------- */
// Array a dimensione fissa (stesso stile di envPresets/records/ecc. —
// niente allocazione dinamica), una voce di testo libero per giorno.
#define DIARY_MAX_ENTRIES 200
#define DIARY_TEXT_MAX 200
typedef struct {
    u32 used;
    u32 year, month, day; // month: 0-11, coerente con tm_mon
    char text[DIARY_TEXT_MAX];
} DiaryEntry;
static DiaryEntry diaryEntries[DIARY_MAX_ENTRIES];
// Data attualmente aperta nella schermata voce diario
static int diaryViewYear = -1, diaryViewMonth = -1, diaryViewDay = -1;
// Scroll del testo nella schermata diario: quante righe (di quelle
// spezzate dal word-wrap) sono nascoste sopra la prima visibile.
// diaryTotalLines viene aggiornato nel disegno (unico punto che sa
// quante righe produce il word-wrap) e letto nell'input per il clamp.
#define DIARY_VISIBLE_LINES 8
static int diaryScrollOffset = 0;
static int diaryTotalLines = 0;
static float diaryScrollRepeatTimer = 0.0f;
// Nota attualmente aperta nella schermata di vista/modifica, e il suo
// scroll — stesso schema già usato per il diario.
static int noteViewIdx = -1;
#define NOTE_VISIBLE_LINES 7
static int noteScrollOffset = 0;
static int noteTotalLines = 0;
static float noteScrollRepeatTimer = 0.0f;
static bool noteDeleteConfirmPending = false;
// Scroll dell'elenco note: quante note (nell'ordine per data di
// modifica) sono nascoste sopra la prima visibile. 4 righe entrano
// comodamente nello spazio disponibile con le dimensioni attuali.
#define NOTES_LIST_VISIBLE_COUNT 4
static int notesListScrollOffset = 0;
static int notesListSelectedIndex = 0; // indice assoluto nell'elenco ordinato, segue lo scroll
static float notesListScrollRepeatTimer = 0.0f;
// Valori in modifica nella schermata di selezione mese/anno, applicati a
// calDisplayMonth/Year solo alla conferma
static int calEditMonth = 0, calEditYear = 2026;

static void loadDiary() {
    FILE* f = fopen(SAVE_PATH_DIARY, "rb");
    if (f) { fread(diaryEntries, sizeof(DiaryEntry), DIARY_MAX_ENTRIES, f); fclose(f); }
    else   { memset(diaryEntries, 0, sizeof(diaryEntries)); }
}
static void saveDiary() {
    FILE* f = fopen(SAVE_PATH_DIARY, "wb");
    if (f) { fwrite(diaryEntries, sizeof(DiaryEntry), DIARY_MAX_ENTRIES, f); fclose(f); }
}
// Trova la voce per una data, o -1 se non esiste ancora
static int findDiaryEntry(int year, int month, int day) {
    for (int i = 0; i < DIARY_MAX_ENTRIES; i++) {
        if (diaryEntries[i].used && (int)diaryEntries[i].year == year &&
            (int)diaryEntries[i].month == month && (int)diaryEntries[i].day == day) {
            return i;
        }
    }
    return -1;
}
// Trova uno slot libero (o riusa quello già esistente per la stessa data)
static int getOrCreateDiaryEntry(int year, int month, int day) {
    int existing = findDiaryEntry(year, month, day);
    if (existing >= 0) return existing;
    for (int i = 0; i < DIARY_MAX_ENTRIES; i++) {
        if (!diaryEntries[i].used) {
            diaryEntries[i].used  = 1;
            diaryEntries[i].year  = year;
            diaryEntries[i].month = month;
            diaryEntries[i].day   = day;
            diaryEntries[i].text[0] = '\0';
            return i;
        }
    }
    return -1; // array pieno (200 voci) — nessuno slot libero
}
// Trova la voce di diario più vicina prima (direction<0) o dopo
// (direction>0) una data, saltando i giorni senza testo scritto.
// Restituisce true e riempie outYear/outMonth/outDay se trovata.
static bool findAdjacentDiaryEntry(int year, int month, int day, int direction, int* outYear, int* outMonth, int* outDay) {
    long currentKey = (long)year * 10000L + (long)month * 100L + (long)day;
    long bestKey = (direction > 0) ? 999999999L : -999999999L;
    int bestIdx = -1;
    for (int i = 0; i < DIARY_MAX_ENTRIES; i++) {
        if (!diaryEntries[i].used || !diaryEntries[i].text[0]) continue;
        long key = (long)diaryEntries[i].year * 10000L + (long)diaryEntries[i].month * 100L + (long)diaryEntries[i].day;
        if (direction > 0) {
            if (key > currentKey && key < bestKey) { bestKey = key; bestIdx = i; }
        } else {
            if (key < currentKey && key > bestKey) { bestKey = key; bestIdx = i; }
        }
    }
    if (bestIdx < 0) return false;
    *outYear  = diaryEntries[bestIdx].year;
    *outMonth = diaryEntries[bestIdx].month;
    *outDay   = diaryEntries[bestIdx].day;
    return true;
}

/* -------------------- NOTE -------------------- */
// Stesso stile del diario (array a dimensione fissa), ma senza legame
// con una data del calendario: voci libere, con orario di creazione e
// di ultima modifica per poterle ordinare.
#define NOTE_MAX_ENTRIES 100
#define NOTE_TEXT_MAX 200
typedef struct {
    u32 used;
    u32 createdTime;  // timestamp unix
    u32 modifiedTime; // timestamp unix, aggiornato ad ogni salvataggio
    char text[NOTE_TEXT_MAX];
} NoteEntry;
static NoteEntry notes[NOTE_MAX_ENTRIES];

static void loadNotes() {
    FILE* f = fopen(SAVE_PATH_NOTES, "rb");
    if (f) { fread(notes, sizeof(NoteEntry), NOTE_MAX_ENTRIES, f); fclose(f); }
    else   { memset(notes, 0, sizeof(notes)); }
}
static void saveNotes() {
    FILE* f = fopen(SAVE_PATH_NOTES, "wb");
    if (f) { fwrite(notes, sizeof(NoteEntry), NOTE_MAX_ENTRIES, f); fclose(f); }
}
static int createNewNote() {
    time_t now = time(NULL);
    for (int i = 0; i < NOTE_MAX_ENTRIES; i++) {
        if (!notes[i].used) {
            notes[i].used = 1;
            notes[i].createdTime = (u32)now;
            notes[i].modifiedTime = (u32)now;
            notes[i].text[0] = '\0';
            return i;
        }
    }
    return -1; // array pieno (100 note) — nessuno slot libero
}
static void deleteNote(int idx) {
    if (idx < 0 || idx >= NOTE_MAX_ENTRIES) return;
    memset(&notes[idx], 0, sizeof(NoteEntry));
    saveNotes();
}
// Riempie noteOrder con gli indici delle note in uso, ordinati dalla
// più recentemente modificata alla meno recente. Restituisce quante ne
// ha trovate.
static int getNotesOrderedByModified(int* noteOrder, int maxCount) {
    int count = 0;
    for (int i = 0; i < NOTE_MAX_ENTRIES && count < maxCount; i++) {
        if (notes[i].used) noteOrder[count++] = i;
    }
    // insertion sort semplice: poche decine di note al massimo, va benissimo
    for (int i = 1; i < count; i++) {
        int key = noteOrder[i];
        u32 keyTime = notes[key].modifiedTime;
        int j = i - 1;
        while (j >= 0 && notes[noteOrder[j]].modifiedTime < keyTime) {
            noteOrder[j + 1] = noteOrder[j];
            j--;
        }
        noteOrder[j + 1] = key;
    }
    return count;
}
// Prima riga del testo, usata come titolo nella lista
static void getNoteTitleLine(int idx, char* out, int outSize) {
    if (idx < 0 || !notes[idx].text[0]) { snprintf(out, outSize, "(Empty note)"); return; }
    const char* src = notes[idx].text;
    int i = 0;
    while (src[i] && src[i] != '\n' && i < outSize - 1) { out[i] = src[i]; i++; }
    out[i] = '\0';
    if (i == 0) snprintf(out, outSize, "(Empty note)");
}

static float envSeasonCycleTimer = 0.0f;
static void updateEnvSeasonCycle() {
    u32 mode = settings.envSeasonMode;
    if (mode != 5 && mode != 6) { envSeasonCycleTimer = 0.0f; return; }
    envSeasonCycleTimer += 0.0166f;
    float intervalSeconds = (float)envIntervalMinutes[settings.envSeasonCycleIntervalIndex % 4] * 60.0f;
    if (envSeasonCycleTimer >= intervalSeconds) {
        envSeasonCycleTimer -= intervalSeconds;
        u32 next;
        if (mode == 5) {
            next = (currentEnvSeason + 1) % 4;
        } else {
            do { next = rand() % 4; } while (next == currentEnvSeason);
        }
        if (next != currentEnvSeason) {
            forestPreviousBgImage = getForestBackgroundImageForSeason(currentForestPhase, currentEnvSeason);
            currentEnvSeason = next;
            forestBgTransitionDuration = 1.5f; // tutte le transizioni stagionali, Winter incluso (era 6.0 solo per Winter, ora uniformato a 1.5 come le altre)
            forestPhaseTransition = forestBgTransitionDuration;
            forestBgFadeSymmetric = true;
        }
    }
}
#define SUN_DAWN_HOUR   7.0f
#define SUN_DUSK_HOUR   19.0f
#define SUN_DAWN_X      90.0f
#define SUN_DUSK_X      300.0f
#define SUN_MOUNTAIN_Y  103.0f
#define SUN_PEAK_Y      30.0f
#define MOON_RISE_HOUR  FOREST_NIGHT_START
#define MOON_SET_HOUR   FOREST_DAWN_START
#define MOON_RISE_X     90.0f
#define MOON_SET_X      300.0f
#define MOON_MOUNTAIN_Y 103.0f
#define MOON_PEAK_Y     30.0f
#define FOREST_STAR_COUNT 85
typedef struct {
    float x, y;
    float phase;
    float pulseFreq;
    float pulseAmp;
    float base;
    float size;
    u32   color;
} ForestStar;
static ForestStar forestStars[FOREST_STAR_COUNT];
static const u32 forestStarColors[4] = {
    COMPILER_COLOR(255, 255, 255, 255),
    COMPILER_COLOR(200, 220, 255, 255),
    COMPILER_COLOR(255, 235, 190, 255),
    COMPILER_COLOR(255, 210, 210, 255)
};
#define FOREST_SHOOT_MAX 4
typedef struct {
    float x, y;
    float vx, vy;
    float life;
    int active;
} ForestShoot;
static ForestShoot forestShoots[FOREST_SHOOT_MAX];
#define FOREST_FIREFLY_COUNT 7
typedef struct {
    float x, y;
    float vx, vy;
    float phase1, phase2;
    float freq1, freq2;
    float blinkPhase, blinkSpeed;
} ForestFirefly;
static ForestFirefly forestFireflies[FOREST_FIREFLY_COUNT];
#define FOREST_BUTTERFLY_COUNT 2
typedef enum {
    BUTTERFLY_FLYING,
    BUTTERFLY_SEEKING,
    BUTTERFLY_PAUSED
} ButterflyState;
typedef struct {
    float x, y;
    float vx, vy;
    float phase1, phase2;
    float freq1, freq2;
    float flapPhase, flapSpeed;
    u32   color;
    bool  active;
    ButterflyState state;
    float targetX, targetY;
    float pauseTimer;
    int   seekCheckTimer;
} ForestButterfly;
static ForestButterfly forestButterflies[FOREST_BUTTERFLY_COUNT];
#define FOREST_HEDGEHOG_WAYPOINT_COUNT 11
static const float forestHedgehogPath[FOREST_HEDGEHOG_WAYPOINT_COUNT][2] = {
    {-15.0f,206.0f}, {1.0f,206.0f}, {84.0f,207.0f}, {123.0f,214.0f}, {153.0f,199.0f}, {198.0f,199.0f},
    {225.0f,199.0f}, {261.0f,226.0f}, {350.0f,224.0f}, {400.0f,197.0f}, {415.0f,197.0f}
};
typedef enum {
    HEDGEHOG_WALKING,
    HEDGEHOG_PAUSED
} HedgehogState;
typedef struct {
    float x, y;
    int   waypointIdx;
    int   direction;
    HedgehogState state;
    float pauseTimer;
    int   pauseCheckTimer;
    float legPhase;
    bool  active;
    int   blinkFramesLeft;
    int   blinkDurationLeft;
} ForestHedgehog;
#define FOREST_HEDGEHOG_COUNT 1
static ForestHedgehog forestHedgehogs[FOREST_HEDGEHOG_COUNT];
static const u32 forestButterflyColors[4] = {
    COMPILER_COLOR(225, 205, 165, 255),
    COMPILER_COLOR(140, 165, 120, 255),
    COMPILER_COLOR(130, 140, 180, 255),
    COMPILER_COLOR(195, 130, 105, 255)
};
#define FOREST_BUTTERFLY_PERCH_COUNT 11
static const float forestButterflyPerches[FOREST_BUTTERFLY_PERCH_COUNT][2] = {
    {357,190}, {362,207}, {311,229}, {270,187}, {195,219}, {207,190},
    {178,225}, {166,209}, {150,178}, {86,206},  {133,221}
};
static int forestShootTimer = 400;
static void initSingleForestShoot(int i) {
    forestShoots[i].x    = rand() % 300 + 20;
    forestShoots[i].y    = 5.0f + rand() % 40;
    forestShoots[i].vx   = -2.5f + (rand() % 20) / 10.0f;
    forestShoots[i].vy   = 1.5f + (rand() % 15) / 10.0f;
    forestShoots[i].life = 1.0f;
    forestShoots[i].active = 1;
}
static void spawnForestShoot() {
    for (int i = 0; i < FOREST_SHOOT_MAX; i++) {
        if (!forestShoots[i].active) {
            initSingleForestShoot(i);
            return;
        }
    }
}
static void resetForestButterflies() {
    for (int i = 0; i < FOREST_BUTTERFLY_COUNT; i++) {
        forestButterflies[i].x = rand() % 400;
        forestButterflies[i].y = 120.0f + rand() % 115;
        float dir = (rand() % 2 == 0) ? 1.0f : -1.0f;
        forestButterflies[i].vx = dir * (0.16f + (rand() % 12) / 100.0f);
        forestButterflies[i].vy = ((rand() % 2 == 0) ? 1.0f : -1.0f) * (0.02f + (rand() % 4) / 100.0f);
        forestButterflies[i].phase1    = rand() % 360;
        forestButterflies[i].phase2    = rand() % 360;
        forestButterflies[i].freq1     = 0.9f + (rand() % 100) / 100.0f;
        forestButterflies[i].freq2     = 0.6f + (rand() % 100) / 100.0f;
        forestButterflies[i].flapPhase = rand() % 360;
        forestButterflies[i].flapSpeed = 11.0f + (rand() % 400) / 100.0f;
        forestButterflies[i].color     = forestButterflyColors[i % 4];
        forestButterflies[i].active    = true;
        forestButterflies[i].state     = BUTTERFLY_FLYING;
        forestButterflies[i].pauseTimer = 0.0f;
        forestButterflies[i].seekCheckTimer = 120 + rand() % 180;
    }
}
static void resetForestHedgehogs() {
    for (int i = 0; i < FOREST_HEDGEHOG_COUNT; i++) {
        bool goingEast = (rand() % 2 == 0);
        forestHedgehogs[i].direction = goingEast ? 1 : -1;
        int startIdx = goingEast ? 0 : (FOREST_HEDGEHOG_WAYPOINT_COUNT - 1);
        forestHedgehogs[i].x = forestHedgehogPath[startIdx][0];
        forestHedgehogs[i].y = forestHedgehogPath[startIdx][1];
        forestHedgehogs[i].waypointIdx = startIdx + forestHedgehogs[i].direction;
        forestHedgehogs[i].state = HEDGEHOG_WALKING;
        forestHedgehogs[i].pauseTimer = 0.0f;
        forestHedgehogs[i].pauseCheckTimer = 90 + rand() % 120;
        forestHedgehogs[i].legPhase = rand() % 360;
        forestHedgehogs[i].active = true;
        forestHedgehogs[i].blinkFramesLeft = 60 + rand() % 180;
        forestHedgehogs[i].blinkDurationLeft = 0;
    }
}
#define FOREST_BIRD_NORMAL_COUNT 4
#define FOREST_BIRD_SMALL_COUNT  3
#define FOREST_BIRD_COUNT (FOREST_BIRD_NORMAL_COUNT + FOREST_BIRD_SMALL_COUNT)
typedef struct {
    float x, y;
    float vx;
    float flapPhase, flapSpeed;
    float scale;
} ForestBird;
static ForestBird forestBirds[FOREST_BIRD_COUNT];
#define FOREST_LEAF_COUNT 12
typedef struct {
    float x, y;
    float vx, vy;
    float swayPhase, swaySpeed;
    float flutterPhase;
    bool  active;
    u32   color;
    int   respawnTimer;
} ForestLeaf;
static ForestLeaf forestLeaves[FOREST_LEAF_COUNT];
static const u32 forestLeafColors[3] = {
    COMPILER_COLOR(150, 110, 60, 255),
    COMPILER_COLOR(180, 140, 70, 255),
    COMPILER_COLOR(110, 130, 60, 255)
};
#define FOREST_PLANE_TRAIL_MAX 22
typedef struct {
    float x, y;
    float alpha;
} PlaneTrailPoint;
typedef struct {
    float x, y;
    float speed;
    int   active;
    float direction;
    PlaneTrailPoint trail[FOREST_PLANE_TRAIL_MAX];
    int   head, count, trailTimer;
} ForestPlane;
static ForestPlane forestPlane;
static int forestPlaneTimer = -1;
static void spawnForestPlane() {
    if (forestPlane.active) return;
    forestPlane.y          = 44.0f + rand() % 50;
    forestPlane.head       = 0;
    forestPlane.count      = 0;
    forestPlane.trailTimer = 0;
    forestPlane.active     = 1;
    forestPlane.direction  = (rand() % 2 == 0) ? 1.0f : -1.0f;
    forestPlane.x          = (forestPlane.direction > 0.0f) ? -60.0f : 460.0f;
    forestPlane.speed      = 0.2f + (rand() % 100) / 600.0f;
}
typedef enum {
    ROBIN_HOP, ROBIN_PAUSE,
    ROBIN_FLY_UP, ROBIN_PERCHED, ROBIN_FLY_DOWN,
    ROBIN_SLEEP_FLY1, ROBIN_SLEEP_FLY2, ROBIN_SLEEPING,
    ROBIN_WAKE_FLY1, ROBIN_WAKE_FLY2,
    ROBIN_NESTVISIT_FLY_PERCH
} RobinState;
typedef enum { ROBIN_PAUSE_ALERT, ROBIN_PAUSE_PECK } RobinPausePhase;
typedef struct {
    float x, y;
    float direction;
    RobinState state;
    float hopStartX, hopTargetX;
    float hopDuration;
    float sleepPoseTimer;
    bool  isDayVisit;
    float dayVisitTimer;
    float hopProgress;
    float flyStartX, flyStartY, flyTargetX, flyTargetY;
    float flyProgress;
    float perchTimer;
    float perchLookTimer;
    bool  perchLookRight;
    int   currentPerchIndex;
    RobinPausePhase pausePhase;
    float pausePhaseTimer;
    float zoneTimer;
    float zoneChangeDeadline;
} ForestRobin;
static ForestRobin forestRobin;
#define ROBIN_MIN_X 109.0f
#define ROBIN_MAX_X 279.0f
#define ROBIN_Y     208.0f
#define ROBIN_HOP_HEIGHT   5.0f
#define ROBIN_HOP_DURATION 0.20f
#define ROBIN_PERCH_COUNT 17
static const float robinPerches[ROBIN_PERCH_COUNT][2] = {
    {158.0f,81.0f}, {40.0f,124.0f}, {66.0f,182.0f}, {114.0f,110.0f}, {168.0f,96.0f},
    {246.0f,89.0f}, {276.0f,121.0f}, {261.0f,192.0f}, {337.0f,107.0f}, {378.0f,119.0f},
    {361.0f,192.0f}, {332.0f,213.0f}, {384.0f,229.0f}, {190.0f,229.0f}, {34.0f,171.0f},
    {106.0f,66.0f}, {302.0f,52.0f}
};
// Posatoi invernali: alberi spogli, posizioni diverse rispetto alle
// altre stagioni. Condivisi da pettirosso e uccello grigio, come quelli
// normali.
#define ROBIN_PERCH_COUNT_WINTER 14
static const float robinPerchesWinter[ROBIN_PERCH_COUNT_WINTER][2] = {
    {59.0f,49.0f}, {83.0f,68.0f}, {76.0f,92.0f}, {20.0f,134.0f}, {249.0f,43.0f},
    {250.0f,17.0f}, {265.0f,40.0f}, {283.0f,76.0f}, {277.0f,118.0f}, {334.0f,106.0f},
    {383.0f,119.0f}, {383.0f,64.0f}, {376.0f,31.0f}, {333.0f,52.0f}
};
static int getActivePerchCount() {
    return (currentEnvSeason == 3) ? ROBIN_PERCH_COUNT_WINTER : ROBIN_PERCH_COUNT;
}
static float getActivePerchX(int idx) {
    return (currentEnvSeason == 3) ? robinPerchesWinter[idx][0] : robinPerches[idx][0];
}
static float getActivePerchY(int idx) {
    return (currentEnvSeason == 3) ? robinPerchesWinter[idx][1] : robinPerches[idx][1];
}
#define ROBIN_FLY_SPEED 1.1f
static bool debugRobinFlyRequested = false;
#define ROBIN_NEST_X       98.0f
#define ROBIN_NEST_Y       51.0f
#define ROBIN_SLEEP_POSE_X 97.0f
#define ROBIN_SLEEP_POSE_Y 42.0f
#define ROBIN_SLEEP_SPOT_X_NORMAL 111.0f
#define ROBIN_SLEEP_SPOT_Y_NORMAL 20.0f
// Punto di sosta prima del nido, solo in Winter (nido di sinistra,
// pettirosso): diverso da quello usato nelle altre stagioni.
#define ROBIN_SLEEP_SPOT_X_WINTER 105.0f
#define ROBIN_SLEEP_SPOT_Y_WINTER 18.0f
static float getRobinSleepSpotX() {
    return (currentEnvSeason == 3) ? ROBIN_SLEEP_SPOT_X_WINTER : ROBIN_SLEEP_SPOT_X_NORMAL;
}
static float getRobinSleepSpotY() {
    return (currentEnvSeason == 3) ? ROBIN_SLEEP_SPOT_Y_WINTER : ROBIN_SLEEP_SPOT_Y_NORMAL;
}
static bool debugRobinSleepRequested = false;
static int robinNestVisitTimer = -1;
static int  robinAutoFlyTimer = -1;
typedef enum {
    GREYBIRD_HOP, GREYBIRD_PAUSE, GREYBIRD_FLY_UP, GREYBIRD_PERCHED, GREYBIRD_FLY_DOWN,
    GREYBIRD_SLEEP_FLY1, GREYBIRD_SLEEP_FLY2, GREYBIRD_SLEEPING,
    GREYBIRD_WAKE_FLY1, GREYBIRD_WAKE_FLY2,
    GREYBIRD_NESTVISIT_FLY_PERCH
} GreyBirdState;
typedef enum { GREYBIRD_PAUSE_ALERT, GREYBIRD_PAUSE_PECK } GreyBirdPausePhase;
typedef struct {
    float x, y;
    float direction;
    GreyBirdState state;
    float hopStartX, hopTargetX;
    float hopDuration;
    float hopProgress;
    GreyBirdPausePhase pausePhase;
    float pausePhaseTimer;
    float zoneTimer, zoneChangeDeadline;
    float flyStartX, flyStartY, flyTargetX, flyTargetY, flyProgress;
    float perchTimer, perchLookTimer;
    bool  perchLookRight;
    int   currentPerchIndex;
    float sleepPoseTimer;
    bool  isDayVisit;
    float dayVisitTimer;
} ForestGreyBird;
static ForestGreyBird forestGreyBird;
static int greyBirdAutoFlyTimer = -1;
#define GREYBIRD_NEST_X       326.0f
#define GREYBIRD_NEST_Y       37.0f
#define GREYBIRD_SLEEP_POSE_X 326.0f
#define GREYBIRD_SLEEP_POSE_Y 28.0f
static int greyBirdNestVisitTimer = -1;
// In Winter gli alberi sono spogli: si vede più cielo, quindi le nuvole
// possono comparire in un range Y più ampio (0-118 invece di 40-100).
static float getRandomCloudY() {
    if (currentEnvSeason == 3) return 0.0f + (float)(rand() % 119);
    return 40.0f + (float)(rand() % 61);
}
static void initSingleCloud(int i) {
    float slot = 620.0f / (float)activeCloudCount;
    clouds[i].x      = -120.0f + slot * (float)i + slot * 0.5f + (float)(rand() % 80 - 40);
    clouds[i].y      = getRandomCloudY();
    clouds[i].layer  = (float)i / (float)activeCloudCount;
    if (i == activeCloudCount - 1) {
        clouds[i].scale = 1.0f;
    } else {
        clouds[i].scale = (0.35f + clouds[i].layer * 0.45f) * 1.4f;
    }
    clouds[i].speed  = (0.05f + (clouds[i].scale * 0.12f)) * 2.0f;
    clouds[i].wobble = rand() % 360;
}
static void initClouds() {
    for (int i = 0; i < activeCloudCount; i++) {
        initSingleCloud(i);
    }
}
static void initForestSky() {
    initClouds();
    for (int i = 0; i < FOREST_STAR_COUNT; i++) {
        forestStars[i].x         = rand() % 400;
        forestStars[i].y         = 5.0f + rand() % 110;
        forestStars[i].phase     = rand() % 360;
        forestStars[i].pulseFreq = 0.08f + (rand() % 17) / 100.0f;
        forestStars[i].pulseAmp  = 0.2f + (rand() % 80) / 100.0f;
        forestStars[i].base      = 0.2f + (rand() % 40) / 100.0f;
        forestStars[i].size      = 0.5f + (rand() % 25) / 100.0f;
        forestStars[i].color     = forestStarColors[rand() % 4];
    }
    for (int i = 0; i < FOREST_SHOOT_MAX; i++) forestShoots[i].active = 0;
    for (int i = 0; i < FOREST_FIREFLY_COUNT; i++) {
        forestFireflies[i].x = rand() % 400;
        forestFireflies[i].y = 120.0f + rand() % 115;
        float dir = (rand() % 2 == 0) ? 1.0f : -1.0f;
        forestFireflies[i].vx = dir * (0.08f + (rand() % 10) / 100.0f);
        forestFireflies[i].vy = ((rand() % 2 == 0) ? 1.0f : -1.0f) * (0.015f + (rand() % 3) / 100.0f);
        forestFireflies[i].phase1     = rand() % 360;
        forestFireflies[i].phase2     = rand() % 360;
        forestFireflies[i].freq1      = 0.8f + (rand() % 100) / 100.0f;
        forestFireflies[i].freq2      = 0.5f + (rand() % 100) / 100.0f;
        forestFireflies[i].blinkPhase = rand() % 360;
        forestFireflies[i].blinkSpeed = 1.2f + (rand() % 100) / 100.0f;
    }
    resetForestButterflies();
    for (int i = 0; i < FOREST_BIRD_COUNT; i++) {
        float dir = (rand() % 2 == 0) ? 1.0f : -1.0f;
        bool isSmall = (i >= FOREST_BIRD_NORMAL_COUNT);
        forestBirds[i].x         = rand() % 400;
        forestBirds[i].y         = isSmall ? (56.0f + rand() % 34) : (10.0f + rand() % 60);
        forestBirds[i].vx        = dir * (0.25f + (rand() % 15) / 100.0f);
        forestBirds[i].flapPhase = rand() % 360;
        forestBirds[i].flapSpeed = isSmall ? (8.0f + (rand() % 250) / 100.0f) : (5.0f + (rand() % 200) / 100.0f);
        forestBirds[i].scale     = isSmall ? 0.55f : 1.0f;
    }
    for (int i = 0; i < FOREST_LEAF_COUNT; i++) {
        forestLeaves[i].active = false;
        forestLeaves[i].respawnTimer = rand() % 400;
    }
    rareCloud.active = false;
    rareCloud.spawnTimer = 3600 + rand() % 3600;
    forestPlane.active = 0;
    forestPlaneTimer = 1200 + rand() % 9600;
    forestRobin.currentPerchIndex = -1;
    forestRobin.direction = 1.0f;
    forestRobin.hopDuration = ROBIN_HOP_DURATION;
    forestRobin.sleepPoseTimer = 0.0f;
    forestRobin.isDayVisit = false;
    forestRobin.dayVisitTimer = 0.0f;
    robinNestVisitTimer = 16800 + rand() % 2400;
    forestGreyBird.x = ROBIN_MAX_X;
    forestGreyBird.y = ROBIN_Y;
    forestGreyBird.direction = -1.0f;
    forestGreyBird.state = GREYBIRD_PAUSE;
    forestGreyBird.pausePhase = GREYBIRD_PAUSE_ALERT;
    forestGreyBird.pausePhaseTimer = 0.4f + (rand() % 40) / 100.0f;
    forestGreyBird.zoneTimer = 0.0f;
    forestGreyBird.zoneChangeDeadline = 4.0f + (rand() % 200) / 100.0f;
    forestGreyBird.hopDuration = ROBIN_HOP_DURATION;
    forestGreyBird.currentPerchIndex = -1;
    greyBirdAutoFlyTimer = 900 + rand() % 1800;
    time_t rawInitRobin = time(NULL);
    struct tm* tmInitRobin = localtime(&rawInitRobin);
    float hourFracInitRobin = tmInitRobin->tm_hour + tmInitRobin->tm_min / 60.0f;
    bool startAlreadyAsleep = (currentForestPhase == FOREST_PHASE_NIGHT) ||
        (currentForestPhase == FOREST_PHASE_DUSK && hourFracInitRobin >= FOREST_DUSK_START + 1.0f);
    if (startAlreadyAsleep) {
        forestRobin.x = ROBIN_SLEEP_POSE_X;
        forestRobin.y = ROBIN_SLEEP_POSE_Y;
        forestRobin.state = ROBIN_SLEEPING;
    } else {
        forestRobin.x = ROBIN_MIN_X;
        forestRobin.y = ROBIN_Y;
        forestRobin.state = ROBIN_PAUSE;
        forestRobin.pausePhase = ROBIN_PAUSE_ALERT;
        forestRobin.pausePhaseTimer = 0.4f + (rand() % 40) / 100.0f;
        forestRobin.zoneTimer = 0.0f;
        forestRobin.zoneChangeDeadline = 4.0f + (rand() % 200) / 100.0f;
    }
    robinAutoFlyTimer = 900 + rand() % 1800;
    forestGreyBird.sleepPoseTimer = 0.0f;
    forestGreyBird.isDayVisit = false;
    forestGreyBird.dayVisitTimer = 0.0f;
    if (startAlreadyAsleep) {
        forestGreyBird.x = GREYBIRD_SLEEP_POSE_X;
        forestGreyBird.y = GREYBIRD_SLEEP_POSE_Y;
        forestGreyBird.state = GREYBIRD_SLEEPING;
    }
    greyBirdNestVisitTimer = 16800 + rand() % 2400;
}
static float getNightFactor(void);
static void drawSun(float t) {
    time_t rawSun = time(NULL);
    struct tm* tmSun = localtime(&rawSun);
    float hourFrac = tmSun->tm_hour + tmSun->tm_min / 60.0f;
    if (hourFrac <= SUN_DAWN_HOUR || hourFrac >= SUN_DUSK_HOUR) return;
    float dayFactor = 1.0f - getNightFactor();
    if (dayFactor <= 0.0f) return;
    float progress = (hourFrac - SUN_DAWN_HOUR) / (SUN_DUSK_HOUR - SUN_DAWN_HOUR);
    float sunX = SUN_DAWN_X + (SUN_DUSK_X - SUN_DAWN_X) * progress;
    float u = (progress - 0.5f) * 2.0f;
    float sunY = SUN_PEAK_Y + (SUN_MOUNTAIN_Y - SUN_PEAK_Y) * (u * u);
    float fadeIn  = progress * (SUN_DUSK_HOUR - SUN_DAWN_HOUR) * 60.0f / 45.0f;
    float fadeOut = (1.0f - progress) * (SUN_DUSK_HOUR - SUN_DAWN_HOUR) * 60.0f / 45.0f;
    float fade = fminf(1.0f, fminf(fadeIn, fadeOut));
    if (fade < 0.0f) fade = 0.0f;
    fade *= dayFactor;
    float pulse = 0.5f + 0.5f * sinf(t * 0.15f);
    u8 core = (u8)(245 + pulse * 10);
    float baseR[4] = {255.0f, 255.0f, 255.0f, (float)core};
    float baseG[4] = {250.0f, 245.0f, 240.0f, 250.0f};
    float baseB[4] = {210.0f, 190.0f, 170.0f, 220.0f};
    float ringAlpha[4] = {35.0f, 60.0f, 110.0f, 255.0f};
    float ringRadius[4] = {34.0f, 24.0f, 15.0f, 9.0f};
    for (int i = 0; i < 4; i++) {
        u8 r = (u8)baseR[i];
        u8 g = (u8)baseG[i];
        u8 b = (u8)baseB[i];
        u8 a = (u8)(ringAlpha[i] * fade);
        C2D_DrawCircleSolid(sunX, sunY, 0.02f, ringRadius[i], C2D_Color32(r, g, b, a));
    }
}
static void drawClouds(float t) {
    for (int i = 0; i < activeCloudCount; i++) {
        float x  = clouds[i].x;
        float y  = clouds[i].y;
        float sc = clouds[i].scale;
        float fx = x + sinf(t * 0.05f + clouds[i].wobble) * 2.5f;
        float fy = y + cosf(t * 0.04f + clouds[i].wobble) * 1.5f;
        C2D_Image regularImg = cloudImage5;
        if (regularImg.tex) {
            float imgAlpha = (160.0f + clouds[i].layer * 70.0f) / 255.0f;
            C2D_ImageTint cloudTint;
            C2D_AlphaImageTint(&cloudTint, imgAlpha);
            float imgW = regularImg.subtex->width;
            float imgH = regularImg.subtex->height;
            float drawX = fx - imgW * sc * 0.5f;
            float drawY = fy - imgH * sc * 0.5f;
            C2D_DrawImageAt(regularImg, drawX, drawY, 0.02f, &cloudTint, sc, sc);
        }
        float dir = getForestCloudDirMul(i);
        clouds[i].x += clouds[i].speed * 0.5f * dir * getForestCloudSpeedMultiplier();
        if (clouds[i].x >  500) { clouds[i].x = -120; clouds[i].y = getRandomCloudY(); }
        if (clouds[i].x < -120) { clouds[i].x =  500; clouds[i].y = getRandomCloudY(); }
    }
}
static void updateAndDrawRareCloud(float t) {
    if (!rareCloud.active) {
        rareCloud.spawnTimer--;
        if (rareCloud.spawnTimer <= 0) {
            bool fromLeft = (rand() % 2 == 0);
            rareCloud.x = fromLeft ? -150.0f : 550.0f;
            rareCloud.movingRight = fromLeft;
            rareCloud.active = true;
        }
        return;
    }
    rareCloud.x += rareCloud.movingRight ? RARE_CLOUD_SPEED : -RARE_CLOUD_SPEED;
    if (rareCloud.x > 560.0f || rareCloud.x < -160.0f) {
        rareCloud.active = false;
        rareCloud.spawnTimer = 7200 + rand() % 10800;
        return;
    }
    if (cloudImage2.tex) {
        float fx = rareCloud.x + sinf(t * 0.04f) * 2.0f;
        float imgW = cloudImage2.subtex->width;
        float imgH = cloudImage2.subtex->height;
        float drawX = fx - imgW * RARE_CLOUD_SCALE * 0.5f;
        float drawY = RARE_CLOUD_Y - imgH * RARE_CLOUD_SCALE * 0.5f;
        C2D_ImageTint tint;
        C2D_AlphaImageTint(&tint, 0.75f);
        C2D_DrawImageAt(cloudImage2, drawX, drawY, 0.02f, &tint, RARE_CLOUD_SCALE, RARE_CLOUD_SCALE);
    }
}
static float getForestPhaseFactor(ForestPhase phase) {
    bool curMatch  = (currentForestPhase  == phase);
    bool prevMatch = (previousForestPhase == phase);
    if (forestPhaseTransition > 0.0f && previousForestPhase != currentForestPhase) {
        float fadeIn = 1.0f - (forestPhaseTransition / FOREST_PHASE_TRANSITION_SECONDS);
        if (curMatch)  return fadeIn;
        if (prevMatch) return 1.0f - fadeIn;
        return 0.0f;
    }
    return curMatch ? 1.0f : 0.0f;
}
static float getNightFactor(void) {
    return getForestPhaseFactor(FOREST_PHASE_NIGHT);
}
static void updateAndDrawForestPlane(float t) {
    float nightFactor = getNightFactor();
    if (currentForestPhase == FOREST_PHASE_NIGHT) {
        if (forestPlaneTimer > 0) forestPlaneTimer--;
        else {
            spawnForestPlane();
            forestPlaneTimer = 1200 + rand() % 9600;
        }
    }
    ForestPlane* p = &forestPlane;
    if (!p->active) return;
    if (p->active == 1) {
        p->x += p->speed * p->direction;
        p->trailTimer++;
        if (p->trailTimer >= 12) {
            p->trailTimer = 0;
            float tx = (p->direction > 0.0f) ? p->x        : p->x + 2.5f;
            float ty = (p->direction > 0.0f) ? p->y + 1.0f : p->y + 0.8f;
            p->trail[p->head].x     = tx;
            p->trail[p->head].y     = ty;
            p->trail[p->head].alpha = (p->direction > 0.0f) ? 0.6f : 0.5f;
            p->head = (p->head + 1) % FOREST_PLANE_TRAIL_MAX;
            if (p->count < FOREST_PLANE_TRAIL_MAX) p->count++;
        }
        bool offscreen = (p->direction > 0.0f) ? (p->x > 460.0f) : (p->x < -60.0f);
        if (offscreen) p->active = 2;
    }
    bool trailVisible = false;
    int firstIdx = (p->head - p->count + FOREST_PLANE_TRAIL_MAX) % FOREST_PLANE_TRAIL_MAX;
    for (int i = 0; i < p->count; i++) {
        int curr = (firstIdx + i) % FOREST_PLANE_TRAIL_MAX;
        if (p->trail[curr].alpha > 0.0f) {
            p->trail[curr].alpha -= 0.001f;
            if (p->trail[curr].alpha < 0.0f) p->trail[curr].alpha = 0.0f;
            else trailVisible = true;
        }
    }
    if (p->active == 2 && !trailVisible) {
        p->active = 0;
        p->count  = 0;
        return;
    }
    bool ltr = (p->direction > 0.0f);
    if (p->count > 1) {
        int firstIdx2 = (p->head - p->count + FOREST_PLANE_TRAIL_MAX) % FOREST_PLANE_TRAIL_MAX;
        for (int i = 1; i < p->count; i++) {
            int prev = (firstIdx2 + i - 1) % FOREST_PLANE_TRAIL_MAX;
            int curr = (firstIdx2 + i) % FOREST_PLANE_TRAIL_MAX;
            if (p->trail[curr].alpha <= 0.0f) continue;
            u8 a = (u8)(p->trail[curr].alpha * 255.0f * nightFactor);
            u32 tc = C2D_Color32(200, 200, 210, a);
            float lw = ltr ? 1.2f : 1.0f;
            C2D_DrawLine(p->trail[prev].x, p->trail[prev].y, tc, p->trail[curr].x, p->trail[curr].y, tc, lw, 0.01f);
        }
    }
    if (p->count > 0 && p->active == 1) {
        int lastIdx = (p->head - 1 + FOREST_PLANE_TRAIL_MAX) % FOREST_PLANE_TRAIL_MAX;
        u8  a  = (u8)(p->trail[lastIdx].alpha * 255.0f * nightFactor);
        u32 tc = C2D_Color32(200, 200, 210, a);
        float ex = ltr ? p->x        : p->x + 2.5f;
        float ey = ltr ? p->y + 1.0f : p->y + 0.8f;
        float lw = ltr ? 1.2f : 1.0f;
        C2D_DrawLine(p->trail[lastIdx].x, p->trail[lastIdx].y, tc, ex, ey, tc, lw, 0.01f);
    }
    if (p->active == 1) {
        C2D_DrawRectSolid(p->x, p->y,        0.01f, 2.5f, 1.8f, C2D_Color32(40, 40, 50, (u8)(220.0f * nightFactor)));
        C2D_DrawRectSolid(p->x, p->y + 0.5f, 0.01f, 0.8f, 0.8f, C2D_Color32(0, 230, 50, (u8)(255.0f * nightFactor)));
        if (((int)(t * 60.0f)) % 45 < 4)
            C2D_DrawCircleSolid(p->x + 2.5f, p->y + 0.8f, 0.01f, 1.2f, C2D_Color32(255, 255, 255, (u8)(255.0f * nightFactor)));
    }
}
static void updateAndDrawForestRobin(float t) {
    ForestRobin* r = &forestRobin;
    bool inSleepSequence = (r->state == ROBIN_SLEEP_FLY1 || r->state == ROBIN_SLEEP_FLY2 ||
                             r->state == ROBIN_SLEEPING   || r->state == ROBIN_WAKE_FLY1  ||
                             r->state == ROBIN_WAKE_FLY2  || r->state == ROBIN_NESTVISIT_FLY_PERCH);
    time_t rawRobin = time(NULL);
    struct tm* tmRobin = localtime(&rawRobin);
    float hourFracRobin = tmRobin->tm_hour + tmRobin->tm_min / 60.0f;
    bool duskFirstHour = (currentForestPhase == FOREST_PHASE_DUSK) && (hourFracRobin < FOREST_DUSK_START + 1.0f);
    static int wasHourSlot = -1;
    bool hourJustBecameSleepHour = (tmRobin->tm_hour == (int)(FOREST_DUSK_START + 1.0f) && wasHourSlot != tmRobin->tm_hour);
    wasHourSlot = tmRobin->tm_hour;
    static bool wasAwakePhase = false;
    bool isAwakePhase = (currentForestPhase == FOREST_PHASE_DAWN || currentForestPhase == FOREST_PHASE_DAY || duskFirstHour);
    if ((hourJustBecameSleepHour || debugRobinSleepRequested) && !inSleepSequence) {
        r->flyStartX = r->x;  r->flyStartY = r->y;
        r->flyTargetX = getRobinSleepSpotX();  r->flyTargetY = getRobinSleepSpotY();
        r->flyProgress = 0.0f;
        r->state = ROBIN_SLEEP_FLY1;
        r->isDayVisit = false;
        inSleepSequence = true;
    } else if (debugRobinSleepRequested && r->state == ROBIN_SLEEPING) {
        r->flyStartX = r->x;  r->flyStartY = r->y;
        r->flyTargetX = getRobinSleepSpotX();  r->flyTargetY = getRobinSleepSpotY();
        r->flyProgress = 0.0f;
        r->state = ROBIN_WAKE_FLY1;
    }
    debugRobinSleepRequested = false;
    if (isAwakePhase && !wasAwakePhase && r->state == ROBIN_SLEEPING) {
        r->flyStartX = r->x;  r->flyStartY = r->y;
        r->flyTargetX = getRobinSleepSpotX();  r->flyTargetY = getRobinSleepSpotY();
        r->flyProgress = 0.0f;
        r->state = ROBIN_WAKE_FLY1;
        inSleepSequence = true;
    }
    wasAwakePhase = isAwakePhase;
    if (robinNestVisitTimer < 0) robinNestVisitTimer = 17700 + rand() % 600;
    bool isDayPhaseStrict = (currentForestPhase == FOREST_PHASE_DAY);
    if (isDayPhaseStrict && (r->state == ROBIN_HOP || r->state == ROBIN_PAUSE)) {
        robinNestVisitTimer--;
        if (robinNestVisitTimer <= 0) {
            int perchIdx = rand() % getActivePerchCount();
            r->flyStartX = r->x;  r->flyStartY = r->y;
            r->flyTargetX = getActivePerchX(perchIdx);
            r->flyTargetY = getActivePerchY(perchIdx);
            r->flyProgress = 0.0f;
            r->state = ROBIN_NESTVISIT_FLY_PERCH;
            r->isDayVisit = true;
            r->currentPerchIndex = perchIdx;
            inSleepSequence = true;
            robinNestVisitTimer = 17700 + rand() % 600;
        }
    }
    float dayFactor = getForestPhaseFactor(FOREST_PHASE_DAY);
    bool groundLikeActive = (dayFactor > 0.0f) || isAwakePhase;
    bool skipRobinDraw = false;
    if (inSleepSequence) {
        if (r->state == ROBIN_SLEEP_FLY1 || r->state == ROBIN_SLEEP_FLY2 ||
            r->state == ROBIN_WAKE_FLY1  || r->state == ROBIN_WAKE_FLY2 ||
            r->state == ROBIN_NESTVISIT_FLY_PERCH) {
            float ddx = r->flyTargetX - r->flyStartX, ddy = r->flyTargetY - r->flyStartY;
            float dist = sqrtf(ddx * ddx + ddy * ddy);
            if (dist < 1.0f) dist = 1.0f;
            r->flyProgress += ROBIN_FLY_SPEED / dist;
            if (r->flyProgress >= 1.0f) {
                r->x = r->flyTargetX;
                r->y = r->flyTargetY;
                if (r->state == ROBIN_NESTVISIT_FLY_PERCH) {
                    r->flyStartX = r->x;  r->flyStartY = r->y;
                    r->flyTargetX = getRobinSleepSpotX();  r->flyTargetY = getRobinSleepSpotY();
                    r->flyProgress = 0.0f;
                    r->state = ROBIN_SLEEP_FLY1;
                } else if (r->state == ROBIN_SLEEP_FLY1) {
                    r->flyStartX = r->x;  r->flyStartY = r->y;
                    r->flyTargetX = ROBIN_SLEEP_POSE_X;  r->flyTargetY = ROBIN_SLEEP_POSE_Y;
                    r->flyProgress = 0.0f;
                    r->state = ROBIN_SLEEP_FLY2;
                } else if (r->state == ROBIN_SLEEP_FLY2) {
                    r->state = ROBIN_SLEEPING;
                    r->sleepPoseTimer = 0.3f + (rand() % 30) / 100.0f;
                    if (r->isDayVisit) r->dayVisitTimer = 10.0f;
                } else if (r->state == ROBIN_WAKE_FLY1) {
                    r->flyStartX = r->x;  r->flyStartY = r->y;
                    r->flyTargetX = ROBIN_MIN_X + rand() % (int)(ROBIN_MAX_X - ROBIN_MIN_X);
                    r->flyTargetY = ROBIN_Y;
                    r->flyProgress = 0.0f;
                    r->state = ROBIN_WAKE_FLY2;
                } else {
                    r->state = ROBIN_PAUSE;
                    r->pausePhase = ROBIN_PAUSE_ALERT;
                    r->pausePhaseTimer = 0.4f + (rand() % 40) / 100.0f;
                    r->zoneTimer = 0.0f;
                    r->zoneChangeDeadline = 4.0f + (rand() % 200) / 100.0f;
                    r->isDayVisit = false;
                }
            } else {
                r->x = r->flyStartX + ddx * r->flyProgress;
                r->y = r->flyStartY + ddy * r->flyProgress;
            }
        } else {
            if (r->sleepPoseTimer > 0.0f) r->sleepPoseTimer -= 0.0166f;
            if (r->isDayVisit) {
                r->dayVisitTimer -= 0.0166f;
                if (r->dayVisitTimer <= 0.0f) {
                    r->flyStartX = r->x;  r->flyStartY = r->y;
                    r->flyTargetX = getRobinSleepSpotX();  r->flyTargetY = getRobinSleepSpotY();
                    r->flyProgress = 0.0f;
                    r->state = ROBIN_WAKE_FLY1;
                }
            }
        }
    } else if (!groundLikeActive) {
        if (currentForestPhase == FOREST_PHASE_NIGHT || currentForestPhase == FOREST_PHASE_DUSK) {
            r->x = ROBIN_SLEEP_POSE_X;
            r->y = ROBIN_SLEEP_POSE_Y;
            r->state = ROBIN_SLEEPING;
            r->sleepPoseTimer = 0.0f;
            r->isDayVisit = false;
        } else {
            skipRobinDraw = true;
        }
    } else {
        bool grounded = (r->state == ROBIN_HOP || r->state == ROBIN_PAUSE);
        if (robinAutoFlyTimer < 0) robinAutoFlyTimer = 900 + rand() % 1800;
        if (grounded) {
            robinAutoFlyTimer--;
            if (robinAutoFlyTimer <= 0) debugRobinFlyRequested = true;
        }
        if (debugRobinFlyRequested && grounded) {
            bool visitNest = duskFirstHour && (rand() % 100 < 35);
            int perchIdx = visitNest ? -1 : (rand() % getActivePerchCount());
            r->flyStartX = r->x;  r->flyStartY = r->y;
            r->flyTargetX = visitNest ? getRobinSleepSpotX() : getActivePerchX(perchIdx);
            r->flyTargetY = visitNest ? getRobinSleepSpotY() : getActivePerchY(perchIdx);
            r->flyProgress = 0.0f;
            r->state = ROBIN_FLY_UP;
            r->currentPerchIndex = perchIdx;
            r->zoneTimer = 0.0f;
            r->zoneChangeDeadline = 4.0f + (rand() % 200) / 100.0f;
            robinAutoFlyTimer = 900 + rand() % 1800;
        }
        debugRobinFlyRequested = false;
        if (r->state == ROBIN_PAUSE) {
            r->zoneTimer += 0.0166f;
            r->pausePhaseTimer -= 0.0166f;
            if (r->pausePhaseTimer <= 0.0f) {
                if (r->pausePhase == ROBIN_PAUSE_ALERT) {
                    bool forceZoneChange = (r->zoneTimer >= r->zoneChangeDeadline);
                    if (!forceZoneChange && rand() % 100 < 55) {
                        r->pausePhase = ROBIN_PAUSE_PECK;
                        r->pausePhaseTimer = 0.15f + (rand() % 10) / 100.0f;
                    } else {
                        if (rand() % 100 < 35) r->direction = -r->direction;
                        float dist = forceZoneChange ? (20.0f + rand() % 16) : (5.0f + rand() % 5);
                        float targetX = r->x + r->direction * dist;
                        if (targetX >= ROBIN_MAX_X) { targetX = ROBIN_MAX_X; r->direction = -1.0f; }
                        else if (targetX <= ROBIN_MIN_X) { targetX = ROBIN_MIN_X; r->direction = 1.0f; }
                        if (fabsf(targetX - r->x) < 2.0f) {
                            r->pausePhase = ROBIN_PAUSE_ALERT;
                            r->pausePhaseTimer = 0.3f + (rand() % 30) / 100.0f;
                        } else {
                            r->hopStartX  = r->x;
                            r->hopTargetX = targetX;
                            r->hopProgress = 0.0f;
                            r->hopDuration = forceZoneChange ? 0.12f : ROBIN_HOP_DURATION;
                            r->state = ROBIN_HOP;
                        }
                    }
                } else {
                    r->pausePhase = ROBIN_PAUSE_ALERT;
                    r->pausePhaseTimer = 0.3f + (rand() % 30) / 100.0f;
                }
            }
        } else if (r->state == ROBIN_HOP) {
            r->hopProgress += 0.0166f / r->hopDuration;
            if (r->hopProgress >= 1.0f) {
                r->x = r->hopTargetX;
                r->state = ROBIN_PAUSE;
                r->pausePhase = ROBIN_PAUSE_ALERT;
                r->pausePhaseTimer = 0.4f + (rand() % 40) / 100.0f;
                r->zoneTimer = 0.0f;
                r->zoneChangeDeadline = 4.0f + (rand() % 200) / 100.0f;
            } else {
                r->x = r->hopStartX + (r->hopTargetX - r->hopStartX) * r->hopProgress;
            }
        } else if (r->state == ROBIN_FLY_UP || r->state == ROBIN_FLY_DOWN) {
            float ddx = r->flyTargetX - r->flyStartX, ddy = r->flyTargetY - r->flyStartY;
            float dist = sqrtf(ddx * ddx + ddy * ddy);
            if (dist < 1.0f) dist = 1.0f;
            r->flyProgress += ROBIN_FLY_SPEED / dist;
            if (r->flyProgress >= 1.0f) {
                r->x = r->flyTargetX;
                r->y = r->flyTargetY;
                if (r->state == ROBIN_FLY_UP) {
                    r->state = ROBIN_PERCHED;
                    r->perchTimer = 2.0f + (rand() % 400) / 100.0f;
                    r->perchLookTimer = 0.7f + (rand() % 80) / 100.0f;
                    r->perchLookRight = (rand() % 2 == 0);
                } else {
                    r->state = ROBIN_PAUSE;
                    r->pausePhase = ROBIN_PAUSE_ALERT;
                    r->pausePhaseTimer = 0.4f + (rand() % 40) / 100.0f;
                }
            } else {
                r->x = r->flyStartX + ddx * r->flyProgress;
                r->y = r->flyStartY + ddy * r->flyProgress;
            }
        } else {
            r->perchTimer -= 0.0166f;
            r->perchLookTimer -= 0.0166f;
            if (r->perchLookTimer <= 0.0f) {
                r->perchLookRight = !r->perchLookRight;
                r->perchLookTimer = 0.7f + (rand() % 80) / 100.0f;
            }
            if (r->perchTimer <= 0.0f) {
                r->flyStartX = r->x;  r->flyStartY = r->y;
                if (duskFirstHour && rand() % 100 < 30) {
                    r->flyTargetX = getRobinSleepSpotX();
                    r->flyTargetY = getRobinSleepSpotY();
                    r->flyProgress = 0.0f;
                    r->state = ROBIN_FLY_UP;
                    r->currentPerchIndex = -1;
                } else if (getActivePerchCount() > 1 && rand() % 100 < 70) {
                    int nextPerch;
                    do { nextPerch = rand() % getActivePerchCount(); } while (nextPerch == r->currentPerchIndex);
                    r->flyTargetX = getActivePerchX(nextPerch);
                    r->flyTargetY = getActivePerchY(nextPerch);
                    r->flyProgress = 0.0f;
                    r->state = ROBIN_FLY_UP;
                    r->currentPerchIndex = nextPerch;
                } else {
                    r->flyTargetX = ROBIN_MIN_X + rand() % (int)(ROBIN_MAX_X - ROBIN_MIN_X);
                    r->flyTargetY = ROBIN_Y;
                    r->flyProgress = 0.0f;
                    r->state = ROBIN_FLY_DOWN;
                    r->currentPerchIndex = -1;
                }
            }
        }
    }
    if (!skipRobinDraw) {
        C2D_Image img;
        bool isFlying = (r->state == ROBIN_FLY_UP || r->state == ROBIN_FLY_DOWN ||
                          r->state == ROBIN_SLEEP_FLY1 || r->state == ROBIN_SLEEP_FLY2 ||
                          r->state == ROBIN_WAKE_FLY1  || r->state == ROBIN_WAKE_FLY2  ||
                          r->state == ROBIN_NESTVISIT_FLY_PERCH);
        if (isFlying) {
            bool flyingLeft = (r->flyTargetX < r->flyStartX);
            bool wingDown = sinf(t * 25.0f) > 0.0f;
            if (flyingLeft) {
                img = wingDown ? robinFlyImageSxB : robinFlyImageSx;
                if (!img.tex) img = wingDown ? robinFlyImageSx : robinFlyImageSxB;
            } else {
                img = wingDown ? robinFlyImageDxB : robinFlyImageDx;
                if (!img.tex) img = wingDown ? robinFlyImageDx : robinFlyImageDxB;
            }
            if (!img.tex) img = flyingLeft ? robinFlyImageSx : robinFlyImageDx;
            if (!img.tex) img = flyingLeft ? robinFlyImageDx : robinFlyImageSx;
        } else if (r->state == ROBIN_SLEEPING) {
            if (currentForestPhase == FOREST_PHASE_DUSK) {
                bool blinkToSleepPose = (fmodf(t, 4.0f) < 0.2f);
                if (blinkToSleepPose) {
                    img = robinSleepImage;
                    if (!img.tex) img = (r->direction >= 0.0f) ? robinImage : robinImage2;
                } else {
                    img = (r->direction >= 0.0f) ? robinImage : robinImage2;
                    if (!img.tex) img = (r->direction >= 0.0f) ? robinImage2 : robinImage;
                }
            } else if (r->sleepPoseTimer > 0.0f) {
                img = (r->direction >= 0.0f) ? robinImage : robinImage2;
                if (!img.tex) img = (r->direction >= 0.0f) ? robinImage2 : robinImage;
            } else {
                img = robinSleepImage;
                if (!img.tex) img = (r->direction >= 0.0f) ? robinImage : robinImage2;
            }
        } else if (r->state == ROBIN_PERCHED) {
            img = r->perchLookRight ? robinImage : robinImage2;
            if (!img.tex) img = r->perchLookRight ? robinImage2 : robinImage;
        } else if (r->state == ROBIN_PAUSE && r->pausePhase == ROBIN_PAUSE_PECK) {
            img = (r->direction >= 0.0f) ? robinPeckDx : robinPeckSx;
            if (!img.tex) img = (r->direction >= 0.0f) ? robinImage : robinImage2;
        } else {
            img = (r->direction >= 0.0f) ? robinImage : robinImage2;
            if (!img.tex) img = (r->direction >= 0.0f) ? robinImage2 : robinImage;
        }
        if (img.tex) {
            bool grounded = (r->state == ROBIN_HOP || r->state == ROBIN_PAUSE);
            float bounce = (r->state == ROBIN_HOP) ? sinf(r->hopProgress * 3.14159265f) * ROBIN_HOP_HEIGHT : 0.0f;
            float drawX = r->x;
            float drawY = grounded ? (ROBIN_Y - bounce) : r->y;
            float imgW = img.subtex->width;
            float imgH = img.subtex->height;
            float topX = drawX - imgW * 0.5f;
            float topY = drawY - imgH;
            float alpha = (inSleepSequence || dayFactor <= 0.0f) ? 1.0f : dayFactor;
            C2D_ImageTint tint;
            C2D_AlphaImageTint(&tint, alpha);
            C2D_DrawImageAt(img, topX, topY, 0.15f, &tint, 1.0f, 1.0f);
        }
    }
    if (nestImage.tex) {
        float nw = nestImage.subtex->width, nh = nestImage.subtex->height;
        C2D_DrawImageAt(nestImage, ROBIN_NEST_X - nw * 0.5f, ROBIN_NEST_Y - nh, 0.15f, NULL, 1.0f, 1.0f);
    }
}
#define GREYBIRD_MIN_PERCH_DIST 12.0f
#define GREYBIRD_MIN_GROUND_DIST 14.0f
static void updateAndDrawForestGreyBird(float t) {
    ForestGreyBird* g = &forestGreyBird;
    bool inSleepSequence = (g->state == GREYBIRD_SLEEP_FLY1 || g->state == GREYBIRD_SLEEP_FLY2 ||
                             g->state == GREYBIRD_SLEEPING   || g->state == GREYBIRD_WAKE_FLY1  ||
                             g->state == GREYBIRD_WAKE_FLY2  || g->state == GREYBIRD_NESTVISIT_FLY_PERCH);
    time_t rawGreyBird = time(NULL);
    struct tm* tmGreyBird = localtime(&rawGreyBird);
    float hourFracGreyBird = tmGreyBird->tm_hour + tmGreyBird->tm_min / 60.0f;
    bool duskFirstHourGB = (currentForestPhase == FOREST_PHASE_DUSK) && (hourFracGreyBird < FOREST_DUSK_START + 1.0f);
    static int wasHourSlotGB = -1;
    bool hourJustBecameSleepHourGB = (tmGreyBird->tm_hour == (int)(FOREST_DUSK_START + 1.0f) && wasHourSlotGB != tmGreyBird->tm_hour);
    wasHourSlotGB = tmGreyBird->tm_hour;
    static bool wasAwakePhaseGB = false;
    bool isAwakePhaseGB = (currentForestPhase == FOREST_PHASE_DAWN || currentForestPhase == FOREST_PHASE_DAY || duskFirstHourGB);
    if (hourJustBecameSleepHourGB && !inSleepSequence) {
        g->flyStartX = g->x;  g->flyStartY = g->y;
        g->flyTargetX = GREYBIRD_NEST_X;  g->flyTargetY = GREYBIRD_NEST_Y;
        g->flyProgress = 0.0f;
        g->state = GREYBIRD_SLEEP_FLY1;
        g->isDayVisit = false;
        inSleepSequence = true;
    }
    if (isAwakePhaseGB && !wasAwakePhaseGB && g->state == GREYBIRD_SLEEPING) {
        g->flyStartX = g->x;  g->flyStartY = g->y;
        g->flyTargetX = GREYBIRD_NEST_X;  g->flyTargetY = GREYBIRD_NEST_Y;
        g->flyProgress = 0.0f;
        g->state = GREYBIRD_WAKE_FLY1;
        inSleepSequence = true;
    }
    wasAwakePhaseGB = isAwakePhaseGB;
    float dayFactor = getForestPhaseFactor(FOREST_PHASE_DAY);
    bool groundLikeActiveGB = (dayFactor > 0.0f) || isAwakePhaseGB;
    bool skipGreyBirdDraw = false;
    if (greyBirdNestVisitTimer < 0) greyBirdNestVisitTimer = 17700 + rand() % 600;
    bool isDayPhaseStrictGB = (currentForestPhase == FOREST_PHASE_DAY);
    if (isDayPhaseStrictGB && (g->state == GREYBIRD_HOP || g->state == GREYBIRD_PAUSE)) {
        greyBirdNestVisitTimer--;
        if (greyBirdNestVisitTimer <= 0) {
            int perchIdx = rand() % getActivePerchCount();
            g->flyStartX = g->x;  g->flyStartY = g->y;
            g->flyTargetX = getActivePerchX(perchIdx);
            g->flyTargetY = getActivePerchY(perchIdx);
            g->flyProgress = 0.0f;
            g->state = GREYBIRD_NESTVISIT_FLY_PERCH;
            g->isDayVisit = true;
            g->currentPerchIndex = perchIdx;
            inSleepSequence = true;
            greyBirdNestVisitTimer = 17700 + rand() % 600;
        }
    }
    if (inSleepSequence) {
        if (g->state == GREYBIRD_SLEEP_FLY1 || g->state == GREYBIRD_SLEEP_FLY2 ||
            g->state == GREYBIRD_WAKE_FLY1  || g->state == GREYBIRD_WAKE_FLY2  ||
            g->state == GREYBIRD_NESTVISIT_FLY_PERCH) {
            float ddx = g->flyTargetX - g->flyStartX, ddy = g->flyTargetY - g->flyStartY;
            float dist = sqrtf(ddx * ddx + ddy * ddy);
            if (dist < 1.0f) dist = 1.0f;
            g->flyProgress += ROBIN_FLY_SPEED / dist;
            if (g->flyProgress >= 1.0f) {
                g->x = g->flyTargetX;
                g->y = g->flyTargetY;
                if (g->state == GREYBIRD_NESTVISIT_FLY_PERCH) {
                    g->flyStartX = g->x;  g->flyStartY = g->y;
                    g->flyTargetX = GREYBIRD_NEST_X;  g->flyTargetY = GREYBIRD_NEST_Y;
                    g->flyProgress = 0.0f;
                    g->state = GREYBIRD_SLEEP_FLY1;
                } else if (g->state == GREYBIRD_SLEEP_FLY1) {
                    g->flyStartX = g->x;  g->flyStartY = g->y;
                    g->flyTargetX = GREYBIRD_SLEEP_POSE_X;  g->flyTargetY = GREYBIRD_SLEEP_POSE_Y;
                    g->flyProgress = 0.0f;
                    g->state = GREYBIRD_SLEEP_FLY2;
                } else if (g->state == GREYBIRD_SLEEP_FLY2) {
                    g->state = GREYBIRD_SLEEPING;
                    g->sleepPoseTimer = 0.3f + (rand() % 30) / 100.0f;
                    if (g->isDayVisit) g->dayVisitTimer = 10.0f;
                } else if (g->state == GREYBIRD_WAKE_FLY1) {
                    g->flyStartX = g->x;  g->flyStartY = g->y;
                    g->flyTargetX = ROBIN_MIN_X + rand() % (int)(ROBIN_MAX_X - ROBIN_MIN_X);
                    g->flyTargetY = ROBIN_Y;
                    g->flyProgress = 0.0f;
                    g->state = GREYBIRD_WAKE_FLY2;
                } else {
                    g->state = GREYBIRD_PAUSE;
                    g->pausePhase = GREYBIRD_PAUSE_ALERT;
                    g->pausePhaseTimer = 0.4f + (rand() % 40) / 100.0f;
                    g->zoneTimer = 0.0f;
                    g->zoneChangeDeadline = 4.0f + (rand() % 200) / 100.0f;
                    g->isDayVisit = false;
                }
            } else {
                g->x = g->flyStartX + ddx * g->flyProgress;
                g->y = g->flyStartY + ddy * g->flyProgress;
            }
        } else {
            if (g->sleepPoseTimer > 0.0f) g->sleepPoseTimer -= 0.0166f;
            if (g->isDayVisit) {
                g->dayVisitTimer -= 0.0166f;
                if (g->dayVisitTimer <= 0.0f) {
                    g->flyStartX = g->x;  g->flyStartY = g->y;
                    g->flyTargetX = GREYBIRD_NEST_X;  g->flyTargetY = GREYBIRD_NEST_Y;
                    g->flyProgress = 0.0f;
                    g->state = GREYBIRD_WAKE_FLY1;
                }
            }
        }
    } else if (!groundLikeActiveGB) {
        if (currentForestPhase == FOREST_PHASE_NIGHT || currentForestPhase == FOREST_PHASE_DUSK) {
            g->x = GREYBIRD_SLEEP_POSE_X;
            g->y = GREYBIRD_SLEEP_POSE_Y;
            g->state = GREYBIRD_SLEEPING;
            g->sleepPoseTimer = 0.0f;
            g->isDayVisit = false;
        } else {
            skipGreyBirdDraw = true;
        }
    } else {
    bool grounded = (g->state == GREYBIRD_HOP || g->state == GREYBIRD_PAUSE);
    if (greyBirdAutoFlyTimer < 0) greyBirdAutoFlyTimer = 900 + rand() % 1800;
    bool wantsFlyToPerch = false;
    if (grounded) {
        greyBirdAutoFlyTimer--;
        if (greyBirdAutoFlyTimer <= 0) wantsFlyToPerch = true;
    }
    if (wantsFlyToPerch && grounded) {
        int perchIdx = -1;
        for (int attempt = 0; attempt < 6; attempt++) {
            int candidate = rand() % getActivePerchCount();
            float pdx = getActivePerchX(candidate) - forestRobin.x;
            float pdy = getActivePerchY(candidate) - forestRobin.y;
            float pdist = sqrtf(pdx * pdx + pdy * pdy);
            if (pdist >= GREYBIRD_MIN_PERCH_DIST) { perchIdx = candidate; break; }
        }
        if (perchIdx >= 0) {
            g->flyStartX = g->x;  g->flyStartY = g->y;
            g->flyTargetX = getActivePerchX(perchIdx);
            g->flyTargetY = getActivePerchY(perchIdx);
            g->flyProgress = 0.0f;
            g->state = GREYBIRD_FLY_UP;
            g->currentPerchIndex = perchIdx;
            g->zoneTimer = 0.0f;
            g->zoneChangeDeadline = 4.0f + (rand() % 200) / 100.0f;
        }
        greyBirdAutoFlyTimer = 900 + rand() % 1800;
    }
    if (g->state == GREYBIRD_PAUSE) {
        g->zoneTimer += 0.0166f;
        g->pausePhaseTimer -= 0.0166f;
        if (g->pausePhaseTimer <= 0.0f) {
            if (g->pausePhase == GREYBIRD_PAUSE_ALERT) {
                bool forceZoneChange = (g->zoneTimer >= g->zoneChangeDeadline);
                if (!forceZoneChange && rand() % 100 < 55) {
                    g->pausePhase = GREYBIRD_PAUSE_PECK;
                    g->pausePhaseTimer = 0.15f + (rand() % 10) / 100.0f;
                } else {
                    if (rand() % 100 < 35) g->direction = -g->direction;
                    float dist = forceZoneChange ? (20.0f + rand() % 16) : (5.0f + rand() % 5);
                    float targetX = g->x + g->direction * dist;
                    if (targetX >= ROBIN_MAX_X) { targetX = ROBIN_MAX_X; g->direction = -1.0f; }
                    else if (targetX <= ROBIN_MIN_X) { targetX = ROBIN_MIN_X; g->direction = 1.0f; }
                    bool robinIsGrounded = (forestRobin.state == ROBIN_HOP || forestRobin.state == ROBIN_PAUSE);
                    if (robinIsGrounded && fabsf(targetX - forestRobin.x) < GREYBIRD_MIN_GROUND_DIST) {
                        g->direction = -g->direction;
                        float retryX = g->x + g->direction * dist;
                        if (retryX >= ROBIN_MAX_X) retryX = ROBIN_MAX_X;
                        else if (retryX <= ROBIN_MIN_X) retryX = ROBIN_MIN_X;
                        if (fabsf(retryX - forestRobin.x) < GREYBIRD_MIN_GROUND_DIST) {
                            targetX = g->x;
                        } else {
                            targetX = retryX;
                        }
                    }
                    if (fabsf(targetX - g->x) < 2.0f) {
                        g->pausePhase = GREYBIRD_PAUSE_ALERT;
                        g->pausePhaseTimer = 0.3f + (rand() % 30) / 100.0f;
                    } else {
                        g->hopStartX  = g->x;
                        g->hopTargetX = targetX;
                        g->hopProgress = 0.0f;
                        g->hopDuration = forceZoneChange ? 0.12f : ROBIN_HOP_DURATION;
                        g->state = GREYBIRD_HOP;
                    }
                }
            } else {
                g->pausePhase = GREYBIRD_PAUSE_ALERT;
                g->pausePhaseTimer = 0.3f + (rand() % 30) / 100.0f;
            }
        }
    } else if (g->state == GREYBIRD_HOP) {
        g->hopProgress += 0.0166f / g->hopDuration;
        if (g->hopProgress >= 1.0f) {
            g->x = g->hopTargetX;
            g->state = GREYBIRD_PAUSE;
            g->pausePhase = GREYBIRD_PAUSE_ALERT;
            g->pausePhaseTimer = 0.4f + (rand() % 40) / 100.0f;
            g->zoneTimer = 0.0f;
            g->zoneChangeDeadline = 4.0f + (rand() % 200) / 100.0f;
        } else {
            g->x = g->hopStartX + (g->hopTargetX - g->hopStartX) * g->hopProgress;
        }
    } else if (g->state == GREYBIRD_FLY_UP || g->state == GREYBIRD_FLY_DOWN) {
        float ddx = g->flyTargetX - g->flyStartX, ddy = g->flyTargetY - g->flyStartY;
        float dist = sqrtf(ddx * ddx + ddy * ddy);
        if (dist < 1.0f) dist = 1.0f;
        g->flyProgress += ROBIN_FLY_SPEED / dist;
        if (g->flyProgress >= 1.0f) {
            g->x = g->flyTargetX;
            g->y = g->flyTargetY;
            if (g->state == GREYBIRD_FLY_UP) {
                g->state = GREYBIRD_PERCHED;
                g->perchTimer = 2.0f + (rand() % 400) / 100.0f;
                g->perchLookTimer = 0.7f + (rand() % 80) / 100.0f;
                g->perchLookRight = (rand() % 2 == 0);
            } else {
                g->state = GREYBIRD_PAUSE;
                g->pausePhase = GREYBIRD_PAUSE_ALERT;
                g->pausePhaseTimer = 0.4f + (rand() % 40) / 100.0f;
            }
        } else {
            g->x = g->flyStartX + ddx * g->flyProgress;
            g->y = g->flyStartY + ddy * g->flyProgress;
        }
    } else {
        g->perchTimer -= 0.0166f;
        g->perchLookTimer -= 0.0166f;
        if (g->perchLookTimer <= 0.0f) {
            g->perchLookRight = !g->perchLookRight;
            g->perchLookTimer = 0.7f + (rand() % 80) / 100.0f;
        }
        if (g->perchTimer <= 0.0f) {
            g->flyStartX = g->x;  g->flyStartY = g->y;
            bool robinOnSamePerch = (forestRobin.state == ROBIN_PERCHED && forestRobin.currentPerchIndex == g->currentPerchIndex);
            if (getActivePerchCount() > 1 && rand() % 100 < 70) {
                int nextPerch, tries = 0;
                do {
                    nextPerch = rand() % getActivePerchCount();
                    tries++;
                } while ((nextPerch == g->currentPerchIndex || nextPerch == forestRobin.currentPerchIndex) && tries < 8);
                g->flyTargetX = getActivePerchX(nextPerch);
                g->flyTargetY = getActivePerchY(nextPerch);
                g->flyProgress = 0.0f;
                g->state = GREYBIRD_FLY_UP;
                g->currentPerchIndex = nextPerch;
            } else {
                float groundX = ROBIN_MIN_X + rand() % (int)(ROBIN_MAX_X - ROBIN_MIN_X);
                bool robinIsGrounded = (forestRobin.state == ROBIN_HOP || forestRobin.state == ROBIN_PAUSE);
                if (robinIsGrounded && fabsf(groundX - forestRobin.x) < GREYBIRD_MIN_GROUND_DIST) {
                    groundX = (groundX < forestRobin.x) ? ROBIN_MIN_X : ROBIN_MAX_X;
                }
                g->flyTargetX = groundX;
                g->flyTargetY = ROBIN_Y;
                g->flyProgress = 0.0f;
                g->state = GREYBIRD_FLY_DOWN;
                g->currentPerchIndex = -1;
            }
            (void)robinOnSamePerch;
        }
    }
    }
    if (!skipGreyBirdDraw) {
        C2D_Image img;
        bool isFlying = (g->state == GREYBIRD_FLY_UP || g->state == GREYBIRD_FLY_DOWN ||
                          g->state == GREYBIRD_SLEEP_FLY1 || g->state == GREYBIRD_SLEEP_FLY2 ||
                          g->state == GREYBIRD_WAKE_FLY1  || g->state == GREYBIRD_WAKE_FLY2  ||
                          g->state == GREYBIRD_NESTVISIT_FLY_PERCH);
        if (isFlying) {
            bool flyingLeft = (g->flyTargetX < g->flyStartX);
            bool wingDown = sinf(t * 25.0f) > 0.0f;
            if (flyingLeft) {
                img = wingDown ? greyBirdFlyImageSxB : greyBirdFlyImageSx;
                if (!img.tex) img = wingDown ? greyBirdFlyImageSx : greyBirdFlyImageSxB;
            } else {
                img = wingDown ? greyBirdFlyImageDxB : greyBirdFlyImageDx;
                if (!img.tex) img = wingDown ? greyBirdFlyImageDx : greyBirdFlyImageDxB;
            }
            if (!img.tex) img = flyingLeft ? greyBirdFlyImageSx : greyBirdFlyImageDx;
            if (!img.tex) img = flyingLeft ? greyBirdFlyImageDx : greyBirdFlyImageSx;
        } else if (g->state == GREYBIRD_SLEEPING) {
            if (currentForestPhase == FOREST_PHASE_DUSK) {
                bool blinkToSleepPose = (fmodf(t, 4.0f) < 0.2f);
                if (blinkToSleepPose) {
                    img = greyBirdSleepImage;
                    if (!img.tex) img = (g->direction >= 0.0f) ? greyBirdImageDx : greyBirdImageSx;
                } else {
                    img = (g->direction >= 0.0f) ? greyBirdImageDx : greyBirdImageSx;
                    if (!img.tex) img = (g->direction >= 0.0f) ? greyBirdImageSx : greyBirdImageDx;
                }
            } else if (g->sleepPoseTimer > 0.0f) {
                img = (g->direction >= 0.0f) ? greyBirdImageDx : greyBirdImageSx;
                if (!img.tex) img = (g->direction >= 0.0f) ? greyBirdImageSx : greyBirdImageDx;
            } else {
                img = greyBirdSleepImage;
                if (!img.tex) img = (g->direction >= 0.0f) ? greyBirdImageDx : greyBirdImageSx;
            }
        } else if (g->state == GREYBIRD_PERCHED) {
            img = g->perchLookRight ? greyBirdImageDx : greyBirdImageSx;
            if (!img.tex) img = g->perchLookRight ? greyBirdImageSx : greyBirdImageDx;
        } else if (g->state == GREYBIRD_PAUSE && g->pausePhase == GREYBIRD_PAUSE_PECK) {
            img = (g->direction >= 0.0f) ? greyBirdPeckDx : greyBirdPeckSx;
            if (!img.tex) img = (g->direction >= 0.0f) ? greyBirdImageDx : greyBirdImageSx;
        } else {
            img = (g->direction >= 0.0f) ? greyBirdImageDx : greyBirdImageSx;
            if (!img.tex) img = (g->direction >= 0.0f) ? greyBirdImageSx : greyBirdImageDx;
        }
        if (img.tex) {
            bool groundedNow = (g->state == GREYBIRD_HOP || g->state == GREYBIRD_PAUSE);
            float bounce = (g->state == GREYBIRD_HOP) ? sinf(g->hopProgress * 3.14159265f) * ROBIN_HOP_HEIGHT : 0.0f;
            float drawX = g->x;
            float drawY = groundedNow ? (ROBIN_Y - bounce) : g->y;
            float imgW = img.subtex->width;
            float imgH = img.subtex->height;
            float topX = drawX - imgW * 0.5f;
            float topY = drawY - imgH;
            float alpha = (inSleepSequence || dayFactor <= 0.0f) ? 1.0f : dayFactor;
            C2D_ImageTint tint;
            C2D_AlphaImageTint(&tint, alpha);
            C2D_DrawImageAt(img, topX, topY, 0.15f, &tint, 1.0f, 1.0f);
        }
    }
    if (nestImage.tex) {
        float gnw = nestImage.subtex->width, gnh = nestImage.subtex->height;
        C2D_DrawImageAt(nestImage, GREYBIRD_NEST_X - gnw * 0.5f, GREYBIRD_NEST_Y - gnh, 0.15f, NULL, 1.0f, 1.0f);
    }
}
#define FOREST_BIRD_TWILIGHT_COUNT 2
static void updateAndDrawForestBirds(float t) {
    float dayFactor  = getForestPhaseFactor(FOREST_PHASE_DAY);
    float dawnFactor  = getForestPhaseFactor(FOREST_PHASE_DAWN);
    float duskFactor  = getForestPhaseFactor(FOREST_PHASE_DUSK);
    float twilightFactor = fmaxf(dawnFactor, duskFactor);
    float overallFactor  = fmaxf(dayFactor, twilightFactor);
    if (overallFactor <= 0.0f) return;
    for (int i = 0; i < FOREST_BIRD_COUNT; i++) {
        bool isTwilightBird = (i < FOREST_BIRD_TWILIGHT_COUNT);
        float factor = isTwilightBird ? overallFactor : dayFactor;
        if (factor <= 0.0f) continue;
        bool isSmall = (i >= FOREST_BIRD_NORMAL_COUNT);
        forestBirds[i].x += forestBirds[i].vx;
        if (forestBirds[i].x >  420.0f) { forestBirds[i].x = -20.0f; forestBirds[i].y = isSmall ? (56.0f + rand() % 34) : (10.0f + rand() % 60); }
        if (forestBirds[i].x < -20.0f)  { forestBirds[i].x =  420.0f; forestBirds[i].y = isSmall ? (56.0f + rand() % 34) : (10.0f + rand() % 60); }
        float flap = sinf(t * forestBirds[i].flapSpeed + forestBirds[i].flapPhase);
        float sc = forestBirds[i].scale;
        float wingLift = flap * 3.0f * sc;
        float bx = forestBirds[i].x, by = forestBirds[i].y;
        u32 birdColor = C2D_Color32(45, 40, 35, (u8)(200.0f * factor));
        C2D_DrawLine(bx - 4.0f * sc, by + wingLift, birdColor, bx, by, birdColor, 1.0f, 0.02f);
        C2D_DrawLine(bx, by, birdColor, bx + 4.0f * sc, by + wingLift, birdColor, 1.0f, 0.02f);
    }
}
static void drawMoon(float t) {
    float nightFactor = getNightFactor();
    if (nightFactor <= 0.0f) return;
    time_t rawMoon = time(NULL);
    struct tm* tmMoon = localtime(&rawMoon);
    float hourFrac = tmMoon->tm_hour + tmMoon->tm_min / 60.0f;
    float span = (24.0f - MOON_RISE_HOUR) + MOON_SET_HOUR;
    float progress;
    if (hourFrac >= MOON_RISE_HOUR) progress = (hourFrac - MOON_RISE_HOUR) / span;
    else                            progress = (hourFrac + 24.0f - MOON_RISE_HOUR) / span;
    if (progress < 0.0f) progress = 0.0f;
    if (progress > 1.0f) progress = 1.0f;
    float moonX  = MOON_RISE_X + (MOON_SET_X - MOON_RISE_X) * progress;
    float u = (progress - 0.5f) * 2.0f;
    float moonY = MOON_PEAK_Y + (MOON_MOUNTAIN_Y - MOON_PEAK_Y) * (u * u);
    float pulse = 0.5f + 0.5f * sinf(t * 0.12f);
    u8 core = (u8)(225 + pulse * 20);
    C2D_DrawCircleSolid(moonX, moonY, 0.02f, 26.0f, C2D_Color32(200, 210, 230, (u8)(30 * nightFactor)));
    C2D_DrawCircleSolid(moonX, moonY, 0.02f, 18.0f, C2D_Color32(210, 218, 235, (u8)(55 * nightFactor)));
    C2D_DrawCircleSolid(moonX, moonY, 0.02f, 12.0f, C2D_Color32(220, 225, 240, (u8)(100 * nightFactor)));
    C2D_DrawCircleSolid(moonX, moonY, 0.02f, 8.0f,  C2D_Color32(core, 235, 245, (u8)(255 * nightFactor)));
    C2D_DrawCircleSolid(moonX - 2.5f, moonY + 2.0f, 0.02f, 2.2f, C2D_Color32(190, 198, 215, (u8)(90 * nightFactor)));
    C2D_DrawCircleSolid(moonX + 3.0f, moonY - 1.5f, 0.02f, 1.6f, C2D_Color32(190, 198, 215, (u8)(80 * nightFactor)));
}
static void drawForestStars(float t) {
    float nightFactor = getNightFactor();
    if (nightFactor <= 0.0f) return;
    for (int i = 0; i < FOREST_STAR_COUNT; i++) {
        float wave      = sinf(t * forestStars[i].pulseFreq + forestStars[i].phase);
        float blink     = 0.5f + 0.5f * wave;
        float intensity = forestStars[i].base * (0.3f + blink * forestStars[i].pulseAmp);
        float s         = forestStars[i].size * (0.6f + blink);
        u32 col = forestStars[i].color;
        u8 cr = (u8)(col & 0xFF), cg = (u8)((col >> 8) & 0xFF), cb = (u8)((col >> 16) & 0xFF);
        float lum = 0.55f + intensity * 0.45f;
        u8 r = (u8)(cr * lum), g = (u8)(cg * lum), b = (u8)(cb * lum);
        C2D_DrawRectSolid(forestStars[i].x, forestStars[i].y, 0.02f, s, s,
                           C2D_Color32(r, g, b, (u8)(220 * nightFactor)));
    }
}
static void updateAndDrawForestShoots(float t) {
    float nightFactor = getNightFactor();
    if (currentForestPhase == FOREST_PHASE_NIGHT && forestPhaseTransition <= 0.0f) {
        forestShootTimer--;
        if (forestShootTimer <= 0) {
            spawnForestShoot();
            forestShootTimer = 300 + rand() % 400;
        }
    }
    if (nightFactor <= 0.0f) return;
    for (int i = 0; i < FOREST_SHOOT_MAX; i++) {
        if (!forestShoots[i].active) continue;
        forestShoots[i].x    += forestShoots[i].vx;
        forestShoots[i].y    += forestShoots[i].vy;
        forestShoots[i].life -= 0.02f;
        if (forestShoots[i].life <= 0.0f || forestShoots[i].x < -50 || forestShoots[i].x > 450 || forestShoots[i].y > 150) {
            forestShoots[i].active = 0;
            continue;
        }
        float a = forestShoots[i].life * nightFactor;
        u32 colorHead = C2D_Color32(255, 255, 245, (u8)(230 * a));
        u32 colorTail = C2D_Color32(255, 255, 245, 0);
        C2D_DrawLine(forestShoots[i].x, forestShoots[i].y, colorHead,
                     forestShoots[i].x - forestShoots[i].vx * 6, forestShoots[i].y - forestShoots[i].vy * 6, colorTail,
                     1.6f, 0.02f);
    }
    (void)t;
}
static void updateAndDrawForestFireflies(float t) {
    float nightFactor = getNightFactor();
    if (nightFactor <= 0.0f) return;
    for (int i = 0; i < FOREST_FIREFLY_COUNT; i++) {
        forestFireflies[i].x += forestFireflies[i].vx;
        forestFireflies[i].y += forestFireflies[i].vy;
        if (forestFireflies[i].y < 120.0f && forestFireflies[i].vy < 0.0f) forestFireflies[i].vy = -forestFireflies[i].vy;
        if (forestFireflies[i].y > 235.0f && forestFireflies[i].vy > 0.0f) forestFireflies[i].vy = -forestFireflies[i].vy;
        if (forestFireflies[i].x < -15.0f)  forestFireflies[i].x = 415.0f;
        if (forestFireflies[i].x > 415.0f)  forestFireflies[i].x = -15.0f;
        float wobbleX = sinf(t * forestFireflies[i].freq1 * 0.5f + forestFireflies[i].phase1) * 7.0f;
        float wobbleY = cosf(t * forestFireflies[i].freq2 * 0.5f + forestFireflies[i].phase2) * 6.0f;
        float fx = forestFireflies[i].x + wobbleX;
        float fy = forestFireflies[i].y + wobbleY;
        float raw1  = 0.5f + 0.5f * sinf(t * forestFireflies[i].blinkSpeed + forestFireflies[i].blinkPhase);
        float raw2  = 0.5f + 0.5f * sinf(t * forestFireflies[i].blinkSpeed * 0.37f + forestFireflies[i].phase2);
        float blink = powf(raw1 * raw2, 0.6f);
        u8 glowA = (u8)(95.0f  * blink * nightFactor);
        u8 coreA = (u8)(255.0f * blink * nightFactor);
        C2D_DrawRectSolid(fx - 3.0f, fy - 3.0f, 0.15f, 6.0f, 6.0f, C2D_Color32(230, 210, 60, glowA));
        C2D_DrawRectSolid(fx - 1.0f, fy - 1.0f, 0.15f, 2.0f, 2.0f, C2D_Color32(255, 250, 150, coreA));
    }
}
static void updateAndDrawForestButterflies(float t) {
    static bool wasDayPhase = false;
    bool isDayPhase = (currentForestPhase == FOREST_PHASE_DAY);
    if (isDayPhase && !wasDayPhase) {
        resetForestButterflies();
    }
    wasDayPhase = isDayPhase;
    bool dayOrDusk = (currentForestPhase == FOREST_PHASE_DAY || currentForestPhase == FOREST_PHASE_DUSK);
    if (!dayOrDusk) return;
    for (int i = 0; i < FOREST_BUTTERFLY_COUNT; i++) {
        ForestButterfly* b = &forestButterflies[i];
        if (!b->active) continue;
        float fx, fy;
        float wingSpread;
        if (b->state == BUTTERFLY_PAUSED) {
            b->pauseTimer -= 0.0166f;
            fx = b->x;
            fy = b->y;
            wingSpread = 4.5f;
            if (b->pauseTimer <= 0.0f) {
                b->state = BUTTERFLY_FLYING;
                b->seekCheckTimer = 180 + rand() % 240;
            }
        } else if (b->state == BUTTERFLY_SEEKING) {
            float dx = b->targetX - b->x, dy = b->targetY - b->y;
            float dist = sqrtf(dx*dx + dy*dy);
            if (dist < 2.5f) {
                b->x = b->targetX;
                b->y = b->targetY;
                b->state = BUTTERFLY_PAUSED;
                b->pauseTimer = 2.0f + (rand() % 250) / 100.0f;
            } else {
                float speed = 0.22f;
                b->x += dx / dist * speed;
                b->y += dy / dist * speed;
                if (dx != 0.0f) b->vx = (dx >= 0.0f) ? fabsf(b->vx) : -fabsf(b->vx);
            }
            fx = b->x;
            fy = b->y;
            float flap = fabsf(sinf(t * b->flapSpeed + b->flapPhase));
            wingSpread = 1.5f + flap * 3.0f;
        } else {
            b->x += b->vx;
            b->y += b->vy;
            if (b->y < 120.0f && b->vy < 0.0f) b->vy = -b->vy;
            if (b->y > 235.0f && b->vy > 0.0f) b->vy = -b->vy;
            if (b->x < -15.0f || b->x > 415.0f) {
                if (isDayPhase) {
                    b->x = (b->x < -15.0f) ? 415.0f : -15.0f;
                } else {
                    b->active = false;
                    continue;
                }
            }
            b->seekCheckTimer--;
            if (b->seekCheckTimer <= 0) {
                if (rand() % 100 < 35) {
                    int p = rand() % FOREST_BUTTERFLY_PERCH_COUNT;
                    b->targetX = forestButterflyPerches[p][0];
                    b->targetY = forestButterflyPerches[p][1];
                    b->state = BUTTERFLY_SEEKING;
                } else {
                    b->seekCheckTimer = 180 + rand() % 240;
                }
            }
            float wobbleY = cosf(t * b->freq2 + b->phase2) * 8.0f;
            fx = b->x;
            fy = b->y + wobbleY;
            float flap = fabsf(sinf(t * b->flapSpeed + b->flapPhase));
            wingSpread = 1.5f + flap * 3.0f;
        }
        float lean = (b->vx >= 0.0f) ? 1.0f : -1.0f;
        u32 col = b->color;
        u8 cr = (u8)(col & 0xFF), cg = (u8)((col >> 8) & 0xFF), cb = (u8)((col >> 16) & 0xFF);
        u32 wingColor = C2D_Color32(cr, cg, cb, 230);
        C2D_DrawCircleSolid(fx, fy - wingSpread, 0.15f, 2.4f, wingColor);
        float bodyLeft = fx - 1.6f - (lean < 0.0f ? 2.0f : 0.0f);
        C2D_DrawRectSolid(bodyLeft, fy - 0.6f, 0.16f, 5.2f, 1.2f, C2D_Color32(35, 28, 22, 255));
    }
}
static void updateAndDrawForestHedgehogs(float t) {
    static int spawnTimer = -1;
    if (spawnTimer < 0) spawnTimer = (180 + rand() % 61) * 60;
    bool isDuskOrNight = (currentForestPhase == FOREST_PHASE_DUSK || currentForestPhase == FOREST_PHASE_NIGHT);
    if (isDuskOrNight && !forestHedgehogs[0].active) {
        spawnTimer--;
        if (spawnTimer <= 0) {
            resetForestHedgehogs();
            spawnTimer = (180 + rand() % 61) * 60;
        }
    }
    if (!isDuskOrNight) return;
    for (int i = 0; i < FOREST_HEDGEHOG_COUNT; i++) {
        ForestHedgehog* h = &forestHedgehogs[i];
        if (!h->active) continue;
        bool legsMoving = false;
        if (h->state == HEDGEHOG_PAUSED) {
            h->pauseTimer -= 0.0166f;
            if (h->pauseTimer <= 0.0f) {
                h->state = HEDGEHOG_WALKING;
                h->pauseCheckTimer = 90 + rand() % 120;
            }
        } else {
            h->pauseCheckTimer--;
            if (h->pauseCheckTimer <= 0) {
                if (rand() % 100 < 55) {
                    h->state = HEDGEHOG_PAUSED;
                    h->pauseTimer = 2.0f + (rand() % 300) / 100.0f;
                } else {
                    h->pauseCheckTimer = 90 + rand() % 120;
                }
            }
            if (h->state == HEDGEHOG_WALKING) {
                if (h->waypointIdx < 0 || h->waypointIdx >= FOREST_HEDGEHOG_WAYPOINT_COUNT) {
                    h->active = false;
                    continue;
                }
                float tx = forestHedgehogPath[h->waypointIdx][0];
                float ty = forestHedgehogPath[h->waypointIdx][1];
                float dx = tx - h->x, dy = ty - h->y;
                float dist = sqrtf(dx*dx + dy*dy);
                float speed = 0.15f;
                if (dist < speed) {
                    h->x = tx; h->y = ty;
                    h->waypointIdx += h->direction;
                } else {
                    h->x += dx / dist * speed;
                    h->y += dy / dist * speed;
                }
                legsMoving = true;
            }
        }
        float fx = h->x, fy = h->y;
        bool footAlt = legsMoving && (sinf(t * 7.0f + h->legPhase) > 0.0f);
        bool goingRight = (h->direction >= 0);
        C2D_Image img = footAlt
            ? (goingRight ? hedgehogImageDxB : hedgehogImageSxB)
            : (goingRight ? hedgehogImageDx  : hedgehogImageSx);
        if (!img.tex) {
            img = goingRight ? hedgehogImageDx : hedgehogImageSx;
            if (!img.tex) img = goingRight ? hedgehogImageSx : hedgehogImageDx;
        }
        if (!img.tex) continue;
        float hedgehogScale = 1.0f;
        float imgW = img.subtex->width  * hedgehogScale;
        float imgH = img.subtex->height * hedgehogScale;
        float drawX = fx - imgW * 0.5f;
        float drawY = fy - imgH;
        C2D_DrawImageAt(img, drawX, drawY, 0.15f, NULL, hedgehogScale, hedgehogScale);
    }
}
// Lepre: entra in schermo da un bordo scelto a caso, cammina per qualche
// secondo, si ferma nella posa "stand" rivolta nella direzione in cui
// camminava, poi a caso decide se proseguire nella stessa direzione o
// invertirla — sempre restando dentro lo schermo una volta entrata.
// Segue il profilo del terreno definito da questi waypoint (non punti di
// sosta, solo la linea da seguire), Y interpolata linearmente in base
// alla X corrente.
static const float hareWaypoints[6][2] = {
    {400.0f, 209.0f}, {344.0f, 221.0f}, {254.0f, 220.0f},
    {190.0f, 214.0f}, {100.0f, 210.0f}, {0.0f, 210.0f}
};
#define HARE_WAYPOINT_COUNT 6
static float getHareYForX(float x) {
    if (x >= hareWaypoints[0][0]) return hareWaypoints[0][1];
    if (x <= hareWaypoints[HARE_WAYPOINT_COUNT - 1][0]) return hareWaypoints[HARE_WAYPOINT_COUNT - 1][1];
    for (int i = 0; i < HARE_WAYPOINT_COUNT - 1; i++) {
        float x1 = hareWaypoints[i][0], x2 = hareWaypoints[i + 1][0];
        if (x <= x1 && x >= x2) {
            float y1 = hareWaypoints[i][1], y2 = hareWaypoints[i + 1][1];
            float frac = (x1 - x) / (x1 - x2);
            return y1 + (y2 - y1) * frac;
        }
    }
    return hareWaypoints[HARE_WAYPOINT_COUNT - 1][1];
}

typedef enum { HARE_WALKING, HARE_STANDING, HARE_EXITING, HARE_AWAY } HareState;
static HareState hareState = HARE_WALKING;
static float hareX = -50.0f;
static int hareDirection = 1; // 1 = verso destra (dx), -1 = verso sinistra (sx)
static float hareSpeed = 0.7f;
static float hareStateTimer = 0.0f;
static float hareStateDuration = 4.0f;
static bool hareHasEntered = false;
static bool hareInited = false;
static float hareAnimCycleDuration = 0.45f; // durata di un ciclo camminata completo (2 frame) — valore fissato, non più regolabile da Debug
static float hareGrazeCycleDuration = 0.8f; // durata di un ciclo brucatura completo (2 frame), più lenta della camminata
static float hareLifetimeTimer = 0.0f; // tempo trascorso in scena (Walking/Standing/Stop), azzerato ad ogni nuovo ingresso
static float hareAwayTimer = 0.0f;
#define HARE_LIFETIME_SECONDS 120.0f
#define HARE_AWAY_SECONDS 20.0f
// Due modalità di sosta, scelte a caso quando smette di camminare
// (30% sosta semplice, 70% con brucatura):
// Modalità 0 (semplice): fase 0 = stand posa1 (1s), fase 1 = stand
//   posa2 (1s), poi riparte.
// Modalità 1 (con brucatura): fase 0 = stand posa1 (1s, preparazione),
//   fase 1 = bruco, fase 2 = stand con la durata divisa a metà tra
//   posa1 e posa2, fase 3 = bruco di nuovo, poi riparte.
static int hareStopMode = 0;
static int hareStopPhase = 0;

static void hareStartPhase(int phase, float duration) {
    hareStopPhase = phase;
    hareStateTimer = 0.0f;
    hareStateDuration = duration;
}
static void hareResumeWalking() {
    bool nearLeftEdge  = hareX <= 40.0f;
    bool nearRightEdge = hareX >= 360.0f;
    if (nearLeftEdge)       hareDirection = 1;
    else if (nearRightEdge) hareDirection = -1;
    else                     hareDirection = (rand() % 2 == 0) ? hareDirection : -hareDirection; // 50/50 stessa direzione o opposta

    hareState = HARE_WALKING;
    hareStateTimer = 0.0f;
    hareStateDuration = 3.0f + (rand() % 301) / 100.0f;
}

static void initHare() {
    hareDirection = (rand() % 2 == 0) ? 1 : -1;
    hareX = (hareDirection == 1) ? -50.0f : 450.0f; // entra da sinistra se va a destra, o viceversa
    hareState = HARE_WALKING;
    hareStateTimer = 0.0f;
    hareStateDuration = 3.0f + (rand() % 301) / 100.0f; // 3.0-6.0s di cammino
    hareHasEntered = false;
    hareLifetimeTimer = 0.0f;
    hareInited = true;
}

static bool hareWasNightPhase = false;
static void updateAndDrawHare(float t) {
    if (!hareInited) initHare();

    bool isNightPhase = (currentForestPhase == FOREST_PHASE_NIGHT);
    if (isNightPhase && !hareWasNightPhase) {
        // Notte appena scattata: se è in scena, esce subito verso il
        // bordo più vicino, qualunque fosse la sua sotto-fase (cammina,
        // sosta, brucatura...).
        if (hareState == HARE_WALKING || hareState == HARE_STANDING) {
            hareDirection = (hareX < 200.0f) ? -1 : 1;
            hareState = HARE_EXITING;
        }
    }
    hareWasNightPhase = isNightPhase;

    if (hareState == HARE_AWAY) {
        if (isNightPhase) {
            // resta assente finché la fase non cambia — qualunque sia
            // quella di arrivo (Alba, ma anche Giorno o Tramonto se si
            // salta l'Alba con un cambio manuale/Cycle/Random), non solo
            // dopo i normali 20s del ciclo casuale.
            return;
        }
        hareAwayTimer += 0.0166f;
        if (hareAwayTimer >= HARE_AWAY_SECONDS) {
            initHare(); // ricompare da un bordo scelto a caso, come al primo avvio
        }
        return; // nulla da disegnare mentre è assente
    }

    hareStateTimer += 0.0166f;
    if (hareState == HARE_WALKING || hareState == HARE_STANDING) {
        hareLifetimeTimer += 0.0166f;
        if (hareLifetimeTimer >= HARE_LIFETIME_SECONDS) {
            // tempo scaduto: esce verso il bordo più vicino, ignorando il
            // normale "resta in schermo" fino a quando non è del tutto fuori
            hareDirection = (hareX < 200.0f) ? -1 : 1;
            hareState = HARE_EXITING;
        }
    }

    if (hareState == HARE_WALKING) {
        hareX += hareSpeed * (float)hareDirection;
        if (!hareHasEntered && hareX > 30.0f && hareX < 370.0f) hareHasEntered = true;

        bool nearLeftEdge  = hareHasEntered && hareX <= 25.0f;
        bool nearRightEdge = hareHasEntered && hareX >= 375.0f;

        if (hareStateTimer >= hareStateDuration || nearLeftEdge || nearRightEdge) {
            hareState = HARE_STANDING;
            hareStopMode = (rand() % 10 < 3) ? 0 : 1; // 30% sosta semplice, 70% con brucatura
            hareStartPhase(0, 1.0f); // entrambe le modalità iniziano con 1s di stand posa1
        }
    } else if (hareState == HARE_STANDING) {
        if (hareStateTimer >= hareStateDuration) {
            if (hareStopMode == 0) {
                // sosta semplice: stand posa1(1s) -> stand posa2(1s) -> riparte
                if (hareStopPhase == 0) {
                    hareStartPhase(1, 1.0f);
                } else {
                    hareResumeWalking();
                }
            } else {
                // sosta con brucatura: stand posa1(1s) -> stand posa2(1s) -> bruco -> stand diviso a metà -> bruco -> riparte
                if (hareStopPhase == 0) {
                    hareStartPhase(1, 1.0f); // stand posa2, 1s
                } else if (hareStopPhase == 1) {
                    hareStartPhase(2, 4.0f + (rand() % 301) / 100.0f); // bruco, 4.0-7.0s
                } else if (hareStopPhase == 2) {
                    hareStartPhase(3, 2.5f + (rand() % 151) / 100.0f); // stand diviso a metà, 2.5-4.0s totali
                } else if (hareStopPhase == 3) {
                    hareStartPhase(4, 4.0f + (rand() % 301) / 100.0f); // bruco di nuovo
                } else { // hareStopPhase == 4
                    hareResumeWalking();
                }
            }
        }
    } else if (hareState == HARE_EXITING) {
        hareX += hareSpeed * (float)hareDirection;
        if (hareX < -60.0f || hareX > 460.0f) {
            hareState = HARE_AWAY;
            hareAwayTimer = 0.0f;
        }
    }

    float hareY = getHareYForX(hareX);

    C2D_Image img;
    if (hareState == HARE_STANDING) {
        bool grazing = false;
        bool showPose2 = false;
        if (hareStopMode == 0) {
            showPose2 = (hareStopPhase == 1); // fase 1 = letteralmente la posa 2
        } else {
            if (hareStopPhase == 1) {
                showPose2 = true; // stand posa2 letterale
            } else if (hareStopPhase == 2 || hareStopPhase == 4) {
                grazing = true;
            } else if (hareStopPhase == 3) {
                showPose2 = (hareStateTimer < hareStateDuration * 0.5f); // invertito: prima posa2, poi posa1
            }
            // fase 0: posa1, showPose2 resta false
        }

        if (grazing) {
            bool grazeFrame = fmodf(t, hareGrazeCycleDuration) < hareGrazeCycleDuration * 0.5f;
            if (hareDirection == 1) img = grazeFrame ? hareGrazeDxImage : hareGrazeDx2Image;
            else                    img = grazeFrame ? hareGrazeSxImage : hareGrazeSx2Image;
        } else {
            if (hareDirection == 1) img = showPose2 ? hareStandDxBImage : hareStandDxImage;
            else                    img = showPose2 ? hareStandSxBImage : hareStandSxImage;
        }
    } else if (hareDirection == 1) {
        bool walkFrame = fmodf(t, hareAnimCycleDuration) < hareAnimCycleDuration * 0.5f;
        img = walkFrame ? hareDx1Image : hareDx2Image;
        if (!img.tex) img = walkFrame ? hareDx2Image : hareDx1Image;
    } else {
        bool walkFrame = fmodf(t, hareAnimCycleDuration) < hareAnimCycleDuration * 0.5f;
        img = walkFrame ? hareWalk1Image : hareWalk2Image;
        if (!img.tex) img = walkFrame ? hareWalk2Image : hareWalk1Image;
    }
    if (img.tex) {
        float imgW = img.subtex->width;
        float imgH = img.subtex->height;
        float drawX = hareX - imgW * 0.5f;
        float drawY = hareY - imgH;
        C2D_DrawImageAt(img, drawX, drawY, 0.15f, NULL, 1.0f, 1.0f); // stessa profondità del pettirosso: l'ordine di chiamata decide chi sta sopra, non il valore
    }
}
// Volpe: nascosta per la maggior parte del tempo. Solo di Notte, un
// timer casuale (in media 2 minuti, stesso principio del volo del gufo)
// la fa "uscire": attraversa lo schermo una volta sola, alternando la
// direzione ad ogni uscita (una volta da destra a sinistra, la
// successiva da sinistra a destra — mai un'inversione a metà
// attraversamento), poi torna nascosta in attesa del prossimo timer.
// A metà schermo si ferma per un secondo (posa foxsxstand, che guarda
// verso chi osserva) poi riprende la corsa. La direzione sx->dx riusa
// gli stessi sprite "sx" specchiati al volo (scala orizzontale
// negativa), nessun nuovo sprite necessario, incluso per la posa ferma.
// Velocità di spostamento e velocità di avvicendamento dei 5 fotogrammi
// sono due controlli indipendenti, entrambi regolabili da Debug.
typedef enum { FOX_HIDDEN, FOX_RUNNING, FOX_PAUSED } FoxState;
static FoxState foxState = FOX_HIDDEN;
static float foxX = 450.0f;
static bool foxGoingRight = false; // alterna ad ogni uscita
static bool foxHasPausedThisRun = false; // evita più soste nella stessa attraversata
static float foxPauseTimer = 0.0f;
#define FOX_PAUSE_DURATION 1.0f
#define FOX_PAUSE_X 200.0f // metà schermo (larghezza scena 400)
static float foxSpeed = 0.80f; // regolabile da Debug
static float foxAnimFrameDuration = 0.18f; // durata di ciascun fotogramma, regolabile da Debug
static int foxAppearTimer = -1; // in frame, conta solo di Notte
static bool debugFoxRunRequested = false;
static void updateAndDrawFox(float t) {
    if (foxState == FOX_HIDDEN) {
        if (foxAppearTimer < 0) foxAppearTimer = 5400 + rand() % 3600; // 90-150s, media 2 minuti
        if (debugFoxRunRequested) {
            debugFoxRunRequested = false;
            foxGoingRight = !foxGoingRight;
            foxX = foxGoingRight ? -100.0f : 450.0f;
            foxState = FOX_RUNNING;
            foxHasPausedThisRun = false;
            foxAppearTimer = -1;
            return;
        }
        if (currentForestPhase == FOREST_PHASE_NIGHT) {
            foxAppearTimer--;
            if (foxAppearTimer <= 0) {
                foxGoingRight = !foxGoingRight;
                foxX = foxGoingRight ? -100.0f : 450.0f;
                foxState = FOX_RUNNING;
                foxHasPausedThisRun = false;
                foxAppearTimer = -1;
            }
        }
        return; // nulla da disegnare mentre è nascosta
    }

    float foxY;
    C2D_Image img;

    if (foxState == FOX_PAUSED) {
        foxPauseTimer += 0.0166f;
        if (foxPauseTimer >= FOX_PAUSE_DURATION) {
            foxState = FOX_RUNNING;
        }
        foxY = getHareYForX(foxX);
        img = foxSxStandImage;
    } else {
        foxX += foxGoingRight ? foxSpeed : -foxSpeed;
        if (foxGoingRight ? (foxX > 460.0f) : (foxX < -50.0f)) {
            foxState = FOX_HIDDEN;
            return;
        }
        if (!foxHasPausedThisRun) {
            bool crossedMidpoint = foxGoingRight ? (foxX >= FOX_PAUSE_X) : (foxX <= FOX_PAUSE_X);
            if (crossedMidpoint) {
                foxHasPausedThisRun = true;
                foxState = FOX_PAUSED;
                foxPauseTimer = 0.0f;
            }
        }
        foxY = getHareYForX(foxX); // stessa linea di terra della lepre
        C2D_Image foxFrames[5] = { foxSx1Image, foxSx2Image, foxSx3Image, foxSx4Image, foxSx5Image };
        int frameIdx = (int)(t / foxAnimFrameDuration) % 5;
        if (frameIdx < 0) frameIdx += 5;
        img = foxFrames[frameIdx];
        if (!img.tex) img = foxSx1Image; // fallback minimo se quel fotogramma specifico non fosse caricato
    }

    if (img.tex) {
        float imgW = img.subtex->width;
        float imgH = img.subtex->height;
        float drawY = foxY - imgH;
        if (foxGoingRight) {
            // sprite "sx" specchiato: scala X negativa, il punto di
            // ancoraggio va spostato di conseguenza per restare centrato
            float drawX = foxX + imgW * 0.5f;
            C2D_DrawImageAt(img, drawX, drawY, 0.15f, NULL, -1.0f, 1.0f);
        } else {
            float drawX = foxX - imgW * 0.5f;
            C2D_DrawImageAt(img, drawX, drawY, 0.15f, NULL, 1.0f, 1.0f);
        }
    }
}
static void updateAndDrawForestLeaves(float t) {
    float spawnRate;
    switch (currentForestPhase) {
        case FOREST_PHASE_NIGHT: spawnRate = 0.0f; break;
        case FOREST_PHASE_DAWN:  spawnRate = 0.25f; break;
        case FOREST_PHASE_DUSK:  spawnRate = 0.25f; break;
        default:                 spawnRate = 0.6f; break;
    }
    // Moltiplicatore stagionale: Spring metà di Summer, Autumn il triplo,
    // Winter niente foglie che cadono (currentEnvSeason: 0=Spring,
    // 1=Summer, 2=Autumn, 3=Winter).
    if (currentEnvSeason == 0)      spawnRate *= 0.5f;
    else if (currentEnvSeason == 2) spawnRate *= 3.0f;
    else if (currentEnvSeason == 3) spawnRate = 0.0f;
    for (int i = 0; i < FOREST_LEAF_COUNT; i++) {
        if (!forestLeaves[i].active) {
            if (spawnRate <= 0.0f) continue;
            forestLeaves[i].respawnTimer--;
            if (forestLeaves[i].respawnTimer <= 0) {
                forestLeaves[i].x = rand() % 400;
                forestLeaves[i].y = 10.0f + rand() % 40;
                forestLeaves[i].vx = -0.30f + (rand() % 60) / 100.0f;
                forestLeaves[i].vy = 0.35f + (rand() % 20) / 100.0f;
                forestLeaves[i].swayPhase    = rand() % 360;
                forestLeaves[i].swaySpeed    = 1.0f + (rand() % 100) / 100.0f;
                forestLeaves[i].flutterPhase = rand() % 360;
                forestLeaves[i].color        = forestLeafColors[rand() % 3];
                forestLeaves[i].active       = true;
            }
            continue;
        }
        forestLeaves[i].x += forestLeaves[i].vx + sinf(t * forestLeaves[i].swaySpeed + forestLeaves[i].swayPhase) * 0.4f;
        forestLeaves[i].y += forestLeaves[i].vy;
        if (forestLeaves[i].y > 235.0f || forestLeaves[i].x < -10.0f || forestLeaves[i].x > 410.0f) {
            forestLeaves[i].active = false;
            float rateForWait = (spawnRate > 0.05f) ? spawnRate : 0.05f;
            forestLeaves[i].respawnTimer = (int)((120 + rand() % 480) / rateForWait);
            continue;
        }
        float flutter = sinf(t * 4.0f + forestLeaves[i].flutterPhase);
        float w = (2.0f + fabsf(flutter) * 1.5f) * 0.85f;
        u32 col = forestLeaves[i].color;
        u8 cr = (u8)(col & 0xFF), cg = (u8)((col >> 8) & 0xFF), cb = (u8)((col >> 16) & 0xFF);
        C2D_DrawRectSolid(forestLeaves[i].x - w * 0.5f, forestLeaves[i].y - 1.5f, 0.15f, w, 3.0f * 0.85f, C2D_Color32(cr, cg, cb, 220));
    }
}

/* -------------------- PIOGGIA -------------------- */
// Pool di gocce che cadono con una leggera inclinazione (non perfettamente
// verticali, più naturale). Disegnate sopra alla chioma (depth 0.15, come
// le foglie), non nascoste dietro al PNG — la pioggia si vede anche
// davanti agli alberi, non solo nei buchi. L'intensità (0.0-1.0) decide
// quante gocce del pool sono attive: per ora impostata a mano da Debug,
// in futuro collegata al meteo reale.
#define RAIN_POOL_MAX 90
typedef struct {
    float x, y;
    float speed;
    float length;
} RainDrop;
static RainDrop rainDrops[RAIN_POOL_MAX];
static float rainIntensity = 0.0f; // 0.0 = assente, 1.0 = massima

static void initRain() {
    for (int i = 0; i < RAIN_POOL_MAX; i++) {
        rainDrops[i].x = (float)(rand() % 400);
        rainDrops[i].y = (float)(rand() % 240);
        rainDrops[i].speed = 4.0f + (rand() % 100) / 50.0f; // 4.0-6.0 px/frame
        rainDrops[i].length = 6.0f + (rand() % 6); // 6-11px
    }
}

static void updateAndDrawRain(float t) {
    (void)t;
    if (rainIntensity <= 0.0f) return;

    int activeCount = (int)(rainIntensity * RAIN_POOL_MAX);
    if (activeCount < 1) activeCount = 1;

    u8 rainAlpha = (u8)(80.0f + rainIntensity * 100.0f); // range più ampio: Extreme ora è visibilmente più fitto/scuro del vecchio massimo
    u32 rainColor = C2D_Color32(190, 205, 225, rainAlpha);

    for (int i = 0; i < activeCount; i++) {
        RainDrop* d = &rainDrops[i];
        d->y += d->speed;
        d->x += d->speed * 0.18f; // leggera inclinazione verso destra, come un vento costante e leggero

        if (d->y > 245.0f || d->x > 410.0f) {
            d->x = -20.0f + rand() % 420;
            d->y = -10.0f - (float)(rand() % 60);
            d->speed = 4.0f + (rand() % 100) / 50.0f;
            d->length = 6.0f + (rand() % 6);
        }

        float dx = d->length * 0.32f; // stessa proporzione dell'inclinazione di caduta
        C2D_DrawLine(d->x, d->y, rainColor, d->x + dx, d->y + d->length, rainColor, 1.0f, 0.15f);
    }
}

/* -------------------- NEVE -------------------- */
// Pool di fiocchi che cadono lentamente con un'ondeggiata laterale dolce
// (non una linea retta come la pioggia). Disponibile solo quando la
// stagione è Winter. Stessa depth della pioggia (0.15, sopra alla chioma).
#define SNOW_POOL_MAX 100
typedef struct {
    float x, y;
    float speed;
    float size;
    float swayPhase;
    float swayAmount;
} SnowFlake;
static SnowFlake snowFlakes[SNOW_POOL_MAX];
static float snowIntensity = 0.0f; // 0.0 = assente, 1.0 = massima (Snow), 0.5 = Light Snow

static void initSnow() {
    for (int i = 0; i < SNOW_POOL_MAX; i++) {
        snowFlakes[i].x = (float)(rand() % 400);
        snowFlakes[i].y = (float)(rand() % 240);
        snowFlakes[i].speed = 1.0f + (rand() % 100) / 100.0f; // 1.0-2.0 px/frame, molto più lenta della pioggia
        snowFlakes[i].size = 1.0f + (rand() % 16) / 10.0f; // 1.0-2.5px, ridotti su richiesta (erano 1.5-3.5px)
        snowFlakes[i].swayPhase = (float)(rand() % 628) / 100.0f;
        snowFlakes[i].swayAmount = 0.4f + (rand() % 60) / 100.0f;
    }
}

static void updateAndDrawSnow(float t) {
    if (snowIntensity <= 0.0f) return;

    int activeCount = (int)(snowIntensity * SNOW_POOL_MAX);
    if (activeCount < 1) activeCount = 1;

    u8 snowAlpha = (u8)(160.0f + snowIntensity * 80.0f);
    if (snowAlpha > 255) snowAlpha = 255;
    u32 snowColor = C2D_Color32(255, 255, 255, snowAlpha);

    for (int i = 0; i < activeCount; i++) {
        SnowFlake* s = &snowFlakes[i];
        s->y += s->speed;
        s->x += sinf(t * 1.5f + s->swayPhase) * s->swayAmount * 0.3f; // ondeggiata laterale dolce, non caduta diagonale rettilinea

        if (s->y > 245.0f) {
            s->x = (float)(rand() % 400);
            s->y = -10.0f - (float)(rand() % 60);
            s->speed = 1.0f + (rand() % 100) / 100.0f;
            s->size = 1.0f + (rand() % 16) / 10.0f;
        }
        if (s->x < -10.0f) s->x = 410.0f;
        if (s->x > 410.0f) s->x = -10.0f;

        C2D_DrawCircleSolid(s->x, s->y, 0.15f, s->size, snowColor);
    }
}

/* -------------------- PIOGGIA/NEVE SCHERMO IN BASSO -------------------- */
// Stessa tecnica visiva di sopra, ma pool indipendenti dimensionati per
// i 320x240 dello schermo touch — condividere lo stesso pool tra due
// passate di disegno nello stesso frame raddoppierebbe la velocità di
// caduta, quindi stato completamente separato. Disegnata sotto a
// pulsanti/testo di ciascun menu (depth 0.05, sopra al solo sfondo).
#define BOTTOM_RAIN_POOL_MAX 40
static RainDrop bottomRainDrops[BOTTOM_RAIN_POOL_MAX];
static void initBottomRain() {
    for (int i = 0; i < BOTTOM_RAIN_POOL_MAX; i++) {
        bottomRainDrops[i].x = (float)(rand() % 320);
        bottomRainDrops[i].y = (float)(rand() % 240);
        bottomRainDrops[i].speed = 4.0f + (rand() % 100) / 50.0f;
        bottomRainDrops[i].length = 6.0f + (rand() % 6);
    }
}
static void updateAndDrawBottomRain() {
    if (rainIntensity <= 0.0f) return;
    int activeCount = (int)(rainIntensity * BOTTOM_RAIN_POOL_MAX);
    if (activeCount < 1) activeCount = 1;
    u8 rainAlpha = (u8)(80.0f + rainIntensity * 100.0f);
    u32 rainColor = C2D_Color32(190, 205, 225, rainAlpha);
    for (int i = 0; i < activeCount; i++) {
        RainDrop* d = &bottomRainDrops[i];
        d->y += d->speed;
        d->x += d->speed * 0.18f;
        if (d->y > 245.0f || d->x > 330.0f) {
            d->x = -20.0f + rand() % 340;
            d->y = -10.0f - (float)(rand() % 60);
            d->speed = 4.0f + (rand() % 100) / 50.0f;
            d->length = 6.0f + (rand() % 6);
        }
        float dx = d->length * 0.32f;
        C2D_DrawLine(d->x, d->y, rainColor, d->x + dx, d->y + d->length, rainColor, 1.0f, 0.05f);
    }
}
#define BOTTOM_SNOW_POOL_MAX 45
static SnowFlake bottomSnowFlakes[BOTTOM_SNOW_POOL_MAX];
static void initBottomSnow() {
    for (int i = 0; i < BOTTOM_SNOW_POOL_MAX; i++) {
        bottomSnowFlakes[i].x = (float)(rand() % 320);
        bottomSnowFlakes[i].y = (float)(rand() % 240);
        bottomSnowFlakes[i].speed = 1.0f + (rand() % 100) / 100.0f;
        bottomSnowFlakes[i].size = 1.0f + (rand() % 16) / 10.0f;
        bottomSnowFlakes[i].swayPhase = (float)(rand() % 628) / 100.0f;
        bottomSnowFlakes[i].swayAmount = 0.4f + (rand() % 60) / 100.0f;
    }
}
static void updateAndDrawBottomSnow(float t) {
    if (snowIntensity <= 0.0f) return;
    int activeCount = (int)(snowIntensity * BOTTOM_SNOW_POOL_MAX);
    if (activeCount < 1) activeCount = 1;
    u8 snowAlpha = (u8)(160.0f + snowIntensity * 80.0f);
    if (snowAlpha > 255) snowAlpha = 255;
    u32 snowColor = C2D_Color32(255, 255, 255, snowAlpha);
    for (int i = 0; i < activeCount; i++) {
        SnowFlake* s = &bottomSnowFlakes[i];
        s->y += s->speed;
        s->x += sinf(t * 1.5f + s->swayPhase) * s->swayAmount * 0.3f;
        if (s->y > 245.0f) {
            s->x = (float)(rand() % 320);
            s->y = -10.0f - (float)(rand() % 60);
            s->speed = 1.0f + (rand() % 100) / 100.0f;
            s->size = 1.0f + (rand() % 16) / 10.0f;
        }
        if (s->x < -10.0f) s->x = 330.0f;
        if (s->x > 330.0f) s->x = -10.0f;
        C2D_DrawCircleSolid(s->x, s->y, 0.05f, s->size, snowColor);
    }
}

/* -------------------- METEO UNIFICATO -------------------- */
// weatherType: 0=Clear, 1-4=Rain Light/Med/Heavy/Extreme, 5=Light Snow,
// 6=Snow, 7=Heavy Snow. Gli stati 5-7 (neve) sono selezionabili solo
// quando la stagione è Winter (currentEnvSeason==3) — se la stagione
// cambia mentre uno di questi è attivo, si torna a Clear invece di
// restare in uno stato non più coerente.
static u32 weatherType = 0;
static u32 realTimeWeatherType = 0; // risultato dell'ultimo fetch riuscito da internet, SEPARATO dal ciclo manuale — così tornare su Real Time dopo aver ciclato a mano ripristina sempre il meteo vero, non l'ultimo stato manuale toccato

static void updateWeatherFromType() {
    // envWeatherMode è la scelta persistita dell'utente (0-7 = stato
    // manuale fisso, 8 = Real Time). weatherType è il valore RISOLTO
    // effettivamente mostrato ogni frame: se in modalità manuale coincide
    // sempre con envWeatherMode; se in Real Time viene ripristinato da
    // realTimeWeatherType (l'ultimo fetch riuscito), non lasciato
    // semplicemente intoccato — altrimenti ciclare a mano tra gli stati e
    // tornare su Real Time restava bloccato sull'ultimo stato manuale
    // toccato invece di tornare al meteo vero.
    if (settings.envWeatherMode <= 7) weatherType = settings.envWeatherMode;
    else                              weatherType = realTimeWeatherType;

    // Niente più vincolo di stagione sulla neve: può cadere in qualunque
    // stagione (utile per location reali dove nevica anche fuori inverno,
    // es. alta quota) — sia scelta a mano che via meteo reale.
    if (weatherType >= 1 && weatherType <= 4) {
        rainIntensity = (float)weatherType / 4.0f;
        snowIntensity = 0.0f;
    } else if (weatherType == 5) {
        rainIntensity = 0.0f;
        snowIntensity = 0.35f;
    } else if (weatherType == 6) {
        rainIntensity = 0.0f;
        snowIntensity = 0.65f;
    } else if (weatherType == 7) {
        rainIntensity = 0.0f;
        snowIntensity = 1.0f;
    } else {
        rainIntensity = 0.0f;
        snowIntensity = 0.0f;
    }
}

// Cicla envWeatherMode allo stato "successivo": Clear + 4 gradi pioggia +
// 3 gradi neve (ora selezionabili in qualunque stagione) + Real Time per
// ultimo — 9 stati fissi, sempre nello stesso ordine.
static void cycleWeatherMode() {
    settings.envWeatherMode = (settings.envWeatherMode + 1) % 9;
    settingsDirty = true;
}

/* -------------------- METEO REALE (internet) -------------------- */
// Interroga Open-Meteo: prima il geocoding (nome città -> coordinate,
// solo se la location è cambiata rispetto all'ultima volta, altrimenti
// si riusano le coordinate già trovate), poi le previsioni vere e
// proprie con quelle coordinate. Il codice WMO ricevuto viene tradotto
// nei nostri 8 stati con mapWmoCodeToTestType — qualunque codice non
// previsto ricade su Clear, mai un valore indefinito.
static bool isWifiConnected() {
    u32 wifiStatus = 0;
    Result ret = ACU_GetWifiStatus(&wifiStatus);
    return (R_SUCCEEDED(ret) && wifiStatus != 0);
}

// Estrattore JSON minimale: cerca "chiave": e legge il numero subito
// dopo — non è un parser generico, funziona solo perché sappiamo che le
// risposte di Open-Meteo sono JSON piatto senza array/oggetti annidati
// nei campi che ci interessano.
static bool extractJsonNumber(const char* json, const char* key, double* outValue) {
    char search[32];
    snprintf(search, sizeof(search), "\"%s\":", key);
    const char* pos = strstr(json, search);
    if (!pos) return false;
    pos += strlen(search);
    *outValue = atof(pos);
    return true;
}

// Mappatura completa dei codici WMO (lo standard usato da Open-Meteo,
// 0-99 ordinati per gravità) sui nostri 8 stati meteo. Qualunque codice
// non esplicitamente elencato (inatteso, futuro, o un errore di rete che
// restituisce un valore strano) ricade in automatico su Clear — mai un
// crash o uno stato indefinito, solo un fallback prudente.
static int mapWmoCodeToTestType(int code) {
    switch (code) {
        case 0: case 1: case 2: case 3:      // sereno / nuvoloso
        case 45: case 48:                     // nebbia (nessun effetto dedicato per ora, ricade su Clear)
            return 0; // Clear
        case 51: case 53: case 55:           // pioggerella
        case 56: case 57:                     // pioggerella gelata
        case 61:                              // pioggia leggera
            return 1; // Rain Light
        case 63:                              // pioggia moderata
        case 80:                              // rovesci leggeri
            return 2; // Rain Med
        case 65:                              // pioggia forte
        case 66: case 67:                     // pioggia gelata
        case 81:                              // rovesci moderati
            return 3; // Rain Heavy
        case 82:                              // rovesci violenti
        case 95: case 96: case 99:           // temporali
            return 4; // Rain Extreme
        case 71: case 77: case 85:           // neve leggera / granelli / rovesci leggeri
            return 5; // Light Snow
        case 73:                              // neve moderata
            return 6; // Snow
        case 75: case 86:                     // neve forte / rovesci forti
            return 7; // Heavy Snow
        default:
            return 0; // qualunque codice non previsto: fallback prudente su Clear
    }
}

static const char* weatherStateLabel(int state) {
    static const char* labels[8] = {"Clear", "Rain Light", "Rain Med", "Rain Heavy", "Rain Extreme", "Light Snow", "Snow", "Heavy Snow"};
    if (state < 0 || state > 7) return "Clear";
    return labels[state];
}

static char weatherFetchStatus[48] = "Not attempted";
static double lastWeatherTemp = -999.0; // -999 = non ancora rilevata

// Cache delle coordinate: il geocoding viene rifatto solo se la location
// è cambiata rispetto all'ultima volta, non ad ogni singolo fetch.
static char geoCachedLocation[32] = "";
static double geoCachedLat = 0.0, geoCachedLon = 0.0;
static bool geoCacheValid = false;

static bool fetchWeatherFromInternet() {
    if (!settings.location[0]) { snprintf(weatherFetchStatus, sizeof(weatherFetchStatus), "No location set"); return false; }
    if (!isWifiConnected()) { snprintf(weatherFetchStatus, sizeof(weatherFetchStatus), "No WiFi connection"); return false; }

    char encodedLocation[64];
    int j = 0;
    for (int i = 0; settings.location[i] && j < (int)sizeof(encodedLocation) - 1; i++) {
        encodedLocation[j++] = (settings.location[i] == ' ') ? '+' : settings.location[i];
    }
    encodedLocation[j] = '\0';

    if (!geoCacheValid || strcmp(geoCachedLocation, settings.location) != 0) {
        char geoUrl[192];
        snprintf(geoUrl, sizeof(geoUrl), "http://geocoding-api.open-meteo.com/v1/search?name=%s&count=1", encodedLocation);

        httpcContext geoCtx;
        if (R_FAILED(httpcOpenContext(&geoCtx, HTTPC_METHOD_GET, geoUrl, 1))) { snprintf(weatherFetchStatus, sizeof(weatherFetchStatus), "Geocoding: open failed"); return false; }
        httpcSetKeepAlive(&geoCtx, HTTPC_KEEPALIVE_DISABLED);
        httpcAddRequestHeaderField(&geoCtx, "User-Agent", "curl/7.64.0");
        httpcAddRequestHeaderField(&geoCtx, "Connection", "close");
        if (R_FAILED(httpcBeginRequest(&geoCtx))) { httpcCloseContext(&geoCtx); snprintf(weatherFetchStatus, sizeof(weatherFetchStatus), "Geocoding: request failed"); return false; }
        u32 geoStatus = 0;
        httpcGetResponseStatusCode(&geoCtx, &geoStatus);
        if (geoStatus != 200) { httpcCloseContext(&geoCtx); snprintf(weatherFetchStatus, sizeof(weatherFetchStatus), "Geocoding: HTTP %u", (unsigned)geoStatus); return false; }
        char geoBuf[512]; memset(geoBuf, 0, sizeof(geoBuf));
        u32 geoRead = 0;
        httpcDownloadData(&geoCtx, (u8*)geoBuf, sizeof(geoBuf) - 1, &geoRead);
        httpcCloseContext(&geoCtx);
        if (geoRead == 0) { snprintf(weatherFetchStatus, sizeof(weatherFetchStatus), "Geocoding: empty response"); return false; }

        double lat, lon;
        if (!extractJsonNumber(geoBuf, "latitude", &lat) || !extractJsonNumber(geoBuf, "longitude", &lon)) {
            snprintf(weatherFetchStatus, sizeof(weatherFetchStatus), "Geocoding: city not found");
            return false;
        }
        geoCachedLat = lat;
        geoCachedLon = lon;
        strncpy(geoCachedLocation, settings.location, sizeof(geoCachedLocation) - 1);
        geoCachedLocation[sizeof(geoCachedLocation) - 1] = '\0';
        geoCacheValid = true;
    }

    char wxUrl[192];
    snprintf(wxUrl, sizeof(wxUrl), "http://api.open-meteo.com/v1/forecast?latitude=%.4f&longitude=%.4f&current_weather=true", geoCachedLat, geoCachedLon);

    httpcContext wxCtx;
    if (R_FAILED(httpcOpenContext(&wxCtx, HTTPC_METHOD_GET, wxUrl, 1))) { snprintf(weatherFetchStatus, sizeof(weatherFetchStatus), "Forecast: open failed"); return false; }
    httpcSetKeepAlive(&wxCtx, HTTPC_KEEPALIVE_DISABLED);
    httpcAddRequestHeaderField(&wxCtx, "User-Agent", "curl/7.64.0");
    httpcAddRequestHeaderField(&wxCtx, "Connection", "close");
    if (R_FAILED(httpcBeginRequest(&wxCtx))) { httpcCloseContext(&wxCtx); snprintf(weatherFetchStatus, sizeof(weatherFetchStatus), "Forecast: request failed"); return false; }
    u32 wxStatus = 0;
    httpcGetResponseStatusCode(&wxCtx, &wxStatus);
    if (wxStatus != 200) { httpcCloseContext(&wxCtx); snprintf(weatherFetchStatus, sizeof(weatherFetchStatus), "Forecast: HTTP %u", (unsigned)wxStatus); return false; }
    char wxBuf[512]; memset(wxBuf, 0, sizeof(wxBuf));
    u32 wxRead = 0;
    httpcDownloadData(&wxCtx, (u8*)wxBuf, sizeof(wxBuf) - 1, &wxRead);
    httpcCloseContext(&wxCtx);
    if (wxRead == 0) { snprintf(weatherFetchStatus, sizeof(weatherFetchStatus), "Forecast: empty response"); return false; }

    const char* cwPos = strstr(wxBuf, "\"current_weather\":");
    if (!cwPos) { snprintf(weatherFetchStatus, sizeof(weatherFetchStatus), "Forecast: current_weather not found"); return false; }

    double wmoCode;
    if (!extractJsonNumber(cwPos, "weathercode", &wmoCode)) {
        snprintf(weatherFetchStatus, sizeof(weatherFetchStatus), "Forecast: weathercode not found");
        return false;
    }
    double temp = 0.0;
    extractJsonNumber(cwPos, "temperature", &temp);

    realTimeWeatherType = mapWmoCodeToTestType((int)wmoCode);
    lastWeatherTemp = temp;
    snprintf(weatherFetchStatus, sizeof(weatherFetchStatus), "OK: %s, %.1fC", weatherStateLabel(realTimeWeatherType), temp);
    return true;
}

// Ricontrollo automatico del meteo reale, a intervallo scelto
// dall'utente (Never/10min/30min/1h) — chiamata una volta al frame dal
// loop principale, non fa nulla se la modalità è "Never".
static const u32 weatherScheduleMinutes[4] = {0, 10, 30, 60};
static float weatherScheduleTimer = 0.0f;
static void updateWeatherSchedule() {
    if (settings.weatherScheduleMode == 0) { weatherScheduleTimer = 0.0f; return; }
    weatherScheduleTimer += 0.0166f;
    float intervalSeconds = (float)weatherScheduleMinutes[settings.weatherScheduleMode % 4] * 60.0f;
    if (weatherScheduleTimer >= intervalSeconds) {
        weatherScheduleTimer -= intervalSeconds;
        fetchWeatherFromInternet();
    }
}



#define OWL_X 348.0f
#define OWL_Y 50.0f
#define OWL_FLY_TARGET_X 46.0f
#define OWL_FLY_TARGET_Y 37.0f
#define OWL_FLY_SPEED 0.9f
#define OWL_FLY_DIP 30.0f
typedef enum { OWL_STATE_NORMAL, OWL_STATE_FLYING } OwlFlightState;
static OwlFlightState owlFlightState = OWL_STATE_NORMAL;
static float owlX = OWL_X, owlY = OWL_Y;
static float owlFlightStartX = OWL_X, owlFlightStartY = OWL_Y;
static float owlFlightTargetX = OWL_FLY_TARGET_X, owlFlightTargetY = OWL_FLY_TARGET_Y;
static float owlFlightProgress = 0.0f;
static float owlFlightTotalDist = 1.0f;
static bool  owlAtHome = true;
static int   owlFlapCounter = 0;
static bool  debugOwlFlyRequested = false;
static int   owlLastMinuteSeen = -1;
static void updateAndDrawForestOwl(float t) {
    (void)t;
    float nightFactor = getNightFactor();
    if (nightFactor <= 0.0f) return;
    if (currentForestPhase == FOREST_PHASE_NIGHT && owlFlightState == OWL_STATE_NORMAL) {
        time_t rawOwl = time(NULL);
        struct tm* tmOwl = localtime(&rawOwl);
        if (tmOwl->tm_min != owlLastMinuteSeen) {
            owlLastMinuteSeen = tmOwl->tm_min;
            debugOwlFlyRequested = true;
        }
    }
    if (debugOwlFlyRequested && owlFlightState == OWL_STATE_NORMAL) {
        owlFlightStartX = owlX;
        owlFlightStartY = owlY;
        owlFlightTargetX = owlAtHome ? OWL_FLY_TARGET_X : OWL_X;
        owlFlightTargetY = owlAtHome ? OWL_FLY_TARGET_Y : OWL_Y;
        float ddx = owlFlightTargetX - owlX, ddy = owlFlightTargetY - owlY;
        owlFlightTotalDist = sqrtf(ddx * ddx + ddy * ddy);
        if (owlFlightTotalDist < 1.0f) owlFlightTotalDist = 1.0f;
        owlFlightProgress = 0.0f;
        owlFlapCounter = 0;
        owlFlightState = OWL_STATE_FLYING;
    }
    debugOwlFlyRequested = false;
    if (owlFlightState == OWL_STATE_FLYING) {
        owlFlightProgress += OWL_FLY_SPEED / owlFlightTotalDist;
        owlFlapCounter++;
        if (owlFlightProgress >= 1.0f) {
            owlX = owlFlightTargetX;
            owlY = owlFlightTargetY;
            owlAtHome = !owlAtHome;
            owlFlightState = OWL_STATE_NORMAL;
        } else {
            float lerpX = owlFlightStartX + (owlFlightTargetX - owlFlightStartX) * owlFlightProgress;
            float lerpY = owlFlightStartY + (owlFlightTargetY - owlFlightStartY) * owlFlightProgress;
            float dip = sinf(owlFlightProgress * 3.14159265f) * OWL_FLY_DIP;
            owlX = lerpX;
            owlY = lerpY + dip;
        }
        bool goingLeft = (owlFlightTargetX < owlFlightStartX);
        bool wingFlapDown = ((owlFlapCounter < 30) && ((owlFlapCounter % 15) < 8))
                          || ((owlFlapCounter >= 120) && (((owlFlapCounter - 120) % 15) < 8));
        C2D_Image flapImg = goingLeft ? owlImage3sxd : owlImage3dxd;
        C2D_Image flyImg;
        if (wingFlapDown && flapImg.tex) {
            flyImg = flapImg;
        } else {
            flyImg = goingLeft ? owlImage3sx : owlImage3dx;
            if (!flyImg.tex) flyImg = goingLeft ? owlImage3dx : owlImage3sx;
        }
        if (!flyImg.tex) return;
        float imgW = flyImg.subtex->width;
        float imgH = flyImg.subtex->height;
        C2D_ImageTint tint;
        C2D_AlphaImageTint(&tint, nightFactor);
        C2D_DrawImageAt(flyImg, owlX - imgW * 0.5f, owlY - imgH * 0.5f, 0.15f, &tint, 1.0f, 1.0f);
        return;
    }
    static int blinkFramesLeft = -1;
    static int blinkDurationLeft = 0;
    if (blinkFramesLeft < 0) blinkFramesLeft = 570 + rand() % 60;
    if (blinkDurationLeft > 0) {
        blinkDurationLeft--;
        if (blinkDurationLeft == 0) blinkFramesLeft = 570 + rand() % 60;
    } else if (blinkFramesLeft > 0) {
        blinkFramesLeft--;
        if (blinkFramesLeft == 0) blinkDurationLeft = 6;
    }
    bool eyesClosed = (blinkDurationLeft > 0);
    C2D_Image img = eyesClosed ? owlImage2 : owlImage1;
    if (!img.tex) img = eyesClosed ? owlImage1 : owlImage2;
    if (!img.tex) return;
    C2D_Image anchorImg = owlImage1.tex ? owlImage1 : img;
    float imgW = anchorImg.subtex->width;
    float imgH = anchorImg.subtex->height;
    float drawX = owlX - imgW * 0.5f;
    float drawY = owlY - imgH * 0.5f;
    C2D_ImageTint tint;
    C2D_AlphaImageTint(&tint, nightFactor);
    C2D_DrawImageAt(img, drawX, drawY, 0.15f, &tint, 1.0f, 1.0f);
}
static float currentTopR = 3.0f,  currentTopG = 10.0f, currentTopB = 22.0f;
static float currentBotR = 10.0f, currentBotG = 35.0f, currentBotB = 55.0f;
static float currentClockR = 255.0f, currentClockG = 255.0f, currentClockB = 255.0f;
static float currentThemeTextR = 95.0f, currentThemeTextG = 210.0f, currentThemeTextB = 230.0f;
static bool settingsDirty = false;
static bool alarmDirty    = false;
static MenuScreen currentScreen = SCREEN_MAIN;
// Zone tattili delle scritte di stato Alarm/Timer in Main, calcolate
// dinamicamente ad ogni frame di disegno (la posizione/larghezza dipende
// dal testo effettivo, es. formato 12h vs 24h, presenza di entrambe).
static bool  mainAlarmTextVisible = false;
static float mainAlarmTextX0, mainAlarmTextX1, mainAlarmTextY0, mainAlarmTextY1;
static bool  mainTimerTextVisible = false;
static float mainTimerTextX0, mainTimerTextX1, mainTimerTextY0, mainTimerTextY1;
static AppRecords records  = {0};
static AppSettings settings = {11, 1, 1, 0, 1, 0, 0.0f, 0.0f, 0, 1, 3, 2, 0, 1};
static AppAlarm    alarmCfg = {0, 0, 0};
static u32 sessionFrames = 0;
static u64 fpsWindowStart = 0;
static int fpsFrameCount = 0;
static float fpsDisplayValue = 0.0f;
static int batteryPercentApprox = 100; // 0/20/40/60/80/100, a scatti — via PTMU, il livello "ufficiale" (stesso usato dall'icona di sistema)
static int batteryPercentPrecise = 100; // 0-100 reale, via mcu::HWC — stessa fonte usata da 3DSident
static bool wifiConnectedCache = false;
static u8 wifiStrengthCache = 0; // 0-3, stesse tacche mostrate dall'HOME Menu
static u8 batteryCharging = 0;
static bool lowerScreenOff = false;
static s16* beepBuffer = NULL;
static u32  beepBufferSamples = 0;
static ndspWaveBuf beepWaveBuf;
static bool audioReady = false;
static u32  alarmEditHour   = 0;
static u32  alarmEditMinute = 0;
static bool alarmEditPM     = false;
static bool alarmRinging       = false;
static int  alarmLastTrigHour  = -1;
static int  alarmLastTrigMin   = -1;
static u32  alarmRepeatTimer   = 0;
static float alarmFlashPhase   = 0.0f;
static TimerState timerState        = TIMER_IDLE;
static u32 timerTotalSeconds        = 0;
static u32 timerRemainingSeconds    = 0;
static u32 timerFrameAccumulator    = 0;
static u32 timerEditHour   = 0;
static u32 timerEditMinute = 0;
static u32 timerEditSecond = 0;

// Sistema di ripetizione per i pulsanti +/- di Alarm e Timer: un tocco
// singolo applica un incremento subito, tenendo premuto parte a
// ripetere dopo un breve ritardo iniziale, poi più veloce.
typedef enum {
    HOLD_NONE = 0,
    HOLD_ALARM_HOUR_UP, HOLD_ALARM_HOUR_DOWN,
    HOLD_ALARM_MIN_UP,  HOLD_ALARM_MIN_DOWN,
    HOLD_TIMER_HOUR_UP, HOLD_TIMER_HOUR_DOWN,
    HOLD_TIMER_MIN_UP,  HOLD_TIMER_MIN_DOWN,
    HOLD_TIMER_SEC_UP,  HOLD_TIMER_SEC_DOWN,
    HOLD_CAL_MONTH_UP,  HOLD_CAL_MONTH_DOWN,
    HOLD_CAL_YEAR_UP,   HOLD_CAL_YEAR_DOWN
} HoldTarget;
static HoldTarget currentHoldTarget = HOLD_NONE;
static float holdTimer = 0.0f;
static bool  holdFirstTickDone = false;
#define HOLD_INITIAL_DELAY  0.4f
#define HOLD_REPEAT_INTERVAL 0.1f

static void applyHoldTargetIncrement(HoldTarget target) {
    bool is12h = !settings.timeFormat24h;
    switch (target) {
        case HOLD_ALARM_HOUR_UP:
            if (is12h) alarmEditHour = (alarmEditHour % 12) + 1;
            else       alarmEditHour = (alarmEditHour + 1) % 24;
            break;
        case HOLD_ALARM_HOUR_DOWN:
            if (is12h) alarmEditHour = (alarmEditHour == 1) ? 12 : alarmEditHour - 1;
            else       alarmEditHour = (alarmEditHour == 0) ? 23 : alarmEditHour - 1;
            break;
        case HOLD_ALARM_MIN_UP:   alarmEditMinute = (alarmEditMinute + 1) % 60; break;
        case HOLD_ALARM_MIN_DOWN: alarmEditMinute = (alarmEditMinute == 0) ? 59 : alarmEditMinute - 1; break;
        case HOLD_TIMER_HOUR_UP:   timerEditHour = (timerEditHour + 1) % 24; break;
        case HOLD_TIMER_HOUR_DOWN: timerEditHour = (timerEditHour == 0) ? 23 : timerEditHour - 1; break;
        case HOLD_TIMER_MIN_UP:   timerEditMinute = (timerEditMinute + 1) % 60; break;
        case HOLD_TIMER_MIN_DOWN: timerEditMinute = (timerEditMinute == 0) ? 59 : timerEditMinute - 1; break;
        case HOLD_TIMER_SEC_UP:   timerEditSecond = (timerEditSecond + 1) % 60; break;
        case HOLD_TIMER_SEC_DOWN: timerEditSecond = (timerEditSecond == 0) ? 59 : timerEditSecond - 1; break;
        case HOLD_CAL_MONTH_UP:   calEditMonth = (calEditMonth + 1) % 12; break;
        case HOLD_CAL_MONTH_DOWN: calEditMonth = (calEditMonth == 0) ? 11 : calEditMonth - 1; break;
        case HOLD_CAL_YEAR_UP:    if (calEditYear < 2099) calEditYear++; break;
        case HOLD_CAL_YEAR_DOWN:  if (calEditYear > 1970) calEditYear--; break;
        default: break;
    }
}
// Stesse coordinate di campo già usate nel disegno/touch di Alarm e
// Timer — i pulsanti +/- stanno appena sopra/sotto ciascun campo
// (che occupa y=80-150).
static HoldTarget getHoldTargetAtTouch(int px, int py) {
    if (currentScreen == SCREEN_ALARM) {
        bool is12h = !settings.timeFormat24h;
        float hourFieldX, minFieldX;
        if (is12h) {
            const float gap = 10.0f;
            const float totalW = 80.0f + gap + 80.0f + gap + 66.0f;
            const float startX = 15.0f + (290.0f - totalW) * 0.5f;
            hourFieldX = startX;
            minFieldX  = hourFieldX + 80.0f + gap;
        } else {
            hourFieldX = 82.0f;
            minFieldX  = 172.0f;
        }
        if (px >= hourFieldX && px <= hourFieldX + 80) {
            if (py >= 66 && py <= 82)   return HOLD_ALARM_HOUR_UP;
            if (py >= 151 && py <= 167) return HOLD_ALARM_HOUR_DOWN;
        }
        if (px >= minFieldX && px <= minFieldX + 80) {
            if (py >= 66 && py <= 82)   return HOLD_ALARM_MIN_UP;
            if (py >= 151 && py <= 167) return HOLD_ALARM_MIN_DOWN;
        }
    } else if (currentScreen == SCREEN_TIMER && timerState == TIMER_IDLE) {
        const float fieldW = 80.0f, gap = 10.0f;
        const float totalW = fieldW*3 + gap*2;
        const float startX = 15.0f + (290.0f - totalW) * 0.5f;
        float hourFieldX = startX;
        float minFieldX  = hourFieldX + fieldW + gap;
        float secFieldX  = minFieldX  + fieldW + gap;
        if (px >= hourFieldX && px <= hourFieldX + fieldW) {
            if (py >= 66 && py <= 82)   return HOLD_TIMER_HOUR_UP;
            if (py >= 151 && py <= 167) return HOLD_TIMER_HOUR_DOWN;
        }
        if (px >= minFieldX && px <= minFieldX + fieldW) {
            if (py >= 66 && py <= 82)   return HOLD_TIMER_MIN_UP;
            if (py >= 151 && py <= 167) return HOLD_TIMER_MIN_DOWN;
        }
        if (px >= secFieldX && px <= secFieldX + fieldW) {
            if (py >= 66 && py <= 82)   return HOLD_TIMER_SEC_UP;
            if (py >= 151 && py <= 167) return HOLD_TIMER_SEC_DOWN;
        }
    } else if (currentScreen == SCREEN_CALENDAR_MONTHYEAR) {
        const float fieldW = 100.0f, gap = 10.0f;
        const float totalW = fieldW*2 + gap;
        const float startX = 15.0f + (290.0f - totalW) * 0.5f;
        float monthFieldX = startX;
        float yearFieldX  = monthFieldX + fieldW + gap;
        if (px >= monthFieldX && px <= monthFieldX + fieldW) {
            if (py >= 60 && py <= 82)   return HOLD_CAL_MONTH_UP;
            if (py >= 136 && py <= 158) return HOLD_CAL_MONTH_DOWN;
        }
        if (px >= yearFieldX && px <= yearFieldX + fieldW) {
            if (py >= 60 && py <= 82)   return HOLD_CAL_YEAR_UP;
            if (py >= 136 && py <= 158) return HOLD_CAL_YEAR_DOWN;
        }
    }
    return HOLD_NONE;
}
static C2D_TextBuf staticBuf;
typedef struct {
    C2D_Text title, hintMove, hintCenter, hintSelect, hintSize, hintStart, hintOff, btnSettings, btnRecords, btnDebug, btnOff, btnEnvironment;
    C2D_Text btnBack;
    C2D_Text setTitle, btnBackSet, btnResetSet, btnCredits, btnAlarmSet, btnTimerSet;
    C2D_Text credTitle, credLine1, credLine2, credLine3, credLine4, btnBackCred;
    C2D_Text alarmTitle, alarmHourLabel, alarmMinLabel, btnAlarmBack, btnAlarmSetConfirm, btnAlarmClear;
    C2D_Text timerTitle, timerHourLabel, timerMinLabel, timerSecLabel, btnTimerBack;
    C2D_Text btnTimerStart, btnTimerPause, btnTimerResume, btnTimerStop, btnTimerReset;
} StaticTexts;
static StaticTexts ui;
static u32 clockPresets[12];
static const char* presetNames[13] = {
    "Birch Bark", "Morning Dew", "Golden Sunbeam", "Winter Frost",
    "Clear Sky", "Kingfisher Blue", "Fern Green", "Fox Ember",
    "Foxglove Bloom", "Heather Twilight", "Rowan Berry", "Forest Canopy",
    "Woodland Spectrum"
};
// Vero ciclo arcobaleno: tinta HSV a piena saturazione/luminosità, non
// più le pastello sfumate di prima (che non toccavano mai i colori
// pieni). Ciclo completo ogni 30 secondi circa (rallentato su richiesta).
static void getRainbowColor(float animTime, float* outR, float* outG, float* outB) {
    float hue = fmodf(animTime * 12.0f, 360.0f);
    float c = 1.0f;
    float x = c * (1.0f - fabsf(fmodf(hue / 60.0f, 2.0f) - 1.0f));
    float r1, g1, b1;
    if      (hue <  60.0f) { r1 = c; g1 = x; b1 = 0.0f; }
    else if (hue < 120.0f) { r1 = x; g1 = c; b1 = 0.0f; }
    else if (hue < 180.0f) { r1 = 0.0f; g1 = c; b1 = x; }
    else if (hue < 240.0f) { r1 = 0.0f; g1 = x; b1 = c; }
    else if (hue < 300.0f) { r1 = x; g1 = 0.0f; b1 = c; }
    else                    { r1 = c; g1 = 0.0f; b1 = x; }
    *outR = r1 * 255.0f;
    *outG = g1 * 255.0f;
    *outB = b1 * 255.0f;
}
static void initColorPresets() {
    clockPresets[0]  = C2D_Color32(255, 255, 255, 255); // Birch Bark
    clockPresets[1]  = C2D_Color32(128, 255, 212, 255); // Morning Dew
    clockPresets[2]  = C2D_Color32(255, 215,   0, 255); // Golden Sunbeam
    clockPresets[3]  = C2D_Color32(150, 190, 220, 255); // Winter Frost
    clockPresets[4]  = C2D_Color32(100, 180, 255, 255); // Clear Sky
    clockPresets[5]  = C2D_Color32(  0, 191, 255, 255); // Kingfisher Blue
    clockPresets[6]  = C2D_Color32( 50, 205,  50, 255); // Fern Green
    clockPresets[7]  = C2D_Color32(255,  69,   0, 255); // Fox Ember
    clockPresets[8]  = C2D_Color32(255,  20, 147, 255); // Foxglove Bloom
    clockPresets[9]  = C2D_Color32(153,  50, 204, 255); // Heather Twilight
    clockPresets[10] = C2D_Color32(230,  30,  30, 255); // Rowan Berry
    clockPresets[11] = C2D_Color32( 34, 120,  60, 255); // Forest Canopy
}
static void initStaticTexts() {
    staticBuf = C2D_TextBufNew(1024);
    #define PT(dst, str) C2D_TextParse(dst, staticBuf, str); C2D_TextOptimize(dst)
    PT(&ui.title,       "ForestClock");
    PT(&ui.hintMove,    "Press L + Analog to Move Clock");
    PT(&ui.hintCenter,  "Press L + A to Center Clock");
    PT(&ui.hintSelect,  "Press SELECT to Cycle Display Modes");
    PT(&ui.hintSize,    "Press D-Pad Left/Right to Resize Clock");
    PT(&ui.hintStart,   "Press START to Close Application");
    PT(&ui.hintOff,     "Tap 'Screen Off' to Disable the Lower Screen");
    PT(&ui.btnSettings, "Settings");
    PT(&ui.btnRecords,  "Manual");
    PT(&ui.btnDebug,    "Debug");
    PT(&ui.btnEnvironment, "Environment");
    PT(&ui.btnOff,      "Screen Off");
    PT(&ui.btnBack,     "Back");
    PT(&ui.setTitle,    "SETTINGS");
    PT(&ui.btnBackSet,  "Back");
    PT(&ui.btnResetSet, "Reset");
    PT(&ui.btnCredits,  "Credits");
    PT(&ui.btnAlarmSet, "Alarm");
    PT(&ui.btnTimerSet, "Timer");
    PT(&ui.credTitle,   "ForestClock Credits");
    PT(&ui.credLine1,   "Developed by: Michele P.");
    PT(&ui.credLine2,   "A Relaxing ForestClock App");
    PT(&ui.credLine3,   "Send feedbacks to Michelep3ds@gmail.com");
    PT(&ui.credLine4,   "Version 1.00");
    PT(&ui.btnBackCred, "Back");
    PT(&ui.alarmTitle,        "ALARM");
    PT(&ui.alarmHourLabel,    "Hour");
    PT(&ui.alarmMinLabel,     "Minute");
    PT(&ui.btnAlarmBack,      "Back");
    PT(&ui.btnAlarmSetConfirm,"SET");
    PT(&ui.btnAlarmClear,     "Clear");
    PT(&ui.timerTitle,      "TIMER");
    PT(&ui.timerHourLabel,  "Hour");
    PT(&ui.timerMinLabel,   "Minute");
    PT(&ui.timerSecLabel,   "Second");
    PT(&ui.btnTimerBack,    "Back");
    PT(&ui.btnTimerStart,   "Start");
    PT(&ui.btnTimerPause,   "Pause");
    PT(&ui.btnTimerResume,  "Resume");
    PT(&ui.btnTimerStop,    "Stop");
    PT(&ui.btnTimerReset,   "Reset");
    #undef PT
}
static void saveRecords() {
    FILE* f = fopen(SAVE_PATH_RECORDS, "wb");
    if (f) { fwrite(&records, sizeof(AppRecords), 1, f); fclose(f); }
}
static void loadRecords() {
    FILE* f = fopen(SAVE_PATH_RECORDS, "rb");
    if (f) { fread(&records, sizeof(AppRecords), 1, f); fclose(f); }
    else   { records = (AppRecords){0}; }
}
static void saveSettings() {
    FILE* f = fopen(SAVE_PATH_SETTINGS, "wb");
    if (f) { fwrite(&settings, sizeof(AppSettings), 1, f); fclose(f); }
    settingsDirty = false;
}
static void loadSettings() {
    FILE* f = fopen(SAVE_PATH_SETTINGS, "rb");
    if (f) { fread(&settings, sizeof(AppSettings), 1, f); fclose(f); }
    else   { settings = (AppSettings){11, 1, 1, 0, 1, 0, 0.0f, 0.0f, 0, 1, 3, 2, 0, 1}; }
}
// Preset Environment: un "colpo d'occhio" completo di stagione, fase,
// nuvole, sky mood, meteo, e setup orologio (size/posizione/colore) —
// salvabile/richiamabile su 4 slot, persistito su SD.
typedef struct {
    u32 used; // 0 = slot vuoto, mai salvato
    u32 envSeasonMode;
    u32 envPhaseMode;
    u32 envCloudSpeedLevel;
    u32 envCloudDirection;
    u32 bgThemeIndex;   // Sky Mood
    u32 envWeatherMode; // Weather status
    u32 clockSizePreset;
    u32 clockColorIndex;
    float clockOffsetX;
    float clockOffsetY;
} EnvPreset;
static EnvPreset envPresets[4];
static void saveEnvPresets() {
    FILE* f = fopen(SAVE_PATH_PRESETS, "wb");
    if (f) { fwrite(envPresets, sizeof(EnvPreset), 4, f); fclose(f); }
}
static void loadEnvPresets() {
    FILE* f = fopen(SAVE_PATH_PRESETS, "rb");
    if (f) { fread(envPresets, sizeof(EnvPreset), 4, f); fclose(f); }
    else   { memset(envPresets, 0, sizeof(envPresets)); }
}
// Reset totale: azzera tutte le impostazioni/record/sveglia/preset in
// memoria E cancella i file fisici dalla SD, così non resta nulla da
// ricaricare al prossimo avvio (un semplice azzeramento in memoria senza
// cancellare i file verrebbe sovrascritto dal salvataggio automatico a
// fine sessione, ma i file restanti sul disco confonderebbero comunque
// un controllo manuale della cartella).
static void performFullReset() {
    settings = (AppSettings){11, 1, 1, 0, 1, 0, 0.0f, 0.0f, 0, 1, 3, 2, 0, 1};
    records  = (AppRecords){0};
    alarmCfg = (AppAlarm){0, 0, 0};
    memset(envPresets, 0, sizeof(envPresets));
    memset(diaryEntries, 0, sizeof(diaryEntries));
    memset(notes, 0, sizeof(notes));
    remove(SAVE_PATH_SETTINGS);
    remove(SAVE_PATH_RECORDS);
    remove(SAVE_PATH_ALARM);
    remove(SAVE_PATH_PRESETS);
    remove(SAVE_PATH_DIARY);
    remove(SAVE_PATH_NOTES);
    settingsDirty = false;
    alarmDirty = false;
    activePresetSlot = -1;
}
static void saveCurrentIntoPreset(int slot) {
    if (slot < 0 || slot > 3) return;
    envPresets[slot].used               = 1;
    envPresets[slot].envSeasonMode      = settings.envSeasonMode;
    envPresets[slot].envPhaseMode       = settings.envPhaseMode;
    envPresets[slot].envCloudSpeedLevel = settings.envCloudSpeedLevel;
    envPresets[slot].envCloudDirection  = settings.envCloudDirection;
    envPresets[slot].bgThemeIndex       = settings.bgThemeIndex;
    envPresets[slot].envWeatherMode     = settings.envWeatherMode;
    envPresets[slot].clockSizePreset    = settings.clockSizePreset;
    envPresets[slot].clockColorIndex    = settings.clockColorIndex;
    envPresets[slot].clockOffsetX       = settings.clockOffsetX;
    envPresets[slot].clockOffsetY       = settings.clockOffsetY;
    saveEnvPresets();
    activePresetSlot = slot;
}
static void recallPreset(int slot) {
    if (slot < 0 || slot > 3 || !envPresets[slot].used) return;
    settings.envSeasonMode      = envPresets[slot].envSeasonMode;
    settings.envPhaseMode       = envPresets[slot].envPhaseMode;
    settings.envCloudSpeedLevel = envPresets[slot].envCloudSpeedLevel;
    settings.envCloudDirection  = envPresets[slot].envCloudDirection;
    settings.bgThemeIndex       = envPresets[slot].bgThemeIndex;
    settings.envWeatherMode     = envPresets[slot].envWeatherMode;
    settings.clockSizePreset    = envPresets[slot].clockSizePreset;
    settings.clockColorIndex    = envPresets[slot].clockColorIndex;
    settings.clockOffsetX       = envPresets[slot].clockOffsetX;
    settings.clockOffsetY       = envPresets[slot].clockOffsetY;
    settingsDirty = true;
    activePresetSlot = slot;
}
static void saveAlarm() {
    FILE* f = fopen(SAVE_PATH_ALARM, "wb");
    if (f) { fwrite(&alarmCfg, sizeof(AppAlarm), 1, f); fclose(f); }
}
static void loadAlarm() {
    FILE* f = fopen(SAVE_PATH_ALARM, "rb");
    if (f) { fread(&alarmCfg, sizeof(AppAlarm), 1, f); fclose(f); }
    else   { alarmCfg = (AppAlarm){0, 0, 0}; }
}
static bool initAudio() {
    if (ndspInit() != 0) return false;
    u32 samplesPerBeep = (BEEP_SAMPLE_RATE * BEEP_DURATION_MS) / 1000;
    u32 samplesPerGap  = (BEEP_SAMPLE_RATE * BEEP_GAP_MS)      / 1000;
    beepBufferSamples  = (samplesPerBeep + samplesPerGap) * BEEP_COUNT;
    beepBuffer = (s16*)linearAlloc(beepBufferSamples * sizeof(s16));
    if (!beepBuffer) { ndspExit(); return false; }
    u32 pos = 0;
    for (int b = 0; b < BEEP_COUNT; b++) {
        for (u32 i = 0; i < samplesPerBeep; i++) {
            float t = (float)i / (float)BEEP_SAMPLE_RATE;
            float sample = sinf(2.0f * M_PI * BEEP_TONE_HZ * t);
            u32 fadeSamples = BEEP_SAMPLE_RATE * 5 / 1000;
            float env = 1.0f;
            if (i < fadeSamples) env = (float)i / (float)fadeSamples;
            else if (i > samplesPerBeep - fadeSamples) env = (float)(samplesPerBeep - i) / (float)fadeSamples;
            beepBuffer[pos++] = (s16)(sample * env * BEEP_VOLUME * 32767.0f);
        }
        for (u32 i = 0; i < samplesPerGap; i++) {
            beepBuffer[pos++] = 0;
        }
    }
    ndspChnReset(0);
    ndspChnSetInterp(0, NDSP_INTERP_LINEAR);
    ndspChnSetRate(0, (float)BEEP_SAMPLE_RATE);
    ndspChnSetFormat(0, NDSP_FORMAT_MONO_PCM16);
    memset(&beepWaveBuf, 0, sizeof(beepWaveBuf));
    beepWaveBuf.data_vaddr = beepBuffer;
    beepWaveBuf.nsamples   = beepBufferSamples;
    beepWaveBuf.looping    = false;
    audioReady = true;
    return true;
}

/* -------------------- SUONI AMBIENTALI -------------------- */
// Parser WAV minimale: scorre i chunk del file cercando "fmt " e "data",
// non assume l'offset fisso a 44 byte (alcuni file hanno chunk extra
// prima dei dati veri e propri). Supporta solo PCM 16-bit; se il file è
// stereo prende solo il canale sinistro (il 3DS qui non ne trae comunque
// beneficio pratico).
typedef struct {
    s16* data;
    u32 samples;
    u32 sampleRate;
    bool loaded;
} WavSound;

static bool loadWavFile(const char* path, WavSound* out) {
    FILE* f = fopen(path, "rb");
    if (!f) return false;
    fseek(f, 0, SEEK_END);
    long fileSize = ftell(f);
    fseek(f, 0, SEEK_SET);
    if (fileSize < 44) { fclose(f); return false; }
    u8* raw = (u8*)malloc(fileSize);
    if (!raw) { fclose(f); return false; }
    if (fread(raw, 1, fileSize, f) != (size_t)fileSize) { free(raw); fclose(f); return false; }
    fclose(f);

    if (memcmp(raw, "RIFF", 4) != 0 || memcmp(raw + 8, "WAVE", 4) != 0) { free(raw); return false; }

    u16 numChannels = 1, bitsPerSample = 16;
    u32 sampleRate = 22050;
    u8* dataPtr = NULL;
    u32 dataSize = 0;

    u32 pos = 12;
    while (pos + 8 <= (u32)fileSize) {
        u32 chunkSize;
        memcpy(&chunkSize, raw + pos + 4, 4);
        if (memcmp(raw + pos, "fmt ", 4) == 0 && pos + 24 <= (u32)fileSize) {
            memcpy(&numChannels,   raw + pos + 10, 2);
            memcpy(&sampleRate,    raw + pos + 12, 4);
            memcpy(&bitsPerSample, raw + pos + 22, 2);
        } else if (memcmp(raw + pos, "data", 4) == 0) {
            dataPtr  = raw + pos + 8;
            dataSize = chunkSize;
        }
        pos += 8 + chunkSize + (chunkSize % 2); // i chunk WAV sono allineati a byte pari
    }

    if (!dataPtr || bitsPerSample != 16 || numChannels < 1) { free(raw); return false; }

    u32 sampleCount = dataSize / 2 / numChannels;
    s16* linBuf = (s16*)linearAlloc(sampleCount * sizeof(s16));
    if (!linBuf) { free(raw); return false; }

    s16* src = (s16*)dataPtr;
    if (numChannels == 1) {
        memcpy(linBuf, src, sampleCount * sizeof(s16));
    } else {
        for (u32 i = 0; i < sampleCount; i++) linBuf[i] = src[i * numChannels]; // solo canale sinistro
    }
    free(raw);

    out->data       = linBuf;
    out->samples    = sampleCount;
    out->sampleRate = sampleRate;
    out->loaded     = true;
    return true;
}

// Cinguettio pettirosso: 4 voci (ni1-ni4), una scelta a caso ad ogni
// riproduzione, a intervallo casuale 5-10 secondi, in tutte le stagioni,
// solo nelle fasi diverse da Notte. Canale NDSP 1 (il beep della sveglia
// resta sul canale 0, indipendente).
#define CHIRP_VOICE_COUNT 4
static WavSound chirpSounds[CHIRP_VOICE_COUNT] = {0};
static ndspWaveBuf chirpWaveBuf;
static float ambientChirpTimer = 0.0f;
static float ambientChirpNextInterval = 5.0f;
static bool  anyChirpLoaded = false;

static void initAmbientAudio() {
    if (!audioReady) return;
    static const char* chirpPaths[CHIRP_VOICE_COUNT] = {
        "romfs:/audio/ni1.wav", "romfs:/audio/ni2.wav",
        "romfs:/audio/ni3.wav", "romfs:/audio/ni4.wav"
    };
    for (int i = 0; i < CHIRP_VOICE_COUNT; i++) {
        if (loadWavFile(chirpPaths[i], &chirpSounds[i])) anyChirpLoaded = true;
    }
    if (!anyChirpLoaded) return;
    ndspChnReset(1);
    ndspChnSetInterp(1, NDSP_INTERP_LINEAR);
    ndspChnSetFormat(1, NDSP_FORMAT_MONO_PCM16);
    { float mix[12] = {0}; mix[0] = 0.50f; mix[1] = 0.50f; ndspChnSetMix(1, mix); } // volume a metà
    memset(&chirpWaveBuf, 0, sizeof(chirpWaveBuf));
    chirpWaveBuf.looping = false;
    ambientChirpNextInterval = 5.0f + (rand() % 501) / 100.0f; // 5.0-10.0s
}

static void updateAmbientSounds() {
    if (!audioReady || !anyChirpLoaded) return;
    if (currentForestPhase == FOREST_PHASE_NIGHT) { ambientChirpTimer = 0.0f; return; }
    ambientChirpTimer += 0.0166f;
    if (ambientChirpTimer >= ambientChirpNextInterval) {
        ambientChirpTimer -= ambientChirpNextInterval;
        ambientChirpNextInterval = 5.0f + (rand() % 501) / 100.0f; // prossimo intervallo, di nuovo 5.0-10.0s

        int voice;
        do { voice = rand() % CHIRP_VOICE_COUNT; } while (!chirpSounds[voice].loaded);
        WavSound* v = &chirpSounds[voice];

        ndspChnWaveBufClear(1);
        ndspChnSetRate(1, (float)v->sampleRate);
        DSP_FlushDataCache(v->data, v->samples * sizeof(s16));
        chirpWaveBuf.data_vaddr = v->data;
        chirpWaveBuf.nsamples   = v->samples;
        chirpWaveBuf.status     = NDSP_WBUF_FREE;
        ndspChnWaveBufAdd(1, &chirpWaveBuf);
    }
}

/* -------------------- PIOGGIA (AUDIO) -------------------- */
// Due loop continui su canali separati (2=normale "rain1", 3=forte
// "rainheavy"), sempre in riproduzione — il volume di ciascuno sfuma
// morbidamente verso il target invece di scattare acceso/spento o
// cambiare di colpo, così il passaggio normale<->forte e l'ingresso/
// uscita dalla pioggia sono sempre fluidi, mai un salto secco.
// weatherType: 1=Light,2=Med,3=Heavy,4=Extreme — "oltre il livello 2"
// vuol dire 3 e 4, quindi rainheavy.
static WavSound rainNormalSound = {0};
static WavSound rainHeavySound  = {0};
static ndspWaveBuf rainNormalWaveBuf, rainHeavyWaveBuf;
static float rainNormalVolume = 0.0f, rainHeavyVolume = 0.0f; // valori correnti, si avvicinano al target ogni frame
static bool rainAudioReady = false;

static void initRainAudio() {
    if (!audioReady) return;
    bool okNormal = loadWavFile("romfs:/audio/rain1.wav", &rainNormalSound);
    bool okHeavy  = loadWavFile("romfs:/audio/rainheavy.wav", &rainHeavySound);
    if (!okNormal && !okHeavy) return;
    rainAudioReady = true;

    if (okNormal) {
        ndspChnReset(2);
        ndspChnSetInterp(2, NDSP_INTERP_LINEAR);
        ndspChnSetRate(2, (float)rainNormalSound.sampleRate);
        ndspChnSetFormat(2, NDSP_FORMAT_MONO_PCM16);
        float mix[12] = {0}; ndspChnSetMix(2, mix); // silenzio all'avvio
        memset(&rainNormalWaveBuf, 0, sizeof(rainNormalWaveBuf));
        rainNormalWaveBuf.data_vaddr = rainNormalSound.data;
        rainNormalWaveBuf.nsamples   = rainNormalSound.samples;
        rainNormalWaveBuf.looping    = true;
        DSP_FlushDataCache(rainNormalSound.data, rainNormalSound.samples * sizeof(s16));
        ndspChnWaveBufAdd(2, &rainNormalWaveBuf);
    }
    if (okHeavy) {
        ndspChnReset(3);
        ndspChnSetInterp(3, NDSP_INTERP_LINEAR);
        ndspChnSetRate(3, (float)rainHeavySound.sampleRate);
        ndspChnSetFormat(3, NDSP_FORMAT_MONO_PCM16);
        float mix[12] = {0}; ndspChnSetMix(3, mix);
        memset(&rainHeavyWaveBuf, 0, sizeof(rainHeavyWaveBuf));
        rainHeavyWaveBuf.data_vaddr = rainHeavySound.data;
        rainHeavyWaveBuf.nsamples   = rainHeavySound.samples;
        rainHeavyWaveBuf.looping    = true;
        DSP_FlushDataCache(rainHeavySound.data, rainHeavySound.samples * sizeof(s16));
        ndspChnWaveBufAdd(3, &rainHeavyWaveBuf);
    }
}

static void updateRainAudio() {
    if (!rainAudioReady) return;
    bool wantHeavy  = (weatherType == 3 || weatherType == 4);
    bool wantNormal = (rainIntensity > 0.0f) && !wantHeavy;
    float targetNormal = wantNormal ? rainIntensity : 0.0f;
    float targetHeavy  = wantHeavy  ? rainIntensity : 0.0f;

    rainNormalVolume += (targetNormal - rainNormalVolume) * 0.02f;
    rainHeavyVolume  += (targetHeavy  - rainHeavyVolume ) * 0.02f;

    if (rainNormalSound.loaded) {
        float mix[12] = {0};
        mix[0] = rainNormalVolume; mix[1] = rainNormalVolume;
        ndspChnSetMix(2, mix);
    }
    if (rainHeavySound.loaded) {
        float mix[12] = {0};
        mix[0] = rainHeavyVolume; mix[1] = rainHeavyVolume;
        ndspChnSetMix(3, mix);
    }
}

/* -------------------- GIORNO (AUDIO AMBIENTALE) -------------------- */
// Loop continuo su canale 4, sempre in riproduzione — il volume sfuma
// dentro durante Alba/Giorno/Tramonto e sfuma fuori durante la Notte,
// stessa tecnica morbida già usata per la pioggia (mai uno scatto secco).
static WavSound daySound = {0};
static ndspWaveBuf dayWaveBuf;
static float dayVolume = 0.0f;
static bool dayAudioReady = false;

static void initDayAudio() {
    if (!audioReady) return;
    if (!loadWavFile("romfs:/audio/day.wav", &daySound)) return;
    dayAudioReady = true;
    ndspChnReset(4);
    ndspChnSetInterp(4, NDSP_INTERP_LINEAR);
    ndspChnSetRate(4, (float)daySound.sampleRate);
    ndspChnSetFormat(4, NDSP_FORMAT_MONO_PCM16);
    float mix[12] = {0}; ndspChnSetMix(4, mix); // silenzio all'avvio
    memset(&dayWaveBuf, 0, sizeof(dayWaveBuf));
    dayWaveBuf.data_vaddr = daySound.data;
    dayWaveBuf.nsamples   = daySound.samples;
    dayWaveBuf.looping    = true;
    DSP_FlushDataCache(daySound.data, daySound.samples * sizeof(s16));
    ndspChnWaveBufAdd(4, &dayWaveBuf);
}

static void updateDayAudio() {
    if (!dayAudioReady) return;
    float target = (currentForestPhase == FOREST_PHASE_NIGHT) ? 0.0f : 0.75f;
    dayVolume += (target - dayVolume) * 0.02f;
    float mix[12] = {0};
    mix[0] = dayVolume; mix[1] = dayVolume;
    ndspChnSetMix(4, mix);
}

/* -------------------- NOTTE (AUDIO AMBIENTALE) -------------------- */
// Loop continuo su canale 5, stessa tecnica di day.wav ma invertita:
// sfuma dentro in Notte, sfuma fuori nelle altre fasi.
static WavSound nightSound = {0};
static ndspWaveBuf nightWaveBuf;
static float nightVolume = 0.0f;
static bool nightAudioReady = false;

static void initNightAudio() {
    if (!audioReady) return;
    if (!loadWavFile("romfs:/audio/night.wav", &nightSound)) return;
    nightAudioReady = true;
    ndspChnReset(5);
    ndspChnSetInterp(5, NDSP_INTERP_LINEAR);
    ndspChnSetRate(5, (float)nightSound.sampleRate);
    ndspChnSetFormat(5, NDSP_FORMAT_MONO_PCM16);
    float mix[12] = {0}; ndspChnSetMix(5, mix);
    memset(&nightWaveBuf, 0, sizeof(nightWaveBuf));
    nightWaveBuf.data_vaddr = nightSound.data;
    nightWaveBuf.nsamples   = nightSound.samples;
    nightWaveBuf.looping    = true;
    DSP_FlushDataCache(nightSound.data, nightSound.samples * sizeof(s16));
    ndspChnWaveBufAdd(5, &nightWaveBuf);
}

static void updateNightAudio() {
    if (!nightAudioReady) return;
    float target = (currentForestPhase == FOREST_PHASE_NIGHT) ? 0.50f : 0.0f;
    nightVolume += (target - nightVolume) * 0.02f;
    float mix[12] = {0};
    mix[0] = nightVolume; mix[1] = nightVolume;
    ndspChnSetMix(5, mix);
}

/* -------------------- GUFO (AUDIO) -------------------- */
// Evento casuale, un solo verso, a intervallo casuale intorno ai 10
// secondi (8-12s, stessa idea del cinguettio) — solo di Notte, come il
// gufo visivo che vola solo in quella fase. Canale 6.
static WavSound owlSound = {0};
static ndspWaveBuf owlWaveBuf;
static float ambientOwlTimer = 0.0f;
static float ambientOwlNextInterval = 10.0f;
static bool owlAudioReady = false;

static void initOwlAudio() {
    if (!audioReady) return;
    if (!loadWavFile("romfs:/audio/owl.wav", &owlSound)) return;
    owlAudioReady = true;
    ndspChnReset(6);
    ndspChnSetInterp(6, NDSP_INTERP_LINEAR);
    ndspChnSetRate(6, (float)owlSound.sampleRate);
    ndspChnSetFormat(6, NDSP_FORMAT_MONO_PCM16);
    memset(&owlWaveBuf, 0, sizeof(owlWaveBuf));
    owlWaveBuf.looping = false;
    ambientOwlNextInterval = 15.0f + (rand() % 501) / 100.0f; // 15.0-20.0s, reso meno frequente
}

static void updateOwlAudio() {
    if (!owlAudioReady) return;
    if (currentForestPhase != FOREST_PHASE_NIGHT) { ambientOwlTimer = 0.0f; return; }
    ambientOwlTimer += 0.0166f;
    if (ambientOwlTimer >= ambientOwlNextInterval) {
        ambientOwlTimer -= ambientOwlNextInterval;
        ambientOwlNextInterval = 15.0f + (rand() % 501) / 100.0f; // 15.0-20.0s, reso meno frequente
        ndspChnWaveBufClear(6);
        DSP_FlushDataCache(owlSound.data, owlSound.samples * sizeof(s16));
        owlWaveBuf.data_vaddr = owlSound.data;
        owlWaveBuf.nsamples   = owlSound.samples;
        owlWaveBuf.status     = NDSP_WBUF_FREE;
        ndspChnWaveBufAdd(6, &owlWaveBuf);
    }
}

static void exitAudio() {
    if (!audioReady) return;
    ndspChnWaveBufClear(0);
    ndspChnWaveBufClear(1);
    ndspChnWaveBufClear(2);
    ndspChnWaveBufClear(3);
    ndspChnWaveBufClear(4);
    ndspChnWaveBufClear(5);
    ndspChnWaveBufClear(6);
    if (beepBuffer) { linearFree(beepBuffer); beepBuffer = NULL; }
    for (int i = 0; i < CHIRP_VOICE_COUNT; i++) {
        if (chirpSounds[i].data) { linearFree(chirpSounds[i].data); chirpSounds[i].data = NULL; }
    }
    if (rainNormalSound.data) { linearFree(rainNormalSound.data); rainNormalSound.data = NULL; }
    if (rainHeavySound.data)  { linearFree(rainHeavySound.data);  rainHeavySound.data  = NULL; }
    if (daySound.data)        { linearFree(daySound.data);        daySound.data        = NULL; }
    if (nightSound.data)      { linearFree(nightSound.data);      nightSound.data      = NULL; }
    if (owlSound.data)        { linearFree(owlSound.data);        owlSound.data        = NULL; }
    ndspExit();
    audioReady = false;
}
static void playTestBeep() {
    if (!audioReady) return;
    ndspChnWaveBufClear(0);
    DSP_FlushDataCache(beepBuffer, beepBufferSamples * sizeof(s16));
    beepWaveBuf.status = NDSP_WBUF_FREE;
    ndspChnWaveBufAdd(0, &beepWaveBuf);
}
static bool ringingFromTimer = false;
static void startAlarmRinging(bool fromTimer) {
    alarmRinging     = true;
    alarmRepeatTimer = 0;
    alarmFlashPhase  = 0.0f;
    lowerScreenOff   = false;
    ringingFromTimer = fromTimer;
}
static void stopAlarmRinging() {
    alarmRinging     = false;
    if (!ringingFromTimer) {
        alarmCfg.enabled = 0;
        alarmDirty       = true;
    }
    if (audioReady) ndspChnWaveBufClear(0);
}
static void updateAlarmRinging() {
    if (!alarmRinging) return;
    if (alarmRepeatTimer == 0) {
        playTestBeep();
        u32 samplesPerBeep = (BEEP_SAMPLE_RATE * BEEP_DURATION_MS) / 1000;
        u32 samplesPerGap  = (BEEP_SAMPLE_RATE * BEEP_GAP_MS)      / 1000;
        u32 groupMs        = ((samplesPerBeep + samplesPerGap) * BEEP_COUNT * 1000) / BEEP_SAMPLE_RATE;
        u32 groupFrames    = (groupMs * 60) / 1000;
        alarmRepeatTimer   = groupFrames + ALARM_REPEAT_PAUSE_FRAMES;
    }
    alarmRepeatTimer--;
    alarmFlashPhase += ALARM_FLASH_SPEED * (1.0f / 60.0f);
}
static void updateTimer() {
    if (timerState != TIMER_RUNNING) return;
    timerFrameAccumulator++;
    if (timerFrameAccumulator >= 60) {
        timerFrameAccumulator = 0;
        if (timerRemainingSeconds > 0) {
            timerRemainingSeconds--;
        }
        if (timerRemainingSeconds == 0) {
            timerState = TIMER_IDLE;
            startAlarmRinging(true);
        }
    }
}
static void drawDebugPointer(C2D_TextBuf buf) {
    if (!debugPointerActive) return;
    u32 white = C2D_Color32(255, 255, 255, 255);
    float cx = debugPointerX, cy = debugPointerY;
    if (debugPointerDisplayMode == 0) {
        float armLen = 8.0f;
        C2D_DrawLine(cx - armLen, cy, white, cx + armLen, cy, white, 1.0f, 0.9f);
        C2D_DrawLine(cx, cy - armLen, white, cx, cy + armLen, white, 1.0f, 0.9f);
    } else {
        C2D_Image img;
        if (debugPointerDisplayMode == 1) img = robinImage;
        else if (debugPointerDisplayMode == 2) img = robinImage2;
        else if (debugPointerDisplayMode == 3) img = nestImage;
        else if (debugPointerDisplayMode == 4) img = robinSleepImage;
        else if (debugPointerDisplayMode == 5) img = greyBirdSleepImage;
        else if (debugPointerDisplayMode == 6) img = hareWalk1Image;
        else if (debugPointerDisplayMode == 7) img = hareWalk2Image;
        else if (debugPointerDisplayMode == 8) img = hareStandSxImage;
        else if (debugPointerDisplayMode == 9) img = hareDx1Image;
        else if (debugPointerDisplayMode == 10) img = hareDx2Image;
        else img = hareStandDxImage;
        if (img.tex) {
            float imgW = img.subtex->width;
            float imgH = img.subtex->height;
            C2D_DrawImageAt(img, cx - imgW * 0.5f, cy - imgH, 0.9f, NULL, 1.0f, 1.0f);
        }
    }
    char coordStr[32];
    snprintf(coordStr, sizeof(coordStr), "X:%.0f Y:%.0f", cx, cy);
    C2D_Text tCoord; C2D_TextParse(&tCoord, buf, coordStr); C2D_TextOptimize(&tCoord);
    C2D_DrawText(&tCoord, C2D_WithColor, 4.0f, 4.0f, 0.9f, 0.5f, 0.5f, white);
}
static C2D_Image getForestBackgroundImageForSeason(ForestPhase phase, u32 season) {
    if (season == 0) { // Spring: tutte e 4 le fasi disponibili
        C2D_Image img;
        if (phase == FOREST_PHASE_DAY)        img = seasonSpringDayImage;
        else if (phase == FOREST_PHASE_DAWN)  img = seasonSpringDawnImage;
        else if (phase == FOREST_PHASE_DUSK)  img = seasonSpringDuskImage;
        else                                   img = seasonSpringNightImage;
        if (img.tex) return img;
    }
    if (season == 3) { // Winter: tutte e 4 le fasi disponibili
        C2D_Image img;
        if (phase == FOREST_PHASE_DAY)        img = seasonWinterDayImage;
        else if (phase == FOREST_PHASE_DAWN)  img = seasonWinterDawnImage;
        else if (phase == FOREST_PHASE_DUSK)  img = seasonWinterDuskImage;
        else                                   img = seasonWinterNightImage;
        if (img.tex) return img;
    }
    if (season == 2) { // Autumn: tutte e 4 le fasi disponibili
        C2D_Image img;
        if (phase == FOREST_PHASE_DAY)        img = seasonAutumnDayImage;
        else if (phase == FOREST_PHASE_DAWN)  img = seasonAutumnDawnImage;
        else if (phase == FOREST_PHASE_DUSK)  img = seasonAutumnDuskImage;
        else                                   img = seasonAutumnNightImage;
        if (img.tex) return img;
    }
    return forestPhaseImages[phase];
}
static C2D_Image getForestBackgroundImage(ForestPhase phase) {
    return getForestBackgroundImageForSeason(phase, currentEnvSeason);
}

static void drawBackground() {
    u32 activeTheme = (settings.bgThemeIndex < 5) ? settings.bgThemeIndex : 0;
    float targetTopR = phaseThemes[currentForestPhase][activeTheme].topR;
    float targetTopG = phaseThemes[currentForestPhase][activeTheme].topG;
    float targetTopB = phaseThemes[currentForestPhase][activeTheme].topB;
    float targetBotR = phaseThemes[currentForestPhase][activeTheme].botR;
    float targetBotG = phaseThemes[currentForestPhase][activeTheme].botG;
    float targetBotB = phaseThemes[currentForestPhase][activeTheme].botB;
    currentTopR += (targetTopR - currentTopR) * THEME_TRANSITION_SPEED;
    currentTopG += (targetTopG - currentTopG) * THEME_TRANSITION_SPEED;
    currentTopB += (targetTopB - currentTopB) * THEME_TRANSITION_SPEED;
    currentBotR += (targetBotR - currentBotR) * THEME_TRANSITION_SPEED;
    currentBotG += (targetBotG - currentBotG) * THEME_TRANSITION_SPEED;
    currentBotB += (targetBotB - currentBotB) * THEME_TRANSITION_SPEED;
    float drawTopR = currentTopR, drawTopG = currentTopG, drawTopB = currentTopB;
    float drawBotR = currentBotR, drawBotG = currentBotG, drawBotB = currentBotB;
    if (alarmRinging) {
        float pulse = (sinf(alarmFlashPhase) * 0.5f) + 0.5f;
        float flashR = 230.0f, flashG = 15.0f, flashB = 10.0f;
        drawTopR += (flashR - drawTopR) * pulse;
        drawTopG += (flashG - drawTopG) * pulse;
        drawTopB += (flashB - drawTopB) * pulse;
        drawBotR += (flashR - drawBotR) * pulse;
        drawBotG += (flashG - drawBotG) * pulse;
        drawBotB += (flashB - drawBotB) * pulse;
    }
    u32 topColor = C2D_Color32((u8)drawTopR, (u8)drawTopG, (u8)drawTopB, 255);
    u32 botColor = C2D_Color32((u8)drawBotR, (u8)drawBotG, (u8)drawBotB, 255);
    C2D_DrawRectangle(0, 0,   0.0f, 400, 120, topColor, topColor, botColor, botColor);
    C2D_DrawRectangle(0, 120, 0.0f, 400, 120, botColor, botColor, botColor, botColor);
    extern float globalAnimTime;
    drawSun(globalAnimTime);
    drawClouds(globalAnimTime);
    updateAndDrawRareCloud(globalAnimTime);
    updateAndDrawForestPlane(globalAnimTime);
    updateAndDrawForestBirds(globalAnimTime);
    drawMoon(globalAnimTime);
    drawForestStars(globalAnimTime);
    updateAndDrawForestShoots(globalAnimTime);
    {
        u32 envPhaseMode = settings.envPhaseMode;
        if (envPhaseMode != 5 && envPhaseMode != 6) {
            ForestPhase desiredPhase;
            if (envPhaseMode == 0) {
                time_t rawPhase = time(NULL);
                struct tm* tmPhase = localtime(&rawPhase);
                float hourFracPhase = tmPhase->tm_hour + tmPhase->tm_min / 60.0f;
                desiredPhase = getForestPhaseForHour(hourFracPhase);
            } else {
                static const ForestPhase envModeToPhase[5] = { FOREST_PHASE_DAY, FOREST_PHASE_DAWN, FOREST_PHASE_DAY, FOREST_PHASE_DUSK, FOREST_PHASE_NIGHT };
                desiredPhase = envModeToPhase[envPhaseMode];
            }
            if (!forestPhaseManualOverride && desiredPhase != currentForestPhase) {
                forestPreviousBgImage = getForestBackgroundImage(currentForestPhase);
                previousForestPhase = currentForestPhase;
                currentForestPhase = desiredPhase;
                forestPhaseTransition = forestBgTransitionDuration = FOREST_PHASE_TRANSITION_SECONDS;
                forestBgFadeSymmetric = false;
            }
        }
        u32 envSeasonMode = settings.envSeasonMode;
        if (envSeasonMode != 5 && envSeasonMode != 6) {
            u32 desiredSeason;
            if (envSeasonMode == 0) {
                time_t rawSeason = time(NULL);
                struct tm* tmSeason = localtime(&rawSeason);
                desiredSeason = getSeasonForDate(tmSeason->tm_mon + 1, tmSeason->tm_mday);
            } else {
                static const u32 envModeToSeason[5] = { 1, 0, 1, 2, 3 };
                desiredSeason = envModeToSeason[envSeasonMode];
            }
            if (desiredSeason != currentEnvSeason) {
                // Cambio stagione: dissolvenza SIMMETRICA (entrambe le
                // immagini sfumano, non una fissa opaca e una sopra) — i
                // buchi nella chioma di ciascuna stagione mostrano
                // contenuti diversi (l'altra stagione, non il cielo),
                // quindi la tecnica usata per le fasi produrrebbe uno
                // scatto netto proprio a fine transizione. Solo arrivando
                // a Winter la dissolvenza è più lunga e dolce.
                forestPreviousBgImage = getForestBackgroundImageForSeason(currentForestPhase, currentEnvSeason);
                currentEnvSeason = desiredSeason;
                forestBgTransitionDuration = 1.5f; // tutte le transizioni stagionali, Winter incluso (era 6.0 solo per Winter, ora uniformato a 1.5 come le altre)
                forestPhaseTransition = forestBgTransitionDuration;
                forestBgFadeSymmetric = true;
            }
        }
        if (forestPhaseTransition > 0.0f) {
            forestPhaseTransition -= 0.0166f;
            if (forestPhaseTransition < 0.0f) forestPhaseTransition = 0.0f;
        }
    }
    if (forestPhaseTransition > 0.0f) {
        float fadeIn = 1.0f - (forestPhaseTransition / forestBgTransitionDuration);
        C2D_Image prevImg = forestPreviousBgImage;
        C2D_Image currImg = getForestBackgroundImage(currentForestPhase);
        if (forestBgFadeSymmetric) {
            if (prevImg.tex) {
                C2D_ImageTint prevTint;
                C2D_AlphaImageTint(&prevTint, 1.0f - fadeIn);
                C2D_DrawImageAt(prevImg, 0, 0, 0.1f, &prevTint, 1.0f, 1.0f);
            }
            if (currImg.tex) {
                C2D_ImageTint currTint;
                C2D_AlphaImageTint(&currTint, fadeIn);
                C2D_DrawImageAt(currImg, 0, 0, 0.1f, &currTint, 1.0f, 1.0f);
            }
        } else {
            if (prevImg.tex) {
                C2D_DrawImageAt(prevImg, 0, 0, 0.1f, NULL, 1.0f, 1.0f);
            }
            if (currImg.tex) {
                C2D_ImageTint currTint;
                C2D_AlphaImageTint(&currTint, fadeIn);
                C2D_DrawImageAt(currImg, 0, 0, 0.1f, &currTint, 1.0f, 1.0f);
            }
        }
    } else {
        C2D_Image img = getForestBackgroundImage(currentForestPhase);
        if (img.tex) {
            C2D_DrawImageAt(img, 0, 0, 0.1f, NULL, 1.0f, 1.0f);
        }
    }
    updateAndDrawForestFireflies(globalAnimTime);
    updateAndDrawForestHedgehogs(globalAnimTime);
    updateAndDrawForestRobin(globalAnimTime);
    updateAndDrawForestGreyBird(globalAnimTime);
    updateAndDrawHare(globalAnimTime);
    updateAndDrawFox(globalAnimTime);
    updateAndDrawForestButterflies(globalAnimTime);
    updateAndDrawForestOwl(globalAnimTime);
    updateAndDrawForestLeaves(globalAnimTime);
    updateAndDrawRain(globalAnimTime);
    updateAndDrawSnow(globalAnimTime);
}
static void drawClock(C2D_TextBuf buf, struct tm* tmv) {
    char timeStr[32], dateStr[32];
    if (settings.timeFormat24h) {
        snprintf(timeStr, sizeof(timeStr), "%02d:%02d:%02d",
                 tmv->tm_hour, tmv->tm_min, tmv->tm_sec);
    } else {
        int hour12 = tmv->tm_hour % 12;
        if (hour12 == 0) hour12 = 12;
        snprintf(timeStr, sizeof(timeStr), "%02d:%02d:%02d %s",
                 hour12, tmv->tm_min, tmv->tm_sec,
                 (tmv->tm_hour >= 12) ? "pm" : "am");
    }
    if (settings.dateFormat == 0)
        snprintf(dateStr, sizeof(dateStr), "%02d/%02d/%04d",
                 tmv->tm_mon+1, tmv->tm_mday, tmv->tm_year+1900);
    else if (settings.dateFormat == 1)
        snprintf(dateStr, sizeof(dateStr), "%02d/%02d/%04d",
                 tmv->tm_mday, tmv->tm_mon+1, tmv->tm_year+1900);
    else
        snprintf(dateStr, sizeof(dateStr), "%04d-%02d-%02d",
                 tmv->tm_year+1900, tmv->tm_mon+1, tmv->tm_mday);
    C2D_Text timeText, dateText;
    C2D_TextParse(&timeText, buf, timeStr); C2D_TextOptimize(&timeText);
    C2D_TextParse(&dateText, buf, dateStr); C2D_TextOptimize(&dateText);
    float tScaleX = 1.35f, tScaleY = 1.60f;
    float dScaleX = 0.75f, dScaleY = 0.85f;
    if (settings.clockSizePreset == 1) {
        tScaleX *= 1.15f; tScaleY *= 1.15f;
        dScaleX *= 1.15f; dScaleY *= 1.15f;
    } else if (settings.clockSizePreset == 2) {
        tScaleX *= 1.30f; tScaleY *= 1.30f;
        dScaleX *= 1.30f; dScaleY *= 1.30f;
    } else if (settings.clockSizePreset == 3) {
        tScaleX *= 0.78f; tScaleY *= 0.78f;
        dScaleX *= 0.78f; dScaleY *= 0.78f;
    }
    float tw, th, dw, dh;
    C2D_TextGetDimensions(&timeText, tScaleX, tScaleY, &tw, &th);
    C2D_TextGetDimensions(&dateText, dScaleX, dScaleY, &dw, &dh);
    float widestHalf = fmaxf(tw, dw) * 0.5f;
    float maxOffsetX = 198.0f - widestHalf;
    if (maxOffsetX < 0.0f) maxOffsetX = 0.0f;
    if (settings.clockOffsetX >  maxOffsetX) settings.clockOffsetX =  maxOffsetX;
    if (settings.clockOffsetX < -maxOffsetX) settings.clockOffsetX = -maxOffsetX;
    const float cx = 200.0f + settings.clockOffsetX;
    float timeX = cx - tw * 0.5f;
    float dateX = cx - dw * 0.5f;
    float timeY = 148.0f - th + settings.clockOffsetY;
    float dateY = 140.0f + settings.clockOffsetY;
    float targetR, targetG, targetB;
    if (settings.clockColorIndex == 12) {
        extern float globalAnimTime;
        getRainbowColor(globalAnimTime, &targetR, &targetG, &targetB);
    } else {
        u32 targetCol = clockPresets[settings.clockColorIndex];
        targetR = (float)(targetCol & 0xFF);
        targetG = (float)((targetCol >> 8) & 0xFF);
        targetB = (float)((targetCol >> 16) & 0xFF);
    }
    currentClockR += (targetR - currentClockR) * COLOR_TRANSITION_SPEED;
    currentClockG += (targetG - currentClockG) * COLOR_TRANSITION_SPEED;
    currentClockB += (targetB - currentClockB) * COLOR_TRANSITION_SPEED;
    float drawClockR = currentClockR, drawClockG = currentClockG, drawClockB = currentClockB;
    if (alarmRinging) {
        float pulse = (sinf(alarmFlashPhase) * 0.5f) + 0.5f;
        drawClockR += (230.0f - drawClockR) * pulse;
        drawClockG += ( 15.0f - drawClockG) * pulse;
        drawClockB += ( 10.0f - drawClockB) * pulse;
    }
    u32 col    = C2D_Color32((u8)drawClockR, (u8)drawClockG, (u8)drawClockB, 255);
    u32 shadow = C2D_Color32(12, 12, 24, 220);
    const float off = 2.0f;
    C2D_DrawText(&timeText, C2D_WithColor, timeX-1+off, timeY+off,   0.48f, tScaleX, tScaleY, shadow);
    C2D_DrawText(&timeText, C2D_WithColor, timeX+1+off, timeY+off,   0.48f, tScaleX, tScaleY, shadow);
    C2D_DrawText(&timeText, C2D_WithColor, timeX+off,   timeY-1+off, 0.48f, tScaleX, tScaleY, shadow);
    C2D_DrawText(&timeText, C2D_WithColor, timeX+off,   timeY+1+off, 0.48f, tScaleX, tScaleY, shadow);
    C2D_DrawText(&timeText, C2D_WithColor, timeX+off,   timeY+off,   0.48f, tScaleX, tScaleY, shadow);
    C2D_DrawText(&timeText, C2D_WithColor, timeX-1, timeY,   0.50f, tScaleX, tScaleY, col);
    C2D_DrawText(&timeText, C2D_WithColor, timeX+1, timeY,   0.50f, tScaleX, tScaleY, col);
    C2D_DrawText(&timeText, C2D_WithColor, timeX,   timeY-1, 0.50f, tScaleX, tScaleY, col);
    C2D_DrawText(&timeText, C2D_WithColor, timeX,   timeY+1, 0.50f, tScaleX, tScaleY, col);
    C2D_DrawText(&timeText, C2D_WithColor, timeX,   timeY,   0.50f, tScaleX, tScaleY, col);
    if (settings.clockMode == 0) {
        const float doff = 1.5f;
        C2D_DrawText(&dateText, C2D_WithColor, dateX-0.8f+doff, dateY+doff, 0.48f, dScaleX, dScaleY, shadow);
        C2D_DrawText(&dateText, C2D_WithColor, dateX+0.8f+doff, dateY+doff, 0.48f, dScaleX, dScaleY, shadow);
        C2D_DrawText(&dateText, C2D_WithColor, dateX+doff,      dateY+doff, 0.48f, dScaleX, dScaleY, shadow);
        C2D_DrawText(&dateText, C2D_WithColor, dateX-0.8f, dateY, 0.50f, dScaleX, dScaleY, col);
        C2D_DrawText(&dateText, C2D_WithColor, dateX+0.8f, dateY, 0.50f, dScaleX, dScaleY, col);
        C2D_DrawText(&dateText, C2D_WithColor, dateX,      dateY, 0.50f, dScaleX, dScaleY, col);
    }
}
static void drawTimerCountdown(C2D_TextBuf buf) {
    if (timerState != TIMER_RUNNING && timerState != TIMER_PAUSED) return;
    u32 h = timerRemainingSeconds / 3600;
    u32 m = (timerRemainingSeconds % 3600) / 60;
    u32 s = timerRemainingSeconds % 60;
    char timerStr[16];
    snprintf(timerStr, sizeof(timerStr), "%02u:%02u:%02u", (unsigned)h, (unsigned)m, (unsigned)s);
    C2D_Text timerText;
    C2D_TextParse(&timerText, buf, timerStr); C2D_TextOptimize(&timerText);
    float tScaleX = 1.35f, tScaleY = 1.60f;
    if (settings.clockSizePreset == 1)      { tScaleX *= 1.15f; tScaleY *= 1.15f; }
    else if (settings.clockSizePreset == 2) { tScaleX *= 1.30f; tScaleY *= 1.30f; }
    else if (settings.clockSizePreset == 3) { tScaleX *= 0.78f; tScaleY *= 0.78f; }
    float tw, th;
    C2D_TextGetDimensions(&timerText, tScaleX, tScaleY, &tw, &th);
    const float cx = 200.0f + settings.clockOffsetX;
    float timerX = cx - tw * 0.5f;
    float timerY = 165.0f + ((240.0f - 165.0f) - th) * 0.5f + settings.clockOffsetY;
    float drawR = currentClockR, drawG = currentClockG, drawB = currentClockB;
    if (alarmRinging) {
        float pulse = (sinf(alarmFlashPhase) * 0.5f) + 0.5f;
        drawR += (230.0f - drawR) * pulse;
        drawG += ( 15.0f - drawG) * pulse;
        drawB += ( 10.0f - drawB) * pulse;
    }
    u32 col    = C2D_Color32((u8)drawR, (u8)drawG, (u8)drawB, 255);
    u32 shadow = C2D_Color32(12, 12, 24, 220);
    const float off = 2.0f;
    C2D_DrawText(&timerText, C2D_WithColor, timerX-1+off, timerY+off,   0.48f, tScaleX, tScaleY, shadow);
    C2D_DrawText(&timerText, C2D_WithColor, timerX+1+off, timerY+off,   0.48f, tScaleX, tScaleY, shadow);
    C2D_DrawText(&timerText, C2D_WithColor, timerX+off,   timerY-1+off, 0.48f, tScaleX, tScaleY, shadow);
    C2D_DrawText(&timerText, C2D_WithColor, timerX+off,   timerY+1+off, 0.48f, tScaleX, tScaleY, shadow);
    C2D_DrawText(&timerText, C2D_WithColor, timerX+off,   timerY+off,   0.48f, tScaleX, tScaleY, shadow);
    C2D_DrawText(&timerText, C2D_WithColor, timerX-1, timerY,   0.50f, tScaleX, tScaleY, col);
    C2D_DrawText(&timerText, C2D_WithColor, timerX+1, timerY,   0.50f, tScaleX, tScaleY, col);
    C2D_DrawText(&timerText, C2D_WithColor, timerX,   timerY-1, 0.50f, tScaleX, tScaleY, col);
    C2D_DrawText(&timerText, C2D_WithColor, timerX,   timerY+1, 0.50f, tScaleX, tScaleY, col);
    C2D_DrawText(&timerText, C2D_WithColor, timerX,   timerY,   0.50f, tScaleX, tScaleY, col);
}
float globalAnimTime = 0.0f;
// Disegna i due pulsantini +/- sopra e sotto un campo largo 80px (usato
// da Alarm e Timer) — stesse coordinate del rilevamento tocco in
// getHoldTargetAtTouch (y=66-79 per "+", y=151-164 per "-").
// Spezza un testo in righe che stanno entro maxWidth pixel, andando a
// capo tra le parole (mai a metà parola) — rispetta anche gli a capo
// manuali (\n) già presenti nel testo (es. dal tasto Invio sulla
// tastiera). Scritta per il diario, ma generica: qualunque testo,
// larghezza massima e dimensione font.
#define WRAP_MAX_LINES 12
#define WRAP_LINE_LEN 200
static int wrapTextIntoLines(const char* text, float maxWidth, float fontScale, C2D_TextBuf buf, char lines[][WRAP_LINE_LEN]) {
    int lineCount = 0;
    char currentLine[WRAP_LINE_LEN] = "";
    char word[WRAP_LINE_LEN];
    int wordLen = 0;
    int textLen = strlen(text);

    for (int i = 0; i <= textLen; i++) {
        char c = text[i];
        bool isBreak = (c == ' ' || c == '\n' || c == '\0');
        if (!isBreak) {
            if (wordLen < WRAP_LINE_LEN - 1) word[wordLen++] = c;
            continue;
        }
        word[wordLen] = '\0';

        if (wordLen > 0) {
            char testLine[WRAP_LINE_LEN * 2 + 2];
            if (currentLine[0]) snprintf(testLine, sizeof(testLine), "%s %s", currentLine, word);
            else                 snprintf(testLine, sizeof(testLine), "%s", word);

            C2D_Text tTest; C2D_TextParse(&tTest, buf, testLine); C2D_TextOptimize(&tTest);
            float tw, th;
            C2D_TextGetDimensions(&tTest, fontScale, fontScale, &tw, &th);

            if (tw <= maxWidth || currentLine[0] == '\0') {
                #pragma GCC diagnostic push
                #pragma GCC diagnostic ignored "-Wformat-truncation"
                snprintf(currentLine, sizeof(currentLine), "%s", testLine);
                #pragma GCC diagnostic pop
            } else {
                if (lineCount < WRAP_MAX_LINES) snprintf(lines[lineCount++], WRAP_LINE_LEN, "%s", currentLine);
                snprintf(currentLine, sizeof(currentLine), "%s", word);
            }
        }
        wordLen = 0;

        if (c == '\n') {
            if (lineCount < WRAP_MAX_LINES) snprintf(lines[lineCount++], WRAP_LINE_LEN, "%s", currentLine);
            currentLine[0] = '\0';
        }
    }
    if (currentLine[0] && lineCount < WRAP_MAX_LINES) {
        snprintf(lines[lineCount++], WRAP_LINE_LEN, "%s", currentLine);
    }
    return lineCount;
}
// Titolo con una singola ombra sfalsata di 2px per un po' di profondità,
// ma caratteri sottili (non più ispessiti a più livelli come nella
// versione "bold" precedente).
static void drawSlimShadowTitle(const char* text, C2D_TextBuf buf, float titleX, float titleY, float titleScale, u32 titleCol, u32 titleShadow) {
    C2D_Text title;
    C2D_TextParse(&title, buf, text);
    C2D_TextOptimize(&title);
    const float off = 2.0f;
    C2D_DrawText(&title, C2D_WithColor, titleX+off, titleY+off, 0.48f, titleScale, titleScale, titleShadow);
    C2D_DrawText(&title, C2D_WithColor, titleX,     titleY,     0.5f,  titleScale, titleScale, titleCol);
}
static void drawStepperButtons(float fieldX, float fieldWidth, float plusY, float minusY, float stepperH, C2D_TextBuf buf, u32 textColor) {
    float sw, sh;
    C2D_DrawRectSolid(fieldX, plusY, 0.0f, fieldWidth, stepperH, C2D_Color32(80, 65, 60, 255));
    char plusStr[2] = "+"; C2D_Text tPlus; C2D_TextParse(&tPlus, buf, plusStr); C2D_TextOptimize(&tPlus);
    C2D_TextGetDimensions(&tPlus, 0.42f, 0.42f, &sw, &sh);
    C2D_DrawText(&tPlus, C2D_WithColor, fieldX+(fieldWidth-sw)*0.5f, plusY+(stepperH-sh)*0.5f, 0.5f, 0.42f, 0.42f, textColor);
    C2D_DrawRectSolid(fieldX, minusY, 0.0f, fieldWidth, stepperH, C2D_Color32(80, 65, 60, 255));
    char minusStr[2] = "-"; C2D_Text tMinus; C2D_TextParse(&tMinus, buf, minusStr); C2D_TextOptimize(&tMinus);
    C2D_TextGetDimensions(&tMinus, 0.42f, 0.42f, &sw, &sh);
    C2D_DrawText(&tMinus, C2D_WithColor, fieldX+(fieldWidth-sw)*0.5f, minusY+(stepperH-sh)*0.5f, 0.5f, 0.42f, 0.42f, textColor);
}
static void drawBottomScreen(C2D_TextBuf buf, C3D_RenderTarget* target) {
    C2D_SceneBegin(target);
    if (lowerScreenOff) {
        C2D_TargetClear(target, C2D_Color32(0, 0, 0, 255));
        return;
    }
    u32 colYellow    = C2D_Color32(240, 200,  30, 255);
    u32 colTextBlack = C2D_Color32( 15,  15,  20, 255);
    u32 colTextWhite = C2D_Color32(255, 255, 255, 255);
    u32 colWhite     = C2D_Color32(225, 218, 190, 255);
    u32 colLightGreen= C2D_Color32( 50, 210, 100, 255);
    u32 colCyan      = C2D_Color32(200, 175,  95, 255);
    u32 colRed       = C2D_Color32(220,  40,  40, 255);
    u32 colForestGreen = C2D_Color32(140, 170, 140, 255); // stesso verde del pulsante PtrObj in Debug, per coerenza
    if (currentScreen == SCREEN_MAIN) {
        u32 bottomBgColor = C2D_Color32(22, 30, 20, 255);
        C2D_TargetClear(target, bottomBgColor);
        updateAndDrawBottomRain();
        updateAndDrawBottomSnow(globalAnimTime);
        C2D_DrawRectSolid(10, 10,  0.0f, 300, 2, C2D_Color32(60, 60, 80, 255));
        C2D_DrawRectSolid(10, 228, 0.0f, 300, 2, C2D_Color32(60, 60, 80, 255));
        char mainBattStr[16]; snprintf(mainBattStr, sizeof(mainBattStr), "%d%%%s", batteryPercentPrecise, batteryCharging ? "+" : "");
        C2D_Text tMainBatt; C2D_TextParse(&tMainBatt, buf, mainBattStr); C2D_TextOptimize(&tMainBatt);
        float mainBattw, mainBatth;
        C2D_TextGetDimensions(&tMainBatt, 0.5f, 0.5f, &mainBattw, &mainBatth);
        C2D_DrawText(&tMainBatt, C2D_WithColor, 310.0f - mainBattw, 15, 0.5f, 0.5f, 0.5f, C2D_Color32(90, 220, 100, 255));
        char wifiLabelStr[8]; snprintf(wifiLabelStr, sizeof(wifiLabelStr), "Wifi");
        C2D_Text tWifiLabel; C2D_TextParse(&tWifiLabel, buf, wifiLabelStr); C2D_TextOptimize(&tWifiLabel);
        float wifiLabelW, wifiLabelH;
        C2D_TextGetDimensions(&tWifiLabel, 0.5f, 0.5f, &wifiLabelW, &wifiLabelH);
        C2D_DrawText(&tWifiLabel, C2D_WithColor, 10, 17, 0.5f, 0.5f, 0.5f, C2D_Color32(90, 220, 100, 255));
        if (!wifiConnectedCache) {
            char wifiXStr[4]; snprintf(wifiXStr, sizeof(wifiXStr), "X");
            C2D_Text tWifiX; C2D_TextParse(&tWifiX, buf, wifiXStr); C2D_TextOptimize(&tWifiX);
            C2D_DrawText(&tWifiX, C2D_WithColor, 10 + wifiLabelW + 6, 15, 0.5f, 0.5f, 0.5f, C2D_Color32(230, 60, 60, 255));
        } else {
            float barW = 4.0f, barGap = 2.0f, barsBaseY = 15.0f + wifiLabelH;
            for (int wb = 0; wb < 3; wb++) {
                float barH = 4.0f + wb * 3.0f;
                float barX = 10.0f + wifiLabelW + 6.0f + wb * (barW + barGap);
                float barY = barsBaseY - barH;
                u32 barCol = (wb < wifiStrengthCache) ? C2D_Color32(90, 220, 100, 255) : C2D_Color32(70, 80, 70, 255);
                C2D_DrawRectSolid(barX, barY, 0.5f, barW, barH, barCol);
            }
        }
        char mainTempStr[16];
        if (lastWeatherTemp < -900.0) snprintf(mainTempStr, sizeof(mainTempStr), "--");
        else snprintf(mainTempStr, sizeof(mainTempStr), "%.1fC", lastWeatherTemp);
        C2D_Text tMainTemp; C2D_TextParse(&tMainTemp, buf, mainTempStr); C2D_TextOptimize(&tMainTemp);
        C2D_DrawText(&tMainTemp, C2D_WithColor, 10, 35, 0.5f, 0.5f, 0.5f, C2D_Color32(90, 220, 100, 255));
        float tw, th;
        const float titleScale = 0.75f * 1.4f;
        C2D_TextGetDimensions(&ui.title, titleScale, titleScale, &tw, &th);
        {
            float titleX = (320.0f - tw) * 0.5f;
            float titleY = 17.0f;
            u32 titleCol    = colForestGreen; // stesso verde dei pulsanti, per coerenza
            u32 titleShadow = C2D_Color32(8, 12, 6, 220);
            const float off = 2.0f;
            C2D_DrawText(&ui.title, C2D_WithColor, titleX-1+off, titleY+off,   0.48f, titleScale, titleScale, titleShadow);
            C2D_DrawText(&ui.title, C2D_WithColor, titleX+1+off, titleY+off,   0.48f, titleScale, titleScale, titleShadow);
            C2D_DrawText(&ui.title, C2D_WithColor, titleX+off,   titleY-1+off, 0.48f, titleScale, titleScale, titleShadow);
            C2D_DrawText(&ui.title, C2D_WithColor, titleX+off,   titleY+1+off, 0.48f, titleScale, titleScale, titleShadow);
            C2D_DrawText(&ui.title, C2D_WithColor, titleX+off,   titleY+off,   0.48f, titleScale, titleScale, titleShadow);
            C2D_DrawText(&ui.title, C2D_WithColor, titleX-1, titleY,   0.5f, titleScale, titleScale, titleCol);
            C2D_DrawText(&ui.title, C2D_WithColor, titleX+1, titleY,   0.5f, titleScale, titleScale, titleCol);
            C2D_DrawText(&ui.title, C2D_WithColor, titleX,   titleY-1, 0.5f, titleScale, titleScale, titleCol);
            C2D_DrawText(&ui.title, C2D_WithColor, titleX,   titleY+1, 0.5f, titleScale, titleScale, titleCol);
            C2D_DrawText(&ui.title, C2D_WithColor, titleX,   titleY,   0.5f, titleScale, titleScale, titleCol);
        }
        float mainAlarmBottomY = 47.0f;
        mainAlarmTextVisible = false;
        mainTimerTextVisible = false;
        if (alarmCfg.enabled) {
            char mainAlarmStr[48];
            if (!settings.timeFormat24h) {
                u32 h12 = alarmCfg.hour % 12; if (h12 == 0) h12 = 12;
                snprintf(mainAlarmStr, sizeof(mainAlarmStr), "Alarm set: %02u:%02u %s",
                         (unsigned)h12, (unsigned)alarmCfg.minute, (alarmCfg.hour >= 12) ? "pm" : "am");
            } else {
                snprintf(mainAlarmStr, sizeof(mainAlarmStr), "Alarm set: %02u:%02u", (unsigned)alarmCfg.hour, (unsigned)alarmCfg.minute);
            }
            C2D_Text tMainAlarm; C2D_TextParse(&tMainAlarm, buf, mainAlarmStr); C2D_TextOptimize(&tMainAlarm);
            C2D_TextGetDimensions(&tMainAlarm, 0.45f, 0.45f, &tw, &th);
            C2D_DrawText(&tMainAlarm, C2D_WithColor, (320.0f-tw)*0.5f, mainAlarmBottomY, 0.5f, 0.45f, 0.45f, C2D_Color32(240,180,100,255));
            mainAlarmTextVisible = true;
            mainAlarmTextX0 = (320.0f-tw)*0.5f; mainAlarmTextX1 = mainAlarmTextX0 + tw;
            mainAlarmTextY0 = mainAlarmBottomY; mainAlarmTextY1 = mainAlarmBottomY + th;
            mainAlarmBottomY += th + 4.0f;
        }
        if (timerState == TIMER_RUNNING || timerState == TIMER_PAUSED) {
            u32 th_ = timerRemainingSeconds / 3600;
            u32 tm_ = (timerRemainingSeconds % 3600) / 60;
            u32 ts_ = timerRemainingSeconds % 60;
            char mainTimerStr[32];
            if (th_ > 0) snprintf(mainTimerStr, sizeof(mainTimerStr), "Timer: %02u:%02u:%02u", (unsigned)th_, (unsigned)tm_, (unsigned)ts_);
            else         snprintf(mainTimerStr, sizeof(mainTimerStr), "Timer: %02u:%02u", (unsigned)tm_, (unsigned)ts_);
            C2D_Text tMainTimer; C2D_TextParse(&tMainTimer, buf, mainTimerStr); C2D_TextOptimize(&tMainTimer);
            C2D_TextGetDimensions(&tMainTimer, 0.45f, 0.45f, &tw, &th);
            C2D_DrawText(&tMainTimer, C2D_WithColor, (320.0f-tw)*0.5f, mainAlarmBottomY, 0.5f, 0.45f, 0.45f, C2D_Color32(120,200,240,255));
            mainTimerTextVisible = true;
            mainTimerTextX0 = (320.0f-tw)*0.5f; mainTimerTextX1 = mainTimerTextX0 + tw;
            mainTimerTextY0 = mainAlarmBottomY; mainTimerTextY1 = mainAlarmBottomY + th;
        }
        float bw, bh;
        C2D_DrawRectSolid(15, 185, 0.7f, 90, 32, colForestGreen);
        C2D_TextGetDimensions(&ui.btnSettings, 0.55f, 0.55f, &bw, &bh);
        C2D_DrawText(&ui.btnSettings, C2D_WithColor, 15+(90-bw)*0.5f, 185+(32-bh)*0.5f, 0.7f, 0.55f, 0.55f, colTextBlack);
        C2D_DrawRectSolid(115, 185, 0.7f, 90, 32, colRed);
        C2D_TextGetDimensions(&ui.btnOff, 0.55f, 0.55f, &bw, &bh);
        C2D_DrawText(&ui.btnOff, C2D_WithColor, 115+(90-bw)*0.5f, 185+(32-bh)*0.5f, 0.7f, 0.55f, 0.55f, colTextWhite);
        C2D_DrawRectSolid(215, 185, 0.7f, 90, 32, colForestGreen);
        C2D_TextGetDimensions(&ui.btnRecords, 0.55f, 0.55f, &bw, &bh);
        C2D_DrawText(&ui.btnRecords, C2D_WithColor, 215+(90-bw)*0.5f, 185+(32-bh)*0.5f, 0.7f, 0.55f, 0.55f, colTextBlack);
        C2D_DrawRectSolid(115, 148, 0.7f, 90, 32, colForestGreen);
        char noteBtnStr[8]; snprintf(noteBtnStr, sizeof(noteBtnStr), "Notes");
        C2D_Text tNoteBtn; C2D_TextParse(&tNoteBtn, buf, noteBtnStr); C2D_TextOptimize(&tNoteBtn);
        C2D_TextGetDimensions(&tNoteBtn, 0.55f, 0.55f, &bw, &bh);
        C2D_DrawText(&tNoteBtn, C2D_WithColor, 115+(90-bw)*0.5f, 148+(32-bh)*0.5f, 0.7f, 0.55f, 0.55f, colTextBlack);
        C2D_DrawRectSolid(15, 148, 0.7f, 90, 32, colForestGreen);
        C2D_TextGetDimensions(&ui.btnEnvironment, 0.45f, 0.45f, &bw, &bh);
        C2D_DrawText(&ui.btnEnvironment, C2D_WithColor, 15+(90-bw)*0.5f, 148+(32-bh)*0.5f, 0.7f, 0.45f, 0.45f, colTextBlack);
        C2D_DrawRectSolid(215, 148, 0.7f, 90, 32, colForestGreen);
        char calBtnStr[16]; snprintf(calBtnStr, sizeof(calBtnStr), "Calendar");
        C2D_Text tCalBtn; C2D_TextParse(&tCalBtn, buf, calBtnStr); C2D_TextOptimize(&tCalBtn);
        C2D_TextGetDimensions(&tCalBtn, 0.5f, 0.5f, &bw, &bh);
        C2D_DrawText(&tCalBtn, C2D_WithColor, 215+(90-bw)*0.5f, 148+(32-bh)*0.5f, 0.7f, 0.5f, 0.5f, colTextBlack);
    }
    else if (currentScreen == SCREEN_RECORDS) {
        u32 bottomBgColor = C2D_Color32(22, 30, 20, 255);
        C2D_TargetClear(target, bottomBgColor);
        C2D_DrawRectSolid(10, 10,  0.0f, 300, 2, C2D_Color32(80, 50, 60, 255));
        C2D_DrawRectSolid(10, 228, 0.0f, 300, 2, C2D_Color32(80, 50, 60, 255));
        float tw, th;
        C2D_TextGetDimensions(&ui.hintMove, 0.50f, 0.50f, &tw, &th);
        C2D_DrawText(&ui.hintMove, C2D_WithColor, (320.0f-tw)*0.5f, 32, 0.5f, 0.50f, 0.50f, C2D_Color32(180,180,180,255));
        C2D_TextGetDimensions(&ui.hintCenter, 0.50f, 0.50f, &tw, &th);
        C2D_DrawText(&ui.hintCenter, C2D_WithColor, (320.0f-tw)*0.5f, 57, 0.5f, 0.50f, 0.50f, C2D_Color32(180,180,180,255));
        C2D_TextGetDimensions(&ui.hintSelect, 0.50f, 0.50f, &tw, &th);
        C2D_DrawText(&ui.hintSelect, C2D_WithColor, (320.0f-tw)*0.5f, 82, 0.5f, 0.50f, 0.50f, C2D_Color32(180,180,180,255));
        C2D_TextGetDimensions(&ui.hintSize, 0.50f, 0.50f, &tw, &th);
        C2D_DrawText(&ui.hintSize, C2D_WithColor, (320.0f-tw)*0.5f, 107, 0.5f, 0.50f, 0.50f, C2D_Color32(180,180,180,255));
        C2D_TextGetDimensions(&ui.hintStart, 0.50f, 0.50f, &tw, &th);
        C2D_DrawText(&ui.hintStart, C2D_WithColor, (320.0f-tw)*0.5f, 132, 0.5f, 0.50f, 0.50f, C2D_Color32(180,180,180,255));
        C2D_TextGetDimensions(&ui.hintOff, 0.50f, 0.50f, &tw, &th);
        C2D_DrawText(&ui.hintOff, C2D_WithColor, (320.0f-tw)*0.5f, 157, 0.5f, 0.50f, 0.50f, C2D_Color32(180,180,180,255));
        float bw, bh;
        C2D_DrawRectSolid(15, 185, 0.0f, 140, 32, colYellow);
        C2D_TextGetDimensions(&ui.btnCredits, 0.50f, 0.50f, &bw, &bh);
        C2D_DrawText(&ui.btnCredits, C2D_WithColor, 15+(140-bw)*0.5f, 185+(32-bh)*0.5f, 0.5f, 0.50f, 0.50f, colTextBlack);
        C2D_DrawRectSolid(165, 185, 0.0f, 140, 32, colYellow);
        C2D_TextGetDimensions(&ui.btnBack, 0.50f, 0.50f, &bw, &bh);
        C2D_DrawText(&ui.btnBack, C2D_WithColor, 165+(140-bw)*0.5f, 185+(32-bh)*0.5f, 0.5f, 0.50f, 0.50f, colTextBlack);
    }
    else if (currentScreen == SCREEN_DEBUG) {
        u32 bottomBgColor = C2D_Color32(22, 30, 20, 255);
        C2D_TargetClear(target, bottomBgColor);
        C2D_DrawRectSolid(10, 10,  0.0f, 300, 2, C2D_Color32(80, 50, 60, 255));
        C2D_DrawRectSolid(10, 228, 0.0f, 300, 2, C2D_Color32(80, 50, 60, 255));
        drawSlimShadowTitle("DEBUG", buf, 20, 20, 0.6f, C2D_Color32(200,200,200,255), C2D_Color32(10, 10, 10, 220));
        char fpsStr[16]; snprintf(fpsStr, sizeof(fpsStr), "%.0f fps", fpsDisplayValue);
        C2D_Text tFps; C2D_TextParse(&tFps, buf, fpsStr); C2D_TextOptimize(&tFps);
        float fpsw, fpsh;
        C2D_TextGetDimensions(&tFps, 0.5f, 0.5f, &fpsw, &fpsh);
        u32 fpsColor = (fpsDisplayValue >= 55.0f) ? C2D_Color32(90, 220, 100, 255)
                      : (fpsDisplayValue >= 40.0f) ? C2D_Color32(230, 200, 60, 255)
                      : C2D_Color32(230, 80, 60, 255);
        C2D_DrawText(&tFps, C2D_WithColor, 305.0f - fpsw, 20, 0.5f, 0.5f, 0.5f, fpsColor);
        {
            float minusX = 15.0f, dispX = 70.0f, plusX = 255.0f;
            float rowY = 55.0f, rowH2 = 32.0f;
            C2D_DrawRectSolid(minusX, rowY, 0.0f, 50.0f, rowH2, C2D_Color32(120, 150, 190, 255));
            char cloudMinusStr[2] = "-"; C2D_Text tCloudMinus; C2D_TextParse(&tCloudMinus, buf, cloudMinusStr); C2D_TextOptimize(&tCloudMinus);
            float cmw, cmh; C2D_TextGetDimensions(&tCloudMinus, 0.5f, 0.5f, &cmw, &cmh);
            C2D_DrawText(&tCloudMinus, C2D_WithColor, minusX+(50.0f-cmw)*0.5f, rowY+(rowH2-cmh)*0.5f, 0.5f, 0.5f, 0.5f, colTextWhite);

            C2D_DrawRectSolid(dispX, rowY, 0.0f, 180.0f, rowH2, C2D_Color32(45, 55, 45, 255));
            char cloudCountStr[24]; snprintf(cloudCountStr, sizeof(cloudCountStr), "Clouds: %d", activeCloudCount);
            C2D_Text tCloudCount; C2D_TextParse(&tCloudCount, buf, cloudCountStr); C2D_TextOptimize(&tCloudCount);
            float ccw, cch; C2D_TextGetDimensions(&tCloudCount, 0.42f, 0.42f, &ccw, &cch);
            C2D_DrawText(&tCloudCount, C2D_WithColor, dispX+(180.0f-ccw)*0.5f, rowY+(rowH2-cch)*0.5f, 0.5f, 0.42f, 0.42f, colWhite);

            C2D_DrawRectSolid(plusX, rowY, 0.0f, 50.0f, rowH2, C2D_Color32(120, 150, 190, 255));
            char cloudPlusStr[2] = "+"; C2D_Text tCloudPlus; C2D_TextParse(&tCloudPlus, buf, cloudPlusStr); C2D_TextOptimize(&tCloudPlus);
            float cpw, cph; C2D_TextGetDimensions(&tCloudPlus, 0.5f, 0.5f, &cpw, &cph);
            C2D_DrawText(&tCloudPlus, C2D_WithColor, plusX+(50.0f-cpw)*0.5f, rowY+(rowH2-cph)*0.5f, 0.5f, 0.5f, 0.5f, colTextWhite);
        }
        {
            float minusX = 15.0f, dispX = 70.0f, plusX = 255.0f;
            float rowY = 185.0f, rowH2 = 32.0f;
            C2D_DrawRectSolid(minusX, rowY, 0.0f, 50.0f, rowH2, C2D_Color32(200, 110, 70, 255));
            char foxAnimMinusStr[2] = "-"; C2D_Text tFoxAnimMinus; C2D_TextParse(&tFoxAnimMinus, buf, foxAnimMinusStr); C2D_TextOptimize(&tFoxAnimMinus);
            float famw, famh; C2D_TextGetDimensions(&tFoxAnimMinus, 0.5f, 0.5f, &famw, &famh);
            C2D_DrawText(&tFoxAnimMinus, C2D_WithColor, minusX+(50.0f-famw)*0.5f, rowY+(rowH2-famh)*0.5f, 0.5f, 0.5f, 0.5f, colTextWhite);

            C2D_DrawRectSolid(dispX, rowY, 0.0f, 180.0f, rowH2, C2D_Color32(45, 55, 45, 255));
            char foxAnimStr[24]; snprintf(foxAnimStr, sizeof(foxAnimStr), "Fox Anim: %.2fs", foxAnimFrameDuration);
            C2D_Text tFoxAnim; C2D_TextParse(&tFoxAnim, buf, foxAnimStr); C2D_TextOptimize(&tFoxAnim);
            float faw, fah; C2D_TextGetDimensions(&tFoxAnim, 0.38f, 0.38f, &faw, &fah);
            C2D_DrawText(&tFoxAnim, C2D_WithColor, dispX+(180.0f-faw)*0.5f, rowY+(rowH2-fah)*0.5f, 0.5f, 0.38f, 0.38f, colWhite);

            C2D_DrawRectSolid(plusX, rowY, 0.0f, 50.0f, rowH2, C2D_Color32(200, 110, 70, 255));
            char foxAnimPlusStr[2] = "+"; C2D_Text tFoxAnimPlus; C2D_TextParse(&tFoxAnimPlus, buf, foxAnimPlusStr); C2D_TextOptimize(&tFoxAnimPlus);
            float fapw, faph; C2D_TextGetDimensions(&tFoxAnimPlus, 0.5f, 0.5f, &fapw, &faph);
            C2D_DrawText(&tFoxAnimPlus, C2D_WithColor, plusX+(50.0f-fapw)*0.5f, rowY+(rowH2-faph)*0.5f, 0.5f, 0.5f, 0.5f, colTextWhite);
        }
        {
            float minusX = 15.0f, dispX = 70.0f, plusX = 255.0f;
            float rowY = 219.0f, rowH2 = 20.0f;
            C2D_DrawRectSolid(minusX, rowY, 0.0f, 50.0f, rowH2, C2D_Color32(200, 110, 70, 255));
            char foxMinusStr[2] = "-"; C2D_Text tFoxMinus; C2D_TextParse(&tFoxMinus, buf, foxMinusStr); C2D_TextOptimize(&tFoxMinus);
            float fmw, fmh; C2D_TextGetDimensions(&tFoxMinus, 0.42f, 0.42f, &fmw, &fmh);
            C2D_DrawText(&tFoxMinus, C2D_WithColor, minusX+(50.0f-fmw)*0.5f, rowY+(rowH2-fmh)*0.5f, 0.5f, 0.42f, 0.42f, colTextWhite);

            C2D_DrawRectSolid(dispX, rowY, 0.0f, 180.0f, rowH2, C2D_Color32(45, 55, 45, 255));
            char foxSpeedStr[24]; snprintf(foxSpeedStr, sizeof(foxSpeedStr), "Fox Speed: %.2f", foxSpeed);
            C2D_Text tFoxSpeed; C2D_TextParse(&tFoxSpeed, buf, foxSpeedStr); C2D_TextOptimize(&tFoxSpeed);
            float fsw, fsh; C2D_TextGetDimensions(&tFoxSpeed, 0.34f, 0.34f, &fsw, &fsh);
            C2D_DrawText(&tFoxSpeed, C2D_WithColor, dispX+(180.0f-fsw)*0.5f, rowY+(rowH2-fsh)*0.5f, 0.5f, 0.34f, 0.34f, colWhite);

            C2D_DrawRectSolid(plusX, rowY, 0.0f, 50.0f, rowH2, C2D_Color32(200, 110, 70, 255));
            char foxPlusStr[2] = "+"; C2D_Text tFoxPlus; C2D_TextParse(&tFoxPlus, buf, foxPlusStr); C2D_TextOptimize(&tFoxPlus);
            float fpw, fph; C2D_TextGetDimensions(&tFoxPlus, 0.42f, 0.42f, &fpw, &fph);
            C2D_DrawText(&tFoxPlus, C2D_WithColor, plusX+(50.0f-fpw)*0.5f, rowY+(rowH2-fph)*0.5f, 0.5f, 0.42f, 0.42f, colTextWhite);
        }
        float bw, bh;
        const float dbw = 68.0f, dbh = 32.0f, dbGap = 6.0f, dbY0 = 95.0f, dbY1 = 145.0f;
        float dbX = 15.0f;
        C2D_DrawRectSolid(dbX, dbY0, 0.0f, dbw, dbh, C2D_Color32(190, 165, 130, 255));
        char hedgehogBtnStr[8]; snprintf(hedgehogBtnStr, sizeof(hedgehogBtnStr), "Riccio");
        C2D_Text tHedgehogBtn; C2D_TextParse(&tHedgehogBtn, buf, hedgehogBtnStr); C2D_TextOptimize(&tHedgehogBtn);
        C2D_TextGetDimensions(&tHedgehogBtn, 0.36f, 0.36f, &bw, &bh);
        C2D_DrawText(&tHedgehogBtn, C2D_WithColor, dbX+(dbw-bw)*0.5f, dbY0+(dbh-bh)*0.5f, 0.5f, 0.36f, 0.36f, colTextBlack);
        dbX += dbw + dbGap;
        C2D_DrawRectSolid(dbX, dbY0, 0.0f, dbw, dbh, C2D_Color32(170, 190, 210, 255));
        char owlFlyBtnStr[8]; snprintf(owlFlyBtnStr, sizeof(owlFlyBtnStr), "Volo");
        C2D_Text tOwlFlyBtn; C2D_TextParse(&tOwlFlyBtn, buf, owlFlyBtnStr); C2D_TextOptimize(&tOwlFlyBtn);
        C2D_TextGetDimensions(&tOwlFlyBtn, 0.36f, 0.36f, &bw, &bh);
        C2D_DrawText(&tOwlFlyBtn, C2D_WithColor, dbX+(dbw-bw)*0.5f, dbY0+(dbh-bh)*0.5f, 0.5f, 0.36f, 0.36f, colTextBlack);
        dbX += dbw + dbGap;
        C2D_DrawRectSolid(dbX, dbY0, 0.0f, dbw, dbh, C2D_Color32(190, 200, 205, 255));
        char planeBtnStr[8]; snprintf(planeBtnStr, sizeof(planeBtnStr), "Aereo");
        C2D_Text tPlaneBtn; C2D_TextParse(&tPlaneBtn, buf, planeBtnStr); C2D_TextOptimize(&tPlaneBtn);
        C2D_TextGetDimensions(&tPlaneBtn, 0.36f, 0.36f, &bw, &bh);
        C2D_DrawText(&tPlaneBtn, C2D_WithColor, dbX+(dbw-bw)*0.5f, dbY0+(dbh-bh)*0.5f, 0.5f, 0.36f, 0.36f, colTextBlack);
        dbX += dbw + dbGap;
        C2D_DrawRectSolid(dbX, dbY0, 0.0f, dbw, dbh, C2D_Color32(120, 130, 180, 255));
        char fetchBtnStr[8]; snprintf(fetchBtnStr, sizeof(fetchBtnStr), "Fetch");
        C2D_Text tFetchBtn; C2D_TextParse(&tFetchBtn, buf, fetchBtnStr); C2D_TextOptimize(&tFetchBtn);
        C2D_TextGetDimensions(&tFetchBtn, 0.36f, 0.36f, &bw, &bh);
        C2D_DrawText(&tFetchBtn, C2D_WithColor, dbX+(dbw-bw)*0.5f, dbY0+(dbh-bh)*0.5f, 0.5f, 0.36f, 0.36f, colTextBlack);
        dbX = 15.0f;
        C2D_DrawRectSolid(dbX, dbY1, 0.0f, dbw, dbh, C2D_Color32(140, 170, 140, 255));
        char ptrObjBtnStr[8]; snprintf(ptrObjBtnStr, sizeof(ptrObjBtnStr), "PtrObj");
        C2D_Text tPtrObjBtn; C2D_TextParse(&tPtrObjBtn, buf, ptrObjBtnStr); C2D_TextOptimize(&tPtrObjBtn);
        C2D_TextGetDimensions(&tPtrObjBtn, 0.34f, 0.34f, &bw, &bh);
        C2D_DrawText(&tPtrObjBtn, C2D_WithColor, dbX+(dbw-bw)*0.5f, dbY1+(dbh-bh)*0.5f, 0.5f, 0.34f, 0.34f, colTextBlack);
        dbX += dbw + dbGap;
        C2D_DrawRectSolid(dbX, dbY1, 0.0f, dbw, dbh, debugPointerActive ? C2D_Color32(255, 100, 100, 255) : C2D_Color32(210, 210, 210, 255));
        char ptrBtnStr[8]; snprintf(ptrBtnStr, sizeof(ptrBtnStr), "Ptr");
        C2D_Text tPtrBtn; C2D_TextParse(&tPtrBtn, buf, ptrBtnStr); C2D_TextOptimize(&tPtrBtn);
        C2D_TextGetDimensions(&tPtrBtn, 0.36f, 0.36f, &bw, &bh);
        C2D_DrawText(&tPtrBtn, C2D_WithColor, dbX+(dbw-bw)*0.5f, dbY1+(dbh-bh)*0.5f, 0.5f, 0.36f, 0.36f, colTextBlack);
        dbX += dbw + dbGap;
        C2D_DrawRectSolid(dbX, dbY1, 0.0f, dbw, dbh, colYellow);
        C2D_TextGetDimensions(&ui.btnBack, 0.36f, 0.36f, &bw, &bh);
        C2D_DrawText(&ui.btnBack, C2D_WithColor, dbX+(dbw-bw)*0.5f, dbY1+(dbh-bh)*0.5f, 0.5f, 0.36f, 0.36f, colTextBlack);
        dbX += dbw + dbGap;
        C2D_DrawRectSolid(dbX, dbY1, 0.0f, dbw, dbh, C2D_Color32(210, 70, 70, 255));
        char resetBtnStr[8]; snprintf(resetBtnStr, sizeof(resetBtnStr), "Reset");
        C2D_Text tResetBtn; C2D_TextParse(&tResetBtn, buf, resetBtnStr); C2D_TextOptimize(&tResetBtn);
        C2D_TextGetDimensions(&tResetBtn, 0.36f, 0.36f, &bw, &bh);
        C2D_DrawText(&tResetBtn, C2D_WithColor, dbX+(dbw-bw)*0.5f, dbY1+(dbh-bh)*0.5f, 0.5f, 0.36f, 0.36f, colTextWhite);

        if (debugResetConfirmPending) {
            C2D_DrawRectSolid(0, 0, 0.8f, 320, 240, C2D_Color32(0, 0, 0, 200));
            char confirmStr[32]; snprintf(confirmStr, sizeof(confirmStr), "Reset EVERYTHING?");
            C2D_Text tConfirm; C2D_TextParse(&tConfirm, buf, confirmStr); C2D_TextOptimize(&tConfirm);
            float cw, ch;
            C2D_TextGetDimensions(&tConfirm, 0.55f, 0.55f, &cw, &ch);
            C2D_DrawText(&tConfirm, C2D_WithColor, (320.0f-cw)*0.5f, 80, 0.85f, 0.55f, 0.55f, C2D_Color32(255,255,255,255));
            char confirmSubStr[48]; snprintf(confirmSubStr, sizeof(confirmSubStr), "Deletes all saves. Cannot be undone.");
            C2D_Text tConfirmSub; C2D_TextParse(&tConfirmSub, buf, confirmSubStr); C2D_TextOptimize(&tConfirmSub);
            float csw, csh;
            C2D_TextGetDimensions(&tConfirmSub, 0.38f, 0.38f, &csw, &csh);
            C2D_DrawText(&tConfirmSub, C2D_WithColor, (320.0f-csw)*0.5f, 105, 0.85f, 0.38f, 0.38f, C2D_Color32(220,180,180,255));
            C2D_DrawRectSolid(70, 150, 0.85f, 80, 32, C2D_Color32(210, 70, 70, 255));
            char yesStr[8]; snprintf(yesStr, sizeof(yesStr), "Yes");
            C2D_Text tYes; C2D_TextParse(&tYes, buf, yesStr); C2D_TextOptimize(&tYes);
            float yw, yh;
            C2D_TextGetDimensions(&tYes, 0.5f, 0.5f, &yw, &yh);
            C2D_DrawText(&tYes, C2D_WithColor, 70+(80.0f-yw)*0.5f, 150+(32.0f-yh)*0.5f, 0.85f, 0.5f, 0.5f, colTextWhite);
            C2D_DrawRectSolid(170, 150, 0.85f, 80, 32, colYellow);
            char noStr[8]; snprintf(noStr, sizeof(noStr), "No");
            C2D_Text tNo; C2D_TextParse(&tNo, buf, noStr); C2D_TextOptimize(&tNo);
            float nw, nh;
            C2D_TextGetDimensions(&tNo, 0.5f, 0.5f, &nw, &nh);
            C2D_DrawText(&tNo, C2D_WithColor, 170+(80.0f-nw)*0.5f, 150+(32.0f-nh)*0.5f, 0.85f, 0.5f, 0.5f, colTextBlack);
        }
    }
    else if (currentScreen == SCREEN_WEATHER_DEBUG) {
        C2D_TargetClear(target, C2D_Color32(20, 26, 32, 255));
        C2D_DrawRectSolid(10, 10,  0.0f, 300, 2, C2D_Color32(120, 130, 180, 255));
        C2D_DrawRectSolid(10, 228, 0.0f, 300, 2, C2D_Color32(120, 130, 180, 255));
        drawSlimShadowTitle("WEATHER FETCH", buf, 20, 15, 0.55f, C2D_Color32(180,190,230,255), C2D_Color32(8, 8, 14, 220));

        float wxw, wxh;
        char wxLocStr[64]; snprintf(wxLocStr, sizeof(wxLocStr), "Location: %s", settings.location[0] ? settings.location : "Not set");
        C2D_Text tWxLoc; C2D_TextParse(&tWxLoc, buf, wxLocStr); C2D_TextOptimize(&tWxLoc);
        C2D_DrawText(&tWxLoc, C2D_WithColor, 20, 45, 0.5f, 0.40f, 0.40f, C2D_Color32(220,220,220,255));

        char wxStatusStr[64]; snprintf(wxStatusStr, sizeof(wxStatusStr), "Status: %s", weatherFetchStatus);
        C2D_Text tWxStatus; C2D_TextParse(&tWxStatus, buf, wxStatusStr); C2D_TextOptimize(&tWxStatus);
        C2D_DrawText(&tWxStatus, C2D_WithColor, 20, 68, 0.5f, 0.36f, 0.36f, C2D_Color32(200,180,140,255));

        char wxGeoStr[64];
        if (geoCacheValid) snprintf(wxGeoStr, sizeof(wxGeoStr), "Cached coords: %.4f, %.4f", geoCachedLat, geoCachedLon);
        else               snprintf(wxGeoStr, sizeof(wxGeoStr), "Cached coords: none yet");
        C2D_Text tWxGeo; C2D_TextParse(&tWxGeo, buf, wxGeoStr); C2D_TextOptimize(&tWxGeo);
        C2D_DrawText(&tWxGeo, C2D_WithColor, 20, 88, 0.5f, 0.36f, 0.36f, C2D_Color32(180,200,220,255));

        char wxCacheForStr[64]; snprintf(wxCacheForStr, sizeof(wxCacheForStr), "Cached for: %s", geoCachedLocation[0] ? geoCachedLocation : "(none)");
        C2D_Text tWxCacheFor; C2D_TextParse(&tWxCacheFor, buf, wxCacheForStr); C2D_TextOptimize(&tWxCacheFor);
        C2D_DrawText(&tWxCacheFor, C2D_WithColor, 20, 108, 0.5f, 0.36f, 0.36f, C2D_Color32(180,200,220,255));

        bool haveWeather = (strncmp(weatherFetchStatus, "OK:", 3) == 0);
        char wxStateStr[48]; snprintf(wxStateStr, sizeof(wxStateStr), "Current state: %s", haveWeather ? weatherStateLabel((int)realTimeWeatherType) : "-");
        C2D_Text tWxState; C2D_TextParse(&tWxState, buf, wxStateStr); C2D_TextOptimize(&tWxState);
        C2D_DrawText(&tWxState, C2D_WithColor, 20, 128, 0.5f, 0.40f, 0.40f, C2D_Color32(150,220,190,255));

        char wxTempStr[32];
        if (lastWeatherTemp < -900.0) snprintf(wxTempStr, sizeof(wxTempStr), "Temp: --");
        else snprintf(wxTempStr, sizeof(wxTempStr), "Temp: %.1fC", lastWeatherTemp);
        C2D_Text tWxTemp; C2D_TextParse(&tWxTemp, buf, wxTempStr); C2D_TextOptimize(&tWxTemp);
        C2D_DrawText(&tWxTemp, C2D_WithColor, 20, 150, 0.5f, 0.40f, 0.40f, C2D_Color32(220,210,180,255));

        char wxWifiStr[24]; snprintf(wxWifiStr, sizeof(wxWifiStr), "WiFi: %s", isWifiConnected() ? "Connected" : "Not connected");
        C2D_Text tWxWifi; C2D_TextParse(&tWxWifi, buf, wxWifiStr); C2D_TextOptimize(&tWxWifi);
        C2D_DrawText(&tWxWifi, C2D_WithColor, 20, 172, 0.5f, 0.36f, 0.36f, C2D_Color32(160,180,200,255));

        C2D_DrawRectSolid(15, 195, 0.0f, 140, 25, C2D_Color32(120, 130, 180, 255));
        char wxFetchBtnStr[16]; snprintf(wxFetchBtnStr, sizeof(wxFetchBtnStr), "Fetch Now");
        C2D_Text tWxFetchBtn; C2D_TextParse(&tWxFetchBtn, buf, wxFetchBtnStr); C2D_TextOptimize(&tWxFetchBtn);
        C2D_TextGetDimensions(&tWxFetchBtn, 0.40f, 0.40f, &wxw, &wxh);
        C2D_DrawText(&tWxFetchBtn, C2D_WithColor, 15+(140.0f-wxw)*0.5f, 195.0f+(25.0f-wxh)*0.5f, 0.5f, 0.40f, 0.40f, colTextWhite);

        C2D_DrawRectSolid(237, 195, 0.0f, 68, 25, colYellow);
        C2D_TextGetDimensions(&ui.btnBack, 0.40f, 0.40f, &wxw, &wxh);
        C2D_DrawText(&ui.btnBack, C2D_WithColor, 237+(68.0f-wxw)*0.5f, 195.0f+(25.0f-wxh)*0.5f, 0.5f, 0.40f, 0.40f, colTextBlack);
    }
    else if (currentScreen == SCREEN_CALENDAR) {
        C2D_TargetClear(target, C2D_Color32(22, 26, 34, 255));

        time_t rawCal = time(NULL);
        struct tm* tmCal = localtime(&rawCal);
        int realYear  = tmCal->tm_year + 1900;
        int realMonth = tmCal->tm_mon; // 0-11
        int realToday = tmCal->tm_mday;
        if (calDisplayMonth < 0) { calDisplayMonth = realMonth; calDisplayYear = realYear; } // primissimo avvio
        int calMonth = calDisplayMonth;
        int calYear  = calDisplayYear;

        // Riga in alto: ora reale sempre, ma il mese/anno sono quelli
        // visualizzati (possono differire da quelli reali se hai
        // navigato con le freccette)
        static const char* calMonthNames[12] = {"Jan","Feb","Mar","Apr","May","Jun","Jul","Aug","Sep","Oct","Nov","Dec"};
        char calHeaderStr[48];
        snprintf(calHeaderStr, sizeof(calHeaderStr), "%02d:%02d  %s %d",
                 tmCal->tm_hour, tmCal->tm_min, calMonthNames[calMonth], calYear);
        C2D_Text tCalHeader; C2D_TextParse(&tCalHeader, buf, calHeaderStr); C2D_TextOptimize(&tCalHeader);
        float chw, chh;
        C2D_TextGetDimensions(&tCalHeader, 0.52f, 0.52f, &chw, &chh);
        C2D_DrawText(&tCalHeader, C2D_WithColor, (320.0f-chw)*0.5f, 14, 0.5f, 0.52f, 0.52f, C2D_Color32(255,255,255,255));
        float headerCenterY = 14.0f + chh * 0.5f;

        // Freccette avanti/indietro mese, posizioni fisse coerenti con le
        // zone tattili (non dipendono dalla larghezza variabile del testo,
        // che cambia con la lunghezza del nome del mese)
        char calPrevStr[2] = "<"; C2D_Text tCalPrev; C2D_TextParse(&tCalPrev, buf, calPrevStr); C2D_TextOptimize(&tCalPrev);
        float cpw, cph; C2D_TextGetDimensions(&tCalPrev, 0.8f, 0.8f, &cpw, &cph);
        C2D_DrawText(&tCalPrev, C2D_WithColor, 92.0f - cpw*0.5f, headerCenterY - cph*0.5f, 0.5f, 0.8f, 0.8f, C2D_Color32(255,255,255,255));
        char calNextStr[2] = ">"; C2D_Text tCalNext; C2D_TextParse(&tCalNext, buf, calNextStr); C2D_TextOptimize(&tCalNext);
        float cnw, cnh; C2D_TextGetDimensions(&tCalNext, 0.8f, 0.8f, &cnw, &cnh);
        C2D_DrawText(&tCalNext, C2D_WithColor, 227.0f - cnw*0.5f, headerCenterY - cnh*0.5f, 0.5f, 0.8f, 0.8f, C2D_Color32(255,255,255,255));

        C2D_DrawRectSolid(15.0f, 12.0f, 0.0f, 60.0f, 20.0f, colYellow);
        char calSettingsStr[16]; snprintf(calSettingsStr, sizeof(calSettingsStr), "Settings");
        C2D_Text tCalSettings; C2D_TextParse(&tCalSettings, buf, calSettingsStr); C2D_TextOptimize(&tCalSettings);
        float cstw, csth;
        C2D_TextGetDimensions(&tCalSettings, 0.36f, 0.36f, &cstw, &csth);
        C2D_DrawText(&tCalSettings, C2D_WithColor, 15.0f+(60.0f-cstw)*0.5f, 12.0f+(20.0f-csth)*0.5f, 0.5f, 0.36f, 0.36f, colTextBlack);

        // Intestazione giorni della settimana, da Domenica
        static const char* calDayLettersSun[7] = {"S","M","T","W","T","F","S"};
        static const char* calDayLettersMon[7] = {"M","T","W","T","F","S","S"};
        const char** calDayLetters = settings.calFirstDayMonday ? calDayLettersMon : calDayLettersSun;
        const float calL = 15.0f, calR = 305.0f;
        const float colW = (calR - calL) / 7.0f;
        const float dayLabelY = 34.0f;
        for (int c = 0; c < 7; c++) {
            char dl[2]; snprintf(dl, sizeof(dl), "%s", calDayLetters[c]);
            C2D_Text tDl; C2D_TextParse(&tDl, buf, dl); C2D_TextOptimize(&tDl);
            float dlw, dlh;
            C2D_TextGetDimensions(&tDl, 0.42f, 0.42f, &dlw, &dlh);
            float cellCenterX = calL + colW * c + colW * 0.5f;
            C2D_DrawText(&tDl, C2D_WithColor, cellCenterX - dlw*0.5f, dayLabelY, 0.5f, 0.42f, 0.42f, C2D_Color32(150,180,220,255));
        }

        // Griglia 6x7 dei numeri dei giorni
        int daysInMonth   = getDaysInMonth(calMonth, calYear);
        int firstWeekdayRaw = getFirstWeekdayOfMonth(calMonth, calYear); // 0=domenica
        int firstWeekday = settings.calFirstDayMonday ? (firstWeekdayRaw + 6) % 7 : firstWeekdayRaw;
        const float gridTopY = 54.0f, gridBottomY = 222.0f;
        const float rowH = (gridBottomY - gridTopY) / 6.0f;

        // Linee della griglia: 8 verticali (bordi delle 7 colonne) e 7
        // orizzontali (bordi delle 6 righe), disegnate prima dei numeri
        // così restano sotto.
        u32 gridLineColor = C2D_Color32(60, 70, 90, 255);
        for (int c = 0; c <= 7; c++) {
            float lineX = calL + colW * c;
            C2D_DrawRectSolid(lineX, gridTopY, 0.0f, 1.0f, gridBottomY - gridTopY, gridLineColor);
        }
        for (int r = 0; r <= 6; r++) {
            float lineY = gridTopY + rowH * r;
            C2D_DrawRectSolid(calL, lineY, 0.0f, calR - calL, 1.0f, gridLineColor);
        }

        bool isCurrentMonthShown = (calMonth == realMonth && calYear == realYear);
        for (int row = 0; row < 6; row++) {
            for (int col = 0; col < 7; col++) {
                int dayNum = row * 7 + col - firstWeekday + 1;
                if (dayNum < 1 || dayNum > daysInMonth) continue; // fuori dal mese corrente, cella vuota
                float cellX = calL + colW * col;
                float cellY = gridTopY + rowH * row;
                bool isToday = isCurrentMonthShown && (dayNum == realToday);
                if (isToday) {
                    C2D_DrawRectSolid(cellX + 3, cellY + 2, 0.0f, colW - 6, rowH - 4, C2D_Color32(90, 130, 190, 255));
                }
                char dayStr[12]; snprintf(dayStr, sizeof(dayStr), "%d", dayNum);
                C2D_Text tDay; C2D_TextParse(&tDay, buf, dayStr); C2D_TextOptimize(&tDay);
                float dw, dh;
                C2D_TextGetDimensions(&tDay, 0.40f, 0.40f, &dw, &dh);
                float cellCenterX = cellX + colW * 0.5f;
                float cellCenterY = cellY + rowH * 0.5f;
                u32 dayColor = isToday ? C2D_Color32(255,255,255,255) : C2D_Color32(200,205,215,255);
                C2D_DrawText(&tDay, C2D_WithColor, cellCenterX - dw*0.5f, cellCenterY - dh*0.5f, 0.5f, 0.40f, 0.40f, dayColor);
                if (findDiaryEntry(calYear, calMonth, dayNum) >= 0) {
                    C2D_DrawRectSolid(cellCenterX - 2.0f, cellY + rowH - 8.0f, 0.0f, 4.0f, 4.0f, C2D_Color32(230, 190, 90, 255));
                }
            }
        }

        C2D_DrawRectSolid(245.0f, 12.0f, 0.0f, 60.0f, 20.0f, colYellow);
        char calBackStr[8]; snprintf(calBackStr, sizeof(calBackStr), "Back");
        C2D_Text tCalBack; C2D_TextParse(&tCalBack, buf, calBackStr); C2D_TextOptimize(&tCalBack);
        float cbw, cbh;
        C2D_TextGetDimensions(&tCalBack, 0.40f, 0.40f, &cbw, &cbh);
        C2D_DrawText(&tCalBack, C2D_WithColor, 245.0f+(60.0f-cbw)*0.5f, 12.0f+(20.0f-cbh)*0.5f, 0.5f, 0.40f, 0.40f, colTextBlack);
    }
    else if (currentScreen == SCREEN_CALENDAR_SETTINGS) {
        C2D_TargetClear(target, C2D_Color32(22, 26, 34, 255));
        C2D_DrawRectSolid(10, 10,  0.0f, 300, 2, C2D_Color32(90, 110, 150, 255));
        C2D_DrawRectSolid(10, 228, 0.0f, 300, 2, C2D_Color32(90, 110, 150, 255));
        drawSlimShadowTitle("CALENDAR SETTINGS", buf, 20, 15, 0.6f, C2D_Color32(150,180,220,255), C2D_Color32(8, 10, 16, 220));

        C2D_DrawRectSolid(15, 55, 0.0f, 290, 32, C2D_Color32(45, 55, 65, 255));
        char fdwLabelStr[24]; snprintf(fdwLabelStr, sizeof(fdwLabelStr), "First Day of Week:");
        C2D_Text tFdwLabel; C2D_TextParse(&tFdwLabel, buf, fdwLabelStr); C2D_TextOptimize(&tFdwLabel);
        float fdwlw, fdwlh;
        C2D_TextGetDimensions(&tFdwLabel, 0.44f, 0.44f, &fdwlw, &fdwlh);
        C2D_DrawText(&tFdwLabel, C2D_WithColor, 25, 55+(32-fdwlh)*0.5f, 0.5f, 0.44f, 0.44f, colWhite);
        char fdwValueStr[16]; snprintf(fdwValueStr, sizeof(fdwValueStr), settings.calFirstDayMonday ? "Monday" : "Sunday");
        C2D_Text tFdwValue; C2D_TextParse(&tFdwValue, buf, fdwValueStr); C2D_TextOptimize(&tFdwValue);
        float fdwvw, fdwvh;
        C2D_TextGetDimensions(&tFdwValue, 0.44f, 0.44f, &fdwvw, &fdwvh);
        C2D_DrawText(&tFdwValue, C2D_WithColor, 295.0f-fdwvw, 55+(32-fdwvh)*0.5f, 0.5f, 0.44f, 0.44f, C2D_Color32(150,220,190,255));

        float csbw, csbh;
        C2D_DrawRectSolid(15, 195, 0.0f, 140, 25, colYellow);
        char csBackStr[8]; snprintf(csBackStr, sizeof(csBackStr), "Back");
        C2D_Text tCsBack; C2D_TextParse(&tCsBack, buf, csBackStr); C2D_TextOptimize(&tCsBack);
        C2D_TextGetDimensions(&tCsBack, 0.44f, 0.44f, &csbw, &csbh);
        C2D_DrawText(&tCsBack, C2D_WithColor, 15+(140.0f-csbw)*0.5f, 195.0f+(25.0f-csbh)*0.5f, 0.5f, 0.44f, 0.44f, colTextBlack);
    }
    else if (currentScreen == SCREEN_DIARY_ENTRY) {
        C2D_TargetClear(target, C2D_Color32(24, 28, 22, 255));

        static const char* deMonthNames[12] = {"Jan","Feb","Mar","Apr","May","Jun","Jul","Aug","Sep","Oct","Nov","Dec"};
        char deTitleStr[32];
        snprintf(deTitleStr, sizeof(deTitleStr), "%d %s %d", diaryViewDay, deMonthNames[diaryViewMonth >= 0 ? diaryViewMonth : 0], diaryViewYear);
        C2D_Text tDeTitle; C2D_TextParse(&tDeTitle, buf, deTitleStr); C2D_TextOptimize(&tDeTitle);
        C2D_DrawText(&tDeTitle, C2D_WithColor, 20, 15, 0.5f, 0.6f, 0.6f, C2D_Color32(220,200,150,255));

        char dePrevStr[2] = "<"; C2D_Text tDePrev; C2D_TextParse(&tDePrev, buf, dePrevStr); C2D_TextOptimize(&tDePrev);
        float dpw, dph; C2D_TextGetDimensions(&tDePrev, 0.6f, 0.6f, &dpw, &dph);
        C2D_DrawText(&tDePrev, C2D_WithColor, 250.0f - dpw*0.5f, 12, 0.5f, 0.6f, 0.6f, C2D_Color32(220,200,150,255));
        char deNextStr[2] = ">"; C2D_Text tDeNext; C2D_TextParse(&tDeNext, buf, deNextStr); C2D_TextOptimize(&tDeNext);
        float dnw, dnh; C2D_TextGetDimensions(&tDeNext, 0.6f, 0.6f, &dnw, &dnh);
        C2D_DrawText(&tDeNext, C2D_WithColor, 288.0f - dnw*0.5f, 12, 0.5f, 0.6f, 0.6f, C2D_Color32(220,200,150,255));

        int deIdx = findDiaryEntry(diaryViewYear, diaryViewMonth, diaryViewDay);
        C2D_DrawRectSolid(15, 50, 0.0f, 290, 130, C2D_Color32(40, 46, 38, 255));
        char deTextStr[DIARY_TEXT_MAX + 8];
        if (deIdx >= 0 && diaryEntries[deIdx].text[0]) {
            snprintf(deTextStr, sizeof(deTextStr), "%s", diaryEntries[deIdx].text);
        } else {
            deTextStr[0] = '\0';
        }
        {
            static char deLines[WRAP_MAX_LINES][WRAP_LINE_LEN];
            int deLineCount = wrapTextIntoLines(deTextStr, 270.0f, 0.38f, buf, deLines);
            diaryTotalLines = deLineCount; // letto dall'input per limitare lo scroll
            float deLineY = 58.0f;
            for (int li = diaryScrollOffset; li < deLineCount && li < diaryScrollOffset + DIARY_VISIBLE_LINES; li++) {
                C2D_Text tDeLine; C2D_TextParse(&tDeLine, buf, deLines[li]); C2D_TextOptimize(&tDeLine);
                C2D_DrawText(&tDeLine, C2D_WithColor, 22, deLineY, 0.5f, 0.38f, 0.38f, C2D_Color32(210,210,200,255));
                deLineY += 15.0f;
            }
            if (deLineCount > DIARY_VISIBLE_LINES) {
                bool arrowVisible = fmodf(globalAnimTime, 1.0f) < 0.6f; // lampeggia
                if (arrowVisible) {
                    char scrollArrowStr[2] = "v";
                    C2D_Text tScrollArrow; C2D_TextParse(&tScrollArrow, buf, scrollArrowStr); C2D_TextOptimize(&tScrollArrow);
                    C2D_DrawText(&tScrollArrow, C2D_WithColor, 288, 181, 0.5f, 0.42f, 0.42f, C2D_Color32(230,200,120,255));
                }
            }
        }

        float debw, debh;
        C2D_DrawRectSolid(15, 195, 0.0f, 130, 25, colYellow);
        char deWriteStr[16]; snprintf(deWriteStr, sizeof(deWriteStr), "Write");
        C2D_Text tDeWrite; C2D_TextParse(&tDeWrite, buf, deWriteStr); C2D_TextOptimize(&tDeWrite);
        C2D_TextGetDimensions(&tDeWrite, 0.44f, 0.44f, &debw, &debh);
        C2D_DrawText(&tDeWrite, C2D_WithColor, 15+(130.0f-debw)*0.5f, 195.0f+(25.0f-debh)*0.5f, 0.5f, 0.44f, 0.44f, colTextBlack);

        C2D_DrawRectSolid(175, 195, 0.0f, 130, 25, colYellow);
        char deBackStr[8]; snprintf(deBackStr, sizeof(deBackStr), "Back");
        C2D_Text tDeBack; C2D_TextParse(&tDeBack, buf, deBackStr); C2D_TextOptimize(&tDeBack);
        C2D_TextGetDimensions(&tDeBack, 0.44f, 0.44f, &debw, &debh);
        C2D_DrawText(&tDeBack, C2D_WithColor, 175+(130.0f-debw)*0.5f, 195.0f+(25.0f-debh)*0.5f, 0.5f, 0.44f, 0.44f, colTextBlack);
    }
    else if (currentScreen == SCREEN_CALENDAR_MONTHYEAR) {
        C2D_TargetClear(target, C2D_Color32(22, 26, 34, 255));
        drawSlimShadowTitle("JUMP TO", buf, 20, 15, 0.55f, C2D_Color32(150,180,220,255), C2D_Color32(8, 10, 16, 220));

        const float fieldW = 100.0f, fieldGap = 10.0f;
        const float totalW = fieldW*2 + fieldGap;
        const float startX = 15.0f + (290.0f - totalW) * 0.5f;
        float monthFieldX = startX;
        float yearFieldX  = monthFieldX + fieldW + fieldGap;

        static const char* cmyMonthNames[12] = {"January","February","March","April","May","June","July","August","September","October","November","December"};
        float cmytw, cmyth;

        C2D_DrawRectSolid(monthFieldX, 84, 0.0f, fieldW, 50, C2D_Color32(45, 55, 65, 255));
        char cmyMonthStr[16]; snprintf(cmyMonthStr, sizeof(cmyMonthStr), "%s", cmyMonthNames[calEditMonth]);
        C2D_Text tCmyMonth; C2D_TextParse(&tCmyMonth, buf, cmyMonthStr); C2D_TextOptimize(&tCmyMonth);
        C2D_TextGetDimensions(&tCmyMonth, 0.46f, 0.46f, &cmytw, &cmyth);
        C2D_DrawText(&tCmyMonth, C2D_WithColor, monthFieldX+(fieldW-cmytw)*0.5f, 84+(50.0f-cmyth)*0.5f, 0.5f, 0.46f, 0.46f, colTextWhite);
        drawStepperButtons(monthFieldX, fieldW, 60.0f, 136.0f, 22.0f, buf, colTextWhite);

        C2D_DrawRectSolid(yearFieldX, 84, 0.0f, fieldW, 50, C2D_Color32(45, 55, 65, 255));
        char cmyYearStr[8]; snprintf(cmyYearStr, sizeof(cmyYearStr), "%d", calEditYear);
        C2D_Text tCmyYear; C2D_TextParse(&tCmyYear, buf, cmyYearStr); C2D_TextOptimize(&tCmyYear);
        C2D_TextGetDimensions(&tCmyYear, 0.56f, 0.56f, &cmytw, &cmyth);
        C2D_DrawText(&tCmyYear, C2D_WithColor, yearFieldX+(fieldW-cmytw)*0.5f, 84+(50.0f-cmyth)*0.5f, 0.5f, 0.56f, 0.56f, colTextWhite);
        drawStepperButtons(yearFieldX, fieldW, 60.0f, 136.0f, 22.0f, buf, colTextWhite);

        float cmybw, cmybh;
        C2D_DrawRectSolid(15, 195, 0.0f, 90, 25, colYellow);
        char cmyGoStr[8]; snprintf(cmyGoStr, sizeof(cmyGoStr), "Go");
        C2D_Text tCmyGo; C2D_TextParse(&tCmyGo, buf, cmyGoStr); C2D_TextOptimize(&tCmyGo);
        C2D_TextGetDimensions(&tCmyGo, 0.55f, 0.55f, &cmybw, &cmybh);
        C2D_DrawText(&tCmyGo, C2D_WithColor, 15+(90.0f-cmybw)*0.5f, 195.0f+(25.0f-cmybh)*0.5f, 0.5f, 0.55f, 0.55f, colTextBlack);

        C2D_DrawRectSolid(115, 195, 0.0f, 90, 25, colYellow);
        char cmyNowStr[8]; snprintf(cmyNowStr, sizeof(cmyNowStr), "Now");
        C2D_Text tCmyNow; C2D_TextParse(&tCmyNow, buf, cmyNowStr); C2D_TextOptimize(&tCmyNow);
        C2D_TextGetDimensions(&tCmyNow, 0.55f, 0.55f, &cmybw, &cmybh);
        C2D_DrawText(&tCmyNow, C2D_WithColor, 115+(90.0f-cmybw)*0.5f, 195.0f+(25.0f-cmybh)*0.5f, 0.5f, 0.55f, 0.55f, colTextBlack);

        C2D_DrawRectSolid(215, 195, 0.0f, 90, 25, colYellow);
        char cmyCancelStr[8]; snprintf(cmyCancelStr, sizeof(cmyCancelStr), "Back");
        C2D_Text tCmyCancel; C2D_TextParse(&tCmyCancel, buf, cmyCancelStr); C2D_TextOptimize(&tCmyCancel);
        C2D_TextGetDimensions(&tCmyCancel, 0.55f, 0.55f, &cmybw, &cmybh);
        C2D_DrawText(&tCmyCancel, C2D_WithColor, 215+(90.0f-cmybw)*0.5f, 195.0f+(25.0f-cmybh)*0.5f, 0.5f, 0.55f, 0.55f, colTextBlack);
    }
    else if (currentScreen == SCREEN_NOTES_LIST) {
        C2D_TargetClear(target, C2D_Color32(26, 24, 30, 255));
        drawSlimShadowTitle("NOTES", buf, 20, 12, 0.6f, C2D_Color32(200,180,220,255), C2D_Color32(10, 8, 14, 220));

        float nlnw, nlnh;
        C2D_DrawRectSolid(165, 8, 0.0f, 70, 24, colYellow);
        char nlNewStr[8]; snprintf(nlNewStr, sizeof(nlNewStr), "New");
        C2D_Text tNlNew; C2D_TextParse(&tNlNew, buf, nlNewStr); C2D_TextOptimize(&tNlNew);
        C2D_TextGetDimensions(&tNlNew, 0.55f, 0.55f, &nlnw, &nlnh);
        C2D_DrawText(&tNlNew, C2D_WithColor, 165+(70.0f-nlnw)*0.5f, 8+(24.0f-nlnh)*0.5f, 0.5f, 0.55f, 0.55f, colTextBlack);

        C2D_DrawRectSolid(240, 8, 0.0f, 65, 24, colYellow);
        char nlBackStr[8]; snprintf(nlBackStr, sizeof(nlBackStr), "Back");
        C2D_Text tNlBack; C2D_TextParse(&tNlBack, buf, nlBackStr); C2D_TextOptimize(&tNlBack);
        float nlbw, nlbh;
        C2D_TextGetDimensions(&tNlBack, 0.55f, 0.55f, &nlbw, &nlbh);
        C2D_DrawText(&tNlBack, C2D_WithColor, 240+(65.0f-nlbw)*0.5f, 8+(24.0f-nlbh)*0.5f, 0.5f, 0.55f, 0.55f, colTextBlack);

        // Elenco a righe separate da uno spazio vero (non più
        // sovrapposte) — più affidabile da rendere pulito che una vera
        // pila con sovrapposizione, che tendeva a confondersi quando i
        // colori tra le card erano simili.
        static int nlOrder[NOTE_MAX_ENTRIES];
        int nlCount = getNotesOrderedByModified(nlOrder, NOTE_MAX_ENTRIES);
        const float nlCardX = 20.0f, nlCardW = 280.0f, nlCardH = 32.0f, nlCardGap = 8.0f;
        const float nlCardStep = nlCardH + nlCardGap;
        const float nlListTopY = 42.0f;
        static float nlCardTouchY0[NOTE_MAX_ENTRIES];
        static int nlCardTouchIdx[NOTE_MAX_ENTRIES];
        int nlMaxScroll = (nlCount > NOTES_LIST_VISIBLE_COUNT) ? (nlCount - NOTES_LIST_VISIBLE_COUNT) : 0;
        if (notesListScrollOffset > nlMaxScroll) notesListScrollOffset = nlMaxScroll; // clamp difensivo (es. dopo una cancellazione)
        if (notesListScrollOffset < 0) notesListScrollOffset = 0;
        int nlVisibleCount = 0;
        for (int ni = notesListScrollOffset; ni < nlCount && nlVisibleCount < NOTES_LIST_VISIBLE_COUNT; ni++) {
            float cardY = nlListTopY + nlVisibleCount * nlCardStep;
            nlCardTouchY0[nlVisibleCount] = cardY;
            nlCardTouchIdx[nlVisibleCount] = nlOrder[ni];
            nlVisibleCount++;
        }
        for (int ni = 0; ni < nlVisibleCount; ni++) {
            int idx = nlCardTouchIdx[ni];
            float cardY = nlCardTouchY0[ni];
            bool isSelected = (notesListScrollOffset + ni == notesListSelectedIndex);
            u32 cardColor = isSelected ? C2D_Color32(95, 80, 115, 255) : C2D_Color32(48, 44, 58, 255);
            C2D_DrawRectSolid(nlCardX, cardY, 0.0f, nlCardW, nlCardH, cardColor);
            char nlTitleLine[48];
            getNoteTitleLine(idx, nlTitleLine, sizeof(nlTitleLine));
            C2D_Text tNlLine; C2D_TextParse(&tNlLine, buf, nlTitleLine); C2D_TextOptimize(&tNlLine);
            float nltw, nlth;
            C2D_TextGetDimensions(&tNlLine, 0.38f, 0.38f, &nltw, &nlth);
            C2D_DrawText(&tNlLine, C2D_WithColor, nlCardX + 10.0f, cardY + (nlCardH - nlth) * 0.5f, 0.5f, 0.38f, 0.38f, colTextWhite);
        }
        if (nlCount > notesListScrollOffset + NOTES_LIST_VISIBLE_COUNT) {
            bool nlArrowVisible = fmodf(globalAnimTime, 1.0f) < 0.6f;
            if (nlArrowVisible) {
                char nlScrollArrowStr[2] = "v";
                C2D_Text tNlScrollArrow; C2D_TextParse(&tNlScrollArrow, buf, nlScrollArrowStr); C2D_TextOptimize(&tNlScrollArrow);
                C2D_DrawText(&tNlScrollArrow, C2D_WithColor, 288, 207, 0.5f, 0.42f, 0.42f, colYellow);
            }
        }
        if (nlCount == 0) {
            char nlEmptyStr[32]; snprintf(nlEmptyStr, sizeof(nlEmptyStr), "No notes yet. Tap New.");
            C2D_Text tNlEmpty; C2D_TextParse(&tNlEmpty, buf, nlEmptyStr); C2D_TextOptimize(&tNlEmpty);
            C2D_DrawText(&tNlEmpty, C2D_WithColor, 20, 55, 0.5f, 0.40f, 0.40f, C2D_Color32(180,180,190,255));
        }
    }
    else if (currentScreen == SCREEN_NOTE_VIEW) {
        C2D_TargetClear(target, C2D_Color32(26, 24, 30, 255));

        char nvTitleLine[48];
        getNoteTitleLine(noteViewIdx, nvTitleLine, sizeof(nvTitleLine));
        C2D_Text tNvTitle; C2D_TextParse(&tNvTitle, buf, nvTitleLine); C2D_TextOptimize(&tNvTitle);
        C2D_DrawText(&tNvTitle, C2D_WithColor, 20, 12, 0.5f, 0.5f, 0.5f, C2D_Color32(220,200,150,255));

        C2D_DrawRectSolid(15, 42, 0.0f, 290, 138, C2D_Color32(40, 38, 46, 255));
        char nvTextStr[NOTE_TEXT_MAX + 8];
        if (noteViewIdx >= 0 && notes[noteViewIdx].text[0]) {
            snprintf(nvTextStr, sizeof(nvTextStr), "%s", notes[noteViewIdx].text);
        } else {
            snprintf(nvTextStr, sizeof(nvTextStr), "Empty. Tap here to write.");
        }
        {
            static char nvLines[WRAP_MAX_LINES][WRAP_LINE_LEN];
            int nvLineCount = wrapTextIntoLines(nvTextStr, 270.0f, 0.38f, buf, nvLines);
            noteTotalLines = nvLineCount;
            float nvLineY = 50.0f;
            for (int li = noteScrollOffset; li < nvLineCount && li < noteScrollOffset + NOTE_VISIBLE_LINES; li++) {
                C2D_Text tNvLine; C2D_TextParse(&tNvLine, buf, nvLines[li]); C2D_TextOptimize(&tNvLine);
                C2D_DrawText(&tNvLine, C2D_WithColor, 22, nvLineY, 0.5f, 0.38f, 0.38f, C2D_Color32(215,210,220,255));
                nvLineY += 15.0f;
            }
            if (nvLineCount > NOTE_VISIBLE_LINES) {
                bool nvArrowVisible = fmodf(globalAnimTime, 1.0f) < 0.6f;
                if (nvArrowVisible) {
                    char nvScrollArrowStr[2] = "v";
                    C2D_Text tNvScrollArrow; C2D_TextParse(&tNvScrollArrow, buf, nvScrollArrowStr); C2D_TextOptimize(&tNvScrollArrow);
                    C2D_DrawText(&tNvScrollArrow, C2D_WithColor, 288, 177, 0.5f, 0.42f, 0.42f, C2D_Color32(230,200,120,255));
                }
            }
        }

        float nvbw, nvbh;
        C2D_DrawRectSolid(15, 195, 0.0f, 90, 25, colYellow);
        char nvWriteStr[8]; snprintf(nvWriteStr, sizeof(nvWriteStr), "Write");
        C2D_Text tNvWrite; C2D_TextParse(&tNvWrite, buf, nvWriteStr); C2D_TextOptimize(&tNvWrite);
        C2D_TextGetDimensions(&tNvWrite, 0.42f, 0.42f, &nvbw, &nvbh);
        C2D_DrawText(&tNvWrite, C2D_WithColor, 15+(90.0f-nvbw)*0.5f, 195.0f+(25.0f-nvbh)*0.5f, 0.5f, 0.42f, 0.42f, colTextBlack);

        C2D_DrawRectSolid(115, 195, 0.0f, 90, 25, C2D_Color32(210, 70, 70, 255));
        char nvDeleteStr[8]; snprintf(nvDeleteStr, sizeof(nvDeleteStr), "Delete");
        C2D_Text tNvDelete; C2D_TextParse(&tNvDelete, buf, nvDeleteStr); C2D_TextOptimize(&tNvDelete);
        C2D_TextGetDimensions(&tNvDelete, 0.42f, 0.42f, &nvbw, &nvbh);
        C2D_DrawText(&tNvDelete, C2D_WithColor, 115+(90.0f-nvbw)*0.5f, 195.0f+(25.0f-nvbh)*0.5f, 0.5f, 0.42f, 0.42f, colTextWhite);

        C2D_DrawRectSolid(215, 195, 0.0f, 90, 25, colYellow);
        char nvBackStr[8]; snprintf(nvBackStr, sizeof(nvBackStr), "Back");
        C2D_Text tNvBack; C2D_TextParse(&tNvBack, buf, nvBackStr); C2D_TextOptimize(&tNvBack);
        C2D_TextGetDimensions(&tNvBack, 0.42f, 0.42f, &nvbw, &nvbh);
        C2D_DrawText(&tNvBack, C2D_WithColor, 215+(90.0f-nvbw)*0.5f, 195.0f+(25.0f-nvbh)*0.5f, 0.5f, 0.42f, 0.42f, colTextBlack);

        if (noteDeleteConfirmPending) {
            C2D_DrawRectSolid(0, 0, 0.8f, 320, 240, C2D_Color32(0, 0, 0, 200));
            char nvConfirmStr[24]; snprintf(nvConfirmStr, sizeof(nvConfirmStr), "Delete this note?");
            C2D_Text tNvConfirm; C2D_TextParse(&tNvConfirm, buf, nvConfirmStr); C2D_TextOptimize(&tNvConfirm);
            float ncw, nch;
            C2D_TextGetDimensions(&tNvConfirm, 0.55f, 0.55f, &ncw, &nch);
            C2D_DrawText(&tNvConfirm, C2D_WithColor, (320.0f-ncw)*0.5f, 90, 0.85f, 0.55f, 0.55f, C2D_Color32(255,255,255,255));
            C2D_DrawRectSolid(70, 140, 0.85f, 80, 32, C2D_Color32(210, 70, 70, 255));
            char nvYesStr[8]; snprintf(nvYesStr, sizeof(nvYesStr), "Yes");
            C2D_Text tNvYes; C2D_TextParse(&tNvYes, buf, nvYesStr); C2D_TextOptimize(&tNvYes);
            float nyw, nyh;
            C2D_TextGetDimensions(&tNvYes, 0.5f, 0.5f, &nyw, &nyh);
            C2D_DrawText(&tNvYes, C2D_WithColor, 70+(80.0f-nyw)*0.5f, 140+(32.0f-nyh)*0.5f, 0.85f, 0.5f, 0.5f, colTextWhite);
            C2D_DrawRectSolid(170, 140, 0.85f, 80, 32, colYellow);
            char nvNoStr[8]; snprintf(nvNoStr, sizeof(nvNoStr), "No");
            C2D_Text tNvNo; C2D_TextParse(&tNvNo, buf, nvNoStr); C2D_TextOptimize(&tNvNo);
            float nnw, nnh;
            C2D_TextGetDimensions(&tNvNo, 0.5f, 0.5f, &nnw, &nnh);
            C2D_DrawText(&tNvNo, C2D_WithColor, 170+(80.0f-nnw)*0.5f, 140+(32.0f-nnh)*0.5f, 0.85f, 0.5f, 0.5f, colTextBlack);
        }
    }
    else if (currentScreen == SCREEN_ENVIRONMENT) {
        C2D_TargetClear(target, C2D_Color32(22, 30, 20, 255));
        C2D_DrawRectSolid(10, 10,  0.0f, 300, 2, C2D_Color32(80, 50, 60, 255));
        C2D_DrawRectSolid(10, 228, 0.0f, 300, 2, C2D_Color32(80, 50, 60, 255));
        drawSlimShadowTitle("ENVIRONMENT", buf, 20, 15, 0.6f, C2D_Color32(150,220,190,255), C2D_Color32(8, 12, 6, 220));
        const float envL = 15.0f, envR = 305.0f, envW = envR - envL;
        const float envGap = 4.0f, rowH = 19.0f, rowStep = rowH + envGap;
        const float rowSeasonSelY = 40.0f, rowSeasonCycleY = rowSeasonSelY + rowStep, row0Y = rowSeasonCycleY + rowStep, row1Y = row0Y + rowStep, row2Y = row1Y + rowStep, row3Y = row2Y + rowStep, row4Y = row3Y + rowStep, row5Y = row4Y + rowStep;
        const float w5 = (envW - 4.0f * envGap) / 5.0f;
        const float w3 = (envW - 2.0f * envGap) / 3.0f;
        float pw, ph;
        float px0 = envL, px1 = envL + (w3 + envGap), px2 = envL + 2.0f * (w3 + envGap);

        // Riga: selezione stagione (Auto/Spring/Summer/Autumn/Winter)
        for (u32 s = 0; s < 5; s++) {
            static const char* envSeasonLabels[5] = {"Auto", "Spring", "Summer", "Autumn", "Winter"};
            float sx = envL + (float)s * (w5 + envGap);
            bool active = (settings.envSeasonMode == s);
            C2D_DrawRectSolid(sx, rowSeasonSelY, 0.0f, w5, rowH, active ? C2D_Color32(70, 210, 130, 255) : colYellow);
            C2D_Text tS; C2D_TextParse(&tS, buf, envSeasonLabels[s]); C2D_TextOptimize(&tS);
            C2D_TextGetDimensions(&tS, 0.40f, 0.40f, &pw, &ph);
            C2D_DrawText(&tS, C2D_WithColor, sx+(w5-pw)*0.5f, rowSeasonSelY+(rowH-ph)*0.5f, 0.5f, 0.40f, 0.40f, colTextBlack);
        }
        // Riga: Cycle / Random / intervallo — stagione
        C2D_DrawRectSolid(px0, rowSeasonCycleY, 0.0f, w3, rowH, (settings.envSeasonMode == 5) ? C2D_Color32(70, 210, 130, 255) : colYellow);
        char sCycleStr[8]; snprintf(sCycleStr, sizeof(sCycleStr), "Cycle");
        C2D_Text tSCycle; C2D_TextParse(&tSCycle, buf, sCycleStr); C2D_TextOptimize(&tSCycle);
        C2D_TextGetDimensions(&tSCycle, 0.40f, 0.40f, &pw, &ph);
        C2D_DrawText(&tSCycle, C2D_WithColor, px0+(w3-pw)*0.5f, rowSeasonCycleY+(rowH-ph)*0.5f, 0.5f, 0.40f, 0.40f, colTextBlack);
        C2D_DrawRectSolid(px1, rowSeasonCycleY, 0.0f, w3, rowH, (settings.envSeasonMode == 6) ? C2D_Color32(70, 210, 130, 255) : colYellow);
        char sRandomStr[8]; snprintf(sRandomStr, sizeof(sRandomStr), "Random");
        C2D_Text tSRandom; C2D_TextParse(&tSRandom, buf, sRandomStr); C2D_TextOptimize(&tSRandom);
        C2D_TextGetDimensions(&tSRandom, 0.40f, 0.40f, &pw, &ph);
        C2D_DrawText(&tSRandom, C2D_WithColor, px1+(w3-pw)*0.5f, rowSeasonCycleY+(rowH-ph)*0.5f, 0.5f, 0.40f, 0.40f, colTextBlack);
        C2D_DrawRectSolid(px2, rowSeasonCycleY, 0.0f, w3, rowH, colYellow);
        char sEveryStr[16]; snprintf(sEveryStr, sizeof(sEveryStr), "Every %um", (unsigned)envIntervalMinutes[settings.envSeasonCycleIntervalIndex % 4]);
        C2D_Text tSEvery; C2D_TextParse(&tSEvery, buf, sEveryStr); C2D_TextOptimize(&tSEvery);
        C2D_TextGetDimensions(&tSEvery, 0.40f, 0.40f, &pw, &ph);
        C2D_DrawText(&tSEvery, C2D_WithColor, px2+(w3-pw)*0.5f, rowSeasonCycleY+(rowH-ph)*0.5f, 0.5f, 0.40f, 0.40f, colTextBlack);

        // Riga: selezione fase (Auto/Dawn/Day/Dusk/Night)
        for (u32 p = 0; p < 5; p++) {
            static const char* envPhaseLabels[5] = {"Auto", "Dawn", "Day", "Dusk", "Night"};
            float px = envL + (float)p * (w5 + envGap);
            bool active = (settings.envPhaseMode == p);
            C2D_DrawRectSolid(px, row0Y, 0.0f, w5, rowH, active ? C2D_Color32(70, 210, 130, 255) : colYellow);
            C2D_Text tP; C2D_TextParse(&tP, buf, envPhaseLabels[p]); C2D_TextOptimize(&tP);
            C2D_TextGetDimensions(&tP, 0.40f, 0.40f, &pw, &ph);
            C2D_DrawText(&tP, C2D_WithColor, px+(w5-pw)*0.5f, row0Y+(rowH-ph)*0.5f, 0.5f, 0.40f, 0.40f, colTextBlack);
        }
        // Riga: Cycle / Random / intervallo — fase
        C2D_DrawRectSolid(px0, row1Y, 0.0f, w3, rowH, (settings.envPhaseMode == 5) ? C2D_Color32(70, 210, 130, 255) : colYellow);
        char pCycleStr[8]; snprintf(pCycleStr, sizeof(pCycleStr), "Cycle");
        C2D_Text tPCycle; C2D_TextParse(&tPCycle, buf, pCycleStr); C2D_TextOptimize(&tPCycle);
        C2D_TextGetDimensions(&tPCycle, 0.40f, 0.40f, &pw, &ph);
        C2D_DrawText(&tPCycle, C2D_WithColor, px0+(w3-pw)*0.5f, row1Y+(rowH-ph)*0.5f, 0.5f, 0.40f, 0.40f, colTextBlack);
        C2D_DrawRectSolid(px1, row1Y, 0.0f, w3, rowH, (settings.envPhaseMode == 6) ? C2D_Color32(70, 210, 130, 255) : colYellow);
        char pRandomStr[8]; snprintf(pRandomStr, sizeof(pRandomStr), "Random");
        C2D_Text tPRandom; C2D_TextParse(&tPRandom, buf, pRandomStr); C2D_TextOptimize(&tPRandom);
        C2D_TextGetDimensions(&tPRandom, 0.40f, 0.40f, &pw, &ph);
        C2D_DrawText(&tPRandom, C2D_WithColor, px1+(w3-pw)*0.5f, row1Y+(rowH-ph)*0.5f, 0.5f, 0.40f, 0.40f, colTextBlack);
        C2D_DrawRectSolid(px2, row1Y, 0.0f, w3, rowH, colYellow);
        char pEveryStr[16]; snprintf(pEveryStr, sizeof(pEveryStr), "Every %um", (unsigned)envIntervalMinutes[settings.envPhaseCycleIntervalIndex % 4]);
        C2D_Text tPEvery; C2D_TextParse(&tPEvery, buf, pEveryStr); C2D_TextOptimize(&tPEvery);
        C2D_TextGetDimensions(&tPEvery, 0.40f, 0.40f, &pw, &ph);
        C2D_DrawText(&tPEvery, C2D_WithColor, px2+(w3-pw)*0.5f, row1Y+(rowH-ph)*0.5f, 0.5f, 0.40f, 0.40f, colTextBlack);
        {
            float halfW = (envW - envGap) * 0.5f;
            float speedX = envL, dirX = envL + halfW + envGap;
            static const char* dirLabels[4] = {"Left", "Right", "Alt.1", "Alt.2"};
            u32 dirIdx = (settings.envCloudDirection < 4) ? settings.envCloudDirection : 0;
            C2D_DrawRectSolid(dirX, row2Y, 0.0f, halfW, rowH, colYellow);
            char dirStr[24]; snprintf(dirStr, sizeof(dirStr), "Cloud Direction: %s", dirLabels[dirIdx]);
            C2D_Text tDir; C2D_TextParse(&tDir, buf, dirStr); C2D_TextOptimize(&tDir);
            C2D_TextGetDimensions(&tDir, 0.40f, 0.40f, &pw, &ph);
            C2D_DrawText(&tDir, C2D_WithColor, dirX+(halfW-pw)*0.5f, row2Y+(rowH-ph)*0.5f, 0.5f, 0.40f, 0.40f, colTextBlack);
            C2D_DrawRectSolid(speedX, row2Y, 0.0f, halfW, rowH, C2D_Color32(45, 55, 45, 255));
            C2D_DrawRectSolid(speedX, row2Y, 0.0f, rowH, rowH, C2D_Color32(70, 70, 75, 255));
            C2D_DrawRectSolid(speedX+halfW-rowH, row2Y, 0.0f, rowH, rowH, C2D_Color32(70, 70, 75, 255));
            char csMinusStr[2] = "-"; C2D_Text tCsMinus; C2D_TextParse(&tCsMinus, buf, csMinusStr); C2D_TextOptimize(&tCsMinus);
            C2D_TextGetDimensions(&tCsMinus, 0.40f, 0.40f, &pw, &ph);
            C2D_DrawText(&tCsMinus, C2D_WithColor, speedX+(rowH-pw)*0.5f, row2Y+(rowH-ph)*0.5f, 0.5f, 0.40f, 0.40f, colWhite);
            char csPlusStr[2] = "+"; C2D_Text tCsPlus; C2D_TextParse(&tCsPlus, buf, csPlusStr); C2D_TextOptimize(&tCsPlus);
            C2D_TextGetDimensions(&tCsPlus, 0.40f, 0.40f, &pw, &ph);
            C2D_DrawText(&tCsPlus, C2D_WithColor, (speedX+halfW-rowH)+(rowH-pw)*0.5f, row2Y+(rowH-ph)*0.5f, 0.5f, 0.40f, 0.40f, colWhite);
            char csStr[24]; snprintf(csStr, sizeof(csStr), "Cloud Speed: %u", (unsigned)settings.envCloudSpeedLevel);
            C2D_Text tCs; C2D_TextParse(&tCs, buf, csStr); C2D_TextOptimize(&tCs);
            C2D_TextGetDimensions(&tCs, 0.40f, 0.40f, &pw, &ph);
            C2D_DrawText(&tCs, C2D_WithColor, speedX+(halfW-pw)*0.5f, row2Y+(rowH-ph)*0.5f, 0.5f, 0.40f, 0.40f, colWhite);
        }
        {
            float halfW = (envW - envGap) * 0.5f;
            float moodX = envL, weatherX = envL + halfW + envGap;

            // Sinistra: "Sky Mood" (era "Forest Mood", larghezza dimezzata)
            u32 envActiveTheme = (settings.bgThemeIndex < 5) ? settings.bgThemeIndex : 0;
            char themePrefixStr[16]; snprintf(themePrefixStr, sizeof(themePrefixStr), "Sky Mood: ");
            C2D_Text tThemePrefix; C2D_TextParse(&tThemePrefix, buf, themePrefixStr); C2D_TextOptimize(&tThemePrefix);
            char themeNameStr[48]; snprintf(themeNameStr, sizeof(themeNameStr), "%s", phaseThemes[currentForestPhase][envActiveTheme].name);
            C2D_Text tThemeName; C2D_TextParse(&tThemeName, buf, themeNameStr); C2D_TextOptimize(&tThemeName);
            C2D_DrawRectSolid(moodX, row3Y, 0.0f, halfW, rowH, C2D_Color32(45, 55, 45, 255));
            u32 targetThemeCol = phaseThemes[currentForestPhase][envActiveTheme].textMenuColor;
            float targetTR = (float)(targetThemeCol & 0xFF);
            float targetTG = (float)((targetThemeCol >> 8) & 0xFF);
            float targetTB = (float)((targetThemeCol >> 16) & 0xFF);
            currentThemeTextR += (targetTR - currentThemeTextR) * COLOR_TRANSITION_SPEED;
            currentThemeTextG += (targetTG - currentThemeTextG) * COLOR_TRANSITION_SPEED;
            currentThemeTextB += (targetTB - currentThemeTextB) * COLOR_TRANSITION_SPEED;
            u32 finalThemeTxtCol = C2D_Color32((u8)currentThemeTextR, (u8)currentThemeTextG, (u8)currentThemeTextB, 255);
            float prefixW, prefixH, nameW, nameH;
            C2D_TextGetDimensions(&tThemePrefix, 0.40f, 0.40f, &prefixW, &prefixH);
            C2D_TextGetDimensions(&tThemeName, 0.40f, 0.40f, &nameW, &nameH);
            float themeBlockX = moodX + (halfW - (prefixW + nameW)) * 0.5f;
            C2D_DrawText(&tThemePrefix, C2D_WithColor, themeBlockX, row3Y+(rowH-prefixH)*0.5f, 0.5f, 0.40f, 0.40f, colWhite);
            C2D_DrawText(&tThemeName, C2D_WithColor, themeBlockX + prefixW, row3Y+(rowH-nameH)*0.5f, 0.5f, 0.40f, 0.40f, finalThemeTxtCol);

            // Destra: "Weather", cicla Clear/Rain Light/Med/Heavy/Extreme,
            // Light Snow/Snow/Heavy Snow (qualunque stagione), Real Time (giallo)
            static const char* weatherLabels[9] = {"Clear", "Rain Light", "Rain Med", "Rain Heavy", "Rain Extreme", "Light Snow", "Snow", "Heavy Snow", "Real Time"};
            C2D_DrawRectSolid(weatherX, row3Y, 0.0f, halfW, rowH, colYellow);
            char weatherStr[24]; snprintf(weatherStr, sizeof(weatherStr), "Weather: %s", weatherLabels[settings.envWeatherMode]);
            C2D_Text tWeather; C2D_TextParse(&tWeather, buf, weatherStr); C2D_TextOptimize(&tWeather);
            float weatherW, weatherH;
            C2D_TextGetDimensions(&tWeather, 0.40f, 0.40f, &weatherW, &weatherH);
            C2D_DrawText(&tWeather, C2D_WithColor, weatherX+(halfW-weatherW)*0.5f, row3Y+(rowH-weatherH)*0.5f, 0.5f, 0.40f, 0.40f, colTextBlack);
        }
        {
            // Riga preset: "Save Preset" (2 unità) + 4 pulsanti numerati (1
            // unità ciascuno), stessa spaziatura (envGap) del resto della
            // schermata. In modalità salvataggio (presetSaveMode) i 4
            // pulsanti numerati lampeggiano; il tap su uno slot in quella
            // modalità salva lì lo stato corrente, altrimenti lo richiama.
            float unitW = (envW - 4.0f * envGap) / 6.0f;
            float saveW = unitW * 2.0f;
            float slotW = unitW;
            float saveX = envL;
            float slot1X = saveX + saveW + envGap;
            float slot2X = slot1X + slotW + envGap;
            float slot3X = slot2X + slotW + envGap;
            float slot4X = slot3X + slotW + envGap;
            float slotXs[4] = {slot1X, slot2X, slot3X, slot4X};

            C2D_DrawRectSolid(saveX, row4Y, 0.0f, saveW, rowH, presetSaveMode ? C2D_Color32(230, 140, 40, 255) : colYellow);
            char saveBtnStr[16]; snprintf(saveBtnStr, sizeof(saveBtnStr), presetSaveMode ? "Cancel" : "Save Preset");
            C2D_Text tSaveBtn; C2D_TextParse(&tSaveBtn, buf, saveBtnStr); C2D_TextOptimize(&tSaveBtn);
            C2D_TextGetDimensions(&tSaveBtn, 0.40f, 0.40f, &pw, &ph);
            C2D_DrawText(&tSaveBtn, C2D_WithColor, saveX+(saveW-pw)*0.5f, row4Y+(rowH-ph)*0.5f, 0.5f, 0.40f, 0.40f, colTextBlack);

            bool blinkOn = fmodf(globalAnimTime, 0.6f) < 0.3f;
            for (int s = 0; s < 4; s++) {
                u32 slotCol;
                if (presetSaveMode) {
                    slotCol = blinkOn ? C2D_Color32(230, 140, 40, 255) : C2D_Color32(120, 90, 40, 255);
                } else if (activePresetSlot == s) {
                    slotCol = C2D_Color32(70, 210, 130, 255);
                } else {
                    slotCol = envPresets[s].used ? colYellow : C2D_Color32(90, 90, 90, 255);
                }
                C2D_DrawRectSolid(slotXs[s], row4Y, 0.0f, slotW, rowH, slotCol);
                char slotStr[4]; snprintf(slotStr, sizeof(slotStr), "%d", s + 1);
                C2D_Text tSlot; C2D_TextParse(&tSlot, buf, slotStr); C2D_TextOptimize(&tSlot);
                C2D_TextGetDimensions(&tSlot, 0.40f, 0.40f, &pw, &ph);
                C2D_DrawText(&tSlot, C2D_WithColor, slotXs[s]+(slotW-pw)*0.5f, row4Y+(rowH-ph)*0.5f, 0.5f, 0.40f, 0.40f, colTextBlack);
            }
        }
        {
            float backW = ((envW - 4.0f * envGap) / 6.0f) * 2.0f; // stessa larghezza di "Save Preset"
            C2D_DrawRectSolid(envL, row5Y, 0.0f, backW, rowH, colYellow);
            C2D_TextGetDimensions(&ui.btnBack, 0.40f, 0.40f, &pw, &ph);
            C2D_DrawText(&ui.btnBack, C2D_WithColor, envL+(backW-pw)*0.5f, row5Y+(rowH-ph)*0.5f, 0.5f, 0.40f, 0.40f, colTextBlack);
        }
    }
    else if (currentScreen == SCREEN_SETTINGS) {
        u32 bottomBgColor = C2D_Color32(22, 30, 20, 255);
        C2D_TargetClear(target, bottomBgColor);
        C2D_DrawRectSolid(10, 10,  0.0f, 300, 2, C2D_Color32(50, 80, 50, 255));
        C2D_DrawRectSolid(10, 228, 0.0f, 300, 2, C2D_Color32(50, 80, 50, 255));
        drawSlimShadowTitle("SETTINGS", buf, 20, 15, 0.65f, colLightGreen, C2D_Color32(8, 12, 6, 220));
        char colStr[64]; snprintf(colStr, sizeof(colStr), "Clock Color: <%s>", presetNames[settings.clockColorIndex]);
        C2D_Text tCol; C2D_TextParse(&tCol, buf, colStr); C2D_TextOptimize(&tCol);
        C2D_DrawRectSolid(15, 45, 0.0f, 290, 24, C2D_Color32(45, 55, 45, 255));
        u32 previewCol;
        if (settings.clockColorIndex == 12) {
            float pr, pg, pb;
            getRainbowColor(globalAnimTime, &pr, &pg, &pb);
            previewCol = C2D_Color32((u8)pr, (u8)pg, (u8)pb, 255);
        } else {
            previewCol = C2D_Color32((u8)currentClockR, (u8)currentClockG, (u8)currentClockB, 255);
        }
        C2D_DrawText(&tCol, C2D_WithColor, 25, 49, 0.5f, 0.46f, 0.46f, previewCol);
        char timeFmtStr[64]; snprintf(timeFmtStr, sizeof(timeFmtStr), "Time Format: <%s>", settings.timeFormat24h ? "24 Hours" : "12 Hours (am/pm)");
        C2D_Text tTF; C2D_TextParse(&tTF, buf, timeFmtStr); C2D_TextOptimize(&tTF);
        C2D_DrawRectSolid(15, 75, 0.0f, 290, 24, C2D_Color32(45, 55, 45, 255));
        C2D_DrawText(&tTF, C2D_WithColor, 25, 79, 0.5f, 0.46f, 0.46f, colWhite);
        char dateFmtStr[64]; snprintf(dateFmtStr, sizeof(dateFmtStr), "Date Format: <%s>", settings.dateFormat == 2 ? "YYYY-MM-DD (ISO)" : (settings.dateFormat ? "DD/MM/YYYY (EU)" : "MM/DD/YYYY (USA)"));
        C2D_Text tDF; C2D_TextParse(&tDF, buf, dateFmtStr); C2D_TextOptimize(&tDF);
        C2D_DrawRectSolid(15, 105, 0.0f, 290, 24, C2D_Color32(45, 55, 45, 255));
        C2D_DrawText(&tDF, C2D_WithColor, 25, 109, 0.5f, 0.46f, 0.46f, colWhite);
        char locStr[64]; snprintf(locStr, sizeof(locStr), "Location: <%s>", settings.location[0] ? settings.location : "Not set");
        C2D_Text tLoc; C2D_TextParse(&tLoc, buf, locStr); C2D_TextOptimize(&tLoc);
        C2D_DrawRectSolid(15, 135, 0.0f, 290, 24, C2D_Color32(45, 55, 45, 255));
        C2D_DrawText(&tLoc, C2D_WithColor, 25, 139, 0.5f, 0.46f, 0.46f, colWhite);
        {
            bool haveWeather = (strncmp(weatherFetchStatus, "OK:", 3) == 0);
            char locWxStr[24]; snprintf(locWxStr, sizeof(locWxStr), "%s", haveWeather ? weatherStateLabel((int)realTimeWeatherType) : "-");
            C2D_Text tLocWx; C2D_TextParse(&tLocWx, buf, locWxStr); C2D_TextOptimize(&tLocWx);
            float locWxW, locWxH;
            C2D_TextGetDimensions(&tLocWx, 0.46f, 0.46f, &locWxW, &locWxH);
            C2D_DrawText(&tLocWx, C2D_WithColor, 295.0f - locWxW, 139, 0.5f, 0.46f, 0.46f, C2D_Color32(150,220,190,255));
        }

        // Riga nuova: "Weather Update" (sinistra, ritenta subito il fetch)
        // e "Schedule" (destra, cicla Never/10min/30min/1h).
        {
            float wRowY = 165.0f, wRowH = 24.0f, wGap = 6.0f;
            float halfW = (290.0f - wGap) * 0.5f;
            float updX = 15.0f, schedX = 15.0f + halfW + wGap;
            float tw, th;
            C2D_DrawRectSolid(updX, wRowY, 0.0f, halfW, wRowH, colYellow);
            char updStr[16]; snprintf(updStr, sizeof(updStr), "Weather Update");
            C2D_Text tUpd; C2D_TextParse(&tUpd, buf, updStr); C2D_TextOptimize(&tUpd);
            C2D_TextGetDimensions(&tUpd, 0.46f, 0.46f, &tw, &th);
            C2D_DrawText(&tUpd, C2D_WithColor, updX+(halfW-tw)*0.5f, wRowY+(wRowH-th)*0.5f, 0.5f, 0.46f, 0.46f, colTextBlack);

            static const char* scheduleLabels[4] = {"Never", "10min", "30min", "1h"};
            C2D_DrawRectSolid(schedX, wRowY, 0.0f, halfW, wRowH, colYellow);
            char schedStr[32]; snprintf(schedStr, sizeof(schedStr), "Check Every: %s", scheduleLabels[settings.weatherScheduleMode % 4]);
            C2D_Text tSched; C2D_TextParse(&tSched, buf, schedStr); C2D_TextOptimize(&tSched);
            C2D_TextGetDimensions(&tSched, 0.46f, 0.46f, &tw, &th);
            C2D_DrawText(&tSched, C2D_WithColor, schedX+(halfW-tw)*0.5f, wRowY+(wRowH-th)*0.5f, 0.5f, 0.46f, 0.46f, colTextBlack);
        }

        float bw, bh;
        C2D_DrawRectSolid(15,  195, 0.0f, 68, 25, colYellow);
        C2D_TextGetDimensions(&ui.btnBackSet, 0.46f, 0.46f, &bw, &bh);
        C2D_DrawText(&ui.btnBackSet, C2D_WithColor, 15+(68-bw)*0.5f, 195+(25-bh)*0.5f, 0.5f, 0.46f, 0.46f, colTextBlack);
        C2D_DrawRectSolid(89, 195, 0.0f, 68, 25, colYellow);
        C2D_TextGetDimensions(&ui.btnAlarmSet, 0.46f, 0.46f, &bw, &bh);
        C2D_DrawText(&ui.btnAlarmSet, C2D_WithColor, 89+(68-bw)*0.5f, 195+(25-bh)*0.5f, 0.5f, 0.46f, 0.46f, colTextBlack);
        C2D_DrawRectSolid(163, 195, 0.0f, 68, 25, colYellow);
        C2D_TextGetDimensions(&ui.btnTimerSet, 0.46f, 0.46f, &bw, &bh);
        C2D_DrawText(&ui.btnTimerSet, C2D_WithColor, 163+(68-bw)*0.5f, 195+(25-bh)*0.5f, 0.5f, 0.46f, 0.46f, colTextBlack);
        C2D_DrawRectSolid(237, 195, 0.0f, 68, 25, colYellow);
        C2D_TextGetDimensions(&ui.btnResetSet, 0.46f, 0.46f, &bw, &bh);
        C2D_DrawText(&ui.btnResetSet, C2D_WithColor, 237+(68-bw)*0.5f, 195+(25-bh)*0.5f, 0.5f, 0.46f, 0.46f, colTextBlack);
    }
    else if (currentScreen == SCREEN_CREDITS) {
        C2D_TargetClear(target, C2D_Color32(20, 25, 35, 255));
        C2D_DrawRectSolid(10, 10,  0.0f, 300, 2, C2D_Color32(40, 60, 90, 255));
        C2D_DrawRectSolid(10, 228, 0.0f, 300, 2, C2D_Color32(40, 60, 90, 255));
        float tw, th;
        {
            C2D_Text tCredMeasure; C2D_TextParse(&tCredMeasure, buf, "FORESTCLOCK CREDITS"); C2D_TextOptimize(&tCredMeasure);
            C2D_TextGetDimensions(&tCredMeasure, 0.70f, 0.70f, &tw, &th);
        }
        drawSlimShadowTitle("FORESTCLOCK CREDITS", buf, (320.0f-tw)*0.5f, 25, 0.70f, colCyan, C2D_Color32(6, 10, 14, 220));
        C2D_DrawText(&ui.credLine1, C2D_WithColor, 25, 75,  0.5f, 0.52f, 0.52f, colWhite);
        C2D_DrawText(&ui.credLine2, C2D_WithColor, 25, 100, 0.5f, 0.52f, 0.52f, colWhite);
        C2D_DrawText(&ui.credLine3, C2D_WithColor, 25, 130, 0.5f, 0.52f, 0.52f, colWhite);
        C2D_DrawText(&ui.credLine4, C2D_WithColor, 25, 155, 0.5f, 0.52f, 0.52f, colWhite);
        float bw, bh;
        C2D_DrawRectSolid(105, 185, 0.0f, 110, 32, colYellow);
        C2D_TextGetDimensions(&ui.btnBackCred, 0.6f, 0.6f, &bw, &bh);
        C2D_DrawText(&ui.btnBackCred, C2D_WithColor, 105+(110-bw)*0.5f, 185+(32-bh)*0.5f, 0.5f, 0.6f, 0.6f, colTextBlack);
    }
    else if (currentScreen == SCREEN_ALARM) {
        C2D_TargetClear(target, C2D_Color32(35, 25, 30, 255));
        C2D_DrawRectSolid(10, 10,  0.0f, 300, 2, C2D_Color32(90, 60, 50, 255));
        C2D_DrawRectSolid(10, 228, 0.0f, 300, 2, C2D_Color32(90, 60, 50, 255));
        bool is12h = !settings.timeFormat24h;
        float tw, th;
        C2D_TextGetDimensions(&ui.alarmTitle, 0.70f, 0.70f, &tw, &th);
        drawSlimShadowTitle("ALARM", buf, (320.0f-tw)*0.5f, 22, 0.70f, C2D_Color32(240,160,80,255), C2D_Color32(14, 8, 6, 220));
        char statusStr[48];
        if (alarmCfg.enabled) {
            if (is12h) {
                u32 h12 = alarmCfg.hour % 12; if (h12 == 0) h12 = 12;
                snprintf(statusStr, sizeof(statusStr), "Alarm set: %02u:%02u %s",
                         (unsigned)h12, (unsigned)alarmCfg.minute, (alarmCfg.hour >= 12) ? "pm" : "am");
            } else {
                snprintf(statusStr, sizeof(statusStr), "Alarm set: %02u:%02u", (unsigned)alarmCfg.hour, (unsigned)alarmCfg.minute);
            }
        } else {
            snprintf(statusStr, sizeof(statusStr), "Alarm is OFF");
        }
        C2D_Text tStatus; C2D_TextParse(&tStatus, buf, statusStr); C2D_TextOptimize(&tStatus);
        C2D_TextGetDimensions(&tStatus, 0.45f, 0.45f, &tw, &th);
        C2D_DrawText(&tStatus, C2D_WithColor, (320.0f-tw)*0.5f, 42, 0.5f, 0.45f, 0.45f, colWhite);
        float hourFieldX, minFieldX, ampmFieldX;
        if (is12h) {
            const float gap = 10.0f;
            const float totalW = 80.0f + gap + 80.0f + gap + 66.0f;
            const float startX = 15.0f + (290.0f - totalW) * 0.5f;
            hourFieldX = startX;
            minFieldX  = hourFieldX + 80.0f + gap;
            ampmFieldX = minFieldX  + 80.0f + gap;
        } else {
            hourFieldX = 82.0f;
            minFieldX  = 172.0f;
            ampmFieldX = 0.0f;
        }
        C2D_DrawRectSolid(hourFieldX, 80, 0.0f, 80, 70, C2D_Color32(50, 40, 40, 255));
        C2D_TextGetDimensions(&ui.alarmHourLabel, 0.45f, 0.45f, &tw, &th);
        C2D_DrawText(&ui.alarmHourLabel, C2D_WithColor, hourFieldX+(80-tw)*0.5f, 86, 0.5f, 0.45f, 0.45f, C2D_Color32(200,150,120,255));
        char hourStr[8]; snprintf(hourStr, sizeof(hourStr), "%02u", (unsigned)alarmEditHour);
        C2D_Text tHour; C2D_TextParse(&tHour, buf, hourStr); C2D_TextOptimize(&tHour);
        C2D_TextGetDimensions(&tHour, 0.9f, 0.9f, &tw, &th);
        C2D_DrawText(&tHour, C2D_WithColor, hourFieldX+(80-tw)*0.5f, 108, 0.5f, 0.9f, 0.9f, colTextWhite);
        drawStepperButtons(hourFieldX, 80.0f, 66.0f, 151.0f, 16.0f, buf, colTextWhite);
        C2D_DrawRectSolid(minFieldX, 80, 0.0f, 80, 70, C2D_Color32(50, 40, 40, 255));
        C2D_TextGetDimensions(&ui.alarmMinLabel, 0.45f, 0.45f, &tw, &th);
        C2D_DrawText(&ui.alarmMinLabel, C2D_WithColor, minFieldX+(80-tw)*0.5f, 86, 0.5f, 0.45f, 0.45f, C2D_Color32(200,150,120,255));
        char minStr[8]; snprintf(minStr, sizeof(minStr), "%02u", (unsigned)alarmEditMinute);
        C2D_Text tMin; C2D_TextParse(&tMin, buf, minStr); C2D_TextOptimize(&tMin);
        C2D_TextGetDimensions(&tMin, 0.9f, 0.9f, &tw, &th);
        C2D_DrawText(&tMin, C2D_WithColor, minFieldX+(80-tw)*0.5f, 108, 0.5f, 0.9f, 0.9f, colTextWhite);
        drawStepperButtons(minFieldX, 80.0f, 66.0f, 151.0f, 16.0f, buf, colTextWhite);
        if (is12h) {
            C2D_DrawRectSolid(ampmFieldX, 91, 0.0f, 66, 48, C2D_Color32(50, 40, 40, 255));
            char ampmStr[4]; snprintf(ampmStr, sizeof(ampmStr), "%s", alarmEditPM ? "PM" : "AM");
            C2D_Text tAmpm; C2D_TextParse(&tAmpm, buf, ampmStr); C2D_TextOptimize(&tAmpm);
            C2D_TextGetDimensions(&tAmpm, 0.72f, 0.72f, &tw, &th);
            C2D_DrawText(&tAmpm, C2D_WithColor, ampmFieldX+(66-tw)*0.5f, 91+(48-th)*0.5f, 0.5f, 0.72f, 0.72f, colTextWhite);
        }
        float abw, abh;
        C2D_DrawRectSolid(15,  190, 0.0f, 90, 30, colYellow);
        C2D_TextGetDimensions(&ui.btnAlarmBack, 0.55f, 0.55f, &abw, &abh);
        C2D_DrawText(&ui.btnAlarmBack, C2D_WithColor, 15+(90-abw)*0.5f, 190+(30-abh)*0.5f, 0.5f, 0.55f, 0.55f, colTextBlack);
        C2D_DrawRectSolid(115, 190, 0.0f, 90, 30, colYellow);
        C2D_TextGetDimensions(&ui.btnAlarmSetConfirm, 0.55f, 0.55f, &abw, &abh);
        C2D_DrawText(&ui.btnAlarmSetConfirm, C2D_WithColor, 115+(90-abw)*0.5f, 190+(30-abh)*0.5f, 0.5f, 0.55f, 0.55f, colTextBlack);
        C2D_DrawRectSolid(215, 190, 0.0f, 90, 30, colYellow);
        C2D_TextGetDimensions(&ui.btnAlarmClear, 0.55f, 0.55f, &abw, &abh);
        C2D_DrawText(&ui.btnAlarmClear, C2D_WithColor, 215+(90-abw)*0.5f, 190+(30-abh)*0.5f, 0.5f, 0.55f, 0.55f, colTextBlack);
    }
    else if (currentScreen == SCREEN_TIMER) {
        C2D_TargetClear(target, C2D_Color32(25, 30, 38, 255));
        C2D_DrawRectSolid(10, 10,  0.0f, 300, 2, C2D_Color32(60, 80, 95, 255));
        C2D_DrawRectSolid(10, 228, 0.0f, 300, 2, C2D_Color32(60, 80, 95, 255));
        float tw, th;
        C2D_TextGetDimensions(&ui.timerTitle, 0.70f, 0.70f, &tw, &th);
        drawSlimShadowTitle("TIMER", buf, (320.0f-tw)*0.5f, 22, 0.70f, C2D_Color32(110,180,230,255), C2D_Color32(6, 10, 14, 220));
        char statusStr[48];
        if (timerState == TIMER_RUNNING || timerState == TIMER_PAUSED) {
            u32 h = timerRemainingSeconds / 3600;
            u32 m = (timerRemainingSeconds % 3600) / 60;
            u32 s = timerRemainingSeconds % 60;
            snprintf(statusStr, sizeof(statusStr), "%s: %02u:%02u:%02u",
                     (timerState == TIMER_PAUSED) ? "Paused" : "Running", (unsigned)h, (unsigned)m, (unsigned)s);
        } else {
            snprintf(statusStr, sizeof(statusStr), "Timer not set");
        }
        C2D_Text tStatus; C2D_TextParse(&tStatus, buf, statusStr); C2D_TextOptimize(&tStatus);
        C2D_TextGetDimensions(&tStatus, 0.45f, 0.45f, &tw, &th);
        C2D_DrawText(&tStatus, C2D_WithColor, (320.0f-tw)*0.5f, 42, 0.5f, 0.45f, 0.45f, colWhite);
        const float fieldW = 80.0f, gap = 10.0f;
        const float totalW = fieldW*3 + gap*2;
        const float startX = 15.0f + (290.0f - totalW) * 0.5f;
        float hourFieldX = startX;
        float minFieldX  = hourFieldX + fieldW + gap;
        float secFieldX  = minFieldX  + fieldW + gap;
        bool fieldsEditable = (timerState == TIMER_IDLE);
        u32 fieldBgCol = fieldsEditable ? C2D_Color32(40, 48, 58, 255) : C2D_Color32(30, 34, 40, 255);
        u32 fieldTxtCol = fieldsEditable ? colTextWhite : C2D_Color32(140, 145, 150, 255);
        C2D_DrawRectSolid(hourFieldX, 80, 0.0f, fieldW, 70, fieldBgCol);
        C2D_TextGetDimensions(&ui.timerHourLabel, 0.42f, 0.42f, &tw, &th);
        C2D_DrawText(&ui.timerHourLabel, C2D_WithColor, hourFieldX+(fieldW-tw)*0.5f, 86, 0.5f, 0.42f, 0.42f, C2D_Color32(150,170,190,255));
        char hourStr[8]; snprintf(hourStr, sizeof(hourStr), "%02u", (unsigned)timerEditHour);
        C2D_Text tHour; C2D_TextParse(&tHour, buf, hourStr); C2D_TextOptimize(&tHour);
        C2D_TextGetDimensions(&tHour, 0.85f, 0.85f, &tw, &th);
        C2D_DrawText(&tHour, C2D_WithColor, hourFieldX+(fieldW-tw)*0.5f, 108, 0.5f, 0.85f, 0.85f, fieldTxtCol);
        C2D_DrawRectSolid(minFieldX, 80, 0.0f, fieldW, 70, fieldBgCol);
        C2D_TextGetDimensions(&ui.timerMinLabel, 0.42f, 0.42f, &tw, &th);
        C2D_DrawText(&ui.timerMinLabel, C2D_WithColor, minFieldX+(fieldW-tw)*0.5f, 86, 0.5f, 0.42f, 0.42f, C2D_Color32(150,170,190,255));
        char minStr[8]; snprintf(minStr, sizeof(minStr), "%02u", (unsigned)timerEditMinute);
        C2D_Text tMin; C2D_TextParse(&tMin, buf, minStr); C2D_TextOptimize(&tMin);
        C2D_TextGetDimensions(&tMin, 0.85f, 0.85f, &tw, &th);
        C2D_DrawText(&tMin, C2D_WithColor, minFieldX+(fieldW-tw)*0.5f, 108, 0.5f, 0.85f, 0.85f, fieldTxtCol);
        C2D_DrawRectSolid(secFieldX, 80, 0.0f, fieldW, 70, fieldBgCol);
        C2D_TextGetDimensions(&ui.timerSecLabel, 0.42f, 0.42f, &tw, &th);
        C2D_DrawText(&ui.timerSecLabel, C2D_WithColor, secFieldX+(fieldW-tw)*0.5f, 86, 0.5f, 0.42f, 0.42f, C2D_Color32(150,170,190,255));
        char secStr[8]; snprintf(secStr, sizeof(secStr), "%02u", (unsigned)timerEditSecond);
        C2D_Text tSec; C2D_TextParse(&tSec, buf, secStr); C2D_TextOptimize(&tSec);
        C2D_TextGetDimensions(&tSec, 0.85f, 0.85f, &tw, &th);
        C2D_DrawText(&tSec, C2D_WithColor, secFieldX+(fieldW-tw)*0.5f, 108, 0.5f, 0.85f, 0.85f, fieldTxtCol);
        if (fieldsEditable) {
            drawStepperButtons(hourFieldX, 80.0f, 66.0f, 151.0f, 16.0f, buf, fieldTxtCol);
            drawStepperButtons(minFieldX, 80.0f, 66.0f, 151.0f, 16.0f, buf, fieldTxtCol);
            drawStepperButtons(secFieldX, 80.0f, 66.0f, 151.0f, 16.0f, buf, fieldTxtCol);
        }
        float tbw, tbh;
        C2D_DrawRectSolid(15,  190, 0.0f, 90, 30, colYellow);
        C2D_TextGetDimensions(&ui.btnTimerBack, 0.55f, 0.55f, &tbw, &tbh);
        C2D_DrawText(&ui.btnTimerBack, C2D_WithColor, 15+(90-tbw)*0.5f, 190+(30-tbh)*0.5f, 0.5f, 0.55f, 0.55f, colTextBlack);
        C2D_Text* centerBtn;
        C2D_Text* rightBtn;
        if (timerState == TIMER_IDLE)         { centerBtn = &ui.btnTimerStart;  rightBtn = &ui.btnTimerReset; }
        else if (timerState == TIMER_RUNNING) { centerBtn = &ui.btnTimerPause;  rightBtn = &ui.btnTimerStop;  }
        else                { centerBtn = &ui.btnTimerResume; rightBtn = &ui.btnTimerStop;  }
        C2D_DrawRectSolid(115, 190, 0.0f, 90, 30, colYellow);
        C2D_TextGetDimensions(centerBtn, 0.55f, 0.55f, &tbw, &tbh);
        C2D_DrawText(centerBtn, C2D_WithColor, 115+(90-tbw)*0.5f, 190+(30-tbh)*0.5f, 0.5f, 0.55f, 0.55f, colTextBlack);
        C2D_DrawRectSolid(215, 190, 0.0f, 90, 30, colYellow);
        C2D_TextGetDimensions(rightBtn, 0.55f, 0.55f, &tbw, &tbh);
        C2D_DrawText(rightBtn, C2D_WithColor, 215+(90-tbw)*0.5f, 190+(30-tbh)*0.5f, 0.5f, 0.55f, 0.55f, colTextBlack);
    }
    if (alarmRinging) {
        float pulse = (sinf(alarmFlashPhase) * 0.5f) + 0.5f;
        u8 popR = (u8)(180.0f + pulse * 60.0f);
        u32 popupBg = C2D_Color32(popR, 25, 20, 255);
        C2D_TargetClear(target, popupBg);
        char popStr[40] = "Tap to stop alarm";
        C2D_Text tPopup; C2D_TextParse(&tPopup, buf, popStr); C2D_TextOptimize(&tPopup);
        float pw, ph;
        C2D_TextGetDimensions(&tPopup, 0.9f, 0.9f, &pw, &ph);
        C2D_DrawText(&tPopup, C2D_WithColor, (320.0f-pw)*0.5f, (240.0f-ph)*0.5f, 0.5f, 0.9f, 0.9f, C2D_Color32(255,255,255,255));
    }
}
static int updateInput() {
    hidScanInput();
    u32 kDown = hidKeysDown();
    u32 kHeld = hidKeysHeld();
    if (currentScreen == SCREEN_DIARY_ENTRY) {
        int maxScroll = (diaryTotalLines > DIARY_VISIBLE_LINES) ? (diaryTotalLines - DIARY_VISIBLE_LINES) : 0;
        bool scrollUp = false, scrollDown = false;
        if (kDown & KEY_DUP) scrollUp = true;
        if (kDown & KEY_DDOWN) scrollDown = true;

        circlePosition diaryCirclePos;
        hidCircleRead(&diaryCirclePos);
        const s16 diaryDeadzone = 40;
        if (diaryCirclePos.dy > diaryDeadzone || diaryCirclePos.dy < -diaryDeadzone) {
            diaryScrollRepeatTimer += 0.0166f;
            if (diaryScrollRepeatTimer >= 0.12f) {
                diaryScrollRepeatTimer = 0.0f;
                if (diaryCirclePos.dy > diaryDeadzone) scrollUp = true;
                else scrollDown = true;
            }
        } else {
            diaryScrollRepeatTimer = 0.0f;
        }

        if (scrollUp && diaryScrollOffset > 0) diaryScrollOffset--;
        if (scrollDown && diaryScrollOffset < maxScroll) diaryScrollOffset++;
    }
    if (currentScreen == SCREEN_NOTE_VIEW && !noteDeleteConfirmPending) {
        int noteMaxScroll = (noteTotalLines > NOTE_VISIBLE_LINES) ? (noteTotalLines - NOTE_VISIBLE_LINES) : 0;
        bool noteScrollUp = false, noteScrollDown = false;
        if (kDown & KEY_DUP) noteScrollUp = true;
        if (kDown & KEY_DDOWN) noteScrollDown = true;

        circlePosition noteCirclePos;
        hidCircleRead(&noteCirclePos);
        const s16 noteDeadzone = 40;
        if (noteCirclePos.dy > noteDeadzone || noteCirclePos.dy < -noteDeadzone) {
            noteScrollRepeatTimer += 0.0166f;
            if (noteScrollRepeatTimer >= 0.12f) {
                noteScrollRepeatTimer = 0.0f;
                if (noteCirclePos.dy > noteDeadzone) noteScrollUp = true;
                else noteScrollDown = true;
            }
        } else {
            noteScrollRepeatTimer = 0.0f;
        }

        if (noteScrollUp && noteScrollOffset > 0) noteScrollOffset--;
        if (noteScrollDown && noteScrollOffset < noteMaxScroll) noteScrollOffset++;
    }
    if (currentScreen == SCREEN_NOTES_LIST) {
        static int nlOrderScroll[NOTE_MAX_ENTRIES];
        int nlCountScroll = getNotesOrderedByModified(nlOrderScroll, NOTE_MAX_ENTRIES);
        if (notesListSelectedIndex >= nlCountScroll) notesListSelectedIndex = (nlCountScroll > 0) ? nlCountScroll - 1 : 0;
        if (notesListSelectedIndex < 0) notesListSelectedIndex = 0;

        bool nlScrollUp = false, nlScrollDown = false;
        if (kDown & KEY_DUP) nlScrollUp = true;
        if (kDown & KEY_DDOWN) nlScrollDown = true;

        circlePosition nlCirclePos;
        hidCircleRead(&nlCirclePos);
        const s16 nlDeadzone = 40;
        if (nlCirclePos.dy > nlDeadzone || nlCirclePos.dy < -nlDeadzone) {
            notesListScrollRepeatTimer += 0.0166f;
            if (notesListScrollRepeatTimer >= 0.12f) {
                notesListScrollRepeatTimer = 0.0f;
                if (nlCirclePos.dy > nlDeadzone) nlScrollUp = true;
                else nlScrollDown = true;
            }
        } else {
            notesListScrollRepeatTimer = 0.0f;
        }

        if (nlScrollUp && notesListSelectedIndex > 0) notesListSelectedIndex--;
        if (nlScrollDown && notesListSelectedIndex < nlCountScroll - 1) notesListSelectedIndex++;

        // lo scroll segue la selezione, tenendola sempre visibile
        if (notesListSelectedIndex < notesListScrollOffset) {
            notesListScrollOffset = notesListSelectedIndex;
        }
        if (notesListSelectedIndex >= notesListScrollOffset + NOTES_LIST_VISIBLE_COUNT) {
            notesListScrollOffset = notesListSelectedIndex - NOTES_LIST_VISIBLE_COUNT + 1;
        }

        if ((kDown & KEY_A) && nlCountScroll > 0) {
            noteViewIdx = nlOrderScroll[notesListSelectedIndex];
            noteScrollOffset = 0;
            currentScreen = SCREEN_NOTE_VIEW;
        }
    }
    if (kHeld & KEY_TOUCH) {
        touchPosition holdTouch;
        hidTouchRead(&holdTouch);
        HoldTarget target = getHoldTargetAtTouch(holdTouch.px, holdTouch.py);
        if (target != HOLD_NONE) {
            if (target != currentHoldTarget) {
                currentHoldTarget = target;
                holdTimer = 0.0f;
                holdFirstTickDone = false;
            } else {
                holdTimer += 0.0166f;
                float threshold = holdFirstTickDone ? HOLD_REPEAT_INTERVAL : HOLD_INITIAL_DELAY;
                if (holdTimer >= threshold) {
                    holdTimer -= threshold;
                    holdFirstTickDone = true;
                    applyHoldTargetIncrement(target);
                }
            }
        } else {
            currentHoldTarget = HOLD_NONE;
            holdFirstTickDone = false;
            holdTimer = 0.0f;
        }
    } else {
        currentHoldTarget = HOLD_NONE;
        holdFirstTickDone = false;
        holdTimer = 0.0f;
    }
    if (kHeld & KEY_L) {
        circlePosition circlePos;
        hidCircleRead(&circlePos);
        const s16 deadzone = 12;
        if (circlePos.dx > deadzone || circlePos.dx < -deadzone) {
            settings.clockOffsetX += (float)circlePos.dx / 60.0f;
            settingsDirty = true;
            activePresetSlot = -1;
        }
        if (circlePos.dy > deadzone || circlePos.dy < -deadzone) {
            settings.clockOffsetY -= (float)circlePos.dy / 60.0f;
            settingsDirty = true;
            activePresetSlot = -1;
        }
        if (settings.clockOffsetY < -110.0f) settings.clockOffsetY = -110.0f;
        if (settings.clockOffsetY >   97.0f) settings.clockOffsetY =   97.0f;
        if (kDown & KEY_A) {
            settings.clockOffsetX = 0.0f;
            settings.clockOffsetY = 0.0f;
            settingsDirty = true;
            activePresetSlot = -1;
            return 0;
        }
    }
    if (debugPointerActive && (kHeld & KEY_R)) {
        circlePosition pointerCirclePos;
        hidCircleRead(&pointerCirclePos);
        const s16 deadzone = 12;
        if (pointerCirclePos.dx > deadzone || pointerCirclePos.dx < -deadzone) {
            debugPointerX += (float)pointerCirclePos.dx / 300.0f;
        }
        if (pointerCirclePos.dy > deadzone || pointerCirclePos.dy < -deadzone) {
            debugPointerY -= (float)pointerCirclePos.dy / 300.0f;
        }
        if (debugPointerX < 0.0f)   debugPointerX = 0.0f;
        if (debugPointerX > 400.0f) debugPointerX = 400.0f;
        if (debugPointerY < 0.0f)   debugPointerY = 0.0f;
        if (debugPointerY > 240.0f) debugPointerY = 240.0f;
    }
    if (kDown & KEY_A) {
        if (alarmRinging) {
            stopAlarmRinging();
            return 0;
        }
    }
    if (kDown & KEY_B) {
        if (currentScreen == SCREEN_SETTINGS) {
            currentScreen = SCREEN_MAIN;
            return 0;
        }
        else if (currentScreen == SCREEN_RECORDS) {
            currentScreen = SCREEN_MAIN;
            return 0;
        }
        else if (currentScreen == SCREEN_DEBUG) {
            if (debugResetConfirmPending) {
                debugResetConfirmPending = false;
                return 0;
            }
            forestPhaseManualOverride = false;
            currentScreen = SCREEN_MAIN;
            return 0;
        }
        else if (currentScreen == SCREEN_WEATHER_DEBUG) {
            currentScreen = SCREEN_DEBUG;
            return 0;
        }
        else if (currentScreen == SCREEN_CALENDAR) {
            currentScreen = SCREEN_MAIN;
            return 0;
        }
        else if (currentScreen == SCREEN_CALENDAR_SETTINGS) {
            currentScreen = SCREEN_CALENDAR;
            return 0;
        }
        else if (currentScreen == SCREEN_DIARY_ENTRY) {
            currentScreen = SCREEN_CALENDAR;
            return 0;
        }
        else if (currentScreen == SCREEN_CALENDAR_MONTHYEAR) {
            currentScreen = SCREEN_CALENDAR;
            return 0;
        }
        else if (currentScreen == SCREEN_NOTES_LIST) {
            currentScreen = SCREEN_MAIN;
            return 0;
        }
        else if (currentScreen == SCREEN_NOTE_VIEW) {
            if (noteDeleteConfirmPending) {
                noteDeleteConfirmPending = false;
                return 0;
            }
            currentScreen = SCREEN_NOTES_LIST;
            return 0;
        }
        else if (currentScreen == SCREEN_ENVIRONMENT) {
            currentScreen = SCREEN_MAIN;
            return 0;
        }
        else if (currentScreen == SCREEN_CREDITS) {
            currentScreen = SCREEN_RECORDS;
            return 0;
        }
        else if (currentScreen == SCREEN_ALARM) {
            currentScreen = SCREEN_SETTINGS;
            return 0;
        }
        else if (currentScreen == SCREEN_TIMER) {
            currentScreen = SCREEN_SETTINGS;
            return 0;
        }
    }
    if (kDown & KEY_SELECT) {
        settings.clockMode = (settings.clockMode + 1) % 3;
        settingsDirty = true;
    }
    if (kDown & KEY_DRIGHT) {
        settings.clockSizePreset = (settings.clockSizePreset + 1) % 4;
        settingsDirty = true;
        activePresetSlot = -1;
    }
    if (kDown & KEY_DLEFT) {
        settings.clockSizePreset = (settings.clockSizePreset + 3) % 4;
        settingsDirty = true;
        activePresetSlot = -1;
    }
    if (kDown & KEY_TOUCH) {
        touchPosition touch;
        hidTouchRead(&touch);
        if (alarmRinging) {
            stopAlarmRinging();
            return 0;
        }
        if (lowerScreenOff) {
            lowerScreenOff = false;
            return 0;
        }
        if (currentScreen == SCREEN_MAIN) {
            if (mainAlarmTextVisible && touch.px >= mainAlarmTextX0 && touch.px <= mainAlarmTextX1 && touch.py >= mainAlarmTextY0 && touch.py <= mainAlarmTextY1) {
                currentScreen = SCREEN_ALARM;
            }
            if (mainTimerTextVisible && touch.px >= mainTimerTextX0 && touch.px <= mainTimerTextX1 && touch.py >= mainTimerTextY0 && touch.py <= mainTimerTextY1) {
                currentScreen = SCREEN_TIMER;
            }
            if (touch.px >= 15 && touch.px <= 105 && touch.py >= 185 && touch.py <= 217) {
                currentScreen = SCREEN_SETTINGS;
            }
            if (touch.px >= 115 && touch.px <= 205 && touch.py >= 185 && touch.py <= 217) {
                lowerScreenOff = true;
            }
            if (touch.px >= 215 && touch.px <= 305 && touch.py >= 185 && touch.py <= 217) {
                currentScreen = SCREEN_RECORDS;
            }
            if (touch.px >= 115 && touch.px <= 205 && touch.py >= 148 && touch.py <= 180) {
                notesListScrollOffset = 0;
                notesListSelectedIndex = 0;
                currentScreen = SCREEN_NOTES_LIST;
            }
            if (touch.px >= 15 && touch.px <= 105 && touch.py >= 148 && touch.py <= 180) {
                currentScreen = SCREEN_ENVIRONMENT;
            }
            if (touch.px >= 215 && touch.px <= 305 && touch.py >= 148 && touch.py <= 180) {
                time_t rawCalReset = time(NULL);
                struct tm* tmCalReset = localtime(&rawCalReset);
                calDisplayMonth = tmCalReset->tm_mon;
                calDisplayYear  = tmCalReset->tm_year + 1900;
                currentScreen = SCREEN_CALENDAR;
            }
        }
        else if (currentScreen == SCREEN_SETTINGS) {
            if (touch.px >= 15 && touch.px <= 305) {
                if (touch.py >= 45 && touch.py <= 69) {
                    settings.clockColorIndex = (settings.clockColorIndex + 1) % 13;
                    settingsDirty = true;
                    activePresetSlot = -1;
                }
                if (touch.py >= 75 && touch.py <= 99) {
                    settings.timeFormat24h = !settings.timeFormat24h;
                    settingsDirty = true;
                }
                if (touch.py >= 105 && touch.py <= 129) {
                    settings.dateFormat = (settings.dateFormat + 1) % 3;
                    settingsDirty = true;
                }
                if (touch.py >= 135 && touch.py <= 159) {
                    SwkbdState swkbd;
                    char kbdBuf[32];
                    swkbdInit(&swkbd, SWKBD_TYPE_NORMAL, 2, 31);
                    swkbdSetHintText(&swkbd, "City name (e.g. London)");
                    swkbdSetInitialText(&swkbd, settings.location);
                    SwkbdButton kbdButton = swkbdInputText(&swkbd, kbdBuf, sizeof(kbdBuf));
                    if (kbdButton == SWKBD_BUTTON_CONFIRM) {
                        strncpy(settings.location, kbdBuf, sizeof(settings.location) - 1);
                        settings.location[sizeof(settings.location) - 1] = '\0';
                        settingsDirty = true;
                        fetchWeatherFromInternet(); // aggiorna subito il meteo sulla nuova location, senza dover premere Weather Update
                    }
                }
                if (touch.py >= 165 && touch.py <= 189) {
                    float wGap = 6.0f;
                    float halfW = (290.0f - wGap) * 0.5f;
                    float updX = 15.0f, schedX = 15.0f + halfW + wGap;
                    if (touch.px >= updX && touch.px <= updX + halfW) {
                        fetchWeatherFromInternet();
                    }
                    if (touch.px >= schedX && touch.px <= schedX + halfW) {
                        settings.weatherScheduleMode = (settings.weatherScheduleMode + 1) % 4;
                        weatherScheduleTimer = 0.0f;
                        settingsDirty = true;
                    }
                }
            }
            if (touch.px >= 15 && touch.px <= 83 && touch.py >= 195 && touch.py <= 220) {
                currentScreen = SCREEN_MAIN;
            }
            if (touch.px >= 89 && touch.px <= 157 && touch.py >= 195 && touch.py <= 220) {
                if (alarmCfg.enabled) {
                    alarmEditMinute = alarmCfg.minute;
                    if (!settings.timeFormat24h) {
                        u32 h12 = alarmCfg.hour % 12; if (h12 == 0) h12 = 12;
                        alarmEditHour = h12;
                        alarmEditPM   = (alarmCfg.hour >= 12);
                    } else {
                        alarmEditHour = alarmCfg.hour;
                        alarmEditPM   = false;
                    }
                } else {
                    alarmEditMinute = 0;
                    alarmEditPM     = false;
                    alarmEditHour   = settings.timeFormat24h ? 0 : 12;
                }
                currentScreen = SCREEN_ALARM;
            }
            if (touch.px >= 163 && touch.px <= 231 && touch.py >= 195 && touch.py <= 220) {
                if (timerState == TIMER_IDLE) {
                    timerEditHour   = timerTotalSeconds / 3600;
                    timerEditMinute = (timerTotalSeconds % 3600) / 60;
                    timerEditSecond = timerTotalSeconds % 60;
                }
                currentScreen = SCREEN_TIMER;
            }
            if (touch.px >= 237 && touch.px <= 305 && touch.py >= 195 && touch.py <= 220) {
                settings = (AppSettings){11, 1, 1, 0, 1, 0, 0.0f, 0.0f, 0, 1, 3, 2, 0, 1};
                settingsDirty = true;
            }
        }
        else if (currentScreen == SCREEN_ALARM) {
            bool is12h = !settings.timeFormat24h;
            float hourFieldX, minFieldX, ampmFieldX;
            {
                HoldTarget tapTarget = getHoldTargetAtTouch(touch.px, touch.py);
                if (tapTarget != HOLD_NONE) applyHoldTargetIncrement(tapTarget);
            }
            if (is12h) {
                const float gap = 10.0f;
                const float totalW = 80.0f + gap + 80.0f + gap + 66.0f;
                const float startX = 15.0f + (290.0f - totalW) * 0.5f;
                hourFieldX = startX;
                minFieldX  = hourFieldX + 80.0f + gap;
                ampmFieldX = minFieldX  + 80.0f + gap;
            } else {
                hourFieldX = 82.0f;
                minFieldX  = 172.0f;
                ampmFieldX = 0.0f;
            }
            if (touch.px >= hourFieldX && touch.px <= hourFieldX + 80 && touch.py >= 80 && touch.py <= 150) {
                if (is12h) alarmEditHour = (alarmEditHour % 12) + 1;
                else       alarmEditHour = (alarmEditHour + 1) % 24;
            }
            if (touch.px >= minFieldX && touch.px <= minFieldX + 80 && touch.py >= 80 && touch.py <= 150) {
                alarmEditMinute = (alarmEditMinute + 1) % 60;
            }
            if (is12h && touch.px >= ampmFieldX && touch.px <= ampmFieldX + 66 && touch.py >= 91 && touch.py <= 139) {
                alarmEditPM = !alarmEditPM;
            }
            if (touch.px >= 15 && touch.px <= 105 && touch.py >= 190 && touch.py <= 220) {
                currentScreen = SCREEN_SETTINGS;
            }
            if (touch.px >= 115 && touch.px <= 205 && touch.py >= 190 && touch.py <= 220) {
                alarmCfg.enabled = 1;
                if (is12h) {
                    u32 h24 = alarmEditHour % 12;
                    if (alarmEditPM) h24 += 12;
                    alarmCfg.hour = h24;
                } else {
                    alarmCfg.hour = alarmEditHour;
                }
                alarmCfg.minute  = alarmEditMinute;
                alarmLastTrigHour = -1;
                alarmLastTrigMin  = -1;
                alarmDirty = true;
            }
            if (touch.px >= 215 && touch.px <= 305 && touch.py >= 190 && touch.py <= 220) {
                alarmCfg = (AppAlarm){0, 0, 0};
                alarmDirty = true;
            }
        }
        else if (currentScreen == SCREEN_TIMER) {
            const float fieldW = 80.0f, gap = 10.0f;
            const float totalW = fieldW*3 + gap*2;
            const float startX = 15.0f + (290.0f - totalW) * 0.5f;
            float hourFieldX = startX;
            float minFieldX  = hourFieldX + fieldW + gap;
            float secFieldX  = minFieldX  + fieldW + gap;
            bool fieldsEditable = (timerState == TIMER_IDLE);
            if (fieldsEditable) {
                HoldTarget tapTarget = getHoldTargetAtTouch(touch.px, touch.py);
                if (tapTarget != HOLD_NONE) applyHoldTargetIncrement(tapTarget);
                if (touch.px >= hourFieldX && touch.px <= hourFieldX + fieldW && touch.py >= 80 && touch.py <= 150) {
                    timerEditHour = (timerEditHour + 1) % 24;
                }
                if (touch.px >= minFieldX && touch.px <= minFieldX + fieldW && touch.py >= 80 && touch.py <= 150) {
                    timerEditMinute = (timerEditMinute + 1) % 60;
                }
                if (touch.px >= secFieldX && touch.px <= secFieldX + fieldW && touch.py >= 80 && touch.py <= 150) {
                    timerEditSecond = (timerEditSecond + 1) % 60;
                }
            }
            if (touch.px >= 15 && touch.px <= 105 && touch.py >= 190 && touch.py <= 220) {
                currentScreen = SCREEN_SETTINGS;
            }
            if (touch.px >= 115 && touch.px <= 205 && touch.py >= 190 && touch.py <= 220) {
                if (timerState == TIMER_IDLE) {
                    u32 totalSec = timerEditHour*3600 + timerEditMinute*60 + timerEditSecond;
                    if (totalSec > 0) {
                        timerTotalSeconds     = totalSec;
                        timerRemainingSeconds = totalSec;
                        timerFrameAccumulator = 0;
                        timerState = TIMER_RUNNING;
                    }
                } else if (timerState == TIMER_RUNNING) {
                    timerState = TIMER_PAUSED;
                } else {
                    timerFrameAccumulator = 0;
                    timerState = TIMER_RUNNING;
                }
            }
            if (touch.px >= 215 && touch.px <= 305 && touch.py >= 190 && touch.py <= 220) {
                if (timerState == TIMER_IDLE) {
                    timerEditHour = timerEditMinute = timerEditSecond = 0;
                } else {
                    timerState = TIMER_IDLE;
                    timerRemainingSeconds = 0;
                }
            }
        }
        else if (currentScreen == SCREEN_RECORDS) {
            if (touch.px >= 15 && touch.px <= 155 && touch.py >= 185 && touch.py <= 217) {
                currentScreen = SCREEN_CREDITS;
            }
            if (touch.px >= 165 && touch.px <= 305 && touch.py >= 185 && touch.py <= 217) {
                currentScreen = SCREEN_MAIN;
            }
        }
        else if (currentScreen == SCREEN_DEBUG) {
            if (debugResetConfirmPending) {
                if (touch.px >= 70 && touch.px <= 150 && touch.py >= 150 && touch.py <= 182) {
                    performFullReset();
                    debugResetConfirmPending = false;
                }
                if (touch.px >= 170 && touch.px <= 250 && touch.py >= 150 && touch.py <= 182) {
                    debugResetConfirmPending = false;
                }
            } else {
            if (touch.px >= 237 && touch.px <= 305 && touch.py >= 145 && touch.py <= 177) {
                debugResetConfirmPending = true;
            }
            if (touch.px >= 15 && touch.px <= 65 && touch.py >= 55 && touch.py <= 87) {
                if (activeCloudCount > 1) { activeCloudCount--; }
            }
            if (touch.px >= 255 && touch.px <= 305 && touch.py >= 55 && touch.py <= 87) {
                if (activeCloudCount < CLOUD_MAX) { activeCloudCount++; initSingleCloud(activeCloudCount - 1); }
            }
            if (touch.px >= 15 && touch.px <= 65 && touch.py >= 185 && touch.py <= 217) {
                if (foxAnimFrameDuration > 0.03f) foxAnimFrameDuration -= 0.01f;
            }
            if (touch.px >= 255 && touch.px <= 305 && touch.py >= 185 && touch.py <= 217) {
                if (foxAnimFrameDuration < 0.50f) foxAnimFrameDuration += 0.01f;
            }
            if (touch.px >= 15 && touch.px <= 65 && touch.py >= 219 && touch.py <= 239) {
                if (foxSpeed > 0.3f) foxSpeed -= 0.1f;
            }
            if (touch.px >= 255 && touch.px <= 305 && touch.py >= 219 && touch.py <= 239) {
                if (foxSpeed < 3.0f) foxSpeed += 0.1f;
            }
            if (touch.px >= 70 && touch.px <= 250 && touch.py >= 219 && touch.py <= 239) {
                debugFoxRunRequested = true;
            }
            if (touch.px >= 15 && touch.px <= 83 && touch.py >= 95 && touch.py <= 127) {
                resetForestHedgehogs();
            }
            if (touch.px >= 89 && touch.px <= 157 && touch.py >= 95 && touch.py <= 127) {
                debugOwlFlyRequested = true;
            }
            if (touch.px >= 163 && touch.px <= 231 && touch.py >= 95 && touch.py <= 127) {
                spawnForestPlane();
            }
            if (touch.px >= 237 && touch.px <= 305 && touch.py >= 95 && touch.py <= 127) {
                currentScreen = SCREEN_WEATHER_DEBUG;
            }
            if (touch.px >= 15 && touch.px <= 83 && touch.py >= 145 && touch.py <= 177) {
                debugPointerDisplayMode = (debugPointerDisplayMode + 1) % 12;
                if (debugPointerDisplayMode != 0) debugPointerActive = true;
            }
            if (touch.px >= 89 && touch.px <= 157 && touch.py >= 145 && touch.py <= 177) {
                debugPointerActive = !debugPointerActive;
            }
            if (touch.px >= 163 && touch.px <= 231 && touch.py >= 145 && touch.py <= 177) {
                forestPhaseManualOverride = false;
                currentScreen = SCREEN_MAIN;
            }
            }
        }
        else if (currentScreen == SCREEN_WEATHER_DEBUG) {
            if (touch.px >= 15 && touch.px <= 155 && touch.py >= 195 && touch.py <= 220) {
                fetchWeatherFromInternet();
            }
            if (touch.px >= 237 && touch.px <= 305 && touch.py >= 195 && touch.py <= 220) {
                currentScreen = SCREEN_DEBUG;
            }
        }
        else if (currentScreen == SCREEN_CALENDAR) {
            if (touch.px >= 245 && touch.px <= 305 && touch.py >= 12 && touch.py <= 32) {
                currentScreen = SCREEN_MAIN;
            }
            if (touch.px >= 15 && touch.px <= 75 && touch.py >= 12 && touch.py <= 32) {
                currentScreen = SCREEN_CALENDAR_SETTINGS;
            }
            if (touch.px >= 76 && touch.px <= 112 && touch.py >= 8 && touch.py <= 38) {
                calDisplayMonth--;
                if (calDisplayMonth < 0) { calDisplayMonth = 11; calDisplayYear--; }
            }
            if (touch.px >= 208 && touch.px <= 244 && touch.py >= 8 && touch.py <= 38) {
                calDisplayMonth++;
                if (calDisplayMonth > 11) { calDisplayMonth = 0; calDisplayYear++; }
            }
            if (touch.px >= 112 && touch.px <= 208 && touch.py >= 8 && touch.py <= 38) {
                calEditMonth = (calDisplayMonth >= 0) ? calDisplayMonth : 0;
                calEditYear  = (calDisplayYear  >= 0) ? calDisplayYear  : 2026;
                currentScreen = SCREEN_CALENDAR_MONTHYEAR;
            }
            if (touch.py >= 54 && touch.py <= 222) {
                int tapMonth = (calDisplayMonth >= 0) ? calDisplayMonth : 0;
                int tapYear  = (calDisplayYear  >= 0) ? calDisplayYear  : 2026;
                int tapDaysInMonth  = getDaysInMonth(tapMonth, tapYear);
                int tapFirstWeekdayRaw = getFirstWeekdayOfMonth(tapMonth, tapYear);
                int tapFirstWeekday = settings.calFirstDayMonday ? (tapFirstWeekdayRaw + 6) % 7 : tapFirstWeekdayRaw;
                const float tapCalL = 15.0f, tapCalR = 305.0f;
                const float tapColW = (tapCalR - tapCalL) / 7.0f;
                const float tapGridTopY = 54.0f, tapGridBottomY = 222.0f;
                const float tapRowH = (tapGridBottomY - tapGridTopY) / 6.0f;
                int tapRow = (int)((touch.py - tapGridTopY) / tapRowH);
                int tapCol = (int)((touch.px - tapCalL) / tapColW);
                if (tapRow >= 0 && tapRow < 6 && tapCol >= 0 && tapCol < 7) {
                    int tapDayNum = tapRow * 7 + tapCol - tapFirstWeekday + 1;
                    if (tapDayNum >= 1 && tapDayNum <= tapDaysInMonth) {
                        diaryViewYear  = tapYear;
                        diaryViewMonth = tapMonth;
                        diaryViewDay   = tapDayNum;
                        diaryScrollOffset = 0;
                        currentScreen  = SCREEN_DIARY_ENTRY;
                    }
                }
            }
        }
        else if (currentScreen == SCREEN_CALENDAR_SETTINGS) {
            if (touch.px >= 15 && touch.px <= 305 && touch.py >= 55 && touch.py <= 87) {
                settings.calFirstDayMonday = !settings.calFirstDayMonday;
                settingsDirty = true;
            }
            if (touch.px >= 15 && touch.px <= 155 && touch.py >= 195 && touch.py <= 220) {
                currentScreen = SCREEN_CALENDAR;
            }
        }
        else if (currentScreen == SCREEN_DIARY_ENTRY) {
            if (touch.px >= 235 && touch.px <= 265 && touch.py >= 8 && touch.py <= 35) {
                int ny, nm, nd;
                if (findAdjacentDiaryEntry(diaryViewYear, diaryViewMonth, diaryViewDay, -1, &ny, &nm, &nd)) {
                    diaryViewYear = ny; diaryViewMonth = nm; diaryViewDay = nd;
                    diaryScrollOffset = 0;
                }
            }
            if (touch.px >= 273 && touch.px <= 303 && touch.py >= 8 && touch.py <= 35) {
                int ny, nm, nd;
                if (findAdjacentDiaryEntry(diaryViewYear, diaryViewMonth, diaryViewDay, 1, &ny, &nm, &nd)) {
                    diaryViewYear = ny; diaryViewMonth = nm; diaryViewDay = nd;
                    diaryScrollOffset = 0;
                }
            }
            bool tappedWriteBtn = (touch.px >= 15 && touch.px <= 145 && touch.py >= 195 && touch.py <= 220);
            bool tappedTextArea = (touch.px >= 15 && touch.px <= 305 && touch.py >= 50 && touch.py <= 180);
            if (tappedWriteBtn || tappedTextArea) {
                int deIdx = getOrCreateDiaryEntry(diaryViewYear, diaryViewMonth, diaryViewDay);
                if (deIdx >= 0) {
                    SwkbdState swkbd;
                    char kbdBuf[DIARY_TEXT_MAX];
                    swkbdInit(&swkbd, SWKBD_TYPE_NORMAL, 2, DIARY_TEXT_MAX - 1);
                    swkbdSetFeatures(&swkbd, SWKBD_MULTILINE);
                    swkbdSetHintText(&swkbd, "Write something about this day...");
                    swkbdSetInitialText(&swkbd, diaryEntries[deIdx].text);
                    SwkbdButton kbdButton = swkbdInputText(&swkbd, kbdBuf, sizeof(kbdBuf));
                    if (kbdButton == SWKBD_BUTTON_CONFIRM) {
                        snprintf(diaryEntries[deIdx].text, DIARY_TEXT_MAX, "%s", kbdBuf);
                        saveDiary();
                        diaryScrollOffset = 0;
                    }
                }
            }
            if (touch.px >= 175 && touch.px <= 305 && touch.py >= 195 && touch.py <= 220) {
                currentScreen = SCREEN_CALENDAR;
            }
        }
        else if (currentScreen == SCREEN_CALENDAR_MONTHYEAR) {
            {
                HoldTarget tapTarget = getHoldTargetAtTouch(touch.px, touch.py);
                if (tapTarget != HOLD_NONE) applyHoldTargetIncrement(tapTarget);
            }
            if (touch.px >= 15 && touch.px <= 105 && touch.py >= 195 && touch.py <= 220) {
                calDisplayMonth = calEditMonth;
                calDisplayYear  = calEditYear;
                currentScreen = SCREEN_CALENDAR;
            }
            if (touch.px >= 115 && touch.px <= 205 && touch.py >= 195 && touch.py <= 220) {
                time_t rawNow = time(NULL);
                struct tm* tmNow = localtime(&rawNow);
                calDisplayMonth = tmNow->tm_mon;
                calDisplayYear  = tmNow->tm_year + 1900;
                currentScreen = SCREEN_CALENDAR;
            }
            if (touch.px >= 215 && touch.px <= 305 && touch.py >= 195 && touch.py <= 220) {
                currentScreen = SCREEN_CALENDAR;
            }
        }
        else if (currentScreen == SCREEN_NOTES_LIST) {
            if (touch.px >= 165 && touch.px <= 235 && touch.py >= 8 && touch.py <= 32) {
                int newIdx = createNewNote();
                if (newIdx >= 0) {
                    noteViewIdx = newIdx;
                    noteScrollOffset = 0;
                    currentScreen = SCREEN_NOTE_VIEW;
                }
            }
            if (touch.px >= 240 && touch.px <= 305 && touch.py >= 8 && touch.py <= 32) {
                currentScreen = SCREEN_MAIN;
            }
            static int nlOrderTouch[NOTE_MAX_ENTRIES];
            int nlCountTouch = getNotesOrderedByModified(nlOrderTouch, NOTE_MAX_ENTRIES);
            const float nlCardXTouch = 20.0f, nlCardWTouch = 280.0f, nlCardHTouch = 32.0f, nlCardGapTouch = 8.0f;
            const float nlCardStepTouch = nlCardHTouch + nlCardGapTouch;
            const float nlListTopYTouch = 42.0f;
            int nlVisibleTouch = 0;
            for (int ni = notesListScrollOffset; ni < nlCountTouch && nlVisibleTouch < NOTES_LIST_VISIBLE_COUNT; ni++) {
                float cardY = nlListTopYTouch + nlVisibleTouch * nlCardStepTouch;
                if (touch.px >= nlCardXTouch && touch.px <= nlCardXTouch + nlCardWTouch &&
                    touch.py >= cardY && touch.py <= cardY + nlCardHTouch) {
                    noteViewIdx = nlOrderTouch[ni];
                    notesListSelectedIndex = ni;
                    noteScrollOffset = 0;
                    currentScreen = SCREEN_NOTE_VIEW;
                    break;
                }
                nlVisibleTouch++;
            }
        }
        else if (currentScreen == SCREEN_NOTE_VIEW) {
            if (noteDeleteConfirmPending) {
                if (touch.px >= 70 && touch.px <= 150 && touch.py >= 140 && touch.py <= 172) {
                    deleteNote(noteViewIdx);
                    noteDeleteConfirmPending = false;
                    currentScreen = SCREEN_NOTES_LIST;
                }
                if (touch.px >= 170 && touch.px <= 250 && touch.py >= 140 && touch.py <= 172) {
                    noteDeleteConfirmPending = false;
                }
            } else {
                bool nvTappedWrite = (touch.px >= 15 && touch.px <= 105 && touch.py >= 195 && touch.py <= 220);
                bool nvTappedText  = (touch.px >= 15 && touch.px <= 305 && touch.py >= 42 && touch.py <= 180);
                if (nvTappedWrite || nvTappedText) {
                    if (noteViewIdx >= 0) {
                        SwkbdState swkbd;
                        char kbdBuf[NOTE_TEXT_MAX];
                        swkbdInit(&swkbd, SWKBD_TYPE_NORMAL, 2, NOTE_TEXT_MAX - 1);
                        swkbdSetFeatures(&swkbd, SWKBD_MULTILINE);
                        swkbdSetHintText(&swkbd, "Write your note...");
                        swkbdSetInitialText(&swkbd, notes[noteViewIdx].text);
                        SwkbdButton kbdButton = swkbdInputText(&swkbd, kbdBuf, sizeof(kbdBuf));
                        if (kbdButton == SWKBD_BUTTON_CONFIRM) {
                            snprintf(notes[noteViewIdx].text, NOTE_TEXT_MAX, "%s", kbdBuf);
                            notes[noteViewIdx].modifiedTime = (u32)time(NULL);
                            saveNotes();
                            noteScrollOffset = 0;
                        }
                    }
                }
                if (touch.px >= 115 && touch.px <= 205 && touch.py >= 195 && touch.py <= 220) {
                    noteDeleteConfirmPending = true;
                }
                if (touch.px >= 215 && touch.px <= 305 && touch.py >= 195 && touch.py <= 220) {
                    currentScreen = SCREEN_NOTES_LIST;
                }
            }
        }
        else if (currentScreen == SCREEN_ENVIRONMENT) {
            const float envL = 15.0f, envGap = 4.0f, rowH = 19.0f, rowStep = rowH + envGap;
            const float rowSeasonSelY = 40.0f, rowSeasonCycleY = rowSeasonSelY + rowStep, row0Y = rowSeasonCycleY + rowStep, row1Y = row0Y + rowStep, row2Y = row1Y + rowStep, row3Y = row2Y + rowStep, row4Y = row3Y + rowStep, row5Y = row4Y + rowStep;
            const float envW = 305.0f - envL;
            const float w5 = (envW - 4.0f * envGap) / 5.0f;
            const float w3 = (envW - 2.0f * envGap) / 3.0f;
            const float px0 = envL, px1 = envL + (w3 + envGap), px2 = envL + 2.0f * (w3 + envGap);
            if (touch.py >= rowSeasonSelY && touch.py <= rowSeasonSelY + rowH) {
                for (u32 s = 0; s < 5; s++) {
                    float sx = envL + (float)s * (w5 + envGap);
                    if (touch.px >= sx && touch.px <= sx + w5) {
                        settings.envSeasonMode = s;
                        settingsDirty = true;
                        activePresetSlot = -1;
                    }
                }
            }
            if (touch.py >= rowSeasonCycleY && touch.py <= rowSeasonCycleY + rowH) {
                if (touch.px >= px0 && touch.px <= px0 + w3) {
                    settings.envSeasonMode = (settings.envSeasonMode == 5) ? 0 : 5;
                    settingsDirty = true;
                    activePresetSlot = -1;
                }
                if (touch.px >= px1 && touch.px <= px1 + w3) {
                    settings.envSeasonMode = (settings.envSeasonMode == 6) ? 0 : 6;
                    settingsDirty = true;
                    activePresetSlot = -1;
                }
                if (touch.px >= px2 && touch.px <= px2 + w3) {
                    settings.envSeasonCycleIntervalIndex = (settings.envSeasonCycleIntervalIndex + 1) % 4;
                    settingsDirty = true;
                }
            }
            if (touch.py >= row0Y && touch.py <= row0Y + rowH) {
                for (u32 p = 0; p < 5; p++) {
                    float px = envL + (float)p * (w5 + envGap);
                    if (touch.px >= px && touch.px <= px + w5) {
                        settings.envPhaseMode = p;
                        settingsDirty = true;
                        activePresetSlot = -1;
                    }
                }
            }
            if (touch.py >= row1Y && touch.py <= row1Y + rowH) {
                if (touch.px >= px0 && touch.px <= px0 + w3) {
                    settings.envPhaseMode = (settings.envPhaseMode == 5) ? 0 : 5;
                    settingsDirty = true;
                    activePresetSlot = -1;
                }
                if (touch.px >= px1 && touch.px <= px1 + w3) {
                    settings.envPhaseMode = (settings.envPhaseMode == 6) ? 0 : 6;
                    settingsDirty = true;
                    activePresetSlot = -1;
                }
                if (touch.px >= px2 && touch.px <= px2 + w3) {
                    settings.envPhaseCycleIntervalIndex = (settings.envPhaseCycleIntervalIndex + 1) % 4;
                    settingsDirty = true;
                }
            }
            if (touch.py >= row2Y && touch.py <= row2Y + rowH) {
                float halfW = (envW - envGap) * 0.5f;
                float speedX = envL, dirX = envL + halfW + envGap;
                if (touch.px >= dirX && touch.px <= dirX + halfW) {
                    settings.envCloudDirection = (settings.envCloudDirection + 1) % 4;
                    settingsDirty = true;
                    activePresetSlot = -1;
                }
                if (touch.px >= speedX && touch.px <= speedX + rowH) {
                    if (settings.envCloudSpeedLevel > 0) settings.envCloudSpeedLevel--;
                    settingsDirty = true;
                    activePresetSlot = -1;
                }
                if (touch.px >= speedX + halfW - rowH && touch.px <= speedX + halfW) {
                    if (settings.envCloudSpeedLevel < 10) settings.envCloudSpeedLevel++;
                    settingsDirty = true;
                    activePresetSlot = -1;
                }
            }
            if (touch.py >= row3Y && touch.py <= row3Y + rowH) {
                float halfW = (envW - envGap) * 0.5f;
                float moodX = envL, weatherX = envL + halfW + envGap;
                if (touch.px >= moodX && touch.px <= moodX + halfW) {
                    settings.bgThemeIndex = (settings.bgThemeIndex + 1) % 5;
                    settingsDirty = true;
                    activePresetSlot = -1;
                }
                if (touch.px >= weatherX && touch.px <= weatherX + halfW) {
                    cycleWeatherMode();
                    activePresetSlot = -1;
                }
            }
            if (touch.py >= row4Y && touch.py <= row4Y + rowH) {
                float unitW = (envW - 4.0f * envGap) / 6.0f;
                float saveW = unitW * 2.0f;
                float slotW = unitW;
                float saveX = envL;
                float slot1X = saveX + saveW + envGap;
                float slot2X = slot1X + slotW + envGap;
                float slot3X = slot2X + slotW + envGap;
                float slot4X = slot3X + slotW + envGap;
                float slotXs[4] = {slot1X, slot2X, slot3X, slot4X};

                if (touch.px >= saveX && touch.px <= saveX + saveW) {
                    presetSaveMode = !presetSaveMode;
                }
                for (int s = 0; s < 4; s++) {
                    if (touch.px >= slotXs[s] && touch.px <= slotXs[s] + slotW) {
                        if (presetSaveMode) {
                            saveCurrentIntoPreset(s);
                            presetSaveMode = false;
                        } else {
                            recallPreset(s);
                        }
                    }
                }
            }
            if (touch.py >= row5Y && touch.py <= row5Y + rowH && touch.px >= envL && touch.px <= envL + ((envW - 4.0f * envGap) / 6.0f) * 2.0f) {
                currentScreen = SCREEN_MAIN;
            }
        }
        else if (currentScreen == SCREEN_CREDITS) {
            if (touch.px >= 105 && touch.px <= 215 && touch.py >= 185 && touch.py <= 217)
                currentScreen = SCREEN_RECORDS;
        }
    }
    if (kDown & KEY_START) return 1;
    return 0;
}
int main() {
    gfxInitDefault();
    C3D_Init(0x10000);
    C2D_Init(C2D_DEFAULT_MAX_OBJECTS);
    C2D_Prepare();
    static u32* socBuffer = NULL;
    socBuffer = (u32*)memalign(0x1000, 0x100000);
    if (socBuffer) socInit(socBuffer, 0x100000);
    httpcInit(0);
    ptmuInit();
    mcuHwcInit();
    acInit();
    romfsInit();
    C2D_SpriteSheet forestSheet = C2D_SpriteSheetLoad("romfs:/gfx/forest_bg.t3x");
    if (forestSheet) {
        for (int i = 0; i < FOREST_PHASE_COUNT; i++) {
            forestPhaseImages[i] = C2D_SpriteSheetGetImage(forestSheet, i);
        }
        cloudImage = C2D_SpriteSheetGetImage(forestSheet, FOREST_PHASE_COUNT);
        cloudImage2 = C2D_SpriteSheetGetImage(forestSheet, FOREST_PHASE_COUNT + 1);
        owlImage1 = C2D_SpriteSheetGetImage(forestSheet, FOREST_PHASE_COUNT + 2);
        owlImage2 = C2D_SpriteSheetGetImage(forestSheet, FOREST_PHASE_COUNT + 3);
        owlImage3dx = C2D_SpriteSheetGetImage(forestSheet, FOREST_PHASE_COUNT + 4);
        owlImage3sx = C2D_SpriteSheetGetImage(forestSheet, FOREST_PHASE_COUNT + 5);
        owlImage3dxd = C2D_SpriteSheetGetImage(forestSheet, FOREST_PHASE_COUNT + 6);
        owlImage3sxd = C2D_SpriteSheetGetImage(forestSheet, FOREST_PHASE_COUNT + 7);
        hedgehogImageDx = C2D_SpriteSheetGetImage(forestSheet, FOREST_PHASE_COUNT + 8);
        hedgehogImageSx = C2D_SpriteSheetGetImage(forestSheet, FOREST_PHASE_COUNT + 9);
        hedgehogImageDxB = C2D_SpriteSheetGetImage(forestSheet, FOREST_PHASE_COUNT + 10);
        hedgehogImageSxB = C2D_SpriteSheetGetImage(forestSheet, FOREST_PHASE_COUNT + 11);
        cloudImage3 = C2D_SpriteSheetGetImage(forestSheet, FOREST_PHASE_COUNT + 12);
        cloudImage4 = C2D_SpriteSheetGetImage(forestSheet, FOREST_PHASE_COUNT + 13);
        cloudImage5 = C2D_SpriteSheetGetImage(forestSheet, FOREST_PHASE_COUNT + 14);
        robinImage = C2D_SpriteSheetGetImage(forestSheet, FOREST_PHASE_COUNT + 15);
        robinImage2 = C2D_SpriteSheetGetImage(forestSheet, FOREST_PHASE_COUNT + 16);
        robinFlyImageSx = C2D_SpriteSheetGetImage(forestSheet, FOREST_PHASE_COUNT + 17);
        robinFlyImageDx = C2D_SpriteSheetGetImage(forestSheet, FOREST_PHASE_COUNT + 18);
        robinFlyImageSxB = C2D_SpriteSheetGetImage(forestSheet, FOREST_PHASE_COUNT + 19);
        robinFlyImageDxB = C2D_SpriteSheetGetImage(forestSheet, FOREST_PHASE_COUNT + 20);
        robinPeckDx = C2D_SpriteSheetGetImage(forestSheet, FOREST_PHASE_COUNT + 21);
        robinPeckSx = C2D_SpriteSheetGetImage(forestSheet, FOREST_PHASE_COUNT + 22);
        nestImage = C2D_SpriteSheetGetImage(forestSheet, FOREST_PHASE_COUNT + 23);
        robinSleepImage = C2D_SpriteSheetGetImage(forestSheet, FOREST_PHASE_COUNT + 24);
        greyBirdSleepImage = C2D_SpriteSheetGetImage(forestSheet, FOREST_PHASE_COUNT + 25);
        greyBirdImageDx = C2D_SpriteSheetGetImage(forestSheet, FOREST_PHASE_COUNT + 26);
        greyBirdPeckDx = C2D_SpriteSheetGetImage(forestSheet, FOREST_PHASE_COUNT + 27);
        greyBirdFlyImageDx = C2D_SpriteSheetGetImage(forestSheet, FOREST_PHASE_COUNT + 28);
        greyBirdFlyImageDxB = C2D_SpriteSheetGetImage(forestSheet, FOREST_PHASE_COUNT + 29);
        greyBirdImageSx = C2D_SpriteSheetGetImage(forestSheet, FOREST_PHASE_COUNT + 30);
        greyBirdPeckSx = C2D_SpriteSheetGetImage(forestSheet, FOREST_PHASE_COUNT + 31);
        greyBirdFlyImageSx = C2D_SpriteSheetGetImage(forestSheet, FOREST_PHASE_COUNT + 32);
        greyBirdFlyImageSxB = C2D_SpriteSheetGetImage(forestSheet, FOREST_PHASE_COUNT + 33);
        hareStandSxImage = C2D_SpriteSheetGetImage(forestSheet, FOREST_PHASE_COUNT + 34);
        hareWalk1Image = C2D_SpriteSheetGetImage(forestSheet, FOREST_PHASE_COUNT + 35);
        hareWalk2Image = C2D_SpriteSheetGetImage(forestSheet, FOREST_PHASE_COUNT + 36);
        hareStandDxImage = C2D_SpriteSheetGetImage(forestSheet, FOREST_PHASE_COUNT + 37);
        hareDx1Image = C2D_SpriteSheetGetImage(forestSheet, FOREST_PHASE_COUNT + 38);
        hareDx2Image = C2D_SpriteSheetGetImage(forestSheet, FOREST_PHASE_COUNT + 39);
        hareGrazeDxImage = C2D_SpriteSheetGetImage(forestSheet, FOREST_PHASE_COUNT + 40);
        hareGrazeSxImage = C2D_SpriteSheetGetImage(forestSheet, FOREST_PHASE_COUNT + 41);
        hareGrazeSx2Image = C2D_SpriteSheetGetImage(forestSheet, FOREST_PHASE_COUNT + 42);
        hareGrazeDx2Image = C2D_SpriteSheetGetImage(forestSheet, FOREST_PHASE_COUNT + 43);
        hareStandSxBImage = C2D_SpriteSheetGetImage(forestSheet, FOREST_PHASE_COUNT + 44);
        hareStandDxBImage = C2D_SpriteSheetGetImage(forestSheet, FOREST_PHASE_COUNT + 45);
        foxSx1Image = C2D_SpriteSheetGetImage(forestSheet, FOREST_PHASE_COUNT + 46);
        foxSx2Image = C2D_SpriteSheetGetImage(forestSheet, FOREST_PHASE_COUNT + 47);
        foxSx3Image = C2D_SpriteSheetGetImage(forestSheet, FOREST_PHASE_COUNT + 48);
        foxSx4Image = C2D_SpriteSheetGetImage(forestSheet, FOREST_PHASE_COUNT + 49);
        foxSx5Image = C2D_SpriteSheetGetImage(forestSheet, FOREST_PHASE_COUNT + 50);
        foxSxStandImage = C2D_SpriteSheetGetImage(forestSheet, FOREST_PHASE_COUNT + 51);
    }
    C2D_SpriteSheet springSheet = C2D_SpriteSheetLoad("romfs:/gfx/forestspring.t3x");
    if (springSheet) {
        seasonSpringDayImage = C2D_SpriteSheetGetImage(springSheet, 0);
        seasonSpringDawnImage = C2D_SpriteSheetGetImage(springSheet, 1);
        seasonSpringDuskImage = C2D_SpriteSheetGetImage(springSheet, 2);
        seasonSpringNightImage = C2D_SpriteSheetGetImage(springSheet, 3);
    }
    C2D_SpriteSheet autumnSheet = C2D_SpriteSheetLoad("romfs:/gfx/forestautumn.t3x");
    if (autumnSheet) {
        seasonAutumnDayImage = C2D_SpriteSheetGetImage(autumnSheet, 0);
        seasonAutumnDawnImage = C2D_SpriteSheetGetImage(autumnSheet, 1);
        seasonAutumnDuskImage = C2D_SpriteSheetGetImage(autumnSheet, 2);
        seasonAutumnNightImage = C2D_SpriteSheetGetImage(autumnSheet, 3);
    }
    C2D_SpriteSheet winterSheet = C2D_SpriteSheetLoad("romfs:/gfx/forestwinter.t3x");
    if (winterSheet) {
        seasonWinterDayImage = C2D_SpriteSheetGetImage(winterSheet, 0);
        seasonWinterDawnImage = C2D_SpriteSheetGetImage(winterSheet, 1);
        seasonWinterDuskImage = C2D_SpriteSheetGetImage(winterSheet, 2);
        seasonWinterNightImage = C2D_SpriteSheetGetImage(winterSheet, 3);
    }
    {
        time_t rawStart = time(NULL);
        struct tm* tmStart = localtime(&rawStart);
        float hourFracStart = tmStart->tm_hour + tmStart->tm_min / 60.0f;
        currentForestPhase = previousForestPhase = getForestPhaseForHour(hourFracStart);
    }
    C3D_RenderTarget* top    = C2D_CreateScreenTarget(GFX_TOP,    GFX_LEFT);
    C3D_RenderTarget* bottom = C2D_CreateScreenTarget(GFX_BOTTOM, GFX_LEFT);
    C2D_TextBuf topBuf    = C2D_TextBufNew(1024);
    C2D_TextBuf bottomBuf = C2D_TextBufNew(4096);
    mkdir(SAVE_DIR, 0777);
    loadRecords();
    loadSettings();
    loadEnvPresets();
    loadDiary();
    loadNotes();
    { // Risolve currentEnvSeason all'avvio in base a envSeasonMode, per evitare un fade indesiderato al primo frame
        u32 startSeasonMode = settings.envSeasonMode;
        if (startSeasonMode == 0) {
            time_t rawStartSeason = time(NULL);
            struct tm* tmStartSeason = localtime(&rawStartSeason);
            currentEnvSeason = getSeasonForDate(tmStartSeason->tm_mon + 1, tmStartSeason->tm_mday);
        } else if (startSeasonMode != 5 && startSeasonMode != 6) {
            static const u32 envModeToSeasonStart[5] = { 1, 0, 1, 2, 3 };
            currentEnvSeason = envModeToSeasonStart[startSeasonMode];
        } // Cycle/Random: resta il default 1 (Summer), poi cicla da sé
    }
    fetchWeatherFromInternet(); // tentativo silenzioso: se fallisce (niente WiFi, location non impostata, ecc.) resta il meteo di default (Clear)
    loadAlarm();
    srand(time(NULL));
    initColorPresets();
    initForestSky();
    initRain();
    initSnow();
    initBottomRain();
    initBottomSnow();
    initStaticTexts();
    initAudio();
    initAmbientAudio();
    initRainAudio();
    initDayAudio();
    initNightAudio();
    initOwlAudio();
    while (aptMainLoop()) {
        if (aptShouldJumpToHome()) {
            aptJumpToHomeMenu();
        }
        if (updateInput()) break;
        updateEnvPhaseCycle();
        updateEnvSeasonCycle();
        updateWeatherFromType();
        updateWeatherSchedule();
        sessionFrames++;
        fpsFrameCount++;
        u64 fpsNow = osGetTime();
        if (fpsWindowStart == 0) fpsWindowStart = fpsNow;
        u64 fpsElapsed = fpsNow - fpsWindowStart;
        if (fpsElapsed >= 1000) {
            fpsDisplayValue = (float)fpsFrameCount * 1000.0f / (float)fpsElapsed;
            fpsFrameCount = 0;
            fpsWindowStart = fpsNow;
            u8 rawBatteryLevel = 0;
            if (R_SUCCEEDED(PTMU_GetBatteryLevel(&rawBatteryLevel))) {
                batteryPercentApprox = (int)rawBatteryLevel * 20; // il 3DS espone solo 0-5 "a scatti", non una percentuale precisa
            }
            PTMU_GetBatteryChargeState(&batteryCharging);
            u8 rawBatteryPrecise = 0;
            if (R_SUCCEEDED(MCUHWC_GetBatteryLevel(&rawBatteryPrecise))) {
                batteryPercentPrecise = (int)rawBatteryPrecise; // già 0-100 reale
            }
            static int wifiCheckCounter = 0;
            wifiCheckCounter++;
            if (wifiCheckCounter >= 5) {
                wifiCheckCounter = 0;
                wifiConnectedCache = isWifiConnected();
                wifiStrengthCache = osGetWifiStrength();
            }
        }
        if (sessionFrames >= 3600) {
            records.totalTimeSeconds += 60;
            sessionFrames -= 3600;
        }
        time_t raw = time(NULL);
        struct tm* tmv = localtime(&raw);
        if (alarmCfg.enabled && !alarmRinging &&
            tmv->tm_hour == (int)alarmCfg.hour && tmv->tm_min == (int)alarmCfg.minute &&
            !(alarmLastTrigHour == tmv->tm_hour && alarmLastTrigMin == tmv->tm_min)) {
            startAlarmRinging(false);
            alarmLastTrigHour = tmv->tm_hour;
            alarmLastTrigMin  = tmv->tm_min;
        }
        updateAlarmRinging();
        updateTimer();
        updateAmbientSounds();
        updateRainAudio();
        updateDayAudio();
        updateNightAudio();
        updateOwlAudio();
        globalAnimTime += 0.0166f;
        C3D_FrameBegin(C3D_FRAME_SYNCDRAW);
        C2D_TargetClear(top, C2D_Color32(0, 0, 0, 255));
        C2D_SceneBegin(top);
        C2D_TextBufClear(topBuf);
        drawBackground();
        if (settings.clockMode != 2) drawClock(topBuf, tmv);
        drawTimerCountdown(topBuf);
        drawDebugPointer(topBuf);
        C2D_TextBufClear(bottomBuf);
        drawBottomScreen(bottomBuf, bottom);
        C3D_FrameEnd(C3D_FRAME_SYNCDRAW);
    }
    if (settingsDirty) saveSettings();
    records.totalTimeSeconds += sessionFrames / 60;
    saveRecords();
    if (alarmDirty) saveAlarm();
    exitAudio();
    C2D_TextBufDelete(topBuf);
    C2D_TextBufDelete(bottomBuf);
    C2D_TextBufDelete(staticBuf);
    if (forestSheet) C2D_SpriteSheetFree(forestSheet);
    romfsExit();
    httpcExit();
    ptmuExit();
    mcuHwcExit();
    acExit();
    if (socBuffer) { socExit(); free(socBuffer); }
    C2D_Fini();
    C3D_Fini();
    gfxExit();
    return 0;
}