#pragma once

#include "WwiseItemType.h"
#include "Misc/TextFilter.h"
#include "WwiseBrowser/WwiseBrowserForwards.h"

typedef TTextFilter< const FString& > StringFilter;

class IWwiseBrowserDataSource
{
public:
	virtual ~IWwiseBrowserDataSource() = default;

	virtual bool Init() = 0;

	virtual void ConstructTree(bool bShouldRefresh) = 0;

	// Constructs the top-level root for the given type, along with its direct children
	virtual FWwiseTreeItemPtr ConstructTreeRoot(EWwiseItemType::Type Type) = 0;

	// Loads children for the item matching first InParentId, then InParentPath
	virtual int32 LoadChildren(const FGuid& InParentId, const FString& InParentPath, TArray<FWwiseTreeItemPtr>& OutChildren) = 0;

	// Loads children for the item
	virtual int32 LoadChildren(FWwiseTreeItemPtr ParentTreeItem) = 0;

	virtual int32 GetChildItemCount(const FWwiseTreeItemPtr& InParentIrem) = 0;

	virtual FWwiseTreeItemPtr GetRootItem(EWwiseItemType::Type RootType) = 0;

	virtual FWwiseTreeItemPtr LoadFilteredRootItem(EWwiseItemType::Type Type, TSharedPtr<StringFilter> CurrentFilterText) = 0;
};
