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

#ifndef __MGLI_GETENV_H__
#define __MGLI_GETENV_H__

#include <Files.h>

#define MGLI_CONFIG_FILE_NAME		"MesaSettings"

/********************************************************************
 * Function: MCFG_readMesaConfigurationFile                         *
 *																	*
 * Description: Read's the configuration file from current          *
 * directory's MesaSettings file.                                   *	
 * 																	*
 ********************************************************************/
void MCFG_readMesaConfigurationFile(char *filename);
/********************************************************************
 * Function: MCFG_readMesaConfigurationFileFromFSSpec               *
 *																	*
 * Description: Read's the configuration file from MesaSettings     *	
 * 																	*
 ********************************************************************/
Boolean MCFG_readMesaConfigurationFileFromFSSpec(const FSSpec *fileSpec);
/********************************************************************
 * Function: MCFG_readMesaConfigurationFile                         *
 *																	*
 * Description: Read's the confituration file memory.               *	
 * 																	*
 ********************************************************************/
void MCFG_setMesaConfigurationFile(int line_num,char **lines);
/********************************************************************
 * Function: MCFG_getEnv                         					*
 *																	*
 * Description: Get's the value of a confiuration named name,       *
 *	            returns null if this variable wasn't defined        *	
 * 																	*
 ********************************************************************/
char *MCFG_getEnv(const char *name);
/********************************************************************
 * Function: MCFG_disposeConfigData                         		*
 *																	*
 * Description: Destroys data used by the configurtaion infos.      *	
 * 																	*
 ********************************************************************/
void MCFG_disposeConfigData(void);
#endif /* __MGLI_GETENV_H__ */