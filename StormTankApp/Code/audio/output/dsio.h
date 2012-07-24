
#ifndef _DSIO_H_
#define _DSIO_H_


typedef void (__stdcall DSIOCALLBACK)(void *parm, float *buf, uint32_t len);

extern "C" {
	extern uint32_t __stdcall dsInit(DSIOCALLBACK *callback, void *parm, void *hWnd);
	extern void __stdcall dsClose();
	extern int32_t __stdcall dsGetCurSmp();
	extern void __stdcall dsSetVolume(float vol);
  extern void __stdcall dsTick();
  extern void __stdcall dsLock();
  extern void __stdcall dsUnlock();
}

#endif