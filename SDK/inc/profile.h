#ifndef PROFILE_H
#define PROFILE_H

/*************************
Parameters
lpAppName
Pointer to a null-terminated string that specifies the section containing the key name. If this parameter is NULL, the iniGetString function copies all section names in the file to the supplied buffer.
lpKeyName
Pointer to the null-terminated string containing the key name whose associated string is to be retrieved. If this parameter is NULL, all key names in the section specified by the lpAppName parameter are copied to the buffer specified by the lpReturnedString parameter.
lpDefault
Pointer to a null-terminated default string. If the lpKeyName key cannot be found in the initialization file, iniGetString copies the default string to the lpReturnedString buffer. This parameter cannot be NULL.
Avoid specifying a default string with trailing blank characters. The function inserts a null character in the lpReturnedString buffer to strip any trailing blanks.

lpReturnedString
Pointer to the buffer that receives the retrieved string.
nSize
Specifies the size, in characters, of the buffer pointed to by the lpReturnedString parameter.
lpFileBuffer
Pointer to the buffer in which stores all data readed from initialization file.
Return Values
The return value is the number of characters copied to the buffer, not including the terminating null character.

If neither lpAppName nor lpKeyName is NULL and the supplied destination buffer is too small to hold the requested string, the string is truncated and followed by a null character, and the return value is equal to nSize minus one.

If either lpAppName or lpKeyName is NULL and the supplied destination buffer is too small to hold all the strings, the last string is truncated and followed by two null characters. In this case, the return value is equal to nSize minus two.

Remarks
The iniGetString function searches the specified initialization file for a key that matches the name specified by the lpKeyName parameter under the section heading specified by the lpAppName parameter. If it finds the key, the function copies the corresponding string to the buffer. If the key does not exist, the function copies the default character string specified by the lpDefault parameter. A section in the initialization file must have the following form:

[section]
key=string
.
.
.

If lpAppName is NULL, iniGetString copies all section names in the specified file to the supplied buffer. If lpKeyName is NULL, the function copies all key names in the specified section to the supplied buffer. An application can use this method to enumerate all of the sections and keys in a file. In either case, each string is followed by a null character and the final string is followed by a second null character. If the supplied destination buffer is too small to hold all the strings, the last string is truncated and followed by two null characters.

If the string associated with lpKeyName is enclosed in single or double quotation marks, the marks are discarded when the iniGetString function retrieves the string.

The iniGetString function is not case-sensitive; the strings can be a combination of uppercase and lowercase letters.
*********************/
uint32_t iniGetString(
    const void  *lpAppName,         // points to section name
    const void  *lpKeyName,         // points to key name
    const void  *lpDefault,         // points to default string
    void        *lpReturnedString,  // points to destination buffer
    uint32_t    nSize,              // size of destination buffer
    const void  *lpFileBuffer       // points to initialization buffer in which stores all data readed from file

);

/******************************************
Parameters
lpAppName
Pointer to a null-terminated string containing the name of the section to which the string will be copied. If the section does not exist, it is created. The name of the section is case-independent; the string can be any combination of uppercase and lowercase letters.
lpKeyName
Pointer to the null-terminated string containing the name of the key to be associated with a string. If the key does not exist in the specified section, it is created. If this parameter is NULL, the entire section, including all entries within the section, is deleted.
lpString
Pointer to a null-terminated string to be written to the file. If this parameter is NULL, the key pointed to by the lpKeyName parameter is deleted.
Windows 95: The system does not support the use of the TAB (\t) character as part of this parameter.

lpFileBuffer
Pointer to a null-terminated string that names the initialization file.
Return Values
If the function successfully copies the string to the initialization file, the return value is nonzero.
******************************************/
int iniWriteString(
  const void  *lpAppName,   // pointer to section name
  const void  *lpKeyName,   // pointer to key name
  const void  *lpString,    // pointer to string to add
  const void  *lpFileBuffer // pointer to initialization filename
);


#endif

