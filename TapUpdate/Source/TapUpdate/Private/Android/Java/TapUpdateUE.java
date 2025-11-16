package com.tds;

import android.app.Activity;
import android.os.Handler;
import android.os.Looper;

import com.taptap.services.update.TapUpdate;
import com.taptap.services.update.TapUpdateCallback;

public class TapUpdateUE {

    public static void init(Activity activity, String clientId, String clientToken) {
        new Handler(Looper.getMainLooper()).post(new Runnable() {
            @Override
            public void run() {
                TapUpdate.init(activity, clientId, clientToken);
            }
        });
    }

    public static void updateGame(Activity activity, int callBackID) {
        new Handler(Looper.getMainLooper()).post(new Runnable() {
            @Override
            public void run() {
                TapUpdate.updateGame(activity, new TapUpdateCallback() {
                    @Override
                    public void onCancel() {
                        // 取消更新的事件
                        onUpdateCancel(callBackID);
                    }
                });
            }
        });
    }

    public native static void onUpdateCancel(int callBackID);
}
