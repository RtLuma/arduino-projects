#ifndef SUART
#define SUART

void xmit(char);
void xmitstr(const char*);
void xmitval(int16_t, int8_t, int8_t);
void xmitf(const char*, ...);
uint8_t rcvr();
void rcvrstr(char *, uint8_t);
uint8_t pickval(char **, uint16_t *, uint8_t);

#endif	/* SUART */
