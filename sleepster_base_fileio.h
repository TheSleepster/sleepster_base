#if !defined(SLEEPSTER_BASE_FILEIO_H)
/* ========================================================================
   $File: sleepster_base_fileio.h $
   $Date: Wed, 30 Apr 25: 04:33PM $
   $Revision: $
   $Creator: Justin Lewis $
   ======================================================================== */

#define SLEEPSTER_BASE_FILEIO_H

#ifndef SL_OVERRIDE_C_STDLIB

internal
PLATFORM_READ_ENTIRE_FILE(PlatformReadEntireFile)
{
    string_u8 Result = {};

    FILE *FileData = fopen(cstr_(Filepath), "rb"); 
    if(FileData != null)
    {
        fseek(FileData, 0, SEEK_END);
        Result.Length = ftell(FileData);
        fseek(FileData, 0, SEEK_SET);
    
        Result.Data = PushArray(Arena, uint8, Result.Length, 4);
        memset(Result.Data, 0, Result.Length + 1);
        fread(Result.Data, sizeof(char), Result.Length, FileData);
    
        fclose(FileData);
    }
    else
    {
        Log(SL_LOG_ERROR, "Failure to read the file '%s'... Are you sure this file exists?...", cstr_(Filepath));
    }
    
    Assert(Filepath.Data != null, "File data is not valid");
    return(Result);
}

internal
PLATFORM_WRITE_ENTIRE_FILE(PlatformWriteEntireFile)
{
    bool8 Result   = false;
    FILE *FileData = fopen(cstr_(Filepath), "wb"); 
    if(FileData != null)
    {
        fwrite(Data.Data, sizeof(char), Data.Length, FileData);
        fclose(FileData);

        Result = true;
    }
    else
    {
        Log(SL_LOG_ERROR, "Failure to read the file '%s'... Are you sure this file exists?...", cstr_(Filepath));
    }

    return(Result);
}

#endif

#endif
