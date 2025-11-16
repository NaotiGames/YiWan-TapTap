// Fill out your copyright notice in the Description page of Project Settings.


#include "PermissionCheckBoxWidgetStyle.h"


FPermissionCheckBoxStyle::FPermissionCheckBoxStyle()
{
}

FPermissionCheckBoxStyle::~FPermissionCheckBoxStyle()
{
}

const FName FPermissionCheckBoxStyle::TypeName(TEXT("FPermissionCheckBoxStyle"));

const FPermissionCheckBoxStyle& FPermissionCheckBoxStyle::GetDefault()
{
	static FPermissionCheckBoxStyle Default;
	return Default;
}

void FPermissionCheckBoxStyle::GetResources(TArray<const FSlateBrush*>& OutBrushes) const
{
	PermissionCheckBoxStyle.GetResources(OutBrushes);
	PermissionTextBlockStyle.GetResources(OutBrushes);
}
