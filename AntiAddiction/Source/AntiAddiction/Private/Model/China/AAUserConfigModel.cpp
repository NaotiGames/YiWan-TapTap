#include "AAUserConfigModel.h"


#include "TUDebuger.h"

#include "Server/AAUStorage.h"


TSharedPtr<FAAUserConfigModel> FAAUserConfigModel::CurrentModel = nullptr;



void FAAUserConfigModel::SaveToLocal(const FString& UserId,TSharedPtr<FAAUserConfigModel> Model) {
	FAAUserConfigModel::CurrentModel = Model;
	TUDataStorage<FAAUStorage>::SaveStruct(FAAUStorage::UserConfig+UserId, Model, true);
}

TSharedPtr<FAAUserConfigModel> FAAUserConfigModel::GetLocalModel() {
	if (FAAUserConfigModel::CurrentModel.IsValid()) {
		return FAAUserConfigModel::CurrentModel;
	}
	// 先读取上次保存服务的配置，如果没有的话，在读取插件中预置的配置。
	TSharedPtr<FAAUserConfigModel> ModelPtr = TUDataStorage<FAAUStorage>::LoadStruct<FAAUserConfigModel>(FAAUStorage::RealNameConfig);
	if (!ModelPtr.IsValid()) {
		TUDebuger::ErrorLog("AntiAddiction Can not Get Local Model");
	}
	FAAUserConfigModel::CurrentModel = ModelPtr;
	return ModelPtr;
}

void FAAUserConfigModel::ResetModel()
{
	FAAUserConfigModel::CurrentModel = nullptr;
}

