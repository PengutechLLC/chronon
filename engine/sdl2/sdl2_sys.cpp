/*
Copyright (C) 1997-2001 Id Software, Inc.
Copyright (C) 2020-2021 Mark E Sowden <markelswo@gmail.com>

This program is free software; you can redistribute it and/or
modify it under the terms of the GNU General Public License
as published by the Free Software Foundation; either version 2
of the License, or (at your option) any later version.

This program is distributed in the hope that it will be useful,
but WITHOUT ANY WARRANTY; without even the implied warranty of
MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.

See the GNU General Public License for more details.

You should have received a copy of the GNU General Public License
along with this program; if not, write to the Free Software
Foundation, Inc., 59 Temple Place - Suite 330, Boston, MA  02111-1307, USA.

*/

#include "qcommon/qcommon.h"

#include <SDL2/SDL.h>

#include "../client/keys.h"


/*
========================================================================
GAME DLL
========================================================================
*/

static void *gameDll = nullptr;

void Sys_UnloadGame()
{
	if ( gameDll == nullptr )
	{
		return;
	}

	SDL_UnloadObject( gameDll );
}

#define GAME_MODULE_NAME "game"
#if defined( _WIN32 )
#	define GAME_MODULE_EXT ".dll"
#else
#	define GAME_MODULE_EXT ".so"
#endif
#define GAME_MODULE_PATH GAME_MODULE_NAME GAME_MODULE_EXT

/**
 * Loads the game dll.
 */
void *Sys_GetGameAPI( void *parms )
{
	if ( gameDll != nullptr )
	{
		Com_Error( ERR_FATAL, "Sys_GetGameAPI without Sys_UnloadingGame" );
	}

	char *path = nullptr;
	while ( ( path = FS_NextPath( path ) ) != nullptr )
	{
		char fullPath[ MAX_OSPATH ];
		snprintf( fullPath, sizeof( fullPath ), "%s/%s", path, GAME_MODULE_PATH );
		gameDll = SDL_LoadObject( fullPath );
		if ( gameDll != nullptr )
		{
			Com_DPrintf( "LoadLibrary (%s)\n", fullPath );
			break;
		}
	}

	typedef void *( *GetGameAPI )( void * );
	auto gameApi = ( GetGameAPI ) SDL_LoadFunction( gameDll, "GetGameAPI" );
	if ( gameApi == nullptr )
	{
		Sys_UnloadGame();
		return nullptr;
	}

	return gameApi( parms );
}
