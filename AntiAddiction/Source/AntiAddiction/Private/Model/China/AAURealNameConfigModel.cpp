#include "AAURealNameConfigModel.h"

#include "TUDebuger.h"

#include "Server/AAUStorage.h"


TSharedPtr<FAAURealNameConfigModel> FAAURealNameConfigModel::CurrentModel = nullptr;



void FAAURealNameConfigModel::SaveToLocal(TSharedPtr<FAAURealNameConfigModel> Model) {
	FAAURealNameConfigModel::CurrentModel = Model;
	TUDataStorage<FAAUStorage>::SaveStruct(FAAUStorage::RealNameConfig, Model, true);
}

TSharedPtr<FAAURealNameConfigModel> FAAURealNameConfigModel::GetLocalModel() {
	if (CurrentModel.IsValid()) {
		return CurrentModel;
	}
	// 先读取上次保存服务的配置，如果没有的话，在读取插件中预置的配置。
	TSharedPtr<FAAURealNameConfigModel> ModelPtr = TUDataStorage<FAAUStorage>::LoadStruct<FAAURealNameConfigModel>(FAAUStorage::RealNameConfig);
	if (!ModelPtr.IsValid()) {
		TUDebuger::ErrorLog("AntiAddiction Can not Get Local Model");
	}
	CurrentModel = ModelPtr;
	return ModelPtr;
}

void FAAURealNameConfigModel::ResetModel()
{
	FAAURealNameConfigModel::CurrentModel = nullptr;
}

