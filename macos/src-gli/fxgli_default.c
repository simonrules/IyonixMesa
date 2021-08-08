/*
 * Mesa 3-D graphics library MacOS Driver
 * Version:  3.2
 *
 * Copyright (C) 1999  Mikl—s Fazekas   All Rights Reserved.
 *
 * Permission is hereby granted, free of charge, to any person obtaining a
 * copy of this software and associated documentation files (the "Software"),
 * to deal in the Software without restriction, including without limitation
 * the rights to use, copy, modify, merge, publish, distribute, sublicense,
 * and/or sell copies of the Software, and to permit persons to whom the
 * Software is furnished to do so, subject to the following conditions:
 *
 * The above copyright notice and this permission notice shall be included
 * in all copies or substantial portions of the Software.
 *
 * THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS
 * OR IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY,
 * FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT.  IN NO EVENT SHALL
 * BRIAN PAUL BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER LIABILITY, WHETHER IN
 * AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING FROM, OUT OF OR IN
 * CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER DEALINGS IN THE SOFTWARE.
 */

#include "fxgli.h"
#include <string.h>

int fxgli_GetDefaultTextureMemorySize(void)
{
	int def = 0;
	if (MCFG_getEnv(kEnv_CheatTextureMemory) != NULL)
	{
		int ans = atoi(MCFG_getEnv(kEnv_CheatTextureMemory));
		if (ans > 0)
			def = ans;
	}
	return def;
}
int fxgli_GetDefaultVideoMemorySize(void)
{
	int def = 0;
	if (MCFG_getEnv(kEnv_CheatVideoMemory) != NULL)
	{
		int ans = atoi(MCFG_getEnv(kEnv_CheatVideoMemory));
		if (ans > 0) 
			def = ans;
	}
	return def;
}
int fxgli_GetDefaultDitherMode(void)
{
	int def = -1;
	if (MCFG_getEnv(kEnv_DitherMode) != NULL)
	{
		int ans = atoi(MCFG_getEnv(kEnv_DitherMode));
		if (ans == 0 || ans == 2 || ans == 4)
		{ 
			def = ans;
		}
	}
	return def;
}

int fxgli_GetDefaultScreenRefresh(void)
{
	int def = 0;
	if (MCFG_getEnv(kEnv_ScreenRefresh) != NULL)
	{
		int ans = atoi(MCFG_getEnv(kEnv_ScreenRefresh));
		if (ans > 0)
		{ 
			def = ans;
		}
	}
	return def;	
}

int fxgli_GetDefaultGammaValue(void)
{ 
    float def = 0.0;
    
	if (MCFG_getEnv(kEnv_DefaultGammaValue) != NULL)
	{
		float ans = atof(MCFG_getEnv(kEnv_DefaultGammaValue));
		if ((ans > 0.5) || (ans < 5.0))
			def = ans;
	}
	
	return (int)(65536*def);
}			
int fxgli_GetDefaultSwapIntervall(void)
{
	int def = 1;
	
	if (MCFG_getEnv(kEnv_SwapIntervall))
	{
		int ans = atoi(MCFG_getEnv(kEnv_SwapIntervall));
		if (ans >= 0 || ans <= 2)
			def = ans;
	}
	return def;	
}

int	fxgli_GetDefaultMaxSwapPendingBuffers(void)
{
	int def = 2;
	
	if (MCFG_getEnv("MESA_3DFX_MAX_PENDING_SWAP_BUFFERS"))
	{
		int ans = atoi(MCFG_getEnv("MESA_3DFX_MAX_PENDING_SWAP_BUFFERS"));
		if (ans >= 0 || ans <= 5)
			def = ans;	
	}
	return def;
}

GLboolean fxgli_IsEmulateWindowRendering(void)
{
	if (MCFG_getEnv(kEnv_EmulateWindowRendering))
		return GL_TRUE;
	else
		return GL_FALSE;
}
GLboolean fxgli_IsEmulateCompliance(void)
{
	if (MCFG_getEnv(kEnv_EmulateCompliance))
		return GL_TRUE;
	else
		return GL_FALSE;
}
GLboolean fxgli_IsEmulateCursor(void)
{
	if (MCFG_getEnv(kEnv_EmulateMouseCursor))
		return GL_TRUE;
	else
		return GL_FALSE;
}