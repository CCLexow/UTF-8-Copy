/*
 * UTF_8_Copy.cpp
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

#include "UTF_8_Copy.h"

UTF_8_Copy::UTF_8_Copy(void)
{
	/* "naked" init */
	/* delete internal variables (will be created after call to <SetString> */
	delete [] pau08UTF8_CharIndices;
	delete [] pau08UTF8_ByteArray;
	delete pstrUTF_8_String;
	u08UTF_8_Enc_Char_Cnt = 0;
	u08UTF_8_StringLength = 0;
	u08Raw_StringLength = 0;
}

UTF_8_Copy::UTF_8_Copy(String strRawString)
{
	SetString(strRawString);
}

UTF_8_Copy::~UTF_8_Copy()
{
	delete [] pau08UTF8_CharIndices;
	delete [] pau08UTF8_ByteArray;
	delete pstrUTF_8_String;
	u08UTF_8_Enc_Char_Cnt = 0;
	u08UTF_8_StringLength = 0;
	u08Raw_StringLength = 0;
}

void UTF_8_Copy::SetString(String strRawString)
{
	bool bInitSuccess = false;
	//Serial.printf("Heap size @ start <SetString>: %u\n", ESP.getFreeHeap());
	/* analyze string */
	if(true == bVerifyUTF_8_Encoding(strRawString))
	{
		/* take over string */
		/*
		if(strUTF_8_String)
		{
			Serial1.print("String before: ");
			Serial1.print(strUTF_8_String);
			Serial1.print(" ");
		}*/
		//Serial.printf("Heap size before: %u\n", ESP.getFreeHeap());
		delete pstrUTF_8_String;
		pstrUTF_8_String = new String(strRawString);
		//Serial.printf("Heap size after: %u\n", ESP.getFreeHeap());
		/*Serial1.print(" String after: ");
		Serial1.print(strUTF_8_String);
		Serial1.println(" ");*/
		/* create array of char indices */
		if(true == bCreateCharIndexArray())
		{
			//Serial.printf("Heap size after <bCreateCharIndexArray>: %u\n", ESP.getFreeHeap());
			/* save raw string length (equals byte count) */
			u08Raw_StringLength = strRawString.length();
			/* remember success */
			bInitSuccess = true;
		}
	}

	/* delete everything in case the initialization failed */
	if(false == bInitSuccess)
	{
		/* malformed UTF-8 encoding found
		 * => abort */
		delete [] pau08UTF8_CharIndices;
		delete [] pau08UTF8_ByteArray;
		delete pstrUTF_8_String;
		u08UTF_8_Enc_Char_Cnt = 0;
		u08UTF_8_StringLength = 0;
		u08Raw_StringLength = 0;
	}
}

String UTF_8_Copy::strGetSubString(uint8_t u08Count, uint8_t u08Offset)
{
	//Serial.printf("Heap size @ start <strGetSubString>: %u\n", ESP.getFreeHeap());
	uint8_t u08Idx;
	uint8_t u08WriteIdx=0;
	uint8_t u08ReadIdxFrom = 0;
	uint8_t u08ReadIdxTo = 0;
    uint8_t u08SubStringArraySize;
    String strRetVal;

    /* verify that internal string contains characters */
    if(0 != u08UTF_8_StringLength)
    {
    		/* verify that offset lies within the string */
    		if(u08Offset < u08UTF_8_StringLength)
    		{
    			/* set read start index */
    			u08ReadIdxFrom = pau08UTF8_CharIndices[u08Offset];

    			/* set read stop index
    			 * -> verify against limits */
    			if((u08Offset + u08Count) >= u08UTF_8_StringLength)
    			{
    				/* substring length does not fit within given string
    				 * => copy only partially to the end of the string  */
    				u08ReadIdxTo = u08Raw_StringLength;
    			}
    			else
    			{
    				/* substring length fits within given string */
    				u08ReadIdxTo = pau08UTF8_CharIndices[u08Offset+u08Count];
    			}
    			/* calculate array size */
    			u08SubStringArraySize = u08ReadIdxTo - u08ReadIdxFrom;
    			/* DEBUG *//*
    			Serial.print("\r\nOffset: ");
    			Serial.print(u08Offset);
    			Serial.print(" count: ");
    			Serial.print(u08Count);
    			Serial.print(" reading from: ");
    			Serial.print(u08ReadIdxFrom);
    			Serial.print(" to: ");
    			Serial.print(u08ReadIdxTo);
    			Serial.print(" -> ");
    			Serial.print(u08SubStringArraySize);
    			Serial.println(" Bytes");*/
    			/* and allocate memory for array */
    			uint8_t *pau08SubStringArray = new uint8_t[u08SubStringArraySize+1]; /* plus terminating Null */
    			/* verify successful memory allocation */
    			if(pau08SubStringArray)
    			{
    				/* preset array with zeros */
    				memset(pau08SubStringArray,0,u08SubStringArraySize+1);
    				/* reset write index */
    				u08WriteIdx = 0;
    				/* copy bytes */
    				for(u08Idx=u08ReadIdxFrom; u08Idx<u08ReadIdxTo; u08Idx++)
    				{
    					pau08SubStringArray[u08WriteIdx++] = pau08UTF8_ByteArray[u08Idx];
    				}
    				/* form into string */
    				strRetVal = String((char *)pau08SubStringArray);
    				delete [] pau08SubStringArray;
    				return strRetVal;
    			}
    			else
    			{
    				return "Err: Memory allocation for substring failed";
    			}
    		}
    		else
    		{
    			return "Err: Offset bigger than character count";
    		}
    }
    else
    {
    		/* internal string either not set or corrupted */
    		return "Err: empty string";
    }
}

String UTF_8_Copy::strGetString(void)
{
	String strRetVal = *pstrUTF_8_String;
	return strRetVal;
}

uint8_t UTF_8_Copy::u08GetStringLength(void)
{
	return u08UTF_8_StringLength;
}

uint8_t UTF_8_Copy::u08GetUTF_8_Encoded_Character_Count(void)
{
	return u08UTF_8_Enc_Char_Cnt;
}

/* Analyzes the given string and returns the count of characters (UTF-8 encoded)
 * returns 0 in case of an error */
uint8_t UTF_8_Copy::u08GetUTF_8_StringLength(String strInput)
{
	//Serial.printf("Heap size @ start <u08GetUTF_8_StringLength>: %u\n", ESP.getFreeHeap());
    uint8_t u08UTF_Length=0;
    uint8_t u08UTF_ByteCount=0;
    uint8_t u08UTF_idx=7;
    uint8_t u08Idx;
    uint8_t u08StringLength=strInput.length()+1;// add one byte for the terminating null byte

	/* allocate array to hold the string bytes */
	uint8_t * pau08ByteArray = new uint8_t[u08StringLength]; // add one byte for the terminating null byte
	/* ensure allocation */
	if(pau08ByteArray)
	{
		/* get bytes from string */
		strInput.getBytes(pau08ByteArray, u08StringLength);
		/* step through bytes to detect the UTF-8 encoded characters */
		for(u08Idx=0; u08Idx<u08StringLength;u08Idx++)
		{
			/* prepare variables */
			u08UTF_ByteCount = 0;
			u08UTF_idx = 7;
			/* UTF-8 encoded characters have Bit 8 set to one */
			if(pau08ByteArray[u08Idx] & (1<<u08UTF_idx))
			{
				/* found a UTF-8 encoded character
				 * -> count */
				u08UTF_8_Enc_Char_Cnt++;
				u08UTF_ByteCount++;
				u08UTF_idx--;
				/* check how many bytes were used to encode this character
				 * (encoded by amount of consecutive bits set to one (7-6-5-4-etc.) */
				while(u08UTF_idx)
				{
					/* Bits 6-5-4 and so on also set to one? */
					if(pau08ByteArray[u08Idx] & (1<<u08UTF_idx))
					{
						/* increment the byte count */
						u08UTF_ByteCount++;
						u08UTF_idx--;
					}
					else
					{
						/* found a Bit set to zero => stop */
						u08UTF_idx=0;
					}
				}
				//Serial.print("\r\nCharacter encoded into: ");
				//Serial.print(u08UTF_ByteCount);
				//Serial.println(" UTF-8 Bytes");
				/* verify against malformed encoding
				 * => check:		1.) enough bytes in the buffer remaining?
				 * 				2.) have the following bytes Bit 8 set to one? (UTF-8 encoding)*/
				if((u08Idx + u08UTF_ByteCount - 1) < u08StringLength)
				{
					/* check the following bytes */
					for(u08UTF_idx=1; u08UTF_idx<u08UTF_ByteCount; u08UTF_idx++)
					{
						/* UTF-8 encoded characters must have Bit 8 set to one */
						if(!(pau08ByteArray[u08Idx + u08UTF_idx] & (1<<7)))
						{
							/* Byte does not have the Bit8 set to one -> malformed UTF-8 encoding found
							 * => abort *//*
							Serial1.print("\r\nRead idx: ");
							Serial1.println(u08Idx);
							Serial1.print("Idx: ");
							Serial1.println(u08Idx + u08UTF_idx);
							Serial1.print("Byte: ");
							Serial1.println(pau08ByteArray[u08Idx + u08UTF_idx]);
							Serial1.println("Malformed UTF-8 encoding found");
							*/
							delete [] pau08ByteArray;
							return 0;
						}
					}
					/* came here => everything is OK
					 * => count up character count */
					u08UTF_Length++;
					/* point array read index to the next character */
					u08Idx += u08UTF_ByteCount - 1;
				}
				else
				{
					/* First UTF-8 Byte indicates more associated bytes than the buffer contains
					 * => abort */
					/*
					Serial1.println("Array does not contain enough bytes");
					Serial1.print("\r\nRead idx: ");
					Serial1.println(u08Idx);
					Serial1.print("Byte count: ");
					Serial1.println(u08UTF_ByteCount);
					Serial1.print("Max read idx: ");
					Serial1.println(u08Idx + u08UTF_ByteCount);
					Serial1.print("Array length: ");
					Serial1.println(u08StringLength);
					*/
					delete [] pau08ByteArray;
					return 0;
				}
			}
			else
			{
				/* this character is ASCII encoded
				 * => count up only if this is not the trailing zero */
				if(pau08ByteArray[u08Idx])
				{
					u08UTF_Length++;
				}
			}
		}
	}
	else
	{
		Serial1.println("Memory alloc error");
		return 0;
	}
	/* release allocated memory */
	delete [] pau08ByteArray;
	/* got here => everything went fine
	 * => return UTF-8 encoded string length */
	return u08UTF_Length;
}

/* verifies the UTF-8 encoding of the given String
 * at the same time:	- counts the UTF-8 encoded characters
 * 					- counts the string length
 * returns 	true, if encoding is correct
 * 			false, if one of the following occurs: 	- UTF-8 encoding malformed
 * 													- memory allocation failed  */
bool UTF_8_Copy::bVerifyUTF_8_Encoding(String strInput)
{
	//Serial.printf("Heap size @ start <bVerifyUTF_8_Encoding>: %u\n", ESP.getFreeHeap());
	u08UTF_8_Enc_Char_Cnt = 0;
	/* get string length (function contains UTF-8 encoding check */
	u08UTF_8_StringLength = u08GetUTF_8_StringLength(strInput);
	/* UTF-8 encoding correct? */
	if(0 != u08UTF_8_StringLength)
	{
		return true;
	}
	else
	{
		return false;
	}
}

/* creates an array containing the indices of the string characters
 * requires that <bVerifyUTF_8_Encoding> returned true prior to this function call */
bool UTF_8_Copy::bCreateCharIndexArray(void)
{
	uint8_t u08Idx;
	uint8_t u08WriteIdx = 0;
    uint8_t u08UTF_ByteCount;
    uint8_t u08UTF_idx;

	/* verify that string length is not zero */
	if(0 != u08UTF_8_StringLength)
	{
		delete [] pau08UTF8_CharIndices;
		delete [] pau08UTF8_ByteArray;
		/* try to allocate memory for the array */
		pau08UTF8_CharIndices = new uint8_t[u08UTF_8_StringLength];
		pau08UTF8_ByteArray = new uint8_t[pstrUTF_8_String->length()+1]; /* add one byte for the Null termination */
		/* verify memory allocation */
		if((0 != pau08UTF8_CharIndices) && (0 != pau08UTF8_ByteArray))
		{
			/* go the easy way, if the string does not contain UTF-8 encoded characters */
			if(0 == u08UTF_8_Enc_Char_Cnt)
			{
				/* fill char indices array */
				for(u08Idx=0; u08Idx<u08UTF_8_StringLength; u08Idx++)
				{
					pau08UTF8_CharIndices[u08Idx] = u08Idx;
				}
				/* get bytes from string */
				pstrUTF_8_String->getBytes(pau08UTF8_ByteArray, pstrUTF_8_String->length()+1);
			}
			else
			{
				/* analyze string characters */
				/* preset indices array with zeros */
				memset(pau08UTF8_CharIndices,0,u08UTF_8_StringLength);
				/* get bytes from string */
				pstrUTF_8_String->getBytes(pau08UTF8_ByteArray, pstrUTF_8_String->length()+1);
				/* create array of indices to the characters */
				for(u08Idx=0; u08Idx<pstrUTF_8_String->length(); u08Idx++)
				{
					/* prepare variables */
					 u08UTF_ByteCount=0;
					 u08UTF_idx=7;
					 //u08WriteIdx = 0;
					 /* check if this byte is UTF-8 encoded */
					 /* UTF-8 encoded characters have Bit 8 set to one */
					if(pau08UTF8_ByteArray[u08Idx] & (1<<u08UTF_idx))
					{
						/* this byte is UTF-8 encoded */
						/* remember index */
						pau08UTF8_CharIndices[u08WriteIdx++] = u08Idx;
						/* check, how many bytes are used for this character */
						u08UTF_ByteCount++;
						u08UTF_idx--;
						/* check how many bytes were used to encode this character
						 * (encoded by amount of consecutive bits set to one (7-6-5-4-etc.) */
						while(u08UTF_idx)
						{
							/* Bits 6-5-4 and so on also set to one? */
							if(pau08UTF8_ByteArray[u08Idx] & (1<<u08UTF_idx))
							{
								/* increment the byte count */
								u08UTF_ByteCount++;
								u08UTF_idx--;
							}
							else
							{
								/* found a Bit set to zero => stop */
								u08UTF_idx=0;
							}
						}
						//Serial.print("UTF-Character with ");
						//Serial.print(u08UTF_ByteCount);
						//Serial.println(" Bytes");
						/* point array read index to the next character */
						u08Idx += u08UTF_ByteCount - 1;
					}
					else
					{
						/* this byte is ascii */
						/* remember index */
						pau08UTF8_CharIndices[u08WriteIdx++] = u08Idx;
					}
				}
			}
			/* DEBUG *//*
			Serial.println("--- Char indices array ---");
			Serial.printf("@ address: %u\r\n", pau08UTF8_CharIndices);
			for(u08Idx=0; u08Idx<u08UTF_8_StringLength; u08Idx++)
			{
				Serial.print(pau08UTF8_CharIndices[u08Idx]);
				Serial.print(" ");
			}
			Serial.println(" ");*/
			/* char indices array successfully created */
			return true;
		}
		else
		{
			Serial.println("Memory allocation failed");
			/* Memory allocation failed */
			return false;
		}
	}
	else
	{
		Serial.println("String is empty");
		/* String is empty */
		return false;
	}
}
