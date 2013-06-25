/*
* Simd Library Tests.
*
* Copyright (c) 2011-2013 Yermalayeu Ihar.
*
* Permission is hereby granted, free of charge, to any person obtaining a copy
* of this software and associated documentation files (the "Software"), to deal
* in the Software without restriction, including without limitation the rights
* to use, copy, modify, merge, publish, distribute, sublicense, and/or sell
* copies of the Software, and to permit persons to whom the Software is
* furnished to do so, subject to the following conditions:
*
* The above copyright notice and this permission notice shall be included in
* all copies or substantial portions of the Software.
*
* THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS OR
* IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY,
* FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT. IN NO EVENT SHALL THE
* AUTHORS OR COPYRIGHT HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER
* LIABILITY, WHETHER IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING FROM,
* OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER DEALINGS IN THE
* SOFTWARE.
*/
#include "Test/TestUtils.h"
#include "Test/TestPerformance.h"

namespace Test
{
	namespace
	{
		struct Func1
		{
			typedef void (*FuncPtr)(const uchar * value, size_t valueStride, size_t width, size_t height,
				uchar * lo, size_t loStride, uchar * hi, size_t hiStride);

			FuncPtr func;
			std::string description;

			Func1(const FuncPtr & f, const std::string & d) : func(f), description(d) {}

			void Call(const View & value, const View & loSrc, const View & hiSrc, View & loDst, View & hiDst) const
			{
				Simd::Copy(loSrc, loDst);
				Simd::Copy(hiSrc, hiDst);
				TEST_PERFORMANCE_TEST(description);
				func(value.data, value.stride, value.width, value.height, loDst.data, loDst.stride, hiDst.data, hiDst.stride);
			}
		};
	}

#define FUNC1(function) Func1(function, std::string(#function))

	bool BackgroundGrowRangeSlowTest(int width, int height, const Func1 & f1, const Func1 & f2)
	{
		bool result = true;

		std::cout << "Test " << f1.description << " & " << f2.description << " [" << width << ", " << height << "]." << std::endl;

		View value(width, height, View::Gray8, NULL, TEST_ALIGN(width));
		FillRandom(value);
		View loSrc(width, height, View::Gray8, NULL, TEST_ALIGN(width));
		FillRandom(loSrc);
		View hiSrc(width, height, View::Gray8, NULL, TEST_ALIGN(width));
		FillRandom(hiSrc);

		View loDst1(width, height, View::Gray8, NULL, TEST_ALIGN(width));
		View hiDst1(width, height, View::Gray8, NULL, TEST_ALIGN(width));
		View loDst2(width, height, View::Gray8, NULL, TEST_ALIGN(width));
		View hiDst2(width, height, View::Gray8, NULL, TEST_ALIGN(width));

		TEST_EXECUTE_AT_LEAST_MIN_TIME(f1.Call(value, loSrc, hiSrc, loDst1, hiDst1));

		TEST_EXECUTE_AT_LEAST_MIN_TIME(f2.Call(value, loSrc, hiSrc, loDst2, hiDst2));

		result = result && Compare(loDst1, loDst2, 0, true, 10);
		result = result && Compare(hiDst1, hiDst2, 0, true, 10);

		return result;
	}

	bool BackgroundGrowRangeSlowTest()
	{
		bool result = true;

		result = result && BackgroundGrowRangeSlowTest(W, H, FUNC1(Simd::Base::BackgroundGrowRangeSlow), FUNC1(Simd::BackgroundGrowRangeSlow));
		result = result && BackgroundGrowRangeSlowTest(W + 1, H - 1, FUNC1(Simd::Base::BackgroundGrowRangeSlow), FUNC1(Simd::BackgroundGrowRangeSlow));

		return result;
	}
}