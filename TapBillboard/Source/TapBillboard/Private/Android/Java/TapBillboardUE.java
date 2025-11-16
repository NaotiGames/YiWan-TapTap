package com.tds;

import android.app.Activity;
import android.os.Handler;
import android.os.Looper;
import android.util.Log;

import com.tapsdk.billboard.Callback;
import com.tapsdk.billboard.CustomLinkListener;
import com.tapsdk.billboard.MarqueeListener;
import com.tapsdk.billboard.OutputStateListener;
import com.tapsdk.billboard.PanelShowStateListener;
import com.tapsdk.billboard.TapBillboard;
import com.tapsdk.billboard.entities.BadgeDetails;
import com.tapsdk.billboard.entities.Marquee;
import com.tapsdk.billboard.entities.MarqueeConfig;
import com.tapsdk.billboard.exceptions.TapBillboardException;
import com.tds.common.entities.Pair;
import com.tds.common.entities.TapBillboardConfig;
import com.tds.common.entities.TapConfig;
import com.tds.common.models.TapRegionType;

import org.json.JSONException;
import org.json.JSONObject;

import java.util.HashSet;
import java.util.Set;

public class TapBillboardUE {
    public static void init(Activity activity, String clientId, String clientToken, String serverUrl, String billboardUrl, String[] dimensionString, int region) {
        Set<Pair<String, String>> dimensionSet = new HashSet<>();
        for (int i = 0; i < dimensionString.length; i += 2)
        {
            dimensionSet.add(Pair.create(dimensionString[i], dimensionString[i+1]));
        }

        TapBillboardConfig billboardConfig = new TapBillboardConfig.Builder()
                .withDimensionSet(dimensionSet)
                .withServerUrl(billboardUrl)
                .build();

        TapConfig.Builder builder = new TapConfig.Builder()
                .withAppContext(activity)
                .withClientId(clientId)
                .withClientToken(clientToken)
                .withServerUrl(serverUrl)
                .withBillboardConfig(billboardConfig)
                .withRegionType(region == 0 ? TapRegionType.CN : TapRegionType.IO);

        TapConfig tdsConfig = builder.build();

        new Handler(Looper.getMainLooper()).post(new Runnable() {
            @Override
            public void run() {
                TapBillboard.init(tdsConfig);
            }
        });
    }

    static CustomLinkListener _CustomLinkListener;

    public static void registerListeners() {
        _CustomLinkListener = new CustomLinkListener() {
            @Override
            public void onCustomUrlClick(String url) {
                NotifyCustomUrl(url);
            }
        };
        TapBillboard.registerCustomLinkListener(_CustomLinkListener);

        TapBillboard.registerOutputStateListener(new OutputStateListener() {
            @Override
            public void onPlayVoice() {
                NotifyAudioStatusChanged(true);
            }
            @Override
            public void onStopVoice() {
                NotifyAudioStatusChanged(false);
            }
        });

        TapBillboard.registerMarqueeListener(new MarqueeListener() {
            @Override
            public void onShow(Marquee marquee, MarqueeConfig config) {
                NotifyMarqueeShow(marquee.toJSON().toString(), config.toJSON().toString());
            }

            @Override
            public void onClose() {
                NotifyMarqueeClose();
            }
        });
    }

    public static void unregisterListeners() {
        TapBillboard.unRegisterOutputStateListener();
        TapBillboard.unRegisterCustomLinkListener(_CustomLinkListener);
        TapBillboard.unRegisterAllMarqueeListeners();
    }

    public static void openPanel(Activity activity, int callBackID) {
        new Handler(Looper.getMainLooper()).post(new Runnable() {
            @Override
            public void run() {
                TapBillboard.openPanel(activity, new Callback<Void>() {
                    @Override
                    public void onError(TapBillboardException tapBillboardException) {
                        NotifyNavigateOpenFailed(callBackID, tapBillboardException.code, tapBillboardException.message);
                    }

                    @Override
                    public void onSuccess(Void result) {
                        NotifyNavigateOpenSuccess(callBackID);
                    }
                },new PanelShowStateListener() {
                    @Override
                    public void onClose() {
                        NotifyNavigateClosed(callBackID);
                    }
                });
            }
        });
    }

    public static void openSplashPanel(Activity activity, int callBackID) {
        new Handler(Looper.getMainLooper()).post(new Runnable() {
            @Override
            public void run() {
                TapBillboard.openSplashPanel(activity, new Callback<Void>() {
                    @Override
                    public void onError(TapBillboardException tapBillboardException) {
                        NotifyNavigateOpenFailed(callBackID, tapBillboardException.code, tapBillboardException.message);
                    }

                    @Override
                    public void onSuccess(Void result) {
                        NotifyNavigateOpenSuccess(callBackID);
                    }
                },new PanelShowStateListener() {
                    @Override
                    public void onClose() {
                        NotifyNavigateClosed(callBackID);
                    }
                });
            }
        });
    }

    public static void getBadgeDetails(int callBackID) {
        TapBillboard.getBadgeDetails(new Callback<BadgeDetails>() {
            @Override
            public void onError(TapBillboardException tapBillboardException) {
                GetBadgeDetailsFailed(callBackID, tapBillboardException.code, tapBillboardException.message);
            }

            @Override
            public void onSuccess(BadgeDetails badgeDetails) {
                GetBadgeDetailsSuccess(callBackID, badgeDetails.showRedDot, badgeDetails.closeButtonImg);
            }
        });
    }

    public static void closePanel() {
        new Handler(Looper.getMainLooper()).post(new Runnable() {
            @Override
            public void run() {
                TapBillboard.closePanel();
            }
        });
    }

    public static void closeSplashPanel() {
        new Handler(Looper.getMainLooper()).post(new Runnable() {
            @Override
            public void run() {
                TapBillboard.closeSplashPanel();
            }
        });
    }

    public static void startFetchMarqueeData(Activity activity) {
        new Handler(Looper.getMainLooper()).post(new Runnable() {
            @Override
            public void run() {
                TapBillboard.startFetchMarqueeData(activity);
            }
        });
    }

    public static void stopFetchMarqueeData(boolean closeNow) {
        new Handler(Looper.getMainLooper()).post(new Runnable() {
            @Override
            public void run() {
                TapBillboard.stopFetchMarqueeData(closeNow);
            }
        });
    }

    public native static void NotifyNavigateOpenFailed(int callBackID, int code, String message);
    public native static void NotifyNavigateOpenSuccess(int callBackID);
    public native static void NotifyNavigateClosed(int callBackID);

    public native static void GetBadgeDetailsFailed(int callBackID, int code, String message);
    public native static void GetBadgeDetailsSuccess(int callBackID, int showRedDot, String closeButtonImg);

    public native static void NotifyCustomUrl(String Url);
    public native static void NotifyAudioStatusChanged(boolean newPlaying);

    public native static void NotifyMarqueeShow(String marquee, String config);

    public native static void NotifyMarqueeClose();
}
