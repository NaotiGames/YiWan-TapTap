// Fill out your copyright notice in the Description page of Project Settings.

#include "AntiAddictionError.h"

 bool FAntiAddictionError::IsTokenExpired() const
{
	if(!error.IsEmpty() && error.Equals("business_code_error"))
	{
		return code == 200000 ;
	}
 	return false;
}