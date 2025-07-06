//
// Created by Square on 7/5/2025.

#pragma once


/**
 * Determine if a debugger is currently attached to the game.
 * @note On Windows this will simply call IsDebuggerPresent
 * @note On Linux this will parse the "/proc/self/status" file and look for a non-zero TracerPid
 * @return True when game is being debugged.
 */
bool is_debugger_attached();

