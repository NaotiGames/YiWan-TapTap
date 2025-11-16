#pragma once
#include "TUType.h"

class ILoginLang {
public:
	virtual ~ILoginLang() = default;

	virtual FString TitleUse() = 0;

	virtual FString TitleLogin() = 0;

	virtual FString QrTitleLogin() = 0;

	virtual FString QrRefresh() = 0;

	virtual FString QrNoticeScanToLogin() = 0;

	virtual FString QrNoticeScanToLogin2() = 0;

	virtual FString WebLogin() = 0;

	virtual FString WebNotice() = 0;

	virtual FString WebNotice2() = 0;
	
	virtual FString WebButtonJumpToWeb() = 0;

	virtual FString QrNoticeCancel() = 0;

	virtual FString QrNoticeCancel2() = 0;

	virtual FString QrnNoticeSuccess() = 0;

	virtual FString QrnNoticeSuccess2() = 0;

	virtual FString WebNoticeLogin() = 0;

	virtual FString WebNoticeFail() = 0;

	virtual FString WebNoticeFail2() = 0;

	virtual FString login_account_logged_tip() = 0;
};

class TULoginLanguage {
public:
	
	static TSharedPtr<ILoginLang> GetCurrentLang();

	// static void SetLangType(Type Type);

private:

	static TSharedPtr<ILoginLang> CurrentLang;
	static ELanguageType LanguageType;
};

class LoginLangCN : public ILoginLang {
public:
	virtual FString TitleUse() override {
		return TEXT("使用");
	}

	virtual FString TitleLogin() override {
		return TEXT("账号登录");
	}

	virtual FString QrTitleLogin() override {
		return TEXT("安全扫码登录");
	}

	virtual FString QrRefresh() override {
		return TEXT("点击刷新");
	}

	virtual FString QrNoticeScanToLogin() override {
		return TEXT("扫描二维码登录");
	}

	virtual FString QrNoticeScanToLogin2() override {
		return TEXT("请使用 <TapBoldStyle>TapTap</> 客户端");
	}
	
	virtual FString WebLogin() override {
		return TEXT("使用网页浏览器完成授权");
	}

	virtual FString WebNotice() override {
		return TEXT("点击按钮前往网页浏览器");
	}

	virtual FString WebNotice2() override {
		return TEXT("授权 TapTap 账号，完成后将自动返回游戏");
	}

	virtual FString WebButtonJumpToWeb() override {
		return TEXT("跳转至 TapTap");
	}

	virtual FString QrNoticeCancel() override {
		return TEXT("您已取消此次登录");
	}

	virtual FString QrNoticeCancel2() override {
		return TEXT("请重新扫码");
	}

	virtual FString QrnNoticeSuccess() override {
		return TEXT("扫码成功");
	}

	virtual FString QrnNoticeSuccess2() override {
		return TEXT("请在手机上确认");
	}

	virtual FString WebNoticeLogin() override {
		return TEXT("正在登录中，请稍后");
	}

	virtual FString WebNoticeFail() override {
		return TEXT("登录失败");
	}

	virtual FString WebNoticeFail2() override {
		return TEXT("请重新点击打开网页进行授权");
	}

	virtual FString login_account_logged_tip() override {
		return TEXT("已登录账号：{0}");
	}
};

class LoginLangIO : public ILoginLang {
public:
	virtual FString TitleUse() override {
		return TEXT("Log In with");
	}

	virtual FString TitleLogin() override {
		return TEXT("");
	}

	virtual FString QrTitleLogin() override {
		return TEXT("Via QR Code");
	}

	virtual FString QrRefresh() override {
		return TEXT("Refresh");
	}
	
	virtual FString QrNoticeScanToLogin() override {
		return TEXT("Scan the QR code");
	}
		
	virtual FString QrNoticeScanToLogin2() override {
		return TEXT("with <TapBoldStyle>TapTap</> App");
	}
	
	virtual FString WebLogin() override {
		return TEXT("Via Web Browser");
	}

	virtual FString WebNotice() override {
		return TEXT("Allow permission to log in with TapTap");
	}

	virtual FString WebNotice2() override {
		return TEXT("You'll be redirected back after login");
	}

	virtual FString WebButtonJumpToWeb() override {
		return TEXT("Go to TapTap");
	}

	virtual FString QrNoticeCancel() override {
		return TEXT("Failed to log in");
	}

	virtual FString QrNoticeCancel2() override {
		return TEXT("Please try again");
	}

	virtual FString QrnNoticeSuccess() override {
		return TEXT("Success");
	}

	virtual FString QrnNoticeSuccess2() override {
		return TEXT("Please confirm login on your phone");
	}

	virtual FString WebNoticeLogin() override {
		return TEXT("Logging in");
	}

	virtual FString WebNoticeFail() override {
		return TEXT("Failed to log in");
	}

	virtual FString WebNoticeFail2() override {
		return TEXT("Please try again");
	}

	virtual FString login_account_logged_tip() override {
		return TEXT("Logged-in account: {0}");
	}
};

class LoginLangID : public ILoginLang {
public:
	virtual FString TitleUse() override {
		return TEXT("Log In dengan ");
	}

	virtual FString TitleLogin() override {
		return TEXT("");
	}

	virtual FString QrTitleLogin() override {
		return TEXT("Melalui Kode QR");
	}

	virtual FString QrRefresh() override {
		return TEXT("Refresh");
	}

	virtual FString QrNoticeScanToLogin() override {
		return TEXT("Pindai kode QR");
	}

	virtual FString QrNoticeScanToLogin2() override {
		return TEXT("dengan apliaksi <TapBoldStyle>TapTap</>");
	}
	
	virtual FString WebLogin() override {
		return TEXT("Melalui Browser Web");
	}

	virtual FString WebNotice() override {
		return TEXT("Berikan izin untuk log in dengan TapTap");
	}

	virtual FString WebNotice2() override {
		return TEXT("Kamu akan diarahkan kembali setelah log in");
	}

	virtual FString WebButtonJumpToWeb() override {
		return TEXT("Buka TapTap");
	}

	virtual FString QrNoticeCancel() override {
		return TEXT("Gagal log in");
	}

	virtual FString QrNoticeCancel2() override {
		return TEXT("Silakan coba lagi.");
	}

	virtual FString QrnNoticeSuccess() override {
		return TEXT("Berhasil");
	}

	virtual FString QrnNoticeSuccess2() override {
		return TEXT("Silakan konfirmasi log in di ponselmu.");
	}

	virtual FString WebNoticeLogin() override {
		return TEXT("Sedang log in...");
	}

	virtual FString WebNoticeFail() override {
		return TEXT("Gagal log in");
	}

	virtual FString WebNoticeFail2() override {
		return TEXT("Silakan coba lagi.");
	}

	virtual FString login_account_logged_tip() override {
		return TEXT("Akun yang login: {0}");
	}
};

class LoginLangJA : public ILoginLang {
public:
	virtual FString TitleUse() override {
		return TEXT("");
	}

	virtual FString TitleLogin() override {
		return TEXT("でログイン");
	}

	virtual FString QrTitleLogin() override {
		return TEXT("QRコードを使う");
	}

	virtual FString QrRefresh() override {
		return TEXT("更新");
	}

	virtual FString QrNoticeScanToLogin() override {
		return TEXT("QRコードをスキャン");
	}

	virtual FString QrNoticeScanToLogin2() override {
		return TEXT("<TapBoldStyle>TapTap</>アプリで");
	}
	
	virtual FString WebLogin() override {
		return TEXT("ウェブブラウザを使う");
	}

	virtual FString WebNotice() override {
		return TEXT("TapTapへのログインを許可します");
	}

	virtual FString WebNotice2() override {
		return TEXT("ログイン後にリダイレクトされます");
	}

	virtual FString WebButtonJumpToWeb() override {
		return TEXT("TapTapを開く");
	}

	virtual FString QrNoticeCancel() override {
		return TEXT("ログインに失敗しました");
	}

	virtual FString QrNoticeCancel2() override {
		return TEXT("もう一度お試しください。");
	}

	virtual FString QrnNoticeSuccess() override {
		return TEXT("成功");
	}

	virtual FString QrnNoticeSuccess2() override {
		return TEXT("お使いの携帯電話でログインを確認してください。");
	}

	virtual FString WebNoticeLogin() override {
		return TEXT("ログイン中…");
	}

	virtual FString WebNoticeFail() override {
		return TEXT("ログインに失敗しました");
	}

	virtual FString WebNoticeFail2() override {
		return TEXT("もう一度お試しください。");
	}

	virtual FString login_account_logged_tip() override {
		return TEXT("ログイン中のアカウント: {0}");
	}
};

class LoginLangKO : public ILoginLang {
public:
	virtual FString TitleUse() override {
		return TEXT("");
	}

	virtual FString TitleLogin() override {
		return TEXT("으로 로그인");
	}

	virtual FString QrTitleLogin() override {
		return TEXT("QR 코드 사용");
	}

	virtual FString QrRefresh() override {
		return TEXT("새로 고치기");
	}

	virtual FString QrNoticeScanToLogin() override {
		return TEXT("QR 코드를 스캔하세요");
	}

	virtual FString QrNoticeScanToLogin2() override {
		return TEXT("<TapBoldStyle>Taptap</> 앱으로");
	}
	
	virtual FString WebLogin() override {
		return TEXT("웹 브라우저 사용");
	}

	virtual FString WebNotice() override {
		return TEXT("Taptap으로 로그인하는 권한을 허락하세요");
	}

	virtual FString WebNotice2() override {
		return TEXT("로그인 후에 원래 화면으로 돌아갑니다");
	}
	
	virtual FString WebButtonJumpToWeb() override {
		return TEXT("Taptap으로 이동");
	}

	virtual FString QrNoticeCancel() override {
		return TEXT("로그인 실패");
	}

	virtual FString QrNoticeCancel2() override {
		return TEXT("다시 시도하세요.");
	}

	virtual FString QrnNoticeSuccess() override {
		return TEXT("성공");
	}

	virtual FString QrnNoticeSuccess2() override {
		return TEXT("스마트폰에서 로그인을 확인하세요.");
	}

	virtual FString WebNoticeLogin() override {
		return TEXT("로그인 중...");
	}

	virtual FString WebNoticeFail() override {
		return TEXT("로그인 실패");
	}

	virtual FString WebNoticeFail2() override {
		return TEXT("다시 시도하세요.");
	}

	virtual FString login_account_logged_tip() override {
		return TEXT("로그인한 계정: {0}");
	}
};

class LoginLangTH : public ILoginLang {
public:
	virtual FString TitleUse() override {
		return TEXT("ล็อกอินด้วย ");
	}

	virtual FString TitleLogin() override {
		return TEXT("");
	}

	virtual FString QrTitleLogin() override {
		return TEXT("ผ่าน QR โค้ด");
	}

	virtual FString QrRefresh() override {
		return TEXT("รีเฟรช");
	}

	virtual FString QrNoticeScanToLogin() override {
		return TEXT("สแกน QR โค้ด");
	}

	virtual FString QrNoticeScanToLogin2() override {
		return TEXT("ด้วยแอป <TapBoldStyle>TapTap</>");
	}

	virtual FString WebLogin() override {
		return TEXT("ผ่านเว็บเบราว์เซอร์");
	}

	virtual FString WebNotice() override {
		return TEXT("อนุญาตให้ล็อกอินด้วย TapTap ");
	}

	virtual FString WebNotice2() override {
		return TEXT("คุณจะถูกเปลี่ยนเส้นทางกลับหลังจากล็อกอิน");
	}

	virtual FString WebButtonJumpToWeb() override {
		return TEXT("ไปที่ TapTap");
	}

	virtual FString QrNoticeCancel() override {
		return TEXT("ไม่สามารถล็อกอินได้");
	}

	virtual FString QrNoticeCancel2() override {
		return TEXT("โปรดลองอีกครั้ง");
	}

	virtual FString QrnNoticeSuccess() override {
		return TEXT("สำเร็จ");
	}

	virtual FString QrnNoticeSuccess2() override {
		return TEXT("โปรดยืนยันการล็อกอินบนโทรศัพท์ของคุณ");
	}

	virtual FString WebNoticeLogin() override {
		return TEXT("กำลังล็อกอิน...");
	}

	virtual FString WebNoticeFail() override {
		return TEXT("ไม่สามารถล็อกอินได้");
	}

	virtual FString WebNoticeFail2() override {
		return TEXT("โปรดลองอีกครั้ง");
	}

	virtual FString login_account_logged_tip() override {
		return TEXT("บัญชีที่ล็อกอิน: {0}");
	}
};

class LoginLangZHTW : public ILoginLang {
public:
	virtual FString TitleUse() override {
		return TEXT("使用");
	}

	virtual FString TitleLogin() override {
		return TEXT("登入");
	}

	virtual FString QrTitleLogin() override {
		return TEXT("透過 QR 碼");
	}

	virtual FString QrRefresh() override {
		return TEXT("重新整理");
	}

	virtual FString QrNoticeScanToLogin() override {
		return TEXT("掃描 QR 碼");
	}
	
	virtual FString QrNoticeScanToLogin2() override {
		return TEXT("使用 <TapBoldStyle>TapTap</> 應用程式");
	}
	
	virtual FString WebLogin() override {
		return TEXT("透過網路瀏覽器");
	}

	virtual FString WebNotice() override {
		return TEXT("請允許使用 TapTap 登入");
	}

	virtual FString WebNotice2() override {
		return TEXT("登入後系統會將你重新導回此頁面");
	}

	virtual FString WebButtonJumpToWeb() override {
		return TEXT("前往 TapTap");
	}

	virtual FString QrNoticeCancel() override {
		return TEXT("登入失敗");
	}

	virtual FString QrNoticeCancel2() override {
		return TEXT("請再試一次。");
	}

	virtual FString QrnNoticeSuccess() override {
		return TEXT("成功");
	}

	virtual FString QrnNoticeSuccess2() override {
		return TEXT("請在您的手機上確認登入。");
	}

	virtual FString WebNoticeLogin() override {
		return TEXT("正在登入……");
	}

	virtual FString WebNoticeFail() override {
		return TEXT("登入失敗");
	}

	virtual FString WebNoticeFail2() override {
		return TEXT("請再試一次。");
	}

	virtual FString login_account_logged_tip() override {
		return TEXT("已登入帳號：{0}");
	}
};


class LoginLangDE : public ILoginLang {
public:
	virtual FString TitleUse() override {
		return TEXT("Anmeldung mit");
	}

	virtual FString TitleLogin() override {
		return TEXT("");
	}

	virtual FString QrTitleLogin() override {
		return TEXT("Per QR-Code");
	}

	virtual FString QrRefresh() override {
		return TEXT("Aktualisierung");
	}

	virtual FString QrNoticeScanToLogin() override {
		return TEXT("Scanne den QR-Code");
	}

	virtual FString QrNoticeScanToLogin2() override {
		return TEXT("nmit der <TapBoldStyle>TapTap</>-App");
	}
	
	virtual FString WebLogin() override {
		return TEXT("Per Webbrowser");
	}

	virtual FString WebNotice() override {
		return TEXT("Erlaube die Anmeldung mit TapTap.");
	}

	virtual FString WebNotice2() override {
		return TEXT("Du wirst nach der Anmeldung zurückgeleitet.");
	}

	virtual FString WebButtonJumpToWeb() override {
		return TEXT("Gehe zu TapTap");
	}

	virtual FString QrNoticeCancel() override {
		return TEXT("Anmeldung fehlgeschlagen");
	}

	virtual FString QrNoticeCancel2() override {
		return TEXT("Versuche es bitte erneut.");
	}

	virtual FString QrnNoticeSuccess() override {
		return TEXT("Erfolgreich");
	}

	virtual FString QrnNoticeSuccess2() override {
		return TEXT("Bitte bestätige die Anmeldung mit deinem Handy.");
	}

	virtual FString WebNoticeLogin() override {
		return TEXT("Ameldung...");
	}

	virtual FString WebNoticeFail() override {
		return TEXT("Anmeldung fehlgeschlagen");
	}

	virtual FString WebNoticeFail2() override {
		return TEXT("Versuche es bitte erneut.");
	}

	virtual FString login_account_logged_tip() override {
		return TEXT("Eingeloggt Konto: {0}");
	}
};


class LoginLangES : public ILoginLang {
public:
	virtual FString TitleUse() override {
		return TEXT("Iniciar sesión con");
	}

	virtual FString TitleLogin() override {
		return TEXT("");
	}

	virtual FString QrTitleLogin() override {
		return TEXT("Mediante código QR");
	}

	virtual FString QrRefresh() override {
		return TEXT("Actualizar");
	}

	virtual FString QrNoticeScanToLogin() override {
		return TEXT("Escanee el código QR");
	}

	virtual FString QrNoticeScanToLogin2() override {
		return TEXT("ncon la aplicación <TapBoldStyle>TapTap</>");
	}
	
	virtual FString WebLogin() override {
		return TEXT("A través del navegador web");
	}

	virtual FString WebNotice() override {
		return TEXT("Será redirigido de vuelta después de iniciar sesión.");
	}

	virtual FString WebNotice2() override {
		return TEXT("Permita iniciar sesión con TapTap.");
	}

	virtual FString WebButtonJumpToWeb() override {
		return TEXT("Ir a TapTap");
	}

	virtual FString QrNoticeCancel() override {
		return TEXT("Error al iniciar sesión");
	}

	virtual FString QrNoticeCancel2() override {
		return TEXT("Por favor, inténtelo de nuevo.");
	}

	virtual FString QrnNoticeSuccess() override {
		return TEXT("Éxito");
	}

	virtual FString QrnNoticeSuccess2() override {
		return TEXT("Por favor, confirme el inicio de sesión en su teléfono.");
	}

	virtual FString WebNoticeLogin() override {
		return TEXT("Iniciando sesión...");
	}

	virtual FString WebNoticeFail() override {
		return TEXT("Error al iniciar sesión");
	}

	virtual FString WebNoticeFail2() override {
		return TEXT("Por favor, inténtalo de nuevo.");
	}

	virtual FString login_account_logged_tip() override {
		return TEXT("Cuenta iniciada: {0}");
	}
};


class LoginLangFR : public ILoginLang {
public:
	virtual FString TitleUse() override {
		return TEXT("Se connecter avec ");
	}

	virtual FString TitleLogin() override {
		return TEXT("");
	}

	virtual FString QrTitleLogin() override {
		return TEXT("Via le Code QR");
	}

	virtual FString QrRefresh() override {
		return TEXT("Actualisé");
	}

	virtual FString QrNoticeScanToLogin() override {
		return TEXT("Scanner le Code QR");
	}

	virtual FString QrNoticeScanToLogin2() override {
		return TEXT("avec l'application <TapBoldStyle>TapTap</>");
	}
	
	virtual FString WebLogin() override {
		return TEXT("Via le Navigateur Web");
	}

	virtual FString WebNotice() override {
		return TEXT("Vous serez redirigé après la connexion");
	}

	virtual FString WebNotice2() override {
		return TEXT("Autoriser la connexion avec TapTap");
	}

	virtual FString WebButtonJumpToWeb() override {
		return TEXT("Aller à TapTap");
	}

	virtual FString QrNoticeCancel() override {
		return TEXT("Échec de connexion");
	}

	virtual FString QrNoticeCancel2() override {
		return TEXT("Veuillez réessayer.");
	}

	virtual FString QrnNoticeSuccess() override {
		return TEXT("Réussi");
	}

	virtual FString QrnNoticeSuccess2() override {
		return TEXT("Veuillez confirmer votre connexion sur votre téléphone.");
	}

	virtual FString WebNoticeLogin() override {
		return TEXT("Connexion...");
	}

	virtual FString WebNoticeFail() override {
		return TEXT("Échec de connexion");
	}

	virtual FString WebNoticeFail2() override {
		return TEXT("Veuillez réessayer.");
	}

	virtual FString login_account_logged_tip() override {
		return TEXT("Compte connecté : {0}");
	}
};


class LoginLangPT : public ILoginLang {
public:
	virtual FString TitleUse() override {
		return TEXT("Entrar com ");
	}

	virtual FString TitleLogin() override {
		return TEXT("");
	}

	virtual FString QrTitleLogin() override {
		return TEXT("Via QR Cod");
	}

	virtual FString QrRefresh() override {
		return TEXT("Atualizar");
	}

	virtual FString QrNoticeScanToLogin() override {
		return TEXT("Escaneie o QR code");
	}

	virtual FString QrNoticeScanToLogin2() override {
		return TEXT("com o aplicativo <TapBoldStyle>TapTap</>");
	}
	
	virtual FString WebLogin() override {
		return TEXT("Através do Navegador da Web");
	}

	virtual FString WebNotice() override {
		return TEXT("Você será redirecionado de volta após o login.");
	}

	virtual FString WebNotice2() override {
		return TEXT("Conceder permissão para fazer login com o TapTap");
	}

	virtual FString WebButtonJumpToWeb() override {
		return TEXT("Ir para TapTap");
	}

	virtual FString QrNoticeCancel() override {
		return TEXT("Falha ao fazer login");
	}

	virtual FString QrNoticeCancel2() override {
		return TEXT("Por favor, tente novamente.");
	}

	virtual FString QrnNoticeSuccess() override {
		return TEXT("Sucesso");
	}

	virtual FString QrnNoticeSuccess2() override {
		return TEXT("Por favor, confirme o login no seu telefone.");
	}

	virtual FString WebNoticeLogin() override {
		return TEXT("Fazendo login…");
	}

	virtual FString WebNoticeFail() override {
		return TEXT("Falha ao fazer login");
	}

	virtual FString WebNoticeFail2() override {
		return TEXT("Por favor, tente novamente.");
	}

	virtual FString login_account_logged_tip() override {
		return TEXT("Conta conectada: {0}");
	}
};


class LoginLangRU : public ILoginLang {
public:
	virtual FString TitleUse() override {
		return TEXT("Войти с ");
	}

	virtual FString TitleLogin() override {
		return TEXT("");
	}

	virtual FString QrTitleLogin() override {
		return TEXT("Через QR-код");
	}

	virtual FString QrRefresh() override {
		return TEXT("Обновить");
	}

	virtual FString QrNoticeScanToLogin() override {
		return TEXT("Просканируйте QR-код");
	}

	virtual FString QrNoticeScanToLogin2() override {
		return TEXT("nприложением <TapBoldStyle>TapTap</>");
	}
	
	virtual FString WebLogin() override {
		return TEXT("Через браузер");
	}

	virtual FString WebNotice() override {
		return TEXT("Вы будете перенаправлены после входа.");
	}

	virtual FString WebNotice2() override {
		return TEXT("Разрешите входить с TapTap");
	}

	virtual FString WebButtonJumpToWeb() override {
		return TEXT("Перейти в TapTap");
	}

	virtual FString QrNoticeCancel() override {
		return TEXT("Не удалось войти");
	}

	virtual FString QrNoticeCancel2() override {
		return TEXT("Попробуйте снова.");
	}

	virtual FString QrnNoticeSuccess() override {
		return TEXT("Успешно");
	}

	virtual FString QrnNoticeSuccess2() override {
		return TEXT("Подтвердите вход на своем телефоне.");
	}

	virtual FString WebNoticeLogin() override {
		return TEXT("Входите...");
	}

	virtual FString WebNoticeFail() override {
		return TEXT("Не удалось войти");
	}

	virtual FString WebNoticeFail2() override {
		return TEXT("Попробуйте снова.");
	}

	virtual FString login_account_logged_tip() override {
		return TEXT("Залогиненный аккаунт: {0}");
	}
};


class LoginLangTR : public ILoginLang {
public:
	virtual FString TitleUse() override {
		return TEXT("");
	}

	virtual FString TitleLogin() override {
		return TEXT(" ile Giriş Yap");
	}

	virtual FString QrTitleLogin() override {
		return TEXT("QR Kodu");
	}

	virtual FString QrRefresh() override {
		return TEXT("Yenile");
	}

	virtual FString QrNoticeScanToLogin() override {
		return TEXT("QR kodunu tara");
	}

	virtual FString QrNoticeScanToLogin2() override {
		return TEXT("<TapBoldStyle>TapTap</> uygulamasını kullanarak");
	}
	
	virtual FString WebLogin() override {
		return TEXT("İnternet Tarayıcısı");
	}

	virtual FString WebNotice() override {
		return TEXT("Girişten sonra yeniden yönlendirileceksiniz.");
	}

	virtual FString WebNotice2() override {
		return TEXT("TapTap ile giriş yapmak için izin ver.");
	}

	virtual FString WebButtonJumpToWeb() override {
		return TEXT("TapTap");
	}

	virtual FString QrNoticeCancel() override {
		return TEXT("Giriş yapılamadı");
	}

	virtual FString QrNoticeCancel2() override {
		return TEXT("Lütfen tekrar deneyin.");
	}

	virtual FString QrnNoticeSuccess() override {
		return TEXT("Başarılı");
	}

	virtual FString QrnNoticeSuccess2() override {
		return TEXT("Lütfen girişi telefonunuzdan onaylayın.");
	}

	virtual FString WebNoticeLogin() override {
		return TEXT("Giriş yapılıyor…");
	}

	virtual FString WebNoticeFail() override {
		return TEXT("Giriş yapılamadı");
	}

	virtual FString WebNoticeFail2() override {
		return TEXT("Lütfen tekrar deneyin.");
	}

	virtual FString login_account_logged_tip() override {
		return TEXT("Giriş yapılan hesap: {0}");
	}
};


class LoginLangVI : public ILoginLang {
public:
	virtual FString TitleUse() override {
		return TEXT("Đăng nhập bằng ");
	}

	virtual FString TitleLogin() override {
		return TEXT("");
	}

	virtual FString QrTitleLogin() override {
		return TEXT("Dùng mã QR");
	}

	virtual FString QrRefresh() override {
		return TEXT("Làm mới");
	}

	virtual FString QrNoticeScanToLogin() override {
		return TEXT("Quét mã QR");
	}

	virtual FString QrNoticeScanToLogin2() override {
		return TEXT("bằng app <TapBoldStyle>TapTap</>");
	}
	
	virtual FString WebLogin() override {
		return TEXT("Dùng Trình duyệt Web");
	}

	virtual FString WebNotice() override {
		return TEXT("Bạn sẽ tự động quay lại sau khi đăng nhập.");
	}

	virtual FString WebNotice2() override {
		return TEXT("Cho phép đăng nhập bằng TapTap");
	}

	virtual FString WebButtonJumpToWeb() override {
		return TEXT("Đến TapTap");
	}

	virtual FString QrNoticeCancel() override {
		return TEXT("Đăng nhập thất bại");
	}

	virtual FString QrNoticeCancel2() override {
		return TEXT("Vui lòng thử lại.");
	}

	virtual FString QrnNoticeSuccess() override {
		return TEXT("Thành công");
	}

	virtual FString QrnNoticeSuccess2() override {
		return TEXT("Vui lòng xác nhận đăng nhập trên điện thoại của bạn.");
	}

	virtual FString WebNoticeLogin() override {
		return TEXT("Đang đăng nhập…");
	}

	virtual FString WebNoticeFail() override {
		return TEXT("Đăng nhập thất bại");
	}

	virtual FString WebNoticeFail2() override {
		return TEXT("Vui lòng thử lại.");
	}

	virtual FString login_account_logged_tip() override {
		return TEXT("Đã đăng nhập tài khoản: {0}");
	}
};