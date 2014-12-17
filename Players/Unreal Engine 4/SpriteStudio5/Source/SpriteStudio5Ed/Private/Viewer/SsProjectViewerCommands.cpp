#include "SpriteStudio5EdPrivatePCH.h"
#include "SsProjectViewerCommands.h"


void FSsProjectViewerCommands::RegisterCommands()
{
	UI_COMMAND(Play, "Play", "Play SsProject", EUserInterfaceActionType::ToggleButton, FInputGesture());
	UI_COMMAND(PrevFrame, "PrevFrame", "Back One Frame", EUserInterfaceActionType::Button, FInputGesture());
	UI_COMMAND(NextFrame, "NextFrame", "Forward One Frame", EUserInterfaceActionType::Button, FInputGesture());
	UI_COMMAND(DrawGrid, "Grid", "Draw Grid", EUserInterfaceActionType::ToggleButton, FInputGesture());
}
