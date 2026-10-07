#include "Extended_BPLib.h"

int32 UExtendedVarsBPLibrary::GetActiveChildrenCount(UPanelWidget* PanelWidget)
{
	if (!IsValid(PanelWidget))
	{
		return 0;
	}

	const TArray<UWidget*> ChildrenWidgets = PanelWidget->GetAllChildren();
	int32 ActiveCount = 0;

	for (UWidget* Each_Child : ChildrenWidgets)
	{
		if (IsValid(Each_Child) && Each_Child->IsVisible() && Each_Child->GetIsEnabled())
		{
			ActiveCount++;
		}
	}

	return ActiveCount;
}