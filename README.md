# ExtendedVars
 
## TARGET PLATFORM
Plugin's itself doesn't have platform restriction but some functions are unique for specific platforms. I don't have Apple and Linux devices. So, I can't test their implementations.
 
# Blueprint Exposed C++ Functions

## Fonts_Group
- Runtime_Font_Load

## Sorters
- Sort_String  
- Sort_Int32  
- Sort_Float  
- Sort_Double  
- Sort_Time  

## Read_Group
- (Android Only) Android_Path_Helper  
- Get_Folder_Contents  
- Search_In_Folder  
- Read_File_From_Path_64 : If your file is 64 bit sized, it will create an UObject container to hold it.
- Read_File_From_Path_32  
- Read_Text_From_Path  

## Write_Group
- Write_File_To_Path  

## Bytes_Group 
- Bytes_x64_To_Hex  
- Bytes_x64_To_Base64  
- Bytes_x64_To_UTF8  
- Bytes_x86_To_Hex  
- Bytes_x86_To_Base64  
- Bytes_x86_To_UTF8  
- Hex_To_Bytes_x64  
- UTF8_To_Bytes_x64  
- Base64_To_Bytes_x64  
- Hex_To_Bytes_x86  
- UTF8_To_Bytes_x86  
- Base64_To_Bytes_x86  
- DetectImageExtension  

## String_Group
- FDateTime_To_String  
- Int64_To_String  
- String_To_Int32  
- String_To_Int64  
- String_To_Double  
- String_To_Float  
- String_To_Hex  
- String_To_Base64  
- Hex_To_String  
- Base64_To_String  
- Beautify_Json  

## Int_Group
- Int32_To_Byte  
- Int32_Place_Family  
- Int32_Truncate_To_Big  
- Int32_Truncate_To_Small  

## Float_Group
- Float_Precision : It will trim your float/double. For example: 3.993423 to 3.99
- Float_Fraction_Count  
- Float_Round_Next  

## Time_Group
- Increment_Date  
- (Windows Only) GetDateTimeWithZone  

## Render_Group
- ConvertGammaSpaceBp  
- ConvertGammaSpace  
- Widget_To_TRT2D  
- TRT2D_To_T2D  
- Export_ImageBuf_File  
- Export_T2D_Colors  
- Export_T2D_Bytes  
- Export_Texture_Bytes_RT  
- Encode_Api_Old : Convert image buffer to BMP, JPG or PNG with old API.
- Encode_Api_New : Convert image buffer to BMP, JPG or PNG with new API.
- DecompressImage : Get raw buffer from encoded image.
- Import_T2D_Bytes  
- Import_T2D_Bytes_LowLevel  
- GetPaintRatio : Think you have a ``UCanvasRenderTarget2D`` to paint some shapes on realtime. You want to get painted area ratio to all.

## Logs
- LogString  

## Saves
- GetVariableByName  
- GetAllVariables  
- SetVariable  
- SaveGameToFile  
- SaveGameToMemory  
- LoadGameFromFile  
- LoadGameFromMemory

## Virtual File System (Windows Only)
You have a file or buffer and you want to share them with other processes/apps on same computer with memory address pointing ``(CreateFileMappingW)`` and without actual file I/O. But other processes should have correct reading functions.