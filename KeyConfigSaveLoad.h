#ifndef _KEY_CONFIG_SAVE_LOAD
#define _KEY_CONFIG_SAVE_LOAD

extern void KeyConfigLoad(int[3][4], int *);
extern void KeyConfigSave(int[3][4], int*);
extern int KeyNameToInt(wchar_t *);
extern wchar_t* IntToKeyName(int);

#endif