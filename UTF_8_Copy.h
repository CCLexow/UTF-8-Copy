/*
 * UTF_8_Copy.h
 *
 *  Created on: 26.09.2017
 *      Author: Carl Christian Lexow - cc.lexow -at- gmail.com
 *
 *
 *  MIT License
 *
 *  Copyright (c) [2017] [Carl Christian Lexow]
 *
 *  Permission is hereby granted, free of charge, to any person obtaining a copy
 *  of this software and associated documentation files (the "Software"), to deal
 *  in the Software without restriction, including without limitation the rights
 *  to use, copy, modify, merge, publish, distribute, sublicense, and/or sell
 *  copies of the Software, and to permit persons to whom the Software is
 *  furnished to do so, subject to the following conditions:
 *
 *  The above copyright notice and this permission notice shall be included in all
 *  copies or substantial portions of the Software.
 *
 *  THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS OR
 *  IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY,
 *  FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT. IN NO EVENT SHALL THE
 *  AUTHORS OR COPYRIGHT HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER
 *  LIABILITY, WHETHER IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING FROM,
 *  OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER DEALINGS IN THE
 *  SOFTWARE.
 */

#ifndef LIB_UTF_8_COPY_UTF_8_COPY_H_
#define LIB_UTF_8_COPY_UTF_8_COPY_H_

#include	 <Arduino.h>

class UTF_8_Copy
{
public:
	UTF_8_Copy(void);
	UTF_8_Copy(String strRawString);
	~UTF_8_Copy();

	void SetString(String strRawString);

	String strGetSubString(uint8_t u08Count, uint8_t u08Offset);

	String strGetString(void);

	uint8_t u08GetStringLength(void);

	uint8_t u08GetUTF_8_Encoded_Character_Count(void);

private:
	String * pstrUTF_8_String;
	uint8_t * pau08UTF8_CharIndices;
	uint8_t * pau08UTF8_ByteArray;
	uint8_t u08UTF_8_Enc_Char_Cnt;
	uint8_t u08UTF_8_StringLength;
	uint8_t u08Raw_StringLength;
	uint8_t u08GetUTF_8_StringLength(String strInput);
	bool bVerifyUTF_8_Encoding(String strInput);
	bool bCreateCharIndexArray(void);
};
#endif /* LIB_UTF_8_COPY_UTF_8_COPY_H_ */
