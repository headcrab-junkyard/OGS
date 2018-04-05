/// @file

#pragma once

void GameUI_StartProgressBar(const char *progressType, int progressSteps);
int GameUI_ContinueProgressBar(int progressPoint, float progressFraction);
void GameUI_StopProgressBar(bool bError, const char *failureReason, const char *extendedReason);

int GameUI_SetProgressBarStatusText(const char *statusText);

void GameUI_SetSecondaryProgressBar(float progress);
void GameUI_SetSecondaryProgressBarText(const char *statusText);