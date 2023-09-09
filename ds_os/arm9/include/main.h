#ifndef _MAIN_H_
#define _MAIN_H_
typedef bool (*gotomenu)();
typedef void (*Actapp)();
typedef struct{
char* name;
u16* icon;
gotomenu go;
Actapp Act;
}APP;

extern u16* sfondo;

int InitApp(gotomenu go, Actapp Act, const char* name, const char* icon);
void HandleApps();
void SetCurApp(int cur);
#endif
