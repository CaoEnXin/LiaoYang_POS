#ifndef DESFIRE_H_
#define DESFIRE_H_

// File Types Definition:
#define DESFIRE_FILETYPE_STDDATA		0x00			// Standard Data Files
#define DESFIRE_FILETYPE_BACKUPDATA		0x01			// Backup Data Files
#define DESFIRE_FILETYPE_VALUE			0x02			// Value Files with Backup
#define DESFIRE_FILETYPE_LINEARRECORD	0x03			// Linear Record Files with Backup
#define DESFIRE_FILETYPE_CYCLICRECORD	0x04			// Cyclic Record Files with Backup

// Communication Settings ¨C Encryption Modes
#define DESFIRE_MODE_PLAIN				0x00			// Plain communication
#define DESFIRE_MODE_ENC				0x03			// Fully enciphered communication
#define DESFIRE_MODE_MAC				0x01			// Plain communication secured  MACing

// define crypt mode
#define DESFIRE_CRYPT_MODE_DES			0x00
#define DESFIRE_CRYPT_MODE_3K3DES		0x01
#define DESFIRE_CRYPT_MODE_AES			0x02

#define DESFIRE_RET_OK						0x00		// Successful operation
#define DESFIRE_RET_NO_CHANGES				0x0C		// No changes done to backup files,commit Transaction/Abort Transaction not nrcessary
#define DESFIRE_RET_OUT_OF_EEPROM_ERROR		0x0E		// Insufficient NV-Memory to complete command
#define DESFIRE_RET_ILLEGAL_COMMAND_CODE	0x1C		// Command code not supported
#define DESFIRE_RET_INTEGRITY_ERROR			0x1E		// CRC or MAC does not match data Padding bytes not valid
#define DESFIRE_RET_NO_SUCH_KEY				0x40		// Invalid key number specified
#define DESFIRE_RET_LENGTH_ERROR			0x7E		// Length of command string invalid
#define DESFIRE_RET_PERMISSION_DENIED		0x9D		// Current configuration / status does not allow the requested command
#define DESFIRE_RET_PARAMETER_ERROR			0x9E		// Value of the parameter(s) invalid
#define DESFIRE_RET_APPLICATION_NOT_FOUND	0xA0		// Requested AID not present on PICC
#define DESFIRE_RET_APP_INTEGRITY_ERROR		0xA1		// Unrecoverable error withinapplication, application will be disable
#define DESFIRE_RET_AUTHENTICATION_ERROR	0xAE		// Current authentication status does not allow the requested command
#define DESFIRE_RET_ADDITIONAL_FRAME		0xAF		// Additional data frame is expected to be sent
#define DESFIRE_RET_BOUNDARY_ERROR			0xBE		// Attempt to read/write data from/to beyond the file's/record's limited,
														// Attempted to exceed the limits of a value file
#define DESFIRE_RET_PICC_INTEGRITY_ERROR	0xC1		// Unrecoverable error within PIC, PICC will be disabled
#define DESFIRE_RET_COMMAND_ABORTED			0xCA		// Previous Command was not fully completed, Not all frames were requested or provided by the PCD
#define DESFIRE_RET_PICC_DISABLED_ERROR		0xCD		// PICC was disabled by an unrecoverable error
#define DESFIRE_RET_COUNT_ERROR				0xCE		// NUmber of Applications limited to 28, no additional CreateApplication possible
#define DESFIRE_RET_DUPLICATE_ERROR			0xDE		// Creation of file/application failed because file/application with same number already exists
#define DESFIRE_RET_EEPROM_ERROR			0xEE		// Could not complete NV-write operation due to loss of power, internal backup/rollback mechanism activated
#define DESFIRE_RET_FILE_NOT_FOUND			0xF0		// Specified file number does not exist
#define DESFIRE_RET_FILE_INTEGRITY_ERROR	0xF1		// Unrecoverable error within file, file will be disabled		

#define DESFIRE_RET_REC_ERROR				0xFF		// Invalid len received from desfire card	

typedef struct
{
	UINT8 AID[3];	// aid name		
	UINT8 MasterKeySetting; // application master key setting
	UINT8 NumberOfKey;	// how many key enabled within this application
	UINT8 CryptoMode;	// which crypto mode supported by this applicaton:
	                    //   DESFIRE_CRYPT_MODE_DES    ---- specifies DES/3DES algorithm for whole application
	                    //   DESFIRE_CRYPT_MODE_3K3DES ---- specifies 3KDES algorithm for whole application
	                    //   DESFIRE_CRYPT_MODE_AES    ---- specifies AES algorithm for whole application
	UINT8 IsSupFID;     // if every EF within this application a ISOFile ID is used
	UINT8 IsSupISOFID;	// if used ISOFile ID for this application, and can select this appliction with ISO Select command		
	UINT8 ISOFID[2];	// ISOFile ID for this application	
	UINT8 LenOfDFName;  // the length of "DF-Name"
	UINT8 DFName[16];   // carries "DF-Name" used in ISo7816-4 mode in combination with ISO Select command
	UINT8 RFU;
}desfire_appinfo;

typedef struct  
{
	UINT8 AID[3];	// application aid 
	UINT8 FID[2];	// ISOFile ID for this application
	UINT8 LenOfDFName; // the length of "DF-Name"
	UINT8 DFName[16]; // "DF-Name" for this application
	UINT8 RFU[2];	
}dfname_info;

typedef struct  
{
	// contains hardware related information:
	UINT8 HW_VendorID; // codes the vendor ID (0x04 for NXP)
	UINT8 HW_Type;	   // codes the type (here 0x01)
	UINT8 HW_SubType;  // codes the subtype (here 0x01)
	UINT8 HW_MajorVer; // codes the major version number (01)
	UINT8 HW_MinorVer; // codes the minor version number (00)
	UINT8 HW_Size;     // codes the storage size (here 0x1A = 8192)
	UINT8 HW_Protocol; // codes the communication protocol byte (here 0x05)

	// contains software related information
	UINT8 SW_VendorID; // codes the vendor ID (here 0x04 for NXP)
	UINT8 SW_Type;	   // codes the type (here 0x01)	
	UINT8 SW_SubType;  // codes the subtype (here 0x01)	
	UINT8 SW_MajorVer; // codes the major version (01)	
	UINT8 SW_MinorVer; // codes the minor version (00)
	UINT8 SW_Size;	   // codes the storage size (here 0x1A = 8192)		
	UINT8 SW_Protocol; // codes the communication protocol type (here 0x05)		
	
	UINT8 UID[7];	   // codes the unique serial number
	UINT8 BatchNo[5];  // codes the production batch number
	UINT8 CW_Prod;	   // codes the calendar week of production
	UINT8 Year_Prod;   // codes the year of production
}desfire_version;

typedef struct
{
	UINT8 file_type; // desfire file type, refer to File Types Definition:
	UINT8 com_sett;  // communication mode:
						// DESFIRE_MODE_PLAIN   --- plain communication for this file
						// DESFIRE_MODE_ENC     --- encriphed communication for this file
						// DESFIRE_MODE_MAC     ---- secured with MAC for this file
	UINT8 read_access_right_keyno; // application key used to authentication for read access
	UINT8 write_access_right_keyno;// application key used to authentication for write access
	UINT8 read_and_write_access_right_keyno;// application key used to authentication for read and write access
	UINT8 change_access_right_keyno; // application key used to authentication for change access	

	// following item is only valid for Standard Data Files and Backup Data Files
	UINT32 file_size;

	// following items only valid for Value Files
	UINT32 lower_limit;		
	UINT32 upper_limit;
	UINT32 limited_credit_value;
	UINT8  limited_credit_enabled;

	// following items only valid for Linear Record Files and Cyclic Record Files
	UINT32 record_size;
	UINT32 max_number_of_records;
	UINT32 current_number_of_records;
}desfire_file;

typedef struct  
{	
	UINT8 FlgISOFileID; // support ISOFile ID for this file
	UINT8 ISOFileID[2]; // ISOFile ID for this file
	UINT8 com_sett;   // communication mode:
						// DESFIRE_MODE_PLAIN   --- plain communication for this file
						// DESFIRE_MODE_ENC     --- encriphed communication for this file
						// DESFIRE_MODE_MAC     ---- secured with MAC for this file
	UINT8 read_access_right_keyno; // application key used to authentication for read access
	UINT8 write_access_right_keyno;// application key used to authentication for write access
	UINT8 read_and_write_access_right_keyno;// application key used to authentication for read and write access
	UINT8 change_access_right_keyno; // application key used to authentication for change access	
	
	UINT32 file_size; // file size
}desfire_std_backup_file_info;

typedef struct  
{
	UINT8 com_sett; // communication mode:
						// DESFIRE_MODE_PLAIN   --- plain communication for this file
						// DESFIRE_MODE_ENC     --- encriphed communication for this file
						// DESFIRE_MODE_MAC     ---- secured with MAC for this file
	UINT8 read_access_right_keyno; // application key used to authentication for read access
	UINT8 write_access_right_keyno;// application key used to authentication for write access
	UINT8 read_and_write_access_right_keyno;// application key used to authentication for read and write access
	UINT8 change_access_right_keyno; // application key used to authentication for change access	
	
	UINT32 lower_limit; // lower limit value of this value file
	UINT32 upper_limit; // upper limit value of this value file
	UINT32 init_value;  // initialize value of this value file
	UINT32 limited_credit_enabled; // support limited credit value for this file
}desfire_value_file_info;

typedef struct  
{	
	UINT8 FlgISOFileID; // support ISOFile ID for this file
	UINT8 ISOFileID[2]; // ISOFile ID for this file
	UINT8 com_sett;   // communication mode:
						// DESFIRE_MODE_PLAIN   --- plain communication for this file
						// DESFIRE_MODE_ENC     --- encriphed communication for this file
						// DESFIRE_MODE_MAC     ---- secured with MAC for this file
	UINT8 read_access_right_keyno; // application key used to authentication for read access
	UINT8 write_access_right_keyno;// application key used to authentication for write access
	UINT8 read_and_write_access_right_keyno;// application key used to authentication for read and write access
	UINT8 change_access_right_keyno; // application key used to authentication for change access	

	UINT32 record_size;  // record size
	UINT32 max_number_of_records; 
	UINT32 FlgRndAcc; // support random access
	UINT32 rnd_acc;	// random access				
}desfire_record_file_info;

// Function:
//		poll for desfire card in RF field, and get the UID of desfire
// Output:
//		pUID			----- 7bytes UID
// Return Value:
//		Refer to NFC module of posapi.h and desfire.h
// Remarks:
//		1. after desfire_active(...) be called, the data rate between PCD and PICC is 424kbits/s
//		2. the UID of desfire card is 7bytes long
INT32 desfire_Active(UINT8 *pUID);
void desfire_Release(void);

// Function:
//		confirms that both entities (PICC and PCD) can trust each other
// Input:
//		keyno		------ the key no used to authentication process
//		key			------ key information (8/16bytes valid)
// Return Value:
//		refer to NFC module of posapi.h and desfire.h
int desfire_Authenticate(UINT8 keyno, UINT8 *key);

// Function:
//		confirms that both entities (PICC and PCD) can trust each other, only valid for EV1
//		3KDES algorithm used.
// Input:
//		keyno		------ the key no used to authentication process
//		key			------ key information (8/16/24bytes valid)
// Return Value:
//		refer to NFC module of posapi.h and desfire.h
int desfire_AuthenticateISO(UINT8 keyno,UINT8 *key);

// Function:
//		confirms that both entities (PICC and PCD) can trust each other, only valid for EV1.
//		AES algorithm used.
// Input:
//		keyno		------ the key no used to authentication process
//		key			------ key information (16bytes valid)
// Return Value:
//		refer to NFC module of posapi.h and desfire.h
int desfire_AuthenticateAES(UINT8 keyno,UINT8 *key);

// Function:
//		Changes the master key configuration settings depending on the currently selected AID.
// Input:
//		KeySetting		------	new master key settings.
//						------  for PICC master key:
//								bit7~bit4	----- 0000 RFU
//								bit3   	----- whether a change of the PICC master key settings is allowed
//								bit2   	----- whether PICC master key authentication is needed before Create- / DeleteApplication
//								bit1   	----- whether PICC master key authentication is needed for application directory access
//								bit0	  	----- whether the PICC master key is changeable
//						------  for Application master key:
//								bit7~bit4	----- hold the Access Rights for changing application keys (ChangeKey command)
//										0x0   	---- Application master key authentication is necessary to change any key (default)
//										0x1~0x0D	---- Authentication with the specified key is necessary to change any key.
//										0x0E		---- Authentication with the key to be changed (same KeyNo) is necessary to change a key.
//										0x0F		---- All Keys (except application master key, see Bit0) within this application are frozen.
//								bit3		----- whether a change of the application master key settings is allowed
//								bit2		----- whether application master key authentication is needed before CreateFile / DeleteFile
//								bit1		----- whether application master key authentication is needed for file directory access
//								bit0		----- whether the application master key is changeable
// Return Value:
//		Refer to NFC module of posapi.h and desfire.h
int desfire_ChangeKeySettings(UINT8 key_setting);

// Function:
//		get configuration information on the PICC and application master key configuration settings,
//		and get maximum number of keys which can be stored within the selected application.
// Output:
//		pKeySetting		----- current master key setting, value as following:
//						------  for PICC master key:
//								bit7~bit4	----- 0000 RFU
//								bit3   	----- whether a change of the PICC master key settings is allowed
//								bit2   	----- whether PICC master key authentication is needed before Create- / DeleteApplication
//								bit1   	----- whether PICC master key authentication is needed for application directory access
//								bit0	  	----- whether the PICC master key is changeable
//						------  for Application master key:
//								bit7~bit4	----- hold the Access Rights for changing application keys (ChangeKey command)
//										0x0   	---- Application master key authentication is necessary to change any key (default)
//										0x1~0x0D	---- Authentication with the specified key is necessary to change any key.
//										0x0E		---- Authentication with the key to be changed (same KeyNo) is necessary to change a key.
//										0x0F		---- All Keys (except application master key, see Bit0) within this application are frozen.
//								bit3		----- whether a change of the application master key settings is allowed
//								bit2		----- whether application master key authentication is needed before CreateFile / DeleteFile
//								bit1		----- whether application master key authentication is needed for file directory access
//								bit0		----- whether the application master key is changeable
//		pMaxKeyNo			-----  maximum number of keys which can be stored within the selected application
// Return Value:
//		Refer to NFC module of posapi.h and desfire.h
int desfire_GetKeySettings(UINT8 *key_setting, UINT8 *max_key_num);

// Function:
//		change one picc master key
// Input:
//        type			------- key type,define below:
//							DESFIRE_CRYPT_MODE_DES   --- DES/3DES key
//							DESFIRE_CRYPT_MODE_3K3DES --- 3K3DES key
//							DESFIRE_CRYPT_MODE_AES    --- AES key
//		key			------- the new key information (16bytes for DES/3DES/AES, and 24bytes for 3K3DES)
//        version        ------- key version
// Return Value:
//		Refer to NFC module of posapi.h and desfire.h
int desfire_ChangePICCMasterKey(UINT8 type, UINT8 *key, UINT8 version);

// Function:
//		change one application key
// Input:
//		keysetting		------- the master key setting of the current application
//        type				------- key type,define below:
//							DESFIRE_CRYPT_MODE_DES   --- DES/3DES key
//							DESFIRE_CRYPT_MODE_3K3DES --- 3K3DES key
//							DESFIRE_CRYPT_MODE_AES    --- AES key
//		keyno			------- the key no for changed
//		oldkey			------- the current key information (16bytes for DES/3DES/AES, and 24bytes for 3K3DES)
//        newkey			------- the new key information (16bytes for DES/3DES/AES, and 24bytes for 3K3DES)
//        version             ------- key version
// Return Value:
//		Refer to NFC module of posapi.h and desfire.h
int desfire_ChangeAppKey(UINT8 keysetting,
								UINT8 type,
								UINT8 keyno,
								UINT8 *oldkey,
								UINT8 *newkey,
								UINT8 version);

// Function:
//		read out the current key version of any key stored on the PICC
// Input:
//		keyno	------- key no
// Output:
//		version	------- key version of this key
// Return Value:
//		Refer to NFC module of posapi.h and desfire.h
int desfire_GetKeyVersion(UINT8 keyno,UINT8 *version);

// Function:
//		create new applications on the PICC.
// Input:
//		info   ---- application information, refer to desfire_appinfo structure
// Return Value:
//		Refer to NFC module of posapi.h and desfire.h
int desfire_CreateApplication(desfire_appinfo *info);

// Function:
//		allows to permanently deactivate applications on the PICC
// Input:
//		aid		------- Application Identifier (3bytes)
// Return Value:
//		Refer to NFC module of posapi.h and desfire.h
int desfire_DeleteApplication(UINT8 *aid);

// Function:
//		returns the Application IDentifiers of all active applications on a PICC.
// Output:
//		num		------ the number of all active applications on a PICC
//		aid		------ Application IDentifiers (3bytes each application)
// Return Value:
//		Refer to NFC module of posapi.h and desfire.h
int desfire_GetApplicationIDs(UINT8 *num, UINT8 *aid);

// Function:
//		returns the ISO/IEC 7816-4 DF-Name of all active applications on a PICC
// Output:
//		appnum	------ the number of all active applications on a PICC
//		info		------ DF-Name info, refer to dfname_info structure
// Return Value:
//		Refer to NFC module of posapi.h and desfire.h
int desfire_GetDFNames(UINT8 *appnum,dfname_info *info);

// Function:
//		select one specific application for further access.
// Input:
//		aid	------- Application Identifier (3bytes)
// Return Value:
//		Refer to NFC module of posapi.h and desfire.h
int desfire_SelectApplication(UINT8 *aid);

// Function:
//		This command releases the PICC user memory.
// Return Value:
//		Refer to NFC module of posapi.h and desfire.h
int desfire_FormatPICC(void);

// Function:
//		returns manufacturing related data of the PICC.
// Output:
//		version	------ refer to desfire_version structure
// Return Value:
//		Refer to NFC module of posapi.h and desfire.h
int desfire_GetVersion(desfire_version *version);

// Function:
//		returns the available bytes on the PICC
// Output:
//		freesize	------ available bytes on the PICC
// Return Value:
//		Refer to NFC module of posapi.h and desfire.h
int desfire_FreeMemory(UINT32 *freesize);
int desfire_SetConfiguration(UINT8 option, UINT8 len, UINT8 *info);
int desfire_GetCardUID(UINT8 *uid);

// Function:
//		returns the File IDentifiers of all active files within the currently selected application.
// Output:
//		num			----- return how many files of the currently selected application
//		fid			----- file IDs (up to 32bytes, 1byte/file)
// Return Value:
//		Refer to NFC module of posapi.h and desfire.h
int desfire_GetFIDs(UINT8 *num, UINT8 *fid);

// Function:
//		returns the 2byte ISO/IEC 7816-4 File IDentifiers of all active files within the currently selected application.
// Output:
//		num			----- return how many files of the currently selected application
//		fid			----- file IDs (up to 64bytes, 2byte/file)
// Return Value:
//		Refer to NFC module of posapi.h and desfire.h
int desfire_GetISOFIDs(UINT8 *num, UINT8 *fid);

// Function:
//		get information on the properties of a specific file.
// Input:
//		fileno		------- the specific file no, value 0~0x1F allowed
// Output:
//		filesetting	------- file setting information,refer to desfire_file structure
// Return Value:
//		Refer to NFC module of posapi.h and desfire.h
int desfire_GetFileSetting(UINT8 fileno, desfire_file *filesetting);

// Function:
//		changes the access parameters of an existing file
// Input:
//		fileno					------  the specific file no, value 0~0x1F allowed
//        comm_set					------  communication setting
										// DESFIRE_MODE_PLAIN   --- plain communication for this file
										// DESFIRE_MODE_ENC     --- encriphed communication for this file
										// DESFIRE_MODE_MAC     ---- secured with MAC for this file
//		old_change_access_keyno		------  old Change Access Right Key No
//		new_read_access_keyno		------  new Read Access Right Key No
//		new_write_access_keyno		------  new Write Access Right Key No
//		new_read_write_access_keyno	------  new Read and Write Access Right Key No
//		new_change_access_keyno		------  new Change Access Right Key No
// Return Value:
//		Refer to NFC module of posapi.h and desfire.h
int desfire_ChangeFileSettings(UINT8 fileno,
									UINT8 comm_set,
									UINT8 old_change_access_keyno,
									UINT8 new_read_access_keyno,
									UINT8 new_write_access_keyno,
									UINT8 new_read_write_access_keyno,
									UINT8 new_change_access_keyno);

// Function:
//		create files for the storage of plain unformatted user data within an existing application on the PICC
// Input:
//		fileno		------ the file number of the new file within the range 0x00 to 0x1F
//		fileinfo		------ refer to desfire_std_backup_file_info structure
// Return Value:
//		Refer to NFC module of posapi.h and desfire.h
int desfire_CreateSTDDataFile(UINT8 fileno, desfire_std_backup_file_info *fileinfo);

// Function:
//		create files for the storage of plain unformatted user data within an existing application on the PICC, 
//		additionally supporting the feature of an integrated backup mechanism
// Input:
//		fileno		------ the file number of the new file within the range 0x00 to 0x1F
//		fileinfo		------ refer to desfire_std_backup_file_info structure
// Return Value:
//		Refer to NFC module of posapi.h and desfire.h
int desfire_CreateBackupDataFile(UINT8 fileno, desfire_std_backup_file_info *fileinfo);

// Function:
//		create files for the storage and manipulation of 32bit signed
//		integer values within an existing application on the PICC
// Input:
//		fileno		------ the file number of the new file within the range 0x00 to 0x1F
//		fileinfo		------ refer to desfire_value_file_info structure
// Return Value:
//		Refer to NFC module of posapi.h and desfire.h
int desfire_CreateValueFile(UINT8 fileno, desfire_value_file_info *fileinfo);

// Function:
//		create files for multiple storage of structural data, for
//		example for loyalty programs, within an existing application on the PICC
// Input:
//		fileno		------ the file number of the new file within the range 0x00 to 0x1F
//		fileinfo		------ refer to desfire_record_file_info structure
// Return Value:
//		Refer to NFC module of posapi.h and desfire.h
int desfire_CreateLinearRecordFile(UINT8 fileno, desfire_record_file_info *fileinfo);

// Function:
//		create files for multiple storage of structural data, for
//		example for loyalty programs, within an existing application on the PICC
// Input:
//		fileno		------ the file number of the new file within the range 0x00 to 0x1F
//		fileinfo		------ refer to desfire_record_file_info structure
// Return Value:
//		Refer to NFC module of posapi.h and desfire.h
int desfire_CreateCyclicRecordFile(UINT8 fileno,desfire_record_file_info *fileinfo);

// Function:
//		The DeleteFile command permanently deactivates a file within the file directory of the currently selected application.
// Input:
//		fileno	------ the file number within the file directory of the currently selected application.
// Return Value:
//		Refer to NFC module of posapi.h and desfire.h
int desfire_DeleteFile(UINT8 fileno);

// Function:
//		Read data from Standard Data Files or Backup Data Files
// Input:
//		fileno			--------	the file number to be read from
//		comm_set		     --------  communication setting
								// DESFIRE_MODE_PLAIN   --- plain communication for this file
								// DESFIRE_MODE_ENC     --- encriphed communication for this file
								// DESFIRE_MODE_MAC     ---- secured with MAC for this file
//		offset			--------  the starting position for the read operation within the file
//		len				--------  the number of data bytes to be read
// Output:
//		outlen			--------  the length of out data
//		dataout			--------  the out data
// Return Value:
//		Refer to NFC module of posapi.h and desfire.h
int desfire_ReadData(UINT8 fileno, 
						UINT8 comm_set, 							
						UINT32 offset, 
						UINT32 len, 
						UINT32 *outlen, 
						UINT8 *dataout);

// Function:
//		Write data to Standard Data Files and Backup Data Files.
// Input:
//		fileno			--------	the file number to be written to
//		comm_set		     --------  communication setting
								// DESFIRE_MODE_PLAIN   --- plain communication for this file
								// DESFIRE_MODE_ENC     --- encriphed communication for this file
								// DESFIRE_MODE_MAC     ---- secured with MAC for this file
//		offset			--------  the starting position for the write operation within the
//									file (= offset value). This parameter has to be in the range from 0 to file size -1
//		len				--------  specifies the number of data bytes to be written. This parameter can be 
//									in the range from 0x01 to 0xFFFFFF
//		datain			-------  Data to send
// Return Value:
//		Refer to NFC module of posapi.h and desfire.h
int desfire_WriteData(UINT8 fileno, 
						UINT8 comm_set, 
						UINT32 offset, 
						UINT32 len, 
						UINT8 *datain);
// Function:
//		Read the currently stored value from Value Files.
// Input:
//		fileno			------- codes the file number,has to be in the range from 0x00 to 0x1F
//		comm_set		     ------- communication setting
								// DESFIRE_MODE_PLAIN   --- plain communication for this file
								// DESFIRE_MODE_ENC     --- encriphed communication for this file
								// DESFIRE_MODE_MAC     ---- secured with MAC for this file
// Output:
//		value			------- the retrive value
// Return Value:
//		Refer to NFC module of posapi.h and desfire.h
int desfire_GetValue(UINT8 fileno, 
						UINT8 comm_set, 
						UINT32 *value);

// Function:
//		Increase a value stored in a Value File.
// Input:
//		fileno			------  the file number. This parameter has to be in the range from 0x00 to 0x1F.
//		comm_set		     ------  communication setting
								// DESFIRE_MODE_PLAIN   --- plain communication for this file
								// DESFIRE_MODE_ENC     --- encriphed communication for this file
								// DESFIRE_MODE_MAC     ---- secured with MAC for this file
//		value			------  the value which will be increased on the current value
//							   stored in the file. Only positive values are allowed for the Credit command.
// Return Value:
//		Refer to NFC module of posapi.h and desfire.h
int desfire_Credit(UINT8 fileno, 
					UINT8 comm_set, 
					UINT32 value);

// Function:
//		Decrease a value stored in a Value File.
// Input:
//		fileno			------  the file number. This parameter has to be in the range from 0x00 to 0x1F.
//		comm_set		     ------  communication setting
								// DESFIRE_MODE_PLAIN   --- plain communication for this file
								// DESFIRE_MODE_ENC     --- encriphed communication for this file
								// DESFIRE_MODE_MAC     ---- secured with MAC for this file
//		value			------  the value which will be subtracted from the current value
//							   stored in the file. Only positive values are allowed for the Debit command.
// Return Value:
//		Refer to NFC module of posapi.h and desfire.h
int desfire_Debit(UINT8 fileno, 
					UINT8 comm_set, 
					UINT32 value);

// Function:
//		Allows a limited increase of a value stored in a Value File without having full
//		Read&Write permissions to the file. This feature can be enabled or disabled during value file creation.
// Input:
//		fileno			-------  the file number. This parameter has to be in the range from 0x00 to 0x1F.
//		comm_set		     -------  communication setting
								// DESFIRE_MODE_PLAIN   --- plain communication for this file
								// DESFIRE_MODE_ENC     --- encriphed communication for this file
								// DESFIRE_MODE_MAC     ---- secured with MAC for this file
//		value			-------  the value which will be increased on the current value
//							   	stored in the file. Only positive values are allowed for the Credit command.
// Return Value:
//		Refer to NFC module of posapi.h and desfire.h
int desfire_LimitedCredit(UINT8 fileno, 
							UINT8 comm_set, 
							UINT32 value);

// Function:
//		The WriteRecord command allows to write data to a record in a Cyclic or Linear Record File.
// Input:
//		fileno	 		---- the file number, has to be in the range from 0x00 to 0x1F.
//		comm_set		     ---- communication setting
								// DESFIRE_MODE_PLAIN   --- plain communication for this file
								// DESFIRE_MODE_ENC     --- encriphed communication for this file
								// DESFIRE_MODE_MAC     ---- secured with MAC for this file
//		offset	 		---- the offset within one single record, the value has to be in therange from 0 to record size - 1.
//		len	 			---- the length of data which is to be written to the record file, the value has to be in the range from 1 to record size - offset.
//		info				---- Record Information
// Return Value:
//		Refer to NFC module of posapi.h and desfire.h
int desfire_WriteRecord(UINT8 fileno, 
							UINT8 comm_set, 
							UINT32 offset, 
							UINT32 len, 
							UINT8 *info);

// Function:
//		The ReadRecords command allows to read out a set of complete records from a Cyclic or Linear Record File.
// Input:
//		fileno    		---- file number in the range from 0x00 to 0x1F.
//		comm_set		     ---- communication setting
								// DESFIRE_MODE_PLAIN   --- plain communication for this file
								// DESFIRE_MODE_ENC     --- encriphed communication for this file
								// DESFIRE_MODE_MAC     ---- secured with MAC for this file
// 		recordize			---- record size of this record file
//		first			---- first recod no
//		num				---- how many records want to be read out
// Output:
//		outlen		----- the length of all record information
//		info			----- Record Information, shall be allocated at least (num * recordize) bytes
// Return Value:
//		Refer to NFC module of posapi.h and desfire.h
int desfire_ReadRecords(UINT8 fileno, 
							UINT8 comm_set,
							UINT32 recordsize,
							UINT32 first, 
							UINT32 num, 
							UINT32 *outlen, 
							UINT8 *info);

// Function:
//		The ClearRecordFile command allows to reset a Cyclic or Linear Record File to the empty state.
// Input:
//		fileno      ------ file number in the range from 0x00 to 0x1F.
// Return Value:
//		Refer to NFC module of posapi.h and desfire.h
int desfire_ClearRecordFile(UINT8 fileno);

// Function:
//		The CommitTransaction command allows to validate all previous write access on Backup Data Files, Value
//		Files and Record Files within one application.
// Return Value:
//		Refer to NFC module of posapi.h and desfire.h
int desfire_CommitTransaction(void);

// Function:
//		The AbortTransaction command allows to invalidate all previous write access on Backup Data Files, Value
//		Files and Record Files within one application.
//		This is useful to cancel a transaction without the need for re-authentication to the PICC, which would lead to
//		the same functionality.	
// Return Value:
//		Refer to NFC module of posapi.h and desfire.h
int desfire_AbortTransaction(void);

#endif

