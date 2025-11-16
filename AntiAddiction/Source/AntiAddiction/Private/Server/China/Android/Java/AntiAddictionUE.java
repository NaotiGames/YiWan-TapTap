package com.tds;

import android.app.Activity;
import android.os.Handler;
import android.os.Looper;
import android.text.TextUtils;

import com.tapsdk.antiaddiction.Config;
import com.tapsdk.antiaddiction.entities.response.CheckPayResult;
import com.tapsdk.antiaddiction.entities.response.SubmitPayResult;
import com.tapsdk.antiaddictionui.AntiAddictionUICallback;
import com.tapsdk.antiaddictionui.AntiAddictionUIKit;
import com.tds.common.bridge.utils.BridgeJsonHelper;

import java.util.Map;

public class AntiAddictionUE {

    public static void init(Activity activity, String clientId, boolean showSwitchAccount, boolean useAgeRange) {
        Handler mainHandler = new Handler(Looper.getMainLooper());
        mainHandler.post(new Runnable() {
            @Override
            public void run() {
                Config config = new Config.Builder().withClientId(clientId)
                                .showSwitchAccount(showSwitchAccount).useAgeRange(useAgeRange).build();
                AntiAddictionUIKit.init(activity, config);
                AntiAddictionUIKit.setAntiAddictionCallback(new AntiAddictionUICallback() {
                    @Override
                    public void onCallback(int code, Map<String, Object> extras) {
                        onCallBack(code, BridgeJsonHelper.object2JsonString(extras));
                    }
                });
            }
        });
    }

    public static void startupWithTapTap(Activity activity, String userIdentifier) {
        Handler mainHandler = new Handler(Looper.getMainLooper());
        mainHandler.post(new Runnable() {
            @Override
            public void run() {
                AntiAddictionUIKit.startupWithTapTap(activity, userIdentifier);
            }
        });
    }

    public static void exit() {
        Handler mainHandler = new Handler(Looper.getMainLooper());
        mainHandler.post(new Runnable() {
            @Override
            public void run() {
                AntiAddictionUIKit.exit();
            }
        });
    }

    public static void setTestEnv(Activity activity, boolean enable) {
        Handler mainHandler = new Handler(Looper.getMainLooper());
        mainHandler.post(new Runnable() {
            @Override
            public void run() {
                AntiAddictionUIKit.setTestEnvironment(activity, enable);
            }
        });
    }

    public static int getAgeRange() {
        return AntiAddictionUIKit.getAgeRange();
    }

    public static int getRemainingTime() {
        return AntiAddictionUIKit.getRemainingTime();
    }

    public static String currentToken() {
        return AntiAddictionUIKit.currentToken();
    }

    public static void enterGame() {
        Handler mainHandler = new Handler(Looper.getMainLooper());
        mainHandler.post(new Runnable() {
            @Override
            public void run() {
                AntiAddictionUIKit.enterGame();
            }
        });
    }

    public static void leaveGame() {
        Handler mainHandler = new Handler(Looper.getMainLooper());
        mainHandler.post(new Runnable() {
            @Override
            public void run() {
                AntiAddictionUIKit.leaveGame();
            }
        });
    }

    public static void checkPayLimit(Activity activity, int payAmount) {
        Handler mainHandler = new Handler(Looper.getMainLooper());
        mainHandler.post(new Runnable() {
            @Override
            public void run() {
                AntiAddictionUIKit.checkPayLimit(activity, payAmount, new com.tapsdk.antiaddictionui.Callback<CheckPayResult>() {
                    @Override
                    public void onSuccess(CheckPayResult result) {
                        onCheckPayLimitSuccess(result.status);
                    }

                    @Override
                    public void onError(Throwable throwable) {
                        onCheckPayLimitFail(throwable.getMessage());
                    }
                });
            }
        });
    }

    public static void submitPayResult(int payAmount) {
        Handler mainHandler = new Handler(Looper.getMainLooper());
        mainHandler.post(new Runnable() {
            @Override
            public void run() {
                AntiAddictionUIKit.submitPayResult(payAmount, new com.tapsdk.antiaddiction.Callback<SubmitPayResult>() {
                    @Override
                    public void onSuccess(SubmitPayResult result) {
                        onSubmitPayResultSuccess(true);
                    }

                    @Override
                    public void onError(Throwable throwable) {
                        String errorMsg;
                        if (!TextUtils.isEmpty(throwable.getMessage())) {
                            errorMsg = throwable.getMessage();
                        } else {
                            errorMsg = throwable.toString();
                        }
                        onSubmitPayResultFail(errorMsg);
                    }
                });
            }
        });
    }

    public static native void onCheckPayLimitSuccess(boolean status);
    public static native void onCheckPayLimitFail(String msg);
    public static native void onSubmitPayResultSuccess(boolean status);
    public static native void onSubmitPayResultFail(String msg);
    public static native void onCallBack(int code, String msg);
}
