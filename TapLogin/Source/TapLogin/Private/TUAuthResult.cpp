#include "TUAuthResult.h"

TUAuthResult TUAuthResult::CancelInit()
{
	return TUAuthResult();
}

TUAuthResult TUAuthResult::SuccessInit(const TSharedPtr<FTUAccessToken>& Token)
{
	TUAuthResult Result;
	Result.ResultType = Success;
	Result.Token = Token;
	return Result;
}

TUAuthResult TUAuthResult::FailInit(const FTUError& Error)
{
	TUAuthResult Result;
	Result.ResultType = Fail;
	Result.Error = MakeShareable(new FTUError(Error));
	return Result;
}

enum TUAuthResult::Type TUAuthResult::GetType() const {
	return ResultType;
}

TSharedPtr<FTUAccessToken> TUAuthResult::GetToken() const {
	return Token;
}

TSharedPtr<FTUError> TUAuthResult::GetError() const {
	return Error;
}

TUAuthResult::TUAuthResult()
{
	ResultType = Cancel;
	Token = nullptr;
	Error = nullptr;
}

