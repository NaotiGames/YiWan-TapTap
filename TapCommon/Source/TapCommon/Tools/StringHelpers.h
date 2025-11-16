// Copyright Epic Games, Inc. All Rights Reserved.

#pragma once


inline void LexFromStringHex(int32& OutValue, const TCHAR* Buffer) { OutValue = FCString::Strtoi(Buffer, nullptr, 16); }

inline FColor HexToColor(const FString& HexString, bool bContainA = false)
{
	int HexLength = bContainA ? 8 : 6;
	if (HexString.Len() < HexLength)
	{
		// 如果输入的十六进制字符串长度不足，返回默认的白色
		return FColor::White;
	}
	FString ResultHexString = HexString;
	if (ResultHexString.Len() > 8)
	{
		ResultHexString.RightInline(8);
	}

	int32 R, G, B, A = 0;
	R = FParse::HexNumber(*ResultHexString.Mid(0, 2));
	G = FParse::HexNumber(*ResultHexString.Mid(2, 2));
	B = FParse::HexNumber(*ResultHexString.Mid(4, 2));
	if (bContainA) {
		A = FParse::HexNumber(*ResultHexString.Mid(6, 2));
	}

	if (bContainA) {
		return FColor(R, G, B, A);
	} else {
		return FColor(R, G, B);
	}
}

// 将FLinearColor转换为十六进制颜色字符串
inline FString ColorToHex(const FColor& Color, bool bContainA = false)
{
	if (bContainA) {
		return FString::Printf(TEXT("%02X%02X%02X%02X"), Color.R, Color.G, Color.B, Color.A);
	} else {
		return FString::Printf(TEXT("%02X%02X%02X"), Color.R, Color.G, Color.B);
	}
}

namespace TUCommon
{

	namespace StringHelpers
	{

		int32 FindFirstOf(const FString& InString, const FString& SplitAt, int32 FirstPos = 0);

		int32 FindFirstNotOf(const FString& InString, const FString& InNotOfChars, int32 FirstPos = 0);

		int32 FindLastNotOf(const FString& InString, const FString& InNotOfChars, int32 StartPos = MAX_int32);

		void SplitByDelimiter(TArray<FString>& OutSplits, const FString& InString, const FString& SplitAt);

		bool StringEquals(const TCHAR * const s1, const TCHAR * const s2);

		bool StringStartsWith(const TCHAR * const s1, const TCHAR * const s2, SIZE_T n);

		void StringToArray(TArray<uint8>& OutArray, const FString& InString);

		FString ArrayToString(const TArray<uint8>& InArray);

		/**
		 * There is a known anomaly in the FString::TConstIterator. It iterates all TCHARs in the string *including* the terminating zero character.
		 * This is not the behaviour we want and setup some helper iterator here which is not including the terminating zero.
		 */
		class FStringIterator
		{
		public:
			FStringIterator(const FString& InString, int32 StartIndex = 0)
				: StringToIterate(InString)
				, Index(StartIndex)
			{
			}

			/** Advances iterator to the next element in the container. */
			FStringIterator& operator++()
			{
				++Index;
				return *this;
			}
			FStringIterator operator++(int)
			{
				FStringIterator Tmp(*this);
				++Index;
				return Tmp;
			}

			/** Moves iterator to the previous element in the container. */
			FStringIterator& operator--()
			{
				--Index;
				return *this;
			}
			FStringIterator operator--(int)
			{
				FStringIterator Tmp(*this);
				--Index;
				return Tmp;
			}

			/** iterator arithmetic support */
			FStringIterator& operator+=(int32 Offset)
			{
				Index += Offset;
				return *this;
			}

			FStringIterator operator+(int32 Offset) const
			{
				FStringIterator Tmp(*this);
				return Tmp += Offset;
			}

			FStringIterator& operator-=(int32 Offset)
			{
				return *this += -Offset;
			}

			FStringIterator operator-(int32 Offset) const
			{
				FStringIterator Tmp(*this);
				return Tmp -= Offset;
			}

			const TCHAR& operator* () const
			{
				return StringToIterate[Index];
			}

			const TCHAR* operator->() const
			{
				return &StringToIterate[Index];
			}

			/** conversion to "bool" returning true if the iterator has not reached the last element. */
			FORCEINLINE explicit operator bool() const
			{
				return Index >= 0 && Index < StringToIterate.Len();
			}

			const TCHAR* GetRemainder() const
			{
				return &StringToIterate[Index];
			}

			/** Returns an index to the current element. */
			int32 GetIndex() const
			{
				return Index;
			}

			/** Resets the iterator to the first element. */
			void Reset()
			{
				Index = 0;
			}

			/** Sets iterator to the last element. */
			void SetToEnd()
			{
				Index = StringToIterate.Len();
			}

			FORCEINLINE friend bool operator==(const FStringIterator& Lhs, const FStringIterator& Rhs) { return &Lhs.StringToIterate == &Rhs.StringToIterate && Lhs.Index == Rhs.Index; }
			FORCEINLINE friend bool operator!=(const FStringIterator& Lhs, const FStringIterator& Rhs) { return &Lhs.StringToIterate != &Rhs.StringToIterate || Lhs.Index != Rhs.Index; }

		private:
			const FString&	StringToIterate;
			int32			Index;
		};

	} // namespace StringHelpers

} // namespace Electra
