#ifndef _KEY_CONFIG
#define _KEY_CONFIG

extern void KeyConfigLoad(int[3][4], int *);
extern void KeyConfigSave(int[3][4], int*);
extern int KeyNameToInt(wchar_t *);
extern wchar_t* IntToKeyName(int);

#endif