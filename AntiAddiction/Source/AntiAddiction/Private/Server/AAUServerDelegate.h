#pragma once


// 时间边界
enum AAUTimeBoundary {
	AAUTimeBoundaryNoLimit,	// 没有限制
	AAUTimeBoundaryCurfew,		// 非可玩时间段，目前政策都是因为该问题
	AAUInvalidClientOrNetworkError, // 应用参数配置错误或网络异常
	AAUTokenExpired // token 失效
};

class AAUServerDelegate {
public:
	
	
	virtual ~AAUServerDelegate() = default;
	// 用户被踢出，如果Boundary返回的是NoLimit，那么是被服务主动踢出
	virtual void KickOut(AAUTimeBoundary Boundary, const FString& Title, const FString& Content) = 0;
};