#pragma once


class IBillboardLang {
public:
	virtual ~IBillboardLang() = default;

	virtual FString billboard_network_error_retry() = 0;

	virtual FString billboard_loading() = 0;
	
};

enum class ELanguageType : uint8;

class TUBillboardLanguage {
public:
	
	static TSharedPtr<IBillboardLang> GetCurrentLang();

private:

	static TSharedPtr<IBillboardLang> CurrentLang;
	static ELanguageType LanguageType;
};

class BillboardLangCN : public IBillboardLang {
public:
	virtual FString billboard_network_error_retry() override {
		return TEXT("页面加载失败，点击重试");
	}

	virtual FString billboard_loading() override {
		return TEXT("加载中...");
	}
	
};

class BillboardLangIO : public IBillboardLang {
public:
	virtual FString billboard_network_error_retry() override {
		return TEXT("Loading failed. Please try again.");
	}

	virtual FString billboard_loading() override {
		return TEXT("Loading...");
	}
	
};

class BillboardLangID : public IBillboardLang {
public:
	virtual FString billboard_network_error_retry() override {
		return TEXT("Gagal memuat halaman. Tekan untuk mencoba lagi.");
	}

	virtual FString billboard_loading() override {
		return TEXT("Sedang memuat...");
	}
};

class BillboardLangJA : public IBillboardLang {
public:
	virtual FString billboard_network_error_retry() override {
		return TEXT("ページの読み込みに失敗しました。再度タップしてお試しください。");
	}

	virtual FString billboard_loading() override {
		return TEXT("ロード中...");
	}
};

class BillboardLangKO : public IBillboardLang {
public:
	virtual FString billboard_network_error_retry() override {
		return TEXT("페이지를 로드하지 못했습니다. 다시 시도해 주세요.");
	}

	virtual FString billboard_loading() override {
		return TEXT("로딩 중...");
	}
};

class BillboardLangTH : public IBillboardLang {
public:
	virtual FString billboard_network_error_retry() override {
		return TEXT("ดาวน์โหลดหน้าดังกล่าวล้มเหลว กรุณาลองใหม่อีกครั้ง");
	}

	virtual FString billboard_loading() override {
		return TEXT("กำลังดาวน์โหลด...");
	}
};

class BillboardLangZHTW : public IBillboardLang {
public:
	virtual FString billboard_network_error_retry() override {
		return TEXT("頁面載入失敗，點擊重試");
	}

	virtual FString billboard_loading() override {
		return TEXT("載入中...");
	}
};


class BillboardLangDE : public IBillboardLang {
public:
	virtual FString billboard_network_error_retry() override {
		return TEXT("Der Ladevorgang ist fehlgeschlagen. Versuche es bitte erneut.");
	}

	virtual FString billboard_loading() override {
		return TEXT("Laden...");
	}
};


class BillboardLangES : public IBillboardLang {
public:
	virtual FString billboard_network_error_retry() override {
		return TEXT("Error de carga. Por favor, inténtelo de nuevo.");
	}

	virtual FString billboard_loading() override {
		return TEXT("Cargando...");
	}
};


class BillboardLangFR : public IBillboardLang {
public:
	virtual FString billboard_network_error_retry() override {
		return TEXT("Chargement échoué. Veuillez réessayer.");
	}

	virtual FString billboard_loading() override {
		return TEXT("Chargement...");
	}
};


class BillboardLangPT : public IBillboardLang {
public:
	virtual FString billboard_network_error_retry() override {
		return TEXT("Falha ao carregar. Por favor, tente novamente.");
	}

	virtual FString billboard_loading() override {
		return TEXT("Carregando...");
	}
};


class BillboardLangRU : public IBillboardLang {
public:
	virtual FString billboard_network_error_retry() override {
		return TEXT("Загрузка не удалась. Попробуйте снова.");
	}

	virtual FString billboard_loading() override {
		return TEXT("Загрузка...");
	}
};


class BillboardLangTR : public IBillboardLang {
public:
	virtual FString billboard_network_error_retry() override {
		return TEXT("Yüklenemedi. Lütfen tekrar deneyin.");
	}

	virtual FString billboard_loading() override {
		return TEXT("Yükleniyor...");
	}
};


class BillboardLangVI : public IBillboardLang {
public:
	virtual FString billboard_network_error_retry() override {
		return TEXT("Tải thất bại. Vui lòng thử lại.");
	}

	virtual FString billboard_loading() override {
		return TEXT("Đang tải...");
	}
};
