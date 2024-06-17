#pragma once

//----------------------------------------------------------------------//
//----------------------------------------------------------------------//
#if UE_BUILD_DEBUG || UE_BUILD_DEVELOPMENT
	// デバッグビルドでの定義
	#define SNPLUGIN_DEBUG (1)
#else  // UE_BUILD_DEBUG || UE_BUILD_DEVELOPMENT
	// リリースビルドでの定義
	#define SNPLUGIN_DEBUG (0)
#endif // UE_BUILD_DEBUG || UE_BUILD_DEVELOPMENT

#if !NO_LOGGING
	#if !SNPLUGIN_DEBUG
		#define SNPLUGIN_ENABLE_DEBUGOUTPUT	(0)
	#else	// SNPLUGIN_DEBUG
		#define SNPLUGIN_ENABLE_DEBUGOUTPUT	(1)
	#endif // SNPLUGIN_DEBUG
#else // NO_LOGGING
	#define SNPLUGIN_ENABLE_DEBUGOUTPUT	(0)
#endif // NO_LOGGING

#if SNPLUGIN_ENABLE_DEBUGOUTPUT
	#define SNPLUGIN_ASSERT(__EXPRESSION__, ...)					checkf(__EXPRESSION__, ##__VA_ARGS__)
	#define SNPLUGIN_ERROR(__FORMAT__, ...)						UE_LOG(LogTemp, Error, __FORMAT__, ##__VA_ARGS__)
	#define SNPLUGIN_WARNING(__FORMAT__, ...)						UE_LOG(LogTemp, Warning, __FORMAT__, ##__VA_ARGS__)
	#define SNPLUGIN_LOG(__FORMAT__, ...)							UE_LOG(LogTemp, Log, __FORMAT__, ##__VA_ARGS__)
#else // SNPLUGIN_ENABLE_DEBUGOUTPUT
	#define SNPLUGIN_ASSERT(__EXPRESSION__, ...)
	#define SNPLUGIN_ERROR(__FORMAT__, ...)
	#define SNPLUGIN_WARNING(__FORMAT__, ...)
	#define SNPLUGIN_LOG(__FORMAT__, ...)
#endif// SNPLUGIN_ENABLE_DEBUGOUTPUT

//! @{@name プライマリーワールドへのポインタを取得
UWorld* GetPrimaryWorld();
//! @}
