#if !defined(SLEEPSTER_BASE_HASH_TABLE_H)
/* ========================================================================
   $File: sleepster_base_hash_table.h $
   $Date: Fri, 14 Mar 25: 01:04AM $
   $Revision: $
   $Creator: Justin Lewis $
   ======================================================================== */

#define SLEEPSTER_BASE_HASH_TABLE_H
constexpr uint32 MAX_HASH_ENTRIES = 512;

struct hash_entry
{
    void *Key;
    void *Value;
};

// TODO(Sleepster): Collisions? Not my problem 
struct hash_table
{
    uint32 EntryCounter;
    uint32 MaxEntries  = MAX_HASH_ENTRIES;

    hash_entry Entries[MAX_HASH_ENTRIES];
};

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
    if(StringCompare((string_u8 *)Entry->Key, (string_u8 *)Key))
    {
        Entry->Value = Value;
        Log(LOG_WARNING, "Value at index %d has been updated...", HashIndex);
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
    if(StringCompare((string_u8 *)Entry->Key, (string_u8 *)Key))
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

