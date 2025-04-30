#if !defined(SLEEPSTER_BASE_HASH_TABLE_H)
/* ========================================================================
   $File: sleepster_base_hash_table.h $
   $Date: Sat, 19 Apr 25: 08:05PM $
   $Revision: $
   $Creator: Justin Lewis $
   ======================================================================== */

#define SLEEPSTER_BASE_HASH_TABLE_H
const uint32 MAX_HASH_ENTRIES = 512;

typedef struct hash_entry_t
{
    void *Key;
    void *Value;
}hash_entry;

// TODO(Sleepster): Collisions? Not my problem 
typedef struct hash_table_t
{
    uint32      EntryCounter;
    uint32      MaxEntries;

    hash_entry *Entries;
}hash_table;

// functions
internal uint32 HashGetIndex__(void *Key);
internal void   HashInsertPair(hash_table *Table, void *Key, void *Value);
internal void*  HashGetValue(hash_table *Table, void *Key);
internal void   HashRemoveValueAtIndex(hash_table *Table, uint32 Index);
internal void   HashRemoveIndex(hash_table *Table, uint32 Index);
internal void   HashClearTable(hash_table *Table);
// fuctions

internal hash_table
HashCreateNew(memory_arena *Arena, u64 EntryCount)
{
    hash_table Result;
    Result.MaxEntries   = EntryCount;
    Result.EntryCounter = 0;
    Result.Entries      = PushArray(Arena, hash_entry, EntryCount, 4);

    return(Result);
}

#define HashGetIndex(key) HashGetIndex__((void *)key)
internal uint32
HashGetIndex__(void *Key)
{
    string_u8 *KeyString = (string_u8 *)Key;

    uint64 HashValue = 14695981039346656037ULL;
    for(uint32 KeyIndex = 0;
        KeyIndex < KeyString->Length;
        ++KeyIndex)
    {
        char C = *(KeyString->Data + KeyIndex);

        HashValue = HashValue ^ C;
        HashValue = HashValue * 1099511628211;
    }
    return((HashValue % MAX_HASH_ENTRIES + MAX_HASH_ENTRIES) % MAX_HASH_ENTRIES);
}

internal void
HashInsertPair(hash_table *Table, void *Key, void *Value)
{
    uint32 HashIndex = HashGetIndex(Key);
    Assert(HashIndex >= 0, "Hash Index is Invalid...");

    hash_entry *Entry = &Table->Entries[HashIndex];
    if(pStringCompare((string_u8 *)Entry->Key, (string_u8 *)Key))
    {
        Entry->Value = Value;
        Log(SL_LOG_WARNING, "Value at index %d has been updated...", HashIndex);
        return;
    }

    Entry->Value = Value;
    Entry->Key   = Key;
    Table->EntryCounter += 1;
}

internal void*
HashGetValue(hash_table *Table, void *Key)
{
    void *Result = 0;
    uint32 HashIndex = HashGetIndex(Key);
    Assert(HashIndex >= 0, "Hash Index is Invalid...");

    hash_entry *Entry = &Table->Entries[HashIndex];
    if(pStringCompare((string_u8 *)Entry->Key, (string_u8 *)Key))
    {
        Result = Table->Entries[HashIndex].Value;
    }

    return Result;
}

internal void
HashRemoveValueAtIndex(hash_table *Table, uint32 Index)
{
    hash_entry *Entry = &Table->Entries[Index];
    Entry->Value = 0;
}

internal void
HashRemoveIndex(hash_table *Table, uint32 Index)
{
    hash_entry *Entry = &Table->Entries[Index];
    Entry->Value = 0;
    Entry->Key   = 0;
}

internal void
HashClearTable(hash_table *Table)
{
    for(uint32 TableIndex = 0;
        TableIndex < MAX_HASH_ENTRIES;
        ++TableIndex)
    {
        Table->Entries[TableIndex].Key   = 0;
        Table->Entries[TableIndex].Value = 0;
    }
}

#endif
